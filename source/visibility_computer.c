#include <pcstream/visibility_computer.h>

PCSTREAM_RET
pcs_visibility_computer_init(pcs_visibility_computer_t *this,
                             int type)
{
  switch (type)
  {
  case PCSTREAM_VISIBILITY_COMPUTER_HULL:
  {
    this->ratio = PCSTREAM_RATIO_DEFAULT;
    this->post  = pcs_visibility_computer_post_hull;
    this->get   = pcs_visibility_computer_get_hull;
  }
  break;
  default:
  {
    this->ratio = PCSTREAM_RATIO_DEFAULT;
    this->post  = pcs_visibility_computer_post_hull;
    this->get   = pcs_visibility_computer_get_hull;
  }
  break;
  }
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_visibility_computer_destroy(pcs_visibility_computer_t *this)
{
  *this = (pcs_visibility_computer_t){0};
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_visibility_computer_post_hull(pcs_visibility_computer_t *this,
                                  const float *MVP,
                                  pcs_mesh_t   hull)
{
  hull.screen_ratio(&hull, MVP, &this->ratio);
  if (this->ratio < 0 || this->ratio > 1.0f)
  {
    this->ratio = PCSTREAM_RATIO_DEFAULT;
    return PCSTREAM_RET_FAIL;
  }
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET
pcs_visibility_computer_get_hull(pcs_visibility_computer_t *this,
                                 PCSTREAM_RATIO *ratio)
{
  if (this->ratio < 0)
    return PCSTREAM_RET_FAIL;

  *ratio = this->ratio;
  return PCSTREAM_RET_SUCCESS;
}
