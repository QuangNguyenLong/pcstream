#include <pcstream/lod_selector.h>

int main(void)
{
  pcs_lod_selector_t vssl = {0};
  pcs_lod_selector_init(&vssl, 0);
  pcs_lod_selector_destroy(&vssl);
}
