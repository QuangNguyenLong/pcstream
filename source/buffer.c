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
#include <pcstream/buffer.h>

pcs_ret_t pcs_buffer_init(pcs_buffer_t *self)
{
  *self = (pcs_buffer_t){0};
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t pcs_buffer_destroy(pcs_buffer_t *self)
{
  if (self->data != PCSTREAM_NULL)
  {
    free(self->data);
  }
  *self = (pcs_buffer_t){0};
  return PCSTREAM_RET_SUCCESS;
}
