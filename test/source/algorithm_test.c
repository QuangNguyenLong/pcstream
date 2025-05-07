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
#include <pcstream/algorithm.h>
#include <stdlib.h>

#define N_MOD      10
#define N_VER      4
#define CURRENT_BW 50000000 // Bytes/s -> 50Mbps
int main(int argc, char **argv)
{
  if (argc < 2)
  {
    return -1;
  }
  pcs_count_t       n_mod            = 0;
  pcs_count_t       n_ver            = 0;
  char             *metadata         = PCSTREAM_NULL;
  size_t            metadata_size    = 0;
  pcs_ratio_t       screen_ratio[]   = {0.1F,
                                        0.2F,
                                        0.05F,
                                        0.05F,
                                        0.05F,
                                        0.05F,
                                        0.0F,
                                        0.0F,
                                        0.2F,
                                        0.3F};
  pcs_bw_t          bw               = 0;
  pcs_lod_version_t selection[N_MOD] = {0};

  n_mod                              = N_MOD;
  n_ver                              = N_VER;
  bw                                 = CURRENT_BW;

  metadata = read_file((const char *)argv[1], &metadata_size);

  PCSTREAM_CHECK_FATAL(pcs_dp_based_solution(n_mod,
                                             n_ver,
                                             (void *)metadata,
                                             metadata_size,
                                             screen_ratio,
                                             bw,
                                             selection));
  free(metadata);
  return 0;
}
