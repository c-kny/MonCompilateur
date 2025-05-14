			# This code was produced by the CERI Compiler
.data
FormatString1:	.string "%llu\n"	# used by printf to display 64-bit unsigned integers
FormatString2:	.string "%f"	# used by printf to display 64-bit floating point numbers
FormatString3:	.string "%c"	# used by printf to display a 8-bit single character
TrueString:	.string "TRUE\n"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE\n"	# used by printf to display the boolean value FALSE
a:	.quad 0
b:	.quad 0
c1:	.byte 0
c2:	.byte 0
denum:	.double 0.0
frac:	.double 0.0
num:	.double 0.0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	movq $0, %rax
	movb $'f', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère 'f'
	pop %rax
	movb %al,c1
	movq $0, %rax
	movb $'a', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère 'a'
	pop %rax
	movb %al,c2
	subq $8,%rsp			# allocate 8 bytes on stack's top
	movl	$0, (%rsp)	# Conversion of 1 (32 bit high part)
	movl	$1072693248, 4(%rsp)	# Conversion of 1 (32 bit low part)
	fldl (%rsp)	# Charge la valeur (DOUBLE) du sommet de la pile dans %st(0)
	addq $8, %rsp	# Supprime l’emplacement du sommet de la pile (on a lu 8 octets)
	fstpl num	# Stocke la valeur de %st(0) dans la variable (et dépile %st(0))
	subq $8,%rsp			# allocate 8 bytes on stack's top
	movl	$0, (%rsp)	# Conversion of 1 (32 bit high part)
	movl	$1072693248, 4(%rsp)	# Conversion of 1 (32 bit low part)
	fldl (%rsp)	# Charge la valeur (DOUBLE) du sommet de la pile dans %st(0)
	addq $8, %rsp	# Supprime l’emplacement du sommet de la pile (on a lu 8 octets)
	fstpl denum	# Stocke la valeur de %st(0) dans la variable (et dépile %st(0))
	fldl num	# Charge le double dans %st(0)
	subq $8, %rsp		# Alloue de l’espace sur la pile
	fstpl (%rsp)		# Stocke %st(0) sur la pile générale
	fldl denum	# Charge le double dans %st(0)
	subq $8, %rsp		# Alloue de l’espace sur la pile
	fstpl (%rsp)		# Stocke %st(0) sur la pile générale
	fldl	8(%rsp)		# Charge op1 (dividende), devient %st(0)
	fldl (%rsp)		# Charge op2 (diviseur), devient %st(0), op1 devient %st(1)
	fdivp %st(0), %st(1)	# %st(1) = op1 / op2, puis %st(0) est dépilé
	fstpl 8(%rsp)		# Stocke le résultat à la place de op1
	addq $8, %rsp		# Supprime l'emplacement de op2 de la pile
	fldl (%rsp)	# Charge la valeur (DOUBLE) du sommet de la pile dans %st(0)
	addq $8, %rsp	# Supprime l’emplacement du sommet de la pile (on a lu 8 octets)
	fstpl frac	# Stocke la valeur de %st(0) dans la variable (et dépile %st(0))
	push $1
	pop a
While0:
	fldl frac	# Charge le double dans %st(0)
	subq $8, %rsp		# Alloue de l’espace sur la pile
	fstpl (%rsp)		# Stocke %st(0) sur la pile générale
	subq $8,%rsp			# allocate 8 bytes on stack's top
	movl	$2576980378, (%rsp)	# Conversion of 0.1 (32 bit high part)
	movl	$1069128089, 4(%rsp)	# Conversion of 0.1 (32 bit low part)
	fldl (%rsp)	
	fldl 8(%rsp)	# first operand -> %st(0) ; second operand -> %st(1)
	 addq $16, %rsp	# 2x pop nothing
	fcomip %st(1)		# compare op1 and op2 -> %RFLAGS and pop
	fstp %st(0)	# clean up
	ja Vrai2	# If above
	push $0		# False
	jmp Suite2
Vrai2:	push $0xFFFFFFFFFFFFFFFF		# True
Suite2:
	pop %rax	# Get the result of expression
	cmpq $0, %rax	# Compare with FALSE
	je EndWhile0	# if FALSE, exit the loop
	movq $0, %rax		# Nettoie %rax
	movb c1, %al	# Charge le caractère dans %al (1 octet)
	push %rax		# Empile en 64 bits via %rax
	pop %rsi	# Caractère à afficher dans %al
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	call	printf@PLT	
	movq $0, %rax
	movb $'=', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère '='
	pop %rsi	# Caractère à afficher dans %al
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	call	printf@PLT	
	fldl frac	# Charge le double dans %st(0)
	subq $8, %rsp		# Alloue de l’espace sur la pile
	fstpl (%rsp)		# Stocke %st(0) sur la pile générale
	fldl (%rsp)		# Charge le DOUBLE du sommet de la pile dans %st(0)
	addq $8, %rsp		# Libère l'espace de la pile (on a lu 8 octets)
	subq $8, %rsp		# Réserve de l’espace temporaire sur la pile
	fstpl (%rsp)		# Stocke %st(0) dans cet emplacement temporaire
	movsd (%rsp), %xmm0	# Copie le DOUBLE vers %xmm0 pour printf
	addq $8, %rsp		# Nettoie la pile temporaire
	movq $FormatString2, %rdi	#"%f\n"
	movl $1, %eax	#Préperation pour appele printf
	call	printf@PLT	
	movq $0, %rax
	movb $'\n', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère '\n'
	pop %rsi	# Caractère à afficher dans %al
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	call	printf@PLT	
	movq $0, %rax		# Nettoie %rax
	movb c2, %al	# Charge le caractère dans %al (1 octet)
	push %rax		# Empile en 64 bits via %rax
	pop %rsi	# Caractère à afficher dans %al
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	call	printf@PLT	
	movq $0, %rax
	movb $'=', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère '='
	pop %rsi	# Caractère à afficher dans %al
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	call	printf@PLT	
	push a
	pop %rdx	#The value to be displayed
	movq $FormatString1, %rsi	#"%llu\n"
	call	printf@PLT	
	movq $0, %rax
	movb $'\n', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère '\n'
	pop %rsi	# Caractère à afficher dans %al
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	call	printf@PLT	
	fldl denum	# Charge le double dans %st(0)
	subq $8, %rsp		# Alloue de l’espace sur la pile
	fstpl (%rsp)		# Stocke %st(0) sur la pile générale
	subq $8,%rsp			# allocate 8 bytes on stack's top
	movl	$0, (%rsp)	# Conversion of 1 (32 bit high part)
	movl	$1072693248, 4(%rsp)	# Conversion of 1 (32 bit low part)
	fldl 8(%rsp)		# Charge op1 (ancien), devient %st(0)
	fldl (%rsp)		# Charge op2 (récent), devient %st(0), op1 devient %st(1)
	faddp %st(0), %st(1)	# %st(1) = op1 + op2, puis %st(0) est dépilé
	fstpl 8(%rsp)		# Stocke le résultat à la place de op1
	addq $8, %rsp		# Supprime l'emplacement de op2 de la pile
	fldl (%rsp)	# Charge la valeur (DOUBLE) du sommet de la pile dans %st(0)
	addq $8, %rsp	# Supprime l’emplacement du sommet de la pile (on a lu 8 octets)
	fstpl denum	# Stocke la valeur de %st(0) dans la variable (et dépile %st(0))
	fldl num	# Charge le double dans %st(0)
	subq $8, %rsp		# Alloue de l’espace sur la pile
	fstpl (%rsp)		# Stocke %st(0) sur la pile générale
	fldl denum	# Charge le double dans %st(0)
	subq $8, %rsp		# Alloue de l’espace sur la pile
	fstpl (%rsp)		# Stocke %st(0) sur la pile générale
	fldl	8(%rsp)		# Charge op1 (dividende), devient %st(0)
	fldl (%rsp)		# Charge op2 (diviseur), devient %st(0), op1 devient %st(1)
	fdivp %st(0), %st(1)	# %st(1) = op1 / op2, puis %st(0) est dépilé
	fstpl 8(%rsp)		# Stocke le résultat à la place de op1
	addq $8, %rsp		# Supprime l'emplacement de op2 de la pile
	fldl (%rsp)	# Charge la valeur (DOUBLE) du sommet de la pile dans %st(0)
	addq $8, %rsp	# Supprime l’emplacement du sommet de la pile (on a lu 8 octets)
	fstpl frac	# Stocke la valeur de %st(0) dans la variable (et dépile %st(0))
	push a
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax	# DIV
	pop a
	push a
	push $3
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	ja Vrai12	# If above
	push $0		# False
	jmp Suite12
Vrai12:	push $0xFFFFFFFFFFFFFFFF		# True
Suite12:
	pop %rdx	# Zero : False, non-zero : true
	cmpq $0, %rdx
	je False11
	movq $TrueString, %rsi	# "TRUE\n"
	jmp Next11
False11:
	movq $FalseString, %rsi	# "FALSE\n"
Next11:
	call	printf@PLT	
	movq $0, %rax
	movb $'\n', %al	# %al est la partie basse de %rax (8 bits) pour stocker un caractère
	push %rax	#Empile la version 64 bits du caractère '\n'
	pop %rsi	# Caractère à afficher dans %al
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	call	printf@PLT	
	jmp While0
EndWhile0:
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
