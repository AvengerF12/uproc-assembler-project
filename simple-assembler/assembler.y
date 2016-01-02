%{

#include <stdio.h>
#include <stdlib.h>
#include "hexConverter.h"

int yylex();
void yyerror (char *);

%}

// the different types of tokens we want to return
%union {
int int_token;
char *str_token;
}

// list the different tokens of each type
%token <int_token> INT O_BRACKET C_BRACKET SEMICOLON COLON COMMA
%token <str_token> STRING END MOV JMP ADD

// indicate which of the below nodes is the root of the parse tree
%start root_node

%%

root_node : instructions;

instructions : instructions instruction_node
             | instruction_node;

instruction_node : add_atoa_instruction
                | mov_itoa_instruction
                | jmp_instruction
                | label_instruction
                | end_instruction ;


label_instruction : STRING COLON
{printf ("Parsed a label! Name: %s\n", $1);};

add_atoa_instruction : ADD O_BRACKET INT C_BRACKET COMMA O_BRACKET INT C_BRACKET SEMICOLON
{printf ("Parsed a add_atoa! 70: %X %X \n", $3, $7);};

mov_itoa_instruction : MOV O_BRACKET INT C_BRACKET COMMA INT SEMICOLON
{printf ("Parsed a mov_itoa! A0: %X %X\n", $3, $6);};

jmp_instruction : JMP STRING SEMICOLON
{printf ("Parsed a jmp! 50: %s\n", $2);};

end_instruction : END SEMICOLON
{printf ("Parsed an end node !\n"); exit (0);};

%%

void yyerror (char * str) { printf (" ERROR : Could not parse !\n" );}

int yywrap () { }

int main () {
    yyparse (); // yyparse is defined for us by flex
}

