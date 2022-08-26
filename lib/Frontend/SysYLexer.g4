lexer grammar SysYLexer;

// options { language = cpp; }

// type
INT :   'int';
FLOAT:  'float';
CONST:  'const';
VOID:   'void';
STRING: '"'(ESC|.)*?'"';
ESC : '\\"'|'\\\\';
// stat
IF  :   'if';
ELSE:   'else';
WHILE:  'while';
FOR:    'for';
BREAK:  'break';
CONTINUE:   'continue';
RETURN: 'return';
//
AGN :   '=';
// arithmetic
ADD :   '+';
SUB :   '-';
MUL :   '*';
DIV :   '/';
MOD :   '%';
// relation
EQL :   '==';
NEQ :   '!=';
LES :   '<';
GRT :   '>';
LEQ :   '<=';
GEQ :   '>=';
// logic
NOT :   '!';
AND :   '&&';
OR  :   '||';

Ident:  [a-zA-Z_][a-zA-Z0-9_]*;

DEC:    [1-9][0-9]*;
OCT :   '0'[0-7]*;
HEX :   ('0x'|'0X')[0-9a-fA-F]+;
fragment DIGIT:  [0-9]+;
fragment HEXDIGIT: [0-9a-fA-F]+;
fragment DOT  :  '.';
fragment EXP  :  [Ee];
fragment EXPHEX: [pP];
fragment HEXPRE: ('0x'|'0X');

fragment EXPON: EXP (ADD|SUB)? DIGIT;

DECFLOAT: FRAC EXPON?
        | DIGIT EXPON
        ;

HEXFLOAT: HEXPRE HEXFRAC BINEXPON
	    | HEXPRE HEXDIGIT BINEXPON
	    ;

fragment HEXFRAC : HEXDIGIT? DOT HEXDIGIT
	    | HEXDIGIT DOT
	    ;

fragment FRAC: DIGIT? DOT DIGIT
    | DIGIT DOT
    ;

fragment BINEXPON: EXPHEX (ADD|SUB)? DIGIT;

COMMA:  ',';
SEMI:   ';';
LBRA:   '(';
RBRA:   ')';
LSBRA:  '[';
RSBRA:  ']';
LCBRA:  '{';
RCBRA:  '}';

WS  :   [ \r\n\t]+ ->  skip;
SL_COMMENT  :   '//' ~ [\r\n]* -> skip;
COMMENT     :   '/*' .*? '*/' -> skip;
