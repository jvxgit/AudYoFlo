#ifndef __CJVXDATAPROCESSOR_H__
#define __CJVXDATAPROCESSOR_H__

#include "jvx.h"
#include "jvx-helpers.h"

class CjvxDataProcessor
{
protected:

	typedef struct
	{
		bool allocated_sender_to_receiver;
		bool allocated_receiver_to_sender;
		std::string rt_error_descr;
		jvxErrorType rt_error_type;
		jvxInt32 rt_error_id;
	} oneSlot;

	struct
	{
		jvxSize slotCnt;
		jvxState inDataLinkMode;

		std::vector<oneSlot> active_slots;

		IjvxObject* thisisme;

		jvxBool supportsMultiThreading;
	} _common_spec_proc_set;

public:

	CjvxDataProcessor()
	{
		_common_spec_proc_set.slotCnt = 0;
		_common_spec_proc_set.inDataLinkMode = JVX_STATE_INIT;
		_common_spec_proc_set.thisisme = NULL;
		_common_spec_proc_set.supportsMultiThreading = false;
	};

	jvxErrorType _data_link_init()
	{
		if(_common_spec_proc_set.inDataLinkMode  == JVX_STATE_INIT)
		{
			_common_spec_proc_set.slotCnt = 0;
			_common_spec_proc_set.inDataLinkMode = JVX_STATE_PREPARED;
			_common_spec_proc_set.active_slots.clear();

		}
		return(JVX_NO_ERROR);
	};

	jvxErrorType _data_link_start()
	{
		if(_common_spec_proc_set.inDataLinkMode  == JVX_STATE_PREPARED)
		{
			_common_spec_proc_set.inDataLinkMode = JVX_STATE_PROCESSING;
		}
		return(JVX_NO_ERROR);
	};

	jvxErrorType _data_link_stop()
	{
		if(_common_spec_proc_set.inDataLinkMode  == JVX_STATE_PROCESSING)
		{
			_common_spec_proc_set.inDataLinkMode = JVX_STATE_PREPARED;
		}
		return(JVX_NO_ERROR);
	};

	jvxErrorType _data_link_terminate()
	{
		if(_common_spec_proc_set.inDataLinkMode  == JVX_STATE_PREPARED)
		{
			_common_spec_proc_set.inDataLinkMode = JVX_STATE_INIT;
			_common_spec_proc_set.active_slots.clear();
		}
		return(JVX_NO_ERROR);
	};

	jvxErrorType _reference_object(IjvxObject** theObj)
	{
		if(theObj)
		{
			*theObj = _common_spec_proc_set.thisisme;
		}
		return(JVX_NO_ERROR);
	};
	

	jvxErrorType _supports_multithreading(jvxBool* supports)
	{
		if(supports)
		{
			*supports = _common_spec_proc_set.supportsMultiThreading;
		}
		return(JVX_NO_ERROR);
	};

	jvxErrorType _process_st( jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
	{
		// Talkthrough
		jvxSize ii;
		jvxSize maxChans = JVX_MAX(theData->con_params.number_channels,theData->con_compat.number_channels);
		jvxSize minChans = JVX_MIN(theData->con_params.number_channels,theData->con_compat.number_channels);

		// This default function does not tolerate a lot of unexpected settings
		assert(theData->con_params.format == theData->con_compat.format);
		assert(theData->con_params.buffersize == theData->con_compat.buffersize);

		if(minChans)
		{
			for(ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % theData->con_params.number_channels;
				jvxSize idxOut = ii % theData->con_compat.number_channels;

				assert(theData->con_params.format == theData->con_compat.format);
				jvx_convertSamples_memcpy(
					theData->con_data.buffers[idx_sender_to_receiver][idxIn],
					theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender][idxOut],
					jvxDataFormat_size[theData->con_params.format],
					theData->con_params.buffersize);
			}
		}
		return(JVX_NO_ERROR);
	};

	jvxErrorType _process_mt(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxSize* channelSelect, jvxSize numEntriesChannels, jvxInt32 offset_input, jvxInt32 offset_output, jvxInt32 numEntries)
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

	jvxErrorType _last_error_process(char* fld_text, jvxSize fldSize, jvxErrorType* err, jvxInt32* id_error, jvxLinkDataDescriptor* theData)
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxState theState = JVX_STATE_NONE;

		if(theState == JVX_STATE_PROCESSING)
		{
			assert(0);
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	};

};

#endif
