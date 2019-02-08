/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_tap_file.c
 *
 *    Description:  在tap字符设备端将自身发出的arp请求包，修改为对端发出的请求包，并将mac
 *    地址最后一位修改，便于使对端mac地址和自身不重复.
 *         Others: 经过实验可以看到邻居表正常更新
 *         0. a.out &
 *         1. ifconfig tap0 192.168.27.2 
 *         2. arping -I tap0 192.168.27.111 -c 1
 *         3. arp -a
 *
 *         1. nc 192.168.27.111 1567 -u
 *         代码中将发出的udp数据包内容进行了打印输出,发现包内容均正常
 *         结论:
 *             1. tap字符设备中的write收到arp请求包后，协议栈对其进行了处理arp_process，并更新了邻居表(arp -a可以查看)
 *             2. 鉴于上面的测试均是原始socket产生的数据，下面需要用nc来测试非raw socket.
 *             2.1 通过UDP的测试发现实际上是通过了UDP/TCP协议栈。因此说明虚拟网卡实际上是能通过TCP，UDP协议栈，不用使用
 *             lwip对其原始socket进行操作。
 *
 *        Version:  1.0
 *        Created:  02/08/2019 02:50:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <linux/if_tun.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>  
 #include <stdlib.h>

#include <net/if_arp.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>

struct arp_pack {
    struct arphdr ahdr;
#if 1         
/*   
*  Ethernet looks like this : This bit is variable sized however...
*/ 
unsigned char       ar_sha[ETH_ALEN];   /*  sender hardware address  */                                                                                      
unsigned char       ar_sip[4];      /*  sender IP address        */
unsigned char       ar_tha[ETH_ALEN];   /*  target hardware address  */
unsigned char       ar_tip[4];      /*  target IP address        */
#endif   
};    

int tun_creat(char *dev,int flags)
{
    struct ifreq ifr;
    int fd,err;
    assert(dev != NULL);
    if((fd = open ("/dev/net/tun",O_RDWR))<0) //you can replace it to tap to create tap device.
        return fd;
    memset(&ifr,0,sizeof (ifr));
    ifr.ifr_flags|=flags;
    if(*dev != '\0')
        strncpy(ifr.ifr_name,dev,IFNAMSIZ);
    if((err = ioctl(fd,TUNSETIFF,(void *)&ifr))<0)
    {
        close (fd);
        return err;
    }
    strcpy(dev,ifr.ifr_name);
    return fd;
}

int main()
{
    int tun,ret;
    int i = 0;
    char tun_name[IFNAMSIZ];
    unsigned char buf[4096];
    tun_name[0]='\0';
    tun = tun_creat(tun_name,IFF_TAP|IFF_NO_PI);//如果需要配置tun设备，则把"IFF_TAP"改成“IFF_TUN”
    if(tun<0)
    {
        perror("tun_create");
        return 1;
    }
    printf("TUN name is %s\n",tun_name);
    while (1) {
        ret = read(tun, buf, sizeof(buf));
        if (ret < 0)
            break;
        printf("收到数据包:长度为:[%d]\n", ret);
        for (i=0; i<ret; i++)
        {
            printf("0x%2.2x ", buf[i]);
        }
        printf("\n");
        struct ethhdr *ehdr = NULL;
        ehdr = (struct ethhdr *)&buf[0];
        if (ret > 14)
        {
            printf("以太网帧类信:[0x%4.4x]\n", htons(ehdr->h_proto));
            if (htons(ehdr->h_proto) == 0x0800)
            {
                struct iphdr *ihdr = NULL;
                ihdr = (struct iphdr *)&buf[14];
                printf("IP数据包 上层协议为[0x%2.2x]ip包长度[%d]\n", 
                        ihdr->protocol, ihdr->ihl);
                if (ihdr->protocol == 0x11)//UDP
                {
                    printf("UDP报文\n");
                    struct udphdr *uhdr = NULL;
                    uhdr = (struct udphdr *)&buf[14+ihdr->ihl*4];
                    printf("UDP数据报文长度[%d]端口[%u]-->[%u]\n", htons(uhdr->len), htons(uhdr->source), htons(uhdr->dest));
                    printf("UDP报文全部内容:\n");
                    for (i=0; i<htons(uhdr->len); i++)
                    {
                        printf ("0x%2.2x ", buf[14+ihdr->ihl*4 + i]);
                    }
                    printf ("\n");

                }
                else if (ihdr->protocol == 0x1)//ICMP
                {
                    printf("ICMP报文\n");
                }
                else if (ihdr->protocol == 0x4)//IP
                {
                    printf("IP报文.\n");
                }
                else if (ihdr->protocol == 0x6)//TCP
                {
                    printf("TCP报文\n");
                }
            }
            else if (htons(ehdr->h_proto) == 0x0806)
            {
                struct arp_pack *arppack = NULL;
                printf("ARP 数据包\n");
                /*
                 * 目得: 追踪arp包为何不解析问题
                 * 交换ip
                 */
                struct arphdr *ahdr = NULL;
                ahdr = (struct arphdr *)&buf[14];
                arppack = (struct arp_pack *)ahdr;

                if (htons(ahdr->ar_op) == 0x1)
                {
                    uint32_t t =  *(uint32_t *)(arppack->ar_sip);
                    char *source = inet_ntoa(*(struct in_addr*)&t); //(htonl(*(uint32_t *)(arppack->ar_sip))))
                        source = strdup(source);
                    t =  *(uint32_t *)(arppack->ar_tip);
                    char *target = inet_ntoa(*(struct in_addr*)&t);//htonl((struct in_addr)arppack->ar_tip))
                        target = strdup(target);
                    printf("arp [%d]请求:[%s] --> [%s]\n", sizeof (in_addr_t), source, target);
                    // 改变mac地址和ip顺序并 转发回自身
                    arppack->ar_sha[ETH_ALEN-1] = arppack->ar_sha[ETH_ALEN-1] + 1;
                    in_addr_t iaddr = 0;
                    iaddr = inet_addr(target);
                    memcpy(arppack->ar_sip, &iaddr, sizeof(iaddr));// = inet_addr(target);
                    iaddr = inet_addr(source);
                    memcpy(arppack->ar_tip, &iaddr, sizeof(iaddr));// = inet_addr(target);
                    ehdr->h_source[ETH_ALEN-1] = ehdr->h_source[ETH_ALEN-1] +1;
                    if (source)free (source);
                    if (target)free (target);
                    ret = write(tun, buf, ret);
                    printf("发送长度[%d]\n", ret);
                    continue;
                }
                else if (htons(ahdr->ar_op) == 0x02)
                {
                    uint32_t t =  *(uint32_t *)(arppack->ar_sip);
                    char *source = inet_ntoa(*(struct in_addr*)&t); //(htonl(*(uint32_t *)(arppack->ar_sip))))
                        source = strdup(source);
                    t =  *(uint32_t *)(arppack->ar_tip);
                    char *target = inet_ntoa(*(struct in_addr*)&t);//htonl((struct in_addr)arppack->ar_tip))
                        target = strdup(target);
                    printf("arp 回应:[%s] --> [%s]\n", source, target);
                    if (source)free (source);
                    if (target)free (target);
                }
                
            }
        }
    }
    return 0;
}

