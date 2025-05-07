/* This file is part of PCSTREAM.
 * Copyright (C) 2025 FIL Research Group, ANSA Laboratory
 *
 * PCSTREAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <pcstream/algorithm.h>
#include <pcstream/lod_selector.h>
#include <stdlib.h>

pcs_ret_t pcs_lod_selector_init(pcs_lod_selector_t *self, int type)
{
  self->n_ver = 0;
  self->n_mod = 0;

  self->v     = PCSTREAM_NULL;

  switch (type)
  {
  case PCSTREAM_LOD_SELECTOR_DP_BASED:
  {
    self->post = pcs_lod_selector_post_dp_based;
    self->get  = pcs_lod_selector_get_dp_based;
  }
  break;
  case PCSTREAM_LOD_SELECTOR_LM_BASED:
  {
    self->post = pcs_lod_selector_post_lm_based;
    self->get  = pcs_lod_selector_get_lm_based;
  }
  break;
  case PCSTREAM_LOD_SELECTOR_EQUAL:
  {
    self->post = pcs_lod_selector_post_equal;
    self->get  = pcs_lod_selector_get_equal;
  }
  break;
  case PCSTREAM_LOD_SELECTOR_HYBRID:
  {
    self->post = pcs_lod_selector_post_hybrid;
    self->get  = pcs_lod_selector_get_hybrid;
  }
  break;
  default:
  {
    self->post = pcs_lod_selector_post_lm_based;
    self->get  = pcs_lod_selector_get_lm_based;
  }
  break;
  }
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t pcs_lod_selector_destroy(pcs_lod_selector_t *self)
{
  if (self->v != PCSTREAM_NULL)
  {
    free(self->v);
  }
  self->v    = PCSTREAM_NULL;
  self->post = PCSTREAM_NULL;
  self->get  = PCSTREAM_NULL;
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_lod_selector_post_dp_based(pcs_lod_selector_t *self,
                                         pcs_count_t         n_mod,
                                         pcs_lod_version_t   n_ver,
                                         void    *metadata,
                                         size_t   metadata_size,
                                         void    *attrib,
                                         pcs_bw_t bandwidth)
{
  pcs_ret_t ret = 0;

  self->n_ver   = n_ver;
  self->n_mod   = n_mod;

  self->v = (pcs_lod_version_t *)malloc(sizeof(pcs_lod_version_t) *
                                        self->n_mod);

  ret     = pcs_dp_based_solution(self->n_mod,
                              self->n_ver,
                              metadata,
                              metadata_size,
                              (pcs_ratio_t *)attrib,
                              bandwidth,
                              self->v);
  return ret;
}

pcs_ret_t
pcs_lod_selector_get_dp_based(pcs_lod_selector_t *self,
                              pcs_lod_version_t **selections_ptr)
{
  if (self->v == PCSTREAM_NULL)
  {
    return PCSTREAM_RET_FAIL;
  }
  *selections_ptr = self->v;
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_lod_selector_post_lm_based(pcs_lod_selector_t *self,
                                         pcs_count_t         n_mod,
                                         pcs_lod_version_t   n_ver,
                                         void    *metadata,
                                         size_t   metadata_size,
                                         void    *attrib,
                                         pcs_bw_t bandwidth)
{
  return PCSTREAM_RET_FAIL;
}
pcs_ret_t pcs_lod_selector_post_equal(pcs_lod_selector_t *self,
                                      pcs_count_t         n_mod,
                                      pcs_lod_version_t   n_ver,
                                      void               *metadata,
                                      size_t   metadata_size,
                                      void    *attrib,
                                      pcs_bw_t bandwidth)
{
  return PCSTREAM_RET_FAIL;
}
pcs_ret_t pcs_lod_selector_post_hybrid(pcs_lod_selector_t *self,
                                       pcs_count_t         n_mod,
                                       pcs_lod_version_t   n_ver,
                                       void               *metadata,
                                       size_t   metadata_size,
                                       void    *attrib,
                                       pcs_bw_t bandwidth)
{
  return PCSTREAM_RET_FAIL;
}
pcs_ret_t
pcs_lod_selector_get_lm_based(pcs_lod_selector_t *self,
                              pcs_lod_version_t **selections_ptr)
{
  return PCSTREAM_RET_FAIL;
}
pcs_ret_t
pcs_lod_selector_get_equal(pcs_lod_selector_t *self,
                           pcs_lod_version_t **selections_ptr)
{
  return PCSTREAM_RET_FAIL;
}
pcs_ret_t
pcs_lod_selector_get_hybrid(pcs_lod_selector_t *self,
                            pcs_lod_version_t **selections_ptr)
{
  return PCSTREAM_RET_FAIL;
}
