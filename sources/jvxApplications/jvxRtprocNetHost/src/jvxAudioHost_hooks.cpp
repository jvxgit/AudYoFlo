#include "jvx.h"

#include "jvxAuTNetwork_master.h"
#include "jvxAuTNetwork_slave.h"

extern "C"
{
jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, 
	jvxApiString* description, jvxComponentType tp, jvxSize id)
{
	switch(tp)
	{
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:
		if(id == 0)
		{
			std::string descr = "Network Master Audio";
			jvxSize mL = JVX_MAXSTRING-1;
			*funcInit = jvxAuTNetwork_master_init;
			*funcTerm = jvxAuTNetwork_master_terminate;
			
			// description is a buffer of length JVX_MAXSTRING
			mL = JVX_MIN(descr.size(),mL);
			memcpy(description, descr.c_str(), mL);
			return(JVX_NO_ERROR);
		}
		if(id == 1)
		{
			std::string descr = "Network Slave Audio";
			jvxSize mL = JVX_MAXSTRING-1;
			*funcInit = jvxAuTNetwork_slave_init;
			*funcTerm = jvxAuTNetwork_slave_terminate;
			
			// description is a buffer of length JVX_MAXSTRING
			mL = JVX_MIN(descr.size(),mL);
			memcpy(description, descr.c_str(), mL);
			return(JVX_NO_ERROR);
		}
		break;
	default:
		break;
	}
	return(JVX_ERROR_ELEMENT_NOT_FOUND);
}
}

#include "CjvxQtComponentWidget_socket_master_dev.h"
#include "CjvxQtComponentWidget_socket_slave_dev.h"

#include "CjvxQtComponentWidget_socket_tech.h"

// =============================================================
// Configure ui extensions
// =============================================================
jvxErrorType
IjvxQtComponentWidget_init(IjvxQtComponentWidget** onReturn, jvxBitField featureClass, const jvxComponentIdentification& cpId)
{
	CjvxQtComponentWidget_socket_master_dev* newElmMDev = NULL;
	CjvxQtComponentWidget_socket_slave_dev* newElmSDev = NULL;
	CjvxQtComponentWidget_socket_tech* newElmTech = NULL;
	switch (cpId.tp)
	{
	case JVX_COMPONENT_AUDIO_DEVICE:
		if (JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER))
		{
			newElmMDev = new CjvxQtComponentWidget_socket_master_dev("VERBOSE=yes,RTUPDATE=yes");
			if (onReturn)
				*onReturn = static_cast<IjvxQtComponentWidget*>(newElmMDev);
			return JVX_NO_ERROR;
		}
		else if (JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE))
		{
			newElmSDev = new CjvxQtComponentWidget_socket_slave_dev("VERBOSE=yes,RTUPDATE=yes");
			if (onReturn)
				*onReturn = static_cast<IjvxQtComponentWidget*>(newElmSDev);
			return JVX_NO_ERROR;
		}
		break;

	case JVX_COMPONENT_AUDIO_TECHNOLOGY:

		/* Shared UI */
		if ((JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER) ||
			JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE)))
		{
			newElmTech = new CjvxQtComponentWidget_socket_tech("VERBOSE=yes,RTUPDATE=yes");

		}

		if (onReturn)
			*onReturn = static_cast<IjvxQtComponentWidget*>(newElmTech);
		return JVX_NO_ERROR;

		break;
	}
	if (onReturn)
		*onReturn = NULL;
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
IjvxQtComponentWidget_terminate(IjvxQtComponentWidget* returnMe, jvxBitField featureClass,
	const jvxComponentIdentification& cpId)
{
	switch (cpId.tp)
	{
	case JVX_COMPONENT_AUDIO_DEVICE:
		if ((JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER) ||
			JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE)))
		{
			delete(returnMe);
		}
		return JVX_NO_ERROR;
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:
		if ((JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER) ||
			JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE)))
		{
			delete(returnMe);
		}
		return JVX_NO_ERROR;
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

// =======================================================================


#include "mainCentral_host.h"

// =============================================================
// Main widget entry functions
// =============================================================
jvxErrorType
mainWindow_UiExtension_host_init(mainWindow_UiExtension_host** onReturn, QWidget* parent)
{
	mainCentral_host* newElm = new mainCentral_host(parent);
	if (onReturn)
	{
		*onReturn = static_cast<mainWindow_UiExtension_host*>(newElm);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mainWindow_UiExtension_host_terminate(mainWindow_UiExtension_host* elm)
{
	delete(elm);
	return JVX_NO_ERROR;
}

// =============================================================
// Specify some runtime host configurations
// =============================================================
#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"
extern "C"
{
jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	configureQtAudioHost_features* theFeaturesA = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesA), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
	if (theFeaturesA)
	{
	}
	
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}

}
