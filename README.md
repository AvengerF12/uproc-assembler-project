# uproc-assembler-project

This project can be split into two components



## Sheet Parser

Automatises the retrieval of values from certain columns of a spreadsheet, converts them into binary and stores them in a separate file.
This is done in order to avoid the original workflow in which the modifications would've been done manually with an hex editor for every single change.
    
### Dependencies

    libzip, libxml2 are required in order for the program to work.

### Usage
    
    Give the spreadsheet file as the first argument and the file in which to redirect the output as the second argument.
    In order to work the spreadsheet must be kept consistent with the target one.
    In particular the column containing the address number and the ones containing the opcodes. 
    If these positions are changed, the constants in xmlParser.c must be changed accordingly.

    Ex:
    make; ./sheetParser spreadsheet.ods output.bin;



## Uproc Assembler

Takes a text file containing the assembly code for the 8-bit cpu and converts it into machine code.
Instructions supported :

- JMP value
- MOV AX, value
- ADD AX, value
- MOV [ADDR], VALUE
- ADD [ADDR], [ADDR]

### Dependencies

    flex, bison are required in order for the program to work.

### Usage
    
    For the full set of machine instructions available check the spreadsheet.
    
    For now the instruction's address is hardcoded.

    Ex:
    make; ./assembler code.asm output.bin;

