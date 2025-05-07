/* This file is part of PCSTREAM.
 * Copyright (C) 2025 FIL Research Group, ANSA Laboratory
 *
 * PCSTREAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef PCSTREAM_CORE_H
#define PCSTREAM_CORE_H

#include "pcstream/pcstream_export.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PCSTREAM_WRAP_ACTION(action) \
  do                                 \
  {                                  \
    action;                          \
  } while (0)

#define PCSTREAM_ACTION_EXIT() \
  PCSTREAM_WRAP_ACTION(exit(EXIT_FAILURE))

#define PCSTREAM_ACTION_LOG_ERROR()                 \
  PCSTREAM_WRAP_ACTION(                             \
      fprintf(stderr,                               \
              "[PCSTREAM ERROR] failed at %s:%d\n", \
              __FILE__,                             \
              __LINE__))

#define PCSTREAM_ACTION_EXIT_ERROR() \
  PCSTREAM_WRAP_ACTION({             \
    PCSTREAM_ACTION_LOG_ERROR();     \
    PCSTREAM_ACTION_EXIT();          \
  })

#define PCSTREAM_CHECK(expr, action)    \
  PCSTREAM_WRAP_ACTION({                \
    if ((expr) != PCSTREAM_RET_SUCCESS) \
    {                                   \
      action;                           \
    }                                   \
  })

#define PCSTREAM_CHECK_ERROR(expr) \
  PCSTREAM_CHECK(expr, PCSTREAM_ACTION_LOG_ERROR())

#define PCSTREAM_CHECK_FATAL(expr) \
  PCSTREAM_CHECK(expr, PCSTREAM_ACTION_EXIT_ERROR())

typedef uint32_t pcs_ret_t;
typedef int64_t  pcs_bw_t; // Bytes/s
typedef float    pcs_ratio_t;
typedef uint64_t pcs_count_t;
typedef uint8_t  pcs_lod_version_t;
typedef uint8_t  pcs_bool_t;

#define PCSTREAM_UNINITIALIZED (-1)
#define PCSTREAM_TRUE          (1)
#define PCSTREAM_FALSE         (0)
#define PCSTREAM_MAX_BUFF      0xffff
#define PCSTREAM_BW_DEFAULT    PCSTREAM_UNINITIALIZED
#define PCSTREAM_RATIO_DEFAULT PCSTREAM_UNINITIALIZED

#ifdef __cplusplus
#define PCSTREAM_NULL nullptr
#else
#define PCSTREAM_NULL ((void *)0)
#endif

#define PCSTREAM_RET_FAIL    0x00
#define PCSTREAM_RET_SUCCESS 0x01

#define PCSTREAM_FLOAT_ERROR (1e-6F)
#define PCSTREAM_FLOAT_HALF  (0.5F)
typedef enum
{
  PCSTREAM_BW_ESTIMATOR_HARMONIC = 1
} PCSTREAM_BW_ESTIMATOR;

typedef enum
{
  PCSTREAM_HTTP_1_1,
  PCSTREAM_HTTP_2_0,
  PCSTREAM_HTTP_3_0
} PCSTREAM_HTTP_VERSION;

typedef enum
{
  PCSTREAM_LOD_SELECTOR_DP_BASED = 1,
  PCSTREAM_LOD_SELECTOR_LM_BASED,
  PCSTREAM_LOD_SELECTOR_EQUAL,
  PCSTREAM_LOD_SELECTOR_HYBRID,
} PCSTREAM_LOD_SELECTOR;

typedef enum
{
  PCSTREAM_REQUEST_HANDLER_H2 = 1
} PCSTREAM_REQUEST_HANDLER;
typedef enum
{
  PCSTREAM_VIEWPORT_ESTIMATOR_VELOCITY = 1
} PCSTREAM_VIEWPORT_ESTIMATOR;

typedef enum
{
  PCSTREAM_VISIBILITY_COMPUTER_HULL = 1
} PCSTREAM_VISIBILITY;

typedef enum
{
  PCSTREAM_VIDEO_DECODER_MPEG_VPCC,
  PCSTREAM_VIDEO_DECODER_MPEG_GPCC,
  PCSTREAM_VIDEO_DECODER_GOOGLE_DRACO,
  PCSTREAM_VIDEO_DECODER_PCL,
  PCSTREAM_VIDEO_DECODER_COUNT
} PCSTREAM_VIDEO_DECODER;

typedef int16_t pcs_mpeg_vpcc_coord_t;
typedef float   pcs_mpeg_gpcc_coord_t;
typedef float   pcs_google_draco_coord_t;
typedef float   pcs_pcl_coord_t;
// typedef enum
// {
//   PCSTREAM_TYPE_FLOAT32,
//   PCSTREAM_TYPE_FLOAT64,
//   PCSTREAM_TYPE_INT8,
//   PCSTREAM_TYPE_UINT8,
//   PCSTREAM_TYPE_INT16,
//   PCSTREAM_TYPE_UINT16,
//   PCSTREAM_TYPE_INT32,
//   PCSTREAM_TYPE_UINT32,
//   PCSTREAM_TYPE_INT64,
//   PCSTREAM_TYPE_UINT64,
//   PCSTREAM_TYPE_COUNT
// } PCSTREAM_TYPE;
//
// extern const size_t pcs_type_size_g[PCSTEAM_TYPE_COUNT];
// extern const PCSTREAM_TYPE
//     pcs_pos_type_g[PCSTREAM_VIDEO_DECODER_COUNT];
#endif
