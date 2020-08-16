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
	ldr x0, =COM_UART_THR
	mov x1, #49
	mov x2, #10
	mov x3, #13
	str x1, [x0]
	str x2, [x0]
	str x3, [x0]