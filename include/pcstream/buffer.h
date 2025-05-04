#ifndef PCSTREAM_BUFFER_H
#define PCSTREAM_BUFFER_H

#include <pcstream/def.h>
#include <stdlib.h>

typedef struct pcs_buffer_t pcs_buffer_t;

struct pcs_buffer_t
{
  char  *data;
  size_t size;
};

PCSTREAM_RET pcs_buffer_init(pcs_buffer_t *self);
PCSTREAM_RET pcs_buffer_destroy(pcs_buffer_t *self);

#endif
