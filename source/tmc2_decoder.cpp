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
#include <pcstream/tmc2_decoder.h>

using namespace pcc;
using namespace std;

extern int _decode_video(PCCDecoderParameters       &decoder_params,
                         const char               *data,
                         const size_t              size)
{
  PCCBitstream        bitstream;
  PCCBitstreamStat    bitstream_stat;
  PCCDecoder          decoder;
  PCCLogger           logger;
  size_t              frame_number = 0;
  size_t              header_size  = 0;
  SampleStreamV3CUnit ssvu;
  bool                b_more_data = 0;

  vector<uint8_t>     vec(reinterpret_cast<const uint8_t *>(data),
                      reinterpret_cast<const uint8_t *>(data) + size);
  logger.initilalize(
      removeFileExtension(decoder_params.compressedStreamPath_),
      false);

  if (!bitstream.initialize(vec))
    return PCSTREAM_RET_FAIL;
  // bitstream.computeMD5();

  bitstream_stat.setHeader(bitstream.size());

  frame_number = decoder_params.startFrameNumber_;

  decoder.setLogger(logger);

  decoder.setParameters(decoder_params);

  header_size = pcc::PCCBitstreamReader::read(bitstream, ssvu);
  bitstream_stat.incrHeader(header_size);

  b_more_data = true;
  while (b_more_data)
  {
    PCCGroupOfFrames   reconstructs;
    PCCContext         context;
    PCCBitstreamReader bitstream_reader;

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

      // this should be replaced to write to buff
      if (!decoder_params.reconstructedDataPath_.empty())
      {
        reconstructs.write(decoder_params.reconstructedDataPath_,
                           frame_number,
                           decoder_params.nbThread_);
      }
      else
        frame_number += reconstructs.getFrameCount();

      b_more_data = (ssvu.getV3CUnitCount() > 0);
    }
  }
  return 1;
}

extern "C"
{

  PCSTREAM_RET decode_video(const char *data, const size_t size)
  {
    PCCDecoderParameters decoder_params;
    int                  ret         = 0;

    decoder_params.startFrameNumber_ = 0;
    decoder_params.inverseColorSpaceConversionConfig_ =
        "yuv420torgb444.cfg";
    decoder_params.reconstructedDataPath_ =
        "del_me/tmp.%04d.ply";


    decoder_params.completePath();
    decoder_params.print();
    ret = _decode_video(decoder_params,
                        data,
                        size);
    if (ret == 0)
      return PCSTREAM_RET_FAIL;
    return PCSTREAM_RET_SUCCESS;
  }
}
