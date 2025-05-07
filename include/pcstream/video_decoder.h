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
#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <pcstream/def.h>
#include <pcstream/gof.h>

#ifdef __cplusplus
extern "C"
{
#endif
  typedef struct pcs_video_decoder_t pcs_video_decoder_t;

  struct pcs_video_decoder_t
  {
    pcs_gof_t reconstruct;
    pcs_ret_t (*post)(pcs_video_decoder_t *,
                      const char *,
                      const size_t);
    pcs_ret_t (*get)(pcs_video_decoder_t *, pcs_gof_t *);
  };

  PCSTREAM_EXPORT pcs_ret_t
  pcs_video_decoder_init(pcs_video_decoder_t *self, int type);
  PCSTREAM_EXPORT pcs_ret_t
  pcs_video_decoder_destroy(pcs_video_decoder_t *self);

#if defined(USE_MPEG_VPCC_CODEC)
  pcs_ret_t
  pcs_video_decoder_post_mpeg_vpcc(pcs_video_decoder_t *self,
                                   const char          *data,
                                   const size_t         size);
  pcs_ret_t
  pcs_video_decoder_get_mpeg_vpcc(pcs_video_decoder_t *self,
                                  pcs_gof_t           *reconstruct);
#endif

#if defined(USE_GOOGLE_DRACO_CODEC)
  pcs_ret_t
  pcs_video_decoder_post_google_draco(pcs_video_decoder_t *self,
                                      const char          *data,
                                      const size_t         size);
  pcs_ret_t
  pcs_video_decoder_get_google_draco(pcs_video_decoder_t *self,
                                     pcs_gof_t *reconstruct);
#endif

#ifdef __cplusplus
}
#endif
#endif
