#ifndef PCSTREAM_ALGORITHM_H
#define PCSTREAM_ALGORITHM_H

#include <pcstream/def.h>

PCSTREAM_EXPORT pcs_ret_t
                          pcs_dp_based_solution(pcs_count_t        n_mod,
                                                pcs_count_t        n_ver,
                                                void              *metadata,
                                                size_t             metadata_size,
                                                pcs_ratio_t       *screen_ratio,
                                                pcs_bw_t           bandwidth,
                                                pcs_lod_version_t *selection);
PCSTREAM_EXPORT pcs_ret_t pcs_lm_based_solution(void);
PCSTREAM_EXPORT pcs_ret_t pcs_hybrid_solution(void);
PCSTREAM_EXPORT pcs_ret_t pcs_equal_solution(void);

#endif
