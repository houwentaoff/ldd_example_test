
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

.global tom_start64
.type tom_start64, "function"
tom_start64:
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
	bl  memset

	bl DebugASM
	bl DebugASM
	bl main
	bl DebugASM
	//b exit
	//bl DebugASM
	
	RESTORE_X1_X30

	//ldr x29, =0x900000
	//ldr x28, [x29]
	//MOV SP, x28

    RET

/* print 1 \r \n */
	.type GetCPUID, "function"
	.cfi_startproc
DebugASM:
#define UART2_BASE      0xff1a0000
#define	COM_UART_THR	0xff1a0000 //UART2_BASE
#ldr x1, =COM_UART_THR
	ldr x1, =0xff1a0000
	mov x2, #49
	mov x3, #10
	mov x4, #13
	mov x0, xzr
	add x0, x0, #0x30  // 2+'0' -> '2'
	#str x0, [x1]
	#isb sy
	str x2, [x1]
	isb sy
	str x3, [x1]
	isb sy
	str x4, [x1]
	isb sy
	ret
	.cfi_endproc

