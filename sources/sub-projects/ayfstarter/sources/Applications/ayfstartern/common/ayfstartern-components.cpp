// =============================================================
// Involve components from static lib
// =============================================================

#include "jvx.h"
#include "ayfAuNMixer.h"
#include "ayfATStarter.h"
#include "ayfAuNStarter.h"
#include "jvxAuTSyncClock.h"
#include "jvxSpNMixChainEnterLeave.h"
#include "jvxSpNAsyncIo_sm.h"
#include "jvxSpNSynchronize.h"
#include "jvxAuNConvert.h"
#include "jvxAuNForwardBuffer.h"
#include "jvxAuNBitstreamDecoder.h"
#include "jvxAuNBitstreamEncoder.h"
#include "jvxSpNLevelControl.h"
#include "ayfViNCameraConvert.h"
#include "jvxViNOpenGLViewer.h"

#define WITH_VIDEO_MIXER

#ifdef WITH_VIDEO_MIXER
#include "jvxViNMixer.h"
#else
#include "jvxViNDefault.h"
#endif

#ifdef JVX_FFMPEG_FILE_IO
#include "jvxPaFfmpeg.h"
#endif

#define JVX_ADDITIONAL_AUDIO_TECHNOLOGIES \
	if (id == cnt) \
	{ \
		adescr->assign("SyncClock Audio"); \
		*funcInit = jvxAuTSyncClock_init; \
		*funcTerm = jvxAuTSyncClock_terminate; \
		return(JVX_NO_ERROR); \
	} \
	cnt++;

#define JVX_ALL_AUDIONODE_CASES \
	case 0: \
		adescr->assign("AYF Audio Mixer Algorithm"); \
		*funcInit = ayfAuNMixer_init; \
		*funcTerm = ayfAuNMixer_terminate; \
		return(JVX_NO_ERROR); \
	case 1: \
		adescr->assign("AYF Starter Algorithm"); \
		*funcInit = ayfAuNStarter_init; \
		*funcTerm = ayfAuNStarter_terminate; \
		return(JVX_NO_ERROR);

#ifdef WITH_VIDEO_MIXER
#define JVX_ALL_VIDEONODE_CASES \
	case 0: \
		adescr->assign("JVX Video Node"); \
		*funcInit = jvxViNMixer_init; \
		*funcTerm = jvxViNMixer_terminate; \
		return(JVX_NO_ERROR); \
	case 1: \
		adescr->assign("JVX Camera Video Converter Node"); \
		* funcInit = ayfViNCameraConvert_init; \
		* funcTerm = ayfViNCameraConvert_terminate; \
		return(JVX_NO_ERROR); \
	case 2: \
	adescr->assign("JVX Open GL Viewer Node"); \
	* funcInit = jvxViNOpenGLViewer_init; \
	* funcTerm = jvxViNOpenGLViewer_terminate; \
	return(JVX_NO_ERROR);


#else
#define JVX_ALL_VIDEONODE_CASES \
	case 0: \
		adescr->assign("JVX Video Node"); \
		*funcInit = jvxViNDefault_init; \
		*funcTerm = jvxViNDefault_terminate; \
		return(JVX_NO_ERROR); 
#endif

#define JVX_ALLOTHER_CLASSES \
	case JVX_COMPONENT_SYSTEM_AUTOMATION: \
		switch (id) \
		{ \
			case 0: \
				adescr->assign("Starter Automation"); \
				* funcInit = ayfATStarter_init; \
				* funcTerm = ayfATStarter_terminate; \
				return(JVX_NO_ERROR); \
				break; \
			default: \
				break; \
		} \
		break; \
	case JVX_COMPONENT_PACKAGE: \
		switch (id) \
		{ \
			case 0: \
				adescr->assign("FFMpeg Packager"); \
				*funcInit = jvxPaFfmpeg_init; \
				*funcTerm = jvxPaFfmpeg_terminate; \
				return(JVX_NO_ERROR); \
			default: \
				break; \
		} \
		break; 

#define JVX_ALL_SPNODE_CASES \
	case 0: \
		adescr->assign("MixChainEnterLeave"); \
		* funcInit = jvxSpNMixChainEnterLeave_init; \
		* funcTerm = jvxSpNMixChainEnterLeave_terminate; \
		return(JVX_NO_ERROR); \
		break; \
	case 1: \
		adescr->assign("Async I/O Node"); \
		*funcInit = jvxSpNAsyncIo_sm_init; \
		*funcTerm = jvxSpNAsyncIo_sm_terminate; \
		return(JVX_NO_ERROR); \
		break; \
	case 2: \
		adescr->assign("Synchronization"); \
		* funcInit = jvxSpNSynchronize_init; \
		* funcTerm = jvxSpNSynchronize_terminate; \
		return(JVX_NO_ERROR); \
		break; \
	case 3: \
		adescr->assign("Forward Buffer"); \
		*funcInit = jvxAuNForwardBuffer_init; \
		*funcTerm = jvxAuNForwardBuffer_terminate; \
		return(JVX_NO_ERROR); \
		break; \
	case 4: \
		adescr->assign("Audio Converter"); \
		*funcInit = jvxAuNConvert_init; \
		*funcTerm = jvxAuNConvert_terminate; \
		return(JVX_NO_ERROR); \
		break; \
	case 5: \
		adescr->assign("Bitstream Decoder"); \
		*funcInit = jvxAuNBitstreamDecoder_init; \
		*funcTerm = jvxAuNBitstreamDecoder_terminate; \
		return(JVX_NO_ERROR); \
		break; \
	case 6: \
		adescr->assign("Bitstream Encoder"); \
		*funcInit = jvxAuNBitstreamEncoder_init; \
		*funcTerm = jvxAuNBitstreamEncoder_terminate; \
		return(JVX_NO_ERROR); \
		break; \
	case 7: \
		adescr->assign("Level Control"); \
		*funcInit = jvxSpNLevelControl_init; \
		*funcTerm = jvxSpNLevelControl_terminate; \
		return(JVX_NO_ERROR); \
		break;

#include "jvxApplications/jvx_init_link_objects_tpl_multi.h"
#include "../common/ayfstartern-common.h"

// =============================================================
// Specify some runtime host configurations
// =============================================================
const jvxModuleOnStart componentsOnLoad_algorithms[] =
{
	{"ayfAuNMixer" },
	{"ayfAuNStarter" },
	NULL
};

const jvxModuleOnStart componentsOnLoad_audiotechnologies[] =
{
	{"jvxAuTGenericWrapper"}, // <- we could also involve the device directly, "jvxAuTSyncClock",
	{"jvxAuTGenericWrapper"},
#ifdef JVX_FFMPEG_FILE_IO
	{"jvxAuTFFMpegReader"},
	{"jvxAuTFFMpegWriter"},
#endif
	nullptr
};

const jvxModuleOnStart componentsOnLoad_automation[] =
{
	{"ayfATStarter"},
	nullptr
};

const jvxModuleOnStart componentsOnLoad_spnodes[] =
{
	{"jvxSpNMixChainEnterLeave"},
	{"jvxSpNMixChainEnterLeave"},
	NULL
};

const jvxModuleOnStart componentsOnLoad_videotechnologies[] =
{
#ifdef JVX_USE_MFVIDEO
	{"jvxViTMfWindows"},
#endif
#ifdef JVX_USE_V4L2VIDEO
	{"jvxViTV4L2"},
#endif
	NULL
};

const jvxModuleOnStart componentsOnLoad_videonodes[] =
{
#ifdef WITH_VIDEO_MIXER
	{"jvxViNMixer"},
#else
	{"jvxViNDefault"},
#endif
	NULL
};
