#ifndef __TJVXPROPERTYEXTERNALBUFFER_H__
#define __TJVXPROPERTYEXTERNALBUFFER_H__

namespace jvx
{
	typedef jvxErrorType(*report_bufferswitch_trigger)(jvxHandle* priv);
	typedef jvxErrorType(*report_bufferswitch_complete)(jvxHandle* priv);

	typedef enum
	{
		JVX_EXTERNAL_BUFFER_NONE = 0,
		JVX_EXTERNAL_BUFFER_1D_CIRCULAR_BUFFER = 1,
		JVX_EXTERNAL_BUFFER_2D_FIELD_BUFFER = 2
	} externalBufferType;

	typedef enum
	{
		JVX_EXTERNAL_BUFFER_SUB_NONE = 0,
		JVX_EXTERNAL_BUFFER_SUB_2D_FULL = 1,
		JVX_EXTERNAL_BUFFER_SUB_2D_INC = 2
	} externalBufferSubType;

#ifdef JVX_OS_WINDOWS
#pragma pack(push, 1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

	struct safeAccessBufferHdl
	{
		jvx_try_lock_buffer try_lockf;
		jvx_lock_buffer lockf;
		jvx_unlock_buffer unlockf;
		jvxHandle* priv;
		jvxBool allowRemove;
	};

	struct multichannel1DCircBufferHeader
	{
		jvxSize16 allowOverwrite;
		jvxSize numWraps;
	};

	struct multichannel2DFieldBufferHeader_common
	{
		jvxSize number_buffers;
		jvxSize seg_x;
		jvxSize seg_y;
	};

	struct multichannel2DFieldBufferHeader_full
	{
		multichannel2DFieldBufferHeader_common common;
		report_bufferswitch_trigger report_triggerf;
		jvxHandle* report_trigger_priv;
		report_bufferswitch_complete report_completef;
		jvxHandle* report_complete_priv;
	};

	struct multichannel2DFieldBufferHeader_inc
	{
		multichannel2DFieldBufferHeader_common common;
		//	jvxSize posi_start;
		//	jvxSize posi_stop;
		jvxSize num_lost;
	};

	struct externalBuffer
	{
		externalBufferType tp;
		externalBufferSubType subTp;
		jvxSize length;
		jvxSize number_channels;
		jvxSize idx_read;
		jvxSize fill_height;
		safeAccessBufferHdl safe_access;

		jvxByte* ptrFld;

		jvxDataFormat formFld;
		jvxDataFormatGroup subFormFld;
		jvxSize szFld;
		jvxData szElmFld;
		jvxSize numElmFldOneChanOneBuf;
		jvxSize numElmFldOneBuf;
		jvxSize numElmFld;

		union
		{
			multichannel1DCircBufferHeader the1DCircBuffer;
			multichannel2DFieldBufferHeader_full the2DFieldBuffer_full;
			multichannel2DFieldBufferHeader_inc the2DFieldBuffer_inc;
		} specific;
	};

#ifdef JVX_OS_WINDOWS
#pragma pack(pop)
#else
#pragma pack(pop)
#endif
};

typedef jvx::externalBuffer jvxExternalBuffer;

#endif



