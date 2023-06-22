#include "uMainWindow.h"
#include <iostream>

#include "jvxAudioHost_common.h"
#include "jvxQtAudioHost_common.h"

#include "jvxTconfigProcessor.h"
#include "jvxTSystemTextLog.h"
#include "jvxTThreads.h"
#include "jvxTDataLogger.h"

#include "jvxTDataConverter.h"
#include "jvxTResampler.h"
#include "jvxTrtAudioFileReader.h"
#include "jvxTrtAudioFileWriter.h"
#include "jvxAuTGenericWrapper.h"
#include "jvxTLogRemoteHandler.h"

#include "jvxQtAudioHostHookupEntries.h"
#include "interfaces/all-hosts/jvxHostHookupEntries.h"

#include "CjvxJson.h"

#ifndef JVX_CONFIGURE_HOST_STATIC_NODE

// Default black listing in case of host that loads dynamic objects
void 
uMainWindow_specific::myComponentFilterCallback(jvxBool* doNotLoad, const char* module_name, jvxComponentType tp, jvxHandle* priv)
{
	*doNotLoad = false;
	switch (tp)
	{
	case JVX_COMPONENT_HOST:
	case JVX_COMPONENT_CONFIG_PROCESSOR:
	case JVX_COMPONENT_SYSTEM_TEXT_LOG:
	case JVX_COMPONENT_CONNECTION:

	case JVX_COMPONENT_DATACONVERTER:
	case JVX_COMPONENT_RESAMPLER:
	case JVX_COMPONENT_RT_AUDIO_FILE_READER:
	case JVX_COMPONENT_RT_AUDIO_FILE_WRITER:
		*doNotLoad = true;
	}
}
#endif

uMainWindow_specific::uMainWindow_specific()
{
	jvxSize i;
	parentRef = NULL;
	tpAll[JVX_COMPONENT_UNKNOWN].reset(JVX_COMPONENT_UNKNOWN);
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		tpAll[i].reset((jvxComponentType)i, 0, 0);
	}

	in_au_node = NULL;
	out_au_node = NULL;
	out_au_device = NULL;
	in_au_device = NULL;
	master_au_device = NULL;
	
	theProcess = NULL;

	the_conn_id = JVX_SIZE_UNSELECTED;
	the_bridge_dev2node = JVX_SIZE_UNSELECTED;
	the_bridge_node2dev = JVX_SIZE_UNSELECTED;
}

void
uMainWindow_specific::init_specific(uMainWindow* par)
{
	this->parentRef = par;
}
// ##################################################################################

/**
 * Hook to create project specific connections
 *///========================================================================
jvxErrorType
uMainWindow_specific::connect_specific()
{
	return(JVX_NO_ERROR);
}

// ##################################################################################
// BOOTUP SPECIFIC
// ##################################################################################

/*
 * Project specific boot-steps. We define
 * 1) Config lines
 * 2) Add the object types handled by the host
 * 3) Add the static libraries we want to use
 * 4) Add the objects to NOT load from DLL  (the black list)
 *///================================================================================
jvxErrorType
uMainWindow_specific::bootup_negotiate_specific()
{
	jvxSize i;
	jvxSize cnt;
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	IjvxObject* addMe = NULL;
	jvxComponentIdentification tpAudioDevL(JVX_COMPONENT_AUDIO_DEVICE, JVX_SIZE_SLOT_RETAIN, JVX_SIZE_SLOT_RETAIN);
	jvxComponentIdentification tpAudioNodeL(JVX_COMPONENT_AUDIO_NODE, JVX_SIZE_SLOT_RETAIN, JVX_SIZE_SLOT_RETAIN);
	jvxComponentIdentification tpVideoDevL(JVX_COMPONENT_VIDEO_DEVICE, JVX_SIZE_SLOT_RETAIN, JVX_SIZE_SLOT_RETAIN);
	jvxComponentIdentification tpVideoNodeL(JVX_COMPONENT_VIDEO_NODE, JVX_SIZE_SLOT_RETAIN, JVX_SIZE_SLOT_RETAIN);
	oneAddedStaticComponent comp;	
	jvxApiString descriptionComponent;

	// =======================================================================
	// Set firwst and afterwards request some additional features from host
	// =======================================================================
	parentRef->theHostFeatures.myMainWidget = parentRef->subWidgets.main.theWidget;

	JVX_START_SLOTS_BASE(parentRef->theHostFeatures.numSlotsComponents, 
		parentRef->_command_line_parameters_hosttype.num_slots_max, 
		parentRef->_command_line_parameters_hosttype.num_subslots_max);
	JVX_START_SLOTS_SUBPRODUCT(parentRef->theHostFeatures.numSlotsComponents, 
		parentRef->_command_line_parameters_hosttype.num_slots_max, 
		parentRef->_command_line_parameters_hosttype.num_subslots_max);

	//parentRef->theHostFeatures.nameDefaultAlgorithm = NULL;
	//parentRef->theHostFeatures.if_autoconnect = static_cast<IjvxAutoDataConnect*>(this);

	parentRef->theHostFeatures.if_report = static_cast<IjvxReport*>(parentRef);
	
	parentRef->theHostFeatures.fHost = parentRef->involvedComponents.theHost.hFHost;
	parentRef->theHostFeatures.hHost = parentRef->involvedHost.hHost;

	// =======================================================================
	// Get the host type features from the application hooks
	// =======================================================================
	jvx_configure_factoryhost_features(static_cast<configureFactoryHost_features*>(&parentRef->theHostFeatures));

	parentRef->boot_initialize_base(parentRef->theHostFeatures.numSlotsComponents);

	//=================================================================
	// Open all static libraries and add to host
	// Do not add before since the accepted types must have been specified
	//=================================================================

	// Open all tool components
	LOAD_ONE_MODULE_LIB_FULL(jvxTSystemTextLog_init, 
		jvxTSystemTextLog_terminate, "Text Log", 
		parentRef->involvedComponents.addedStaticObjects, 
		parentRef->involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTconfigProcessor_init, 
		jvxTconfigProcessor_terminate, "Config Parser", 
		parentRef->involvedComponents.addedStaticObjects, 
		parentRef->involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTThreads_init,
		jvxTThreads_terminate, "Threads",
		parentRef->involvedComponents.addedStaticObjects, 
		parentRef->involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTDataLogger_init,
		jvxTDataLogger_terminate, "Data Logger",
		parentRef->involvedComponents.addedStaticObjects,
		parentRef->involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTLogRemoteHandler_init,
		jvxTDataLogger_terminate, "Remote Log Handler",
		parentRef->involvedComponents.addedStaticObjects,
		parentRef->involvedComponents.theHost.hFHost);

	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_HOST);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_MIN_HOST);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_OFF_HOST);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_FACTORY_HOST);

	/*
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_CONFIG_PROCESSOR);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_SYSTEM_TEXT_LOG);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_THREADS);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_DATALOGGER);
	*/

	LOAD_ONE_MODULE_LIB_FULL(jvxTDataConverter_init, jvxTDataConverter_terminate, "Data Converter", parentRef->involvedComponents.addedStaticObjects, parentRef->involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTResampler_init, jvxTResampler_terminate, "Resampler", parentRef->involvedComponents.addedStaticObjects, parentRef->involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileReader_init, jvxTrtAudioFileReader_terminate, "RT Audio Reader", parentRef->involvedComponents.addedStaticObjects, parentRef->involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileWriter_init, jvxTrtAudioFileWriter_terminate, "RT Audio Writer", parentRef->involvedComponents.addedStaticObjects, parentRef->involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxAuTGenericWrapper_init, jvxAuTGenericWrapper_terminate, "Generic Wrapper Audio", parentRef->involvedComponents.addedStaticObjects, parentRef->involvedComponents.theHost.hFHost);

	/*
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_AUDIO_TECHNOLOGY);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_DATACONVERTER);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_RESAMPLER);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_RT_AUDIO_FILE_READER);
	parentRef->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_RT_AUDIO_FILE_WRITER);
	*/
	
	parentRef->static_load_loop();

	// =====================================================================================================

	parentRef->involvedComponents.theHost.hFHost->set_component_load_filter_function(parentRef->theHostFeatures.cb_loadfilter, parentRef->theHostFeatures.cb_loadfilter_priv);
	return(res);
}

/*
 * This function is called once the bootup has just been completed
 *///=================================================================================
jvxErrorType
uMainWindow_specific::postbootup_specific()
{
/*
	jvxState stat = JVX_STATE_NONE;
	jvxErrorType res = JVX_NO_ERROR;
	IjvxObject* theObject = NULL;
	jvxApiString  fldStr;
	jvxSize num = 0;
	jvxSize foundComp = JVX_SIZE_UNSELECTED;
	jvxSize i;
	jvxBool requiresNewStart = true;
	jvxSize slotId = 0;
	jvxSize slotSubId = 0;
	//jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];
	
	// Check technology 
	jvxComponentIdentification tpIdT;
	tpIdT.tp = JVX_COMPONENT_AUDIO_TECHNOLOGY;
	tpIdT.slotid = 0;
	tpIdT.slotsubid = 0;

	jvx_componentIdPropsCombo newRefElm;
	parentRef->involvedHost.hHost->state_component(tpIdT , &stat);
	if(stat >= JVX_STATE_SELECTED)
	{
		theObject = NULL;
		res = parentRef->involvedHost.hHost->request_object_component(tpIdT, &theObject);
		assert(res == JVX_NO_ERROR);
		assert(theObject != NULL);

		theObject->descriptor(&fldStr);
		if((std::string)fldStr.std_str() != "JVX_GENERIC_WRAPPER")
		{

			res = parentRef->involvedHost.hHost->deactivate_component(tpIdT);
			assert(res == JVX_NO_ERROR);

			jvx_componentIdentification_properties_erase(
				parentRef->involvedHost.hHost,
				parentRef->theRegisteredComponentReferenceTriples,
				tpIdT);

			res = parentRef->involvedHost.hHost->unselect_component(tpIdT);
			assert(res == JVX_NO_ERROR);
		}
		else
		{
			requiresNewStart = false;
		}
	}

	assert(!requiresNewStart);
*/
	return(JVX_NO_ERROR);
}

/**
 * This function to be called after bootup has been completed when the QT event handler
 * has taken over control.
 *///=======================================================================
void
uMainWindow_specific::bootDelayed_specific()
{
	jvxBool wantsToAdjust = false;
	parentRef->subWidgets.main.theWidget->inform_bootup_complete(&wantsToAdjust);
	if(wantsToAdjust)
	{
		this->adjustSize();
	}
	QRect rec = this->geometry();
	QSize sz;
	sz.setHeight(rec.height());
	sz.setWidth(rec.width());
	this->setMinimumSize(sz);
}

// ##################################################################################
// SHUTDOWN PROCESS
// ##################################################################################

/**
 * Hook to allow some project specific shutdown functionalities
 *///==========================================================================================
jvxErrorType
uMainWindow_specific::shutdown_specific()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	/*
	* This is done by the app lib
	if (parentRef->theHostFeatures.automation.if_autoconnect)
	{
		parentRef->involvedComponents.theHost.hFHost->remove_external_interface(parentRef->theHostFeatures.automation.if_autoconnect, JVX_INTERFACE_AUTO_DATA_CONNECT);
	}
	*/
	parentRef->involvedComponents.theHost.hFHost->set_component_load_filter_function(NULL, NULL);

	// Do not allow that host components are loaded via DLL
	parentRef->involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_HOST);
	parentRef->involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_MIN_HOST);
	parentRef->involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_OFF_HOST);
	parentRef->involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_FACTORY_HOST);

	parentRef->static_unload_loop();

	parentRef->shutdown_terminate_base();

	jvx_invalidate_factoryhost_features(&parentRef->theHostFeatures);

	// This interface is handled separately.
	// this->involvedComponents.theHost.hFHost->remove_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxQtWidgetHost*>(this)), JVX_INTERFACE_QT_WIDGET_HOST);
	return(JVX_NO_ERROR);
}

/**
 * This function to be called right before shutting down the host, opposite of bootDelayed_specific
 *///=============================================================================================
jvxErrorType
uMainWindow_specific::pre_shutdown_specific()
{
	jvxSize i;
	
	// Deactivate technology and device elements
	for (i = 0; i < parentRef->theHostFeatures.numSlotsComponents[JVX_COMPONENT_AUDIO_TECHNOLOGY]; i++)
	{
		parentRef->subWidgets.theAudioDialog->new_tech_slot(i);
		parentRef->subWidgets.theAudioDialog->select_technology(0);
	}

	return(JVX_NO_ERROR);
}

// =================================================================================================
// Host specific update window function
// =================================================================================================

void
uMainWindow_specific::updateWindow_specific(jvxCBitField prio)
{
    jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
#if 0
	parentRef->menuAlgorithms_Available->clear();
	jvxState stat = JVX_STATE_NONE;
	jvxSize num = 0;
	jvxSize selectionId = JVX_SIZE_UNSELECTED;
	jvxApiString strL;
	jvxComponentIdentification& tpIdN = tpAll[JVX_COMPONENT_AUDIO_NODE];


	res = parentRef->involvedHost.hHost->state_component(tpIdN , &stat);
	assert(res == JVX_NO_ERROR);

	res = parentRef->involvedHost.hHost->number_components_system(tpIdN , &num);
	assert(res == JVX_NO_ERROR);
	res = parentRef->involvedHost.hHost->selection_component(tpIdN , &selectionId);

	for(i = 0; i < num; i++)
	{
		std::string descr;
		std::string mod;
		jvxComponentAccessType acTp = JVX_COMPONENT_ACCESS_UNKNOWN;
		parentRef->involvedHost.hHost->description_component_system(tpIdN ,i, &strL);
		assert(res == JVX_NO_ERROR);
		
		descr = strL.std_str();

		parentRef->involvedHost.hHost->module_reference_component_system(tpIdN , i, &strL, &acTp);
		assert(res == JVX_NO_ERROR);
		
		mod = strL.std_str();

		QAction* act = new QAction(this);
		switch (acTp)
		{
		case JVX_COMPONENT_ACCESS_DYNAMIC_LIB:
			descr += " <d>";
			act->setToolTip(mod.c_str());
			break;
		case JVX_COMPONENT_ACCESS_STATIC_LIB:
			descr += " <s>";
			break;
		default:
			break;
		}
		act->setText(descr.c_str());


		act->setProperty("identification", QVariant((int)i));
#ifndef JVX_CONFIGURE_HOST_STATIC_NODE
		connect(act, SIGNAL(triggered()),this, SLOT(action_int_selection_slot()));
#endif
		act->setCheckable(true);
		if(i == selectionId)
		{
			act->setChecked(true);
		}
		parentRef->menuAlgorithms_Available->addAction(act);

/*
		res = this->involvedComponents.theHost.hFHost->select_component(JVX_COMPONENT_AUDIO_NODE, 0);
		assert(res == JVX_NO_ERROR);
		this->involvedComponents.theHost.hFHost->activate_component(JVX_COMPONENT_AUDIO_NODE);
		assert(res == JVX_NO_ERROR);
		*/
	}
#endif
	parentRef->subWidgets.main.theWidget->inform_update_window(prio);
}

// #####################################################################################
//  SEQUENCER PART
// #####################################################################################

/*
void
uMainWindow_specific::addDefaultSequence(bool	onlyIfNoSequence, jvxOneSequencerStepDefinition* defaultSteps_run, jvxSize numSteps_run, jvxOneSequencerStepDefinition* defaultSteps_leave, jvxSize numSteps_leave)
{
	IjvxSequencer* sequencer = NULL;
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxSize numSeq = 0;
	jvxSize numStepsRun = 0;
	jvxSize numStepsLeave = 0;
	jvxSize i,k;
	std::string nmNewSequence = "Default Sequence";
	jvxSize nmCnt = 0;

	jvxSize idSeq = 0;
	jvxSequencerQueueType seqType = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	jvxSequencerElementType tpElm = JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE;
	std::string descr = "description";
	jvxInt32 funcId = -1;
	jvxInt64 timeout = 10000;
	std::string label_step;
	std::string label_cond1;
	std::string label_cond2;
	jvxSize idSeqActivate = JVX_SIZE_UNSELECTED;
	jvxBool foundDefaultWithinDefinedSequences = false;

	// Check that we are in the mode to allow adding a sequence
	// TODO

	// If so, go ahead
	res = parentRef->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if((res == JVX_NO_ERROR) && sequencer)
	{
		// First check if such a sequence is already there..
		sequencer->number_sequences(&numSeq);

		// jump out if there is already one sequence which may e.g. be taken from config file
		if(!
			((numSeq > 0) && onlyIfNoSequence))
		{

			// Deactivate all sequences
			for(i = 0; i < numSeq; i++)
			{
				sequencer->mark_sequence_process(i, false);
			}

			// Check if there is a sequence to realize default sequencing
			for(i = 0; i < numSeq; i++)
			{
				sequencer->description_sequence(i, NULL, &numStepsRun, &numStepsLeave, NULL);
				if(
					(numStepsRun == numSteps_run) &&
					(numStepsLeave == numSteps_leave))
				{
					jvxBool runQueueMatches = true;
					jvxBool leaveQueueMatches = true;

					for(k = 0; k < numSteps_run; k++)
					{
						sequencer->description_step_sequence(i, k, JVX_SEQUENCER_QUEUE_TYPE_RUN, NULL, &tpElm, &tpIdN , NULL, NULL, NULL, NULL, NULL);
						if( !
							(defaultSteps_run[k].tpElm == tpElm)&&
							(jvxComponentIdentification(defaultSteps_run[k].tpComp, defaultSteps_run[k].slotid, defaultSteps_run[k].slotsubid) == tpIdN ))
						{
							runQueueMatches = false;
							break;
						}
					}
					for(k = 0; k < numSteps_leave; k++)
					{
						sequencer->description_step_sequence(i, k, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, NULL, &tpElm, &tpIdN , NULL, NULL, NULL, NULL, NULL);
						if( !
							(defaultSteps_leave[k].tpElm == tpElm)&&
							(jvxComponentIdentification(defaultSteps_leave[k].tpComp, defaultSteps_leave[k].slotid, defaultSteps_leave[k].slotsubid) == tpIdN ))
						{
							leaveQueueMatches = false;
							break;
						}
					}

					if(runQueueMatches && leaveQueueMatches)
					{
						idSeqActivate = i;
						break;
					}
				}
			}

			if(JVX_CHECK_SIZE_SELECTED( idSeqActivate ))
			{
				sequencer->mark_sequence_process(idSeqActivate, true);
			}
			else
			{
				// If no instance of the default sequence is present, add the appropriate default steps
				resL = JVX_ERROR_DUPLICATE_ENTRY;
				while(resL != JVX_NO_ERROR)
				{
					resL = sequencer->add_sequence(("Default Sequence" + jvx_size2String(nmCnt)).c_str());
					nmCnt ++;
					assert(nmCnt < 1000);
				}

				for(k= 0; k < numSteps_run; k++)
				{
					sequencer->insert_step_sequence_at(numSeq, JVX_SEQUENCER_QUEUE_TYPE_RUN, defaultSteps_run[k].tpElm, jvxComponentIdentification( defaultSteps_run[k].tpComp, defaultSteps_run[k].slotid, defaultSteps_run[k].slotsubid),
						defaultSteps_run[k].descr.c_str(), defaultSteps_run[k].funcId, defaultSteps_run[k].timeout, defaultSteps_run[k].label_step.c_str(),
						defaultSteps_run[k].label_cond1.c_str(), defaultSteps_run[k].label_cond2.c_str(), -1);
				}
				for(k= 0; k < numSteps_leave; k++)
				{
					sequencer->insert_step_sequence_at(numSeq, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, defaultSteps_leave[k].tpElm, jvxComponentIdentification(defaultSteps_leave[k].tpComp, defaultSteps_leave[k].slotid, defaultSteps_leave[k].slotsubid),
						defaultSteps_leave[k].descr.c_str(), defaultSteps_leave[k].funcId, defaultSteps_leave[k].timeout,  defaultSteps_leave[k].label_step.c_str(),
						defaultSteps_leave[k].label_cond1.c_str(), defaultSteps_leave[k].label_cond2.c_str(), -1);
				}
				sequencer->mark_sequence_process(numSeq, true);
			}
		}
		parentRef->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	}
}

void
uMainWindow_specific::removeAllSequencer()
{
	IjvxSequencer* sequencer = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	// If so, go ahead
	res = parentRef->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if((res == JVX_NO_ERROR) && sequencer)
	{
		// First check if such a sequence is already there..
		sequencer->reset_sequences();
		parentRef->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	}
}
*/

void
uMainWindow_specific::display_data_connections(IjvxDataConnections* connections)
{
	jvxSize num = 0, num_in, num_out;
	jvxSize i, j;
	jvxApiString txtD;
	IjvxConnectorFactory* theFac = NULL;
	IjvxInputConnector* theInCon = NULL;
	IjvxObject* theObj = NULL;
	jvxComponentIdentification tp;
	IjvxInputConnectorSelect* inCon = NULL;
	IjvxOutputConnectorSelect* outCon = NULL;

	std::cout << "Running auto connect" << std::endl;
	connections->number_connection_factories(&num);
	for (i = 0; i < num; i++)
	{
		connections->reference_connection_factory(i, &theFac, NULL);
		std::cout << "Connection factory #" << i << " <" << txtD.std_str() << ">:" << std::endl;
		if (theFac)
		{
			theFac->request_reference_object(&theObj);
			if (theObj)
			{
				theObj->request_specialization(NULL, &tp, NULL, NULL);
				std::cout << "# Origin: " << jvxComponentIdentification_txt(tp) << std::endl;
			}
			theFac->return_reference_object(theObj);

			num_in = 0;
			num_out = 0;
			theFac->number_input_connectors(&num_in);
			for (j = 0; j < num_in; j++)
			{
				std::cout << "--> Input connector #" << j << ":" << std::flush;
				theFac->reference_input_connector(j, &inCon);
				theFac->return_reference_input_connector(inCon);
			}
			theFac->number_output_connectors(&num_out);
			for (j = 0; j < num_out; j++)
			{
				std::cout << "--> Output connector #" << j << ":" << std::flush;
				theFac->reference_output_connector(j, &outCon);
				theFac->return_reference_output_connector(outCon);
				//std::cout << "<" << txtD.std_str() << ">" << std::endl;
			}
			connections->return_reference_connection_factory(theFac);
			theFac = NULL;
		}
	}
}

jvxErrorType
uMainWindow::pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp)
{
	jvx_componentIdPropsCombo refProps;
	// Forward state switch to automation function
	CjvxReportCommandRequest_ss elm(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH, tp, ss,
		jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
	elm.set_immediate(true);

	switch (ss)
	{
	case JVX_STATE_SWITCH_UNSELECT:

		jvx_componentIdentification_properties_find(
			theRegisteredComponentReferenceTriples,
			tp, refProps);

		if (subWidgets.main.theWidget)
		{
			subWidgets.main.theWidget->inform_unselect(tp, refProps.accProps);
		}

		if (subWidgets.theAudioDialog)
		{
			// Report to configureAudio dialog
			subWidgets.theAudioDialog->inform_component_unselect(tp, refProps.accProps);
		}

		this->request_command(elm);

		jvx_componentIdentification_properties_erase(
			involvedHost.hHost, theRegisteredComponentReferenceTriples,
			tp);
		break;
	case JVX_STATE_SWITCH_DEACTIVATE:

		jvx_componentIdentification_properties_find(
			theRegisteredComponentReferenceTriples,
			tp, refProps);
		
		// The order here is crucial: we may deassociate ui elements in mainCentral and must not have removed those before
		if (subWidgets.main.theWidget)
		{
			subWidgets.main.theWidget->inform_inactive(tp, refProps.accProps);
		}
		if (subWidgets.theAudioDialog)
		{
			// Report to configureAudio dialog
			subWidgets.theAudioDialog->inform_component_inactive(tp, refProps.accProps);
		}

		this->request_command(elm);
		break;

	case JVX_STATE_SWITCH_POSTPROCESS:

		this->request_command(elm);
		break;

	default:
		break;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc)
{
	jvx_componentIdPropsCombo refProps;
	CjvxReportCommandRequest_ss elm(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH, tp, ss,
		jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
	elm.set_immediate(true);

	if (suc == JVX_NO_ERROR)
	{
		switch (ss)
		{
		case JVX_STATE_SWITCH_SELECT:
			jvx_componentIdentification_properties_direct_add(
				involvedHost.hHost, theRegisteredComponentReferenceTriples,
				tp, &refProps);

			if (subWidgets.theAudioDialog)
			{
				// Report to configureAudio dialog
				subWidgets.theAudioDialog->inform_component_select(tp, refProps.accProps);
			}
			if (subWidgets.main.theWidget)
			{
				subWidgets.main.theWidget->inform_select(tp, refProps.accProps);
			}			
			
			this->request_command(elm);

			break;

		case JVX_STATE_SWITCH_ACTIVATE:

			jvx_componentIdentification_properties_find(
				theRegisteredComponentReferenceTriples,
				tp, refProps);
			if (subWidgets.theAudioDialog)
			{
				// Report to configureAudio dialog
				subWidgets.theAudioDialog->inform_component_active(tp, refProps.accProps);
			}
			if (subWidgets.main.theWidget)
			{
				subWidgets.main.theWidget->inform_active(tp, refProps.accProps);
			}

			this->request_command(elm);
			break;

		case JVX_STATE_SWITCH_PREPARE:

			this->request_command(elm);
			break;

		default:
			break;
		}
	}
	return(JVX_NO_ERROR);
}

JVX_PROPERTIES_CALLBACK_DEFINE(uMainWindow, cb_save_config)
{
	jvxErrorType res = JVX_NO_ERROR;
	emit emit_saveConfigFile();
	return res;
}

JVX_PROPERTIES_CALLBACK_DEFINE(uMainWindow, cb_load_config)
{
	jvxErrorType res = JVX_NO_ERROR;
	emit emit_openConfigFile();
	return res;
}

JVX_PROPERTIES_CALLBACK_DEFINE(uMainWindow, cb_shutdown)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::cout << "Callback for Shutdown currently not implemented" << std::endl;
	return res;
}

JVX_PROPERTIES_CALLBACK_DEFINE(uMainWindow, cb_restart)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::cout << "Callback for Restart currently not implemented" << std::endl;
	return res;
}

JVX_PROPERTIES_CALLBACK_DEFINE(uMainWindow, cb_xchg_property)
{
	jvxErrorType res = JVX_NO_ERROR;
	// std::cout << "Callback for XChange Property currently not implemented" << std::endl;
	return res;
}

