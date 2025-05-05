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
    return -1;
  PCSTREAM_COUNT       n_mod               = 0;
  PCSTREAM_COUNT       n_ver               = 0;
  char                 metadata[BUFF_SIZE] = {0};
  size_t               metadata_size       = 0;
  PCSTREAM_RATIO       screen_ratio[]      = {0.1f,
                                              0.2f,
                                              0.05f,
                                              0.05f,
                                              0.05f,
                                              0.05f,
                                              0.0f,
                                              0.0f,
                                              0.2f,
                                              0.3f};
  PCSTREAM_BW          bw                  = 0;
  PCSTREAM_LOD_VERSION selection[N_MOD]    = {0};

  PCSTREAM_RET         ret                 = 0;

  n_mod                                    = N_MOD;
  n_ver                                    = N_VER;
  bw                                       = CURRENT_BW;

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
    return -1;
  return 0;
}
