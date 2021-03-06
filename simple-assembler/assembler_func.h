#ifndef ASSEMBLER_STRUCTS
#define ASSEMBLER_STRUCTS
typedef struct instruction_i
{
    int8_t opcode;
    
    int8_t *args;
    int n_args;

} instruction;

typedef struct label_i
{
    char *name;
    int cur_pos;

} label;

#endif

int file_write_b(char *o_file_name, int8_t *buffer, size_t size, size_t count);
void *safe_realloc (void* ptr, size_t new_size);
instruction *create_new_instr(int8_t instr_opcode, int8_t *instr_args, int instr_n_args);
int free_instr(instruction **to_del);
label *create_new_label(char *label_name, int label_pos);
