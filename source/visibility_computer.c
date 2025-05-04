#include <pcstream/visibility_computer.h>

PCSTREAM_RET
pcs_visibility_computer_init(pcs_visibility_computer_t *self,
                             int type)
{
  switch (type)
  {
  case PCSTREAM_VISIBILITY_COMPUTER_HULL:
  {
    self->ratio = PCSTREAM_RATIO_DEFAULT;
    self->post  = pcs_visibility_computer_post_hull;
    self->get   = pcs_visibility_computer_get_hull;
  }
  break;
  default:
  {
    self->ratio = PCSTREAM_RATIO_DEFAULT;
    self->post  = pcs_visibility_computer_post_hull;
    self->get   = pcs_visibility_computer_get_hull;
  }
  break;
  }
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_visibility_computer_destroy(pcs_visibility_computer_t *self)
{
  *self = (pcs_visibility_computer_t){0};
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_visibility_computer_post_hull(pcs_visibility_computer_t *self,
                                  const float *MVP,
                                  pcs_mesh_t   hull)
{
  hull.screen_ratio(&hull, MVP, &self->ratio);
  if (self->ratio < 0 || self->ratio > 1.0f)
  {
    self->ratio = PCSTREAM_RATIO_DEFAULT;
    return PCSTREAM_RET_FAIL;
  }
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_visibility_computer_get_hull(pcs_visibility_computer_t *self,
                                 PCSTREAM_RATIO *ratio)
{
  if (self->ratio < 0)
    return PCSTREAM_RET_FAIL;

  *ratio = self->ratio;
  return PCSTREAM_RET_SUCCESS;
}
