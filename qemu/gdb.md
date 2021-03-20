# Use Cgdb And Qemu To Debug Kernel 

## restart nfs service
```/etc/exports
/home/work/worke/github/buildroot-2018.02.2/output/target *(rw,sync,no_root_squash)
```
`sudo vim /etc/default/nfs-kernel-server`
`末尾增加 RPCNFSDOPTS="--nfs-version 2,3,4 --debug --syslog"`
`sudo /etc/init.d/nfs-kernel-server restart`
`sudo service nfs-kernel-server  start`

## file:qemu-ifup
```
#!/bin/sh
echo "Executing /etc/qemu-ifup"
sudo ifconfig $1 10.0.0.1
```
## rootfs
```
etc/inittab
# Put a getty on the serial port
console::respawn:/sbin/getty -L  console 0 vt100 # GENERIC_SERIAL
```
```
    dev/console
    sudo mknod console c  5 1
```
### rootfs in buildroot
```makefile
package/busybox/busybox.mk:191
ifeq ($(BR2_TARGET_GENERIC_GETTY),y)    
    define BUSYBOX_SET_GETTY                
    $(SED) '/# GENERIC_SERIAL$$/s~^.*#~ttyS0::respawn:/sbin/getty -L $(SYSTEM_GETTY_OP    TIONS) ttyS0 $(SYSTEM_GETTY_BAUDRATE) $(SYSTEM_GETTY_TERM) #~' \                      
    $(TARGET_DIR)/etc/inittab       
endef 
```

## kernel
enable nfs client, e1000e, nfs on rootfs
打开内核debug信息.

## cmd
```start.sh
  sudo output/host/bin/qemu-system-x86_64 \
-smp 2 \
-m 2048M \
-kernel ./output/images/bzImage  \
-gdb tcp::1234 -S \
-nographic \
-net nic,vlan=0 \
-device e1000e,netdev=dev0 \
-netdev tap,id=dev0,ifname=tap0,script=./qemu-ifup \
-append "nokaslr root=/dev/nfs nfsroot=10.0.0.1:/opt/buildroot-2018.02.2/output/target rw     nfsaddrs=10.0.0.2:10.0.0.1:10.0.0.1:255.255.255.0  console=ttyS0 init=/linuxrc crashkernel=128M"
```
## gdb
`sudo service nfs-kernel-server restart`
`sudo exportfs -a`
`cgdb -d x86_64-buildroot-linux-gnu-gdb ./vmlinux`

`target remote 127.0.0.1:1234`

## debug Removal optimization

* Add `#pragma GCC optimize("O0")` in file head.  
* Add `__attribute__((optimize("-O0")))` in function head.  

## other
### dont use gdb
1. `nm vmlinux | grep [function name]`
2. `addr2line -e vmlinux [addr]`

### use gdb
* *.ko和vmlinux均可以使用gdb.
* `list *[func name]+[addr]` `eg: list *start_kernel+0x10`

### use objdump
* `objdump -d hello.o > assemble.txt`

### eg
`[<c14a18f3>] ? common_interrupt+0x33/0x38` 0x33:相对common_interrupt的偏移地址, 0x38:函数总大小

### 退出qemu

`Ctrl-a x组合键可以退出qemu`
