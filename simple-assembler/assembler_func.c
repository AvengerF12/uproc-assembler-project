#include <stdio.h>
#include <stdlib.h>

#include "assembler_func.h"

int file_write_b(char *o_file_name, int8_t *buffer, size_t size, size_t count);
void *safe_realloc (void* ptr, size_t new_size);
int append_int_buffer(int8_t **old_buffer, int8_t *to_append_buffer, int *buffer_size, int append_size);
instruction *create_new_instr(int8_t instr_opcode, int8_t *instr_args, int instr_n_args);


int file_write_b(char *o_file_name, int8_t *buffer, size_t size, size_t count)
{
    FILE * o_file = NULL;

    o_file = fopen (o_file_name, "wb+");
    if (o_file == NULL) {fputs ("File error", stderr); exit(4);}

    fwrite(buffer, size, count, o_file);

    fclose(o_file);

    return 0;
}


void *safe_realloc (void* ptr, size_t new_size)
{
	void *temp_ptr = realloc(ptr, new_size);

	if(temp_ptr == NULL){
		fprintf(stderr, "Unexpected behaviour: realloc");
		free(ptr);
		exit(1);
	}

	return temp_ptr;
}


instruction *create_new_instr(int8_t instr_opcode, int8_t *instr_args, int instr_n_args)
{
    instruction *new_instr = NULL;
    new_instr = malloc(sizeof(instruction));

    new_instr->opcode = instr_opcode;
    new_instr->n_args = instr_n_args;

    new_instr->args = malloc(sizeof(int8_t)*instr_n_args);

    for(int i=0;i<instr_n_args;i++){
        (new_instr->args)[i] = instr_args[i];
    }

    return new_instr;
}


int free_instr(instruction **to_del)
{

    free((*to_del)->args);

    free(*to_del);

    return 0;
}

