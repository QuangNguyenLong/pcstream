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
#ifndef PCSTREAM_VISIBILITY_COMPUTER_H
#define PCSTREAM_VISIBILITY_COMPUTER_H

#include <pcstream/def.h>
#include <pcstream/mesh.h>
typedef struct pcs_visibility_computer_t pcs_visibility_computer_t;

struct pcs_visibility_computer_t
{
  pcs_ratio_t ratio;

  pcs_ret_t (*post)(pcs_visibility_computer_t *,
                    const float *, // MVP
                    pcs_mesh_t     // box
  );
  pcs_ret_t (*get)(pcs_visibility_computer_t *, pcs_ratio_t *);
};

PCSTREAM_EXPORT pcs_ret_t pcs_visibility_computer_init(
    pcs_visibility_computer_t *self, int type);
PCSTREAM_EXPORT pcs_ret_t
pcs_visibility_computer_destroy(pcs_visibility_computer_t *self);

pcs_ret_t
pcs_visibility_computer_post_hull(pcs_visibility_computer_t *self,
                                  const float               *MVP,
                                  pcs_mesh_t                 hull);

pcs_ret_t
pcs_visibility_computer_get_hull(pcs_visibility_computer_t *self,
                                 pcs_ratio_t               *ratio);

#endif
