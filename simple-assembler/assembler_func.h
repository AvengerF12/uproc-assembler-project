#include <stdlib.h>

int file_write_b(char *o_file_name, int *buffer, size_t size);
void* safe_realloc (void* ptr, size_t new_size);
int append_int_buffer(int **old_buffer, int *to_append_buffer, int buffer_size, int append_size);

