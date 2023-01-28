#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementJvxNetworkSlaveDevice.h"
#include "jvx-qt-helpers.h"
#include <QVariant>

void 
CjvxSaWrapperElementJvxNetworkSlaveDevice::onButtonListen()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt16 valI16 = c_false;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;
	std::string command;
	jvxApiString fldStr;
	jvxApiString fldStrP;
	std::string txtError;
	std::string getSetTag = propertyGetSetTag;
	getSetTag = jvx_makePathExpr(propPrefix, getSetTag);
	propName = jvx_makePathExpr(getSetTag, "JVX_SOCKET_CONN_STATE");
	ident.reset(propName.c_str());
	trans.reset(true);
	res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, propName, propRef))
	{
		if (jvx_bitTest(selLst.bitFieldSelected(), 0))
		{
			// Trigger start listening
			command = "listen();";
		}
		else
		{
			// Trigger stop listening
			command = "disconnect();";
		}
		fldStr.assign_const(command.c_str(), command.size());
		propName = jvx_makePathExpr(propertyGetSetTag, "JVX_SOCKET_COMMAND");
		ident.reset(propName.c_str());
		trans.reset(true, 0, thePropDescriptor.decTp, false);
		res = propRef->set_property(callGate, 
			jPRG(&fldStr, 1, JVX_DATAFORMAT_STRING),
			ident, trans);
		if (res != JVX_NO_ERROR)
		{
			txtError = "Error sending command " + command + " to audio device";
			if (res == JVX_ERROR_INVALID_SETTING)
			{
				ident.reset(propName.c_str());
				trans.reset(false);
				propRef->get_property(callGate, jPRG(&fldStrP, 1, JVX_DATAFORMAT_STRING), ident, trans);
				txtError += ", reason: ";
				txtError += fldStrP.std_str();
			}
			else
			{
				txtError += jvxErrorType_descr(res);
			}
			std::cout << txtError << std::endl;
		}
		if (backwardRef)
		{
			backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
				myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
		}
	}
}

void 
CjvxSaWrapperElementJvxNetworkSlaveDevice::onEditPort()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxInt32 valI32 = 0;
	QString str = theReferences.lineEdit_connectionPort->text();
	valI32 = str.toInt();
	propName = jvx_makePathExpr(propertyGetSetTag, "JVX_SOCKET_HOSTPORT");
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, 
		jPRG(&valI32, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
	assert(res == JVX_NO_ERROR);
	if (backwardRef)
	{
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

// ========================================================================00

void
CjvxSaWrapperElementJvxNetworkSlaveDevice::onNewBuffersize()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxSize valS = 0;
	QString str = theReferences.lineEdit_buffersize->text();
	valS = str.toInt();
	propName = jvx_makePathExpr(propertyGetSetTag, "audioconfig/buffersize");
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, 
		jPRG(&valS, 1, JVX_DATAFORMAT_SIZE),
		ident, trans);
	assert(res == JVX_NO_ERROR);
	if (backwardRef)
	{
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

void
CjvxSaWrapperElementJvxNetworkSlaveDevice::onNewSamplerate()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxSize valS = 0;
	QString str = theReferences.lineEdit_samplerate->text();
	valS = str.toInt();
	propName = jvx_makePathExpr(propertyGetSetTag, "audioconfig/samplerate");
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, 
		jPRG(&valS, 1, JVX_DATAFORMAT_SIZE),
		ident, trans);
	assert(res == JVX_NO_ERROR);
	if (backwardRef)
	{
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

void
CjvxSaWrapperElementJvxNetworkSlaveDevice::onNewNumInChannels()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxSize valS = 0;
	QString str = theReferences.lineEdit_numberInChannels->text();
	valS = str.toInt();
	propName = jvx_makePathExpr(propertyGetSetTag, "audioconfig/numberinputchannels");
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, 
		jPRG(&valS, 1, JVX_DATAFORMAT_SIZE), ident, trans);
	assert(res == JVX_NO_ERROR);
	if (backwardRef)
	{
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

void
CjvxSaWrapperElementJvxNetworkSlaveDevice::onNewNumOutChannels()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxSize valS = 0;
	QString str = theReferences.lineEdit_numberOutChannels->text();
	valS = str.toInt();
	propName = jvx_makePathExpr(propertyGetSetTag, "audioconfig/numberoutputchannels");
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate, 
		jPRG(&valS, 1, JVX_DATAFORMAT_SIZE), ident, trans);
	assert(res == JVX_NO_ERROR);
	if (backwardRef)
	{
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

void
CjvxSaWrapperElementJvxNetworkSlaveDevice::onNewFormat(int idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxInt16 valI16 = idx;
	propName = jvx_makePathExpr(propertyGetSetTag, "audioconfig/audioformat");
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate,
		jPRG(&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	assert(res == JVX_NO_ERROR);
	if (backwardRef)
	{
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}

void 
CjvxSaWrapperElementJvxNetworkSlaveDevice::onClickAudioAuto()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxInt16 valI16 = 0;
	propName = jvx_makePathExpr(propertyGetSetTag, "audioconfig/automode");
	ident.reset(propName.c_str());
	trans.reset(true);
	res = propRef->get_property(callGate, jPRG(&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	assert(res == JVX_NO_ERROR);

	if (valI16 == c_false)
		valI16 = c_true;
	else
		valI16 = c_false;
	ident.reset(propName.c_str());
	trans.reset(true, 0, thePropDescriptor.decTp, false);
	res = propRef->set_property(callGate,
		jPRG(&valI16, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	assert(res == JVX_NO_ERROR);

	if (backwardRef)
	{
		backwardRef->reportPropertySet(selector_lst.c_str(), thePropDescriptor.descriptor.c_str(), myBaseProps.group_id_emit,
			myBaseProps.reportTp, static_cast<CjvxSaWrapperElementBase*>(this), res);
	}
}
