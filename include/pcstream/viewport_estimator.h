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
#ifndef PCSTREAM_VIEWPORT_ESTIMATOR_H
#define PCSTREAM_VIEWPORT_ESTIMATOR_H

#include <pcstream/def.h>
#include <pcstream/vec3f.h>

typedef struct pcs_viewport_estimator_t pcs_viewport_estimator_t;

struct pcs_viewport_estimator_t
{
  long long deltat; // time between 2 consecutive frame (ms)
  float     esMVP[4][4];

  pcs_ret_t (*post)(pcs_viewport_estimator_t *,
                    pcs_vec3f_t, // Pcurr
                    pcs_vec3f_t, // Pold
                    pcs_vec3f_t, // Vcurr
                    pcs_vec3f_t, // Vold
                    long long    // dtec = te - tc
  );
  pcs_ret_t (*get)(pcs_viewport_estimator_t *,
                   float ** // MVP matrix ptr to change
  );
};

PCSTREAM_EXPORT pcs_ret_t pcs_viewport_estimator_init(
    pcs_viewport_estimator_t *self, long long deltat, int type);

PCSTREAM_EXPORT pcs_ret_t
pcs_viewport_estimator_destroy(pcs_viewport_estimator_t *self);

pcs_ret_t
pcs_viewport_estimator_post_velocity(pcs_viewport_estimator_t *self,
                                     pcs_vec3f_t               Pcurr,
                                     pcs_vec3f_t               Pold,
                                     pcs_vec3f_t               Vcurr,
                                     pcs_vec3f_t               Vold,
                                     long long                 dtec);

pcs_ret_t
pcs_viewport_estimator_get_velocity(pcs_viewport_estimator_t *self,
                                    float **esMVP_ptr);

#endif
