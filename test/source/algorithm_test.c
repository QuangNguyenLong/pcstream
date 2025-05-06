#include <pcstream/algorithm.h>
#include <stdio.h>
#include <stdlib.h>

#define N_MOD      10
#define N_VER      4
#define CURRENT_BW 50000000 // Bytes/s -> 50Mbps
#define BUFF_SIZE  1024
size_t read_file(const char *filepath, char *buff, size_t buff_size)
{
  FILE *file = fopen(filepath, "rb");
  if (!file)
  {
    perror("Error opening file");
    return 0;
  }

  size_t bytes_read = fread(buff, 1, buff_size, file);

  fclose(file);
  return bytes_read;
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    return -1;
  }
  pcs_count_t       n_mod               = 0;
  pcs_count_t       n_ver               = 0;
  char              metadata[BUFF_SIZE] = {0};
  size_t            metadata_size       = 0;
  pcs_ratio_t       screen_ratio[]      = {0.1F,
                                           0.2F,
                                           0.05F,
                                           0.05F,
                                           0.05F,
                                           0.05F,
                                           0.0F,
                                           0.0F,
                                           0.2F,
                                           0.3F};
  pcs_bw_t          bw                  = 0;
  pcs_lod_version_t selection[N_MOD]    = {0};

  pcs_ret_t         ret                 = 0;

  n_mod                                 = N_MOD;
  n_ver                                 = N_VER;
  bw                                    = CURRENT_BW;

  metadata_size =
      read_file((const char *)argv[1], metadata, sizeof(metadata));

  ret = pcs_dp_based_solution(n_mod,
                              n_ver,
                              (void *)metadata,
                              metadata_size,
                              screen_ratio,
                              bw,
                              selection);
  if (ret == PCSTREAM_RET_FAIL)
  {
    return -1;
  }
  return 0;
}
