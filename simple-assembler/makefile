C=gcc
CFLAGS=-I.

assembler-make: 
	bison -d assembler.y
	flex assembler.l
	$(C) -g -Wall -lfl -o assembler lex.yy.c assembler.tab.c $(CFLAGS)
	rm lex.yy.c assembler.tab.c assembler.tab.h
