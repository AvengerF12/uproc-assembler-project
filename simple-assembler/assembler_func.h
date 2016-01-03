#include <stdlib.h>

int file_write_b(char *o_file_name, int8_t *buffer, size_t size, size_t count);
void *safe_realloc (void* ptr, size_t new_size);
int append_int_buffer(int8_t **old_buffer, int8_t *to_append_buffer, int *buffer_size, int append_size);
