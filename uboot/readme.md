# uboot一些命令

+ 网络启动
`bootcmd=run netboot`

+ 动态获取ip并tftp启动内核  
`setenv netboot 'dhcp && tftpboot ${netstart} ${kernel_img} && bootm'
saveenv`
  后续新版的uboot 使用`dhcp 0xc0000000`替换`dhcp&tftpboot` 所使用的变量值为 `serverip=192.168.10.12` `bootfile=image.ub` `netboot=dhcp 0xc0000000` `run netboot`
+ 通过串口下载数据到ddr
`loady` `crtl a,s`

# 该目录中包含arm-gnu-gcc和armclang的一些描述


# other
## warm rst && warm boot
### 怎么进入热启动

###
* bootrom首先运行检查warmrst的函数(有些寄存器会做标示),因cpu subsystem没有断电，`warmrst`寄存器`CPU_JUMP_ADDR`值得以保存;此模式下不会检查启动拨码开关,直接`bl CPU_JUMP_ADDR`,若该值未被设置，则`jump to normalboot`处
* bootrom normalboot会检查拨码进行download模式或拨码启动的流程

## 冷启动
+ 断电重启
