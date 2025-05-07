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
#include <pcstream/visibility_computer.h>

pcs_ret_t
pcs_visibility_computer_init(pcs_visibility_computer_t *self,
                             int                        type)
{
  switch (type)
  {
  case PCSTREAM_VISIBILITY_COMPUTER_HULL:
  {
    self->ratio = PCSTREAM_RATIO_DEFAULT;
    self->post  = pcs_visibility_computer_post_hull;
    self->get   = pcs_visibility_computer_get_hull;
  }
  break;
  default:
  {
    self->ratio = PCSTREAM_RATIO_DEFAULT;
    self->post  = pcs_visibility_computer_post_hull;
    self->get   = pcs_visibility_computer_get_hull;
  }
  break;
  }
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t
pcs_visibility_computer_destroy(pcs_visibility_computer_t *self)
{
  *self = (pcs_visibility_computer_t){0};
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t
pcs_visibility_computer_post_hull(pcs_visibility_computer_t *self,
                                  const float               *MVP,
                                  pcs_mesh_t                 hull)
{
  hull.screen_ratio(&hull, MVP, &self->ratio);
  if (self->ratio < 0 || self->ratio > 1.0F)
  {
    self->ratio = PCSTREAM_RATIO_DEFAULT;
    return PCSTREAM_RET_FAIL;
  }
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t
pcs_visibility_computer_get_hull(pcs_visibility_computer_t *self,
                                 pcs_ratio_t               *ratio)
{
  if (self->ratio < 0)
    return PCSTREAM_RET_FAIL;

  *ratio = self->ratio;
  return PCSTREAM_RET_SUCCESS;
}
