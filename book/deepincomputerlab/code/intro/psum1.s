	.file	"psum1.c"
	.text
	.globl	psum1
	.type	psum1, @function
psum1:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	movq	-32(%rbp), %rdx
	movl	%eax, (%rdx)
	movq	-32(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -12(%rbp)
	movq	$1, -8(%rbp)
	jmp	.L2
.L3:
	movq	-8(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax		// a++
	addq	%rdx, %rax
	movss	(%rax), %xmm0		// %xmm0 = a[i]
	movss	-12(%rbp), %xmm1	// %xmm1 = v
	addss	%xmm1, %xmm0		// %xmm0 = a[i] + v
	movss	%xmm0, -12(%rbp)	// v = %xmm0
	movq	-8(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-32(%rbp), %rax		// 
	addq	%rax, %rdx		// p++
	movl	-12(%rbp), %eax		// 
	movl	%eax, (%rdx)		// p[i+1]= v
	addq	$1, -8(%rbp)		// i++
.L2:
	movq	-8(%rbp), %rax
	cmpq	-40(%rbp), %rax
	jl	.L3
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	psum1, .-psum1
	.globl	psum2
	.type	psum2, @function
psum2:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)		// &a[0]
	movq	%rsi, -32(%rbp)		// &p[0]
	movq	%rdx, -40(%rbp)		// n
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	movq	-32(%rbp), %rdx
	movl	%eax, (%rdx)
	movq	-32(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -12(%rbp)		// v_last = p[0]
	movq	$1, -8(%rbp)		// i = 1
	jmp	.L5
.L6:
	movq	-8(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax		// a++
	movss	(%rax), %xmm0 		// %xmm0 = a[i]
	addss	-12(%rbp), %xmm0	// %xmm0 = a[i] + v_last
	movss	%xmm0, -16(%rbp)	// v = %xmm0
	movq	-8(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-32(%rbp), %rax		// %rax = &p[0]	
	addq	%rax, %rdx		// p++
	movl	-16(%rbp), %eax		// 
	movl	%eax, (%rdx)		//p[i+1]=v;
	movl	-16(%rbp), %eax
	movl	%eax, -12(%rbp)		// v_last = v;
	addq	$1, -8(%rbp)		// i++
.L5:
	movq	-8(%rbp), %rax
	cmpq	-40(%rbp), %rax
	jl	.L6
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	psum2, .-psum2
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
