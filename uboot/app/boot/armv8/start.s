
.macro SAVE_X1_X30
    STP X1,X2, [SP, #-16]!
    STP X3,X4, [SP, #-16]!
    STP X5,X6, [SP, #-16]!
    STP X7,X8, [SP, #-16]!
    STP X9,X10, [SP, #-16]!
    STP X11,X12, [SP, #-16]!
    STP X13,X14, [SP, #-16]!
    STP X15,X16, [SP, #-16]!
    STP X17,X18, [SP, #-16]!
    STP X19,X20, [SP, #-16]!
    STP X21,X22, [SP, #-16]!
    STP X23,X24, [SP, #-16]!
    STP X25,X26, [SP, #-16]!
    STP X27,X28, [SP, #-16]!
    STP X29,X30, [SP, #-16]!
.endm

.macro RESTORE_X1_X30
    LDP X29,X30, [SP], #16
    LDP X27,X28, [SP], #16
    LDP X25,X26, [SP], #16
    LDP X23,X24, [SP], #16
    LDP X21,X22, [SP], #16
    LDP X19,X20, [SP], #16
    LDP X17,X18, [SP], #16
    LDP X15,X16, [SP], #16
    LDP X13,X14, [SP], #16
    LDP X11,X12, [SP], #16
    LDP X9,X10, [SP], #16
    LDP X7,X8, [SP], #16
    LDP X5,X6, [SP], #16
    LDP X3,X4, [SP], #16
    LDP X1,X2, [SP], #16
.endm

.global vector_table_el1
.global vector_table_el2
.global vector_table_el3

.global tom_start64
.type tom_start64, "function"
tom_start64:
// Initialize vector.
//	LDR X1, = vector_table_el2
//	MSR VBAR_EL3, X1
	LDR X1, = vector_table_el2
	MSR VBAR_EL2, X1
	LDR X1, = vector_table_el2
	MSR VBAR_EL1, X1

	//ldr x29, =0x900000
	//mov x28, sp
	//str x28, [x29]

	SAVE_X1_X30
/* Initializing exceptions. */
/* Initializing registers. */
/* Configuring the MMU and caches. */
/* Enabling NEON and Floating Point. */
/* Changing Exception levels. */
// Initialize the register bank.

	MOV X0, XZR
	MOV X1, XZR
	MOV X2, XZR
	MOV X3, XZR
	MOV X4, XZR
	MOV X5, XZR
	MOV X6, XZR
	MOV X7, XZR
	MOV X8, XZR
	MOV X9, XZR
	MOV X10, XZR
	MOV X11, XZR
	MOV X12, XZR
	MOV X13, XZR
	MOV X14, XZR
	MOV X15, XZR
	MOV X16, XZR
	MOV X17, XZR
	MOV X18, XZR
	MOV X19, XZR
	MOV X20, XZR
	MOV X21, XZR
	MOV X22, XZR
	MOV X23, XZR
	MOV X24, XZR
	MOV X25, XZR
	MOV X26, XZR
	MOV X27, XZR
	MOV X28, XZR
	MOV X29, XZR
	MOV X30, XZR
	
/*
#define CPU_STACK_SIZE 0x80000
// init sp ,new stack
// Initialize the stack pointer.
	ADR X1, __el1_stack_top
	ADD X1, X1, #4 
	MRS X2, MPIDR_EL1 
	AND X2, X2, #0xFF   // X2 == CPU number.
	a MOV X3, #0x80000  //cpu stack size
	MUL X3, X2, X3 // Create separated stack spaces
	SUB X1, X1, X3 // for each processor
	MOV SP, X1
*/

	// Zero the bss
	ldr x0, =__bss_start__ // Start of block
	mov x1, #0             // Fill value
	ldr x2, =__bss_end__   // End of block
	sub x2, x2, x0         // Length of block
	bl  memset   // 需要mmu(cache)初始化，否则会异常, 这里因为是uboot mmu已经初始化过了,若bootrom后则必须先进行mmu初始化
	mov x0, #0x1
	bl DebugASM
	mov x0, #0x2
	bl DebugASM
	bl main
	mov x0, #0x3
	bl DebugASM
	//b exit
	//bl DebugASM
	
	RESTORE_X1_X30

	//ldr x29, =0x900000
	//ldr x28, [x29]
	//MOV SP, x28

    RET

.global DebugASM
/* print 1 \r \n */
	.type GetCPUID, "function"
	.cfi_startproc
DebugASM:
#define UART2_BASE      0xff1a0000
#define	COM_UART_THR	0xff1a0000 //UART2_BASE
//    ldr x1, =COM_UART_THR
	ldr x1, =0xff1a0000
	//mov x2, #48
	mov x3, #10
	mov x4, #13
	mov x5, xzr
	add x5, x0, #0x30  // 2+'0' -> '2'
	str x5, [x1]
	//isb sy
	//str x2, [x1]
	isb sy
	str x3, [x1]
	isb sy
	str x4, [x1]
	isb sy
	ret
	.cfi_endproc

.global DebugASM_ISR
/* print 1 \r \n */
	.type DebugASM_ISR, "function"
	.cfi_startproc
DebugASM_ISR:
#define UART2_BASE      0xff1a0000
#define	COM_UART_THR	0xff1a0000 //UART2_BASE
//ldr x1, =COM_UART_THR
	ldr x1, =0xff1a0000
	//mov x2, #48
	mov x3, #10
	mov x4, #13
	mov x5, xzr
	add x5, x0, #0x30  // 2+'0' -> '2'
	str x5, [x1]
	//isb sy
	//str x2, [x1]
	isb sy
	str x3, [x1]
	isb sy
	str x4, [x1]
	isb sy
	eret
	.cfi_endproc

