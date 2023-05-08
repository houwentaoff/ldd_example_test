# uboot一些命令

+ 网络启动
`bootcmd=run netboot`

+ 动态获取ip并tftp启动内核  
`setenv netboot 'dhcp && tftpboot ${netstart} ${kernel_img} && bootm'
saveenv`

+ 通过串口下载数据到ddr
`loady` `crtl a,s`

# 该目录中包含arm-gnu-gcc和armclang的一些描述
