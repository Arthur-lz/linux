	.file	"main.c"
	.globl	counter
	.data
	.align 8
	.type	counter, @object
	.size	counter, 8
counter:
	.quad	2
	.section	.rodata
.LC0:
	.string	"%ld"
	.text
	.globl	handler1
	.type	handler1, @function
handler1:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	counter(%rip), %rax
	subq	$1, %rax
	movq	%rax, counter(%rip)
	movq	%rax, %rsi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movq	stdout(%rip), %rax
	movq	%rax, %rdi
	call	fflush
	movl	$0, %edi
	call	_exit
	.cfi_endproc
.LFE2:
	.size	handler1, .-handler1
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
	subq	$16, %rsp
	movq	counter(%rip), %rax
	movq	%rax, %rsi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movq	stdout(%rip), %rax
	movq	%rax, %rdi
	call	fflush
	movl	$handler1, %esi
	movl	$64, %edi
	call	signal
	call	Fork
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jne	.L3
.L4:
	jmp	.L4
.L3:
	movl	-4(%rbp), %eax
	movl	$64, %esi
	movl	%eax, %edi
	call	kill
	movl	$0, %edx
	movl	$0, %esi
	movl	$-1, %edi
	movl	$0, %eax
	call	waitpid
	movq	counter(%rip), %rax
	addq	$1, %rax
	movq	%rax, counter(%rip)
	movq	%rax, %rsi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %edi
	call	exit
	.cfi_endproc
.LFE3:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
