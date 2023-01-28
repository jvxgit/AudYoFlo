#include "jvx.h"
#include "CjvxAudioNodeSimple.h"

CjvxAudioNodeSimple::CjvxAudioNodeSimple(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);
	
}

CjvxAudioNodeSimple::~CjvxAudioNodeSimple()
{
}

jvxErrorType 
CjvxAudioNodeSimple::activate()
{
	jvxErrorType res = CjvxAudioNode::activate();
	if(res == JVX_NO_ERROR)
	{
		genSimple_node::init__properties_active();
		genSimple_node::allocate__properties_active();
		genSimple_node::register__properties_active(static_cast<CjvxProperties*>(this));
	}
	return(res);
};

jvxErrorType 
CjvxAudioNodeSimple::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		genSimple_node::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genSimple_node::deallocate__properties_active();
	}
	return(res);
};

jvxErrorType 
CjvxAudioNodeSimple::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{

	jvxErrorType res = JVX_NO_ERROR;

	// This default function does not tolerate a lot of unexpected settings
	assert(theData->con_params.format == theData->con_compat.format);
	assert(theData->con_params.buffersize == theData->con_compat.buffersize);
	
	res = process_buffer(
		theData->con_data.buffers[idx_sender_to_receiver],
		theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender],
		theData->con_params.buffersize,
		theData->con_params.number_channels,
		theData->con_compat.number_channels,
		theData->con_params.format);

	return(res);
};

jvxErrorType 
CjvxAudioNodeSimple::process_buffer(jvxHandle** buffers_input, jvxHandle** buffers_output, jvxSize bSize,
		jvxSize numChans_input,jvxSize numChans_output,jvxDataFormat format)
{
	jvxSize ii, j;

	jvxData* ptrInDbl = NULL, *ptrOutDbl = NULL;
	jvxData* ptrInFlt = NULL, *ptrOutFlt = NULL;
	jvxInt64* ptrInInt64 = NULL, *ptrOutInt64 = NULL;
	jvxInt32* ptrInInt32 = NULL, *ptrOutInt32 = NULL;
	jvxInt16* ptrInInt16 = NULL, *ptrOutInt16 = NULL;
	jvxInt8* ptrInInt8 = NULL, *ptrOutInt8 = NULL;
	jvxData tmp;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize maxChans = JVX_MAX(numChans_input, numChans_output);

	if(
		(numChans_input > 0) && 
		(numChans_output > 0))
	{
		for(ii = 0; ii < maxChans; ii++)
		{
			jvxSize idxIn = ii % numChans_input;
			jvxSize idxOut = ii % numChans_output;

			switch(format)
			{
			case JVX_DATAFORMAT_DATA:
				ptrInDbl = (jvxData*)buffers_input[idxIn];
				ptrOutDbl = (jvxData*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInDbl[j];
					tmp *= genSimple_node::properties_active.volume.value;
					ptrOutDbl[j] = tmp;
				}
				break;

			case JVX_DATAFORMAT_64BIT_LE:
				ptrInInt64 = (jvxInt64*)buffers_input[idxIn];
				ptrOutInt64 = (jvxInt64*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = (jvxData)ptrInInt64[j];
					tmp *= genSimple_node::properties_active.volume.value;
					ptrOutInt64[j] = (jvxInt64)tmp;
				}
				break;

			case JVX_DATAFORMAT_32BIT_LE:
				ptrInInt32 = (jvxInt32*)buffers_input[idxIn];
				ptrOutInt32 = (jvxInt32*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt32[j];
					tmp *= genSimple_node::properties_active.volume.value;
					ptrOutInt32[j] = (jvxInt32)tmp;
				}
				break;

			case JVX_DATAFORMAT_16BIT_LE:
				ptrInInt16 = (jvxInt16*)buffers_input[idxIn];
				ptrOutInt16 = (jvxInt16*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt16[j];
					tmp *= genSimple_node::properties_active.volume.value;
					ptrOutInt16[j] = (jvxInt16)tmp;
				}
				break;
			case JVX_DATAFORMAT_8BIT:
				ptrInInt8 = (jvxInt8*)buffers_input[idxIn];
				ptrOutInt8 = (jvxInt8*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt8[j];
					tmp *= genSimple_node::properties_active.volume.value;
					ptrOutInt8[j] = (jvxInt8)tmp;
				}
				break;
			default: 
				assert(0);
			}
		}
	}
	for(ii = 0; ii < genSimple_node::properties_active.numberdummyloop.value; ii++)
	{
		int a = 0;
		// Do nothing
	}
	return(res);
}
