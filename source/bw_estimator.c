#include <pcstream/bw_estimator.h>
#include <pcstream/def.h>

pcs_ret_t pcs_bw_estimator_init(pcs_bw_estimator_t *self, int type)
{
  *self           = (pcs_bw_estimator_t){0};
  pcs_bw_t dls_es = PCSTREAM_BW_DEFAULT;

  switch (type)
  {
  case PCSTREAM_BW_ESTIMATOR_HARMONIC:
  {
    self->dls_es = dls_es;
    self->post   = pcs_bw_estimator_post_harmonic;
    self->get    = pcs_bw_estimator_get_harmonic;
  }
  break;
  default:
  {
    self->dls_es = dls_es;
    self->post   = pcs_bw_estimator_post_harmonic;
    self->get    = pcs_bw_estimator_get_harmonic;
  }
  break;
  }
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_bw_estimator_destroy(pcs_bw_estimator_t *self)
{
  *self        = (pcs_bw_estimator_t){0};
  self->dls_es = PCSTREAM_BW_DEFAULT;
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_bw_estimator_post_harmonic(pcs_bw_estimator_t *self,
                                         pcs_bw_t           *dls_arr,
                                         size_t dls_count)
{
  float  sum         = 0.0F;
  size_t count_valid = 0;
  self->dls_es       = PCSTREAM_BW_DEFAULT;

  if (dls_count == 0)
  {
    return PCSTREAM_RET_FAIL;
  }
  for (size_t i = 0; i < dls_count; i++)
  {
    if (dls_arr[i] >= 0)
    {
      count_valid++;
      sum += 1.0F / (float)dls_arr[i];
    }
  }
  self->dls_es = (pcs_bw_t)((float)count_valid / sum);

  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_bw_estimator_get_harmonic(pcs_bw_estimator_t *self,
                                        pcs_bw_t           *dls_es)
{
  if (self->dls_es == PCSTREAM_BW_DEFAULT)
  {
    return PCSTREAM_RET_FAIL;
  }

  *dls_es = self->dls_es;
  return PCSTREAM_RET_SUCCESS;
}
