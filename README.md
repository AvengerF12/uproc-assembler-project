# uproc-assembler-project

This project can be split into two components



## Sheet Parser

Automatises the retrieval of some values from a spreadsheet in order to avoid the tedious task of copying them manually every time there is a change.
    
### Dependencies

    libzip, libxml2 are required to be installed in order for the program to work.

### Usage
    
    Give the spreadsheet file as the first argument and the file in which to redirect the output as the second argument.
    In order to work the spreadsheet must be kept consistent with the target one.
    In particular the column containing the address number and the ones containing the opcodes. 
    If these positions change, the constants in xmlParser.h must be changed accordingly.

    Ex ./sheetParser spreadsheet.ods output.bin



## Uproc Assembler

### TODO
