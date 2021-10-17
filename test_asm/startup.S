#include "arch.h"

.global GetCPUID
//weak函数在汇编中也可使用，不用定义函数体，.c文件中定义了即可（类型为(void func(void))）
.weak test_config
// 可以使用#include , #ifdef 
//  寄存器设置模板如下
//	ldr x0, =COM_UART_THR
//  mov x1, #49
//  str x1, [x0]
#include "v8_system.h"

#ifdef XXXX
	mov x20, #1
#else
	mov x20, xzr
#endif

#define	COM_UART_THR	0x11223344
	bl GetCPUID // 结果保存在x0
	ldr x1, =COM_UART_THR
	mov x2, #49
	mov x3, #10
	mov x4, #13
	add x0, x0, #0x30  // 2+'0' -> '2'
	str x0, [x1]
	str x2, [x1]
	str x3, [x1]
	str x4, [x1]

#if defined(AABBCC)
	bl func1
#endif
    bl test_config 
// a53 a55获取cpu id的api  c代码中可以使用 int getcpuid()来进行声明调用
	.type GetCPUID, "function"
	.cfi_startproc
GetCPUID:

    mrs x0, MIDR_EL1
    ubfx    x0, x0, #4, #12 // extract PartNum
    cmp x0, #0xD0B          // Cortex-A76
    b.eq DynamIQ
    cmp x0, #0xD0A          // Cortex-A75
    b.eq DynamIQ
    cmp x0, #0xD05          // Cortex-A55
    b.eq DynamIQ
    b Others
DynamIQ:
    mrs x0, MPIDR_EL1
    ubfx    x0, x0, #MPIDR_EL1_AFF1_LSB, #MPIDR_EL1_AFF_WIDTH
    ret
                                                                                                                              
Others:
    mrs x0, MPIDR_EL1
    ubfx    x0, x0, #MPIDR_EL1_AFF0_LSB, #MPIDR_EL1_AFF_WIDTH
    ret
    .cfi_endproc   	
