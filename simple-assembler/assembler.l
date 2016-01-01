%{

# include <stdio.h>
# include "assembler.tab.h" // the output of bison
void yyerror (char *); // we need to forward declare these functions ,
int yyparse (void); // don ’t worry about them

%}

%%

";" return SEMICOLON;

\/\/[[:blank:][:alnum:]]+\n {printf("got comment token \n");}

"START" {printf("got START token \n"); return START;}
"END" {printf ("got END token \n"); return END;}

"MOV" {printf ("got MOV token \n"); return MOV;}
"JMP" {printf ("got JMP token \n"); return JMP;}

\[[0-9A-Fa-f]+\] {printf ("got ADDR token \n"); return ADDR;}
[0-9A-Fa-f]+ {printf ("got INT token \n"); return INT;}

"," {printf ("got COMMA token \n"); return COMMA;}

[\t\n]+ ; // do nothing on whitespace

.+\n;

%%

void yyerror (char * str) { printf (" ERROR : Could not parse !\n" );}

int yywrap (void) { }

int main (void) {
    // we don ’t want to do anything extra , just start the parser
    yyparse (); // yyparse is defined for us by flex
}
