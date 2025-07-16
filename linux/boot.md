# 打印 printk
* 在调试内核启动过程中最重要的一步是printk 打印的输出;如下是uboot中输入bootm跳转到linux的打印，代码中强制去掉了image.ub的校验相关，进内核的前一步打印为`Starting kernel ...`
``` log
 bootm 0xc0000000

## Loading kernel from FIT Image at c0000000 ...

   Using 'conf@system-top.dtb' configuration

   Trying 'kernel@1' kernel subimage

     Description:  Linux kernel

     Type:         Kernel Image

     Compression:  uncompressed

     Data Start:   0xc00000cc

     Data Size:    20361216 Bytes = 19.4 MiB

     Architecture: AArch64

     OS:           Linux

     Load Address: 0xe0000000

     Entry Point:  0xe0000000

     Hash algo:    sha1

     Hash value:   dfc11c3c2e15d8736fb4ff1db5939c1d129946aa

## Loading ramdisk from FIT Image at c0000000 ...

   Using 'conf@system-top.dtb' configuration

   Trying 'ramdisk@1' ramdisk subimage

     Description:  linux-user-image

     Type:         RAMDisk Image

     Compression:  gzip compressed

     Data Start:   0xc136f6d8

     Data Size:    10761114 Bytes = 10.3 MiB

     Architecture: AArch64

     OS:           Linux

     Load Address: unavailable

     Entry Point:  unavailable

     Hash algo:    sha1

     Hash value:   54dc3c38df897520f80be6f490040d86a3e1c166

WARNING: 'compression' nodes for ramdisks are deprecated, please fix your .its file!

## Loading fdt from FIT Image at c0000000 ...

   Using 'conf@system-top.dtb' configuration

   Trying 'fdt@system-top.dtb' fdt subimage

     Description:  Flattened Device Tree blob

     Type:         Flat Device Tree

     Compression:  uncompressed

     Data Start:   0xc136b1d0

     Data Size:    17477 Bytes = 17.1 KiB

     Architecture: AArch64

     Hash algo:    sha1

     Hash value:   fa875decbff53b1b2f9fde93cc90a5504c0922ac

   Booting using the fdt blob at 0xc136b1d0

Working FDT set to c136b1d0

   Loading Kernel Image

   Loading Ramdisk to efe16000, end f085939a ... OK

   Loading Device Tree to 00000000c47f8000, end 00000000c47ff444 ... OK

Working FDT set to c47f8000

Hart: 0, FDT: 0xc47f8000

## Transferring control to kernel (at address e0000000) ...



Starting kernel ...
```
* earlycon=tomuart,mmio32,0x45000000 的打印如下，如下环境为riscV单core；弃用opensbi的打印，在vprintk_store的尾部加入如下函数`early_printk`，将buffer中的打印实时打印
* <img width="832" height="112" alt="image" src="https://github.com/user-attachments/assets/d51b6de6-0960-4937-802f-ce4d84fd9e6e" />

```
[    0.000000]earlycon: tomuart0 at MMIO32 0x0000000045000000 (options '')')

[    0.000000]printk: legacy bootconsole [tomuart0] enableded

[xx]=>register_earlycon line[163] tomaaa 

[    0.000000]Memory limited to 4084MBMB

[    0.000000]efi: UEFI not found.d.
```
