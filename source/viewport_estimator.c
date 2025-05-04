#include <pcstream/vec3f.h>
#include <pcstream/viewport_estimator.h>
#include <string.h>
PCSTREAM_RET pcs_viewport_estimator_init(
    pcs_viewport_estimator_t *self, long long dt, int type)
{
  if (dt <= 0)
    return PCSTREAM_RET_FAIL;

  switch (type)
  {
  case PCSTREAM_VIEWPORT_ESTIMATOR_VELOCITY:
  {
    self->dt = dt;

    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        self->esMVP[i][j] = 0;

    self->post = pcs_viewport_estimator_post_velocity;
    self->get  = pcs_viewport_estimator_get_velocity;
  }
  break;
  default:
  {
    self->dt = dt;

    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        self->esMVP[i][j] = 0;

    self->post = pcs_viewport_estimator_post_velocity;
    self->get  = pcs_viewport_estimator_get_velocity;
  }
  break;
  }
  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET
pcs_viewport_estimator_destroy(pcs_viewport_estimator_t *self)
{
  self->dt = 0;

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      self->esMVP[i][j] = 0;

  self->post = PCSTREAM_NULL;
  self->get  = PCSTREAM_NULL;
  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET
pcs_viewport_estimator_post_velocity(pcs_viewport_estimator_t *self,
                                     pcs_vec3f_t Pc,
                                     pcs_vec3f_t Po,
                                     pcs_vec3f_t Vc,
                                     pcs_vec3f_t Vo,
                                     long long   dtec)
{
  pcs_vec3f_t Pe     = {0};
  pcs_vec3f_t pvel   = {0};
  pcs_vec3f_t Ve     = {0};
  float       dtheta = 0.0f;
  pcs_vec3f_t omega  = {0};
  pcs_vec3f_t target = {0};
  pcs_vec3f_t fwd    = {0};
  pcs_vec3f_t side   = {0};
  pcs_vec3f_t up     = {0};
  pcs_vec3f_t y      = {0, 1, 0};

  pvel               = pcs_vec3f_mul_scalar(pcs_vec3f_sub(Pc, Po),
                              (float)dtec / (float)self->dt);

  Pe                 = pcs_vec3f_add(Pc, pvel);

  dtheta = pcs_vec3f_dot(Vc, Vo) / pcs_vec3f_magnitude(Vc) /
           pcs_vec3f_magnitude(Vo);

  omega           = pcs_vec3f_normalize(pcs_vec3f_cross(Vc, Vo));

  Ve              = pcs_vec3f_rotate(Vc, dtheta, omega);

  target          = pcs_vec3f_add(Pe, Ve);

  fwd             = pcs_vec3f_normalize(pcs_vec3f_sub(target, Pe));
  side            = pcs_vec3f_normalize(pcs_vec3f_cross(fwd, y));
  up              = pcs_vec3f_normalize(pcs_vec3f_cross(side, fwd));

  float tmp[4][4] = {
      {side.x, side.y, side.z, -1.0f * pcs_vec3f_dot(side, Pe)},
      {up.x, up.y, up.z, -1.0f * pcs_vec3f_dot(up, Pe)},
      {-fwd.x, -fwd.y, -fwd.z, -1.0f * pcs_vec3f_dot(fwd, Pe)},
      {0.0f, 0.0f, 0.0f, 1.0f}
  };

  memcpy(self->esMVP, tmp, sizeof(self->esMVP));

  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET
pcs_viewport_estimator_get_velocity(pcs_viewport_estimator_t *self,
                                    float **esMVP_ptr)
{
  *esMVP_ptr = &(self->esMVP[0][0]);
  return PCSTREAM_RET_SUCCESS;
}
