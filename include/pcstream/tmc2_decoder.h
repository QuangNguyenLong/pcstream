#ifndef PCSTREAM_TMC2_DECODER_H
#define PCSTREAM_TMC2_DECODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <pcstream/def.h>

PCSTREAM_RET decode_video(const char *data, const size_t size);

#ifdef __cplusplus
}
#endif
#endif
