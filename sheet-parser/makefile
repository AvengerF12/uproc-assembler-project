C=gcc
CFLAGS=-I.
DEPS = xmlParser.h hexConverter.h struct.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

parsermake: xmlParser.o sheetParser.o hexConverter.o 
	$(C) -g -O0 -Wall -lzip -lxml2 -o sheetParser hexConverter.o xmlParser.o sheetParser.o -I.
