#include "CjvxPaFfmpeg.h"

#ifndef JVX_COMPONENT_ACCESS_CALLING_CONVENTION
#define JVX_COMPONENT_ACCESS_CALLING_CONVENTION JVX_CALLINGCONVENTION
#endif

extern "C"
{
	jvxErrorType JVX_COMPONENT_ACCESS_CALLING_CONVENTION jvxAuCFfmpeg_init(IjvxObject** retObject,
			IjvxGlobalInstance** ret_glob , IjvxObject* templ );
	jvxErrorType JVX_COMPONENT_ACCESS_CALLING_CONVENTION jvxAuCFfmpeg_terminate(IjvxObject* cls);

	jvxErrorType JVX_COMPONENT_ACCESS_CALLING_CONVENTION jvxAuTFFMpegReader_init(IjvxObject** retObject,
		IjvxGlobalInstance** ret_glob, IjvxObject* templ);
	jvxErrorType JVX_COMPONENT_ACCESS_CALLING_CONVENTION jvxAuTFFMpegReader_terminate(IjvxObject* cls);

	jvxErrorType JVX_COMPONENT_ACCESS_CALLING_CONVENTION jvxAuTFFMpegWriter_init(IjvxObject** retObject,
		IjvxGlobalInstance** ret_glob, IjvxObject* templ);
	jvxErrorType JVX_COMPONENT_ACCESS_CALLING_CONVENTION jvxAuTFFMpegWriter_terminate(IjvxObject* cls);

}

CjvxPaFfmpeg::CjvxPaFfmpeg(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	JVX_DECLARE_OBJECT_REFERENCES(tpComp, IjvxPackage);
}

jvxErrorType 
CjvxPaFfmpeg::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_PACKAGE:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxPackage*>(this));
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
};

jvxErrorType 
CjvxPaFfmpeg::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_PACKAGE:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxPackage*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

jvxErrorType
CjvxPaFfmpeg::object_hidden_interface(IjvxObject** objRef) 
{
	if (objRef)
	{
		*objRef = static_cast<IjvxObject*>(this);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxPaFfmpeg::number_components(jvxSize* numOnReturn) 
{
	if (numOnReturn)
	{
		*numOnReturn = 3;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxPaFfmpeg::request_entries_component(jvxSize idx,
	jvxApiString* description, jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	switch (idx)
	{
	case 0:
		if (funcInit && funcTerm && description)
		{
			description->assign("FFMpeg Audio File Input");
			*funcInit = jvxAuTFFMpegReader_init;
			*funcTerm = jvxAuTFFMpegReader_terminate;
			res = JVX_NO_ERROR;
		}
		break;
	case 1:
		if (funcInit && funcTerm && description)
		{
			description->assign("FFMpeg Audio File Output");
			*funcInit = jvxAuTFFMpegWriter_init;
			*funcTerm = jvxAuTFFMpegWriter_terminate;
			res = JVX_NO_ERROR;
		}
		break;
	case 2:
		if (funcInit && funcTerm && description)
		{
			description->assign("Ffmpeg Audio Codec");
			*funcInit = jvxAuCFfmpeg_init;
			*funcTerm = jvxAuCFfmpeg_terminate;
			res = JVX_NO_ERROR;
		}
		break;
	default:
		break;
	}
	return res;
}

jvxErrorType 
CjvxPaFfmpeg::release_entries_component(jvxSize idx)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	switch (idx)
	{
	case 0:
	case 1:
	case 2:
		res = JVX_NO_ERROR;
		break;
	default:
		break;
	}
	return res;
}
