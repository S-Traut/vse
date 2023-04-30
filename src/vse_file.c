#include "vse_file.h"

#include "stdio.h"
#include <stdlib.h>

VseFile vse_file_read(char *file_path)
{
  FILE *file = fopen(file_path, "rb");
  fseek(file, 0L, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *data = (char *)malloc(sizeof(char) * size + 1);
  fread(data, size, size, file);
  fclose(file);

  return (VseFile){
      .data = data,
      .size = size,
  };
}

void vse_file_destroy(VseFile file)
{
  free(file.data);
}
