parser grammar SysYParser;

options { tokenVocab=SysYLexer;}

compUnit:   element* EOF;

element :   decl
        |   funcDef
        ;

decl    :   constDecl
        |   varDecl
        ;

constDecl:  CONST bType constDef (COMMA constDef)* SEMI;

bType   :   INT
	|   FLOAT
	;

constDef:   Ident (LSBRA constExp RSBRA)* AGN constInitVal
        ;

constInitVal:   constExp
        |       LCBRA (constInitVal (COMMA constInitVal)*)? RCBRA
        ;

varDecl :   bType varDef (COMMA varDef)* SEMI;

varDecl_in  : bType varDef (COMMA varDef)*;

varDef  :   Ident (LSBRA constExp RSBRA)*
        |   Ident (LSBRA constExp RSBRA)* AGN initVal
        ;

initVal :   exp 
        |   LCBRA (initVal (COMMA initVal)*)? RCBRA
        ;

funcDef :   funcType Ident LBRA funcFParams? RBRA block;

funcType:   VOID
        |   INT
	|   FLOAT
        ;

funcFParams: funcFParam (COMMA funcFParam)*;

funcFParam: bType Ident (LSBRA RSBRA (LSBRA exp RSBRA)*)?;

block   :   LCBRA blockItem* RCBRA;

blockItem:  decl 
        |   stmt
        ;

stmt_in :   lVal AGN exp
        |   exp?   
        ;                  

stmt    :   lVal AGN exp SEMI 
        |   exp? SEMI 
        |   block
        |   IF LBRA cond RBRA stmt (ELSE stmt)?
        |   FOR LBRA (stmt_in|varDecl_in) SEMI cond SEMI stmt_in RBRA stmt
        |   WHILE LBRA cond RBRA stmt
        |   BREAK SEMI 
        |   CONTINUE SEMI
        |   RETURN exp? SEMI
        ;

exp     :   addExp;
 
cond    :   lOrExp;

lVal    :   Ident (LSBRA exp RSBRA)*;

primaryExp: LBRA exp RBRA 
        |   lVal
        |   number
        ;

number  :   floatConst
	|   intConst
	;

unaryExp:   primaryExp 
        |   Ident LBRA funcRParams? RBRA
        |   unaryOp unaryExp
        ;

unaryOp :   ADD 
        |   SUB 
        |   NOT
        ;

funcRParams:(exp|STRING) (COMMA (exp|STRING))*;

mulExp  :   unaryExp 
        |   mulExp (MUL|DIV|MOD) unaryExp
        ;

addExp  :   mulExp 
        |   addExp (ADD|SUB) mulExp
        ;

relExp  :   addExp 
        |   relExp (LES|GRT|LEQ|GEQ) addExp
        ;

eqExp   :   relExp 
        |   eqExp (EQL|NEQ) relExp
        ;

lAndExp :   eqExp 
        |   lAndExp AND eqExp
        ;

lOrExp  :   lAndExp 
        |   lOrExp OR lAndExp
        ;

constExp:   addExp;

intConst:   DEC
        |   OCT
        |   HEX
        ;

floatConst: DECFLOAT
        |   HEXFLOAT
        ;