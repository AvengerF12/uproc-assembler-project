#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

#include <string.h>
#include <ctype.h>

#include "struct.h"

#define N_OP_ADD_CELLS 9
#define MAX_N_TABLES 1
#define MAX_REPEAT_ALLOWED 100


struct opcode_table *streamDoc(char *xml_buffer);
int free_xml_table(struct opcode_table **xml_content);

static int processNode(xmlTextReaderPtr reader, struct opcode_table *xml_content);
static int read_next_text_elem(xmlTextReaderPtr reader);
static int get_text_value(xmlTextReaderPtr xml_reader);
static int get_attr_value_i(xmlTextReaderPtr reader, xmlChar *attribute);
static int check_if_opcode(int cell_pos);
static int check_valid_string(const xmlChar *string);

static const int table_addr_cell = 1;
static const int table_opcode_cell[] = {45,46,48,49,51,52,54,55};

static const size_t row_size = sizeof(int) * N_OP_ADD_CELLS;

static int table_count = 0;
static int cell_count = 0;
static int row_count = 0;

static int row_is_valid = 0;


/* Parses each element of the first xml table that it founds.
 * Checks whether a row is valid or not by parsing the table_addr_cell.
 * If valid it retrieves the numbers in the table_opcode_cell[] positions, otherwise it skips the row.
 * It stores all of the data required inside an opcode_table struct and finally it returns it to the caller.
*/

struct opcode_table *streamDoc(char *xml_buffer) {
	
	xmlDoc *doc = xmlReadMemory(xml_buffer, strlen(xml_buffer) * sizeof(char), "content.xml", NULL, 0);

    struct opcode_table *content = malloc(sizeof(struct opcode_table));
    content->xml_table_content = NULL;
    content -> n_cell_per_row = N_OP_ADD_CELLS;
    content -> addr_row_count = 0;

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

            stop_bit = processNode(reader, content);
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

    table_count = 0;
    cell_count = 0;
    row_count = 0;
    row_is_valid = 0;

	return content;
}


static int processNode(xmlTextReaderPtr reader, struct opcode_table *xml_content)
{
    const xmlChar *name;
	int type = xmlTextReaderNodeType(reader);
	int value = 0;

    name = xmlTextReaderConstName(reader);

    if (name == NULL){
        name = BAD_CAST "--";
    }

	if(strcmp(name, "table:table") == 0){
		table_count++;
//		printf("Table n: %d\n", table_count);
	}

	// Count the number of rows in the current table and reset the number of cells each time
    else if(strcmp(name, "table:table-row") == 0 && type != XML_READER_TYPE_END_ELEMENT){

		const int attr_value = get_attr_value_i(reader, "table:number-rows-repeated");

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
		const int attr_value = get_attr_value_i(reader, "table:number-columns-repeated");
		// Space that it extends for +1 for itself
		cell_count += attr_value + 1;
//		printf("Covered, %d\n", cell_count);
	}

	// Count the number of cells in the current line
    else if(strcmp(name, "table:table-cell") == 0 && type != XML_READER_TYPE_END_ELEMENT){

        // Number of attributes
		const int attr_value = get_attr_value_i(reader, "table:number-columns-repeated");
		int op_pos = 0;

		cell_count+=attr_value;

		// This if handles the address cell
        if(table_addr_cell == cell_count){

			value = get_text_value(reader);

			if(value != -1){
            
				row_is_valid = 1;

                xml_content -> addr_row_count++;
            	// Increase the size of the array for each line with a valid address code
            	int **temp_p = realloc(xml_content -> xml_table_content, sizeof(int*)*xml_content -> addr_row_count);

                if(temp_p == NULL){
                    fprintf(stderr, "Unexpected behaviour: realloc");
                    free_xml_table(&xml_content);
                    exit(4);
                }

            	temp_p[xml_content -> addr_row_count-1] = malloc(row_size);

                // Initialise integers
                for(int i=0;i<N_OP_ADD_CELLS;i++){
                    temp_p[xml_content -> addr_row_count-1][i] = 0;
                }

            	xml_content -> xml_table_content = temp_p;

            	// Registers new value in the table
            	(xml_content -> xml_table_content)[xml_content -> addr_row_count-1][table_addr_cell-1] = value;
//            	printf("a:%X %d %d\n",(xml_content -> xml_table_content)[xml_content -> addr_row_count-1][table_addr_cell-1], row_count, cell_count);
			}
        }	

		op_pos = check_if_opcode(cell_count);

		// This 'if' handles the attribute repetition on opcode cells
		if(row_is_valid && attr_value < MAX_REPEAT_ALLOWED && op_pos != -1){

//			printf("N attr: %d, %s\n", xmlTextReaderAttributeCount(reader), xmlTextReaderReadOuterXml(reader));

//			printf("Calling 1: %d\n", op_pos);
			// Write number, in the current position, of the two that compose the opcode
			value = get_text_value(reader);

            if(value == -1){
				exit(3);
            }

        	(xml_content -> xml_table_content)[xml_content -> addr_row_count-1][op_pos+1] = value;


			// If the column is repeated in the opcode section then first write the second number and then the first
			if(attr_value == 2){

				op_pos = check_if_opcode(cell_count);

//				printf("Calling 2: %d %d\n", op_pos, attr_value);
				// Write first number of the two that compose the opcode
				(xml_content -> xml_table_content)[xml_content -> addr_row_count-1][op_pos] = value;


//				printf("%X %d %d \n", (xml_content -> xml_table_content)[xml_content -> addr_row_count-1][op_pos], row_count, cell_count-1);
			}

//			printf("%X %d %d \n", (xml_content -> xml_table_content)[xml_content -> addr_row_count-1][op_pos+1], row_count, cell_count);

		}
	}

	return 0;
}


static int check_if_opcode(int cell_pos)
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
			int n_cell = get_attr_value_i(xml_reader, "table:number-columns-repeated");
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


static int get_text_value(xmlTextReaderPtr xml_reader)
{
    xmlChar *attr_value = xmlTextReaderGetAttribute(xml_reader, "office:value");
	xmlChar *value_type = xmlTextReaderGetAttribute(xml_reader, "office:value-type");

	int result = 0;

	if(value_type == NULL){
        xmlFree(attr_value);
        xmlFree(value_type);
		return -1;
	}

    if(strcmp(value_type, "string") == 0){
        read_next_text_elem(xml_reader);
		const xmlChar *value = xmlTextReaderConstValue(xml_reader);
	
		if(check_valid_string(value) == -1){
            xmlFree(attr_value);
            xmlFree(value_type);
			return -1;
		}

        result = strtol(value, NULL, 16);

    } else if(strcmp(value_type, "float") == 0){
    	if(attr_value == NULL){
            xmlFree(attr_value);
            xmlFree(value_type);
			return -1;
		}

        result = strtol(attr_value, NULL, 16);

    }

//	printf("Value:%s, %X\n", attr_value, result);	

	if(strcmp(value_type, "string") != 0 && strcmp(attr_value,"0") != 0 && result == 0){
		fprintf(stderr, "Unexpected behaviour: error in conversion");
		exit(2);
	}

	xmlFree(attr_value);
	xmlFree(value_type);

	return result;

}


static int get_attr_value_i(xmlTextReaderPtr reader, xmlChar *attribute)
{
	xmlChar *attr_value = xmlTextReaderGetAttribute(reader, attribute);
	int attr_int = 0;
	
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


int free_xml_table(struct opcode_table **xml_content)
{
    for(int i=0;i<(*xml_content)->addr_row_count;i++){
        free((*xml_content)->xml_table_content[i]);
    }

    free((*xml_content)->xml_table_content);

    free(*xml_content);

    return 0;
}

