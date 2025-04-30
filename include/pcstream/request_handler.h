#ifndef PCSTREAM_REQUEST_HANDLER_H
#define PCSTREAM_REQUEST_HANDLER_H

#include <pcstream/buffer.h>
#include <pcstream/def.h>

enum
{
  PCSTREAM_REQUEST_HANDLER_H2 = 1
};

typedef struct pcs_request_handler_t pcs_request_handler_t;

struct pcs_request_handler_t
{
  // stuff should be replaced by bin_mpd
  PCSTREAM_COUNT       seq_count;
  PCSTREAM_LOD_VERSION rep_count;
  PCSTREAM_COUNT       seg_count;

  PCSTREAM_COUNT       curr_seg;
  // stuff needs to be free on destroy
  // bin_mpd
  void              *bin_mpd; // GF_MPD

  char                *base_url;
  pcs_buffer_t        *info_list; // size = seg_count
  pcs_buffer_t       **hull_list; // size = seq_count * seg_count

  pcs_buffer_t        *curr_content; // size = seq_count

  PCSTREAM_BW         *dl_speeds; // in bytes/second, size = seq_count

  PCSTREAM_RET(*post_init)
  (pcs_request_handler_t *,
   const char *,
   const char *,
   const char *,
   const char *);
  PCSTREAM_RET(*post_segment)
  (pcs_request_handler_t *, PCSTREAM_LOD_VERSION *);
  PCSTREAM_RET(*get_init)
  (pcs_request_handler_t *,
   pcs_buffer_t  **info_list_ptr,
   pcs_buffer_t ***hull_list_ptr);
  PCSTREAM_RET(*get_segment)
  (pcs_request_handler_t *, pcs_buffer_t **curr_content_ptr);
  PCSTREAM_RET (*get_dl_speeds)(pcs_request_handler_t *, PCSTREAM_BW **);
};

PCSTREAM_RET pcs_request_handler_init(pcs_request_handler_t *this, int type);
PCSTREAM_RET pcs_request_handler_destroy(pcs_request_handler_t *this);

PCSTREAM_RET pcs_request_handler_post_init_h2(pcs_request_handler_t *this,
                                          const char *bin_mpd_url,
                                          const char *info_mpd_url,
                                          const char *hull_mpd_url,
                                          const char *base_url);

PCSTREAM_RET
pcs_request_handler_post_segment_h2(pcs_request_handler_t *this,
                                PCSTREAM_LOD_VERSION *selection);

PCSTREAM_RET
pcs_request_handler_get_init_h2(pcs_request_handler_t *this,
                            pcs_buffer_t  **info_list_ptr,
                            pcs_buffer_t ***hull_list_ptr);
PCSTREAM_RET
pcs_request_handler_get_segment_h2(pcs_request_handler_t *this,
                               pcs_buffer_t **curr_content_ptr);

PCSTREAM_RET
pcs_request_handler_get_dl_speeds_h2(pcs_request_handler_t *this,
                                 PCSTREAM_BW **dl_speeds_ptr);

#endif
