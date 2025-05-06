#ifndef PCSTREAM_LOD_SELECTOR_H
#define PCSTREAM_LOD_SELECTOR_H

#include <pcstream/def.h>
typedef struct pcs_lod_selector_t pcs_lod_selector_t;

struct pcs_lod_selector_t
{
  pcs_lod_version_t *v;
  pcs_count_t        n_ver;
  pcs_count_t        n_mod;

  pcs_ret_t (*post)(pcs_lod_selector_t *,
                    pcs_count_t,
                    pcs_lod_version_t,
                    void *,
                    size_t,
                    void *,
                    pcs_bw_t);
  pcs_ret_t (*get)(pcs_lod_selector_t *, pcs_lod_version_t **);
};

PCSTREAM_EXPORT pcs_ret_t
pcs_lod_selector_init(pcs_lod_selector_t *self, int type);
PCSTREAM_EXPORT pcs_ret_t
          pcs_lod_selector_destroy(pcs_lod_selector_t *self);

pcs_ret_t pcs_lod_selector_post_dp_based(pcs_lod_selector_t *self,
                                         pcs_count_t         n_mod,
                                         pcs_lod_version_t   n_ver,
                                         void    *metadata,
                                         size_t   metadata_size,
                                         void    *attrib,
                                         pcs_bw_t bandwidth);
pcs_ret_t pcs_lod_selector_post_lm_based(pcs_lod_selector_t *self,
                                         pcs_count_t         n_mod,
                                         pcs_lod_version_t   n_ver,
                                         void    *metadata,
                                         size_t   metadata_size,
                                         void    *attrib,
                                         pcs_bw_t bandwidth);
pcs_ret_t pcs_lod_selector_post_equal(pcs_lod_selector_t *self,
                                      pcs_count_t         n_mod,
                                      pcs_lod_version_t   n_ver,
                                      void               *metadata,
                                      size_t   metadata_size,
                                      void    *attrib,
                                      pcs_bw_t bandwidth);
pcs_ret_t pcs_lod_selector_post_hybrid(pcs_lod_selector_t *self,
                                       pcs_count_t         n_mod,
                                       pcs_lod_version_t   n_ver,
                                       void               *metadata,
                                       size_t   metadata_size,
                                       void    *attrib,
                                       pcs_bw_t bandwidth);

pcs_ret_t
pcs_lod_selector_get_dp_based(pcs_lod_selector_t *self,
                              pcs_lod_version_t **selections_ptr);
pcs_ret_t
pcs_lod_selector_get_lm_based(pcs_lod_selector_t *self,
                              pcs_lod_version_t **selections_ptr);
pcs_ret_t
pcs_lod_selector_get_equal(pcs_lod_selector_t *self,
                           pcs_lod_version_t **selections_ptr);
pcs_ret_t
pcs_lod_selector_get_hybrid(pcs_lod_selector_t *self,
                            pcs_lod_version_t **selections_ptr);

#endif
