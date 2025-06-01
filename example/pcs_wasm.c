#include <emscripten/emscripten.h>
#include <pcstream/pcstream.h>
#include <stdlib.h>

EMSCRIPTEN_KEEPALIVE
uint32_t wasm_pcs_harmonic_bw_estimate(pcs_bw_t *bw_arr,
                                       uint32_t  arr_len,
                                       pcs_bw_t *bw_es)
{
  if (bw_es == NULL || bw_arr == NULL)
  {
    return 0;
  }
  pcs_bw_t           ret  = 0;
  pcs_bw_estimator_t bwes = {0};
  pcs_bw_estimator_init(&bwes, PCSTREAM_BW_ESTIMATOR_HARMONIC);

  bwes.post(&bwes, bw_arr, (size_t)arr_len);
  bwes.get(&bwes, bw_es);

  pcs_bw_estimator_destroy(&bwes);
  return 1;
}

EMSCRIPTEN_KEEPALIVE
uint32_t
wasm_pcs_velocity_viewport_estimate(uint32_t delta_t,
                                    float    p_curr_x,
                                    float    p_curr_y,
                                    float    p_curr_z,
                                    float    p_old_x,
                                    float    p_old_y,
                                    float    p_old_z,
                                    float    v_curr_x,
                                    float    v_curr_y,
                                    float    v_curr_z,
                                    float    v_old_x,
                                    float    v_old_y,
                                    float    v_old_z,
                                    uint32_t estimate_duration,
                                    float   *esMVP)
{
  if (esMVP == NULL)
  {
    return 0;
  }
  pcs_vec3f_t              Pcurr = {p_curr_x, p_curr_y, p_curr_z};
  pcs_vec3f_t              Pold  = {p_old_x, p_old_y, p_old_z};
  pcs_vec3f_t              Vcurr = {v_curr_x, v_curr_y, v_curr_z};
  pcs_vec3f_t              Vold  = {v_old_x, v_old_y, v_old_z};
  float                   *esMVP_ptr = PCSTREAM_NULL;

  pcs_viewport_estimator_t vpes      = {0};

  pcs_viewport_estimator_init(
      &vpes, delta_t, PCSTREAM_VIEWPORT_ESTIMATOR_VELOCITY);

  vpes.post(&vpes, Pcurr, Pold, Vcurr, Vold, estimate_duration);
  vpes.get(&vpes, &esMVP_ptr);

  for (int i = 0; i < 4 * 4; i++)
  {
    esMVP[i] = esMVP_ptr[i];
  }

  pcs_viewport_estimator_destroy(&vpes);
  return 1;
}

EMSCRIPTEN_KEEPALIVE
uint32_t wasm_pcs_hull_visibility_compute(float      *esMVP,
                                          const char *hull_data,
                                          uint32_t    hull_size,
                                          float      *screen_area)
{
  if (screen_area == NULL)
  {
    return 0;
  }

  pcs_mesh_t                hull = {0};
  pcs_visibility_computer_t vscp = {0};

  pcs_mesh_init(&hull);
  pcs_visibility_computer_init(&vscp,
                               PCSTREAM_VISIBILITY_COMPUTER_HULL);

  hull.read_from_buff_serial(&hull, hull_data, (size_t)hull_size);

  vscp.post(&vscp, esMVP, hull);
  vscp.get(&vscp, screen_area);

  pcs_visibility_computer_destroy(&vscp);
  pcs_mesh_destroy(&hull);

  return 1;
}

// EMSCRIPTEN_KEEPALIVE
// uint32_t wasm_pcs_dp_based_lod_select()
//{
//
// }

EMSCRIPTEN_KEEPALIVE
pcs_point_cloud_t *wasm_pcs_filrg_pcc_buffer_to_gof(const char *data,
                                                    uint32_t    size,
                                                    uint32_t   *num)
{
  pcs_video_decoder_t vddc = {0};
  pcs_gof_t           rec  = {0};

  pcs_video_decoder_init(&vddc, PCSTREAM_VIDEO_DECODER_FILRG_PCC);

  vddc.post(&vddc, data, (size_t)size);
  vddc.get(&vddc, &rec);

  *num = (uint32_t)rec.size;

  // pcs_video_decoder_destroy(&vddc);
  return rec.frames;
}

EMSCRIPTEN_KEEPALIVE
pcs_point_cloud_t *wasm_pcs_gof_get_frame(pcs_point_cloud_t *gof,
                                          int                index)
{
  return &gof[index];
}

EMSCRIPTEN_KEEPALIVE
pcs_filrg_pcc_coord_t *
wasm_pcs_filrg_pcc_frame_get_positions(pcs_point_cloud_t *frame)
{
  return (pcs_filrg_pcc_coord_t *)frame->positions;
}

EMSCRIPTEN_KEEPALIVE
uint8_t *wasm_pcs_frame_get_colors(pcs_point_cloud_t *frame)
{
  return frame->colors;
}

EMSCRIPTEN_KEEPALIVE
uint32_t wasm_pcs_frame_get_size(pcs_point_cloud_t *frame)
{
  return (uint32_t)frame->size;
}

EMSCRIPTEN_KEEPALIVE
void wasm_free(void *ptr)
{
  if (ptr != NULL)
  {
    free(ptr);
  }
}

EMSCRIPTEN_KEEPALIVE
void *wasm_malloc(uint32_t size)
{
  if (size != 0)
  {
    return malloc((size_t)size);
  }
  return NULL;
}
