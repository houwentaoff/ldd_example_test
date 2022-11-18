# uboot一些命令

+ 网络启动
`bootcmd=run netboot`

+ 动态获取ip并tftp启动内核  
`setenv netboot 'dhcp && tftpboot ${netstart} ${kernel_img} && bootm'
saveenv`
