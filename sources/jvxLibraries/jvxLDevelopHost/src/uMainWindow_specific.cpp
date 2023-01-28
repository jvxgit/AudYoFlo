#include "uMainWindow.h"
#include "uMainWindow_widgets.h"
#include "jvxQtDevelopHostHookupEntries.h"
#include "interfaces/all-hosts/jvxHostHookupEntries.h"

#include "jvxTconfigProcessor.h"
#include "jvxTSystemTextLog.h"
#include "jvxTThreads.h"
#include "jvxTDataLogger.h"

#include "jvxTrtAudioFileReader.h"
#include "jvxTrtAudioFileWriter.h"
#include "jvxAuTGenericWrapper.h"
#include "jvxTDataConverter.h"
#include "jvxTResampler.h"

jvxErrorType
uMainWindow::boot_negotiate_specific()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxApiString descriptionComponent;
	oneAddedStaticComponent comp;

	JVX_START_SLOTS_BASE(theHostFeatures.numSlotsComponents, _command_line_parameters_hosttype.num_slots_max, _command_line_parameters_hosttype.num_subslots_max);
	JVX_START_SLOTS_SUBPRODUCT(theHostFeatures.numSlotsComponents, _command_line_parameters_hosttype.num_slots_max, _command_line_parameters_hosttype.num_subslots_max);
	
	jvx_configure_factoryhost_features(static_cast<configureFactoryHost_features*>(&theHostFeatures));

	boot_initialize_base(theHostFeatures.numSlotsComponents);

	// 1
		// Open all tool components
	LOAD_ONE_MODULE_LIB_FULL(jvxTSystemTextLog_init, jvxTSystemTextLog_terminate, "System Text Log",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTconfigProcessor_init,
		jvxTconfigProcessor_terminate, "Config Parser",
		involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTThreads_init,
		jvxTThreads_terminate, "Threads",
		involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTDataLogger_init,
		jvxTDataLogger_terminate, "Data Logger",
		involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
		
	// Do not allow that host components are loaded via DLL
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_HOST);
	/*
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_SYSTEM_TEXT_LOG);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_CONFIG_PROCESSOR);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_THREADS);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_DATALOGGER);
	*/
	LOAD_ONE_MODULE_LIB_FULL(jvxTDataConverter_init,
		jvxTDataConverter_terminate,
		"Data Converter",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTResampler_init,
		jvxTResampler_terminate,
		"Resampler",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileReader_init,
		jvxTrtAudioFileReader_terminate,
		"RT Audio Reader",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileWriter_init,
		jvxTrtAudioFileWriter_terminate,
		"RT Audio Writer",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxAuTGenericWrapper_init,
		jvxAuTGenericWrapper_terminate,
		"Generic Wrapper",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);

	/*
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_DATACONVERTER);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_RESAMPLER);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_RT_AUDIO_FILE_READER);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_RT_AUDIO_FILE_WRITER);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_AUDIO_TECHNOLOGY);
	*/
	// 2 
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		jvxSize cnt = 0;
		while (1)
		{
			// memset(descriptionComponent, 0, JVX_MAXSTRING);
			comp.reset();
			resL = jvx_access_link_objects(&comp.funcInit, &comp.funcTerm, &descriptionComponent, (jvxComponentType)i, cnt);
			if (resL == JVX_NO_ERROR)
			{
				comp.theStaticObject = NULL;
				resL = comp.funcInit(&comp.theStaticObject, &comp.theStaticGlobal, NULL);
				assert(resL == JVX_NO_ERROR);
				assert(comp.theStaticObject);
				resL = this->involvedComponents.theHost.hFHost->add_external_component(comp.theStaticObject, 
					comp.theStaticGlobal, descriptionComponent.c_str(), true, comp.funcInit, comp.funcTerm);
				if (resL == JVX_NO_ERROR)
				{
					this->involvedComponents.addedStaticObjects.push_back(comp);
				}
				else
				{
					comp.funcTerm(comp.theStaticObject);
				}				
			}
			else
			{
				break;
			}
			cnt++;
		}
	}

	involvedComponents.theHost.hFHost->set_component_load_filter_function(theHostFeatures.cb_loadfilter, theHostFeatures.cb_loadfilter_priv);
	return(res);
}

jvxErrorType
uMainWindow::postbootup_specific()
{	
	return(JVX_NO_ERROR);
}

void
uMainWindow::bootDelayed_specific()
{
	jvxBool wantsToAdjust = false;
	subWidgets.main.theWidget->inform_bootup_complete(&wantsToAdjust);
	if(wantsToAdjust)
	{
		this->adjustSize();
	}
}
// ==========================================================================================

jvxErrorType
uMainWindow::shutdown_specific()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_HOST);

	for (i = 0; i < this->involvedComponents.addedStaticObjects.size(); i++)
	{
		UNLOAD_ONE_MODULE_LIB_FULL(this->involvedComponents.addedStaticObjects[i], involvedComponents.theHost.hFHost);
	}
	this->involvedComponents.addedStaticObjects.clear();

	// Release data connections and host type handler
	shutdown_terminate_base();

	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::connect_specific()
{

	return(JVX_NO_ERROR);
}
