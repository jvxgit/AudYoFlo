#include "common/CjvxSingleInputConnector.h"

CjvxSingleInputConnector::CjvxSingleInputConnector() 
{
}

jvxErrorType 
CjvxSingleInputConnector::activate(IjvxObject* theObj, 
	IjvxConnectorFactory* conFac, const std::string& nm,
	CjvxSingleInputConnector_report* reportArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::activate(theObj, conFac, nullptr, nm);
	assert(res == JVX_NO_ERROR);

	res = CjvxInputConnectorCore::activate(this, this);
	assert(res == JVX_NO_ERROR);

	this->report = reportArg;
	return res;
}

jvxErrorType
CjvxSingleInputConnector::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::deactivate();
	assert(res == JVX_NO_ERROR);

	res = CjvxInputConnectorCore::deactivate();
	assert(res == JVX_NO_ERROR);

	return res;
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

