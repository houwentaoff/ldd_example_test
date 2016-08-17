/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_netlinkapp2.c
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  Wednesday, August 17, 2016 10:21:16 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>

#define NETLINK_TEST 25
#define MAX_PAYLOAD 1024 /*消息最大负载为1024字节*/

int main(int argc, char* argv[])
{
    struct sockaddr_nl dest_addr;
    struct nlmsghdr *nlh = NULL;
    //struct iovec iov;
    int sock_fd=-1;
    //struct msghdr msg;

    if(-1 == (sock_fd=socket(PF_NETLINK, SOCK_RAW,NETLINK_TEST))){
          perror("can't create netlink socket!");
          return 1;
    }
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = PF_NETLINK;
    dest_addr.nl_pid = 0; /*我们的消息是发给内核的*/
    dest_addr.nl_groups = 0; /*在本示例中不存在使用该值的情况*/

   /*不再调用bind()函数了
   if(-1 == bind(sock_fd, (struct sockaddr*)&dest_addr, sizeof(dest_addr))){
          perror("can't bind sockfd with sockaddr_nl!");
          return 1;
   }*/

   if(NULL == (nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD)))){
          perror("alloc mem failed!");
          return 1;
   }
   memset(nlh,0,MAX_PAYLOAD);
   /* 填充Netlink消息头部 */
   nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
   nlh->nlmsg_pid = getpid();//我们希望得到内核回应，所以得告诉内核我们ID号
   nlh->nlmsg_type = NLMSG_NOOP; //指明我们的Netlink是消息负载是一条空消息
   nlh->nlmsg_flags = 0;

   /*设置Netlink的消息内容，来自我们命令行输入的第一个参数*/
   strcpy(NLMSG_DATA(nlh), "rr");

   /*这个模板就用不上了。*/
   /*
   memset(&iov, 0, sizeof(iov));
   iov.iov_base = (void *)nlh;
   iov.iov_len = nlh->nlmsg_len;
   memset(&msg, 0, sizeof(msg));
   msg.msg_iov = &iov;
   msg.msg_iovlen = 1;
   */

   //sendmsg(sock_fd, &msg, 0); //不再用这种方式发消息到内核
   sendto(sock_fd,nlh,NLMSG_LENGTH(MAX_PAYLOAD),0,(struct sockaddr*)(&dest_addr),sizeof(dest_addr)); 
        
   //接收内核消息的消息
   printf("waiting message from kernel!\n");
   //memset((char*)NLMSG_DATA(nlh),0,1024);
   memset(nlh,0,MAX_PAYLOAD); //清空整个Netlink消息头包括消息头和负载
   //recvmsg(sock_fd,&msg,0);
   recvfrom(sock_fd,nlh,NLMSG_LENGTH(MAX_PAYLOAD),0,(struct sockaddr*)(&dest_addr),NULL);
   printf("Got response: %s\n",NLMSG_DATA(nlh)); 

   /* 关闭netlink套接字 */
   close(sock_fd);
   free(nlh);
   return 0;
}


