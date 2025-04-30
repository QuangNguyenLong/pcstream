#include <pcstream/buffer.h>
#include <pcstream/bw_estimator.h>
#include <pcstream/lod_selector.h>
#include <pcstream/request_handler.h>
#include <pcstream/viewport_estimator.h>
#include <pcstream/visibility_computer.h>
#include <stdio.h>

int main(void)
{
  pcs_buffer_t  *info_list_ptr    = PCSTREAM_NULL;
  pcs_buffer_t **hull_list_ptr    = PCSTREAM_NULL;
  pcs_buffer_t  *curr_content_ptr = PCSTREAM_NULL;
  PCSTREAM_BW   *dl_speeds_ptr    = PCSTREAM_NULL;
  float         *esMVP_ptr        = PCSTREAM_NULL;
  long long      dtec             = 0;
  long long      dt               = 0;
  PCSTREAM_BW    Ra               = 0;

  dtec = 1000; // 1000ms per each estimation, this should be replaced
               // be the segment duration
  dt = 33; // 33ms per frame interval, this should be replaced by 1 /
           // fps

  // stuff needs to be free
  PCSTREAM_LOD_VERSION     *selects     = PCSTREAM_NULL;
  pcs_request_handler_t     hd          = {0};
  pcs_bw_estimator_t        bwes        = {0};
  pcs_viewport_estimator_t  vpes        = {0};
  pcs_visibility_computer_t vscp        = {0};
  pcs_lod_selector_t        vssl        = {0};
  PCSTREAM_RATIO           *screen_area = PCSTREAM_NULL;

  pcs_request_handler_init(&hd, PCSTREAM_REQUEST_HANDLER_H2);
  pcs_bw_estimator_init(&bwes, PCSTREAM_BW_ESTIMATOR_HARMONIC);
  pcs_viewport_estimator_init(
      &vpes, dt, PCSTREAM_VIEWPORT_ESTIMATOR_VELOCITY);
  pcs_visibility_computer_init(&vscp,
                               PCSTREAM_VISIBILITY_COMPUTER_HULL);
  pcs_lod_selector_init(&vssl, PCSTREAM_LOD_SELECTOR_DP_BASED);

  // session initialization
  hd.post_init(&hd,
               "https://127.0.0.1:8080/manifest.mpd",
               "https://127.0.0.1:8080/manifest-info.mpd",
               "https://127.0.0.1:8080/manifest-hull.mpd",
               "https://127.0.0.1:8080");

  hd.get_init(&hd, &info_list_ptr, &hull_list_ptr);

  // first segment
  selects = (PCSTREAM_LOD_VERSION *)malloc(
      sizeof(PCSTREAM_LOD_VERSION) * hd.seq_count);
  for (PCSTREAM_COUNT i = 0; i < hd.seq_count; i++)
    selects[i] = 0;

  hd.post_segment(&hd, selects);
  hd.get_segment(&hd, &curr_content_ptr);
  /*decoder should start working here*/

  // loop
  screen_area =
      (PCSTREAM_RATIO *)malloc(sizeof(PCSTREAM_RATIO) * hd.seq_count);
  for (PCSTREAM_COUNT i = 0; i < hd.seq_count; i++)
    screen_area[i] = 0;

  while (hd.curr_seg != hd.seg_count)
  {
    pcs_vec3f_t Pc = {0, 0, 0.1f};
    pcs_vec3f_t Po = {0, 0, 0};
    pcs_vec3f_t Vc = {1, 1, 1};
    pcs_vec3f_t Vo = {1, 1, 1};

    hd.get_dl_speeds(&hd, &dl_speeds_ptr);
    bwes.post(&bwes, dl_speeds_ptr, (size_t)hd.seq_count);
    bwes.get(&bwes, &Ra);

    vpes.post(&vpes, Pc, Po, Vc, Vo, dtec);
    vpes.get(&vpes, &esMVP_ptr);

    for (PCSTREAM_COUNT seq = 0; seq < hd.seq_count; seq++)
    {
      pcs_mesh_t m = {0};
      pcs_mesh_init(&m);

      m.read_from_buff_serial(&m,
                              hull_list_ptr[seq][hd.curr_seg].data,
                              hull_list_ptr[seq][hd.curr_seg].size);
      vscp.post(&vscp, esMVP_ptr, m);
      vscp.get(&vscp, &screen_area[seq]);
      pcs_mesh_destroy(&m);
    }

    vssl.post(&vssl,
              hd.seq_count,
              hd.rep_count,
              (void *)info_list_ptr[hd.curr_seg].data,
              info_list_ptr[hd.curr_seg].size,
              (void *)screen_area,
              Ra);

    hd.post_segment(&hd, selects);
    hd.get_segment(&hd, &curr_content_ptr);
  }
  // done, stop session
  free(selects);
  pcs_request_handler_destroy(&hd);
  pcs_bw_estimator_destroy(&bwes);
  pcs_viewport_estimator_destroy(&vpes);
  pcs_visibility_computer_destroy(&vscp);
  pcs_lod_selector_destroy(&vssl);
  free(screen_area);
}
