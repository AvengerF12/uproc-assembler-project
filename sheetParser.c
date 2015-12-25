#include "xmlParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zip.h>


char *read_zip_content(char *);


int main(int argc, char *argv[])
{

    char *xml_content = read_zip_content(argv[1]);

    int **xml_table = streamDoc(xml_content);

    printf("%X\n",xml_table[3][0]);

    free_xml_table(&xml_table);

    free(xml_content);

    return 0;
}


char *read_zip_content(char *zip_name)
{
    int err = 0;
    const char *f_to_read = "content.xml";
    char *content_data = NULL;

    zip_t *z_file = zip_open(zip_name, ZIP_RDONLY, &err);
    if(z_file == NULL) {fputs("Cannot open zip file", stderr);}

    struct zip_stat content_stat;
    zip_stat_init(&content_stat);

    int f_stat_r = zip_stat(z_file, f_to_read, 0, &content_stat);
    if(f_stat_r == -1) {fputs("Cannot get stats", stderr);}

    zip_file_t *f_content = zip_fopen(z_file, f_to_read, ZIP_FL_UNCHANGED);
    if(f_content == NULL) {fputs("Cannot open content file", stderr);}

    content_data = malloc(sizeof(char)*content_stat.size + 1);

    zip_fread(f_content, content_data, content_stat.size);

    int f_close_r = zip_fclose(f_content);
    if(f_close_r == -1) {fputs("Some error occurred while closing the file.", stderr);};

    int close_r = zip_close(z_file);
    if(close_r == -1){fputs("Some error occurred while closing the zip file.", stderr);};

    return content_data;
}
