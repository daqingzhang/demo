	.text
	.syntax unified
	.thumb
	.type start, % function
	.global _start
	.global start
	.global nmi_handler
_start:

start:
	movs r0, #10
	movs r1, #0
loop:
	adds r1,r0,r1
	subs r0,#1
	cmp  r0,#0
	bne loop
deadloop:
	b deadloop
nmi_handler:
	bx lr
	.end
