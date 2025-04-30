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

PCSTREAM_RET pcs_request_handler_init(pcs_request_handler_t *this,
                                      int type)
{
  this->seq_count     = 0;
  this->seg_count     = 0;
  this->rep_count     = 0;
  this->curr_seg      = 0;
  this->bin_mpd       = PCSTREAM_NULL;
  this->base_url      = PCSTREAM_NULL;

  this->info_list     = PCSTREAM_NULL;
  this->hull_list     = PCSTREAM_NULL;
  this->curr_content  = PCSTREAM_NULL;

  this->dl_speeds     = PCSTREAM_NULL;

  this->post_init     = PCSTREAM_NULL;
  this->post_segment  = PCSTREAM_NULL;
  this->get_init      = PCSTREAM_NULL;
  this->get_segment   = PCSTREAM_NULL;
  this->get_dl_speeds = PCSTREAM_NULL;

  switch (type)
  {
  case PCSTREAM_REQUEST_HANDLER_H2:
  {
    this->post_init     = pcs_request_handler_post_init_h2;
    this->post_segment  = pcs_request_handler_post_segment_h2;
    this->get_init      = pcs_request_handler_get_init_h2;
    this->get_segment   = pcs_request_handler_get_segment_h2;
    this->get_dl_speeds = pcs_request_handler_get_dl_speeds_h2;
  }
  break;
  default:
  {
    this->post_init     = pcs_request_handler_post_init_h2;
    this->post_segment  = pcs_request_handler_post_segment_h2;
    this->get_init      = pcs_request_handler_get_init_h2;
    this->get_segment   = pcs_request_handler_get_segment_h2;
    this->get_dl_speeds = pcs_request_handler_get_dl_speeds_h2;
  }
  break;
  }

  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET pcs_request_handler_destroy(pcs_request_handler_t *this)
{
  if (this->bin_mpd != PCSTREAM_NULL)
    gf_mpd_del((GF_MPD *)this->bin_mpd);
  if (this->base_url != PCSTREAM_NULL)
    free(this->base_url);
  if (this->info_list != PCSTREAM_NULL)
  {
    for (PCSTREAM_COUNT i = 0; i < this->seg_count; i++)
      pcs_buffer_destroy(&(this->info_list[i]));
    free(this->info_list);
  }
  if (this->hull_list != PCSTREAM_NULL)
  {
    for (PCSTREAM_COUNT i = 0; i < this->seq_count; i++)
    {
      for (PCSTREAM_COUNT j = 0; j < this->seg_count; j++)
        pcs_buffer_destroy(&(this->hull_list[i][j]));
      free(this->hull_list[i]);
    }
    free(this->hull_list);
  }
  if (this->curr_content != PCSTREAM_NULL)
  {
    for (PCSTREAM_COUNT i = 0; i < this->seg_count; i++)
      pcs_buffer_destroy(&(this->curr_content[i]));
    free(this->curr_content);
  }
  if (this->dl_speeds != PCSTREAM_NULL)
    free(this->dl_speeds);
  this->seq_count     = 0;
  this->seg_count     = 0;
  this->rep_count     = 0;

  this->bin_mpd       = PCSTREAM_NULL;
  this->base_url      = PCSTREAM_NULL;

  this->info_list     = PCSTREAM_NULL;
  this->hull_list     = PCSTREAM_NULL;
  this->curr_content  = PCSTREAM_NULL;
  this->dl_speeds     = PCSTREAM_NULL;

  this->post_init     = PCSTREAM_NULL;
  this->post_segment  = PCSTREAM_NULL;
  this->get_init      = PCSTREAM_NULL;
  this->get_segment   = PCSTREAM_NULL;
  this->get_dl_speeds = PCSTREAM_NULL;
  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET
pcs_request_handler_post_init_h2(pcs_request_handler_t *this,
                                 const char *bin_mpd_url,
                                 const char *info_mpd_url,
                                 const char *hull_mpd_url,
                                 const char *base_url)
{
  GF_XMLNode                  *bin_xml_root  = PCSTREAM_NULL;
  GF_XMLNode                  *info_xml_root = PCSTREAM_NULL;
  GF_XMLNode                  *hull_xml_root = PCSTREAM_NULL;

  PCSTREAM_COUNT               n_seg         = 0;
  PCSTREAM_LOD_VERSION         n_ver         = 0;
  PCSTREAM_COUNT               n_seq         = 0;

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

  this->base_url = (char *)malloc(strlen(base_url) + 1);
  strcpy(this->base_url, base_url);

  this->bin_mpd = (void *)bin_mpd;

  // getting n_seq, n_ver, n_seg ?

  period_ptr    = (GF_MPD_Period *)(gf_list_get(bin_mpd->periods, 0));

  n_seq =
      (PCSTREAM_COUNT)(gf_list_count(period_ptr->adaptation_sets));

  seq_ptr = (GF_MPD_AdaptationSet *)(gf_list_get(
      period_ptr->adaptation_sets, 0));

  n_ver =
      (PCSTREAM_LOD_VERSION)(gf_list_count(seq_ptr->representations));

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
      n_seg += (PCSTREAM_COUNT)(entry_ptr->repeat_count);
  }
  n_seg += 1; // plus the init segment

  this->seq_count = n_seq;
  this->seg_count = n_seg;
  this->rep_count = n_ver;

  // now get the infos
  // first get the string url

  period_ptr = (GF_MPD_Period *)(gf_list_get(info_mpd->periods, 0));
  seq_ptr    = (GF_MPD_AdaptationSet *)(gf_list_get(
      period_ptr->adaptation_sets, 0));
  rep_ptr    = (GF_MPD_Representation *)(gf_list_get(
      seq_ptr->representations, 0));

  media      = rep_ptr->segment_template->media;
  init       = rep_ptr->segment_template->initialization;

  this->info_list =
      (pcs_buffer_t *)malloc(sizeof(pcs_buffer_t) * this->seg_count);
  for (PCSTREAM_COUNT i = 0; i < this->seq_count; i++)
    pcs_buffer_init(&(this->info_list[i]));

  for (PCSTREAM_COUNT i = 0; i < this->seg_count; i++)
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
                           &(this->info_list[i]),
                           PCSTREAM_NULL);

    free(info_url);
    free(info_path);
  }

  // now get the hulls
  period_ptr = (GF_MPD_Period *)(gf_list_get(hull_mpd->periods, 0));

  this->hull_list = (pcs_buffer_t **)malloc(sizeof(pcs_buffer_t *) *
                                            this->seq_count);
  for (PCSTREAM_COUNT i = 0; i < this->seq_count; i++)
  {
    this->hull_list[i] = (pcs_buffer_t *)malloc(sizeof(pcs_buffer_t) *
                                                this->seg_count);
    for (PCSTREAM_COUNT j = 0; j < this->seg_count; j++)
      pcs_buffer_init(&(this->hull_list[i][j]));
  }

  for (PCSTREAM_COUNT seq = 0; seq < this->seq_count; seq++)
  {
    seq_ptr = (GF_MPD_AdaptationSet *)(gf_list_get(
        period_ptr->adaptation_sets, (unsigned int)seq));
    rep_ptr = (GF_MPD_Representation *)(gf_list_get(
        seq_ptr->representations, 0));

    media   = rep_ptr->segment_template->media;
    init    = rep_ptr->segment_template->initialization;

    for (PCSTREAM_COUNT seg = 0; seg < this->seg_count; seg++)
    {
      char *hull_path = PCSTREAM_NULL;
      char *hull_url  = PCSTREAM_NULL;
      char *template  = PCSTREAM_NULL;

      template        = seg == 0 ? init : media;

      hull_path =
          format_template(template, seq_ptr->id, rep_ptr->id, (int)seg);
      hull_url = merge_url_path(base_url, hull_path);

      pcs_http_get_to_buffer(hull_url,
                             PCSTREAM_HTTP_2_0,
                             &(this->hull_list[seq][seg]),
                             PCSTREAM_NULL);

      free(hull_url);
      free(hull_path);
    }
  }

  // init current content binaries buffer (per segment)
  this->curr_content =
      (pcs_buffer_t *)malloc(sizeof(pcs_buffer_t) * this->seq_count);
  for (PCSTREAM_COUNT i = 0; i < this->seq_count; i++)
    pcs_buffer_init(&(this->curr_content[i]));
  // init dl_speeds
  this->dl_speeds =
      (PCSTREAM_BW *)malloc(sizeof(PCSTREAM_BW) * this->seq_count);
  for (PCSTREAM_COUNT i = 0; i < this->seq_count; i++)
    this->dl_speeds[i] = 0;

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

PCSTREAM_RET
pcs_request_handler_post_segment_h2(pcs_request_handler_t *this,
                                    PCSTREAM_LOD_VERSION *selection)
{
  GF_MPD                *mpd_ptr    = PCSTREAM_NULL;
  GF_MPD_Period         *period_ptr = PCSTREAM_NULL;
  GF_MPD_AdaptationSet  *seq_ptr    = PCSTREAM_NULL;
  GF_MPD_Representation *rep_ptr    = PCSTREAM_NULL;
  char                  *media      = PCSTREAM_NULL;
  char                  *init       = PCSTREAM_NULL;
  PCSTREAM_COUNT         seg        = 0;
  seg                               = this->curr_seg;
  mpd_ptr                           = (GF_MPD *)this->bin_mpd;
  period_ptr = (GF_MPD_Period *)(gf_list_get(mpd_ptr->periods, 0));

  for (PCSTREAM_COUNT seq = 0; seq < this->seq_count; seq++)
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
    bin_path =
        format_template(template, seq_ptr->id, rep_ptr->id, (int)seg);
    bin_url = merge_url_path(this->base_url, bin_path);

    pcs_buffer_destroy(&(this->curr_content[seq]));

    pcs_http_get_to_buffer(bin_url,
                           PCSTREAM_HTTP_2_0,
                           &(this->curr_content[seq]),
                           &(this->dl_speeds[seq]));

    free(bin_url);
    free(bin_path);
  }
  this->curr_seg++;
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_request_handler_get_init_h2(pcs_request_handler_t *this,
                                pcs_buffer_t  **info_list_ptr,
                                pcs_buffer_t ***hull_list_ptr)
{
  if (this->info_list == PCSTREAM_NULL ||
      this->hull_list == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;
  *info_list_ptr = this->info_list;
  *hull_list_ptr = this->hull_list;
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_request_handler_get_segment_h2(pcs_request_handler_t *this,
                                   pcs_buffer_t **curr_content_ptr)
{
  if (this->curr_content == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;
  *curr_content_ptr = this->curr_content;
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_request_handler_get_dl_speeds_h2(pcs_request_handler_t *this,
                                     PCSTREAM_BW **dl_speeds_ptr)
{
  if (this->dl_speeds == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;
  *dl_speeds_ptr = this->dl_speeds;
  return PCSTREAM_RET_SUCCESS;
}
