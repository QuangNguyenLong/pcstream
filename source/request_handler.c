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
#include <pcstream/http.h>
#include <pcstream/request_handler.h>
#include <stdio.h>

// for parsing mpd files
#include <gpac/mpd.h>
#include <gpac/xml.h>

#include <stdlib.h>
#include <string.h>
static char *format_template(const char *template,
                             int         adaptation_set_id,
                             const char *representation_id,
                             int         number)
{
  const char *ptr             = template;
  char       *result          = NULL;
  const char *start           = NULL;
  const char *end             = NULL;
  char        format_spec[16] = {0};
  size_t      len             = 0;
  char        final_fmt[32]   = {0};
  char        number_str[64]  = {0};
  size_t      result_len      = 0;

  result                      = malloc(1024); // Adjust if needed
  if (!result)
    return NULL;

  result[0] = '\0';

  while (*ptr)
  {
    if (strncmp(ptr, "$RepresentationID$", 18) == 0)
    {
      strcat(result, representation_id);
      ptr += 18;
    }
    else if (strncmp(ptr, "$AdaptationSetID$", 17) == 0)
    {
      char temp[32];
      sprintf(temp, "%d", adaptation_set_id);
      strcat(result, temp);
      ptr += 17;
    }
    else if (strncmp(ptr, "$Number%", 8) == 0)
    {
      start = ptr + 8;
      end   = strchr(start, '$');
      if (!end)
        break;

      len = (size_t)(end - start);
      strncpy(format_spec, start, len);
      format_spec[len] = '\0';

      sprintf(final_fmt, "%%%s", format_spec);

      sprintf(number_str, final_fmt, number);
      strcat(result, number_str);

      ptr = end + 1;
    }
    else
    {
      result_len             = strlen(result);
      result[result_len]     = *ptr;
      result[result_len + 1] = '\0';
      ptr++;
    }
  }

  return result;
}

static char *merge_url_path(const char *base, const char *path)
{
  size_t len_base   = strlen(base);
  size_t len_path   = strlen(path);

  int    need_slash = (len_base > 0 && base[len_base - 1] != '/') &&
                   (len_path > 0 && path[0] != '/');

  size_t total_len = len_base + len_path + (need_slash ? 1 : 0) + 1;

  char  *result    = malloc(total_len);
  if (!result)
    return NULL;

  strcpy(result, base);

  if (need_slash)
    strcat(result, "/");

  strcat(result, path);
  return result;
}

pcs_ret_t pcs_request_handler_init(pcs_request_handler_t *self,
                                   int                    type)
{
  self->seq_count     = 0;
  self->seg_count     = 0;
  self->rep_count     = 0;
  self->curr_seg      = 0;
  self->bin_mpd       = PCSTREAM_NULL;
  self->base_url      = PCSTREAM_NULL;

  self->info_list     = PCSTREAM_NULL;
  self->hull_list     = PCSTREAM_NULL;
  self->curr_content  = PCSTREAM_NULL;

  self->dl_speeds     = PCSTREAM_NULL;

  self->post_init     = PCSTREAM_NULL;
  self->post_segment  = PCSTREAM_NULL;
  self->get_init      = PCSTREAM_NULL;
  self->get_segment   = PCSTREAM_NULL;
  self->get_dl_speeds = PCSTREAM_NULL;

  switch (type)
  {
  case PCSTREAM_REQUEST_HANDLER_H2:
  {
    self->post_init     = pcs_request_handler_post_init_h2;
    self->post_segment  = pcs_request_handler_post_segment_h2;
    self->get_init      = pcs_request_handler_get_init_h2;
    self->get_segment   = pcs_request_handler_get_segment_h2;
    self->get_dl_speeds = pcs_request_handler_get_dl_speeds_h2;
  }
  break;
  default:
  {
    self->post_init     = pcs_request_handler_post_init_h2;
    self->post_segment  = pcs_request_handler_post_segment_h2;
    self->get_init      = pcs_request_handler_get_init_h2;
    self->get_segment   = pcs_request_handler_get_segment_h2;
    self->get_dl_speeds = pcs_request_handler_get_dl_speeds_h2;
  }
  break;
  }

  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_request_handler_destroy(pcs_request_handler_t *self)
{
  if (self->bin_mpd != PCSTREAM_NULL)
    gf_mpd_del((GF_MPD *)self->bin_mpd);
  if (self->base_url != PCSTREAM_NULL)
    free(self->base_url);
  if (self->info_list != PCSTREAM_NULL)
  {
    for (pcs_count_t i = 0; i < self->seg_count; i++)
      pcs_buffer_destroy(&(self->info_list[i]));
    free(self->info_list);
  }
  if (self->hull_list != PCSTREAM_NULL)
  {
    for (pcs_count_t i = 0; i < self->seq_count; i++)
    {
      for (pcs_count_t j = 0; j < self->seg_count; j++)
        pcs_buffer_destroy(&(self->hull_list[i][j]));
      free(self->hull_list[i]);
    }
    free(self->hull_list);
  }
  if (self->curr_content != PCSTREAM_NULL)
  {
    for (pcs_count_t i = 0; i < self->seg_count; i++)
      pcs_buffer_destroy(&(self->curr_content[i]));
    free(self->curr_content);
  }
  if (self->dl_speeds != PCSTREAM_NULL)
    free(self->dl_speeds);
  self->seq_count     = 0;
  self->seg_count     = 0;
  self->rep_count     = 0;

  self->bin_mpd       = PCSTREAM_NULL;
  self->base_url      = PCSTREAM_NULL;

  self->info_list     = PCSTREAM_NULL;
  self->hull_list     = PCSTREAM_NULL;
  self->curr_content  = PCSTREAM_NULL;
  self->dl_speeds     = PCSTREAM_NULL;

  self->post_init     = PCSTREAM_NULL;
  self->post_segment  = PCSTREAM_NULL;
  self->get_init      = PCSTREAM_NULL;
  self->get_segment   = PCSTREAM_NULL;
  self->get_dl_speeds = PCSTREAM_NULL;
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t
pcs_request_handler_post_init_h2(pcs_request_handler_t *self,
                                 const char            *bin_mpd_url,
                                 const char            *info_mpd_url,
                                 const char            *hull_mpd_url,
                                 const char            *base_url)
{
  GF_XMLNode                  *bin_xml_root  = PCSTREAM_NULL;
  GF_XMLNode                  *info_xml_root = PCSTREAM_NULL;
  GF_XMLNode                  *hull_xml_root = PCSTREAM_NULL;

  pcs_count_t                  n_seg         = 0;
  pcs_lod_version_t            n_ver         = 0;
  pcs_count_t                  n_seq         = 0;

  GF_MPD_Period               *period_ptr    = PCSTREAM_NULL;
  GF_MPD_AdaptationSet        *seq_ptr       = PCSTREAM_NULL;
  GF_MPD_Representation       *rep_ptr       = PCSTREAM_NULL;
  GF_List                     *entries_ptr   = PCSTREAM_NULL;

  GF_MPD_SegmentTimelineEntry *entry_ptr     = PCSTREAM_NULL;

  uint32_t                     entry_count   = 0;
  char                        *media         = PCSTREAM_NULL;
  char                        *init          = PCSTREAM_NULL;

  GF_MPD                      *bin_mpd       = PCSTREAM_NULL;

  // needs to be free
  pcs_buffer_t                 bin_mpd_buf   = {0};
  pcs_buffer_t                 info_mpd_buf  = {0};
  pcs_buffer_t                 hull_mpd_buf  = {0};
  GF_MPD                      *info_mpd      = PCSTREAM_NULL;
  GF_MPD                      *hull_mpd      = PCSTREAM_NULL;
  GF_DOMParser                *bin_parser    = PCSTREAM_NULL;
  GF_DOMParser                *info_parser   = PCSTREAM_NULL;
  GF_DOMParser                *hull_parser   = PCSTREAM_NULL;

  pcs_buffer_init(&bin_mpd_buf);
  pcs_buffer_init(&info_mpd_buf);
  pcs_buffer_init(&hull_mpd_buf);

  bin_mpd     = gf_mpd_new();
  info_mpd    = gf_mpd_new();
  hull_mpd    = gf_mpd_new();

  bin_parser  = gf_xml_dom_new();
  info_parser = gf_xml_dom_new();
  hull_parser = gf_xml_dom_new();

  // get the string
  pcs_http_get_to_buffer(
      bin_mpd_url, PCSTREAM_HTTP_2_0, &bin_mpd_buf, PCSTREAM_NULL);
  pcs_http_get_to_buffer(
      info_mpd_url, PCSTREAM_HTTP_2_0, &info_mpd_buf, PCSTREAM_NULL);
  pcs_http_get_to_buffer(
      hull_mpd_url, PCSTREAM_HTTP_2_0, &hull_mpd_buf, PCSTREAM_NULL);

  // string to xml
  gf_xml_dom_parse_string(bin_parser, bin_mpd_buf.data);
  gf_xml_dom_parse_string(info_parser, info_mpd_buf.data);
  gf_xml_dom_parse_string(hull_parser, hull_mpd_buf.data);

  // get root
  bin_xml_root  = gf_xml_dom_get_root(bin_parser);
  info_xml_root = gf_xml_dom_get_root(info_parser);
  hull_xml_root = gf_xml_dom_get_root(hull_parser);

  // get the mpd
  gf_mpd_init_from_dom(bin_xml_root, bin_mpd, base_url);
  gf_mpd_init_from_dom(info_xml_root, info_mpd, base_url);
  gf_mpd_init_from_dom(hull_xml_root, hull_mpd, base_url);

  self->base_url = (char *)malloc(strlen(base_url) + 1);
  strcpy(self->base_url, base_url);

  self->bin_mpd = (void *)bin_mpd;

  // getting n_seq, n_ver, n_seg ?

  period_ptr = (GF_MPD_Period *)(gf_list_get(bin_mpd->periods, 0));

  n_seq = (pcs_count_t)(gf_list_count(period_ptr->adaptation_sets));

  seq_ptr = (GF_MPD_AdaptationSet *)(gf_list_get(
      period_ptr->adaptation_sets, 0));

  n_ver =
      (pcs_lod_version_t)(gf_list_count(seq_ptr->representations));

  rep_ptr     = (GF_MPD_Representation *)(gf_list_get(
      seq_ptr->representations, 0));

  entries_ptr = rep_ptr->segment_template->segment_timeline->entries;
  entry_count = gf_list_count(entries_ptr);
  for (uint32_t i = 0; i < entry_count; i++)
  {
    entry_ptr =
        (GF_MPD_SegmentTimelineEntry *)(gf_list_get(entries_ptr, i));
    n_seg += 1; // the entry itself
    if (entry_ptr->repeat_count > 0)
      n_seg += (pcs_count_t)(entry_ptr->repeat_count);
  }
  n_seg += 1; // plus the init segment

  self->seq_count = n_seq;
  self->seg_count = n_seg;
  self->rep_count = n_ver;

  // now get the infos
  // first get the string url

  period_ptr = (GF_MPD_Period *)(gf_list_get(info_mpd->periods, 0));
  seq_ptr    = (GF_MPD_AdaptationSet *)(gf_list_get(
      period_ptr->adaptation_sets, 0));
  rep_ptr    = (GF_MPD_Representation *)(gf_list_get(
      seq_ptr->representations, 0));

  media      = rep_ptr->segment_template->media;
  init       = rep_ptr->segment_template->initialization;

  self->info_list =
      (pcs_buffer_t *)malloc(sizeof(pcs_buffer_t) * self->seg_count);
  for (pcs_count_t i = 0; i < self->seq_count; i++)
    pcs_buffer_init(&(self->info_list[i]));

  for (pcs_count_t i = 0; i < self->seg_count; i++)
  {
    char *info_path = PCSTREAM_NULL;
    char *info_url  = PCSTREAM_NULL;
    char *template  = PCSTREAM_NULL;

    template        = i == 0 ? init : media;

    info_path =
        format_template(template, seq_ptr->id, rep_ptr->id, (int)i);
    info_url = merge_url_path(base_url, info_path);

    pcs_http_get_to_buffer(info_url,
                           PCSTREAM_HTTP_2_0,
                           &(self->info_list[i]),
                           PCSTREAM_NULL);

    free(info_url);
    free(info_path);
  }

  // now get the hulls
  period_ptr = (GF_MPD_Period *)(gf_list_get(hull_mpd->periods, 0));

  self->hull_list = (pcs_buffer_t **)malloc(sizeof(pcs_buffer_t *) *
                                            self->seq_count);
  for (pcs_count_t i = 0; i < self->seq_count; i++)
  {
    self->hull_list[i] = (pcs_buffer_t *)malloc(
        sizeof(pcs_buffer_t) * self->seg_count);
    for (pcs_count_t j = 0; j < self->seg_count; j++)
      pcs_buffer_init(&(self->hull_list[i][j]));
  }

  for (pcs_count_t seq = 0; seq < self->seq_count; seq++)
  {
    seq_ptr = (GF_MPD_AdaptationSet *)(gf_list_get(
        period_ptr->adaptation_sets, (unsigned int)seq));
    rep_ptr = (GF_MPD_Representation *)(gf_list_get(
        seq_ptr->representations, 0));

    media   = rep_ptr->segment_template->media;
    init    = rep_ptr->segment_template->initialization;

    for (pcs_count_t seg = 0; seg < self->seg_count; seg++)
    {
      char *hull_path = PCSTREAM_NULL;
      char *hull_url  = PCSTREAM_NULL;
      char *template  = PCSTREAM_NULL;

      template        = seg == 0 ? init : media;

      hull_path       = format_template(
          template, seq_ptr->id, rep_ptr->id, (int)seg);
      hull_url = merge_url_path(base_url, hull_path);

      pcs_http_get_to_buffer(hull_url,
                             PCSTREAM_HTTP_2_0,
                             &(self->hull_list[seq][seg]),
                             PCSTREAM_NULL);

      free(hull_url);
      free(hull_path);
    }
  }

  // init current content binaries buffer (per segment)
  self->curr_content =
      (pcs_buffer_t *)malloc(sizeof(pcs_buffer_t) * self->seq_count);
  for (pcs_count_t i = 0; i < self->seq_count; i++)
    pcs_buffer_init(&(self->curr_content[i]));
  // init dl_speeds
  self->dl_speeds =
      (pcs_bw_t *)malloc(sizeof(pcs_bw_t) * self->seq_count);
  for (pcs_count_t i = 0; i < self->seq_count; i++)
    self->dl_speeds[i] = 0;

  // free stuff
  // gf_mpd_del(bin_mpd);
  pcs_buffer_destroy(&bin_mpd_buf);
  pcs_buffer_destroy(&info_mpd_buf);
  pcs_buffer_destroy(&hull_mpd_buf);

  gf_mpd_del(info_mpd);
  gf_mpd_del(hull_mpd);
  gf_xml_dom_del(bin_parser);
  gf_xml_dom_del(info_parser);
  gf_xml_dom_del(hull_parser);
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t
pcs_request_handler_post_segment_h2(pcs_request_handler_t *self,
                                    pcs_lod_version_t     *selection)
{
  GF_MPD                *mpd_ptr    = PCSTREAM_NULL;
  GF_MPD_Period         *period_ptr = PCSTREAM_NULL;
  GF_MPD_AdaptationSet  *seq_ptr    = PCSTREAM_NULL;
  GF_MPD_Representation *rep_ptr    = PCSTREAM_NULL;
  char                  *media      = PCSTREAM_NULL;
  char                  *init       = PCSTREAM_NULL;
  pcs_count_t            seg        = 0;
  seg                               = self->curr_seg;
  mpd_ptr                           = (GF_MPD *)self->bin_mpd;
  period_ptr = (GF_MPD_Period *)(gf_list_get(mpd_ptr->periods, 0));

  for (pcs_count_t seq = 0; seq < self->seq_count; seq++)
  {
    char *bin_path = PCSTREAM_NULL;
    char *bin_url  = PCSTREAM_NULL;
    char *template = PCSTREAM_NULL;

    seq_ptr        = (GF_MPD_AdaptationSet *)(gf_list_get(
        period_ptr->adaptation_sets, (unsigned int)seq));
    rep_ptr        = (GF_MPD_Representation *)(gf_list_get(
        seq_ptr->representations, selection[seq]));

    media          = rep_ptr->segment_template->media;
    init           = rep_ptr->segment_template->initialization;

    template       = seg == 0 ? init : media;
    bin_path       = format_template(
        template, seq_ptr->id, rep_ptr->id, (int)seg);
    bin_url = merge_url_path(self->base_url, bin_path);

    pcs_buffer_destroy(&(self->curr_content[seq]));

    pcs_http_get_to_buffer(bin_url,
                           PCSTREAM_HTTP_2_0,
                           &(self->curr_content[seq]),
                           &(self->dl_speeds[seq]));

    free(bin_url);
    free(bin_path);
  }
  self->curr_seg++;
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t
pcs_request_handler_get_init_h2(pcs_request_handler_t *self,
                                pcs_buffer_t         **info_list_ptr,
                                pcs_buffer_t        ***hull_list_ptr)
{
  if (self->info_list == PCSTREAM_NULL ||
      self->hull_list == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;
  *info_list_ptr = self->info_list;
  *hull_list_ptr = self->hull_list;
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t
pcs_request_handler_get_segment_h2(pcs_request_handler_t *self,
                                   pcs_buffer_t **curr_content_ptr)
{
  if (self->curr_content == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;
  *curr_content_ptr = self->curr_content;
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t
pcs_request_handler_get_dl_speeds_h2(pcs_request_handler_t *self,
                                     pcs_bw_t **dl_speeds_ptr)
{
  if (self->dl_speeds == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;
  *dl_speeds_ptr = self->dl_speeds;
  return PCSTREAM_RET_SUCCESS;
}
