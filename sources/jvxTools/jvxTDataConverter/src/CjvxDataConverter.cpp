#include "jvx.h"

#include "CjvxDataConverter.h"


#define QUANT_DOUBLE(a) (a>0.0?a+0.5:a-0.5)

CjvxDataConverter::CjvxDataConverter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_DATACONVERTER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDataConverter*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	
	whichConversion = ERROR_ERROR;
	
}

CjvxDataConverter::~CjvxDataConverter()
{
}

jvxErrorType 
	CjvxDataConverter::initialize(IjvxHiddenInterface* hostRef)
{
	jvxErrorType res = CjvxObject::_initialize(hostRef);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_select();
	}

	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_activate();
	}
	return(res);
}


jvxErrorType
CjvxDataConverter::prepare(jvxDataFormat inForm, jvxDataFormat outForm)
{
	jvxErrorType res = CjvxObject::_prepare();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_start();
	}

	if(res == JVX_NO_ERROR)
	{
		whichConversion = ERROR_ERROR;
		switch(inForm)
		{
		case JVX_DATAFORMAT_DATA:
			switch(outForm)
			{
			case JVX_DATAFORMAT_DATA:
				whichConversion = DATA_DATA;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				whichConversion = DATA_INT32;
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				whichConversion = DATA_INT16;
				break;
			default:
				assert(0);
			}
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			switch(outForm)
			{
			case JVX_DATAFORMAT_DATA:
				whichConversion = INT32_DATA;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				whichConversion = INT32_INT32;
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				whichConversion = INT32_INT16;
				break;
			default:
				assert(0);
			}
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			switch(outForm)
			{
			case JVX_DATAFORMAT_DATA:
				whichConversion = INT16_DATA;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				whichConversion = INT16_INT32;
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				whichConversion = INT16_INT16;
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}

	}
	return(res);
}

jvxErrorType 
CjvxDataConverter::process(const jvxHandle* fldIn, jvxHandle* fldOut, jvxSize numSamples)
{
	int i;
	jvxData tmp;
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		jvxData* fldDblIn = (jvxData*)fldIn;
		jvxInt32* fldInt32In = (jvxInt32*)fldIn;
		jvxInt16* fldInt16In = (jvxInt16*)fldIn;

		jvxData* fldDblOut = (jvxData*)fldOut;
		jvxInt32* fldInt32Out = (jvxInt32*)fldOut;
		jvxInt16* fldInt16Out = (jvxInt16*)fldOut;

		switch(whichConversion)
		{
		case DATA_DATA:
			for(i=0; i < numSamples; i++)
			{
				*fldDblOut++ = *fldDblIn++;
			}
			break;
		case DATA_INT32:
			for(i=0; i < numSamples; i++)
			{
				tmp = *fldDblIn++ * JVX_MAX_INT_32_DATA;
				*fldInt32Out++ = (jvxInt32)(QUANT_DOUBLE(tmp));
			}
			break;
		case DATA_INT16:
			for(i=0; i < numSamples; i++)
			{
				tmp = *fldDblIn++ * JVX_MAX_INT_16_DATA;
				*fldInt16Out++ = (jvxInt16)(QUANT_DOUBLE(tmp));
			}
			break;
		case INT32_DATA:
			for(i=0; i < numSamples; i++)
			{
				tmp = (jvxData)(*fldInt32In++);
				*fldDblOut++ = tmp * JVX_MAX_INT_32_DIV;
			}
			break;
		case INT32_INT32:
			memcpy(fldInt32Out, fldInt32In, sizeof(jvxInt32)* numSamples);
			break;
		case INT32_INT16:
			for(i=0; i < numSamples; i++)
			{
				*fldInt16Out++ = ((*fldInt32In++) >> 16);
			}
			break;

		case INT16_DATA:
			for(i=0; i < numSamples; i++)
			{
				tmp = (jvxData)(*fldInt16In++);
				*fldDblOut++ = tmp * JVX_MAX_INT_16_DIV;
			}
			break;
		case INT16_INT32:
			for(i=0; i < numSamples; i++)
			{
				*fldInt32Out++ = (((jvxInt32)*fldInt16In++) << 16);
			}
			break;
		case INT16_INT16:
			memcpy(fldInt16Out, fldInt16In, sizeof(jvxInt16)* numSamples);
			break;
		default: 
			res = JVX_ERROR_UNEXPECTED;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
CjvxDataConverter::postprocess()
{
	jvxErrorType res = CjvxObject::_stop();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_postprocess();
	}

	if(res == JVX_NO_ERROR)
	{
		whichConversion = ERROR_ERROR;
	}
	return(res);
}


jvxErrorType 
CjvxDataConverter::terminate()
{
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		this->postprocess();
	}

	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		CjvxObject::_deactivate();
		CjvxObject::_unselect();
		CjvxObject::_terminate();
	}
	return(JVX_NO_ERROR);
}

