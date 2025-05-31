# CERIcompiler

A simple compiler.
From : Pascal-like imperative LL(k) langage
To : 64 bit 80x86 assembly langage (AT&T)

**Download the repository :**

> git clone git@framagit.org:jourlin/cericompiler.git

**Build the compiler and test it :**

> make test

**Have a look at the output :**

> gedit test.s

**Debug the executable :**

> ddd ./test

**Commit the new version :**

> git commit -a -m "What's new..."

**Send to your framagit :**

> git push -u origin master

**Get from your framagit :**

> git pull -u origin master

**This version Can handle :**

// Program := [DeclarationPart] StatementPart
// DeclarationPart := "[" Identifier {"," Identifier} "]"
// StatementPart := Statement {";" Statement} "."
// Statement := AssignementStatement
// AssignementStatement := Identifier ":=" Expression

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
// SimpleExpression := Term {AdditiveOperator Term}
// Term := Factor {MultiplicativeOperator Factor}
// Factor := Number | Letter | "(" Expression ")"| "!" Factor
// Number := Digit{Digit}
// Identifier := Letter {(Letter|Digit)}

// AdditiveOperator := "+" | "-" | "||"
// MultiplicativeOperator := "*" | "/" | "%" | "&&"
// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="  
// Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
// Letter := "a"|...|"z"


**MonCompilateur - Pascal Compiler Project**

**Propriétés**
// Mon compilateur prend en charge les instructions, mots-clés et types suivants :
// DISPLAY, IF, THEN, ELSE, BEGIN, END, FOR, TO|DOWNTO, WHILE, DO, VAR, INTEGER, BOOLEAN, CHAR, DOUBLE, STEP, CASE, OF

**Travail Personelle**
// Les parties suivantes du code représentent le travail que j'ai réalisé personnellement dans ce projet.
// Il s'agit soit de fonctionnalités entièrement développées par moi-même, soit d'améliorations et d'adaptations importantes apportées au code fourni par l'enseignant.
// Pour chaque point, j'explique brièvement la modification ou l'ajout, ainsi que la motivation ou le problème résolu.


**Ajout du support de l'opérateur NOT (`!`)** 
//Fichier : 'compilateur.cpp'
//Fonction : `Factor()`
//Description :  Ajout du traitement de l'opérateur NOT pour les expressions booléennes, avec vérification du type et génération correcte du code assembleur.

**Amélioration des déclarations de variables**
//Fichier : 'compilateur.cpp'
//Fonction : `VarDeclarationPart()`
//Description :
Dans ma version, j’ai ajouté une vérification dès le début pour être sûr que la section de déclaration commence bien par le mot-clé VAR.
Ça n’existait pas dans le code de base. Grâce à ce contrôle, si on oublie d’écrire VAR ou qu’on fait une faute de frappe, le compilateur affiche directement une erreur claire au lieu de continuer avec des problèmes plus difficiles à comprendre plus loin dans le code

**Ajout de la gestion du type DOUBLE dans les affectations**
// Fichier : 'compilateur.cpp'
// Fonction : AssignementStatement()
// Description :
Dans cette fonction, j’ai ajouté tout le nécessaire pour que les variables de type DOUBLE soient correctement prises en charge lors de l’affectation.
Concrètement, j’ai intégré le code qui permet de :
    -charger la valeur DOUBLE depuis le sommet de la pile (fldl),
    -libérer la place utilisée sur la pile (addq $8, %rsp),
puis stocker la valeur dans la variable cible avec fstpl variable.
Cette gestion n’existait pas dans le code initial. Grâce à ça, il est désormais possible de déclarer et d’affecter des variables de type DOUBLE dans les programmes Pascal que mon compilateur traite.

**Ajout d’un arrêt explicite en cas de type inconnu**  
// Fichier : 'compilateur.cpp'  
// Fonction : `Type()`  
// Description :  
J’ai ajouté un appel à `exit(-1)` après l’affichage du message d’erreur pour les types inconnus.  
Ce choix permet d’arrêter immédiatement le programme si jamais un type non reconnu est rencontré lors de la déclaration,  
ce qui évite tout comportement imprévisible ou des erreurs silencieuses plus loin dans le code.

**Adaptation et clarification de la gestion du IF**
//Fichier : 'compilateur.cpp'
//Fonction : `IfStatement()`
//Description : J'ai adapté la gestion du label (`TagNumber`) pour garantir l'unicité des étiquettes en utilisant l'incrémentation avant affectation 
(`++TagNumber`).  J'ai également choisi de stocker le type retourné par `Expression()` dans une variable locale avant de le vérifier, afin de rendre le code plus lisible et plus facilement déboguable.
**Example de code Pascal compilé**
VAR 
      a,b : INTEGER;
      c: BOOLEAN.
a:=1;
b:=6;
WHILE a<12 DO
BEGIN
        IF a%2==0 THEN
                DISPLAY a
        ELSE
                DISPLAY a*2;
        a:=a+1;
        c:=(a<=b);
        DISPLAY c
END.

-Output :
2
TRUE
2
TRUE
6
TRUE
4
TRUE
10
TRUE
6
FALSE
14
FALSE
8
FALSE
18
FALSE
10
FALSE
22
FALSE

**Implémentation complète de la boucle FOR avec support de TO, DOWNTO et STEP**
// Fichier : 'compilateur.cpp'
// Fonction : `ForStatement()`
// Description : 
> Ma fonction prend en charge le mot-clé `TO` pour les boucles ascendantes et `DOWNTO` pour les boucles descendantes.
> J’ai ajouté la prise en charge du mot-clé optionnel STEP, qui permet une incrémentation ou une décrémentation personnalisée. De plus, le cas où STEP = 0 est contrôlé et génère une erreur.
> La gestion des erreurs pour chaque étape (mots-clés manquants, STEP nul, type incorrect, etc.).

**Fait quoi cette fonction ?**  
Cette fonction analyse et traduit une boucle FOR en code assembleur. Voici son déroulement :
1. La variable `step_cpt` est initialisée à 1 pour gérer la valeur par défaut du pas d’incrémentation ou de décrémentation dans la boucle FOR. Si l’utilisateur n’utilise pas le mot-clé `STEP`, la boucle avancera (ou reculera) de 1 à chaque itération, conformément au comportement standard du Pascal.
2. J’effectue cette vérification (contrôle d’identifiant et de type) directement dans la fonction, car la variable utilisée dans une boucle FOR doit être un identifiant entier selon la syntaxe du Pascal. Elle vérifie donc que la variable de boucle est bien de type INTEGER avant de poursuivre le reste de l’analyse.
3. Elle lit et affecte la valeur initiale à cette variable.
4. Le sens de la boucle est déterminé à l’aide des mots-clés `TO` (croissant) ou `DOWNTO` (décroissant), et stocké dans le booléen `ascending` afin d’exécuter correctement la progression ou la régression.
5. Elle prend en compte le mot-clé optionnel `STEP` pour une incrémentation ou décrémentation personnalisée, et génère une erreur si `STEP` est égal à zéro.
6. Elle vérifie la présence du mot-clé `DO` après la définition de la borne et du pas, conformément à la syntaxe Pascal.
7. Elle génère des étiquettes (`TestFor`, `LoopFor`, `EndFor`) pour contrôler le flux d’exécution de la boucle en assembleur.
8. À chaque itération :
    - Elle compare la valeur courante de la variable de boucle à la borne finale :
        - Si la condition de poursuite n’est plus respectée (`jg` pour TO, `jl` pour DOWNTO), la boucle est terminée.
        - Sinon, le corps de la boucle (l’instruction suivant `DO`) est exécuté.
    - Ensuite, la variable de boucle est modifiée selon la valeur de `STEP` (ajout ou soustraction).
    - Un saut (`jmp LoopFor...`) permet de répéter l’itération.
9. L’étiquette `EndFor...` marque la fin de la boucle et permet la reprise normale du flux d’exécution après la boucle FOR.
10. À chaque étape, la fonction effectue des contrôles d’erreur pour garantir la conformité de la syntaxe et la sécurité d’exécution.
**Examples des codes Pascal compilé**
1.**Test FOR-**
VAR
    i : INTEGER.

FOR i:=1 TO 5 DO
BEGIN
    DISPLAY i;
    DISPLAY '\n'
END.

-Output :
1

2

3

4

5

2.**Test FOR-STEP**

VAR
    i : INTEGER.
i:=1;
FOR i:=1 TO 10 STEP 3 DO
BEGIN
    DISPLAY i;
    DISPLAY '\n'
END.

-Output :
1

4

7

10

**Implémentation complète de la structure CASE**
// Fichier : 'compilateur.cpp'
// Fonctions principales : `CaseStatement()`, `CaseListElement(tag, type_case)`, `CaseLabelList(tag, label_id, type_case)`
// Description :
J’ai entièrement développé la gestion de la structure CASE dans le compilateur, ce qui n’était pas présent dans le code de base.  
Cette implémentation permet de traiter des instructions conditionnelles multiples, à la manière du switch-case du Pascal.

### Conception générale de la structure CASE

La structure CASE, inspirée du switch-case dans d’autres langages, permet de traiter des instructions conditionnelles multiples de manière concise et efficace.  
Contrairement aux autres structures de contrôle (comme IF, WHILE ou FOR), chaque branche d’un CASE doit être clairement identifiée et accessible via une étiquette unique dans le code généré.  
Cela implique une gestion particulière :
- Chaque instruction CASE et chaque branche doivent être distinguées (via des paramètres comme `tag` et `label_id`).
- Le type de l’expression contrôlée (par exemple, INTEGER ou CHAR) doit être vérifié à chaque branche pour garantir la conformité du code.
- La génération des étiquettes et des sauts conditionnels doit être systématique pour éviter toute ambiguïté, surtout si plusieurs instructions CASE sont imbriquées dans un programme.

Pour toutes ces raisons, la conception des fonctions liées à la structure CASE diffère sensiblement des autres structures du compilateur :  
elles prennent en entrée des paramètres spécifiques permettant d’assurer un code assembleur correct, modulaire et généralisable.

Pour permettre la gestion de l’instruction CASE dans le langage, plusieurs modifications et ajouts ont été nécessaires dans différents fichiers du projet :

1. **Extension de l’analyseur lexical (`tokeniser.l`) :**
    - Ajout des mots-clés `CASE` et `OF` dans la liste des mots-clés reconnus.

2. **Ajout dans l’analyseur syntaxique (`compilateur.cpp`) :**
   - Insertion de l’appel à la fonction `CaseStatement()` dans la fonction `Statement()`.

3. **Implémentation des fonctions dédiées :**
   - `CaseStatement()`, `CaseListElement()`, `CaseLabelList()` (voir explication détaillée ci-dessous).


**Fait quoi ces fonctions ?**

1. **`CaseLabelList(tag, label_id, type_case)`**
   - Cette fonction compare la valeur de l’expression contrôlée par le CASE à chacune des constantes (entiers ou caractères) spécifiées pour une branche donnée.
   - Pour chaque constante (séparée par des virgules), elle génère le code assembleur approprié : `cmpq` pour les types INTEGER et `cmpb` pour les types CHAR.
   - Si la comparaison est vraie, elle effectue un saut conditionnel (`je Case<tag>_<label_id>`) vers l’étiquette correspondant à la branche.
   - Elle vérifie que seules les constantes compatibles (INTEGER ou CHAR) sont utilisées, et signale une erreur en cas de type non supporté.
   - Cette conception rend possible la gestion de plusieurs valeurs pour une même branche, tout en assurant la robustesse et la lisibilité du code généré.

2. **`CaseListElement(tag, type_case)`**
   - Cette fonction gère chaque élément (branche) de la liste du CASE.
   - Pour chaque branche, elle appelle `CaseLabelList()` pour traiter les constantes associées, puis vérifie la présence du symbole `:`.
   - Elle génère une étiquette unique pour chaque branche (`Case<tag>_<label_id>`) afin d’y associer le code à exécuter si la valeur contrôlée correspond.
   - Elle gère aussi la possibilité d’une branche vide, permettant une alternative de type "default" (semblable à ELSE dans d’autres langages).

3. **`CaseStatement()`**
   - Cette fonction analyse la syntaxe complète de l’instruction CASE, selon la grammaire du Pascal.
   - Elle évalue l’expression de contrôle et vérifie qu’elle est de type INTEGER ou CHAR.
   - Elle orchestre l’appel aux fonctions `CaseListElement()` pour gérer toutes les branches du CASE.
   - Elle génère des étiquettes uniques pour chaque branche, ainsi qu’une étiquette finale (`FinCase<tag>`) pour marquer la sortie de la structure CASE.
   - Elle gère également toutes les erreurs potentielles de syntaxe ou de type lors de l’analyse et de la génération de code.

**Examples des codes Pascal compilé**
1.**Test CASE-INT**
VAR
    i: INTEGER.
BEGIN
    i := 2;
    CASE i OF
        1: DISPLAY 'A';
        2,3: DISPLAY 'B';
        4: DISPLAY 'C';
    END.
END. 

-Output : B

1.**Test CASE-CHAR**
VAR 
    y : CHAR.
BEGIN
    y := 'C'; 
    CASE y OF
        'A': DISPLAY 'A'; 
        'B': DISPLAY 'C';
        'C': DISPLAY 'B'; 
        'D': DISPLAY 'D';
    END.
END.

-Output : B



**Exemple supplémentaire de code Pascal compilé**
**Test WHILE**
VAR		
    a,b: INTEGER;
	c1,c2: CHAR;
	num, denum, frac: DOUBLE.
c1:='f';
c2:='a';
num:=1.0;
denum:=1.0;
frac:=num/denum;
a:=1;
WHILE frac>0.1 DO
BEGIN
	DISPLAY c1;
	DISPLAY '=';
	DISPLAY frac;
	DISPLAY '\n';
	DISPLAY c2;
	DISPLAY '=';
	DISPLAY a;
	DISPLAY '\n';
	denum:=denum+1.0;
	frac:=num/denum;
	a:=a+1;
	DISPLAY (a>3);
	DISPLAY '\n'
END.