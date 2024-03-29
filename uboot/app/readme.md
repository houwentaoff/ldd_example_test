# gnu-gcc 环境下的baremetal

# uboot启动裸机程序

## 选用编译器
* AArch64 ELF bare-metal target (aarch64-none-elf) :`https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads`
* AArch64 GNU/Linux target (aarch64-none-linux-gnu) : 同上

## 汇编参考
* 汇编ref -> `https://developer.arm.com/documentation/102438/0100/Learning-about-assembly-language`
* armv8指令参考 `https://developer.arm.com/architectures/instruction-sets/base-isas/a64` 
     -> learn more `https://developer.arm.com/documentation/ddi0596/2021-12/Base-Instructions`

## gccc->ds5 IDE 参考
* gcc迁移为ds5/ads IDE环境时需要参考的文档 `migration_and_compatibility_guide_100068_6.20_00_en.pdf` --> `https://developer.arm.com/documentation/100068/0620/?lang=en`
* `Image$$ execution region symbols`arm链接器使用的特有符号 `armlink_user_guide_100070_0612_00_en.pdf` --> `https://developer.arm.com/documentation/100070/0612/?lang=en`（配合scatter使用）

## ADS/DS5 Scatter文件

## ds5 简单配置

## ds5 start.s 和 arm-gnu-gcc 中的start.s

## armv8启动参考
* `boot` 参考手册 `DAI0527A_baremetal_boot_code_for_ARMv8_A_processors.pdf` -> `https://developer.arm.com/documentation/dai0527/a/?lang=en`
* `armv8` 参考手册 `DEN0024A_v8_architecture_PG.pdf` -> `https://developer.arm.com/documentation/den0024/a/?lang=en` `DDI0487J_a_a-profile_architecture_reference_manual.pdf` -> `https://developer.arm.com/documentation/ddi0487/ja?lang=en`
* `armv7` 参考手册 `DEN0013D_cortex_a_series_PG.pdf` -> `https://developer.arm.com/documentation/den0013/d/?lang=en`
## armv8 寄存器
* `armv8`寄存器 `SysReg_xml_v88A-2021-12.pdf` -> `https://developer.arm.com/documentation/ddi0595/2021-12/?lang=en`
## armv8 MMU
* `armv8 mmu`手册 `learn_the_architecture_-_aarch64_memory_management_guide_101811_0103_00_en.pdf` -> `https://developer.arm.com/documentation/101811/0103/?lang=en` 还有一部分见armv8 手册MMU描述
MMU寄存器`TTBR0_EL3` `MAIR_EL3` `TCR_EL3` 描述见 `SysReg_xml_v88A-2021-12.pdf`

## armv8 gic  
* `armv8 gic`手册 `IHI0069H_gic_architecture_specification.pdf` -> `https://developer.arm.com/documentation/ihi0069/h/?lang=en`

## 如何测试
1. `make`
3. `uboot: loady txt-sections ` 串口命令loady下载程序 
2. `uboot: go main.addr/entry addr. (Entry point address,asm中的main函数地址)`

## 如何编写ld文件
`ld --verbose`会打印出默认的lds; lds已经打包进`ld` 程序里面去了
lds文件中能看到txt段并不死放在开头，提前即可.

## 如何获取_start文件(main之前的start.S)
* 已知反汇编中有`_start`部分反汇编代码, 那么是从哪里来的呢?
    通过`map`文件可以看到`_start`符号地址为`/opt/rk3399/linux-sdk/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/../aarch64-linux-gnu/libc/usr/lib/crt1.o`
    反汇编该文件就会得到`_start`部分的汇编代码`aarch64-linux-gnu-objdump -S /opt/rk3399/linux-sdk/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/../aarch64-linux-gnu/libc/usr/lib/crt1.o > start.S`
    
```start.S
(start.S)    
Disassembly of section .text:

0000000000000000 <_start>:
   0:	d280001d 	mov	x29, #0x0                   	// #0
   4:	d280001e 	mov	x30, #0x0                   	// #0
   8:	aa0003e5 	mov	x5, x0
   c:	f94003e1 	ldr	x1, [sp]
  10:	910023e2 	add	x2, sp, #0x8
  14:	910003e6 	mov	x6, sp
  18:	580000c0 	ldr	x0, 30 <_start+0x30>
  1c:	580000e3 	ldr	x3, 38 <_start+0x38>
  20:	58000104 	ldr	x4, 40 <_start+0x40>
  24:	94000000 	bl	0 <__libc_start_main>
  28:	94000000 	bl	0 <abort>

```
* 同理可以获取到`__libc_start_main` 反汇编

## 如何将elf转换成bin文件
* bin文件应该全为data，即`file hello.bin` 结果应为data,如果不为data说明含有文件头信息被`file`命令解析出来了
* bin应该比elf小很多，如果不是，原因都由`ld`链接文件导致,请仔细检查,如txt段前有头文件信息或`--gap-fill=0xff`

## 如何显示main地址
* 1
``` hello.ld
(hello.ld)
ENTRY(main) #显示如下的 entry point address

```
```bash
`readelf -h hello.elf`
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           AArch64
  Version:                           0x1
  Entry point address:               0x800560
  Start of program headers:          64 (bytes into file)
  Start of section headers:          4777984 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         5
  Size of section headers:           64 (bytes)
  Number of section headers:         37
  Section header string table index: 34
```
* 2
  从反汇编asm文件中可以找到main的地址
## entry point  
  该符号作用为`t32`或加载器使用，便于将`PC`设置到该值上，若使用`t32` 手动设置`pc`地址或者默认使用`entry point`位置  
```t32 script
r.s pc 0x33000000
go
```
## 如何运行
* `go 0x800560` 直接从main启动，由于没有从start.s的_start开始执行汇编所以glibc并没有初始化，构造函数并不能执行，malloc也不能被执行
printf建议使用uboot自身的地址强制赋值使用即可.另外还需注意字符串后面必须加'\0'，都斯因为没有初始化，不过初始化的全局变量还是能正
常使用
* 已添加malloc需要实现的函数和printf. `malloc``printf`已可使用
* 已添加start.s

## 全局变量的初始化
```hello.c
(hello.c)
static int a=0x23;//data段; 通过readelf -s hello.elf |grep ' a' 查看其地址，然后在map文件中查看所属段
static int b=0x0;//bss段
static int c;//bss段
int d= 0x34;//data段
int e;//bss段
...
char *p = "hello\0";//rodata段
```
* 未初始化的变量会放入`bss`段,`hello.bin`中并不包含`bss`这部分，需在`main`之前进行初始化(start.S).   
* 已经初始化的变量会放入`data`段,`hello.bin`中包含`data段`部分 
* 常量字符串放入`rodata`段；(通过在`bin`文件中查找字符串常量,取出地址后，在`map`文件中查找范围能看到位于`.rodata`段,使用`readelf -S hello.elf`能看到所有段的范围)

