#include "jvx.h"
#include "myCentralWidget.h"
//#include <QWidget>

void myComponentFilterCallback(jvxBool* doNotLoad, const char* module_name, jvxComponentType tp, jvxHandle* priv)
{
	*doNotLoad = false;
	switch (tp)
	{
	case JVX_COMPONENT_HOST:
	//case JVX_COMPONENT_AUDIO_TECHNOLOGY:
	//case JVX_COMPONENT_CONFIG_PROCESSOR:
	//case JVX_COMPONENT_DATACONVERTER:
	//case JVX_COMPONENT_RESAMPLER:
	//case JVX_COMPONENT_RT_AUDIO_FILE_READER:
	//case JVX_COMPONENT_RT_AUDIO_FILE_WRITER:
	//case JVX_COMPONENT_CONNECTION:
	//case JVX_COMPONENT_THREADCONTROLLER:
		*doNotLoad = true;
	}
}

// =============================================================
// Configure link objects
// =============================================================
extern "C"
{
jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* description, jvxComponentType tp, jvxSize id)
{
	return(JVX_ERROR_ELEMENT_NOT_FOUND);		
}
}
// =============================================================
// Configure ui extensions
// =============================================================
JVX_INTERFACE IjvxQtComponentWidget;
jvxErrorType
IjvxQtComponentWidget_init(IjvxQtComponentWidget** onReturn, jvxBitField featureClass, const jvxComponentIdentification& cpId)
{
	if (onReturn)
		*onReturn = NULL;
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
IjvxQtComponentWidget_terminate(IjvxQtComponentWidget* returnMe, jvxBitField featureClass, const jvxComponentIdentification& cpId)
{
	assert(returnMe == NULL);
	return JVX_ERROR_UNSUPPORTED;
}

// =======================================================================

// =============================================================
// Main widget entry functions
// =============================================================
jvxErrorType
mainWindow_UiExtension_host_init(mainWindow_UiExtension_host** onReturn, QWidget* parent)
{
#ifdef JVX_PROJECT_NAMESPACE
	JVX_PROJECT_NAMESPACE::myCentralWidget* elm = new JVX_PROJECT_NAMESPACE::myCentralWidget(parent);
#else
	myCentralWidget* elm = new myCentralWidget(parent);
#endif
	if (onReturn)
	{
		*onReturn = static_cast<mainWindow_UiExtension_host*>(elm);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mainWindow_UiExtension_host_terminate(mainWindow_UiExtension_host* elm)
{
	delete(elm);
	return JVX_NO_ERROR;
}

const char* componentsOnLoad_audiotechnologies[] =
{
	"jvxAuTGenericWrapper",
	nullptr
};

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
	if(theFeaturesA)
	{
		theFeaturesA->cb_loadfilter = myComponentFilterCallback;
		theFeaturesA->cb_loadfilter_priv = NULL;

		theFeaturesA->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_TECHNOLOGY] = true;
		theFeaturesA->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_TECHNOLOGY] = componentsOnLoad_audiotechnologies;
	}
	
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}

}
