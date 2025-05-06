#ifndef PCSTREAM_POINT_CLOUD_H
#define PCSTREAM_POINT_CLOUD_H

#include <pcstream/def.h>
#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct pcs_point_cloud_t pcs_point_cloud_t;

  struct pcs_point_cloud_t
  {

    // len = 3 * size *
    // pcs_type_size_g[pcs_pos_type_g[PCSTREAM_VIDEO_DECOER]]
    void          *positions;
    unsigned char *colors; // len = 3 * size
    // float         *normals;
    size_t         size; // number of points
    pcs_ret_t (*copy)(pcs_point_cloud_t *,
                      void *,
                      PCSTREAM_VIDEO_DECODER);
  };

  PCSTREAM_EXPORT pcs_ret_t
  pcs_point_cloud_init(pcs_point_cloud_t *self);
  PCSTREAM_EXPORT pcs_ret_t
            pcs_point_cloud_destroy(pcs_point_cloud_t *self);
  pcs_ret_t pcs_point_cloud_copy(pcs_point_cloud_t     *self,
                                 void                  *src,
                                 PCSTREAM_VIDEO_DECODER decoder);

#ifdef __cplusplus
}
#endif
#endif
