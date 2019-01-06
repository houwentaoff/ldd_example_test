# Use Cgdb And Qemu To Debug Kernel 

## restart nfs service
```/etc/exports
/home/work/worke/github/buildroot-2018.02.2/output/target *(rw,sync,no_root_squash)
```
`sudo service nfs-kernel-server  start`

## file:qemu-ifup
```
#!/bin/sh
echo "Executing /etc/qemu-ifup"
sudo ifconfig $1 10.0.0.1
```
## rootfs
```etc/inittab
# Put a getty on the serial port
console::respawn:/sbin/getty -L  console 0 vt100 # GENERIC_SERIAL
```
```dev/console
```

## kernel
enable nfs client, e1000e

## cmd
`sudo output/host/bin/qemu-system-x86_64 \
-smp 2 \
-m 1024M \
-kernel ./output/images/bzImage  \
-gdb tcp::1234 -S \
-nographic \
-net nic,vlan=0 \
-device e1000e,netdev=dev0 \
-netdev tap,id=dev0,ifname=tap0,script=./qemu-ifup \
-append "nokaslr root=/dev/nfs nfsroot=10.0.0.1:/home/work/worke/github/buildroot-2018.02.2/output/target rw     nfsaddrs=10.0.0.2:10.0.0.1:10.0.0.1:255.255.255.0  console=ttyS0 init=/linuxrc"`

## gdb
`cgdb -d x86_64-buildroot-linux-gnu-gdb ./vmlinux`

`target remote 127.0.0.1:1234`

