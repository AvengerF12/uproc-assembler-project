C=gcc
CFLAGS=-I.
DEPS = xmlParser.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

parsermake: xmlParser.o sheetParser.o 
	gcc -Wall -lzip -lxml2 -o sheetParser xmlParser.o sheetParser.o -I.
