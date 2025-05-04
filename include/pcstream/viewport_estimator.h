#ifndef PCSTREAM_VIEWPORT_ESTIMATOR_H
#define PCSTREAM_VIEWPORT_ESTIMATOR_H

#include <pcstream/def.h>
#include <pcstream/vec3f.h>

typedef struct pcs_viewport_estimator_t pcs_viewport_estimator_t;

struct pcs_viewport_estimator_t
{
  long long dt; // time between 2 consecutive frame (ms)
  float     esMVP[4][4];

  PCSTREAM_RET(*post)
  (pcs_viewport_estimator_t *,
   pcs_vec3f_t, // Pc
   pcs_vec3f_t, // Po
   pcs_vec3f_t, // Vc
   pcs_vec3f_t, // Vo
   long long    // dtec = te - tc
  );
  PCSTREAM_RET(*get)
  (pcs_viewport_estimator_t *,
   float ** // MVP matrix ptr to change
  );
};

PCSTREAM_RET pcs_viewport_estimator_init(
    pcs_viewport_estimator_t *self, long long dt, int type);

PCSTREAM_RET
pcs_viewport_estimator_destroy(pcs_viewport_estimator_t *self);

PCSTREAM_RET
pcs_viewport_estimator_post_velocity(pcs_viewport_estimator_t *self,
                                     pcs_vec3f_t Pc,
                                     pcs_vec3f_t Po,
                                     pcs_vec3f_t Vc,
                                     pcs_vec3f_t Vo,
                                     long long   dtec);

PCSTREAM_RET
pcs_viewport_estimator_get_velocity(pcs_viewport_estimator_t *self,
                                    float **esMVP_ptr);

#endif
