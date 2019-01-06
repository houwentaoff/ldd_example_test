/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_packet.c
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  09/25/2018 10:48:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h> /*  For ETH_P_ALL */
#include <net/if_arp.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <linux/sockios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>

#define BUFSIZE 2048

static void hex_print(const u_char *buf, int len, int offset)
{
    u_int i, j, jm;
    int c;

    printf("\n");
    for (i = 0; i < len; i += 0x10) {
        printf(" %04x: ", (u_int)(i + offset));
        jm = len - i;
        jm = jm > 16 ? 16 : jm;

        for (j = 0; j < jm; j++) {
            if ((j % 2) == 1)
                printf("%02x ", (u_int) buf[i+j]);
            else printf("%02x", (u_int) buf[i+j]);
        }
        for (; j < 16; j++) {
            if ((j % 2) == 1) printf("   ");
            else printf(" ");
        }
        printf(" ");

        for (j = 0; j < jm; j++) {
            c = buf[i+j];
            c = isprint(c) ? c : '.';
            printf("%c", c);
        }
        printf("\n");
    }
}


int main()
{
    int listenfd;
    int n = 0;
    unsigned char buf[BUFSIZE];
    struct ifreq ifr;
    int i = 0;
    struct sockaddr_ll fromaddr;
    unsigned char src_mac[ETH_ALEN]={0};
    socklen_t len = sizeof(fromaddr);
    int ifidx = 0;

    if ((listenfd = socket(
//                    AF_INET,
                    PF_PACKET,
//                    SOCK_STREAM,
                    SOCK_RAW,
//                    htons(ETH_P_IP)))
//                    htons(ETH_P_ARP)))
                    htons(ETH_P_ALL)))
//                    0))
                    < 0) 
    {

        perror("socket");
        exit(1);
    }
    bzero(&fromaddr, sizeof(fromaddr));
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "tap0", sizeof("tap0"));
    if (ioctl(listenfd, SIOCGIFINDEX, &ifr) != 0)
    {
         perror("ioctl(SIOCGIFINDEX)");
         return -1;
    }
    ifidx = ifr.ifr_ifindex;
    if(ioctl(listenfd, SIOCGIFHWADDR,&ifr) < 0){
        perror("get dev MAC addr error:");
        exit(1);
    }
    memcpy(src_mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
    printf("MAC :%02X-%02X-%02X-%02X-%02X-%02X\n",src_mac[0],src_mac[1],src_mac[2],src_mac[3],src_mac[4],src_mac[5]);
    printf("interface Index:%d\n",ifidx);
    fromaddr.sll_ifindex = ifidx;
    fromaddr.sll_family = PF_PACKET;
    fromaddr.sll_protocol=htons(ETH_P_ALL);//ETH_P_ALL);

//    fromaddr.sll_hatype=ARPHRD_ETHER;
//    fromaddr.sll_halen=ETH_ALEN;
//    fromaddr.sll_pkttype=PACKET_HOST;
//    memcpy(fromaddr.sll_addr, src_mac, ETH_ALEN);
    
//    struct sockaddr_in xx;
//    bzero(&xx, sizeof(xx));
//    xx.sin_family = AF_INET;
//    xx.sin_addr.s_addr =htonl(INADDR_ANY);
//    xx.sin_port = htons(9999);
    int ret = bind(listenfd, (struct sockaddr*)&fromaddr, sizeof(fromaddr));
    if (ret < 0)
    {
        perror("bind err");
        return -3;
    }

    ifr.ifr_flags &= ~IFF_PROMISC;
    printf("name %s\n", ifr.ifr_name);
    if (ioctl(listenfd, SIOCSIFFLAGS, &ifr) < 0)
    {
        perror("set flag");
        return -2;
    }
    for (;;) {
        unsigned char src_mac[18] = "";    
        unsigned char dst_mac[18] = "";    
#if 0
        n = recvfrom(listenfd, buf, sizeof(buf), 0, (struct sockaddr *)&fromaddr, &len);
#else
        //n = recv(listenfd, buf, BUFSIZE, 0);
        n = read(listenfd, buf, BUFSIZE);
#endif
        if (n > 0)
        {
            sprintf(dst_mac,"%02x:%02x:%02x:%02x:%02x:%02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);    
            sprintf(src_mac,"%02x:%02x:%02x:%02x:%02x:%02x", buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);    
            printf("recv [%d] type[0x%x][0x%x]\n", n, buf[12], buf[13]);
            //判断是否为IP数据包    
            if(buf[12]==0x08 && buf[13]==0x00)    
            {       
                printf("______________IP数据报_______________\n");    
                printf("MAC:%s >> %s\n",src_mac,dst_mac);    
            }//判断是否为ARP数据包    
            else if(buf[12]==0x08 && buf[13]==0x06)    
            {    
                printf("______________ARP数据报_______________\n");    
                printf("MAC:%s >> %s\n",src_mac,dst_mac);    
            }//判断是否为RARP数据包    
            else if(buf[12]==0x80 && buf[13]==0x35)    
            {    
                printf("______________RARP数据报_______________\n");    
                printf("MAC:%s>>%s\n",src_mac,dst_mac);    
            }    
            //send(listenfd, buf, n, 0);
            sleep(4);
            write(listenfd, buf, n);

//            for (i=0; i<n; i++)
//            {
//                printf("0x%X ", buf[i]);
//            }
//            printf("\n");
//            hex_print(buf, n, 0);
            //decode_eth(buf);
        }
    }
    return 0;
}

