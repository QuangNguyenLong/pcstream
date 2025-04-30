#ifndef PCSTREAM_HTTP_H
#define PCSTREAM_HTTP_H

#include <pcstream/buffer.h>
#include <pcstream/def.h>

typedef enum
{
  PCSTREAM_HTTP_1_1,
  PCSTREAM_HTTP_2_0,
  PCSTREAM_HTTP_3_0
} PCSTREAM_HTTP_VERSION;

PCSTREAM_RET
pcs_http_get_to_buffer(const char           *url,
                       PCSTREAM_HTTP_VERSION ver,
                       pcs_buffer_t         *des,
                       PCSTREAM_BW          *dl_speed);

#endif
