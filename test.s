			# This code was produced by the CERI Compiler
.data
FormatString1:	.string "%llu\n"	# used by printf to display 64-bit unsigned integers
	.data
	.align 8
a:	.quad 0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $5
	push $6
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop a
	push a
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	movq $FormatString1, %rdi	#"%llu\n"
	movq (%rsp), %rsi	#"%llu\n"
	movl $0, %eax	
	call	printf@PLT	
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
