//Rework all the function in order to give the control to the programmer
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

#include <string.h>

#define N_OP_ADD_CELLS 9
#define MAX_N_TABLES 1
#define MAX_REPEAT_ALLOWED 100

int **streamDoc(char *xml_buffer);
static int processNode(xmlTextReaderPtr reader);
static long get_attr_value_i(xmlTextReaderPtr reader, xmlChar *attribute);
static long check_if_opcode(long cell_pos);

static xmlDoc *doc = NULL;

static const int table_addr_cell = 1;
static const int table_opcode_cell[] = {45,46,48,49,51,52,54,55};

static const size_t row_size = sizeof(int) * N_OP_ADD_CELLS;
int **table_op_addr_cont= NULL; 

static long table_count = 0;
static long cell_count = 0;
static long row_count = 0;

static long addr_row_count = 0;


// Parses each element of the first xml table
int **streamDoc(char *xml_buffer) {
	
	doc = xmlReadMemory(xml_buffer, strlen(xml_buffer) * sizeof(char), "content.xml", NULL, 0);

	table_op_addr_cont = NULL;

    xmlTextReaderPtr reader;
    int ret;

    // When set to 1 stops the loop
	int stop_bit = 0;

    reader = xmlReaderWalker(doc);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);

        while (ret == 1) {

			if(table_count > MAX_N_TABLES || stop_bit == 1){
				ret = 0;
				break;
			}

            stop_bit = processNode(reader);
            ret = xmlTextReaderRead(reader);
        }   

        xmlFreeTextReader(reader);

        if (ret != 0) {
            fprintf(stderr, "xml document : failed to parse\n");
        }   

    } else {
        fprintf(stderr, "Unable to open xml document\n");
    }

	xmlFreeDoc(doc);

    xmlCleanupParser();

	return table_op_addr_cont;
}


static int processNode(xmlTextReaderPtr reader)
{
    const xmlChar *name, *value;
	int type = xmlTextReaderNodeType(reader);

    name = xmlTextReaderConstName(reader);

    if (name == NULL){
        name = BAD_CAST "--";
    }

    value = xmlTextReaderConstValue(reader);

	if(strcmp(name, "table:table") == 0){
		table_count++;
        printf("Tables n%d\n", table_count);
	}

	// Count the number of rows in the current table and reset the number of cells each time
    else if(strcmp(name, "table:table-row") == 0 && type != XML_READER_TYPE_END_ELEMENT){

		const int attr_value = get_attr_value_i(reader, "table:number-rows-repeated");

		if(attr_value != 0 && attr_value < MAX_REPEAT_ALLOWED){
			row_count += attr_value;
		} else if(attr_value == 0 && attr_value < MAX_REPEAT_ALLOWED){
			row_count++;
		} else if(attr_value > MAX_REPEAT_ALLOWED) {
			return 1;
		}

		cell_count = 0;
	}

	// Count the number of cells in the current line
    else if(strcmp(name, "table:table-cell") == 0 && type != XML_READER_TYPE_END_ELEMENT){
        // Number of attributes
		long attr_value = get_attr_value_i(reader, "table:number-columns-repeated");
		
		if(attr_value != 0 && attr_value < MAX_REPEAT_ALLOWED){

			// This 'if' handles the attribute repetition on opcode cells
			int op_pos = 0;
			if(op_pos = check_if_opcode(cell_count) != -1){

				value = xmlTextReaderConstValue(reader);
        		table_op_addr_cont[addr_row_count][op_pos+1] = strtol(value, NULL, 16);
                printf("%X %d %d \n", table_op_addr_cont[row_count-1][op_pos+1], row_count, cell_count+1);
			}

			cell_count += attr_value;

		} else{
			
			cell_count++;
		}
	}

   else if(strcmp(name, "#text") == 0){
		if(table_addr_cell == cell_count){
			
            addr_row_count++;
/*			// Increase the size of the array for each line with a valid address code
			int **temp_p = realloc(table_op_addr_cont, sizeof(int*)*addr_row_count);
            temp_p[addr_row_count-1] = malloc(row_size);
			table_op_addr_cont = temp_p;

            // Registers new value in the table
			table_op_addr_cont[addr_row_count-1][cell_count-1] = strtol(value, NULL, 16);
//			printf("addr: %X %d %d \n", table_op_addr_cont[row_count-1][cell_count-1], row_count, cell_count);
*/		}
/*
		int op_pos = 0;
		if(op_pos = check_if_opcode(cell_count) != -1){
			table_op_addr_cont[addr_row_count-1][op_pos+1] = strtol(value, NULL, 16);
//			printf("%X %d %d\n", table_op_addr_cont[row_count-1][op_pos+1], row_count, cell_count);
		}*/
	}

	printf("%s\n", name);
	return 0;
}


static long check_if_opcode(long cell_pos)
{
	for(int i=0;i<N_OP_ADD_CELLS-1;i++){
        if(table_opcode_cell[i] == cell_pos){
			return i;
        }
    }

	return -1;
}


static long get_attr_value_i(xmlTextReaderPtr reader, xmlChar *attribute)
{
	xmlChar *attr_value = xmlTextReaderGetAttribute(reader, attribute);
	long attr_int = 0;
	
	if(attr_value != NULL){
		attr_int = strtol(attr_value, NULL, 10);
	}

    xmlFree(attr_value);
	
	return attr_int;
}


int free_xml_table(int **xml_table_ptr)
{
    for(int i=0;i<addr_row_count;i++){
        free(xml_table_ptr[i]);
    }

    free(xml_table_ptr);

    return 0;
}
