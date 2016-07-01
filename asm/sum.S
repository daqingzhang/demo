.equ STACK_TOP, 0x20000800
.text
.syntax unified
.thumb
.global _start
.type start, % function

_start:

start:
	.word STACK_TOP,start
	movs r0, #10
	movs r1, #0
loop:
	adds r1,r0,r1
	subs r0,#1
	cmp  r0,#0
	bne loop
deadloop:
	b deadloop
	.end
