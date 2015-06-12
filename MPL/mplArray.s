	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZNK3mpl12vertex_array3dotEi
	.align	1, 0x90
__ZNK3mpl12vertex_array3dotEi:
Leh_func_begin1:
	pushq	%rbp
Ltmp0:
	movq	%rsp, %rbp
Ltmp1:
	movq	%rdi, %rax
	movl	16(%rsi), %ecx
	imull	%ecx, %edx
	movslq	%edx, %rdx
	cmpl	$3, %ecx
	movq	(%rsi), %rcx
	jne	LBB1_2
	shlq	$4, %rdx
	movaps	(%rcx,%rdx), %xmm0
	movaps	16(%rcx,%rdx), %xmm1
	movaps	32(%rcx,%rdx), %xmm2
	mulps	%xmm1, %xmm1
	mulps	%xmm0, %xmm0
	addps	%xmm1, %xmm0
	mulps	%xmm2, %xmm2
	addps	%xmm0, %xmm2
	movaps	%xmm2, (%rdi)
	popq	%rbp
	ret
LBB1_2:
	shlq	$4, %rdx
	movaps	(%rcx,%rdx), %xmm0
	movaps	16(%rcx,%rdx), %xmm1
	mulps	%xmm1, %xmm1
	mulps	%xmm0, %xmm0
	addps	%xmm1, %xmm0
	movaps	%xmm0, (%rdi)
	popq	%rbp
	ret
Leh_func_end1:

	.section	__TEXT,__literal4,4byte_literals
	.align	2
LCPI2_0:
	.long	1065353216
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZNK3mpl12vertex_array14ungroup_vertexEi
	.align	1, 0x90
__ZNK3mpl12vertex_array14ungroup_vertexEi:
Leh_func_begin2:
	pushq	%rbp
Ltmp2:
	movq	%rsp, %rbp
Ltmp3:
	movl	%edx, %eax
	sarl	$2, %eax
	movq	%rdi, %rcx
	movl	16(%rsi), %r8d
	imull	%r8d, %eax
	cmpl	$3, %r8d
	movq	(%rsi), %rsi
	je	LBB2_2
	andq	$3, %rdx
	pxor	%xmm0, %xmm0
	jmp	LBB2_3
LBB2_2:
	leal	2(%rax), %r8d
	movslq	%r8d, %r8
	shlq	$4, %r8
	addq	%rsi, %r8
	andq	$3, %rdx
	movss	(%r8,%rdx,4), %xmm0
LBB2_3:
	movslq	%eax, %rax
	movq	%rax, %r8
	shlq	$4, %r8
	addq	%rsi, %r8
	movss	(%r8,%rdx,4), %xmm1
	unpcklps	%xmm0, %xmm1
	incl	%eax
	movslq	%eax, %rax
	shlq	$4, %rax
	addq	%rsi, %rax
	movss	(%rax,%rdx,4), %xmm0
	movss	LCPI2_0(%rip), %xmm2
	unpcklps	%xmm2, %xmm0
	unpcklps	%xmm0, %xmm1
	movaps	%xmm1, (%rdi)
	movq	%rcx, %rax
	popq	%rbp
	ret
Leh_func_end2:

	.globl	__ZN3mpl12vertex_array10set_vertexEiPKf
	.align	1, 0x90
__ZN3mpl12vertex_array10set_vertexEiPKf:
Leh_func_begin3:
	pushq	%rbp
Ltmp4:
	movq	%rsp, %rbp
Ltmp5:
	movl	16(%rdi), %eax
	testl	%eax, %eax
	jle	LBB3_3
	movl	%esi, %ecx
	sarl	$2, %ecx
	andq	$3, %rsi
	xorl	%r8d, %r8d
	.align	4, 0x90
LBB3_2:
	movss	(%rdx,%r8,4), %xmm0
	imull	%ecx, %eax
	addl	%r8d, %eax
	movslq	%eax, %rax
	shlq	$4, %rax
	addq	(%rdi), %rax
	movss	%xmm0, (%rax,%rsi,4)
	incq	%r8
	movl	16(%rdi), %eax
	cmpl	%r8d, %eax
	jg	LBB3_2
LBB3_3:
	popq	%rbp
	ret
Leh_func_end3:

	.globl	__ZN3mpl12vertex_arrayC1Ev
	.align	1, 0x90
__ZN3mpl12vertex_arrayC1Ev:
Leh_func_begin4:
	pushq	%rbp
Ltmp6:
	movq	%rsp, %rbp
Ltmp7:
	movq	$0, (%rdi)
	movl	$0, 8(%rdi)
	movl	$0, 12(%rdi)
	movl	$3, 16(%rdi)
	popq	%rbp
	ret
Leh_func_end4:

	.globl	__ZN3mpl12vertex_arrayC2Ev
	.align	1, 0x90
__ZN3mpl12vertex_arrayC2Ev:
Leh_func_begin5:
	pushq	%rbp
Ltmp8:
	movq	%rsp, %rbp
Ltmp9:
	movq	$0, (%rdi)
	movl	$0, 8(%rdi)
	movl	$0, 12(%rdi)
	movl	$3, 16(%rdi)
	popq	%rbp
	ret
Leh_func_end5:

	.globl	__ZNK3mpl12vertex_array3sumEi
	.align	1, 0x90
__ZNK3mpl12vertex_array3sumEi:
Leh_func_begin6:
	pushq	%rbp
Ltmp10:
	movq	%rsp, %rbp
Ltmp11:
	movq	%rdi, %rax
	movl	16(%rsi), %ecx
	imull	%ecx, %edx
	movslq	%edx, %rdx
	cmpl	$3, %ecx
	movq	(%rsi), %rcx
	jne	LBB6_2
	shlq	$4, %rdx
	movaps	(%rcx,%rdx), %xmm0
	addps	16(%rcx,%rdx), %xmm0
	addps	32(%rcx,%rdx), %xmm0
	jmp	LBB6_3
LBB6_2:
	shlq	$4, %rdx
	movaps	(%rcx,%rdx), %xmm0
	addps	16(%rcx,%rdx), %xmm0
LBB6_3:
	movaps	%xmm0, (%rdi)
	popq	%rbp
	ret
Leh_func_end6:

	.globl	__ZN3mpl12vertex_arraypLERKNS_6float4E
	.align	1, 0x90
__ZN3mpl12vertex_arraypLERKNS_6float4E:
Leh_func_begin7:
	pushq	%rbp
Ltmp12:
	movq	%rsp, %rbp
Ltmp13:
	movl	8(%rdi), %eax
	movl	16(%rdi), %ecx
	cltd
	idivl	%ecx
	leal	(,%rax,4), %edx
	testl	%edx, %edx
	jle	LBB7_6
	shll	$2, %eax
	xorl	%r10d, %r10d
	movabsq	$4294967296, %rdx
	.align	4, 0x90
LBB7_4:
	testl	%ecx, %ecx
	jle	LBB7_3
	xorl	%r9d, %r9d
	movq	%r10, %r8
	.align	4, 0x90
LBB7_2:
	movss	(%rsi,%r9,4), %xmm0
	pshufd	$0, %xmm0, %xmm0
	movq	(%rdi), %rcx
	movq	%r8, %r11
	sarq	$32, %r11
	shlq	$4, %r11
	addps	(%rcx,%r11), %xmm0
	movaps	%xmm0, (%rcx,%r11)
	addq	%rdx, %r8
	incq	%r9
	movl	16(%rdi), %ecx
	cmpl	%r9d, %ecx
	jg	LBB7_2
LBB7_3:
	addq	%rdx, %r10
	decl	%eax
	jne	LBB7_4
LBB7_6:
	movq	%rdi, %rax
	popq	%rbp
	ret
Leh_func_end7:

	.globl	__ZN3mpl12vertex_arraypLERKS0_
	.align	1, 0x90
__ZN3mpl12vertex_arraypLERKS0_:
Leh_func_begin8:
	pushq	%rbp
Ltmp14:
	movq	%rsp, %rbp
Ltmp15:
	cmpl	$0, 8(%rdi)
	jle	LBB8_3
	xorl	%eax, %eax
	xorl	%ecx, %ecx
	.align	4, 0x90
LBB8_2:
	movq	(%rdi), %rdx
	movaps	(%rdx,%rax), %xmm0
	movq	(%rsi), %r8
	addps	(%r8,%rax), %xmm0
	movaps	%xmm0, (%rdx,%rax)
	addq	$16, %rax
	incl	%ecx
	cmpl	%ecx, 8(%rdi)
	jg	LBB8_2
LBB8_3:
	movq	%rdi, %rax
	popq	%rbp
	ret
Leh_func_end8:

	.globl	__ZN3mpl12vertex_array9normalizeEv
	.align	1, 0x90
__ZN3mpl12vertex_array9normalizeEv:
Leh_func_begin9:
	pushq	%rbp
Ltmp16:
	movq	%rsp, %rbp
Ltmp17:
	movl	16(%rdi), %eax
	cmpl	$3, %eax
	je	LBB9_5
	cmpl	$2, %eax
	jne	LBB9_8
	cmpl	$0, 8(%rdi)
	jle	LBB9_8
	movl	$16, %eax
	xorl	%ecx, %ecx
	.align	4, 0x90
LBB9_4:
	movq	(%rdi), %rdx
	movaps	-16(%rdx,%rax), %xmm0
	movaps	(%rdx,%rax), %xmm1
	movaps	%xmm1, %xmm2
	mulps	%xmm2, %xmm2
	movaps	%xmm0, %xmm3
	mulps	%xmm3, %xmm3
	addps	%xmm2, %xmm3
	rsqrtps	%xmm3, %xmm2
	mulps	%xmm2, %xmm0
	movaps	%xmm0, -16(%rdx,%rax)
	mulps	%xmm2, %xmm1
	movaps	%xmm1, (%rdx,%rax)
	addq	$32, %rax
	addl	$2, %ecx
	cmpl	%ecx, 8(%rdi)
	jg	LBB9_4
	jmp	LBB9_8
LBB9_5:
	cmpl	$0, 8(%rdi)
	jle	LBB9_8
	movl	$32, %eax
	xorl	%ecx, %ecx
	.align	4, 0x90
LBB9_7:
	movq	(%rdi), %rdx
	movaps	-32(%rdx,%rax), %xmm0
	movaps	-16(%rdx,%rax), %xmm1
	movaps	(%rdx,%rax), %xmm2
	movaps	%xmm1, %xmm3
	mulps	%xmm3, %xmm3
	movaps	%xmm0, %xmm4
	mulps	%xmm4, %xmm4
	addps	%xmm3, %xmm4
	movaps	%xmm2, %xmm3
	mulps	%xmm3, %xmm3
	addps	%xmm4, %xmm3
	rsqrtps	%xmm3, %xmm3
	mulps	%xmm3, %xmm0
	movaps	%xmm0, -32(%rdx,%rax)
	mulps	%xmm3, %xmm1
	movaps	%xmm1, -16(%rdx,%rax)
	mulps	%xmm3, %xmm2
	movaps	%xmm2, (%rdx,%rax)
	addq	$48, %rax
	addl	$3, %ecx
	cmpl	%ecx, 8(%rdi)
	jg	LBB9_7
LBB9_8:
	popq	%rbp
	ret
Leh_func_end9:

	.globl	__ZN3mpl12vertex_arraymLEf
	.align	1, 0x90
__ZN3mpl12vertex_arraymLEf:
Leh_func_begin10:
	pushq	%rbp
Ltmp18:
	movq	%rsp, %rbp
Ltmp19:
	cmpl	$0, 8(%rdi)
	jle	LBB10_3
	pshufd	$0, %xmm0, %xmm0
	xorl	%eax, %eax
	xorl	%ecx, %ecx
	.align	4, 0x90
LBB10_2:
	movq	(%rdi), %rdx
	movaps	(%rdx,%rax), %xmm1
	mulps	%xmm0, %xmm1
	movaps	%xmm1, (%rdx,%rax)
	addq	$16, %rax
	incl	%ecx
	cmpl	%ecx, 8(%rdi)
	jg	LBB10_2
LBB10_3:
	movq	%rdi, %rax
	popq	%rbp
	ret
Leh_func_end10:

	.globl	__ZN3mpl12vertex_arraymLERKS0_
	.align	1, 0x90
__ZN3mpl12vertex_arraymLERKS0_:
Leh_func_begin11:
	pushq	%rbp
Ltmp20:
	movq	%rsp, %rbp
Ltmp21:
	cmpl	$0, 8(%rdi)
	jle	LBB11_3
	xorl	%eax, %eax
	xorl	%ecx, %ecx
	.align	4, 0x90
LBB11_2:
	movq	(%rdi), %rdx
	movaps	(%rdx,%rax), %xmm0
	movq	(%rsi), %r8
	mulps	(%r8,%rax), %xmm0
	movaps	%xmm0, (%rdx,%rax)
	addq	$16, %rax
	incl	%ecx
	cmpl	%ecx, 8(%rdi)
	jg	LBB11_2
LBB11_3:
	movq	%rdi, %rax
	popq	%rbp
	ret
Leh_func_end11:

	.globl	__ZN3mpl12vertex_arraymIERKNS_6float4E
	.align	1, 0x90
__ZN3mpl12vertex_arraymIERKNS_6float4E:
Leh_func_begin12:
	pushq	%rbp
Ltmp22:
	movq	%rsp, %rbp
Ltmp23:
	movl	8(%rdi), %eax
	movl	16(%rdi), %ecx
	cltd
	idivl	%ecx
	leal	(,%rax,4), %edx
	testl	%edx, %edx
	jle	LBB12_6
	shll	$2, %eax
	xorl	%r10d, %r10d
	movabsq	$4294967296, %rdx
	.align	4, 0x90
LBB12_4:
	testl	%ecx, %ecx
	jle	LBB12_3
	xorl	%r9d, %r9d
	movq	%r10, %r8
	.align	4, 0x90
LBB12_2:
	movq	(%rdi), %rcx
	movq	%r8, %r11
	sarq	$32, %r11
	shlq	$4, %r11
	movaps	(%rcx,%r11), %xmm0
	movss	(%rsi,%r9,4), %xmm1
	pshufd	$0, %xmm1, %xmm1
	subps	%xmm1, %xmm0
	movaps	%xmm0, (%rcx,%r11)
	addq	%rdx, %r8
	incq	%r9
	movl	16(%rdi), %ecx
	cmpl	%r9d, %ecx
	jg	LBB12_2
LBB12_3:
	addq	%rdx, %r10
	decl	%eax
	jne	LBB12_4
LBB12_6:
	movq	%rdi, %rax
	popq	%rbp
	ret
Leh_func_end12:

	.globl	__ZN3mpl12vertex_arraymIERKS0_
	.align	1, 0x90
__ZN3mpl12vertex_arraymIERKS0_:
Leh_func_begin13:
	pushq	%rbp
Ltmp24:
	movq	%rsp, %rbp
Ltmp25:
	cmpl	$0, 8(%rdi)
	jle	LBB13_3
	xorl	%eax, %eax
	xorl	%ecx, %ecx
	.align	4, 0x90
LBB13_2:
	movq	(%rdi), %rdx
	movaps	(%rdx,%rax), %xmm0
	movq	(%rsi), %r8
	subps	(%r8,%rax), %xmm0
	movaps	%xmm0, (%rdx,%rax)
	addq	$16, %rax
	incl	%ecx
	cmpl	%ecx, 8(%rdi)
	jg	LBB13_2
LBB13_3:
	movq	%rdi, %rax
	popq	%rbp
	ret
Leh_func_end13:

	.globl	__ZNK3mpl12vertex_array6lengthEi
	.align	1, 0x90
__ZNK3mpl12vertex_array6lengthEi:
Leh_func_begin14:
	pushq	%rbp
Ltmp26:
	movq	%rsp, %rbp
Ltmp27:
	movq	%rdi, %rax
	movl	16(%rsi), %ecx
	imull	%ecx, %edx
	movslq	%edx, %rdx
	cmpl	$3, %ecx
	movq	(%rsi), %rcx
	jne	LBB14_2
	shlq	$4, %rdx
	movaps	(%rcx,%rdx), %xmm0
	movaps	16(%rcx,%rdx), %xmm1
	movaps	32(%rcx,%rdx), %xmm2
	mulps	%xmm1, %xmm1
	mulps	%xmm0, %xmm0
	addps	%xmm1, %xmm0
	jmp	LBB14_3
LBB14_2:
	shlq	$4, %rdx
	movaps	(%rcx,%rdx), %xmm2
	movaps	16(%rcx,%rdx), %xmm0
	mulps	%xmm0, %xmm0
LBB14_3:
	mulps	%xmm2, %xmm2
	addps	%xmm0, %xmm2
	sqrtps	%xmm2, %xmm0
	movaps	%xmm0, (%rdi)
	popq	%rbp
	ret
Leh_func_end14:

	.globl	__ZN3mpl12vertex_array3dotERKS0_RKNS_6float4ERNS_12scalar_arrayE
	.align	1, 0x90
__ZN3mpl12vertex_array3dotERKS0_RKNS_6float4ERNS_12scalar_arrayE:
Leh_func_begin15:
	pushq	%rbp
Ltmp28:
	movq	%rsp, %rbp
Ltmp29:
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
Ltmp30:
	movq	%rdx, %rbx
	movq	%rsi, %r14
	movq	%rdi, %r15
	movl	8(%r15), %eax
	cltd
	idivl	16(%r15)
	cmpl	%eax, 12(%rbx)
	movl	%eax, %r12d
	jge	LBB15_5
	movl	%r12d, 12(%rbx)
	movq	(%rbx), %rdi
	testq	%rdi, %rdi
	jne	LBB15_3
	movl	%r12d, %eax
	jmp	LBB15_4
LBB15_3:
	callq	__ZdaPv
	movl	12(%rbx), %eax
LBB15_4:
	movslq	%eax, %rdi
	shlq	$4, %rdi
	callq	__Znam
	movq	%rax, (%rbx)
LBB15_5:
	testl	%r12d, %r12d
	movl	$0, %eax
	cmovnsl	%r12d, %eax
	movl	%eax, 8(%rbx)
	movl	16(%r15), %eax
	cmpl	$3, %eax
	je	LBB15_10
	cmpl	$2, %eax
	jne	LBB15_13
	testl	%r12d, %r12d
	movss	(%r14), %xmm0
	movss	4(%r14), %xmm1
	jle	LBB15_13
	pshufd	$0, %xmm1, %xmm1
	pshufd	$0, %xmm0, %xmm0
	movl	%r12d, %r14d
	xorl	%eax, %eax
	movabsq	$8589934592, %rcx
	movq	%rax, %rdx
	.align	4, 0x90
LBB15_9:
	movq	(%r15), %rsi
	movq	%rax, %rdi
	sarq	$32, %rdi
	shlq	$4, %rdi
	movaps	(%rsi,%rdi), %xmm2
	mulps	%xmm0, %xmm2
	orq	$16, %rdi
	movaps	(%rsi,%rdi), %xmm3
	mulps	%xmm1, %xmm3
	addps	%xmm2, %xmm3
	movq	(%rbx), %rsi
	movaps	%xmm3, (%rsi,%rdx)
	addq	%rcx, %rax
	addq	$16, %rdx
	decq	%r14
	jne	LBB15_9
	jmp	LBB15_13
LBB15_10:
	testl	%r12d, %r12d
	movss	8(%r14), %xmm0
	movss	(%r14), %xmm1
	movss	4(%r14), %xmm2
	jle	LBB15_13
	pshufd	$0, %xmm0, %xmm0
	pshufd	$0, %xmm2, %xmm2
	pshufd	$0, %xmm1, %xmm1
	movl	%r12d, %eax
	xorl	%ecx, %ecx
	movabsq	$8589934592, %rdx
	movabsq	$4294967296, %rsi
	movabsq	$12884901888, %rdi
	movq	%rcx, %r8
	.align	4, 0x90
LBB15_12:
	leaq	(%rcx,%rdx), %r9
	leaq	(%rcx,%rsi), %r10
	movq	(%r15), %r11
	movq	%rcx, %r14
	sarq	$32, %r14
	shlq	$4, %r14
	movaps	(%r11,%r14), %xmm3
	mulps	%xmm1, %xmm3
	sarq	$32, %r10
	shlq	$4, %r10
	movaps	(%r11,%r10), %xmm4
	mulps	%xmm2, %xmm4
	addps	%xmm3, %xmm4
	sarq	$32, %r9
	shlq	$4, %r9
	movaps	(%r11,%r9), %xmm3
	mulps	%xmm0, %xmm3
	addps	%xmm4, %xmm3
	movq	(%rbx), %r9
	movaps	%xmm3, (%r9,%r8)
	addq	%rdi, %rcx
	addq	$16, %r8
	decq	%rax
	jne	LBB15_12
LBB15_13:
	movq	%rbx, %rax
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
Leh_func_end15:

	.globl	__ZN3mpl12vertex_array3dotERKS0_S2_RNS_12scalar_arrayE
	.align	1, 0x90
__ZN3mpl12vertex_array3dotERKS0_S2_RNS_12scalar_arrayE:
Leh_func_begin16:
	pushq	%rbp
Ltmp31:
	movq	%rsp, %rbp
Ltmp32:
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$40, %rsp
Ltmp33:
	movq	%rdx, %rcx
	movq	%rcx, -48(%rbp)
	movq	%rsi, %rdx
	movq	%rdx, -56(%rbp)
	movq	%rdi, %rsi
	movq	%rsi, -64(%rbp)
	movl	8(%rdx), %eax
	movl	16(%rdx), %ebx
	cltd
	idivl	%ebx
	movl	%eax, %r14d
	movl	8(%rsi), %eax
	movl	16(%rsi), %r15d
	cltd
	idivl	%r15d
	movl	%eax, %r12d
	cmpl	%r14d, %r12d
	movl	%r14d, %r13d
	cmovll	%r12d, %r13d
	cmpl	%ebx, %r15d
	movl	%ebx, %eax
	cmovll	%r15d, %eax
	movl	%eax, -68(%rbp)
	cmpl	%r13d, 12(%rcx)
	jge	LBB16_5
	movq	%rcx, %rdx
	movl	%r13d, 12(%rdx)
	movq	(%rdx), %rdi
	testq	%rdi, %rdi
	jne	LBB16_3
	movl	%r13d, %eax
	jmp	LBB16_4
LBB16_3:
	callq	__ZdaPv
	movq	-48(%rbp), %rdx
	movl	12(%rdx), %eax
LBB16_4:
	movslq	%eax, %rdi
	shlq	$4, %rdi
	callq	__Znam
	movq	-48(%rbp), %rdx
	movq	%rax, (%rdx)
LBB16_5:
	testl	%r13d, %r13d
	movl	$0, %eax
	cmovnsl	%r13d, %eax
	movq	-48(%rbp), %rdx
	movl	%eax, 8(%rdx)
	cmpl	$3, -68(%rbp)
	je	LBB16_10
	cmpl	$2, -68(%rbp)
	jne	LBB16_13
	testl	%r13d, %r13d
	jle	LBB16_13
	notl	%r12d
	notl	%r14d
	cmpl	%r12d, %r14d
	cmovlel	%r12d, %r14d
	notl	%r14d
	shlq	$32, %r15
	shlq	$32, %rbx
	xorl	%eax, %eax
	movabsq	$4294967296, %rcx
	movq	%rax, %rdx
	movq	%rax, %rsi
	.align	4, 0x90
LBB16_9:
	leaq	(%rax,%rcx), %rdi
	sarq	$32, %rdi
	shlq	$4, %rdi
	movq	-64(%rbp), %r8
	movq	(%r8), %r8
	movaps	(%r8,%rdi), %xmm0
	leaq	(%rdx,%rcx), %rdi
	sarq	$32, %rdi
	shlq	$4, %rdi
	movq	-56(%rbp), %r9
	movq	(%r9), %r9
	mulps	(%r9,%rdi), %xmm0
	movq	%rax, %rdi
	sarq	$32, %rdi
	shlq	$4, %rdi
	movaps	(%r8,%rdi), %xmm1
	movq	%rdx, %rdi
	sarq	$32, %rdi
	shlq	$4, %rdi
	mulps	(%r9,%rdi), %xmm1
	addps	%xmm0, %xmm1
	movq	-48(%rbp), %rdi
	movq	(%rdi), %rdi
	movaps	%xmm1, (%rdi,%rsi)
	addq	%r15, %rax
	addq	%rbx, %rdx
	addq	$16, %rsi
	decq	%r14
	jne	LBB16_9
	jmp	LBB16_13
LBB16_10:
	testl	%r13d, %r13d
	jle	LBB16_13
	notl	%r12d
	notl	%r14d
	cmpl	%r12d, %r14d
	cmovlel	%r12d, %r14d
	notl	%r14d
	shlq	$32, %r15
	shlq	$32, %rbx
	xorl	%eax, %eax
	movabsq	$8589934592, %rcx
	movabsq	$4294967296, %rdx
	movq	%rax, %rsi
	movq	%rax, %rdi
	.align	4, 0x90
LBB16_12:
	leaq	(%rax,%rcx), %r8
	sarq	$32, %r8
	shlq	$4, %r8
	movq	-64(%rbp), %r9
	movq	(%r9), %r9
	movaps	(%r9,%r8), %xmm0
	leaq	(%rsi,%rcx), %r8
	sarq	$32, %r8
	shlq	$4, %r8
	movq	-56(%rbp), %r10
	movq	(%r10), %r10
	mulps	(%r10,%r8), %xmm0
	leaq	(%rax,%rdx), %r8
	sarq	$32, %r8
	shlq	$4, %r8
	movaps	(%r9,%r8), %xmm1
	leaq	(%rsi,%rdx), %r8
	sarq	$32, %r8
	shlq	$4, %r8
	mulps	(%r10,%r8), %xmm1
	movq	%rax, %r8
	sarq	$32, %r8
	shlq	$4, %r8
	movaps	(%r9,%r8), %xmm2
	movq	%rsi, %r8
	sarq	$32, %r8
	shlq	$4, %r8
	mulps	(%r10,%r8), %xmm2
	addps	%xmm1, %xmm2
	addps	%xmm0, %xmm2
	movq	-48(%rbp), %r8
	movq	(%r8), %r8
	movaps	%xmm2, (%r8,%rdi)
	addq	%r15, %rax
	addq	%rbx, %rsi
	addq	$16, %rdi
	decq	%r14
	jne	LBB16_12
LBB16_13:
	movq	-48(%rbp), %rax
	addq	$40, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
Leh_func_end16:

	.globl	__ZN3mpl12vertex_array6createEiNS0_8format_tE
	.align	1, 0x90
__ZN3mpl12vertex_array6createEiNS0_8format_tE:
Leh_func_begin17:
	pushq	%rbp
Ltmp34:
	movq	%rsp, %rbp
Ltmp35:
	pushq	%r14
	pushq	%rbx
Ltmp36:
	movl	%edx, 16(%rdi)
	movl	%esi, %eax
	sarl	$2, %eax
	testb	$3, %sil
	setne	%cl
	movzbl	%cl, %ebx
	addl	%eax, %ebx
	imull	%edx, %ebx
	cmpl	%ebx, 12(%rdi)
	movq	%rdi, %r14
	jge	LBB17_5
	movl	%ebx, 12(%r14)
	movq	(%r14), %rdi
	testq	%rdi, %rdi
	jne	LBB17_3
	movl	%ebx, %eax
	jmp	LBB17_4
LBB17_3:
	callq	__ZdaPv
	movl	12(%r14), %eax
LBB17_4:
	movslq	%eax, %rdi
	shlq	$4, %rdi
	callq	__Znam
	movq	%rax, (%r14)
LBB17_5:
	testl	%ebx, %ebx
	movl	$0, %eax
	cmovnsl	%ebx, %eax
	movl	%eax, 8(%r14)
	popq	%rbx
	popq	%r14
	popq	%rbp
	ret
Leh_func_end17:

	.globl	__ZN3mpl12vertex_array5crossERKS0_S2_RS0_
	.align	1, 0x90
__ZN3mpl12vertex_array5crossERKS0_S2_RS0_:
Leh_func_begin18:
	pushq	%rbp
Ltmp37:
	movq	%rsp, %rbp
Ltmp38:
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
Ltmp39:
	movq	%rsi, %rbx
	movq	%rdi, %r14
	movl	16(%r14), %ecx
	cmpl	$3, %ecx
	movq	%rdx, %r15
	jne	LBB18_10
	movl	16(%rbx), %esi
	cmpl	$3, %esi
	jne	LBB18_10
	movl	8(%rbx), %eax
	cltd
	idivl	%esi
	movl	%eax, %esi
	shll	$2, %esi
	movl	8(%r14), %eax
	cltd
	idivl	%ecx
	shll	$2, %eax
	cmpl	%esi, %eax
	cmovgel	%esi, %eax
	sarl	$2, %eax
	leal	(%rax,%rax,2), %r12d
	movl	$3, 16(%r15)
	cmpl	%r12d, 12(%r15)
	jge	LBB18_7
	movl	%r12d, 12(%r15)
	movq	(%r15), %rdi
	testq	%rdi, %rdi
	jne	LBB18_5
	movl	%r12d, %eax
	jmp	LBB18_6
LBB18_5:
	callq	__ZdaPv
	movl	12(%r15), %eax
LBB18_6:
	movslq	%eax, %rdi
	shlq	$4, %rdi
	callq	__Znam
	movq	%rax, (%r15)
LBB18_7:
	xorl	%eax, %eax
	testl	%r12d, %r12d
	cmovsl	%eax, %r12d
	movl	%r12d, 8(%r15)
	movl	8(%rbx), %ecx
	movl	8(%r14), %edx
	cmpl	%ecx, %edx
	cmovll	%edx, %ecx
	testl	%ecx, %ecx
	jle	LBB18_16
	movabsq	$8589934592, %rdx
	movabsq	$4294967296, %rsi
	xorl	%edi, %edi
	movabsq	$12884901888, %r8
	.align	4, 0x90
LBB18_9:
	movq	(%rbx), %r9
	movq	%rsi, %r10
	sarq	$32, %r10
	shlq	$4, %r10
	movaps	(%r9,%r10), %xmm0
	movq	(%r14), %r11
	movq	%rdx, %r12
	sarq	$32, %r12
	shlq	$4, %r12
	movaps	(%r11,%r12), %xmm1
	movaps	%xmm1, %xmm2
	mulps	%xmm0, %xmm2
	movaps	(%r9,%r12), %xmm3
	movaps	(%r11,%r10), %xmm4
	movaps	%xmm4, %xmm5
	mulps	%xmm3, %xmm5
	subps	%xmm2, %xmm5
	movaps	(%r9,%rdi), %xmm2
	movaps	(%r11,%rdi), %xmm6
	movq	(%r15), %r9
	movaps	%xmm5, (%r9,%rdi)
	mulps	%xmm6, %xmm3
	mulps	%xmm2, %xmm1
	subps	%xmm3, %xmm1
	movq	(%r15), %r9
	movaps	%xmm1, (%r9,%r10)
	mulps	%xmm2, %xmm4
	mulps	%xmm0, %xmm6
	subps	%xmm4, %xmm6
	movq	(%r15), %r9
	movaps	%xmm6, (%r9,%r12)
	addq	%r8, %rsi
	addq	%r8, %rdx
	addq	$48, %rdi
	addl	$3, %eax
	cmpl	%ecx, %eax
	jl	LBB18_9
	jmp	LBB18_16
LBB18_10:
	movl	$3, 16(%r15)
	cmpl	$0, 12(%r15)
	jns	LBB18_15
	movl	$0, 12(%r15)
	movq	(%r15), %rdi
	testq	%rdi, %rdi
	jne	LBB18_13
	xorl	%edi, %edi
	jmp	LBB18_14
LBB18_13:
	callq	__ZdaPv
	movslq	12(%r15), %rdi
	shlq	$4, %rdi
LBB18_14:
	callq	__Znam
	movq	%rax, (%r15)
LBB18_15:
	movl	$0, 8(%r15)
LBB18_16:
	movq	%r15, %rax
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
Leh_func_end18:

	.globl	__ZN3mpl12vertex_array4copyEPKfiNS0_8format_tE
	.align	1, 0x90
__ZN3mpl12vertex_array4copyEPKfiNS0_8format_tE:
Leh_func_begin19:
	pushq	%rbp
Ltmp40:
	movq	%rsp, %rbp
Ltmp41:
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
Ltmp42:
	movl	%edx, %ebx
	movq	%rsi, %r14
	movl	%ecx, 16(%rdi)
	movl	%ebx, %eax
	sarl	$2, %eax
	testb	$3, %bl
	setne	%dl
	movzbl	%dl, %r15d
	addl	%eax, %r15d
	imull	%ecx, %r15d
	cmpl	%r15d, 12(%rdi)
	movq	%rdi, %r12
	jge	LBB19_5
	movl	%r15d, 12(%r12)
	movq	(%r12), %rdi
	testq	%rdi, %rdi
	jne	LBB19_3
	movl	%r15d, %eax
	jmp	LBB19_4
LBB19_3:
	callq	__ZdaPv
	movl	12(%r12), %eax
LBB19_4:
	movslq	%eax, %rdi
	shlq	$4, %rdi
	callq	__Znam
	movq	%rax, (%r12)
LBB19_5:
	xorl	%eax, %eax
	testl	%r15d, %r15d
	cmovsl	%eax, %r15d
	movl	%r15d, 8(%r12)
	testl	%ebx, %ebx
	jg	LBB19_7
	movl	16(%r12), %ecx
	movl	%r15d, %eax
	jmp	LBB19_12
	.align	4, 0x90
LBB19_7:
	movl	16(%r12), %ecx
	testl	%ecx, %ecx
	jle	LBB19_10
	movl	%ecx, %edx
	imull	%eax, %edx
	movl	%eax, %esi
	sarl	$2, %esi
	imull	%ecx, %esi
	movl	%eax, %edi
	andl	$3, %edi
	xorl	%r8d, %r8d
	.align	4, 0x90
LBB19_9:
	leal	(%rdx,%r8), %ecx
	movslq	%ecx, %rcx
	movss	(%r14,%rcx,4), %xmm0
	leal	(%rsi,%r8), %ecx
	movslq	%ecx, %rcx
	shlq	$4, %rcx
	addq	(%r12), %rcx
	movss	%xmm0, (%rcx,%rdi,4)
	incl	%r8d
	movl	16(%r12), %ecx
	cmpl	%r8d, %ecx
	jg	LBB19_9
LBB19_10:
	incl	%eax
	cmpl	%eax, %ebx
	jne	LBB19_7
	movl	8(%r12), %eax
LBB19_12:
	cltd
	idivl	%ecx
	leal	(,%rax,4), %edx
	cmpl	%ebx, %edx
	jle	LBB19_19
	shll	$2, %eax
	.align	4, 0x90
LBB19_14:
	testl	%ecx, %ecx
	jle	LBB19_18
	movl	%ebx, %edx
	sarl	$2, %edx
	imull	%edx, %ecx
	movl	%ebx, %edx
	andl	$3, %edx
	xorl	%esi, %esi
	.align	4, 0x90
LBB19_16:
	leal	(%rcx,%rsi), %edi
	movslq	%edi, %rdi
	shlq	$4, %rdi
	addq	(%r12), %rdi
	movl	$0, (%rdi,%rdx,4)
	incl	%esi
	movl	16(%r12), %edi
	cmpl	%esi, %edi
	jg	LBB19_16
	movl	%edi, %ecx
LBB19_18:
	incl	%ebx
	cmpl	%ebx, %eax
	jne	LBB19_14
LBB19_19:
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
Leh_func_end19:

	.globl	__ZN3mpl12vertex_array4copyERKS0_
	.align	1, 0x90
__ZN3mpl12vertex_array4copyERKS0_:
Leh_func_begin20:
	pushq	%rbp
Ltmp43:
	movq	%rsp, %rbp
Ltmp44:
	pushq	%r15
	pushq	%r14
	pushq	%rbx
	subq	$8, %rsp
Ltmp45:
	movq	%rsi, %rbx
	movl	8(%rbx), %eax
	movl	16(%rbx), %r14d
	movl	%r14d, 16(%rdi)
	cltd
	idivl	%r14d
	shll	$2, %eax
	sarl	$2, %eax
	imull	%eax, %r14d
	cmpl	%r14d, 12(%rdi)
	movq	%rdi, %r15
	jge	LBB20_5
	movl	%r14d, 12(%r15)
	movq	(%r15), %rdi
	testq	%rdi, %rdi
	jne	LBB20_3
	movl	%r14d, %eax
	jmp	LBB20_4
LBB20_3:
	callq	__ZdaPv
	movl	12(%r15), %eax
LBB20_4:
	movslq	%eax, %rdi
	shlq	$4, %rdi
	callq	__Znam
	movq	%rax, (%r15)
LBB20_5:
	xorl	%eax, %eax
	testl	%r14d, %r14d
	cmovsl	%eax, %r14d
	movl	%r14d, 8(%r15)
	testl	%r14d, %r14d
	jle	LBB20_8
	xorl	%ecx, %ecx
	.align	4, 0x90
LBB20_7:
	movq	(%rbx), %rdx
	movaps	(%rdx,%rcx), %xmm0
	movq	(%r15), %rdx
	movaps	%xmm0, (%rdx,%rcx)
	addq	$16, %rcx
	incl	%eax
	cmpl	%eax, 8(%r15)
	jg	LBB20_7
LBB20_8:
	addq	$8, %rsp
	popq	%rbx
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
Leh_func_end20:

	.globl	__ZN3mpl12vertex_arrayC1EiNS0_8format_tE
	.align	1, 0x90
__ZN3mpl12vertex_arrayC1EiNS0_8format_tE:
Leh_func_begin21:
	pushq	%rbp
Ltmp49:
	movq	%rsp, %rbp
Ltmp50:
	pushq	%r14
	pushq	%rbx
Ltmp51:
	movq	$0, (%rdi)
	movl	$0, 8(%rdi)
	movl	$0, 12(%rdi)
	movl	%edx, 16(%rdi)
	movl	%esi, %eax
	sarl	$2, %eax
	testb	$3, %sil
	setne	%cl
	movzbl	%cl, %ebx
	addl	%eax, %ebx
	imull	%edx, %ebx
	testl	%ebx, %ebx
	movq	%rdi, %r14
	jle	LBB21_3
	movl	%ebx, 12(%r14)
Ltmp46:
	movslq	%ebx, %rdi
	shlq	$4, %rdi
	callq	__Znam
Ltmp47:
	movq	%rax, (%r14)
LBB21_3:
	testl	%ebx, %ebx
	movl	$0, %eax
	cmovnsl	%ebx, %eax
	movl	%eax, 8(%r14)
	popq	%rbx
	popq	%r14
	popq	%rbp
	ret
LBB21_4:
Ltmp48:
	movq	%rax, %rbx
	movq	(%r14), %rdi
	testq	%rdi, %rdi
	je	LBB21_6
	callq	__ZdaPv
LBB21_6:
	movq	%rbx, %rdi
	callq	__Unwind_Resume_or_Rethrow
Leh_func_end21:
	.section	__TEXT,__gcc_except_tab
	.align	2
GCC_except_table21:
Lexception21:
	.byte	255
	.byte	155
	.byte	156
	.space	1
	.byte	3
	.byte	26
Lset0 = Ltmp46-Leh_func_begin21
	.long	Lset0
Lset1 = Ltmp47-Ltmp46
	.long	Lset1
Lset2 = Ltmp48-Leh_func_begin21
	.long	Lset2
	.byte	0
Lset3 = Ltmp47-Leh_func_begin21
	.long	Lset3
Lset4 = Leh_func_end21-Ltmp47
	.long	Lset4
	.long	0
	.byte	0
	.align	2

	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN3mpl12vertex_arrayC2EiNS0_8format_tE
	.align	1, 0x90
__ZN3mpl12vertex_arrayC2EiNS0_8format_tE:
Leh_func_begin22:
	pushq	%rbp
Ltmp55:
	movq	%rsp, %rbp
Ltmp56:
	pushq	%r14
	pushq	%rbx
Ltmp57:
	movq	$0, (%rdi)
	movl	$0, 8(%rdi)
	movl	$0, 12(%rdi)
	movl	%edx, 16(%rdi)
	movl	%esi, %eax
	sarl	$2, %eax
	testb	$3, %sil
	setne	%cl
	movzbl	%cl, %ebx
	addl	%eax, %ebx
	imull	%edx, %ebx
	testl	%ebx, %ebx
	movq	%rdi, %r14
	jle	LBB22_3
	movl	%ebx, 12(%r14)
Ltmp52:
	movslq	%ebx, %rdi
	shlq	$4, %rdi
	callq	__Znam
Ltmp53:
	movq	%rax, (%r14)
LBB22_3:
	testl	%ebx, %ebx
	movl	$0, %eax
	cmovnsl	%ebx, %eax
	movl	%eax, 8(%r14)
	popq	%rbx
	popq	%r14
	popq	%rbp
	ret
LBB22_4:
Ltmp54:
	movq	%rax, %rbx
	movq	(%r14), %rdi
	testq	%rdi, %rdi
	je	LBB22_6
	callq	__ZdaPv
LBB22_6:
	movq	%rbx, %rdi
	callq	__Unwind_Resume_or_Rethrow
Leh_func_end22:
	.section	__TEXT,__gcc_except_tab
	.align	2
GCC_except_table22:
Lexception22:
	.byte	255
	.byte	155
	.byte	156
	.space	1
	.byte	3
	.byte	26
Lset5 = Ltmp52-Leh_func_begin22
	.long	Lset5
Lset6 = Ltmp53-Ltmp52
	.long	Lset6
Lset7 = Ltmp54-Leh_func_begin22
	.long	Lset7
	.byte	0
Lset8 = Ltmp53-Leh_func_begin22
	.long	Lset8
Lset9 = Leh_func_end22-Ltmp53
	.long	Lset9
	.long	0
	.byte	0
	.align	2

	.section	__TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame0:
Lsection_eh_frame:
Leh_frame_common:
Lset10 = Leh_frame_common_end-Leh_frame_common_begin
	.long	Lset10
Leh_frame_common_begin:
	.long	0
	.byte	1
	.asciz	 "zPLR"
	.byte	1
	.byte	120
	.byte	16
	.byte	7
	.byte	155
	.long	___gxx_personality_v0@GOTPCREL+4
	.byte	16
	.byte	16
	.byte	12
	.byte	7
	.byte	8
	.byte	144
	.byte	1
	.align	3
Leh_frame_common_end:
	.globl	__ZNK3mpl12vertex_array3dotEi.eh
__ZNK3mpl12vertex_array3dotEi.eh:
Lset11 = Leh_frame_end1-Leh_frame_begin1
	.long	Lset11
Leh_frame_begin1:
Lset12 = Leh_frame_begin1-Leh_frame_common
	.long	Lset12
Ltmp58:
	.quad	Leh_func_begin1-Ltmp58
Lset13 = Leh_func_end1-Leh_func_begin1
	.quad	Lset13
	.byte	8
	.quad	0
	.byte	4
Lset14 = Ltmp0-Leh_func_begin1
	.long	Lset14
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset15 = Ltmp1-Ltmp0
	.long	Lset15
	.byte	13
	.byte	6
	.align	3
Leh_frame_end1:

	.globl	__ZNK3mpl12vertex_array14ungroup_vertexEi.eh
__ZNK3mpl12vertex_array14ungroup_vertexEi.eh:
Lset16 = Leh_frame_end2-Leh_frame_begin2
	.long	Lset16
Leh_frame_begin2:
Lset17 = Leh_frame_begin2-Leh_frame_common
	.long	Lset17
Ltmp59:
	.quad	Leh_func_begin2-Ltmp59
Lset18 = Leh_func_end2-Leh_func_begin2
	.quad	Lset18
	.byte	8
	.quad	0
	.byte	4
Lset19 = Ltmp2-Leh_func_begin2
	.long	Lset19
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset20 = Ltmp3-Ltmp2
	.long	Lset20
	.byte	13
	.byte	6
	.align	3
Leh_frame_end2:

	.globl	__ZN3mpl12vertex_array10set_vertexEiPKf.eh
__ZN3mpl12vertex_array10set_vertexEiPKf.eh:
Lset21 = Leh_frame_end3-Leh_frame_begin3
	.long	Lset21
Leh_frame_begin3:
Lset22 = Leh_frame_begin3-Leh_frame_common
	.long	Lset22
Ltmp60:
	.quad	Leh_func_begin3-Ltmp60
Lset23 = Leh_func_end3-Leh_func_begin3
	.quad	Lset23
	.byte	8
	.quad	0
	.byte	4
Lset24 = Ltmp4-Leh_func_begin3
	.long	Lset24
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset25 = Ltmp5-Ltmp4
	.long	Lset25
	.byte	13
	.byte	6
	.align	3
Leh_frame_end3:

	.globl	__ZN3mpl12vertex_arrayC1Ev.eh
__ZN3mpl12vertex_arrayC1Ev.eh:
Lset26 = Leh_frame_end4-Leh_frame_begin4
	.long	Lset26
Leh_frame_begin4:
Lset27 = Leh_frame_begin4-Leh_frame_common
	.long	Lset27
Ltmp61:
	.quad	Leh_func_begin4-Ltmp61
Lset28 = Leh_func_end4-Leh_func_begin4
	.quad	Lset28
	.byte	8
	.quad	0
	.byte	4
Lset29 = Ltmp6-Leh_func_begin4
	.long	Lset29
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset30 = Ltmp7-Ltmp6
	.long	Lset30
	.byte	13
	.byte	6
	.align	3
Leh_frame_end4:

	.globl	__ZN3mpl12vertex_arrayC2Ev.eh
__ZN3mpl12vertex_arrayC2Ev.eh:
Lset31 = Leh_frame_end5-Leh_frame_begin5
	.long	Lset31
Leh_frame_begin5:
Lset32 = Leh_frame_begin5-Leh_frame_common
	.long	Lset32
Ltmp62:
	.quad	Leh_func_begin5-Ltmp62
Lset33 = Leh_func_end5-Leh_func_begin5
	.quad	Lset33
	.byte	8
	.quad	0
	.byte	4
Lset34 = Ltmp8-Leh_func_begin5
	.long	Lset34
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset35 = Ltmp9-Ltmp8
	.long	Lset35
	.byte	13
	.byte	6
	.align	3
Leh_frame_end5:

	.globl	__ZNK3mpl12vertex_array3sumEi.eh
__ZNK3mpl12vertex_array3sumEi.eh:
Lset36 = Leh_frame_end6-Leh_frame_begin6
	.long	Lset36
Leh_frame_begin6:
Lset37 = Leh_frame_begin6-Leh_frame_common
	.long	Lset37
Ltmp63:
	.quad	Leh_func_begin6-Ltmp63
Lset38 = Leh_func_end6-Leh_func_begin6
	.quad	Lset38
	.byte	8
	.quad	0
	.byte	4
Lset39 = Ltmp10-Leh_func_begin6
	.long	Lset39
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset40 = Ltmp11-Ltmp10
	.long	Lset40
	.byte	13
	.byte	6
	.align	3
Leh_frame_end6:

	.globl	__ZN3mpl12vertex_arraypLERKNS_6float4E.eh
__ZN3mpl12vertex_arraypLERKNS_6float4E.eh:
Lset41 = Leh_frame_end7-Leh_frame_begin7
	.long	Lset41
Leh_frame_begin7:
Lset42 = Leh_frame_begin7-Leh_frame_common
	.long	Lset42
Ltmp64:
	.quad	Leh_func_begin7-Ltmp64
Lset43 = Leh_func_end7-Leh_func_begin7
	.quad	Lset43
	.byte	8
	.quad	0
	.byte	4
Lset44 = Ltmp12-Leh_func_begin7
	.long	Lset44
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset45 = Ltmp13-Ltmp12
	.long	Lset45
	.byte	13
	.byte	6
	.align	3
Leh_frame_end7:

	.globl	__ZN3mpl12vertex_arraypLERKS0_.eh
__ZN3mpl12vertex_arraypLERKS0_.eh:
Lset46 = Leh_frame_end8-Leh_frame_begin8
	.long	Lset46
Leh_frame_begin8:
Lset47 = Leh_frame_begin8-Leh_frame_common
	.long	Lset47
Ltmp65:
	.quad	Leh_func_begin8-Ltmp65
Lset48 = Leh_func_end8-Leh_func_begin8
	.quad	Lset48
	.byte	8
	.quad	0
	.byte	4
Lset49 = Ltmp14-Leh_func_begin8
	.long	Lset49
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset50 = Ltmp15-Ltmp14
	.long	Lset50
	.byte	13
	.byte	6
	.align	3
Leh_frame_end8:

	.globl	__ZN3mpl12vertex_array9normalizeEv.eh
__ZN3mpl12vertex_array9normalizeEv.eh:
Lset51 = Leh_frame_end9-Leh_frame_begin9
	.long	Lset51
Leh_frame_begin9:
Lset52 = Leh_frame_begin9-Leh_frame_common
	.long	Lset52
Ltmp66:
	.quad	Leh_func_begin9-Ltmp66
Lset53 = Leh_func_end9-Leh_func_begin9
	.quad	Lset53
	.byte	8
	.quad	0
	.byte	4
Lset54 = Ltmp16-Leh_func_begin9
	.long	Lset54
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset55 = Ltmp17-Ltmp16
	.long	Lset55
	.byte	13
	.byte	6
	.align	3
Leh_frame_end9:

	.globl	__ZN3mpl12vertex_arraymLEf.eh
__ZN3mpl12vertex_arraymLEf.eh:
Lset56 = Leh_frame_end10-Leh_frame_begin10
	.long	Lset56
Leh_frame_begin10:
Lset57 = Leh_frame_begin10-Leh_frame_common
	.long	Lset57
Ltmp67:
	.quad	Leh_func_begin10-Ltmp67
Lset58 = Leh_func_end10-Leh_func_begin10
	.quad	Lset58
	.byte	8
	.quad	0
	.byte	4
Lset59 = Ltmp18-Leh_func_begin10
	.long	Lset59
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset60 = Ltmp19-Ltmp18
	.long	Lset60
	.byte	13
	.byte	6
	.align	3
Leh_frame_end10:

	.globl	__ZN3mpl12vertex_arraymLERKS0_.eh
__ZN3mpl12vertex_arraymLERKS0_.eh:
Lset61 = Leh_frame_end11-Leh_frame_begin11
	.long	Lset61
Leh_frame_begin11:
Lset62 = Leh_frame_begin11-Leh_frame_common
	.long	Lset62
Ltmp68:
	.quad	Leh_func_begin11-Ltmp68
Lset63 = Leh_func_end11-Leh_func_begin11
	.quad	Lset63
	.byte	8
	.quad	0
	.byte	4
Lset64 = Ltmp20-Leh_func_begin11
	.long	Lset64
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset65 = Ltmp21-Ltmp20
	.long	Lset65
	.byte	13
	.byte	6
	.align	3
Leh_frame_end11:

	.globl	__ZN3mpl12vertex_arraymIERKNS_6float4E.eh
__ZN3mpl12vertex_arraymIERKNS_6float4E.eh:
Lset66 = Leh_frame_end12-Leh_frame_begin12
	.long	Lset66
Leh_frame_begin12:
Lset67 = Leh_frame_begin12-Leh_frame_common
	.long	Lset67
Ltmp69:
	.quad	Leh_func_begin12-Ltmp69
Lset68 = Leh_func_end12-Leh_func_begin12
	.quad	Lset68
	.byte	8
	.quad	0
	.byte	4
Lset69 = Ltmp22-Leh_func_begin12
	.long	Lset69
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset70 = Ltmp23-Ltmp22
	.long	Lset70
	.byte	13
	.byte	6
	.align	3
Leh_frame_end12:

	.globl	__ZN3mpl12vertex_arraymIERKS0_.eh
__ZN3mpl12vertex_arraymIERKS0_.eh:
Lset71 = Leh_frame_end13-Leh_frame_begin13
	.long	Lset71
Leh_frame_begin13:
Lset72 = Leh_frame_begin13-Leh_frame_common
	.long	Lset72
Ltmp70:
	.quad	Leh_func_begin13-Ltmp70
Lset73 = Leh_func_end13-Leh_func_begin13
	.quad	Lset73
	.byte	8
	.quad	0
	.byte	4
Lset74 = Ltmp24-Leh_func_begin13
	.long	Lset74
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset75 = Ltmp25-Ltmp24
	.long	Lset75
	.byte	13
	.byte	6
	.align	3
Leh_frame_end13:

	.globl	__ZNK3mpl12vertex_array6lengthEi.eh
__ZNK3mpl12vertex_array6lengthEi.eh:
Lset76 = Leh_frame_end14-Leh_frame_begin14
	.long	Lset76
Leh_frame_begin14:
Lset77 = Leh_frame_begin14-Leh_frame_common
	.long	Lset77
Ltmp71:
	.quad	Leh_func_begin14-Ltmp71
Lset78 = Leh_func_end14-Leh_func_begin14
	.quad	Lset78
	.byte	8
	.quad	0
	.byte	4
Lset79 = Ltmp26-Leh_func_begin14
	.long	Lset79
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset80 = Ltmp27-Ltmp26
	.long	Lset80
	.byte	13
	.byte	6
	.align	3
Leh_frame_end14:

	.globl	__ZN3mpl12vertex_array3dotERKS0_RKNS_6float4ERNS_12scalar_arrayE.eh
__ZN3mpl12vertex_array3dotERKS0_RKNS_6float4ERNS_12scalar_arrayE.eh:
Lset81 = Leh_frame_end15-Leh_frame_begin15
	.long	Lset81
Leh_frame_begin15:
Lset82 = Leh_frame_begin15-Leh_frame_common
	.long	Lset82
Ltmp72:
	.quad	Leh_func_begin15-Ltmp72
Lset83 = Leh_func_end15-Leh_func_begin15
	.quad	Lset83
	.byte	8
	.quad	0
	.byte	4
Lset84 = Ltmp28-Leh_func_begin15
	.long	Lset84
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset85 = Ltmp29-Ltmp28
	.long	Lset85
	.byte	13
	.byte	6
	.byte	4
Lset86 = Ltmp30-Ltmp29
	.long	Lset86
	.byte	131
	.byte	6
	.byte	140
	.byte	5
	.byte	142
	.byte	4
	.byte	143
	.byte	3
	.align	3
Leh_frame_end15:

	.globl	__ZN3mpl12vertex_array3dotERKS0_S2_RNS_12scalar_arrayE.eh
__ZN3mpl12vertex_array3dotERKS0_S2_RNS_12scalar_arrayE.eh:
Lset87 = Leh_frame_end16-Leh_frame_begin16
	.long	Lset87
Leh_frame_begin16:
Lset88 = Leh_frame_begin16-Leh_frame_common
	.long	Lset88
Ltmp73:
	.quad	Leh_func_begin16-Ltmp73
Lset89 = Leh_func_end16-Leh_func_begin16
	.quad	Lset89
	.byte	8
	.quad	0
	.byte	4
Lset90 = Ltmp31-Leh_func_begin16
	.long	Lset90
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset91 = Ltmp32-Ltmp31
	.long	Lset91
	.byte	13
	.byte	6
	.byte	4
Lset92 = Ltmp33-Ltmp32
	.long	Lset92
	.byte	131
	.byte	7
	.byte	140
	.byte	6
	.byte	141
	.byte	5
	.byte	142
	.byte	4
	.byte	143
	.byte	3
	.align	3
Leh_frame_end16:

	.globl	__ZN3mpl12vertex_array6createEiNS0_8format_tE.eh
__ZN3mpl12vertex_array6createEiNS0_8format_tE.eh:
Lset93 = Leh_frame_end17-Leh_frame_begin17
	.long	Lset93
Leh_frame_begin17:
Lset94 = Leh_frame_begin17-Leh_frame_common
	.long	Lset94
Ltmp74:
	.quad	Leh_func_begin17-Ltmp74
Lset95 = Leh_func_end17-Leh_func_begin17
	.quad	Lset95
	.byte	8
	.quad	0
	.byte	4
Lset96 = Ltmp34-Leh_func_begin17
	.long	Lset96
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset97 = Ltmp35-Ltmp34
	.long	Lset97
	.byte	13
	.byte	6
	.byte	4
Lset98 = Ltmp36-Ltmp35
	.long	Lset98
	.byte	131
	.byte	4
	.byte	142
	.byte	3
	.align	3
Leh_frame_end17:

	.globl	__ZN3mpl12vertex_array5crossERKS0_S2_RS0_.eh
__ZN3mpl12vertex_array5crossERKS0_S2_RS0_.eh:
Lset99 = Leh_frame_end18-Leh_frame_begin18
	.long	Lset99
Leh_frame_begin18:
Lset100 = Leh_frame_begin18-Leh_frame_common
	.long	Lset100
Ltmp75:
	.quad	Leh_func_begin18-Ltmp75
Lset101 = Leh_func_end18-Leh_func_begin18
	.quad	Lset101
	.byte	8
	.quad	0
	.byte	4
Lset102 = Ltmp37-Leh_func_begin18
	.long	Lset102
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset103 = Ltmp38-Ltmp37
	.long	Lset103
	.byte	13
	.byte	6
	.byte	4
Lset104 = Ltmp39-Ltmp38
	.long	Lset104
	.byte	131
	.byte	6
	.byte	140
	.byte	5
	.byte	142
	.byte	4
	.byte	143
	.byte	3
	.align	3
Leh_frame_end18:

	.globl	__ZN3mpl12vertex_array4copyEPKfiNS0_8format_tE.eh
__ZN3mpl12vertex_array4copyEPKfiNS0_8format_tE.eh:
Lset105 = Leh_frame_end19-Leh_frame_begin19
	.long	Lset105
Leh_frame_begin19:
Lset106 = Leh_frame_begin19-Leh_frame_common
	.long	Lset106
Ltmp76:
	.quad	Leh_func_begin19-Ltmp76
Lset107 = Leh_func_end19-Leh_func_begin19
	.quad	Lset107
	.byte	8
	.quad	0
	.byte	4
Lset108 = Ltmp40-Leh_func_begin19
	.long	Lset108
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset109 = Ltmp41-Ltmp40
	.long	Lset109
	.byte	13
	.byte	6
	.byte	4
Lset110 = Ltmp42-Ltmp41
	.long	Lset110
	.byte	131
	.byte	6
	.byte	140
	.byte	5
	.byte	142
	.byte	4
	.byte	143
	.byte	3
	.align	3
Leh_frame_end19:

	.globl	__ZN3mpl12vertex_array4copyERKS0_.eh
__ZN3mpl12vertex_array4copyERKS0_.eh:
Lset111 = Leh_frame_end20-Leh_frame_begin20
	.long	Lset111
Leh_frame_begin20:
Lset112 = Leh_frame_begin20-Leh_frame_common
	.long	Lset112
Ltmp77:
	.quad	Leh_func_begin20-Ltmp77
Lset113 = Leh_func_end20-Leh_func_begin20
	.quad	Lset113
	.byte	8
	.quad	0
	.byte	4
Lset114 = Ltmp43-Leh_func_begin20
	.long	Lset114
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset115 = Ltmp44-Ltmp43
	.long	Lset115
	.byte	13
	.byte	6
	.byte	4
Lset116 = Ltmp45-Ltmp44
	.long	Lset116
	.byte	131
	.byte	5
	.byte	142
	.byte	4
	.byte	143
	.byte	3
	.align	3
Leh_frame_end20:

	.globl	__ZN3mpl12vertex_arrayC1EiNS0_8format_tE.eh
__ZN3mpl12vertex_arrayC1EiNS0_8format_tE.eh:
Lset117 = Leh_frame_end21-Leh_frame_begin21
	.long	Lset117
Leh_frame_begin21:
Lset118 = Leh_frame_begin21-Leh_frame_common
	.long	Lset118
Ltmp78:
	.quad	Leh_func_begin21-Ltmp78
Lset119 = Leh_func_end21-Leh_func_begin21
	.quad	Lset119
	.byte	8
Ltmp79:
	.quad	Lexception21-Ltmp79
	.byte	4
Lset120 = Ltmp49-Leh_func_begin21
	.long	Lset120
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset121 = Ltmp50-Ltmp49
	.long	Lset121
	.byte	13
	.byte	6
	.byte	4
Lset122 = Ltmp51-Ltmp50
	.long	Lset122
	.byte	131
	.byte	4
	.byte	142
	.byte	3
	.align	3
Leh_frame_end21:

	.globl	__ZN3mpl12vertex_arrayC2EiNS0_8format_tE.eh
__ZN3mpl12vertex_arrayC2EiNS0_8format_tE.eh:
Lset123 = Leh_frame_end22-Leh_frame_begin22
	.long	Lset123
Leh_frame_begin22:
Lset124 = Leh_frame_begin22-Leh_frame_common
	.long	Lset124
Ltmp80:
	.quad	Leh_func_begin22-Ltmp80
Lset125 = Leh_func_end22-Leh_func_begin22
	.quad	Lset125
	.byte	8
Ltmp81:
	.quad	Lexception22-Ltmp81
	.byte	4
Lset126 = Ltmp55-Leh_func_begin22
	.long	Lset126
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset127 = Ltmp56-Ltmp55
	.long	Lset127
	.byte	13
	.byte	6
	.byte	4
Lset128 = Ltmp57-Ltmp56
	.long	Lset128
	.byte	131
	.byte	4
	.byte	142
	.byte	3
	.align	3
Leh_frame_end22:


.subsections_via_symbols
