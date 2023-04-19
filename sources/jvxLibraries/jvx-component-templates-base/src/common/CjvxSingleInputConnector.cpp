#include "common/CjvxSingleInputConnector.h"

CjvxSingleInputConnector::CjvxSingleInputConnector() 
{
}

jvxErrorType 
CjvxSingleInputConnector::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleInputConnector::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		return JVX_NO_ERROR;
	}

jvxErrorType 
CjvxSingleInputConnector::number_next(jvxSize*) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleInputConnector::reference_next(jvxSize, IjvxConnectionIterator**) 
	{
		return JVX_NO_ERROR;
	}

jvxErrorType 
CjvxSingleInputConnector::reference_component(jvxComponentIdentification*, jvxApiString*, jvxApiString*)
	{
		return JVX_NO_ERROR;
	}

