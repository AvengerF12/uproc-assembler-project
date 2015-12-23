#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

#include <string.h>
#include <ctype.h>

#define N_OP_ADD_CELLS 9
#define MAX_N_TABLES 1
#define MAX_REPEAT_ALLOWED 100

int **streamDoc(char *xml_buffer);
static int processNode(xmlTextReaderPtr reader);
static int read_next_text_elem(xmlTextReaderPtr reader);
static long get_text_value(xmlTextReaderPtr xml_reader);
static long get_attr_value_i(xmlTextReaderPtr reader, xmlChar *attribute);
static long check_if_opcode(long cell_pos);
static int check_valid_string(const xmlChar *string);

static xmlDoc *doc = NULL;

static const int table_start_row = 9;
static const int table_addr_cell = 1;
static const int table_opcode_cell[] = {45,46,48,49,51,52,54,55};

static const size_t row_size = sizeof(int) * N_OP_ADD_CELLS;
int **table_op_addr_cont= NULL; 

static long table_count = 0;
static long cell_count = 0;
static long row_count = 0;

static int addr_row_count = 0;
static int row_is_valid = 0;


// Parses each element of the first xml table
int **streamDoc(char *xml_buffer) {
	
	doc = xmlReadMemory(xml_buffer, strlen(xml_buffer) * sizeof(char), "content.xml", NULL, 0);

	int **table_op_addr_cont= NULL;

	table_op_addr_cont = malloc(sizeof(int*));
    table_op_addr_cont[0] = malloc(row_size);

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
    const xmlChar *name;
	int type = xmlTextReaderNodeType(reader);
	long value;

    name = xmlTextReaderConstName(reader);

    if (name == NULL){
        name = BAD_CAST "--";
    }

	if(strcmp(name, "table:table") == 0){
		table_count++;
		printf("Table n: %d\n", table_count);
	}

	// Count the number of rows in the current table and reset the number of cells each time
    else if(strcmp(name, "table:table-row") == 0 && type != XML_READER_TYPE_END_ELEMENT){

		const long attr_value = get_attr_value_i(reader, "table:number-rows-repeated");

		if(attr_value < MAX_REPEAT_ALLOWED){
			row_count += attr_value;

		} else {
			// Terminate loop
			return 1;
		}

		cell_count = 0;
		row_is_valid = 0;
	}

	else if(strcmp(name, "table:covered-table-cell") == 0 && type != XML_READER_TYPE_END_ELEMENT){
		const long attr_value = get_attr_value_i(reader, "table:number-columns-repeated");
		// Space that it extends for +1 for itself
		cell_count += attr_value + 1;
//		printf("Covered, %d\n", cell_count);
	}

	// Count the number of cells in the current line
    else if(strcmp(name, "table:table-cell") == 0 && type != XML_READER_TYPE_END_ELEMENT){

        // Number of attributes
		const long attr_value = get_attr_value_i(reader, "table:number-columns-repeated");
		int op_pos = 0;

		cell_count+=attr_value;

		// This if handles the address cell
        if(table_addr_cell == cell_count){

			value = get_text_value(reader);

			if(value != -1){
            
				row_is_valid = 1;

            	addr_row_count++;
            	// Increase the size of the array for each line with a valid address code
            	int **temp_p = realloc(table_op_addr_cont, sizeof(int*)*addr_row_count);
            	temp_p[addr_row_count-1] = malloc(row_size);
            	table_op_addr_cont = temp_p;

            	// Registers new value in the table
            	table_op_addr_cont[addr_row_count-1][table_addr_cell-1] = value;
            	printf("a:%X %d %d\n",table_op_addr_cont[addr_row_count-1][table_addr_cell-1], row_count, cell_count);
			}
        }	

		op_pos = check_if_opcode(cell_count);

		// This 'if' handles the attribute repetition on opcode cells
		if(row_is_valid && attr_value < MAX_REPEAT_ALLOWED && (op_pos != -1)){

//			printf("Cells:%d, Rows:%d, Addr:%d\n", cell_count, row_count, table_op_addr_cont[addr_row_count-1][0]);
//			printf("N attr: %d, %s\n", xmlTextReaderAttributeCount(reader), xmlTextReaderReadOuterXml(reader));

//			printf("Calling 1: %d\n", op_pos);
			// Write number, in the current position, of the two that compose the opcode
			value = get_text_value(reader);

            if(value == -1){
				exit(3);
                printf("Skip\n");
            }

        	table_op_addr_cont[addr_row_count-1][op_pos+1] = value;


			// If the column is repeated in the opcode section then first write the second number and then the first
			if(attr_value == 2){

				op_pos = check_if_opcode(cell_count);

//				printf("Calling 2: %d %d\n", op_pos, attr_value);
				// Write first number of the two that compose the opcode
				table_op_addr_cont[addr_row_count-1][op_pos] = value;


				printf("%X %d %d \n", table_op_addr_cont[addr_row_count-1][op_pos+2], row_count, cell_count-1);
			}

			printf("%X %d %d \n", table_op_addr_cont[addr_row_count-1][op_pos+1], row_count, cell_count);

		}		
	}

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


// Reads the next text element without moving the main reader pointer
static int read_next_text_elem(xmlTextReaderPtr xml_reader)
{

	int ret = 0;
	int type = 0;
	int cell_depth = 0;

	do {
		ret = xmlTextReaderRead(xml_reader);
		const xmlChar *name = xmlTextReaderConstName(xml_reader);
		type = xmlTextReaderNodeType(xml_reader);

		cell_depth++;

		if(strcmp(name, "#text") == 0 && type != XML_READER_TYPE_END_ELEMENT){
            return 0;
        }

		else if(strcmp(name, "table:table-cell") == 0 && type != XML_READER_TYPE_END_ELEMENT){
			long n_cell = get_attr_value_i(xml_reader, "table:number-columns-repeated");
			cell_count+=n_cell;
		}
	
		else if(strcmp(name, "table:table-row") == 0 && type != XML_READER_TYPE_END_ELEMENT){
			fprintf(stderr, "Unexpected behaviour: Skipped row! Name: %s\n", name);
			exit(1);
		}

		if(cell_depth > 2){
			return -1;
		}

	} while(ret == 1);	
}


static long get_text_value(xmlTextReaderPtr xml_reader)
{
    xmlChar *attr_value = xmlTextReaderGetAttribute(xml_reader, "office:value");
	xmlChar *value_type = xmlTextReaderGetAttribute(xml_reader, "office:value-type");

	long result = 0;

	if(value_type == NULL){
		return -1;
	}

    if(strcmp(value_type, "string") == 0){
        read_next_text_elem(xml_reader);
		const xmlChar *value = xmlTextReaderConstValue(xml_reader);
	
		if(check_valid_string(value) == -1){
			return -1;
		}

        result = strtol(value, NULL, 16);

    } else if(strcmp(value_type, "float") == 0){
    	if(attr_value == NULL){
			return -1;
		}

        result = strtol(attr_value, NULL, 16);

    }

//	printf("Value:%s, %X\n", attr_value, result);	

	if(strcmp(value_type, "string") != 0 && strcmp(attr_value,"0") != 0 && result == 0){
		printf("Unexpected behaviour: error in conversion");
		exit(2);
	}

	xmlFree(attr_value);
	xmlFree(value_type);

	return result;

}


static long get_attr_value_i(xmlTextReaderPtr reader, xmlChar *attribute)
{
	xmlChar *attr_value = xmlTextReaderGetAttribute(reader, attribute);
	long attr_int = 0;
	
	if(attr_value != NULL){

		attr_int = strtol(attr_value, NULL, 10);
		xmlFree(attr_value);
		return attr_int;
	} else {

    	xmlFree(attr_value);
		return 1;
	}
}


static int check_valid_string(const xmlChar *string)
{
	if(string == NULL){
		return -1;
	}

	for(int i=0;string[i]!='\0';i++){
		if(!isxdigit(string[i])){
			return -1;
		}
	}
	
	return 0;
}


int free_xml_table(int **xml_table_ptr)
{
    for(int i=0;i<addr_row_count;i++){
        free(xml_table_ptr[i]);
    }

    free(xml_table_ptr);

    return 0;
}
