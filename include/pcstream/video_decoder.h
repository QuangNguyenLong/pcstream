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
    PCSTREAM_RET(*post)
    (pcs_video_decoder_t *, const char *, const size_t);
    PCSTREAM_RET (*get)(pcs_video_decoder_t *, pcs_gof_t *);
  };

  PCSTREAM_EXPORT PCSTREAM_RET
  pcs_video_decoder_init(pcs_video_decoder_t *self, int type);
  PCSTREAM_EXPORT PCSTREAM_RET
  pcs_video_decoder_destroy(pcs_video_decoder_t *self);

#if defined(USE_MPEG_VPCC_CODEC)
  PCSTREAM_RET
  pcs_video_decoder_post_mpeg_vpcc(pcs_video_decoder_t *self,
                                   const char          *data,
                                   const size_t         size);
  PCSTREAM_RET
  pcs_video_decoder_get_mpeg_vpcc(pcs_video_decoder_t *self,
                                  pcs_gof_t           *reconstruct);
#endif

#if defined(USE_GOOGLE_DRACO_CODEC)
  PCSTREAM_RET
  pcs_video_decoder_post_google_draco(pcs_video_decoder_t *self,
                                      const char          *data,
                                      const size_t         size);
  PCSTREAM_RET
  pcs_video_decoder_get_google_draco(pcs_video_decoder_t *self,
                                     pcs_gof_t *reconstruct);
#endif

#ifdef __cplusplus
}
#endif
#endif
