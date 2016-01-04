%{

#include <stdio.h>
#include <stdlib.h>

#include "assembler_func.h"

int yylex();
void yyerror (char *);

extern FILE * yyin;

int8_t *instr_buffer = NULL;
int buffer_size = 0;
int last_jump = 0;

%}

// the different types of tokens we want to return
%union {
int8_t int_token;
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
{
    printf ("Parsed a label! Name: %s\n", $1);
    last_jump = buffer_size;
    free($1);
};

add_atoa_instruction : ADD O_BRACKET INT C_BRACKET COMMA O_BRACKET INT C_BRACKET SEMICOLON
{
    printf ("Parsed a add_atoa! 70: %X %X \n", $3, $7);     
    int8_t instr_array[3] = {0x70, $3, $7}; // ADD opcode + 1st argument + 2nd argument
    append_int_buffer(&instr_buffer, instr_array, &buffer_size, 3);
};

mov_itoa_instruction : MOV O_BRACKET INT C_BRACKET COMMA INT SEMICOLON
{
    printf ("Parsed a mov_itoa! A0: %X %X\n", $3, $6);
    int8_t instr_array[3] = {0xA0, $3, $6}; // ADD opcode + 1st argument + 2nd argument
    append_int_buffer(&instr_buffer, instr_array, &buffer_size, 3);
};

jmp_instruction : JMP STRING SEMICOLON
{
    printf ("Parsed a jmp! 50: %s\n", $2);
    int8_t instr_array[3] = {0x50, last_jump};
    append_int_buffer(&instr_buffer, instr_array, &buffer_size, 2);
    free($2);
};

end_instruction : END SEMICOLON
{
    printf ("Parsed an end node !\n");
};

%%

// C code section


void yyerror (char * str) { printf (" ERROR : Could not parse !\n" );}


int main (int argc, char *argv[]) {

    // Open a file handle to a particular file
    FILE *in_file = fopen(argv[1], "r");
    // Make sure it's valid
    if (in_file == NULL) {
        printf("I can't open file: %s", argv[1]);
        return -1;
    }
    // Set lex to read from it instead of defaulting to STDIN
    yyin = in_file;

    // Parse through the input until there is no more
    do {
        yyparse();
    } while (!feof(yyin));

    fclose(in_file);

    file_write_b(argv[2], instr_buffer, sizeof(int8_t), buffer_size);

    free(instr_buffer);

    return 0;
}

