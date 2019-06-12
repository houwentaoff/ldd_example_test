# 如何在不同系统中配置kdump环境

## 下载 编译 安装

1. wget ....
2. apt-get install zlib1g-dev liblzma-dev libxen-dev
3. 确保在源码中`./configure` 都是yes
4. 需要file命令支持
5. kdump-tools kexec-tools crash

## 命令组成
* kexec : 内核崩溃时,使用kexec -p --> Load the new kernel for use on panic. 使用新版本
* kdump
* vmcore-dmesg
* bin-to-hex
* kdump-config `sh`
* crash : 调试内存镜像core文件

## 关键步骤
1. 第一个捕获内核必须打开调试等信息
2. 第2个内核为随便什么内核，版本什么都没关系，因为是为了捕获第一个内核，千万不能和第一个内核一样，否则不能成功，目前我采用的ubuntu系统的pae内核


## 配置组成
* `/etc/default/kdump-tools`
* `/usr/sbin/kdump-config`

## 加载kdump kernel
kdump-config status
current state   : ready to kdump

kdump-config show
DUMP_MODE:        kdump
USE_KDUMP:        1
KDUMP_SYSCTL:     kernel.panic_on_oops=1
KDUMP_COREDIR:    /var/crash
crashkernel addr: 0x10000000
current state:    ready to kdump

kexec command:
  kexec -p --command-line="BOOT_IMAGE=/boot/vmlinuz-3.18.9-rt5 root=UUID=820b7305-2333-49c4-86d0-4df32d095bd0 ro quiet irqpoll maxcpus=1 nousb" --initrd=/boot/initrd.img-3.18.9-rt5 /boot/vmlinuz-3.18.9-rt5

## 卸载kdump kernel
kdump-config unload
[ ok ] unloaded kdump kernel.

kdump-config show
DUMP_MODE:        kdump
USE_KDUMP:        1
KDUMP_SYSCTL:     kernel.panic_on_oops=1
KDUMP_COREDIR:    /var/crash
crashkernel addr: 0x10000000
current state:    Not ready to kdump

kexec command:
  /sbin/kexec -p --command-line="BOOT_IMAGE=/boot/vmlinuz-3.18.9-rt5 root=UUID=820b7305-2333-49c4-86d0-4df32d095bd0 ro quiet irqpoll maxcpus=1 nousb" --initrd=/boot/initrd.img-3.18.9-rt5 /boot/vmlinuz-3.18.9-rt5

## 多内核
update-grub
grub-mkconfig

## 原理
1. 内核安装`kexec`系统调用

## 失败
https://www.cnblogs.com/lshs/p/6038935.html
objcopy --strip-debug ./vmlinux
make  modules_install INSTALL_MOD_STRIP=1 install

## crash使用

crash [OPTION]... NAMELIST MEMORY-IMAGE[@ADDRESS]     (dumpfile form)
crash [OPTION]... [NAMELIST]                          (live system form)
NAMELIST
    This is a pathname to an uncompressed kernel image (a vmlinux file), or a Xen hypervi‐
    sor image (a xen-syms file) which has been compiled with the "-g"  option.   If  using
    the dumpfile form, a vmlinux file may be compressed in either gzip or bzip2 formats.

`crash /mnt/vmlinux` : vmlinux:非压缩文件

## makedumpfile 

## 如何判断捕获内核是否加载
/sys/kernel/kexec_crash_loaded 的值。“1”为已经加载，“0”为还未加载。
可以通过向 /sys/kernel/kexec_crash_size 中输入一个比其原值小的数来缩小甚至完全释放 crashkernel。
## 查看系统版本
1. 
```sh
plt@ubuntu14:~$ lsb_release  -a
No LSB modules are available.
Distributor ID:IDUbuntu
Description:DescriptionUbuntu 14.04.5 LTS
Release:Release14.04
Codename:Codenametrusty
```
2. 
```sh
cat /etc/issue
Debian GNU/Linux 8 \n \l
```
## debian

dpkg --get-selections | grep linux
apt-get remove linux-headers-3.16.0   linux-image-3.16.0
update-grub
1. `apt-get install kdump-tools`
2. 修改`/etc/default/grub`
```
GRUB_CMDLINE_LINUX_DEFAULT="quiet"  -> GRUB_CMDLINE_LINUX_DEFAULT="quiet crashkernel=512M"
```
3. 更新grub`update-grub`
4. 修改`/etc/default/kdump-tools`文件
```conf
USE_KDUMP=1
```
5. 重启查看`cat /proc/iomem`文件;可以看到当前内存的分配情况，其中有一条“09000000-10ffffff : Crash kernel”的记录。可以计算一下这段地址的大小，为128Ｍ.
6. 查看kdump状态`/usr/sbin/kdump-config status`

## ubuntu
1. `apt-get install kexec-tools crash
`

## 测试
1. 使内核马上崩溃
`echo c > /proc/sysrq-trigger`
2. 产生的crash默认路径`/var/crash/`
3. 查找最新产生的crash文件`find /var/crash -type f -mtime -1`
4. `crash /usr/lib/debug/boot/vmlinux-3.5.0-17-generic dump.201305061817`
5. crash目录结构
```
生成dump文件后/var/crash的目录结构：

├── 201305061817
│   ├── config_link -> /boot/config-3.5.0-17-generic
│   ├── dump.201305061817
│   ├── kernel_link -> /usr/lib/debug/boot/vmlinux-3.5.0-17-generic
│   └── system.map_link -> /boot/System.map-3.5.0-17-generic
├── config_link -> /boot/config-3.5.0-17-generic
├── kernel_link -> /usr/lib/debug/boot/vmlinux-3.5.0-17-generic
├── kexec_cmd
└── system.map_link -> /boot/System.map-3.5.0-17-generic
  ump.201305061817就是生成的dump文件，后面的一串数字诶当时的时间戳
```


## 
1. `cp arch/x86/boot/bzImage /boot/vmlinuz-3.8.0-rc5`
2. `chmod a+x vmlinuz-3.8.0-rc5`
3. `update-initramfs -u -k  3.18.0-rc5 ` #version 
4. `cp System.map /boot/System.map-3.8.0-rc5`

https://blog.csdn.net/quqi99/article/details/8546687

## qemu快速reboot
kexec -l  --command-line="nokaslr root=/dev/nfs nfsroot=10.0.0.1:/home/work/wo
rke/github/buildroot-2018.02.2/output/target rw nfsaddrs=10.0.0.2:10.0.0.1:10.0.
0.1:255.255.255.0 console=ttyS0 init=/linuxrc quiet irqpoll maxcpus=1 nousb"  /b
zImage
kexec -e
系统会reboot进入kexec中load的内核
kexec -p  --command-line="nokaslr root=/dev/nfs nfsroot=10.0.0.1:/home/work/wo
rke/github/buildroot-2018.02.2/output/target rw nfsaddrs=10.0.0.2:10.0.0.1:10.0.
0.1:255.255.255.0 console=ttyS0 init=/linuxrc quiet irqpoll maxcpus=1 nousb"  /b
zImage
echo c > /proc/sysrq
系统会重启并进入捕获内核

