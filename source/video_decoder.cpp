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
#include <pcstream/video_decoder.h>

#if defined(USE_MPEG_VPCC_CODEC)
#include <PCCBitstream.h>
#include <PCCBitstreamReader.h>
#include <PCCChrono.h>
#include <PCCCommon.h>
#include <PCCContext.h>
#include <PCCDecoder.h>
#include <PCCDecoderParameters.h>
#include <PCCFrameContext.h>
#include <PCCGroupOfFrames.h>
#include <PCCMemory.h>
#endif

#if defined(USE_GOOGLE_DRACO_CODEC)
#include <draco/compression/decode.h>
#endif

#if defined(USE_FILRG_PCC_CODEC)
#include <pcc/pcc.h>
#endif

pcs_ret_t pcs_video_decoder_init(pcs_video_decoder_t *self, int type)
{
  *self = (pcs_video_decoder_t){0};
  pcs_gof_init(&(self->reconstruct));

  // the reason for using switch here instead of using #if and #else
  // is because one might dev a program that support multiple codec,
  // so ofcourse, the library should be able to create multiple type
  // of decoder
  switch (type)
  {
#if defined(USE_FILRG_PCC_CODEC)
  case PCSTREAM_VIDEO_DECODER_FILRG_PCC:
  {
    self->post = pcs_video_decoder_post_filrg_pcc;
    self->get  = pcs_video_decoder_get_filrg_pcc;
  }
  break;
#endif
#if defined(USE_MPEG_VPCC_CODEC)
  case PCSTREAM_VIDEO_DECODER_MPEG_VPCC:
  {
    self->post = pcs_video_decoder_post_mpeg_vpcc;
    self->get  = pcs_video_decoder_get_mpeg_vpcc;
  }
  break;
#endif
#if defined(USE_GOOGLE_DRACO_CODEC)
  case PCSTREAM_VIDEO_DECODER_GOOGLE_DRACO:
  {
    self->post = pcs_video_decoder_post_google_draco;
    self->get  = pcs_video_decoder_get_google_draco;
  }
  break;
#endif
  default:
    return PCSTREAM_RET_FAIL;
    break;
  }

  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t pcs_video_decoder_destroy(pcs_video_decoder_t *self)
{
  pcs_gof_destroy(&(self->reconstruct));
  *self = (pcs_video_decoder_t){0};
  return PCSTREAM_RET_SUCCESS;
}

#if defined(USE_FILRG_PCC_CODEC)
pcs_ret_t pcs_video_decoder_post_filrg_pcc(pcs_video_decoder_t *self,
                                           const char          *data,
                                           const size_t         size)
{
  pcc_point_cloud_t *pcds     = PCSTREAM_NULL;
  uint64_t             num_pcds = 0;

  pcc_multiple_decode(data, (uint64_t)size, &pcds, &num_pcds);

  pcs_gof_destroy((&self->reconstruct));
  pcs_gof_init((&self->reconstruct));

  self->reconstruct.size   = num_pcds;
  self->reconstruct.frames = (pcs_point_cloud_t *)malloc(
      sizeof(pcs_point_cloud_t) * num_pcds);

  for (uint64_t i = 0; i < num_pcds; i++)
  {
    pcs_point_cloud_init(&(self->reconstruct.frames[i]));

    self->reconstruct.frames[i].positions = pcds[i].positions;
    self->reconstruct.frames[i].colors    = pcds[i].colors;
    self->reconstruct.frames[i].size      = (size_t)pcds[i].size;
  }
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t pcs_video_decoder_get_filrg_pcc(pcs_video_decoder_t *self,
                                          pcs_gof_t *reconstruct)
{
  *reconstruct = self->reconstruct;
  return PCSTREAM_RET_SUCCESS;
}
#endif

#if defined(USE_MPEG_VPCC_CODEC)

static int
_mpeg_vpcc_decode_video(pcc::PCCDecoderParameters &decoder_params,
                        const char                *data,
                        const size_t               size,
                        pcs_gof_t                 *recons)
{
  pcc::PCCBitstream        bitstream;
  pcc::PCCBitstreamStat    bitstream_stat;
  pcc::PCCDecoder          decoder;
  pcc::PCCLogger           logger;
  size_t                   frame_number = 0;
  size_t                   header_size  = 0;
  pcc::SampleStreamV3CUnit ssvu;
  bool                     b_more_data = 0;

  std::vector<uint8_t> vec(reinterpret_cast<const uint8_t *>(data),
                           reinterpret_cast<const uint8_t *>(data) +
                               size);
  logger.initilalize(
      pcc::removeFileExtension(decoder_params.compressedStreamPath_),
      false);

  if (!bitstream.initialize(vec))
    return PCSTREAM_RET_FAIL;

  bitstream_stat.setHeader(bitstream.size());

  frame_number = decoder_params.startFrameNumber_;

  decoder.setLogger(logger);

  decoder.setParameters(decoder_params);

  header_size = pcc::PCCBitstreamReader::read(bitstream, ssvu);
  bitstream_stat.incrHeader(header_size);

  b_more_data = true;
  while (b_more_data)
  {
    pcc::PCCGroupOfFrames   reconstructs;
    pcc::PCCContext         context;
    pcc::PCCBitstreamReader bitstream_reader;

    context.setBitstreamStat(bitstream_stat);
    if (bitstream_reader.decode(ssvu, context) == 0)
    {
      return 0;
    }
    if (context.checkProfile() != 0)
    {
      return 0;
    }
    decoder_params.setReconstructionParameters(
        context.getVps()
            .getProfileTierLevel()
            .getProfileReconstructionIdc());
    decoder.setReconstructionParameters(decoder_params);

    context.resizeAtlas(context.getVps().getAtlasCountMinus1() + 1);
    for (uint32_t atlId = 0;
         atlId < context.getVps().getAtlasCountMinus1() + 1;
         atlId++)
    {
      int ret = 0;
      context.getAtlas(atlId).allocateVideoFrames(context, 0);
      context.setAtlasIndex(atlId);
      ret = decoder.decode(context, reconstructs, atlId);
      if (ret != 0)
        return ret;

      recons->copy(recons,
                   (void *)&reconstructs,
                   PCSTREAM_VIDEO_DECODER_MPEG_VPCC);
      b_more_data = (ssvu.getV3CUnitCount() > 0);
    }
  }
  return 1;
}

static pcs_ret_t
_decode_video(const char *data, const size_t size, pcs_gof_t *recons)
{
  pcc::PCCDecoderParameters decoder_params;
  int                       ret    = 0;

  decoder_params.startFrameNumber_ = 0;
  decoder_params.completePath();
  decoder_params.print();
  ret = _mpeg_vpcc_decode_video(decoder_params, data, size, recons);
  if (ret == 0)
    return PCSTREAM_RET_FAIL;
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_video_decoder_post_mpeg_vpcc(pcs_video_decoder_t *self,
                                           const char          *data,
                                           const size_t         size)
{
  return _decode_video(data, size, &(self->reconstruct));
}
pcs_ret_t pcs_video_decoder_get_mpeg_vpcc(pcs_video_decoder_t *self,
                                          pcs_gof_t *reconstruct)
{
  *reconstruct = self->reconstruct;
  return PCSTREAM_RET_SUCCESS;
}
#endif

#if defined(USE_GOOGLE_DRACO_CODEC)
pcs_ret_t pcs_video_decoder_post_google_draco(
    pcs_video_decoder_t *self, const char *data, const size_t size)
{
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t
pcs_video_decoder_get_google_draco(pcs_video_decoder_t *self,
                                   pcs_gof_t           *reconstruct)
{
  return PCSTREAM_RET_SUCCESS;
}
#endif
