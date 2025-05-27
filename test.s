			# This code was produced by the CERI Compiler
.data
FormatString1:	.string "%llu\n"	# used by printf to display 64-bit unsigned integers
FormatString2:	.string "%lf\n"	# used by printf to display 64-bit floating point numbers
FormatString3:	.string "%c\n"	# used by printf to display a 8-bit single character
TrueString:	.string "TRUE\n"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE\n"	# used by printf to display the boolean value FALSE
y:	.byte 0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	movq $0, %rax
	movb $'C', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère 'C'
	pop %rax
	movb %al,y
	push y
	pop %rax	# Valeur à comparer dans le CASE
	cmpb $'A', %al
	je Case1_1
	jmp SkipCase1_1:
Case1_1:
	movq $0, %rax
	movb $'A', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère 'A'
	pop %rsi			# get character in the 8 lowest bits of %si
	movq $FormatString3, %rdi	# "%c\n"
	movl	$0, %eax
	call	printf@PLT
	jmp FinCase1
SkipCase1_1:
	cmpb $'B', %al
	je Case1_2
	jmp SkipCase1_2:
Case1_2:
