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
#include <pcstream/point_cloud.h>
#include <stdlib.h>

#if defined(USE_MPEG_VPCC_CODEC)
#include <PCCPointSet.h>
#endif

#if defined(USE_GOOGLE_DRACO_CODEC)
#endif

pcs_ret_t _pcs_point_cloud_free(pcs_point_cloud_t *pc)
{
  if (pc->positions != PCSTREAM_NULL)
    free(pc->positions);
  if (pc->colors != PCSTREAM_NULL)
    free(pc->colors);
  pc->positions = PCSTREAM_NULL;
  pc->colors    = PCSTREAM_NULL;
  pc->size      = 0;
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t _pcs_point_cloud_alloc(pcs_point_cloud_t     *pc,
                                    size_t                 size,
                                    PCSTREAM_VIDEO_DECODER decoder)
{
  // PCSTREAM_TYPE pos_type;
  size_t pos_type_size = 0;

  if (pc == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;

  _pcs_point_cloud_free(pc);

  // pos_type      = pcs_pos_type_g[decoder];
  // pos_type_size = pcs_type_size_g[pos_type];

  switch (decoder)
  {
  case PCSTREAM_VIDEO_DECODER_MPEG_VPCC:
      pos_type_size = sizeof(pcs_mpeg_vpcc_coord_t);
    break;
  case PCSTREAM_VIDEO_DECODER_MPEG_GPCC:
      pos_type_size = sizeof(pcs_mpeg_gpcc_coord_t);
    break;
  case PCSTREAM_VIDEO_DECODER_GOOGLE_DRACO:
      pos_type_size = sizeof(pcs_google_draco_coord_t);
    break;
  case PCSTREAM_VIDEO_DECODER_PCL:
      pos_type_size = sizeof(pcs_pcl_coord_t);
    break;
  default:
      pos_type_size = sizeof(pcs_mpeg_vpcc_coord_t);
    break;
  }

  pc->size      = size;
  pc->positions = malloc(pos_type_size * 3 * size);
  pc->colors =
      (unsigned char *)malloc(sizeof(unsigned char) * 3 * size);
  return PCSTREAM_RET_SUCCESS;
}

#if defined(USE_MPEG_VPCC_CODEC)
static pcs_ret_t
_pcs_point_cloud_copy_mpeg_vpcc(pcs_point_cloud_t *des, void *src)
{
  pcc::PCCPointSet3 *pcc_src = PCSTREAM_NULL;

  pcc_src                    = (pcc::PCCPointSet3 *)src;

  _pcs_point_cloud_alloc(des,
                         pcc_src->getPointCount(),
                         PCSTREAM_VIDEO_DECODER_MPEG_VPCC);

  memcpy(des->positions,
         pcc_src->getPositions().data(),
         des->size * 3 * sizeof(pcs_mpeg_vpcc_coord_t));

  memcpy(des->colors,
         pcc_src->getColors().data(),
         des->size * 3 * sizeof(unsigned char));

  return PCSTREAM_RET_SUCCESS;
}
#endif

pcs_ret_t pcs_point_cloud_init(pcs_point_cloud_t *self)
{
  *self      = (pcs_point_cloud_t){0};
  self->copy = pcs_point_cloud_copy;

  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t pcs_point_cloud_destroy(pcs_point_cloud_t *self)
{
  _pcs_point_cloud_free(self);
  *self = (pcs_point_cloud_t){0};

  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_point_cloud_copy(pcs_point_cloud_t     *self,
                                  void                  *src,
                                  PCSTREAM_VIDEO_DECODER decoder)
{
  switch (decoder)
  {
#if defined(USE_MPEG_VPCC_CODEC)
  case PCSTREAM_VIDEO_DECODER_MPEG_VPCC:
    return _pcs_point_cloud_copy_mpeg_vpcc(self, src);
    break;
#endif
  default:
    return PCSTREAM_RET_FAIL;
    break;
  }
  return PCSTREAM_RET_FAIL;
}
