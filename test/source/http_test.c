#include <pcstream/http.h>

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    return -1;
  }
  pcs_buffer_t buff = {0};

  PCSTREAM_CHECK_FATAL(pcs_buffer_init(&buff));

  PCSTREAM_CHECK_FATAL(pcs_http_get_to_buffer(
      argv[1], PCSTREAM_HTTP_2_0, &buff, PCSTREAM_NULL));

  PCSTREAM_CHECK_FATAL(pcs_buffer_destroy(&buff));
}
