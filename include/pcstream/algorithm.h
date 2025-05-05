#ifndef PCSTREAM_ALGORITHM_H
#define PCSTREAM_ALGORITHM_H

#include <pcstream/def.h>

PCSTREAM_EXPORT PCSTREAM_RET
pcs_dp_based_solution(PCSTREAM_COUNT        n_mod,
                      PCSTREAM_COUNT        n_ver,
                      void                 *metadata,
                      size_t                metadata_size,
                      PCSTREAM_RATIO       *screen_ratio,
                      PCSTREAM_BW           bandwidth,
                      PCSTREAM_LOD_VERSION *selection);
PCSTREAM_EXPORT PCSTREAM_RET pcs_lm_based_solution(void);
PCSTREAM_EXPORT PCSTREAM_RET pcs_hybrid_solution(void);
PCSTREAM_EXPORT PCSTREAM_RET pcs_equal_solution(void);

#endif
