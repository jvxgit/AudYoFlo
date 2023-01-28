//! Processing parameter struct for remote calls
typedef struct
{
	jvxInt32 samplerate;
	jvxInt32 channelsinput;
	jvxInt32 channelsoutput;
	jvxInt32 buffersize;
	jvxDataFormat format;
} jvxProcessingParameters;

typedef enum
{
	JVX_CONTROL_WINDOW_NONE = -1,
	JVX_CONTROL_WINDOW_QT = 0,		// Window handler in plugin is QT
	JVX_CONTROL_WINDOW_WIN32 = 1	// Window handler is type WIN32
} jvxWindowManagerType;
