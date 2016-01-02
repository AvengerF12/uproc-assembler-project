%{

# include <stdio.h>
# include "assembler.tab.h" // the output of bison
void yyerror (char *); // we need to forward declare these functions ,
int yyparse (void); // don ’t worry about them

%}

%%

";" return SEMICOLON;
":" return COLON;
"," return COMMA;

\/\/[[:blank:][:alnum:]]+\n {printf("got comment token \n");}

"MOV" return MOV;
"JMP" return JMP;
"END" return END;

\[[0-9A-Fa-f]+\] return ADDR;
[0-9A-Fa-f]+ return INT;
[A-Za-z]+ return STRING;

[\t\n]+ ; // do nothing on whitespace

%%

void yyerror (char * str) { printf (" ERROR : Could not parse !\n" );}

int yywrap (void) { }

int main (void) {
    yyparse (); // yyparse is defined for us by flex
}
