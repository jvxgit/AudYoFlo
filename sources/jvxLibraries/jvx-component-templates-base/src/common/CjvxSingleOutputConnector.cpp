#include "common/CjvxSingleOutputConnector.h"

CjvxSingleOutputConnector::CjvxSingleOutputConnector()
{
}

jvxErrorType 
CjvxSingleOutputConnector::activate(IjvxObject* theObj, IjvxConnectorFactory* conFac, const std::string& nm, 
	CjvxSingleConnector_report<CjvxSingleOutputConnector>* reportArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxInputOutputConnectorCore::activate(theObj, conFac, nullptr, nm);
	assert(res == JVX_NO_ERROR);

	res = CjvxOutputConnectorCore::activate(this, this);
	assert(res == JVX_NO_ERROR);

	report = reportArg;
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

jvxErrorType
CjvxSingleOutputConnector::updateFixedProcessingArgs(const jvxLinkDataDescriptor_con_params& params, jvxBool requesTestChain)
{
	// We only accept ONE setting!!
	neg_output._update_parameters_fixed(params.number_channels,
		params.buffersize, params.rate,
		params.format, params.format_group,
		JVX_DATAFLOW_PUSH_ON_PULL, nullptr);
	return JVX_NO_ERROR;
}