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
#ifndef PCSTREAM_ALGORITHM_H
#define PCSTREAM_ALGORITHM_H

#include <pcstream/def.h>

PCSTREAM_EXPORT pcs_ret_t
                          pcs_dp_based_solution(pcs_count_t        n_mod,
                                                pcs_count_t        n_ver,
                                                void              *metadata,
                                                size_t             metadata_size,
                                                pcs_ratio_t       *screen_ratio,
                                                pcs_bw_t           bandwidth,
                                                pcs_lod_version_t *selection);
PCSTREAM_EXPORT pcs_ret_t pcs_lm_based_solution(void);
PCSTREAM_EXPORT pcs_ret_t pcs_hybrid_solution(void);
PCSTREAM_EXPORT pcs_ret_t pcs_equal_solution(void);

#endif
