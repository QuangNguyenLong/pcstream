#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *filepath, size_t *out_size)
{
  FILE *file = fopen(filepath, "rb");
  if (!file)
  {
    perror("Error opening file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = (size_t)ftell(file);
  rewind(file);

  char *buffer = malloc(file_size);
  if (!buffer)
  {
    perror("Memory allocation failed");
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(buffer, 1, file_size, file);
  fclose(file);

  if (out_size)
    *out_size = bytes_read;

  return buffer;
}
