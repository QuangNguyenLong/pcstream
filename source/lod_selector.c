#include <pcstream/algorithm.h>
#include <pcstream/lod_selector.h>
#include <stdlib.h>

PCSTREAM_RET
pcs_lod_selector_init(pcs_lod_selector_t *this, int type)
{
  this->n_ver = 0;
  this->n_mod = 0;

  this->v     = PCSTREAM_NULL;

  switch (type)
  {
  case PCSTREAM_LOD_SELECTOR_DP_BASED:
  {
    this->post = pcs_lod_selector_post_dp_based;
    this->get  = pcs_lod_selector_get_dp_based;
  }
  break;
  case PCSTREAM_LOD_SELECTOR_LM_BASED:
  {
    this->post = pcs_lod_selector_post_lm_based;
    this->get  = pcs_lod_selector_get_lm_based;
  }
  break;
  case PCSTREAM_LOD_SELECTOR_EQUAL:
  {
    this->post = pcs_lod_selector_post_equal;
    this->get  = pcs_lod_selector_get_equal;
  }
  break;
  case PCSTREAM_LOD_SELECTOR_HYBRID:
  {
    this->post = pcs_lod_selector_post_hybrid;
    this->get  = pcs_lod_selector_get_hybrid;
  }
  break;
  default:
  {
    this->post = pcs_lod_selector_post_lm_based;
    this->get  = pcs_lod_selector_get_lm_based;
  }
  break;
  }
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_lod_selector_destroy(pcs_lod_selector_t *this)
{
  if (this->v != PCSTREAM_NULL)
    free(this->v);

  this->v    = PCSTREAM_NULL;
  this->post = PCSTREAM_NULL;
  this->get  = PCSTREAM_NULL;
}

PCSTREAM_RET
pcs_lod_selector_post_dp_based(pcs_lod_selector_t *this,
                               PCSTREAM_COUNT       n_mod,
                               PCSTREAM_LOD_VERSION n_ver,
                               void                *metadata,
                               size_t               metadata_size,
                               void                *attrib,
                               PCSTREAM_BW          bw)
{
  PCSTREAM_RET ret = 0;

  this->n_ver      = n_ver;
  this->n_mod      = n_mod;

  this->v          = (PCSTREAM_LOD_VERSION *)malloc(
      sizeof(PCSTREAM_LOD_VERSION) * this->n_mod);

  ret = pcs_dp_based_solution(this->n_mod,
                              this->n_ver,
                              metadata,
                              metadata_size,
                              (PCSTREAM_RATIO *)attrib,
                              bw,
                              this->v);
  return ret;
}

PCSTREAM_RET
pcs_lod_selector_get_dp_based(pcs_lod_selector_t *this,
                              PCSTREAM_LOD_VERSION **selections_ptr)
{
  if (this->v == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;

  *selections_ptr = this->v;
  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET
pcs_lod_selector_post_lm_based(pcs_lod_selector_t *this,
                               PCSTREAM_COUNT       n_mod,
                               PCSTREAM_LOD_VERSION n_ver,
                               void                *metadata,
                               size_t               metadata_size,
                               void                *attrib,
                               PCSTREAM_BW          bw)
{
  return PCSTREAM_RET_FAIL;
}
PCSTREAM_RET
pcs_lod_selector_post_equal(pcs_lod_selector_t *this,
                            PCSTREAM_COUNT       n_mod,
                            PCSTREAM_LOD_VERSION n_ver,
                            void                *metadata,
                            size_t               metadata_size,
                            void                *attrib,
                            PCSTREAM_BW          bw)
{
  return PCSTREAM_RET_FAIL;
}
PCSTREAM_RET
pcs_lod_selector_post_hybrid(pcs_lod_selector_t *this,
                             PCSTREAM_COUNT       n_mod,
                             PCSTREAM_LOD_VERSION n_ver,
                             void                *metadata,
                             size_t               metadata_size,
                             void                *attrib,
                             PCSTREAM_BW          bw)
{
  return PCSTREAM_RET_FAIL;
}
PCSTREAM_RET
pcs_lod_selector_get_lm_based(pcs_lod_selector_t *this,
                              PCSTREAM_LOD_VERSION **selections_ptr)
{
  return PCSTREAM_RET_FAIL;
}
PCSTREAM_RET
pcs_lod_selector_get_equal(pcs_lod_selector_t *this,
                           PCSTREAM_LOD_VERSION **selections_ptr)
{
  return PCSTREAM_RET_FAIL;
}
PCSTREAM_RET
pcs_lod_selector_get_hybrid(pcs_lod_selector_t *this,
                            PCSTREAM_LOD_VERSION **selections_ptr)
{
  return PCSTREAM_RET_FAIL;
}
