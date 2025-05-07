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
#include <pcstream/http.h>
#include <pcstream/video_decoder.h>
int main(int argc, char **argv)
{
  if (argc < 2)
  {
    return -1;
  }
  pcs_buffer_t        buff = {0};
  pcs_gof_t           rec  = {0};
  pcs_video_decoder_t dcr  = {0};

  pcs_gof_init(&rec);
  pcs_buffer_init(&buff);
  pcs_video_decoder_init(&dcr, PCSTREAM_VIDEO_DECODER_MPEG_VPCC);

  pcs_http_get_to_buffer((const char *)argv[1],
                         PCSTREAM_HTTP_2_0,
                         &buff,
                         PCSTREAM_NULL);

  dcr.post(&dcr, buff.data, buff.size);
  dcr.get(&dcr, &rec);

  pcs_buffer_destroy(&buff);
  pcs_gof_destroy(&rec);
  return 0;
}
