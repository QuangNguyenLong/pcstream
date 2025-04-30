#include <pcstream/buffer.h>
#include <pcstream/http.h>
#include <pcstream/tmc2_decoder.h>
int main(void)
{
  pcs_buffer_t buff = {0};
  PCSTREAM_RET ret  = 0;

  pcs_buffer_init(&buff);

  pcs_http_get_to_buffer("https://127.0.0.1:8080/3.seg00001.r3.bin",
                         PCSTREAM_HTTP_2_0,
                         &buff,
                         PCSTREAM_NULL);

  ret = decode_video(buff.data, buff.size);

  pcs_buffer_destroy(&buff);
}
