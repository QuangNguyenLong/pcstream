#ifndef PCSTREAM_GOF_H
#define PCSTREAM_GOF_H

#include <pcstream/def.h>
#include <pcstream/point_cloud.h>

#ifdef __cplusplus
extern "C"
{
#endif
  typedef struct pcs_gof_t pcs_gof_t;

  /**
   * @struct pcs_gof_t
   * @brief Structure represents a group of frames.
   * @details This structure holds a collection of frames and
   * provides functions to manage and manipulate them. It includes
   * the frames themselves (`pcs_point_cloud_t`) and the size of the
   * group.
   */
  struct pcs_gof_t
  {
    /** @brief Pointer to the array of frames (pcs_point_cloud_t) in
     * the group. */
    pcs_point_cloud_t *frames;
    size_t             size;
    pcs_ret_t (*copy)(pcs_gof_t *, void *, PCSTREAM_VIDEO_DECODER);
  };

  PCSTREAM_EXPORT pcs_ret_t pcs_gof_init(pcs_gof_t *self);
  PCSTREAM_EXPORT pcs_ret_t pcs_gof_destroy(pcs_gof_t *self);
  pcs_ret_t                 pcs_gof_copy(pcs_gof_t             *self,
                                         void                  *src,
                                         PCSTREAM_VIDEO_DECODER decoder);

#ifdef __cplusplus
}
#endif
#endif
