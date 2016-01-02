#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Convert the text into binary

static int file_write_b(char *o_file_name, int *buffer, size_t size);
int hex_to_bin(char *i_buffer, char *o_file_name);


static int file_write_b(char *o_file_name, int *buffer, size_t size)
{
    FILE * o_file = NULL;

    o_file = fopen (o_file_name, "wb+");
    if (o_file == NULL) {fputs ("File error", stderr); exit(4);}

    fwrite(buffer, sizeof(int), size, o_file);

    fclose(o_file);

    return 0;
}


// Takes an input buffer of hexes and converts them into binary
int hex_to_bin(char *o_file_name, char *i_buffer)
{
    int len_buffer = strlen(i_buffer);

    int *o_buffer = NULL;
    char *p_end = NULL;

    int n_hex = 0;
    int is_same_number = 0;


    for(int k = 0;k < len_buffer; k++){
        if(isalnum(i_buffer[k]) && is_same_number == 0){
            n_hex++;
            is_same_number = 1;
        }

        while(isspace(i_buffer[k])){
            k++;
            is_same_number = 0;
        }
    }

    o_buffer = malloc(sizeof(int)*(n_hex));
    if (o_buffer == NULL) {fputs ("Memory error",stderr); exit (5);}

    o_buffer[0] = strtol(i_buffer, &p_end, 16);

    for(int i=1;i<n_hex;i++){
        o_buffer[i] = strtol(p_end, &p_end, 16);
    }

    file_write_b(o_file_name, o_buffer, n_hex);

    free(o_buffer);

    return 0;
}

