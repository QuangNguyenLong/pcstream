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
#include <pcstream/bw_estimator.h>

int main(void)
{
  pcs_bw_t           dles   = 0;
  pcs_bw_t           rdls[] = {1000000, 2000000, 6000000}; // Bytes/s
  pcs_bw_estimator_t bwes   = {0};

  PCSTREAM_CHECK_FATAL(
      pcs_bw_estimator_init(&bwes, PCSTREAM_BW_ESTIMATOR_HARMONIC));

  PCSTREAM_CHECK_FATAL(
      bwes.post(&bwes, rdls, sizeof(rdls) / sizeof(rdls[0])));
  PCSTREAM_CHECK_FATAL(bwes.get(&bwes, &dles));
  PCSTREAM_CHECK_FATAL(pcs_bw_estimator_destroy(&bwes));
  return 0;
}
