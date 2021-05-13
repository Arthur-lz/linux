	.file	"z3test.c"
	.text
	.globl	fix_set_diag
	.type	fix_set_diag, @function
fix_set_diag:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movq	$0, -8(%rbp)
	jmp	.L2
.L3:
	movq	-8(%rbp), %rax
	salq	$6, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movq	-8(%rbp), %rax
	movl	-28(%rbp), %edx
	movl	%edx, (%rcx,%rax,4)
	addq	$1, -8(%rbp)
.L2:
	cmpq	$15, -8(%rbp)
	jle	.L3
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	fix_set_diag, .-fix_set_diag
	.section	.rodata
.LC0:
	.string	"%d"
	.text
	.globl	print_array
	.type	print_array, @function
print_array:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	$0, -8(%rbp)
	jmp	.L5
.L8:
	movq	$0, -16(%rbp)
	jmp	.L6
.L7:
	movq	-8(%rbp), %rax
	salq	$6, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rax, %rdx
	movq	-16(%rbp), %rax
	movl	(%rdx,%rax,4), %eax
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	addq	$1, -16(%rbp)
.L6:
	cmpq	$15, -16(%rbp)
	jle	.L7
	movl	$10, %edi
	call	putchar
	addq	$1, -8(%rbp)
.L5:
	cmpq	$15, -8(%rbp)
	jle	.L8
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	print_array, .-print_array
	.globl	fix_set_diag_opt
	.type	fix_set_diag_opt, @function
fix_set_diag_opt:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-24(%rbp), %rax
	addq	$1024, %rax
	addq	$64, %rax
	movq	%rax, -16(%rbp)
.L10:
	movq	-8(%rbp), %rax
	movl	-28(%rbp), %edx
	movl	%edx, (%rax)
	addq	$68, -8(%rbp)
	movq	-8(%rbp), %rax
	cmpq	-16(%rbp), %rax
	jne	.L10
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	fix_set_diag_opt, .-fix_set_diag_opt
	.section	.rodata
.LC1:
	.string	"A[i]=&A[i][0]"
	.text
	.globl	it_eq
	.type	it_eq, @function
it_eq:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$40, %rsp
	.cfi_offset 3, -24
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movl	%edx, -40(%rbp)
	movl	-36(%rbp), %eax
	movslq	%eax, %rdx
	subq	$1, %rdx
	movq	%rdx, -24(%rbp)
	movslq	%eax, %rdx
	movq	%rdx, %rcx
	movl	$0, %ebx
	movl	-40(%rbp), %edx
	movslq	%edx, %rcx
	movslq	%eax, %rdx
	imulq	%rcx, %rdx
	leaq	0(,%rdx,4), %rcx
	movq	-48(%rbp), %rdx
	addq	%rdx, %rcx
	movl	-40(%rbp), %edx
	movslq	%edx, %rdx
	cltq
	imulq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	cmpq	%rax, %rcx
	jne	.L11
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
.L11:
	addq	$40, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	it_eq, .-it_eq
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$1080, %rsp
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rsp, %r10
	movq	%r10, %r14
	movl	$5, -52(%rbp)
	movl	-52(%rbp), %r10d
	movl	-52(%rbp), %r11d
	movslq	%r10d, %r15
	subq	$1, %r15
	movq	%r15, -64(%rbp)
	movslq	%r10d, %r15
	movq	%r15, %r12
	movl	$0, %r13d
	movslq	%r11d, %r12
	subq	$1, %r12
	movq	%r12, -72(%rbp)
	movslq	%r10d, %r12
	movq	%r12, %r8
	movl	$0, %r9d
	movslq	%r11d, %r12
	movq	%r12, %rsi
	movl	$0, %edi
	movq	%r9, %r13
	imulq	%rsi, %r13
	movq	%rdi, %r12
	imulq	%r8, %r12
	addq	%r13, %r12
	movq	%r8, %rax
	mulq	%rsi
	movq	%rax, %rsi
	movq	%rdx, %rdi
	leaq	(%r12,%rdi), %r8
	movq	%r8, %rdi
	movslq	%r10d, %rsi
	movq	%rsi, %rcx
	movl	$0, %ebx
	movslq	%r11d, %rsi
	movq	%rsi, -1120(%rbp)
	movq	$0, -1112(%rbp)
	movq	%rbx, %rdi
	movq	-1120(%rbp), %rax
	movq	-1112(%rbp), %rdx
	imulq	%rax, %rdi
	movq	%rdx, %rsi
	imulq	%rcx, %rsi
	addq	%rdi, %rsi
	mulq	%rcx
	leaq	(%rsi,%rdx), %rcx
	movq	%rcx, %rdx
	movslq	%r10d, %rdx
	movslq	%r11d, %rax
	imulq	%rdx, %rax
	salq	$2, %rax
	leaq	3(%rax), %rdx
	movl	$16, %eax
	subq	$1, %rax
	addq	%rdx, %rax
	movl	$16, %ebx
	movl	$0, %edx
	divq	%rbx
	imulq	$16, %rax, %rax
	subq	%rax, %rsp
	movq	%rsp, %rax
	addq	$3, %rax
	shrq	$2, %rax
	salq	$2, %rax
	movq	%rax, -80(%rbp)
	movq	-80(%rbp), %rax
	movl	$3, %edx
	movq	%rax, %rsi
	movl	$5, %edi
	call	it_eq
	movl	$0, %eax
	nop
	movq	%r14, %rsp
	cmpl	$1, %eax
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
