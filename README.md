# uproc-assembler-project

This project can be split into two components



## Sheet Parser

Automatises the retrieval of some values from a spreadsheet in order to avoid the tedious task of copying them manually every time there is a change.
    
### Dependencies

    libzip, libxml2 are required in order for the program to work.

### Usage
    
    Give the spreadsheet file as the first argument and the file in which to redirect the output as the second argument.
    In order to work the spreadsheet must be kept consistent with the target one.
    In particular the column containing the address number and the ones containing the opcodes. 
    If these positions change, the constants in xmlParser.h must be changed accordingly.

    Ex:
    make; ./sheetParser spreadsheet.ods output.bin;



## Uproc Assembler

Automatises the retrieval of some values from a spreadsheet in order to avoid the tedious task of copying them manually every time there is a change.
    
### Dependencies

    flex, bison are required in order for the program to work.

### Usage
    
    Take a text file containing the assembly code for the 8-bit cpu and convert it into machine code.
    For the full set of machine instructions available check the according spreadsheet.
    
    For now the instruction's address in ROM is hardcoded.

    Ex:
    make; ./assembler code.asm output.bin;

