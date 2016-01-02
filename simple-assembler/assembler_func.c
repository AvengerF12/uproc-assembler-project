#include <stdio.h>
#include <stdlib.h>

int file_write_b(char *o_file_name, int *buffer, size_t size);
void* safe_realloc (void* ptr, size_t new_size);
int append_int_buffer(int **old_buffer, int *to_append_buffer, int buffer_size, int append_size);


int file_write_b(char *o_file_name, int *buffer, size_t size)
{
    FILE * o_file = NULL;

    o_file = fopen (o_file_name, "wb+");
    if (o_file == NULL) {fputs ("File error", stderr); exit(4);}

    fwrite(buffer, sizeof(int), size, o_file);

    fclose(o_file);

    return 0;
}


void* safe_realloc (void* ptr, size_t new_size)
{
	int *temp_ptr = realloc(ptr, sizeof(int*)*new_size);

	if(temp_ptr == NULL){
		fprintf(stderr, "Unexpected behaviour: realloc");
		free(ptr);
		exit(1);
	}

	return temp_ptr;
}


int append_int_buffer(int **old_buffer, int *to_append_buffer, int buffer_size, int append_size)
{
    // Allocate space required for all the new values
    *old_buffer = safe_realloc(*old_buffer, (buffer_size+append_size)*sizeof(int));

    int temp_size = buffer_size;

    // Add all the new values one at time
    for(int i=0;i<append_size;i++){
        *old_buffer[temp_size] = to_append_buffer[i];

        printf("SUP: %X %d\n", *old_buffer[temp_size], temp_size);

        temp_size++;

    }


    return 0;
}

