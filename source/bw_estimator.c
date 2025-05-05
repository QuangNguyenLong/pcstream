#include <pcstream/bw_estimator.h>
#include <pcstream/def.h>

PCSTREAM_RET pcs_bw_estimator_init(pcs_bw_estimator_t *self, int type)
{
  *self          = (pcs_bw_estimator_t){0};
  PCSTREAM_BW Ra = PCSTREAM_BW_DEFAULT;

  switch (type)
  {
  case PCSTREAM_BW_ESTIMATOR_HARMONIC:
  {
    self->Ra   = Ra;
    self->post = pcs_bw_estimator_post_harmonic;
    self->get  = pcs_bw_estimator_get_harmonic;
  }
  break;
  default:
  {
    self->Ra   = Ra;
    self->post = pcs_bw_estimator_post_harmonic;
    self->get  = pcs_bw_estimator_get_harmonic;
  }
  break;
  }
  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET pcs_bw_estimator_destroy(pcs_bw_estimator_t *self)
{
  *self    = (pcs_bw_estimator_t){0};
  self->Ra = PCSTREAM_BW_DEFAULT;
  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET
pcs_bw_estimator_post_harmonic(pcs_bw_estimator_t *self,
                               PCSTREAM_BW        *R,
                               size_t              M)
{
  float  sum         = 0.0f;
  size_t count_valid = 0;
  self->Ra           = PCSTREAM_BW_DEFAULT;

  if (M == 0)
    return PCSTREAM_RET_FAIL;

  for (size_t i = 0; i < M; i++)
  {
    if (R[i] >= 0)
    {
      count_valid++;
      sum += 1.0f / (float)R[i];
    }
  }
  self->Ra = (PCSTREAM_BW)((float)count_valid / sum);

  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET pcs_bw_estimator_get_harmonic(pcs_bw_estimator_t *self,
                                           PCSTREAM_BW        *Ra)
{
  if (self->Ra == PCSTREAM_BW_DEFAULT)
    return PCSTREAM_RET_FAIL;

  *Ra = self->Ra;
  return PCSTREAM_RET_SUCCESS;
}
