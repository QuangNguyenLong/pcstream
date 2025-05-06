#ifndef PCSTREAM_VISIBILITY_COMPUTER_H
#define PCSTREAM_VISIBILITY_COMPUTER_H

#include <pcstream/def.h>
#include <pcstream/mesh.h>
typedef struct pcs_visibility_computer_t pcs_visibility_computer_t;

struct pcs_visibility_computer_t
{
  pcs_ratio_t ratio;

  pcs_ret_t (*post)(pcs_visibility_computer_t *,
                    const float *, // MVP
                    pcs_mesh_t     // box
  );
  pcs_ret_t (*get)(pcs_visibility_computer_t *, pcs_ratio_t *);
};

PCSTREAM_EXPORT pcs_ret_t pcs_visibility_computer_init(
    pcs_visibility_computer_t *self, int type);
PCSTREAM_EXPORT pcs_ret_t
pcs_visibility_computer_destroy(pcs_visibility_computer_t *self);

pcs_ret_t
pcs_visibility_computer_post_hull(pcs_visibility_computer_t *self,
                                  const float               *MVP,
                                  pcs_mesh_t                 hull);

pcs_ret_t
pcs_visibility_computer_get_hull(pcs_visibility_computer_t *self,
                                 pcs_ratio_t               *ratio);

#endif
