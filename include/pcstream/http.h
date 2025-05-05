#ifndef PCSTREAM_HTTP_H
#define PCSTREAM_HTTP_H

#include <pcstream/buffer.h>
#include <pcstream/def.h>

PCSTREAM_EXPORT PCSTREAM_RET
pcs_http_get_to_buffer(const char           *url,
                       PCSTREAM_HTTP_VERSION ver,
                       pcs_buffer_t         *des,
                       PCSTREAM_BW          *dl_speed);

#endif
