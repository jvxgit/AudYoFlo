#include "jvx.h"
#include "CjvxAuNOpenGl.h"
#include "mcg_export_project.h"

CjvxAuNOpenGl::CjvxAuNOpenGl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);
	
}

CjvxAuNOpenGl::~CjvxAuNOpenGl()
{
}

#ifdef JVX_EXTERNAL_CALL_ENABLED
JVX_MEXCALL_SELECT(CjvxAuNOpenGl, CjvxAudioNode, _theExtCallObjectName)
JVX_MEXCALL_UNSELECT(CjvxAuNOpenGl, CjvxAudioNode, _theExtCallObjectName)
JVX_MEXCALL_PREPARE(CjvxAuNOpenGl, CjvxAudioNode)
JVX_MEXCALL_POSTPROCESS(CjvxAuNOpenGl, CjvxAudioNode)
JVX_MEXCALL_PREPARE_SENDER_TO_RECEIVER(CjvxAuNOpenGl, CjvxAudioNode)
JVX_MEXCALL_PREPARE_COMPLETE_RECEIVER_TO_SENDER(CjvxAuNOpenGl, CjvxAudioNode, &_common_set_ldslave.theData_out)
JVX_MEXCALL_POSTPROCESS_SENDER_TO_RECEIVER(CjvxAuNOpenGl, CjvxAudioNode)
JVX_MEXCALL_BEFORE_POSTPROCESS_RECEIVER_TO_SENDER(CjvxAuNOpenGl, CjvxAudioNode)
JVX_MEXCALL_PUT_CONFIGURATION(CjvxAuNOpenGl, CjvxAudioNode)
JVX_MEXCALL_GET_CONFIGURATION(CjvxAuNOpenGl, CjvxAudioNode)
#endif

jvxErrorType 
CjvxAuNOpenGl::activate()
{
	jvxErrorType res = CjvxAudioNode::activate();
	if(res == JVX_NO_ERROR)
	{
#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::activate();
#endif
		genSimple_node::init__properties_active();
		genSimple_node::allocate__properties_active();
		genSimple_node::register__properties_active(static_cast<CjvxProperties*>(this));
	}
	return(res);
};

jvxErrorType 
CjvxAuNOpenGl::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		genSimple_node::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genSimple_node::deallocate__properties_active();
#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::deactivate();
#endif

	}
	return(res);
};

jvxErrorType 
CjvxAuNOpenGl::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{

	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_EXTERNAL_CALL_ENABLED
	
	// Pass control to Matlab processing
	res = CjvxMexCalls::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender);

#else

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
		
#endif

	return(res);
};

jvxErrorType 
CjvxAuNOpenGl::process_buffer(jvxHandle** buffers_input, jvxHandle** buffers_output, jvxSize bSize,
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

	jvxData volume = genSimple_node::properties_active.volume.value;

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
					tmp *= volume;
					ptrOutDbl[j] = tmp;
				}
				break;

			case JVX_DATAFORMAT_64BIT_LE:
				ptrInInt64 = (jvxInt64*)buffers_input[idxIn];
				ptrOutInt64 = (jvxInt64*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt64[j];
					tmp *= volume;
					ptrOutInt64[j] = (jvxInt64)tmp;
				}
				break;

			case JVX_DATAFORMAT_32BIT_LE:
				ptrInInt32 = (jvxInt32*)buffers_input[idxIn];
				ptrOutInt32 = (jvxInt32*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt32[j];
					tmp *= volume;
					ptrOutInt32[j] = (jvxInt32)tmp;
				}
				break;

			case JVX_DATAFORMAT_16BIT_LE:
				ptrInInt16 = (jvxInt16*)buffers_input[idxIn];
				ptrOutInt16 = (jvxInt16*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt16[j];
					tmp *= volume;
					ptrOutInt16[j] = (jvxInt16)tmp;
				}
				break;
			case JVX_DATAFORMAT_8BIT:
				ptrInInt8 = (jvxInt8*)buffers_input[idxIn];
				ptrOutInt8 = (jvxInt8*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt8[j];
					tmp *= volume;
					ptrOutInt8[j] = (jvxInt16)tmp;
				}
				break;
			default: 
				assert(0);
			}
		}
	}
	return(res);
}

#ifdef JVX_EXTERNAL_CALL_ENABLED

jvxErrorType 
CjvxAuNOpenGl::myFirstMatlabEntry()
{
	return(JVX_NO_ERROR);
}

#endif

