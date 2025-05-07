#include <pcstream/buffer.h>
#include <pcstream/bw_estimator.h>
#include <pcstream/lod_selector.h>
#include <pcstream/request_handler.h>
#include <pcstream/video_decoder.h>
#include <pcstream/viewport_estimator.h>
#include <pcstream/visibility_computer.h>
#include <stdio.h>

#define PCS_DTEC 1000
#define PCS_DT   33

// argv[1] = "https://127.0.0.1:8080/manifest.mpd"
// argv[2] = "https://127.0.0.1:8080/manifest-info.mpd"
// argv[3] = "https://127.0.0.1:8080/manifest-hull.mpd"
// argv[4] = "https://127.0.0.1:8080"

int main(int argc, char **argv)
{
  if (argc < 4)
  {
    return -1;
  }
  pcs_buffer_t  *info_list_ptr    = PCSTREAM_NULL;
  pcs_buffer_t **hull_list_ptr    = PCSTREAM_NULL;
  pcs_buffer_t  *curr_content_ptr = PCSTREAM_NULL;
  pcs_bw_t      *dl_speeds_ptr    = PCSTREAM_NULL;
  float         *esMVP_ptr        = PCSTREAM_NULL;
  long long      dtec             = 0;
  long long      deltat           = 0;
  pcs_bw_t       dl_es            = 0;

  dtec   = PCS_DTEC; // 1000ms per each estimation, this should be
                     // replaced be the segment duration
  deltat = PCS_DT;   // 33ms per frame interval, this should be
                     // replaced by 1 / fps

  // stuff needs to be free
  pcs_lod_version_t        *selects     = PCSTREAM_NULL;
  pcs_request_handler_t     hand        = {0};
  pcs_bw_estimator_t        bwes        = {0};
  pcs_viewport_estimator_t  vpes        = {0};
  pcs_visibility_computer_t vscp        = {0};
  pcs_lod_selector_t        vssl        = {0};
  pcs_video_decoder_t      *vddc        = PCSTREAM_NULL;
  pcs_ratio_t              *screen_area = PCSTREAM_NULL;

  pcs_request_handler_init(&hand, PCSTREAM_REQUEST_HANDLER_H2);
  pcs_bw_estimator_init(&bwes, PCSTREAM_BW_ESTIMATOR_HARMONIC);
  pcs_viewport_estimator_init(
      &vpes, deltat, PCSTREAM_VIEWPORT_ESTIMATOR_VELOCITY);
  pcs_visibility_computer_init(&vscp,
                               PCSTREAM_VISIBILITY_COMPUTER_HULL);
  pcs_lod_selector_init(&vssl, PCSTREAM_LOD_SELECTOR_DP_BASED);

  // session initialization
  hand.post_init(&hand, argv[1], argv[2], argv[3], argv[4]);

  hand.get_init(&hand, &info_list_ptr, &hull_list_ptr);

  // first segment
  selects = (pcs_lod_version_t *)malloc(sizeof(pcs_lod_version_t) *
                                        hand.seq_count);
  for (pcs_count_t i = 0; i < hand.seq_count; i++)
  {
    selects[i] = 0;
  }
  hand.post_segment(&hand, selects);
  hand.get_segment(&hand, &curr_content_ptr);
  /*decoder should start working here*/

  // loop
  screen_area =
      (pcs_ratio_t *)malloc(sizeof(pcs_ratio_t) * hand.seq_count);
  for (pcs_count_t i = 0; i < hand.seq_count; i++)
  {
    screen_area[i] = 0;
  }
  vddc = (pcs_video_decoder_t *)malloc(sizeof(pcs_video_decoder_t) *
                                       hand.seq_count);
  for (pcs_count_t i = 0; i < hand.seq_count; i++)
  {
    pcs_video_decoder_init(&(vddc[i]), 0);
  }
  while (hand.curr_seg != hand.seg_count)
  {

    for (pcs_count_t seq = 0; seq < hand.seq_count; seq++)
    {
      vddc[seq].post(&(vddc[seq]),
                     curr_content_ptr[seq].data,
                     curr_content_ptr[seq].size);
      // add render, maybe?
      // If pcs is done, consider writing WEBASM and let webGL render
      // instead?
    }

    pcs_vec3f_t Pcurr = {0, 0, 0.1F};
    pcs_vec3f_t Pold  = {0, 0, 0};
    pcs_vec3f_t Vcurr = {1, 1, 1};
    pcs_vec3f_t Vold  = {1, 1, 1};

    hand.get_dl_speeds(&hand, &dl_speeds_ptr);
    bwes.post(&bwes, dl_speeds_ptr, (size_t)hand.seq_count);
    bwes.get(&bwes, &dl_es);

    vpes.post(&vpes, Pcurr, Pold, Vcurr, Vold, dtec);
    vpes.get(&vpes, &esMVP_ptr);

    for (pcs_count_t seq = 0; seq < hand.seq_count; seq++)
    {
      pcs_mesh_t mesh = {0};
      pcs_mesh_init(&mesh);

      mesh.read_from_buff_serial(
          &mesh,
          hull_list_ptr[seq][hand.curr_seg].data,
          hull_list_ptr[seq][hand.curr_seg].size);
      vscp.post(&vscp, esMVP_ptr, mesh);
      vscp.get(&vscp, &screen_area[seq]);
      pcs_mesh_destroy(&mesh);
    }

    vssl.post(&vssl,
              hand.seq_count,
              hand.rep_count,
              (void *)info_list_ptr[hand.curr_seg].data,
              info_list_ptr[hand.curr_seg].size,
              (void *)screen_area,
              dl_es);

    hand.post_segment(&hand, selects);
    hand.get_segment(&hand, &curr_content_ptr);
  }
  // done, stop session

  for (pcs_count_t i = 0; i < hand.seq_count; i++)
  {
    pcs_video_decoder_destroy(&(vddc[i]));
  }
  free(vddc);
  free(selects);
  pcs_request_handler_destroy(&hand);
  pcs_bw_estimator_destroy(&bwes);
  pcs_viewport_estimator_destroy(&vpes);
  pcs_visibility_computer_destroy(&vscp);
  pcs_lod_selector_destroy(&vssl);
  free(screen_area);
}
