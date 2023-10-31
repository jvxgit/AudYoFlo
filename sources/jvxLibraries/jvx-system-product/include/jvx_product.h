#ifndef __JVX_PRODUCT_H__
#define __JVX_PRODUCT_H__

#include "jvx_ns.h"

/* // Typedefs */
#include "typedefs/jvxTools/TjvxAudioFileIo.h"
#include "typedefs/jvxTools/TjvxGenericWrapperApi.h"

/* // Interfaces */
#include "interfaces/IjvxExternalAudioChannels.h"
#include "interfaces/jvxTools/IjvxRtAudioFileReader.h"
#include "interfaces/jvxTools/IjvxRtAudioFileWriter.h"
#include "interfaces/jvxTools/IjvxResampler.h"
#include "interfaces/jvxTools/IjvxDataConverter.h"

#include "interfaces/property-extender/IjvxPropertyExtenderStreamAudioPlugin.h"
#include "interfaces/property-extender/IjvxPropertyExtenderDirectMixerControl.h"
#include "interfaces/property-extender/IjvxPropertyExtenderHrtfDispenser.h"
#include "interfaces/property-extender/IjvxPropertyExtenderChainControl.h"
#include "interfaces/property-extender/IjvxPropertyExtenderHeadTrackerProvider.h"
#include "interfaces/property-extender/IjvxPropertyExtenderNftfProvider.h"
#include "interfaces/property-extender/IjvxPropertyExtenderSpatialDirectionDirect.h"

#include "interfaces/jvxCommunication/IjvxAudioEncoder.h"
#include "interfaces/jvxCommunication/IjvxAudioDecoder.h"
#include "interfaces/jvxCommunication/IjvxAudioCodec.h"

/* // Helper functions etc */
#include "jvx-helpers-product.h"

#include "codeFragments/general/jvx_audio_common_config.h"

#endif
