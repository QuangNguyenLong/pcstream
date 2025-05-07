#include <pcstream/gof.h>

int main(void)
{
  pcs_gof_t gof = {0};
  PCSTREAM_CHECK_FATAL(pcs_gof_init(&gof));
  PCSTREAM_CHECK_FATAL(pcs_gof_destroy(&gof));
}
