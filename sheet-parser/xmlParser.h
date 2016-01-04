#ifndef OPCODE_STRUCT
#define OPCODE_STRUCT
typedef struct
{
    int **xml_table_content;

    int n_cell_per_row;

    int addr_row_count;
} opcode_table;
#endif

opcode_table *streamDoc(char *xml_buffer);
int free_xml_table(opcode_table **);
