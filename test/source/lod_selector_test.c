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
#include "utils.h"
#include <pcstream/lod_selector.h>

#define N_MOD 10
#define N_VER 4

int main(int argc, char **argv)
{
  pcs_lod_selector_t vssl           = {0};
  char              *buff           = PCSTREAM_NULL;
  size_t             size           = 0;
  const pcs_ratio_t  screen_ratio[] = {
      0.1F, 0.1F, 0.1F, 0.1F, 0.1F, 0.1F, 0.1F, 0.0F, 0.2F, 0.1F};
  const pcs_bw_t bw = 10000000;

  PCSTREAM_CHECK_FATAL(pcs_lod_selector_init(&vssl, 1));

  buff = read_file((const char *)argv[1], &size);

  PCSTREAM_CHECK_FATAL(vssl.post(
      &vssl, N_MOD, N_VER, (void *)buff, size, screen_ratio, bw));

  free(buff);
  PCSTREAM_CHECK_FATAL(pcs_lod_selector_destroy(&vssl));
}
