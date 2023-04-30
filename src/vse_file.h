#ifndef VSE_FILE
#define VSE_FILE

#include <inttypes.h>

typedef struct
{
  char *data;
  uint32_t size;
} VseFile;

VseFile vse_file_read(char *file_path);
void vse_file_destroy(VseFile file);

#endif