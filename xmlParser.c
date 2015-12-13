#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

#include <string.h>


void streamDoc(char *xml_buffer);
static void processNode(xmlTextReaderPtr reader);
static int get_attr_value_i(xmlTextReaderPtr reader, xmlChar *attribute);


static const int table_addr_cell = 1;
static const int table_opcode_cell = 45;


static char ***table_content = NULL;


static int table_count = 0;
static int cell_count = 0;
static int row_count = 0;


static int row_tag_state = 0;
static int cell_tag_state = 0;


void streamDoc(char *xml_buffer) {
	
	xmlDoc *doc = xmlReadMemory(xml_buffer, strlen(xml_buffer) * sizeof(char), "content.xml", NULL, 0);

    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderWalker(doc);
    if (reader != NULL) {
            ret = xmlTextReaderRead(reader);

            while (ret == 1) {

				if(table_count > 1){
					ret = 0;
					break;
				}

                processNode(reader);
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
}


static void processNode(xmlTextReaderPtr reader)
{
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);

    if (name == NULL){
        name = BAD_CAST "--";
    }

    value = xmlTextReaderConstValue(reader);

	if(strcmp(name, "table:table") == 0){
		table_count++;
	}

	if(strcmp(name, "table:table-row") == 0){

		if(row_tag_state == 0){
			const int attr_value = get_attr_value_i(reader, "table:number-rows-repeated");

			if(attr_value != 0){
				row_count += attr_value;
			} else {
				row_count++;
			}

			cell_count = 0;
			row_tag_state = 1;

		} else {
			row_tag_state = 0;
		}
	}

	if(strcmp(name, "table:table-cell") == 0){
		const int attr_value = get_attr_value_i(reader, "table:number-columns-repeated");
		
		if(attr_value != 0){
			cell_count += attr_value;
		} else {
			
			if(cell_tag_state == 0){

			cell_count++;
   
            cell_tag_state = 1;

			} else {

            cell_tag_state = 0;

        	}
			
		}
	}

	if(strcmp(name, "#text") == 0){
		//printf("%s %s %d %d %d\n", name, value, row_count, cell_count, xmlTextReaderNodeType(reader));
	}

	printf("%s %s %d %d %d\n", name, value, row_count, cell_count, xmlTextReaderNodeType(reader));

}


static int get_attr_value_i(xmlTextReaderPtr reader, xmlChar *attribute)
{
	const xmlChar *attr_value = xmlTextReaderGetAttribute(reader, attribute);
	int attr_int = 0;
	
	if(attr_value != NULL){
		char *pEnd = NULL;
		attr_int = strtol(attr_value, &pEnd, 10);
	}

	
	return attr_int;
}

