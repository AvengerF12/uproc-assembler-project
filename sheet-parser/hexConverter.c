#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Convert the text into binary

static int file_write_b(char *o_file_name, int *buffer, size_t size);
int hex_to_bin(char *i_buffer, char *o_file_name);
//void binToHex(char *inFileName, char *outFileName);

/*
char *file_read(char *i_file_name, char *r_options)
{
    // Input file
    FILE * i_file = NULL;

    char *buffer = NULL;

    size_t result;
    long l_size;

    inFile = fopen (i_file_name, r_options);
    if (i_file==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (i_file, 0 , SEEK_END);
    l_size = ftell (i_file);
    rewind (i_file);

    buffer = calloc(sizeof(char), l_size + 1);

    // copy the file into the buffer:
    result = fread (buffer, 1, l_size, i_file);
    if (result != l_size) {fputs ("Reading error",stderr); exit (3);}

    buffer[l_size] = '\0';

    fclose(i_file);

    return buffer;
}

*/


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
    int l_buffer = strlen(i_buffer);

    int *o_buffer = NULL;
    char *p_end = NULL;

    int n_hex = 0;
    int is_same_number = 0;


    for(int k = 0;k < l_buffer; k++){
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

/*
void binToHex(char *inFileName, char *outFileName)
{
    FILE * inFile = NULL;
    FILE * outFile = NULL;

    int * inBuffer = NULL;
    char * outBuffer = NULL;

    size_t result;
    long lSize;

    inFile = fopen ( inFileName , "rb+" );
    if (inFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (inFile , 0 , SEEK_END);
    lSize = ftell (inFile) / sizeof(int); // Bytes since the beginning of the bin file
    rewind (inFile);

     // allocate memory to contain the whole file:
    inBuffer = (int*) malloc (sizeof(int)*lSize); //needed to represent one of the hex numbers contained
    if (inBuffer == NULL) {fputs ("Memory error",stderr); exit (2);}

     // copy the file into the buffer:
    result = fread (inBuffer, sizeof(int), lSize, inFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
    
    // the whole file is now loaded in the memory buffer. 

    fclose (inFile);

    char * tmpStr = NULL;

    tmpStr = (char*) malloc(sizeof(char)*10); // 8 digits + space + \0
    if (tmpStr == NULL) {fputs ("Memory error",stderr); exit (4);}

    outBuffer = (char*) malloc(lSize*sizeof(char)*10);
    if (outBuffer == NULL) {fputs ("Memory error",stderr); exit (5);}
    outBuffer[0] = '\0'; // Correct way to strcat

    outFile = fopen (outFileName, "w+");
    if (outFile == NULL) {fputs ("File error", stderr); exit(6);}

    for(int i = 0;i < lSize; i++){
        snprintf(tmpStr, 10, "%08X\n", inBuffer[i]);
        strcat(outBuffer, tmpStr);
    }
    
    fwrite(outBuffer, sizeof(char), lSize*9, outFile); // 9 = 8 digits + space... no \0 apparently
    
    // terminate
    free (inBuffer);
    free (outBuffer);
    free (tmpStr);

    fclose(outFile);

}
*/
