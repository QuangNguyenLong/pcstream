#ifndef PCSTREAM_GOF_H
#define PCSTREAM_GOF_H

#include <pcstream/def.h>
#include <pcstream/point_cloud.h>

#ifdef __cplusplus
extern "C"
{
#endif
  typedef struct pcs_gof_t pcs_gof_t;

  struct pcs_gof_t
  {
    pcs_point_cloud_t *frames;
    size_t             size;
    PCSTREAM_RET (*copy)(pcs_gof_t *, void *, PCSTREAM_VIDEO_DECODER);
  };

  PCSTREAM_RET pcs_gof_init(pcs_gof_t *self);
  PCSTREAM_RET pcs_gof_destroy(pcs_gof_t *self);
  PCSTREAM_RET
  pcs_gof_copy(pcs_gof_t             *self,
               void                  *src,
               PCSTREAM_VIDEO_DECODER decoder);

#ifdef __cplusplus
}
#endif
#endif
