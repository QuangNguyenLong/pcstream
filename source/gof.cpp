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
#include <pcstream/gof.h>
#include <stdlib.h>

#if defined(USE_MPEG_VPCC_CODEC)
#include <PCCGroupOfFrames.h>
#include <PCCPointSet.h>
#endif

static pcs_ret_t _pcs_gof_free(pcs_gof_t *gof)
{
  if (gof->frames != PCSTREAM_NULL)
  {
    for (size_t i = 0; i < gof->size; i++)
      pcs_point_cloud_destroy(&(gof->frames[i]));
    free(gof->frames);
  }
  gof->frames = PCSTREAM_NULL;
  gof->size   = 0;
  return PCSTREAM_RET_SUCCESS;
}

static pcs_ret_t _pcs_gof_alloc(pcs_gof_t *gof, size_t size)
{
  if (gof == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;

  _pcs_gof_free(gof);

  gof->size = size;
  gof->frames =
      (pcs_point_cloud_t *)malloc(sizeof(pcs_point_cloud_t) * size);
  for (size_t i = 0; i < size; i++)
    pcs_point_cloud_init(&(gof->frames[i]));

  return PCSTREAM_RET_SUCCESS;
}

#if defined(USE_MPEG_VPCC_CODEC)
pcs_ret_t _pcs_gof_copy_mpeg_vpcc(pcs_gof_t *gof, void *src)
{
  pcc::PCCGroupOfFrames *pcc_gof     = PCSTREAM_NULL;
  size_t                 frame_count = 0;
  pcc_gof                            = (pcc::PCCGroupOfFrames *)src;
  frame_count                        = pcc_gof->getFrameCount();

  _pcs_gof_alloc(gof, frame_count);
  for (size_t i = 0; i < frame_count; i++)
  {
    gof->frames[i].copy(&(gof->frames[i]),
                        (void *)&((*pcc_gof)[i]),
                        PCSTREAM_VIDEO_DECODER_MPEG_VPCC);
  }
  return PCSTREAM_RET_SUCCESS;
}
#endif

pcs_ret_t pcs_gof_init(pcs_gof_t *self)
{
  *self      = (pcs_gof_t){0};
  self->copy = pcs_gof_copy;
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t pcs_gof_destroy(pcs_gof_t *self)
{
  _pcs_gof_free(self);
  *self = (pcs_gof_t){0};
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_gof_copy(pcs_gof_t             *self,
                       void                  *src,
                       PCSTREAM_VIDEO_DECODER decoder)
{
  switch (decoder)
  {
  case PCSTREAM_VIDEO_DECODER_MPEG_VPCC:
#if defined(USE_MPEG_VPCC_CODEC)
    return _pcs_gof_copy_mpeg_vpcc(self, src);
#endif
    break;
  default:
    return PCSTREAM_RET_SUCCESS;
    break;
  }
  return PCSTREAM_RET_FAIL;
}
