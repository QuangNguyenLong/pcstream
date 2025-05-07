#include <pcstream/buffer.h>

int main(void)
{
  pcs_buffer_t buff = {0};
  PCSTREAM_CHECK_FATAL(pcs_buffer_init(&buff));
  PCSTREAM_CHECK_FATAL(pcs_buffer_destroy(&buff));
}
