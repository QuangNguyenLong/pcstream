#include <pcstream/buffer.h>
#include <pcstream/http.h>
#include <pcstream/video_decoder.h>
int main(int argc, char **argv)
{
  if (argc < 2)
    return -1;
  pcs_buffer_t        buff = {0};
  pcs_gof_t           rec  = {0};
  pcs_video_decoder_t dc   = {0};

  pcs_gof_init(&rec);
  pcs_buffer_init(&buff);
  pcs_video_decoder_init(&dc, PCSTREAM_VIDEO_DECODER_MPEG_VPCC);

  pcs_http_get_to_buffer((const char *)argv[1],
                         PCSTREAM_HTTP_2_0,
                         &buff,
                         PCSTREAM_NULL);

  dc.post(&dc, buff.data, buff.size);
  dc.get(&dc, &rec);

  pcs_buffer_destroy(&buff);
  pcs_gof_destroy(&rec);
  return 0;
}
