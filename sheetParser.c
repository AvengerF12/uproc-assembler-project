#include "xmlParser.h"
#include "hexConverter.h"
#include "struct.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zip.h>


char *read_zip_content(char *);


int main(int argc, char *argv[])
{

    char *xml_content = read_zip_content(argv[1]);

    struct opcode_table *xml_table = streamDoc(xml_content);

    int row_count = xml_table->addr_row_count;
    int cell_per_row = xml_table->n_cell_per_row;

    int buffer_size = row_count * cell_per_row;
    int buffer_index = 0;
    char *hex_buffer = calloc(sizeof(char) + 1, buffer_size); // + 1 is needed for '\0' and spaces between numbers


    // Convert the table into a single string of values
    for(int i=0;i<row_count;i++){
        for(int k=0;k<cell_per_row;k++){
            if(k==0){
                hex_buffer[buffer_index] = '\n';
            } else {
                char *ch = malloc(sizeof(char)+1);
                snprintf(ch, 2, "%X\n", xml_table->xml_table_content[i][k]);
                hex_buffer[buffer_index] = ch[0];
                free(ch);
            }

            buffer_index++;
        }
    }

    hex_buffer[buffer_size*2-1] = '\0';

    hex_to_bin(argv[2], hex_buffer);

    free(hex_buffer);

    free_xml_table(&xml_table);
    free(xml_content);

    return 0;
}


char *read_zip_content(char *zip_name)
{
    int err = 0;
    const char *f_to_read = "content.xml";
    char *content_data = NULL;

    // Open the zip file
    zip_t *z_file = zip_open(zip_name, ZIP_RDONLY, &err);
    if(z_file == NULL) {fputs("Cannot open zip file", stderr);}

    // Initialize zip struct
    struct zip_stat content_stat;
    zip_stat_init(&content_stat);

    // Read zip info
    int f_stat_r = zip_stat(z_file, f_to_read, 0, &content_stat);
    if(f_stat_r == -1) {fputs("Cannot get stats", stderr);}

    // Open the file inside the zip content
    zip_file_t *f_content = zip_fopen(z_file, f_to_read, ZIP_FL_UNCHANGED);
    if(f_content == NULL) {fputs("Cannot open content file", stderr);}

    content_data = calloc(sizeof(char), content_stat.size + 1);

    // Read file's content
    zip_fread(f_content, content_data, content_stat.size);

    // Close file
    int f_close_r = zip_fclose(f_content);
    if(f_close_r == -1) {fputs("Some error occurred while closing the file.", stderr);};

    // Close zip
    int close_r = zip_close(z_file);
    if(close_r == -1){fputs("Some error occurred while closing the zip file.", stderr);};

    return content_data;
}
