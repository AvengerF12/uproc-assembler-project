%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler_func.h"

int yylex();
void yyerror (char *);

int create_instr_buffer(instruction **queue, int n_queue, int8_t **buffer);
int free_instr_queue(instruction ***queue, int n_queue);
int check_label_name(label **queue, int n_queue, char *label_name);
int find_n_bytes_used(instruction **queue, int cur_pos);
int free_label_queue(label ***queue, int n_queue);

extern FILE * yyin;

instruction **instr_queue = NULL;
label **label_queue = NULL;

int instr_count = 0;
int label_count = 0;

%}

// the different types of tokens we want to return
%union {
int8_t int_token;
char *str_token;
}

// list the different tokens of each type
%token <int_token> INT O_BRACKET C_BRACKET SEMICOLON COLON COMMA AX
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
                | mov_itoax_instruction
                | add_itoax_instruction
                | end_instruction ;


label_instruction : STRING COLON
{
    printf ("Parsed a label! Name: %s\n", $1);

    label_count++;
    label_queue = safe_realloc(label_queue, label_count*sizeof(label));
    int cur_pos_ram = find_n_bytes_used(instr_queue, instr_count);
    label_queue[label_count-1] = create_new_label($1, cur_pos_ram);
};

add_atoa_instruction : ADD O_BRACKET INT C_BRACKET COMMA O_BRACKET INT C_BRACKET SEMICOLON
{
    printf ("Parsed a add_atoa! 70: %X %X \n", $3, $7);     

    int8_t args[] = {$3, $7};
    instr_count++;
    instr_queue = safe_realloc(instr_queue, instr_count*sizeof(instruction));
    instr_queue[instr_count-1] = create_new_instr(0x70, args, sizeof(args)/sizeof(int8_t));
};

mov_itoa_instruction : MOV O_BRACKET INT C_BRACKET COMMA INT SEMICOLON
{
    printf ("Parsed a mov_itoa! A0: %X %X\n", $3, $6);

    int8_t args[] = {$3, $6};
    instr_count++;
    instr_queue = safe_realloc(instr_queue, instr_count*sizeof(instruction));
    instr_queue[instr_count-1] = create_new_instr(0xA0, args, sizeof(args)/sizeof(int8_t));

};

jmp_instruction : JMP STRING SEMICOLON
{
    printf ("Parsed a jmp! 50: %s\n", $2);
    int result = check_label_name(label_queue, label_count, $2);
    
    if(result != -1){
        
        int8_t args[] = {(label_queue[result])->cur_pos};
        instr_count++;
        instr_queue = safe_realloc(instr_queue, instr_count*sizeof(instruction));
        instr_queue[instr_count-1] = create_new_instr(0x50, args, sizeof(args)/sizeof(int8_t));
    }
};

mov_itoax_instruction : MOV AX COMMA INT SEMICOLON
                     {
    printf ("Parsed a mov_itoax! 20: %X \n", $4);

    int8_t args[] = {$4};
    instr_count++;
    instr_queue = safe_realloc(instr_queue, instr_count*sizeof(instruction));
    instr_queue[instr_count-1] = create_new_instr(0x20, args, sizeof(args)/sizeof(int8_t));

};

add_itoax_instruction : ADD AX COMMA INT SEMICOLON
                                           {
    printf ("Parsed a add_itoax! 60: %X \n", $4);

    int8_t args[] = {$4};
    instr_count++;
    instr_queue = safe_realloc(instr_queue, instr_count*sizeof(instruction));
    instr_queue[instr_count-1] = create_new_instr(0x60, args, sizeof(args)/sizeof(int8_t));

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


    int8_t *instr_buffer = NULL;

    int buffer_len = create_instr_buffer(instr_queue, instr_count, &instr_buffer);

    file_write_b(argv[2], instr_buffer, sizeof(int8_t), buffer_len);

    free_instr_queue(&instr_queue, instr_count);

    free_label_queue(&label_queue, label_count);

    free(instr_buffer);

    return 0;
}


int create_instr_buffer(instruction **queue, int n_queue, int8_t **buffer)
{
    int buffer_size = 0;

    for(int i=0;i<n_queue;i++){
        int instr_len = (queue[i])->n_args + 1; // +1 needed for opcode

        buffer_size += instr_len; 
        *buffer = safe_realloc(*buffer, sizeof(int8_t)*buffer_size);

        int buffer_start_pos = buffer_size - instr_len; // Position from which to start appending
        
        (*buffer)[buffer_start_pos] = (queue[i])->opcode;

        buffer_start_pos++; // Avoid overwriting opcode


        for(int k=0;k<(queue[i])->n_args;k++){
            (*buffer)[buffer_start_pos+k] = ((queue[i])->args)[k];
        }
    }

    return buffer_size;
}


int free_instr_queue(instruction ***queue, int n_queue)
{
    for(int i=0;i<n_queue;i++){
        free_instr(&((*queue)[i]));
    }

    free(*queue);

    return 0;
}


int free_label_queue(label ***queue, int n_queue)
{
    for(int i=0;i<n_queue;i++){
        free(((*queue)[i])->name);
        free((*queue)[i]);
    }

    free(*queue);

    return 0;
}


int check_label_name(label **queue, int n_queue, char *label_name)
{
    for(int i=0;i<n_queue;i++){
        int cmp_result = strcmp((queue[i])->name, label_name);

        if(cmp_result == 0){
            free(label_name);
            
            return i;
        }
    }

    return -1; // There was no label with that name
}


int find_n_bytes_used(instruction **queue, int cur_pos)
{
    int tot_amount = 0;

    for(int i=0;i<cur_pos;i++){
        tot_amount += (queue[i])->n_args + 1;
    }

    return tot_amount;
}

