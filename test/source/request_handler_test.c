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
#include <pcstream/request_handler.h>

int main(int argc, char **argv)
{
  pcs_request_handler_t hand             = {0};
  pcs_buffer_t         *info_list_ptr    = PCSTREAM_NULL;
  pcs_buffer_t        **hull_list_ptr    = PCSTREAM_NULL;
  pcs_buffer_t         *curr_content_ptr = PCSTREAM_NULL;

  pcs_lod_version_t    *selects          = PCSTREAM_NULL;

  PCSTREAM_CHECK_FATAL(pcs_request_handler_init(&hand, 1));

  PCSTREAM_CHECK_FATAL(
      hand.post_init(&hand, argv[1], argv[2], argv[3], argv[4]));

  PCSTREAM_CHECK_FATAL(
      hand.get_init(&hand, &info_list_ptr, &hull_list_ptr));

  selects = (pcs_lod_version_t *)malloc(sizeof(pcs_lod_version_t) *
                                        hand.seq_count);
  for (pcs_count_t i = 0; i < hand.seq_count; i++)
  {
    selects[i] = 0;
  }
  PCSTREAM_CHECK_FATAL(hand.post_segment(&hand, selects));
  PCSTREAM_CHECK_FATAL(hand.get_segment(&hand, &curr_content_ptr));

  free(selects);
  PCSTREAM_CHECK_FATAL(pcs_request_handler_destroy(&hand));
}
