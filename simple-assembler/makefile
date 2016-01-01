C=gcc
CFLAGS=-I.

parsermake:
	bison -d assembler.y
	flex assembler.l
	$(C) -Wall -lfl -o assembler lex.yy.c assembler.tab.c
	rm lex.yy.c assembler.tab.c assembler.tab.h
