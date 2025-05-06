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
