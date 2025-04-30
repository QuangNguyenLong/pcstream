#ifndef PCSTREAM_CORE_H
#define PCSTREAM_CORE_H

#include <stdint.h>
#include <stddef.h>

#define PCSTREAM_RET           uint32_t
#define PCSTREAM_BW            int64_t // Bytes/s
#define PCSTREAM_RATIO         float
#define PCSTREAM_COUNT         uint64_t
#define PCSTREAM_LOD_VERSION   uint8_t
#define PCSTREAM_BOOL          uint8_t

#define PCSTREAM_UNINITIALIZED -1
#define PCSTREAM_TRUE          (1)
#define PCSTREAM_FALSE         (0)
#define PCSTREAM_MAX_BUFF      0xffff
#define PCSTREAM_BW_DEFAULT    PCSTREAM_UNINITIALIZED
#define PCSTREAM_RATIO_DEFAULT PCSTREAM_UNINITIALIZED

#define PCSTREAM_NULL          ((void *)0)

#define PCSTREAM_RET_FAIL      0x00
#define PCSTREAM_RET_SUCCESS   0x01

#define PCSTREAM_FLOAT_ERROR   1e-6f

#endif
