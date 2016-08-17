#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <net/netlink.h> 
#include <net/sock.h>

#define NETLINK_TEST 25
#define MAX_MSGSIZE 100
void sendnlmsg(char * message);
int pid;
int err;
struct sock *nl_sk = NULL;
int flag = 0;

void sendnlmsg(char *message)
{
    struct sk_buff *skb_1 = NULL;
    struct nlmsghdr *nlh = NULL;
    int len = NLMSG_SPACE(MAX_MSGSIZE);//4字节对齐的长度
    int slen = 0;
    if(!message || !nl_sk)
    {
        return ;
    }
    skb_1 = alloc_skb(len,GFP_KERNEL);
    if(!skb_1)
    {
        printk(KERN_ERR "my_net_link:alloc_skb_1 error\n");
    }
    slen = strlen(message);
    nlh = nlmsg_put(skb_1,0,0,0,MAX_MSGSIZE,0);

    NETLINK_CB(skb_1).portid = 0;
    NETLINK_CB(skb_1).dst_group = 0;

    message[slen]= '\0';
    memcpy(NLMSG_DATA(nlh),message, 1);//nlmsg的data部分指针
    printk("my_net_link:send message '%s'.\n",(char *)NLMSG_DATA(nlh));
    printk("len[%d]\n", len);

    netlink_unicast(nl_sk,skb_1,pid,MSG_DONTWAIT);

}

static void nl_data_ready(struct sk_buff *__skb)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    char str[100];
    char send_buf[100]={"I am from kernel\n"};
    struct completion cmpl;
    int i=10;
    skb = skb_get (__skb);
    printk("%d\n", NLMSG_SPACE(0));
    if(skb->len >= NLMSG_SPACE(0))
    {
        nlh = nlmsg_hdr(skb);

        memcpy(str, NLMSG_DATA(nlh), sizeof(str));
        printk("Message received:%s\n",str) ;
        pid = nlh->nlmsg_pid;
        sendnlmsg(send_buf);
#if 0
        while(i--)
        {
            init_completion(&cmpl);
            wait_for_completion_timeout(&cmpl, HZ);// * HZ);
            sendnlmsg(send_buf);
        }
#endif
        flag = 1;
        kfree_skb(skb);
    }

}

// Initialize netlink

int netlink_init(void)
{

    struct netlink_kernel_cfg cfg;
    
    memset(&cfg, 0, sizeof(cfg));
    cfg.groups = 1;
    cfg.input  = nl_data_ready;

//    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, THIS_MODULE,
//            nl_data_ready, NULL, THIS_MODULE);

    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);

    if(!nl_sk){
        printk(KERN_ERR "my_net_link: create netlink socket error.\n");
        return 1;
    }

    printk("my_net_link_3: create netlink socket ok.\n");


    return 0;
}

static void netlink_exit(void)
{
    if(nl_sk != NULL){
        sock_release(nl_sk->sk_socket);
    }

    printk("my_net_link: self module exited\n");
}

module_init(netlink_init);
module_exit(netlink_exit);

MODULE_AUTHOR("frankzfz");
MODULE_LICENSE("GPL");
