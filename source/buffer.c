#include <pcstream/buffer.h>

PCSTREAM_RET pcs_buffer_init(pcs_buffer_t *self)
{
  self->data = PCSTREAM_NULL;
  self->size = 0;
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET pcs_buffer_destroy(pcs_buffer_t *self)
{
  if (self->data != PCSTREAM_NULL)
    free(self->data);
  self->data = PCSTREAM_NULL;
  self->size = 0;
  return PCSTREAM_RET_SUCCESS;
}
