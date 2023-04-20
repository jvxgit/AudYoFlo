#include "common/CjvxSingleOutputConnector.h"

CjvxSingleOutputConnector::CjvxSingleOutputConnector()
{
}

jvxErrorType 
CjvxSingleOutputConnector::activate(IjvxObject* theObj, IjvxConnectorFactory* conFac, const std::string& nm)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::activate(theObj, conFac, nullptr, nm);
	assert(res == JVX_NO_ERROR);

	res = CjvxOutputConnectorCore::activate(this, this);
	assert(res == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxSingleOutputConnector::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::deactivate();
	assert(res == JVX_NO_ERROR);

	res = CjvxOutputConnectorCore::deactivate();
	assert(res == JVX_NO_ERROR);

	return res;
}