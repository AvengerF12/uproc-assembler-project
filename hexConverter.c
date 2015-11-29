#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Convert the text into binary

/*Takes an input file containing hex values as an argument and outputs a binary file with those same values. Nedeed to ease introduction of values inside LogicCircuit.*/

void hexToBin(char *inFileName, char *outFileName);
void binToHex(char *inFileName, char *outFileName);


int main (int argc, char *argv[])
{
    char * inputFile = argv[2];
    char * outputFile = argv[3];
    
    if(argc < 4){
        printf("Incorrect arguments. Insert an option (-d to decrypt and -e to encrypt) the input file and the output file");
        return 0;
    }

    
    switch(argv[1][1]){
        case 'e':
            hexToBin(inputFile, outputFile);
            break;
        case 'd':
            binToHex(inputFile, outputFile);
            break;
    }
    

    return 0;
}


// No more memory errors/leaks...
void hexToBin(char *inFileName, char *outFileName)
{
    
    FILE * inFile = NULL;
    FILE * outFile = NULL;

    char * inBuffer = NULL;
    int * outBuffer = NULL;

    size_t result;
    long lSize;

    inFile = fopen ( inFileName , "r+" );
    if (inFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (inFile , 0 , SEEK_END);
    lSize = ftell (inFile);
    rewind (inFile);
    
    // allocate memory to contain the whole file:
    inBuffer = (char*) malloc (sizeof(char)*lSize + 1); // 1 needed for \0
    if (inBuffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    result = fread (inBuffer,1,lSize,inFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
    
    inBuffer[lSize] = '\0';

    fclose (inFile);


    outFile = fopen (outFileName, "wb+");
    if (outFile == NULL) {fputs ("File error", stderr); exit(4);}

    char *pEnd = NULL;

    int nHex = 0;
    int isSameNumber = 0;


    for(int k = 0;k < lSize; k++){
        if(isalnum(inBuffer[k]) && isSameNumber == 0){
            nHex++;
            isSameNumber = 1;
        }

        while(isspace(inBuffer[k])){
            k++;
            isSameNumber = 0;
        }
    }

    outBuffer = (int*) malloc(sizeof(int)*(nHex)); // Needs 4 byte of memory for each char because hex
    if (outBuffer == NULL) {fputs ("Memory error",stderr); exit (5);}

    outBuffer[0] = strtol(inBuffer, &pEnd, 16);

    for(int i=1;i<nHex;i++){
        outBuffer[i] = strtol(pEnd, &pEnd, 16);
    }

    fwrite(outBuffer, sizeof(int), nHex, outFile);

    // terminate
    free (inBuffer);
    free (outBuffer);

    fclose(outFile);

}


// Don't touch, please... took a while to remove all the memory errors...
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
    
    /* the whole file is now loaded in the memory buffer. */

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
