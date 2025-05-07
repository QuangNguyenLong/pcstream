#include <pcstream/bw_estimator.h>

int main(void)
{
  pcs_bw_t           dles   = 0;
  pcs_bw_t           rdls[] = {1000000, 2000000, 6000000}; // Bytes/s
  pcs_bw_estimator_t bwes   = {0};

  PCSTREAM_CHECK_FATAL(
      pcs_bw_estimator_init(&bwes, PCSTREAM_BW_ESTIMATOR_HARMONIC));

  PCSTREAM_CHECK_FATAL(
      bwes.post(&bwes, rdls, sizeof(rdls) / sizeof(rdls[0])));
  PCSTREAM_CHECK_FATAL(bwes.get(&bwes, &dles));
  PCSTREAM_CHECK_FATAL(pcs_bw_estimator_destroy(&bwes));
  return 0;
}
