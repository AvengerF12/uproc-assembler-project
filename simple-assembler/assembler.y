%{

#include <stdio.h>
#include <stdlib.h>
extern int yylex();
extern void yyerror (char *);

%}

// the different types of tokens we want to return
%union {
int int_token;
char *str_token;
}

// list the different tokens of each type
%token <int_token> INT ADDR SEMICOLON COLON COMMA
%token <str_token> STRING END MOV JMP

// indicate which of the below nodes is the root of the parse tree
%start root_node

%%

root_node : instructions;

instructions : instructions instruction_node
             | instruction_node;

instruction_node : mov_atoa_instruction
                | mov_itoa_instruction
                | jmp_instruction
                | label_instruction
                | end_node ;

label_instruction : STRING COLON
{printf ("Parsed a label!\n");};

mov_atoa_instruction : MOV ADDR COMMA ADDR SEMICOLON
{printf ("Parsed a mov_atoa!\n");};

mov_itoa_instruction : MOV ADDR COMMA INT SEMICOLON
{printf ("Parsed a mov_itoa!\n");};

jmp_instruction : JMP STRING SEMICOLON
{printf ("Parsed a jmp!\n");};

end_node : END SEMICOLON
{printf ("Parsed an end node !\n"); exit (0);};
