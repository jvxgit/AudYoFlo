//
//  jvxAudioProcessor_env.h
//  myJvxApp
//
//  Created by Hauke Krüger on 25.12.15.
//  Copyright © 2015 Javox Solutions. All rights reserved.
//

#ifndef jvxAudioProcessor_env_h
#define jvxAudioProcessor_env_h

// Common defines
typedef void (*callbackProcessOneBuffer)(void* priv);

#ifdef JVX_IOS_COMPILE_RTPROC_FWK

// Pull in jvx rtproc headers cpp
#include "jvx.h"
#include "jvx_config.h"

#else

#define JVX_MAP_CBS_MAIN_THREAD
typedef int jvxBool;

#ifndef true
#define true 1
#endif

#ifndef true
#define false 0
#endif

// Samplerates to test for listing to user
static int jvxUsefulSRates[] =
{
    8000, 11025, 16000, 22050, 32000, 44100,48000, 64000, 88200, 96000, 192000, -1
};


// Buffersizes to test for listing to user
static int jvxUsefulBSizes[] =
{
    16, 32, 48, 64, 96, 128, 160, 192, 256, 320, 384, 441, 480, 512, 640, 768, 882, 960, 1024, 1536, 2048, -1
};

typedef enum
{
    JVX_STATE_NONE = 0x0,
    JVX_STATE_INIT = 0x1,
    JVX_STATE_SELECTED = 0x2,			// Plugin has been opened but has not yet been initialized
    JVX_STATE_ACTIVE = 0x4,		// Plugin has currently been initialized
    JVX_STATE_PREPARED = 0x8,			// Plugin is ready for processing
    JVX_STATE_PROCESSING = 0x10,		// Plugin is in processing state
    JVX_STATE_COMPLETE = 0x20,		// Plugin is in processing state
    JVX_STATE_DONTCARE = 0xFFFFFFFF
} jvxState;

typedef enum
{
    JVX_NO_ERROR = 0,
//#include "typedefs/TpjvxTypes_errorcodes.h"
    JVX_ERROR_UNKNOWN,
    JVX_ERROR_INVALID_ARGUMENT,
    JVX_ERROR_WRONG_STATE,
    JVX_ERROR_ID_OUT_OF_BOUNDS,
    JVX_ERROR_UNSUPPORTED,
    JVX_ERROR_ELEMENT_NOT_FOUND,
    JVX_ERROR_DUPLICATE_ENTRY,
    JVX_ERROR_OPEN_FILE_FAILED,
    JVX_ERROR_UNEXPECTED,
    JVX_ERROR_NOT_READY,
    JVX_ERROR_COMPONENT_BUSY,
    JVX_ERROR_BUFFER_OVERFLOW,
    JVX_ERROR_BUFFER_UNDERRUN,
    JVX_ERROR_WRONG_STATE_SUBMODULE,
    JVX_ERROR_MESSAGE_QUEUE_FULL,
    JVX_ERROR_PARSE_ERROR,
    JVX_ERROR_WRONG_SECTION_TYPE,
    JVX_ERROR_INTERNAL,
    JVX_ERROR_EMPTY_LIST,
    JVX_ERROR_CALL_SUB_COMPONENT_FAILED,
    JVX_ERROR_THREADING_MISMATCH,
    JVX_ERROR_SYSTEMCALL_FAILED,
    JVX_ERROR_SIZE_MISMATCH,
    JVX_ERROR_READ_ONLY,
    JVX_ERROR_INVALID_SETTING,
    JVX_ERROR_TIMEOUT,
    JVX_ERROR_ABORT,
    JVX_ERROR_POSTPONE,
    JVX_ERROR_END_OF_FILE,
    JVX_ERROR_STANDARD_EXCEPTION,
    JVX_ERROR_SEQUENCER_TOOK_CONTROL,
    JVX_ERROR_THREAD_ONCE_MORE,
    JVX_ERROR_PROCESS_COMPLETE,
    JVX_ERROR_DSP,
    JVX_ERROR_ALREADY_IN_USE,
    
    JVX_ERROR_LIMIT
} jvxErrorType;

#endif

#endif /* jvxAudioProcessor_env_h */
