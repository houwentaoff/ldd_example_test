# arm-clang 环境下的baremetal

## 选用编译器
armclang编译器:`https://developer.arm.com/documentation/ka005198/latest`


## scatter文件

## 初始化堆
```c
#include <rt_heap.h>
_init_alloc((unsigned long)(void *)Image$$ARM_LIB_HEAP$$ZI$$Base, (unsigned long)(void *)Image$$ARM_LIB_HEAP$$ZI$$Base + 0x1000)
```
## 初始化栈
```
IMPORT |Image$$ARM_LIB_STACK$$ZI$$Base|
ldr x1, =|Image$$ARM_LIB_STACK$$ZI$$Base|
mov sp, x1
```

## 初始化浮点数
```c
#include <locale.h>
extern void _fp_init(void);
setlocale(LC_ALL, "C")
_fp_init()
```
在调用如上2个函数后就能使用`printf`打印浮点数

## zero the bss
使用`bl __main`会使用C库初始化堆栈,bss,float 但若使用`bl functest`则需要自己初始化bss,堆栈，float.
```asm
IMPORT |Image$$BSS$$ZI$$Base|
IMPORT |Image$$BSS$$ZI$$Limit|
EXTERN memset

ldr x0, = |Image$$BSS$$ZI$$Base|
mov x1, #0
ldr x2, = |Image$$BSS$$ZI$$Limit|
sub x2, x2, x0
bl memset
```

## printf使用
 和aarch64-none-elf不同的是，armclang单独实现的C库，需要实现不同的函数才能使用printf
 使用trace32汇编单步debug时可以看到printf具体过程和重载的函数

## 参考
 Image$$之类的宏的含义和用法可在`DUI0041C.pdf`->`ARM Software Development Toolkit reference Guide`中找到
