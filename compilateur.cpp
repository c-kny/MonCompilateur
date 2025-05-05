//  A compiler from a very simple Pascal-like structured language LL(k)
//  to 64-bit 80x86 Assembly langage
//  Copyright (C) 2019 Pierre Jourlin
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Build with "make compilateur"


#include <string>
#include <iostream>
#include <cstdlib>
#include <set>
#include <map>
#include <FlexLexer.h>
#include "tokeniser.h"
#include <cstring>

using namespace std;

enum OPREL {EQU, DIFF, INF, SUP, INFE, SUPE, WTFR};
enum OPADD {ADD, SUB, OR, WTFA};
enum OPMUL {MUL, DIV, MOD, AND ,WTFM};
enum TYPES {INTEGER, BOOLEAN};

TOKEN current;				// Current token


FlexLexer* lexer = new yyFlexLexer; // This is the flex tokeniser
// tokens can be read using lexer->yylex()
// lexer->yylex() returns the type of the lexicon entry (see enum TOKEN in tokeniser.h)
// and lexer->YYText() returns the lexicon entry as a string

map<string, enum TYPES> DeclaredVariables; // Stocker les variables déclarées et leurs types
//long long pour 64 bits
unsigned long long TagNumber=0;

//controle variable declareé ou pas
bool IsDeclared(const char *id){
	return DeclaredVariables.find(id)!=DeclaredVariables.end();
}


void Error(string s){
	cerr << "Ligne n°"<<lexer->lineno()<<", lu : '"<<lexer->YYText()<<"'("<<current<<"), mais ";
	cerr<< s << endl;
	exit(-1);
}

// Program := [DeclarationPart] StatementPart
// DeclarationPart := "[" Letter {"," Letter} "]"
// StatementPart := Statement {";" Statement} "."
// Statement := AssignementStatement
// AssignementStatement := Letter "=" Expression

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
// SimpleExpression := Term {AdditiveOperator Term}
// Term := Factor {MultiplicativeOperator Factor}
// Factor := Number | Letter | "(" Expression ")"| "!" Factor
// Number := Digit{Digit}

// AdditiveOperator := "+" | "-" | "||"
// MultiplicativeOperator := "*" | "/" | "%" | "&&"
// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="  
// Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
// Letter := "a"|...|"z"

enum TYPES Expression(void);			// Called by Term() and calls Term()
void Statement(void);
void StatementPart(void);

enum TYPES Identifier(void){
	cout << "\tpush "<<lexer->YYText()<<endl;
	current=(TOKEN) lexer->yylex();
	return INTEGER;
}

enum TYPES Number(void){
	cout <<"\tpush $"<<atoi(lexer->YYText())<<endl;
	current=(TOKEN) lexer->yylex();
	return INTEGER;
}

enum TYPES Factor(void){
	enum TYPES type;
	if(current==LPARENT){
		current=(TOKEN) lexer->yylex();
		type=Expression();
		if(current!=RPARENT)
			Error("')' était attendu");		// ")" expected
		else
			current=(TOKEN) lexer->yylex();
			return type;
	}
	else 
		if (current==NUMBER)
			type=Number();
	    else
				if(current==ID)
					type=Identifier();
				else
					Error("'(' ou chiffre ou lettre attendue");
	return type;
}

// MultiplicativeOperator := "*" | "/" | "%" | "&&"
OPMUL MultiplicativeOperator(void){
	OPMUL opmul;
	if(strcmp(lexer->YYText(),"*")==0)
		opmul=MUL;
	else if(strcmp(lexer->YYText(),"/")==0)
		opmul=DIV;
	else if(strcmp(lexer->YYText(),"%")==0)
		opmul=MOD;
	else if(strcmp(lexer->YYText(),"&&")==0)
		opmul=AND;
	else opmul=WTFM;
	current=(TOKEN) lexer->yylex();
	return opmul;
}

// Term := Factor {MultiplicativeOperator Factor}
enum TYPES Term(void){
	TYPES type1, type2;
	OPMUL mulop;
	type1=Factor();
	while(current==MULOP){
		mulop=MultiplicativeOperator();		// Save operator in local variable
		type2=Factor();
		if(type1!=type2)
			Error("types incompatibles dans l'expression");
		cout << "\tpop %rbx"<<endl;	// get first operand
		cout << "\tpop %rax"<<endl;	// get second operand
		switch(mulop){
			//S'il y a un opérateur && (AND), le type doit être BOOLEAN
			case AND:
			    if(type2!=BOOLEAN)
					Error("le type doit être BOOLEAN dans l'expression");
				cout << "\tmulq	%rbx"<<endl;	// a * b -> %rdx:%rax
				cout << "\tpush %rax\t# AND"<<endl;	// store result
				break;
			//Si *, /, % est présent, le type doit être INTEGER
			case MUL:
				if(type2!=INTEGER)
					Error("le type doit être INTEGER dans l'expression");
				cout << "\tmulq	%rbx"<<endl;	// a * b -> %rdx:%rax
				cout << "\tpush %rax\t# MUL"<<endl;	// store result
				break;
			case DIV:
				if(type2!=INTEGER)
					Error("le type doit être INTEGER dans l'expression");
				cout << "\tmovq $0, %rdx"<<endl; 	// Higher part of numerator  
				cout << "\tdiv %rbx"<<endl;			// quotient goes to %rax
				cout << "\tpush %rax\t# DIV"<<endl;		// store result
				break;
			case MOD:
				if(type2!=INTEGER)
					Error("le type doit être INTEGER dans l'expression");
				cout << "\tmovq $0, %rdx"<<endl; 	// Higher part of numerator  
				cout << "\tdiv %rbx"<<endl;			// remainder goes to %rdx
				cout << "\tpush %rdx\t# MOD"<<endl;		// store result
				break;
			default:
				Error("opérateur multiplicatif attendu");
		}
	}
	return type1;
}

// AdditiveOperator := "+" | "-" | "||"
OPADD AdditiveOperator(void){
	OPADD opadd;
	if(strcmp(lexer->YYText(),"+")==0)
		opadd=ADD;
	else if(strcmp(lexer->YYText(),"-")==0)
		opadd=SUB;
	else if(strcmp(lexer->YYText(),"||")==0)
		opadd=OR;
	else opadd=WTFA;
	current=(TOKEN) lexer->yylex();
	return opadd;
}

// SimpleExpression := Term {AdditiveOperator Term}
enum TYPES SimpleExpression(void){
	TYPES type1, type2;
	OPADD adop;
	type1=Term();
	while(current==ADDOP){
		adop=AdditiveOperator();		// Save operator in local variable
		type2=Term();
		if(type1!=type2)
			Error("types incompatibles dans l'expression");
		cout << "\tpop %rbx"<<endl;	// get first operand
		cout << "\tpop %rax"<<endl;	// get second operand
		switch(adop){
			///S'il y a un opérateur OR, le type doit être BOOLEAN
			case OR:
				if(type2!=BOOLEAN)
					Error("le type doit être BOOLEAN dans l'expression");
				cout << "\taddq	%rbx, %rax\t# OR"<<endl;// operand1 OR operand2
				break;
			///S'il y a un opérateur ADD,SUB le type doit être INTEGER
			case ADD:
				if(type2!=INTEGER)
					Error("le type doit être INTEGER dans l'expression");
				cout << "\taddq	%rbx, %rax\t# ADD"<<endl;	// add both operands
				break;			
			case SUB:
				if(type2!=INTEGER)
					Error("le type doit être INTEGER dans l'expression");
				cout << "\tsubq	%rbx, %rax\t# SUB"<<endl;	// substract both operands
				break;
			default:
				Error("opérateur additif inconnu");
		}
		cout << "\tpush %rax"<<endl;			// store result
	}
	return type1;
}

enum TYPES Type(void){
	if(current!=KEYWORD)
		Error("Type attendu");
	if(strcmp(lexer->YYText(), "INTEGER")==0){
		current=(TOKEN) lexer->yylex();
		return INTEGER;
	}
	else if(strcmp(lexer->YYText(), "BOOLEAN")==0){
		current=(TOKEN) lexer->yylex();
		return BOOLEAN;
	}
	else
		Error("Type Inconnue");
}


// VarDeclaration := Ident {"," Ident} ":" Type
void VarDeclaration(void){
	set<string> idents;		//Créer un ensemble de strings non répétitives(avec set)
	enum TYPES type;
	if(current!=ID)
		Error("Un identificateur était attendu");
	idents.insert(lexer->YYText());		// ajoute le nom lu par le lexer à l'ensemble
	current=(TOKEN) lexer->yylex();

	while(current==COMMA){
		current=(TOKEN) lexer->yylex();
		if(current!=ID)
			Error("Un identificateur était attendu");
		idents.insert(lexer->YYText());		// ajoute le nom lu par le lexer à l'ensemble
		current=(TOKEN) lexer->yylex();
	}
	if(current!=COLON)
		Error("Caractère ':' attendu");
	current=(TOKEN) lexer->yylex();

	type=Type();  	//La fonction Type() est appelée, cette valeur est attribuée à la variable de type
	for(set<string>::iterator it=idents.begin(); it!=idents.end(); ++it){
		cout<<*it<<":\t.quad 0"<<endl;		// Réserve de la mémoire pour chaque identifiant et crée une variable de 64 bits initialisée à 0.
		DeclaredVariables[*it] = type;		// enregistre le type de chaque identifiant
	}
}

// VarDeclarationPart := "VAR" VarDeclaration {";" VarDeclaration} "."
void VarDeclarationPart(void){
	current=(TOKEN) lexer->yylex();
	VarDeclaration();

	while(current==SEMICOLON){
		current=(TOKEN) lexer->yylex();
		VarDeclaration();
	}
	if(current!=DOT)
		Error("caractère '.' attendu");
	current=(TOKEN) lexer->yylex();
}

// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="  
OPREL RelationalOperator(void){
	OPREL oprel;
	if(strcmp(lexer->YYText(),"==")==0)
		oprel=EQU;
	else if(strcmp(lexer->YYText(),"!=")==0)
		oprel=DIFF;
	else if(strcmp(lexer->YYText(),"<")==0)
		oprel=INF;
	else if(strcmp(lexer->YYText(),">")==0)
		oprel=SUP;
	else if(strcmp(lexer->YYText(),"<=")==0)
		oprel=INFE;
	else if(strcmp(lexer->YYText(),">=")==0)
		oprel=SUPE;
	else oprel=WTFR;
	current=(TOKEN) lexer->yylex();
	return oprel;
}

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
enum TYPES Expression(void){
	TYPES type1, type2;
	OPREL oprel;
	type1=SimpleExpression();
	if(current==RELOP){
		oprel=RelationalOperator();
		type2=SimpleExpression();

		if(type2!=type1)
			Error("types incompatibles pour la comparaison");
		cout << "\tpop %rax"<<endl;
		cout << "\tpop %rbx"<<endl;
		cout << "\tcmpq %rax, %rbx"<<endl;
		switch(oprel){
			case EQU:
				cout << "\tje Vrai"<<++TagNumber<<"\t# If equal"<<endl;
				break;
			case DIFF:
				cout << "\tjne Vrai"<<++TagNumber<<"\t# If different"<<endl;
				break;
			case SUPE:
				cout << "\tjae Vrai"<<++TagNumber<<"\t# If above or equal"<<endl;
				break;
			case INFE:
				cout << "\tjbe Vrai"<<++TagNumber<<"\t# If below or equal"<<endl;
				break;
			case INF:
				cout << "\tjb Vrai"<<++TagNumber<<"\t# If below"<<endl;
				break;
			case SUP:
				cout << "\tja Vrai"<<++TagNumber<<"\t# If above"<<endl;
				break;
			default:
				Error("Opérateur de comparaison inconnu");
		}
		cout << "\tpush $0\t\t# False"<<endl;
		cout << "\tjmp Suite"<<TagNumber<<endl;
		cout << "Vrai"<<TagNumber<<":\tpush $0xFFFFFFFFFFFFFFFF\t\t# True"<<endl;	
		cout << "Suite"<<TagNumber<<":"<<endl;
		return BOOLEAN;
	}
	return type1;
}

void Statement(void); //Pour les appels avat déclaration

// AssignementStatement := Identifier ":=" Expression
void AssignementStatement(void){
	enum TYPES type1, type2;
	string variable;
	if(current!=ID)
		Error("Identificateur attendu");
	if(!IsDeclared(lexer->YYText())){
		cerr << "Erreur : Variable '"<<lexer->YYText()<<"' non déclarée"<<endl;
		exit(-1);
	}
	variable=lexer->YYText();
	//// On consulte dans la map le type associé à la variable donnée
	type1=DeclaredVariables[variable]; 		 // type1 == INTEGER
	current=(TOKEN) lexer->yylex();
	if(current!=ASSIGN)
		Error("caractères ':=' attendus");
	current=(TOKEN) lexer->yylex();
	type2=Expression();
	if(type2!=type1){
		Error("types incompatibles dans l'affectation");
	}
	else
		cout << "\tpop "<<variable<<endl;
}

//DisplayAssignement := "DISPLAY" Expression
void DisplayAssignement(void){
	enum TYPES type;
	unsigned long long tag=++TagNumber;
	current=(TOKEN) lexer->yylex();
	type=Expression();
	//l'instruction DISPLAY <expression> qui permet d'afficher le résultat d'une expression si son type est INTEGER sur la sortie standard
	if(type==INTEGER){
		cout<<"\tpop %rdx\t#The value to be displayed"<<endl;			//%rdx doit contenir la valeur à afficher
		cout<<"\tmovq $FormatString1, %rsi\t#\"%llu\\n\""<<endl;
	}
	else if(type==BOOLEAN){
		cout << "\tpop %rdx\t# Zero : False, non-zero : true"<<endl;
		cout << "\tcmpq $0, %rdx"<<endl;
		cout << "\tje False"<<tag<<endl;
		cout << "\tmovq $TrueString, %rsi\t# \"TRUE\\n\""<<endl;
		cout << "\tjmp Next"<<tag<<endl;
		cout << "False"<<tag<<":"<<endl;
		cout << "\tmovq $FalseString, %rsi\t# \"FALSE\\n\""<<endl;
		cout << "Next"<<tag<<":"<<endl;
	}
	else
		Error("DISPLAY ne fonctionne que pour les nombres entiers");
<<<<<<< HEAD
	// Pour mac :
	//cout<<"\tpop %rdx\t#The value to be displayed"<<endl;			//%rdx doit contenir la valeur à afficher
	//cout<<"\tmovq $FormatString1, %rdi\t#\"%llu\\n\""<<endl;
	//cout<<"\tmovl $0, %eax\t"<<endl;
	//cout<<"\tpush %rbp\t#save the value in %rbp (modified by printf)"<<endl;
	//cout<<"\tcall	printf@PLT\t"<<endl;
	//cout<<"\tpop %rbp\t#restore %rbp value"<<endl;

	cout<<"\tmovq $FormatString1, %rdi\t#\"%llu\\n\""<<endl;
	cout<<"\tmovq (%rsp), %rsi\t#\"%llu\\n\""<<endl;
	cout<<"\tmovl $0, %eax\t"<<endl;
	cout<<"\tcall	printf@PLT\t"<<endl;
=======
	cout << "\tmovl	$1, %edi"<<endl;
	cout << "\tmovl	$0, %eax"<<endl;
	cout << "\tcall	__printf_chk@PLT"<<endl;
>>>>>>> 0544a30 (+TP6)
}

//IfStatement := "IF" Expression "THEN" Statement [ "ELSE" Statement ]
void IfStatement(void){
	unsigned long long tag = TagNumber++;
	enum TYPES type_controle;
	// Read new token after IF
	current=(TOKEN) lexer->yylex();

	//1.Expression
	type_controle=Expression();
		if(type_controle!=BOOLEAN)
			Error("La condition IF doit être de type BOOLEAN");
	cout<<"\tpop %rax\t# Get the result of expression"<<endl;
	cout<<"\tcmpq $0, %rax"<<endl;
	cout<<"\tje Else"<<tag<<"\t# if FALSE, jump to Else"<<tag<<endl;
	//THEN control
	if(current!=KEYWORD||strcmp(lexer->YYText(),"THEN")!=0)
		Error("mot-clé 'THEN' attendu");
	current=(TOKEN) lexer->yylex();
	Statement();
	cout<<"\tjmp Next"<<tag<<"\t# Do not execute the else statement"<<endl;
	cout<<"Else"<<tag<<":"<<endl; // Might be the same effective adress than Next:
	if(current==KEYWORD&&strcmp(lexer->YYText(),"ELSE")==0){
		current=(TOKEN) lexer->yylex();
		Statement();
	}
	cout<<"Next"<<tag<<":"<<endl;
}
//WhileStatement := "WHILE" Expression "DO" Statement
void WhileStatement(void){
	unsigned long long tag=TagNumber++;
	enum TYPES type_controle;
	cout<<"While"<<tag<<":"<<endl;
	current=(TOKEN) lexer->yylex();
	//1.Expression
	type_controle=Expression();
		if(type_controle!=BOOLEAN)
			Error("La condition WHILE doit être de type BOOLEAN");
	cout << "\tpop %rax\t# Get the result of expression" << endl;
    cout << "\tcmpq $0, %rax\t# Compare with FALSE" << endl;
    cout << "\tje EndWhile" << tag << "\t# if FALSE, exit the loop" << endl;
	if(current!=KEYWORD||strcmp(lexer->YYText(), "DO")!=0)
		Error("mot-clé DO attendu");
	current=(TOKEN) lexer->yylex();
	Statement();
	cout<<"\tjmp While"<<tag<<endl;
	cout<<"EndWhile"<<tag<<":"<<endl;
}

//ForStatement := "FOR" AssignementStatement "To" Expression "DO" Statement
void ForStatement(void){
	unsigned long long tag=TagNumber++;
	enum TYPES type_contol;
	// to hold the variable name
	string variable;
	cout<<"For"<<tag<<":"<<endl;

	current=(TOKEN) lexer->yylex();
	//1. Attribuer la valeur initiale (AssignementStatement)
	if(current!=ID)		//for doit être connître identifiante
		Error("Identificateur attendu pour la boucle FOR");

	// Save variable name
	variable=lexer->YYText();
	if(DeclaredVariables[variable]!=INTEGER)
		Error("La variable de boucle FOR doit être de type INTEGER");
	AssignementStatement();		//Cette fonctionne déja faire controle de type.

	//2.Controle TO ou DOWNTO
	bool ascending = true;
	if(current==KEYWORD&&strcmp(lexer->YYText(),"TO")==0)
		ascending=true;
	else if(current==KEYWORD&&strcmp(lexer->YYText(),"DOWNTO")==0)
		ascending=false;
	else
		Error("'TO' ou 'DOWNTO' attendu après assignation");

	current=(TOKEN) lexer->yylex();
	// 3. Lire la valeur de fin (Expression) et le type controlée.
	type_contol=Expression();
	if(type_contol!=INTEGER)
		Error("L'expression doit être de type INTEGER");
	cout<<"TestFor"<<tag<<":"<<endl;
	cout<<"\tpop %rbx\t# End value of the FOR"<<endl;
    cout<<"\tpop %rax\t# Current value of "<<variable<<endl;
    cout<<"\tcmpq %rbx, %rax"<<endl;

    if(ascending)
        cout<< "\tjg EndFor"<<tag <<"\t# if greater, exit"<<endl;
    else
        cout<<"\tjl EndFor"<< tag<<"\t# if less, exit"<<endl;

	//4.Controle DO
	if (!(current==KEYWORD&&strcmp(lexer->YYText(),"DO")==0))
	Error("'DO' attendu après 'TO' ou 'DOWNTO'");

	//Lire token aprés de DO
	current=(TOKEN) lexer->yylex();

	Statement();		// Étant donné que la partie suivant DO est une « Statement », son type est vérifié en elle-même.
	cout<<"\tpush "<<variable<<endl;
	cout<<"\tpush $1"<<endl;
	cout<<"\tpop %rbx"<<endl;
	cout<<"\tpop %rax"<<endl;
	if(ascending)
		cout<<"\taddq %rbx, %rax"<<endl;
	else
		cout<<"\tsubq %rbx, %rax"<<endl;
	cout<<"\tpush %rax" << endl;
	cout<<"\tpop "<<variable<<endl;

	cout<<"\tjmp TestFor"<<tag<<endl;
	cout<<"EndFor"<<tag<<":"<< endl;
}

//BlockStatement := "BEGIN" Statement { ";" Statement } "END"
void BlockStatement(void){
	current=(TOKEN) lexer->yylex();
	Statement();
	while(current==SEMICOLON){
		current=(TOKEN) lexer->yylex();	// Skip the ";"
		Statement();
	};
	if(current!=KEYWORD||strcmp(lexer->YYText(), "END")!=0)
		Error("mot-clé END attendu");
	current=(TOKEN) lexer->yylex();
}

// Statement := AssignementStatement | IfStatement | WhileStatement | ForStatement | BlockStatement | DisplayAssignement
void Statement(void){
	if(current==ID)
		AssignementStatement();
	else if(current==KEYWORD){
		if(strcmp(lexer->YYText(),"IF")==0)
			IfStatement();
		else if(strcmp(lexer->YYText(),"WHILE")==0)
			WhileStatement();
		else if(strcmp(lexer->YYText(),"FOR")==0)
			ForStatement();
		else if(strcmp(lexer->YYText(),"BEGIN")==0)
			BlockStatement();
		else if(strcmp(lexer->YYText(),"DISPLAY")==0)
			DisplayAssignement();
		else
			Error("mot clé inconnu");
	}
	else
		Error("insturction attendue");
}

// StatementPart := Statement {";" Statement} "."
void StatementPart(void){
	cout << "\t.align 8"<<endl;	// Alignement on addresses that are a multiple of 8 (64 bits = 8 bytes)
	cout << "\t.text\t\t# The following lines contain the program"<<endl;
	cout << "\t.globl main\t# The main function must be visible from outside"<<endl;
	cout << "main:\t\t\t# The main function body :"<<endl;
	cout << "\tmovq %rsp, %rbp\t# Save the position of the stack's top"<<endl;
	Statement();
	while(current==SEMICOLON){
		current=(TOKEN) lexer->yylex();
		Statement();
	}
	if(current!=DOT)
		Error("caractère '.' attendu");
	current=(TOKEN) lexer->yylex();
}

// Program := [DeclarationPart] StatementPart
void Program(void){
	if(current==RBRACKET)
		VarDeclarationPart();
	StatementPart();	
}

int main(void){	// First version : Source code on standard input and assembly code on standard output
	// Header for gcc assembler / linker
	cout << "\t\t\t# This code was produced by the CERI Compiler"<<endl;
	cout << ".data"<<endl;
	cout << "FormatString1:\t.string \"%llu\\n\"\t# used by printf to display 64-bit unsigned integers"<<endl;
	cout << "TrueString:\t.string \"TRUE\\n\"\t# used by printf to display the boolean value TRUE"<<endl; 
	cout << "FalseString:\t.string \"FALSE\\n\"\t# used by printf to display the boolean value FALSE"<<endl;
	// Let's proceed to the analysis and code production
	current=(TOKEN) lexer->yylex();
	Program();
	// Trailer for the gcc assembler / linker
	cout << "\tmovq %rbp, %rsp\t\t# Restore the position of the stack's top"<<endl;
	cout << "\tret\t\t\t# Return from main function"<<endl;
	if(current!=FEOF){
		cerr <<"Caractères en trop à la fin du programme : ["<<current<<"]";
		Error("."); // unexpected characters at the end of program
	}
}