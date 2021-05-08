	.file	"hello.c"
	.text
	.globl	cmpx
	.type	cmpx, @function
cmpx:
.LFB11:
	.cfi_startproc
	cmpq	%rsi, %rdi
	setl	%al
	ret
	.cfi_endproc
.LFE11:
	.size	cmpx, .-cmpx
	.globl	cmpy
	.type	cmpy, @function
cmpy:
.LFB12:
	.cfi_startproc
	cmpq	%rsi, %rdi
	setl	%al
	movzbl	%al, %eax
	ret
	.cfi_endproc
.LFE12:
	.size	cmpy, .-cmpy
	.globl	cmpz
	.type	cmpz, @function
cmpz:
.LFB13:
	.cfi_startproc
	cmpq	%rsi, %rdi
	sete	%al
	movzbl	%al, %eax
	ret
	.cfi_endproc
.LFE13:
	.size	cmpz, .-cmpz
	.globl	scale
	.type	scale, @function
scale:
.LFB14:
	.cfi_startproc
	leaq	(%rdi,%rsi,4), %rax
	leaq	(%rdx,%rdx,2), %rdx
	leaq	(%rax,%rdx,4), %rax
	ret
	.cfi_endproc
.LFE14:
	.size	scale, .-scale
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"stardard C"
.LC1:
	.string	"hello, world!"
	.text
	.globl	main
	.type	main, @function
main:
.LFB15:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$.LC0, %edi
	call	puts
	movl	$.LC1, %edi
	call	puts
	movl	$0, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE15:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
