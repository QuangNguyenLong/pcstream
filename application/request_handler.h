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
#ifndef PCSTREAM_REQUEST_HANDLER_H
#define PCSTREAM_REQUEST_HANDLER_H

#include <pcstream/buffer.h>
#include <pcstream/def.h>

typedef struct pcs_request_handler_t pcs_request_handler_t;

struct pcs_request_handler_t
{
  // stuff should be replaced by bin_mpd
  pcs_count_t       seq_count;
  pcs_lod_version_t rep_count;
  pcs_count_t       seg_count;

  pcs_count_t       curr_seg;
  // stuff needs to be free on destroy
  // bin_mpd
  void             *bin_mpd; // GF_MPD

  char             *base_url;
  pcs_buffer_t     *info_list; // size = seg_count
  pcs_buffer_t    **hull_list; // size = seq_count * seg_count

  pcs_buffer_t     *curr_content; // size = seq_count

  pcs_bw_t         *dl_speeds; // in bytes/second, size = seq_count

  pcs_ret_t (*post_init)(pcs_request_handler_t *,
                         const char *,
                         const char *,
                         const char *,
                         const char *);
  pcs_ret_t (*post_segment)(pcs_request_handler_t *,
                            pcs_lod_version_t *);
  pcs_ret_t (*get_init)(pcs_request_handler_t *,
                        pcs_buffer_t  **info_list_ptr,
                        pcs_buffer_t ***hull_list_ptr);
  pcs_ret_t (*get_segment)(pcs_request_handler_t *,
                           pcs_buffer_t **curr_content_ptr);
  pcs_ret_t (*get_dl_speeds)(pcs_request_handler_t *, pcs_bw_t **);
};

PCSTREAM_EXPORT pcs_ret_t
pcs_request_handler_init(pcs_request_handler_t *self, int type);
PCSTREAM_EXPORT pcs_ret_t
pcs_request_handler_destroy(pcs_request_handler_t *self);

pcs_ret_t
pcs_request_handler_post_init_h2(pcs_request_handler_t *self,
                                 const char            *bin_mpd_url,
                                 const char            *info_mpd_url,
                                 const char            *hull_mpd_url,
                                 const char            *base_url);

pcs_ret_t
pcs_request_handler_post_segment_h2(pcs_request_handler_t *self,
                                    pcs_lod_version_t *selection);

pcs_ret_t
pcs_request_handler_get_init_h2(pcs_request_handler_t *self,
                                pcs_buffer_t         **info_list_ptr,
                                pcs_buffer_t ***hull_list_ptr);
pcs_ret_t
pcs_request_handler_get_segment_h2(pcs_request_handler_t *self,
                                   pcs_buffer_t **curr_content_ptr);

pcs_ret_t
pcs_request_handler_get_dl_speeds_h2(pcs_request_handler_t *self,
                                     pcs_bw_t **dl_speeds_ptr);

#endif
