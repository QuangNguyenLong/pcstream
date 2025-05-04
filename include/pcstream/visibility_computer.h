#ifndef PCSTREAM_VISIBILITY_COMPUTER_H
#define PCSTREAM_VISIBILITY_COMPUTER_H

#include <pcstream/def.h>
#include <pcstream/mesh.h>
typedef struct pcs_visibility_computer_t pcs_visibility_computer_t;

struct pcs_visibility_computer_t
{
  PCSTREAM_RATIO ratio;

  PCSTREAM_RET(*post)
  (pcs_visibility_computer_t *,
   const float *, // MVP
   pcs_mesh_t     // box
  );
  PCSTREAM_RET (*get)(pcs_visibility_computer_t *, PCSTREAM_RATIO *);
};

PCSTREAM_RET
pcs_visibility_computer_init(pcs_visibility_computer_t *self,
                             int type);
PCSTREAM_RET
pcs_visibility_computer_destroy(pcs_visibility_computer_t *self);

PCSTREAM_RET
pcs_visibility_computer_post_hull(pcs_visibility_computer_t *self,
                                  const float *MVP,
                                  pcs_mesh_t   hull);

PCSTREAM_RET
pcs_visibility_computer_get_hull(pcs_visibility_computer_t *self,
                                 PCSTREAM_RATIO *ratio);

#endif
