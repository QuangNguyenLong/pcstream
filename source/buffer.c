#include <pcstream/buffer.h>

PCSTREAM_RET pcs_buffer_init(pcs_buffer_t *this)
{
  this->data = PCSTREAM_NULL;
  this->size = 0;
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET pcs_buffer_destroy(pcs_buffer_t *this)
{
  if (this->data != PCSTREAM_NULL)
    free(this->data);
  this->data = PCSTREAM_NULL;
  this->size = 0;
  return PCSTREAM_RET_SUCCESS;
}
