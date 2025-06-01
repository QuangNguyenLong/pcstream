/* This file is part of PCSTREAM.
 * Copyright (C) 2025 FIL Research Group, ANSA Laboratory
 *
 * PCSTREAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <pcstream/vec3f.h>
#include <pcstream/viewport_estimator.h>
#include <string.h>
pcs_ret_t pcs_viewport_estimator_init(pcs_viewport_estimator_t *self,
                                      uint32_t deltat,
                                      int      type)
{
  if (deltat <= 0)
    return PCSTREAM_RET_FAIL;

  switch (type)
  {
  case PCSTREAM_VIEWPORT_ESTIMATOR_VELOCITY:
  {
    self->deltat = deltat;

    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        self->esMVP[i][j] = 0;
      }
    }

    self->post = pcs_viewport_estimator_post_velocity;
    self->get  = pcs_viewport_estimator_get_velocity;
  }
  break;
  default:
  {
    self->deltat = deltat;

    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        self->esMVP[i][j] = 0;
      }
    }

    self->post = pcs_viewport_estimator_post_velocity;
    self->get  = pcs_viewport_estimator_get_velocity;
  }
  break;
  }
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t
pcs_viewport_estimator_destroy(pcs_viewport_estimator_t *self)
{
  self->deltat = 0;

  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      self->esMVP[i][j] = 0;
    }
  }

  self->post = PCSTREAM_NULL;
  self->get  = PCSTREAM_NULL;
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t
pcs_viewport_estimator_post_velocity(pcs_viewport_estimator_t *self,
                                     pcs_vec3f_t               Pcurr,
                                     pcs_vec3f_t               Pold,
                                     pcs_vec3f_t               Vcurr,
                                     pcs_vec3f_t               Vold,
                                     uint32_t                  dtec)
{
  pcs_vec3f_t Pes    = {0};
  pcs_vec3f_t pvel   = {0};
  pcs_vec3f_t Ves    = {0};
  float       dtheta = 0.0F;
  pcs_vec3f_t omega  = {0};
  pcs_vec3f_t target = {0};
  pcs_vec3f_t fwd    = {0};
  pcs_vec3f_t side   = {0};
  pcs_vec3f_t upv    = {0};
  pcs_vec3f_t yvec   = {0, 1, 0};

  pvel   = pcs_vec3f_mul_scalar(pcs_vec3f_sub(Pcurr, Pold),
                              (float)dtec / (float)self->deltat);

  Pes    = pcs_vec3f_add(Pcurr, pvel);

  dtheta = pcs_vec3f_dot(Vcurr, Vold) / pcs_vec3f_magnitude(Vcurr) /
           pcs_vec3f_magnitude(Vold);

  omega  = pcs_vec3f_normalize(pcs_vec3f_cross(Vcurr, Vold));

  Ves    = pcs_vec3f_rotate(Vcurr, dtheta, omega);

  target = pcs_vec3f_add(Pes, Ves);

  fwd    = pcs_vec3f_normalize(pcs_vec3f_sub(target, Pes));
  side   = pcs_vec3f_normalize(pcs_vec3f_cross(fwd, yvec));
  upv    = pcs_vec3f_normalize(pcs_vec3f_cross(side, fwd));

  float tmp[4][4] = {
      {side.x, side.y, side.z, -1.0F * pcs_vec3f_dot(side, Pes)},
      {upv.x, upv.y, upv.z, -1.0F * pcs_vec3f_dot(upv, Pes)},
      {-fwd.x, -fwd.y, -fwd.z, -1.0F * pcs_vec3f_dot(fwd, Pes)},
      {0.0F, 0.0F, 0.0F, 1.0F}
  };

  memcpy(self->esMVP, tmp, sizeof(self->esMVP));

  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t
pcs_viewport_estimator_get_velocity(pcs_viewport_estimator_t *self,
                                    float **esMVP_ptr)
{
  *esMVP_ptr = &(self->esMVP[0][0]);
  return PCSTREAM_RET_SUCCESS;
}
