%{

#include <stdio.h>
#include <stdlib.h>
extern int yylex();
extern void yyerror (char *);

%}

%error-verbose;

// the different types of tokens we want to return
%union {
int int_token;
char *str_token;
}

// list the different tokens of each type
%token <int_token> INT ADDR SEMICOLON COMMA
%token <str_token> START END MOV JMP

// indicate which of the below nodes is the root of the parse tree
%start root_node

%%

root_node : instructions;

instructions : /*empty*/
             | instructions instruction_node
             | instruction_node;

start_node : START SEMICOLON
{printf (" parsed a start node!\n");};

instruction_node : mov_atoa_instruction
                | mov_itoa_instruction
                | jmp_instruction
                | start_node
                | end_node ;

mov_atoa_instruction : MOV ADDR COMMA ADDR SEMICOLON
{printf (" parsed a mov_atoa!\n");};

mov_itoa_instruction : MOV ADDR COMMA INT SEMICOLON
{printf (" parsed a mov_itoa!\n");};

jmp_instruction : JMP INT SEMICOLON
{printf (" parsed a jmp!\n");};


end_node : END SEMICOLON
{printf (" parsed an end node !\n"); exit (0);};
