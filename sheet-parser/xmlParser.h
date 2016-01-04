#ifndef OPCODE_STRUCT
#define OPCODE_STRUCT
struct opcode_table
{
    int **xml_table_content;

    int n_cell_per_row;

    int addr_row_count;
};
#endif

struct opcode_table *streamDoc(char *xml_buffer);
int free_xml_table(struct opcode_table **);
