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
#ifndef PCSTREAM_MESH_H
#define PCSTREAM_MESH_H

#include <pcstream/def.h>
#include <stdint.h>
typedef struct pcs_mesh_t pcs_mesh_t;

struct pcs_mesh_t
{
  float    *pos;
  uint32_t  num_verts;
  uint32_t *indices;
  uint32_t  num_indices;

  pcs_ret_t (*read_from_buff_serial)(pcs_mesh_t *,
                                     const char *,
                                     pcs_count_t);
  pcs_ret_t (*write_to_buff_serial)(pcs_mesh_t *,
                                    char **,
                                    pcs_count_t *);

  pcs_ret_t (*write_to_file_ply)(pcs_mesh_t *,
                                 const char *,
                                 pcs_bool_t);
  pcs_ret_t (*read_from_file_ply)(pcs_mesh_t *, const char *);

  pcs_ret_t (*screen_ratio)(pcs_mesh_t *, const float *, float *);
};

PCSTREAM_EXPORT pcs_ret_t pcs_mesh_init(pcs_mesh_t *self);
PCSTREAM_EXPORT pcs_ret_t pcs_mesh_destroy(pcs_mesh_t *self);

pcs_ret_t pcs_mesh_read_from_buff_serial(pcs_mesh_t *self,
                                         const char *data,
                                         pcs_count_t size);
pcs_ret_t pcs_mesh_write_to_buff_serial(pcs_mesh_t  *self,
                                        char       **data_out,
                                        pcs_count_t *size_out);

pcs_ret_t pcs_mesh_read_from_file_ply(pcs_mesh_t *self,
                                      const char *filepath);
pcs_ret_t pcs_mesh_write_to_file_ply(pcs_mesh_t *self,
                                     const char *filepath,
                                     pcs_bool_t  binary);

pcs_ret_t pcs_mesh_screen_ratio(pcs_mesh_t  *self,
                                const float *mvp,
                                float       *screen_ratio);

#endif
