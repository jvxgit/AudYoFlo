#include "uMainWindow.h"

#include "jvxAuTGenericWrapper.h"
#include "jvxAuTAsio.h"
#include "jvxAuNNoiseReductionMc.h"

jvxErrorType
uMainWindow::bootup_specific()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	IjvxConfigurationLine* theConfigLines = NULL;

	res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLines));
	if((res == JVX_NO_ERROR) && theConfigLines)
	{
		theConfigLines->add_line(JVX_COMPONENT_AUDIO_DEVICE, "Use the audio parameters from the audio device in the audio node and the camera");
		theConfigLines->add_slave_line(0, JVX_COMPONENT_AUDIO_NODE);

		for(i = 0; i < JVX_NUM_CONFIGURATIONLINE_PROPERTIES_AUDIO; i++)
		{
			jvxSize id = 0;
			if(jvx_findPropertyDescriptor(jvxProperties_configLine_audio[i], &id, NULL, NULL, NULL))
			{
				theConfigLines->add_configuration_property_line(0, id);
			}
		}

		this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle*>(theConfigLines));
	}

	/*
	* Add the types to be handled by host 
	*/
	IjvxHostTypeHandler* theTypeHandler = NULL;
	res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle**>(&theTypeHandler));
	if((res == JVX_NO_ERROR) && theTypeHandler)
	{
		jvxComponentType tp[2];

		tp[0] = JVX_COMPONENT_AUDIO_TECHNOLOGY;
		tp[1] = JVX_COMPONENT_AUDIO_DEVICE;
		res = theTypeHandler->add_type_host(tp, 2, "Jvx Audio Technologies", "audio", JVX_COMPONENT_TYPE_TECHNOLOGY);
		assert(res == JVX_NO_ERROR);

		tp[0] = JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY;
		tp[1] = JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE;
		res = theTypeHandler->add_type_host(tp, 2, "Jvx External Controller Technologies", "ex_control", JVX_COMPONENT_TYPE_TECHNOLOGY);
		assert(res == JVX_NO_ERROR);

		tp[0] = JVX_COMPONENT_AUDIO_NODE;
		tp[1] = JVX_COMPONENT_UNKNOWN;
		res = theTypeHandler->add_type_host(tp, 1, "Jvx Audio Node", "audio_node", JVX_COMPONENT_TYPE_NODE);
		assert(res == JVX_NO_ERROR);

		/*			theTypeHandler->add_type_host(JVX_COMPONENT_SCANNER_TECHNOLOGY, "", "camera", JVX_COMPONENT_TYPE_TECHNOLOGY);
		theTypeHandler->add_type_host(JVX_COMPONENT_SCANNER_TECHNOLOGY, "", "lamp", JVX_COMPONENT_TYPE_TECHNOLOGY);
		theTypeHandler->add_type_host(JVX_COMPONENT_SCANNER_TECHNOLOGY, "", "spectrum_processor", JVX_COMPONENT_TYPE_TECHNOLOGY);

		theTypeHandler->add_type_host(JVX_COMPONENT_SCANNER_TECHNOLOGY, "", "", JVX_COMPONENT_TYPE_ENDPOINT);
		*/
		this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle*>(theTypeHandler));
	}

	// Reference set before..
	//this->involvedComponents.theHost.hHost->add_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxQtWidgetHost*>(this)), JVX_INTERFACE_QT_WIDGET_HOST);

	// Do not allow that host components are loaded via DLL
	this->involvedComponents.theHost.hHost->add_component_load_blacklist(JVX_COMPONENT_HOST);
	this->involvedComponents.theHost.hHost->add_component_load_blacklist(JVX_COMPONENT_AUDIO_TECHNOLOGY);
	this->involvedComponents.theHost.hHost->add_component_load_blacklist(JVX_COMPONENT_AUDIO_NODE);

	this->involvedComponents.components.theWrapper = NULL;
	resL = jvxAuTGenericWrapper_init(&this->involvedComponents.components.theWrapper);
	assert(resL == JVX_NO_ERROR);

	this->involvedComponents.components.theAsioDriver = NULL;
	resL = jvxAuTAsio_init(&this->involvedComponents.components.theAsioDriver);
	assert(resL == JVX_NO_ERROR);

	this->involvedComponents.components.theAlgorithm = NULL;
	resL = JVX_LOAD_LIB_NODE(&this->involvedComponents.components.theAlgorithm);
	assert(resL == JVX_NO_ERROR);

	this->involvedComponents.theHost.hHost->add_external_component(this->involvedComponents.components.theWrapper, "Generic Wrapper Module");
	this->involvedComponents.theHost.hHost->add_external_component(this->involvedComponents.components.theAsioDriver, "Asio Module");
	this->involvedComponents.theHost.hHost->add_external_component(this->involvedComponents.components.theAlgorithm, "The Algorithm");

	return(res);
}

jvxErrorType
uMainWindow::shutdown_specific()
{
	jvxErrorType resL = JVX_NO_ERROR;
	
	this->involvedComponents.theHost.hHost->remove_external_component(this->involvedComponents.components.theWrapper);
	this->involvedComponents.theHost.hHost->remove_external_component(this->involvedComponents.components.theAsioDriver);
	this->involvedComponents.theHost.hHost->remove_external_component(this->involvedComponents.components.theAlgorithm);

	jvxAuTGenericWrapper_terminate(this->involvedComponents.components.theWrapper);
	jvxAuTAsio_terminate(this->involvedComponents.components.theAsioDriver);
	JVX_UNLOAD_LIB_NODE(this->involvedComponents.components.theAsioDriver);

	// This interface is handled separately.
	// this->involvedComponents.theHost.hHost->remove_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxQtWidgetHost*>(this)), JVX_INTERFACE_QT_WIDGET_HOST);
	return(JVX_NO_ERROR);
}


jvxErrorType
uMainWindow::connect_specific()
{
	return(JVX_NO_ERROR);
}