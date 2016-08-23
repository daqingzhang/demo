#include "FreeRTOS.h"
#include "task.h"
#include "asm.h"

.global uxCriticalNesting;
.global pxCurrentTCB;
.global vTaskSwitchContext;


.global __dsb
__dsb:
	mov r0,r0
	dsb
	mov pc,lr


.global __isb
__isb:
	mov r0, r0
	isb
	mov pc, lr

//__asm void vPortSVCHandler( void )

.global vPortSVCHandler
vPortSVCHandler:
	.align 3

	ldr	r3, =pxCurrentTCB	/* Restore the context. */
	ldr r1, [r3]			/* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
	ldr r0, [r1]			/* The first item in pxCurrentTCB is the task top of stack. */
	ldmia r0!, {r4-r11}	
	msr psp, r0				/* Restore the task stack pointer. */
	isb
	mov r0, #0
	msr	basepri, r0
	orr r14, #0xd
	bx r14

//__asm void prvStartFirstTask( void )

.global prvStartFirstTask
prvStartFirstTask:

	.align 8

	/* Use the NVIC offset register to locate the stack. */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]
	/* Set the msp back to the start of the stack. */
	msr msp, r0
	/* Globally enable interrupts. */
	cpsie i
	dsb
	isb
	/* Call SVC to start the first task. */
	svc 0
	nop


//__asm void xPortPendSVHandler( void )

.global xPortPendSVHandler
xPortPendSVHandler:

	.align 8

	mrs r0, psp
	isb

	ldr	r3, =pxCurrentTCB		/* Get the location of the current TCB. */
	ldr	r2, [r3]

	stmdb r0!, {r4-r11}			/* Save the remaining registers. */
	str r0, [r2]				/* Save the new top of stack into the first member of the TCB. */

	stmdb sp!, {r3, r14}
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0
	bl vTaskSwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3, r14}

	ldr r1, [r3]
	ldr r0, [r1]				/* The first item in pxCurrentTCB is the task top of stack. */
	ldmia r0!, {r4-r11}			/* Pop the registers and the critical nesting count. */
	msr psp, r0
	isb
	bx r14
	nop
	mov pc, lr

//__asm uint32_t ulPortSetInterruptMask( void )

.global ulPortSetInterruptMask
ulPortSetInterruptMask:
	.align 8

	mrs r0, basepri
	mov r1, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r1
	bx r14

//__asm void vPortClearInterruptMask( uint32_t ulNewMask )

.global vPortClearInterruptMask
vPortClearInterruptMask:
	.align 8

	msr basepri, r0
	bx r14

//__asm uint32_t vPortGetIPSR( void )

.global vPortGetIPSR
vPortGetIPSR:
	.align 8

	mrs r0, ipsr
	bx r14

