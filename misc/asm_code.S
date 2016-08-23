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
	stmfd sp!,{r4,lr}
	cmp r0,#0
	beq do_sum2
	mov r4,r0
do_sum1:
	sub r4,r4,#1
	add r0,r0,r4
	cmp r4,#0
	bne do_sum1
do_sum2:
	ldmfd sp!,{r4,pc}
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
	cmp r2,#0
	beq do_sn2
do_sn1:
	add r0,r0,r1
	add r3,r3,r0
	sub r2,r2,#1
	cmp r2,#0
	bne do_sn1
do_sn2:
	mov r0,r3
	ldmfd sp!,{r3,pc}

/*
 * void *memset_s(void *dst, int n, unsigned size)
 * r0 = dst,
 * r1 = n,
 * r2 = size
 */
.globl xmemset3
xmemset3:
	stmfd sp!,{r3,lr}
	mov r3,r0
	add r2,r2,r0
xmemset3_1:
	cmp r0,r2
	beq xmemset3_2
	strb r1,[r0]
	add r0,r0,#1
	b xmemset3_1
xmemset3_2:
	mov r0,r3
	ldmfd sp!,{r3,pc}

/*
 * r0 = *s
 * r1 = n
 * r2 = size
 * r4 = iaddr
 * r5 = aaddr
 * r6 = m
 * r7,r8, r9 is free
 */
.globl xmemset4
xmemset4:
	stmfd sp!, {r3-r9,lr}
	cmp r2,#0
	beq xmemset4_end

	cmp r2,#4
	bls xmemset_ls4B

xmemset4_hi4B:
	mov r4,r0
	bic r5,r0,#0x3
	add r5,r5,#0x4
	sub r6,r5,r4

	cmp r6,#4
	beq xmemset4_1

/*
 * not aligned data
 * r7 = dst
 * r8 = dst end
 */
xmemset4_0:
	mov r7,r4
	add r8,r4,r6
xmemset4_0_loop:
	strb r1,[r7]
	add r7,r7,#1
	cmp r7,r8
	bne xmemset4_0_loop
	sub r2,r2,r6	/* len = len - m */
	b xmemset4_2

xmemset4_1:
	sub r5,r5,#4

/* aligned data */
xmemset4_2:
	mov r6,r2,lsr #2	/* m = len >> 2 ,r2 = len */
	cmp r6,#0
	beq xmemset4_3

	mov r7,r5
	add r8,r5,r6,lsl #2
	and r1,r1,#0xFF
	mov r9,r1
	orr r9,r1,lsl #8
	orr r9,r1,lsl #16
	orr r9,r1,lsl #24
xmemset4_2_loop:
	str r9,[r7]
	add r7,r7,#4
	cmp r7,r8
	bne xmemset4_2_loop
	mov r5,r7

/* not aligned data */
xmemset4_3:
	mov r7,r6,lsl #2 
	sub r6,r2,r7
	cmp r6,#0
	beq xmemset4_4

	mov r7,r5	/* first addr */
	add r8,r5,r6	/* last addr */
xmemset4_3_loop:
	strb r1,[r7]
	add r7,r7,#1
	cmp r7,r8
	bne xmemset4_3_loop

xmemset4_4:
	b xmemset4_end

xmemset_ls4B:
	mov r4,r0
	add r2,r2,r0
xmemset4_ls4B_loop:
	strb r1,[r4]
	add r4,r4,#1
	cmp r4,r2
	bne xmemset4_ls4B_loop

xmemset4_end:
	ldmfd sp!, {r3-r9,pc}

/*
 * int operate_bits1(int org, int val,int mask)
 * org = org & (~(mask << 4))
 * org = org | ((0xFFF & val) << 4)
 *
 * r0 = org, r1 = val, r2 = mask
 */
.globl operate_bits1
operate_bits1:
	and r1,r1,r2
	bic r0,r0,r2,lsl #4
	orr r0,r0,r1,lsl #4
	mov pc,lr

