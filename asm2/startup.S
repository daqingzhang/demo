	.equ STACK_TOP, 0x20000800
	.syntax unified
	.global vectors_tab
	.global start
	.global nmi_hander
	.thumb

vectors_tab:
	.word STACK_TOP
	.word start
	.word nmi_handler
	.word 0x00000000
	.end
