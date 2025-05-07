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
#ifndef PCSTREAM_GOF_H
#define PCSTREAM_GOF_H

#include <pcstream/def.h>
#include <pcstream/point_cloud.h>

#ifdef __cplusplus
extern "C"
{
#endif
  typedef struct pcs_gof_t pcs_gof_t;

  /**
   * @struct pcs_gof_t
   * @brief Structure representing a group of frames.
   * @details This structure holds a collection of frames and
   * provides functions to manage and manipulate them. It includes
   * the frames themselves (`pcs_point_cloud_t`) and the size of the
   * group.
   */
  struct pcs_gof_t
  {
    /** @brief Pointer to the array of frames (pcs_point_cloud_t) in
     * the group. */
    pcs_point_cloud_t *frames;
    size_t             size;
    pcs_ret_t (*copy)(pcs_gof_t *, void *, PCSTREAM_VIDEO_DECODER);
  };

  PCSTREAM_EXPORT pcs_ret_t pcs_gof_init(pcs_gof_t *self);
  PCSTREAM_EXPORT pcs_ret_t pcs_gof_destroy(pcs_gof_t *self);
  pcs_ret_t                 pcs_gof_copy(pcs_gof_t             *self,
                                         void                  *src,
                                         PCSTREAM_VIDEO_DECODER decoder);

#ifdef __cplusplus
}
#endif
#endif
