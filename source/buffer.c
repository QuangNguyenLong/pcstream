#include <pcstream/buffer.h>

pcs_ret_t pcs_buffer_init(pcs_buffer_t *self)
{
  *self = (pcs_buffer_t){0};
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t pcs_buffer_destroy(pcs_buffer_t *self)
{
  if (self->data != PCSTREAM_NULL)
  {
    free(self->data);
  }
  *self = (pcs_buffer_t){0};
  return PCSTREAM_RET_SUCCESS;
}
