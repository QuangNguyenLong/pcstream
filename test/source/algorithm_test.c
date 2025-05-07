#include "utils.h"
#include <pcstream/algorithm.h>
#include <stdlib.h>

#define N_MOD      10
#define N_VER      4
#define CURRENT_BW 50000000 // Bytes/s -> 50Mbps
int main(int argc, char **argv)
{
  if (argc < 2)
  {
    return -1;
  }
  pcs_count_t       n_mod            = 0;
  pcs_count_t       n_ver            = 0;
  char             *metadata         = PCSTREAM_NULL;
  size_t            metadata_size    = 0;
  pcs_ratio_t       screen_ratio[]   = {0.1F,
                                        0.2F,
                                        0.05F,
                                        0.05F,
                                        0.05F,
                                        0.05F,
                                        0.0F,
                                        0.0F,
                                        0.2F,
                                        0.3F};
  pcs_bw_t          bw               = 0;
  pcs_lod_version_t selection[N_MOD] = {0};

  n_mod                              = N_MOD;
  n_ver                              = N_VER;
  bw                                 = CURRENT_BW;

  metadata = read_file((const char *)argv[1], &metadata_size);

  PCSTREAM_CHECK_FATAL(pcs_dp_based_solution(n_mod,
                                             n_ver,
                                             (void *)metadata,
                                             metadata_size,
                                             screen_ratio,
                                             bw,
                                             selection));
  free(metadata);
  return 0;
}
