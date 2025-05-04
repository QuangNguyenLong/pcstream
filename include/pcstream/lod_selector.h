#ifndef PCSTREAM_LOD_SELECTOR_H
#define PCSTREAM_LOD_SELECTOR_H

#include <pcstream/def.h>
typedef struct pcs_lod_selector_t pcs_lod_selector_t;

struct pcs_lod_selector_t
{
  PCSTREAM_LOD_VERSION *v;
  PCSTREAM_COUNT        n_ver;
  PCSTREAM_COUNT        n_mod;

  PCSTREAM_RET(*post)
  (pcs_lod_selector_t *,
   PCSTREAM_COUNT,
   PCSTREAM_LOD_VERSION,
   void *,
   size_t,
   void *,
   PCSTREAM_BW);
  PCSTREAM_RET(*get)
  (pcs_lod_selector_t *, PCSTREAM_LOD_VERSION **);
};

PCSTREAM_RET
pcs_lod_selector_init(pcs_lod_selector_t *self, int type);
PCSTREAM_RET
pcs_lod_selector_destroy(pcs_lod_selector_t *self);

PCSTREAM_RET
pcs_lod_selector_post_dp_based(pcs_lod_selector_t *self,
                               PCSTREAM_COUNT       n_mod,
                               PCSTREAM_LOD_VERSION n_ver,
                               void                *metadata,
                               size_t               metadata_size,
                               void                *attrib,
                               PCSTREAM_BW          bw);
PCSTREAM_RET
pcs_lod_selector_post_lm_based(pcs_lod_selector_t *self,
                               PCSTREAM_COUNT       n_mod,
                               PCSTREAM_LOD_VERSION n_ver,
                               void                *metadata,
                               size_t               metadata_size,
                               void                *attrib,
                               PCSTREAM_BW          bw);
PCSTREAM_RET
pcs_lod_selector_post_equal(pcs_lod_selector_t *self,
                            PCSTREAM_COUNT       n_mod,
                            PCSTREAM_LOD_VERSION n_ver,
                            void                *metadata,
                            size_t               metadata_size,
                            void                *attrib,
                            PCSTREAM_BW          bw);
PCSTREAM_RET
pcs_lod_selector_post_hybrid(pcs_lod_selector_t *self,
                             PCSTREAM_COUNT       n_mod,
                             PCSTREAM_LOD_VERSION n_ver,
                             void                *metadata,
                             size_t               metadata_size,
                             void                *attrib,
                             PCSTREAM_BW          bw);

PCSTREAM_RET
pcs_lod_selector_get_dp_based(pcs_lod_selector_t *self,
                              PCSTREAM_LOD_VERSION **selections_ptr);
PCSTREAM_RET
pcs_lod_selector_get_lm_based(pcs_lod_selector_t *self,
                              PCSTREAM_LOD_VERSION **selections_ptr);
PCSTREAM_RET
pcs_lod_selector_get_equal(pcs_lod_selector_t *self,
                           PCSTREAM_LOD_VERSION **selections_ptr);
PCSTREAM_RET
pcs_lod_selector_get_hybrid(pcs_lod_selector_t *self,
                            PCSTREAM_LOD_VERSION **selections_ptr);

#endif
