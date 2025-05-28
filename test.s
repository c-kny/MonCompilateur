			# This code was produced by the CERI Compiler
.data
FormatString1:	.string "%llu\n"	# used by printf to display 64-bit unsigned integers
FormatString2:	.string "%lf\n"	# used by printf to display 64-bit floating point numbers
FormatString3:	.string "%c\n"	# used by printf to display a 8-bit single character
TrueString:	.string "TRUE\n"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE\n"	# used by printf to display the boolean value FALSE
i:	.quad 0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $1
	pop i
	push $5
TestFor1:
	pop %rbx		# limite (TO/DOWNTO) dans %rbx
LoopFor1:
	mov i, %rax	# valeur courante i
	cmpq %rbx, %rax
	jg EndFor1	# if greater, exit
	push i
	pop %rsi	# The value to be displayed
	movq $FormatString1, %rdi	# "%llu\n"
	movl	$0, %eax
	call	printf@PLT
	movq $0, %rax
	movb $'\n', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère '\n'
	pop %rsi			# get character in the 8 lowest bits of %si
	movq $FormatString3, %rdi	# "%c\n"
	movl	$0, %eax
	call	printf@PLT
	mov i, %rax
	addq $1, %rax
	mov %rax, i
	jmp LoopFor1
EndFor1:
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
