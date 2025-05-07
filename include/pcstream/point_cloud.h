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
#ifndef PCSTREAM_POINT_CLOUD_H
#define PCSTREAM_POINT_CLOUD_H

#include <pcstream/def.h>
#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct pcs_point_cloud_t pcs_point_cloud_t;

  struct pcs_point_cloud_t
  {

    // len = 3 * size *
    // pcs_type_size_g[pcs_pos_type_g[PCSTREAM_VIDEO_DECOER]]
    void          *positions;
    unsigned char *colors; // len = 3 * size
    // float         *normals;
    size_t         size; // number of points
    pcs_ret_t (*copy)(pcs_point_cloud_t *,
                      void *,
                      PCSTREAM_VIDEO_DECODER);
  };

  PCSTREAM_EXPORT pcs_ret_t
  pcs_point_cloud_init(pcs_point_cloud_t *self);
  PCSTREAM_EXPORT pcs_ret_t
            pcs_point_cloud_destroy(pcs_point_cloud_t *self);
  pcs_ret_t pcs_point_cloud_copy(pcs_point_cloud_t     *self,
                                 void                  *src,
                                 PCSTREAM_VIDEO_DECODER decoder);

#ifdef __cplusplus
}
#endif
#endif
