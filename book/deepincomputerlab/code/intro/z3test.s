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
	.globl	main
	.type	main, @function
main:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$1040, %rsp
	leaq	-1040(%rbp), %rsi
	movl	$0, %eax
	movl	$128, %edx
	movq	%rsi, %rdi
	movq	%rdx, %rcx
	rep stosq
	movl	$9, -4(%rbp)
	movl	-4(%rbp), %edx
	leaq	-1040(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	fix_set_diag_opt
	leaq	-1040(%rbp), %rax
	movq	%rax, %rdi
	call	print_array
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
