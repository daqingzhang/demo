.globl num_add_one
num_add_one:
	stmfd sp!,{r4-r8,lr}
	mov r4,#1
	ldr r5,=0xabcd1230
	add r0,r0,r4
	add r0,r0,r5
	ldmfd sp!,{r4-r8,pc}

.globl num_shift
num_shift:
	stmfd sp!,{r4-r8,lr}
	mov r4,r0
	mov r4,r4,lsl #1
	mov r4,r4,ror #3
	mov r0,r4
	ldmfd sp!,{r4-r8,pc}

.globl num_add
num_add:
	add r0,r0,r1
	mov pc,lr

.globl num_sub
num_sub:
	sub r0,r0,r1
	mov pc,lr

.globl num_mul
num_mul:
	mul r0,r0,r1
	mov pc,lr
/*
 * s(n) = 1 + 2 +...+ n
 */
.globl num_sum
num_sum:
	stmfd sp!,{r3-r8,lr}
	mov r4,r0
	sub r0,r0,r0
	mov r5,r0
do_sum1:
	cmp r4,#0
	beq do_sum2
	add r5,r5,#1
	add r0,r0,r5
	sub r4,r4,#1
	b do_sum1
do_sum2:
	ldmfd sp!,{r3-r8,pc}
/*
 * a(0) = x;
 * a(i+1) = a(i) + d;
 * s(n) = a(0) + a(1) + ... + a(n)
 *
 * r0 = x, r1 = d, r2 = n
 */

.globl num_sn
num_sn:
	stmfd sp!,{r3,lr}
	mov r3,r0
	sub r2,r2,#1
do_sn1:
	cmp r2,#0
	beq do_sn2
	add r0,r0,r1
	add r3,r3,r0
	sub r2,r2,#1
	b do_sn1
do_sn2:
	mov r0,r3
	ldmfd sp!,{r3,pc}


/*
 * void *memcpy_s0(void *dst,const void *src, unsigned size);
 * r0 = dst, r1 = src, r2 = size
 */
.globl memcpy_s0
memcpy_s0:
	stmfd sp!,{r3-r8,lr}
	mov r8,#0
	mov r3,r0
s_mcpy_16b:
	cmp r2,#16
	blt s_mcpy_1b
	ldmia r1!, {r4-r7}
	stmia r0!, {r4-r7}
	sub r2,r2,#16
	b s_mcpy_16b
s_mcpy_1b:
	cmp r4,#0
	beq e_mcpy_1b
	ldrb r4,[r1]
	strb r4,[r0]
	add r1,r1,#1
	add r0,r0,#1
	sub r2,r2,#1
	b s_mcpy_1b
e_mcpy_1b:
	mov r0,r3
	ldmfd sp!,{r3-r8,pc}
/*
 * void *memset_s(void *dst, int n, unsigned size)
 * r0 = dst,
 * r1 = n,
 * r2 = size
 */

.globl memset_s1
memset_s1:
	stmfd sp!,{r4,lr}
	mov r4,r0	
	add r2,r2,r0

memset_s1_begin:
	cmp r2,r4
	beq memset_s1_end
	strb r1,[r4]
	add r4,r4,#1
	b memset_s1_begin

memset_s1_end:
	ldmfd sp!,{r4,pc}




.globl memset_s2
memset_s2:
	stmfd sp!,{r3-r8,lr}

	mov r4,r0
	add r5,r0,r2

	cmp r2,#4
	bls memset_less_4B

	cmp r2,#16
	bls memset_less_16B
	b   memset_more_32B

memset_less_4B:
	strb r1,[r4]
	add r4,r4,#1
	cmp r4,r5
	beq memset_less_4B
	b   memset_end

memset_less_16B:

	bic r6,r4,#0x3
	orr r6,r6,#0x4
	cmp r4,r6
	beq memset_align32

memset_align8:
	strb r1,[r4]
	add r4,r4,#1

	cmp r4,r5
	beq memset_end

	cmp r4,r6
	bne memset_align8

memset_align32:
	str r1,[r4]
	add r4,r4,#4

	cmp r4,r5
	bne memset_align32

memset_more_32B:

memset_end:
	ldmfd sp!,{r3-r8,pc}


