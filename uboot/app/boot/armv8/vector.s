.global vector_table_el1
.global vector_table_el2
.global vector_table_el3
.global do_crash

.global DebugASM
.global DebugASM_ISR
//Vector tables must be placed at a 2KB-aligned address
//.balign 0x800
.align 11
vector_table_el2:
curr_el_sp0_sync:
// The exception handler for the synchronous
// exception from the current EL using SP0.
	//mov x0, #0x1
	//b DebugASM_ISR
	ldr x0, =do_crash
	br x0
	b .
.balign 0x80
curr_el_sp0_irq:
// The exception handler for the IRQ exception
// from the current EL using SP0.
	mov x0, #0x2
	b DebugASM_ISR
	b .
.balign 0x80
curr_el_sp0_fiq:
// The exception handler for the FIQ exception
// from the current EL using SP0.
	mov x0, #0x3
	b DebugASM_ISR
	b .
.balign 0x80
curr_el_sp0_serror:
// The exception handler for the system error
// exception from the current EL using SP0.
	mov x0, #0x4
	b DebugASM
	b .


.balign 0x80
curr_el_spx_sync:
// The exception handler for the synchronous
// exception from the current EL using the
// current SP.
	//mov x0, #0x9
	//b DebugASM_ISR
	ldr x0, =do_crash
	br x0
	b .
.balign 0x80
curr_el_spx_irq:
// The exception handler for IRQ exception
// from the current EL using the current SP.
	mov x0, #0x6
	b DebugASM
	b .
.balign 0x80
curr_el_spx_fiq:
// The exception handler for the FIQ exception
// from the current EL using the current SP.
	mov x0, #0x7
	b DebugASM
	b .
.balign 0x80
curr_el_spx_serror:
// The exception handler for the system error
// exception from the current EL using the
// current SP.
	mov x0, #0x8
	b DebugASM
	b .


.balign 0x80
lower_el_aarch64_sync:
// The exception handler for the synchronous
// exception from a lower EL (AArch64).
	mov x0, #0x9
	b DebugASM
	b .
.balign 0x80
lower_el_aarch64_irq:
// The exception handler for the IRQ exception
// from a lower EL (AArch64).
	mov x0, #0x1
	b DebugASM
	b .
.balign 0x80
lower_el_aarch64_fiq:
// The exception handler for the FIQ exception
// from a lower EL (AArch64).
	mov x0, #0x2
	b DebugASM
	b .
.balign 0x80
lower_el_aarch64_serror: // The exception handler for the system error
// exception from a lower EL(AArch64).
	mov x0, #0x3
	b DebugASM
	b .


.balign 0x80
lower_el_aarch32_sync:
// The exception handler for the synchronous
// exception from a lower EL(AArch32).
	mov x0, #0x4
	b DebugASM
	b .
.balign 0x80
lower_el_aarch32_irq:
// The exception handler for the IRQ exception
// from a lower EL (AArch32).
	mov x0, #0x5
	b DebugASM
	b .
.balign 0x80
lower_el_aarch32_fiq:
// The exception handler for the FIQ exception
// from a lower EL (AArch32).
	mov x0, #0x6
	b DebugASM
	b .
.balign 0x80
lower_el_aarch32_serror: // The exception handler for the system error
// exception from a lower EL(AArch32).
	mov x0, #0x7
	b DebugASM
	b .

