#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtGui/QKeyEvent>

#include "uMainWindow.h"
#include "jvxQtAudioHostHookupEntries.h"
#include "jvxHost_config.h"

#ifndef JVX_SPLASH_SUPPRESSED
#include <QtWidgets/QSplashScreen>
#endif
#include "jvx-helpers.h"
#include <iostream>

uMainWindow::uMainWindow() :
	viewer_props(static_allocateData, static_deallocateData, static_copyData),
	viewer_plots(static_allocateData, static_deallocateData, static_copyData)
{
	this->subWidgets.main.theWidget = NULL;
	subWidgets.theAudioDialog = NULL;
	subWidgets.theConnectionsFrame = NULL;
	subWidgets.theConnectionsWidget = NULL;
	subWidgets.theSequencerFrame = NULL;
	subWidgets.theSequencerWidget = NULL;
	
	systemParams.auto_start = false;
	systemParams.auto_stop = false;
	systemParams.skipStateSelected = true;

	subWidgets.bridgeMenuComponents = NULL;

	id_period_seq_ms = JVX_SIZE_UNSELECTED;
	id_rtview_period_maincentral_ms = JVX_SIZE_UNSELECTED;
	id_autostart_id = JVX_SIZE_UNSELECTED;
	id_autostop_id = JVX_SIZE_UNSELECTED;

	// Link the configure host features
	confHostFeatures = static_cast<configureHost_features*>(&theHostFeatures);

}

uMainWindow::~uMainWindow(void)
{
}

void
uMainWindow::postMessage_outThread(const char* txt, jvxReportPriority prio)
{
	QString txtq = txt;
	emit emit_postMessage(txtq, prio);
}

void
uMainWindow::postMessage_inThread(QString txt, jvxReportPriority prio)
{
	switch(prio)
	{
	case JVX_REPORT_PRIORITY_ERROR:
		std::cout << "ERR:" << std::flush;
		break;
	case JVX_REPORT_PRIORITY_WARNING:
		std::cout << "WARN:" << std::flush;
		break;
	case JVX_REPORT_PRIORITY_INFO:
		std::cout << "INFO:" << std::flush;
		break;
	case JVX_REPORT_PRIORITY_SUCCESS:
		std::cout << "SUCCESS:" << std::flush;
		break;
	}

	std::cout << txt.toLatin1().constData() << std::endl;
}

jvxErrorType
uMainWindow::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	postMessage_outThread(txt, prio);
	return JVX_NO_ERROR;
}

// ===========================================================================

jvxErrorType
uMainWindow::request_command(const CjvxReportCommandRequest& request)
{
	jvxErrorType res = JVX_NO_ERROR;
	CjvxReportCommandRequest* ptr = NULL;

	if (request.request() == jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN)
	{
		const CjvxReportCommandRequest_uid* ptrReq = castCommandRequest<CjvxReportCommandRequest_uid>(request);
		if (ptrReq)
		{
			IjvxDataConnections* theDataConnections = NULL;
			if (involvedComponents.theHost.hFHost)
			{
				theDataConnections = reqInterfaceObj<IjvxDataConnections>(involvedComponents.theHost.hFHost);
				if (theDataConnections)
				{
					jvxSize num = 0;
					jvxSize uId = JVX_SIZE_UNSELECTED;
					ptrReq->uid(&uId);

					res = theDataConnections->add_process_test(uId, &num, request.immediate());
					if (res == JVX_ERROR_POSTPONE)
					{
						// In case we receive a POSTPONE, this indicates that we need to change thread
						emit emit_triggerTestChain();
					}
					else
					{
						if (num)
						{
							jvxCBitField requestStart = 0;
							jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
							jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
							jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);

							this->report_command_request(requestStart, NULL, 0);
						}
					}
					retInterfaceObj<IjvxDataConnections>(involvedComponents.theHost.hFHost, theDataConnections);
				}
			}
		}
		return res;
	}

	if (request.immediate())
	{
		switch (request.request())
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE:
			if (subWidgets.theSequencerWidget)
			{
				res = subWidgets.theSequencerWidget->immediate_sequencer_step();
			}
			break;		
		}

		if (this->theHostFeatures.automation.if_report_automate)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			jvxReportCommandBroadcastType broad = request.broadcast();
			switch (broad)
			{
			case jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION:
				resL = this->theHostFeatures.automation.if_report_automate->request_command(request);
				break;
			}
		}
		return res;
	}	
	else
	{
		ptr = jvx_command_request_copy_alloc(request);
		emit emit_request_command(ptr);
	}
	
	return res;
}

jvxErrorType 
uMainWindow::interface_sub_report(IjvxSubReport** subReport)
{
	return JVX_ERROR_UNSUPPORTED;
}

void
uMainWindow::request_command_inThread(CjvxReportCommandRequest* request)
{
	jvxReportCommandDataType tp = request->datatype();
	const CjvxReportCommandRequest_rm* iface_rm = NULL;
	const CjvxReportCommandRequest_id* iface_id = NULL;
	jvxReportCommandRequest req = request->request();
	jvxComponentIdentification cp;
	jvxCBitField prio = 0;
	IjvxObject* theObj = NULL;
	IjvxInterfaceFactory* theFac = NULL;
	jvxComponentIdentification cpId = request->origin();
	jvxSize schedId = JVX_SIZE_UNSELECTED;
	jvxErrorType resL = JVX_NO_ERROR;

	// Pass this to the main widget
	if (request && subWidgets.main.theWidget)
	{
		resL = subWidgets.main.theWidget->report_command_request(*request);
	}

	switch (tp)
	{
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_BASE:
		switch(req)
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST:
			cp = request->origin();
			if (subWidgets.bridgeMenuComponents)
			{
				subWidgets.bridgeMenuComponents->updateWindowSingle(cpId);
			}
			break;
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES:
			cp = request->origin();
			subWidgets.main.theWidget->inform_update_properties(cp);
			break;
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED:

			jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
			this->updateWindow(prio);
			break;
		}
		break;
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE:
		request->specialization(reinterpret_cast<const jvxHandle**>(&iface_rm), tp);
		if (iface_rm)
		{
			jvxHandle* userData = request->user_data();
			schedId = iface_rm->schedule_id();
			if (involvedHost.hHost)
			{
				involvedHost.hHost->request_object_selected_component(cpId, &theObj);
				if (theObj)
				{
					theObj->interface_factory(&theFac);
					if (theFac)
					{
						IjvxSchedule* theSchedule = NULL;
						theFac->request_hidden_interface(JVX_INTERFACE_SCHEDULE, reinterpret_cast<jvxHandle**>(&theSchedule));
						if (theSchedule)
						{
							theSchedule->schedule_main_loop(schedId, userData);
							theFac->return_hidden_interface(JVX_INTERFACE_SCHEDULE, reinterpret_cast<jvxHandle*>(theSchedule));
						}
						else
						{
							jvxApiString astr;
							theObj->description(&astr);
							std::cout << __FUNCTION__ << " : Request to re-schedule in main thread failed for object <" << astr.std_str() << ">, reason: JVX_INTERFACE_SCHEDULE not supported." << std::endl;
						}
					}
					involvedHost.hHost->return_object_selected_component(cpId, theObj);
				}
			}
		}
		break;
	
	default:
		break;
	}

	if (this->theHostFeatures.automation.if_report_automate)
	{
		jvxErrorType resL = JVX_NO_ERROR;
		jvxReportCommandBroadcastType broad = request->broadcast();
		switch (broad)
		{
		case jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION:
			resL = this->theHostFeatures.automation.if_report_automate->request_command(*request);
			break;
		}
	}

	//delete request;
	jvx_command_request_copy_dealloc(request);
}

// ============================================================================
jvxErrorType
uMainWindow::report_command_request(jvxCBitField request, 
	jvxHandle* caseSpecificData, jvxSize szSpecificData)
{
	jvxSize num = 0;
	if (
		(jvx_cbitTest(request, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT)))
	{
		jvxSize uId = JVX_SIZE_UNSELECTED;
		if (caseSpecificData)
		{
			uId = (intptr_t)caseSpecificData;
		}
		this->request_command(CjvxReportCommandRequest_uid(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN, JVX_COMPONENT_UNKNOWN, uId));
	}
	else
	{
		emit emit_report_command_request((short)request,
			caseSpecificData, szSpecificData);
	}
	return JVX_NO_ERROR;
}

void
uMainWindow::trigger_test_chain_inThread()
{
	if (involvedComponents.theHost.hFHost)
	{
		jvxCBitField requestStart = 0;
		jvxSize numTested = 0;
		IjvxDataConnections* theDataConnections = reqInterfaceObj<IjvxDataConnections>(involvedComponents.theHost.hFHost);
		if (theDataConnections)
		{
			theDataConnections->trigger_process_test_all(&numTested);
			retInterfaceObj<IjvxDataConnections>(involvedComponents.theHost.hFHost, theDataConnections);
		}
		if (numTested)
		{
			jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
			jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
			jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);

			this->report_command_request(requestStart, NULL, 0);
		}
	}
}

void
uMainWindow::report_command_request_inThread(jvxCBitField request, 
	jvxHandle* caseSpecificData, jvxSize szSpecificData)
{
	jvxCBitField theRequest = (jvxCBitField)request;
	jvxCBitField requestStart;
	jvx_bitFClear(requestStart);

	if (jvx_cbitTest(theRequest, JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT))
	{
		this->on_request_startstop(JVX_START_STOP_INTENDED_STOP);
	}
	if (jvx_cbitTest(theRequest, JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER_SHIFT))
	{
		this->on_request_startstop(JVX_START_STOP_INTENDED_START);
	}
	/*
	if (jvx_cbitTest(theRequest, JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT))
	{
		// this->restartProperties();
	}
	*/
	if (
		(jvx_cbitTest(theRequest, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT))|| 
		(jvx_cbitTest(theRequest, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT))||
		(jvx_cbitTest(theRequest, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT)))
	{
		this->updateWindow(theRequest);
	}

	if (jvx_cbitTest(theRequest, JVX_REPORT_REQUEST_RECONFIGURE_SHIFT))
	{
		this->updateWindow(JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	}

	if (
		(jvx_cbitTest(theRequest, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT)))
	{
		// This case should have been handled outside
		assert(0);
		/*
		IjvxDataConnections* theDataConnections = NULL;
		IjvxDataConnectionProcess* theRef = NULL;
		IjvxConnectionMaster* theMas = NULL;
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
		jvxBool testOk = false;
		jvxSize i;
		jvxSize num;

		if(involvedComponents.theHost.hFHost)
		{
			jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));
			if ((res == JVX_NO_ERROR) && theDataConnections)
			{
				if (caseSpecificData == NULL)
				{
					theDataConnections->number_connections_process(&num);
					for (i = 0; i < num; i++)
					{
						theDataConnections->reference_connection_process(i, &theRef);
						if (theRef)
						{
							jvxApiString nmChain;
							// Run test on chain, the result is stored internally and will be shown in next "updateWindow"
							jvxErrorType resL = theRef->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A_NULL);// JVX_CONNECTION_FEEDBACK_REF_CALL(fdb));

							theRef->status_chain(&testOk JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
							if (!testOk)
							{
								theRef->descriptor_connection(&nmChain);
								this->report_simple_text_message(("Problem testing chain " + nmChain.std_str() + ", please review connection manager.").c_str(), JVX_REPORT_PRIORITY_ERROR);
							}
							theDataConnections->return_reference_connection_process(theRef);
							jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
							jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
							jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);

							this->report_command_request(requestStart, NULL, 0);
						}
					}
				}
				else
				{
					jvxSize uId = (intptr_t)caseSpecificData;
					jvxApiString nmChain;
					JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

					theDataConnections->reference_connection_process_uid(uId, &theRef);
					if (theRef)
					{
						// Run test on chain, the result is stored internally and will be shown in next "updateWindow"
						jvxErrorType resL = theRef->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A_NULL);// JVX_CONNECTION_FEEDBACK_REF_CALL(fdb));

						theRef->status_chain(&testOk JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
						if (!testOk)
						{
							theRef->descriptor_connection(&nmChain);
							this->report_simple_text_message(("Problem testing chain " + nmChain.std_str() + ", please review connection manager.").c_str(), JVX_REPORT_PRIORITY_ERROR);
						}
						theDataConnections->return_reference_connection_process(theRef);
						jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
						jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
						jvx_bitSet(requestStart, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);

						this->report_command_request(requestStart, NULL, 0);
					}
					else
					{
						std::cout << "Failed to find connection process with ID = " << uId << std::endl;
					}
				}
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theDataConnections));
				theDataConnections = NULL;
			}
		}
		this->updateWindow(theRequest);
		*/
	}
	if (
		(jvx_cbitTest(theRequest, JVX_REPORT_REQUEST_REACTIVATE_SHIFT)))
	{
		jvxComponentIdentification* cpTp = (jvxComponentIdentification*)caseSpecificData;
		if (cpTp)
		{
			if(subWidgets.theAudioDialog)
			{
				subWidgets.theAudioDialog->reactivateComponent(*cpTp);
			}
		}
	}
}


jvxErrorType
uMainWindow::report_internals_have_changed(const jvxComponentIdentification& tpFrom, IjvxObject* theObj, jvxPropertyCategoryType cat, 
	jvxSize propId, bool onlyContent, jvxSize offs, jvxSize num, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurp)
{
	// Check config lines..
	jvxErrorType res = JVX_NO_ERROR;

	// Follow deployment of properties as specified in the configuration lines
	IjvxConfigurationLine* theConfigLine = NULL;

	// Config parameters to config line
	res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLine));

	if((res == JVX_NO_ERROR) && theConfigLine)
	{
		theConfigLine->report_property_was_set(tpFrom, cat, propId, onlyContent, tpTo);

		this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle*>(theConfigLine));
	}

	// Report to update all UIs since important stuff has changed
	report_internals_have_changed_outThread(tpFrom, theObj,  cat,  propId, onlyContent, tpTo, callpurp);

	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::report_take_over_property(const jvxComponentIdentification& tpFrom, IjvxObject* objRef,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent
		, jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurp)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvx_propertyReferenceTriple theTriple;
	jvxCallManagerProperties callGate;
	jvx_initPropertyReferenceTriple(&theTriple);
	jvx_getReferencePropertiesObject(this->involvedHost.hHost, &theTriple, tpTo);

	if(jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		res = theTriple.theProps->request_takeover_property(callGate, tpFrom, objRef,
			fld, numElements, format, offsetStart, onlyContent,
			cat, propId);
		jvx_returnReferencePropertiesObject(this->involvedHost.hHost, &theTriple, tpTo);
	}
	return res;
}


void
uMainWindow::report_internals_have_changed_outThread(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, 
	jvxSize propId, bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurp)
{
	// Switch thread to come into main thread
	emit emit_report_internals_have_changed_inThread(tp, theObj, cat, propId, onlyContent, tpTo, callpurp);
}

void
uMainWindow::report_internals_have_changed_inThread(jvxComponentIdentification tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId, 
	bool onlyContent, jvxComponentIdentification tpTo, jvxPropertyCallPurpose callpurp)
{

	if(subWidgets.theAudioDialog)
	{
		// Report to configureAudio dialog
		subWidgets.theAudioDialog->inform_internals_have_changed(tp, theObj, cat, propId, onlyContent, tpTo, callpurp);
	}

	if(subWidgets.main.theWidget)
	{
		// Report to main widget
		this->subWidgets.main.theWidget->inform_internals_have_changed(tp, theObj, cat, propId, onlyContent, tpTo, callpurp);
	}
}

jvxErrorType
uMainWindow::report_os_specific(jvxSize commandId, void* context)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
uMainWindow::report_component_select_on_config(const jvxComponentIdentification& tp)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::report_component_unselect_on_config(const jvxComponentIdentification& tp)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::report_component_active_on_config(const jvxComponentIdentification& tp)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::report_component_inactive_on_config(const jvxComponentIdentification& tp)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::forward_trigger_operation(
		jvxMainWindowController_trigger_operation_ids id_operation,
		jvxHandle* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification* tp = NULL;
	jvxSequencerStatus seqStat = JVX_SEQUENCER_STATUS_NONE;
	jvxCBitField bfld = 0;

	if (subWidgets.theSequencerWidget)
	{
		subWidgets.theSequencerWidget->status_process(&seqStat, NULL);
	}

	switch (id_operation)
	{
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_START:
		if (seqStat == JVX_SEQUENCER_STATUS_NONE)
		{
			this->on_request_startstop(JVX_START_STOP_INTENDED_START);
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_STOP:
		if (seqStat != JVX_SEQUENCER_STATUS_NONE)
		{
			this->on_request_startstop(JVX_START_STOP_INTENDED_STOP);
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SWITCH_SLOT:

		tp = (jvxComponentIdentification*)theData;
		if (tp)
		{
			tpAll[tp->tp] = *tp;
		}
		this->updateWindow((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT); // WHich priority to choose
		break;

	case JVX_MAINWINDOWCONTROLLER_TRIGGER_GET_SLOT:

		tp = (jvxComponentIdentification*)theData;
		if (tp)
		{
			*tp = tpAll[tp->tp];
		}
		this->updateWindow((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT); // WHich priority to choose
		break;

	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SET_WINDOW_TITLE:
		this->setWindowTitle(reinterpret_cast<const char*>(theData));
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_ADJUST_SIZE:
		this->adjustSize();
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SAFE_CONFIG:
		this->saveConfigFile();
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_OPEN_CONFIG_FULL:
		//this->openConfigFile_core((const char*)theData, false);
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_OPEN_CONFIG_OVLAY:
		//this->openConfigFile_core((const char*)theData, true);
		res = JVX_NO_ERROR;
		break;

	case JVX_MAINWINDOWCONTROLLER_TRIGGER_UPDATE_VIEWER:
		this->updateWindow();
		break;

	case JVX_MAINWINDOWCONTROLLER_TRIGGER_IMMEDIATE_TEST_ALL_CHAINS:
		jvx_bitSet(bfld, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT);
		this->report_command_request_inThread(bfld, NULL, 0);
		break;

	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return(res);
}

void 
uMainWindow::report_widget_closed(QWidget* theClosedWidget)
{
	updateWindow(0);
}

void 
uMainWindow::report_widget_specific(jvxSize id, jvxHandle* spec)
{
	switch (id)
	{
	case JVX_QTWIDGET_SPECIFIC_REPORT_UDPATE_WINDOW:
		updateWindow(0);
		break;
	}
}

/*
 * =================================================================================
 * BOOTUP AND SHUTDOWN callbacks
 * =================================================================================
 */

jvxErrorType 
uMainWindow::boot_initialize_specific(jvxApiString* errloc)
{
	QVBoxLayout* myVLayout = NULL;
	QGridLayout* myGLayout = NULL;
	QSize sz, sz2;
	QLayout* mlay = NULL;

	QWidget* theNewWidget = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	theEventLoop.initialize(involvedComponents.theHost.hFHost, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, 1000);
	theEventLoop.start();
	involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoop*>(&theEventLoop)), JVX_INTERFACE_EVENTLOOP);

	// ====================================================================
	// Start main central widget
	// We need a valid host reference with a valid report reference when setting up the main widget
	subWidgets.main.theWidget = NULL;
	mainWindow_UiExtension_host_init(&subWidgets.main.theWidget, static_cast<QWidget*>(this));
	assert(subWidgets.main.theWidget);
	subWidgets.main.theWidget->init_submodule(involvedHost.hHost);
	subWidgets.main.theWidget->return_widget(&theNewWidget);
	subWidgets.main.theWidget->get_reference_feature(&theHostFeatures.prv_features);
	assert(theNewWidget);

	// ==========================================================================================
	IjvxProperties* theProps = NULL;
	jvxCallManagerProperties callGate;
	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
	if (theProps)
	{
		theCallback_saveconfig.cb = cb_save_config;
		theCallback_saveconfig.cb_priv = reinterpret_cast<jvxHandle*>(this);
		jvx_set_property(theProps, &theCallback_saveconfig, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_save_config", callGate);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);

		theCallback_exchg_property.cb = cb_xchg_property;
		theCallback_exchg_property.cb_priv = reinterpret_cast<jvxHandle*>(this);
		jvx_set_property(theProps, &theCallback_exchg_property, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_xchg_property", callGate);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);

		theCallback_shutdown.cb = cb_shutdown;
		theCallback_shutdown.cb_priv = reinterpret_cast<jvxHandle*>(this);
		jvx_set_property(theProps, &theCallback_shutdown, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_shutdown", callGate);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);

		theCallback_restart.cb = cb_restart;
		theCallback_restart.cb_priv = reinterpret_cast<jvxHandle*>(this);
		jvx_set_property(theProps, &theCallback_restart, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_restart", callGate);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);
	}
	theProps = NULL;

	res = bootup_negotiate_specific();
	if (res != JVX_NO_ERROR)
		return res;

	/* Incorporate what was found before */
	if (theHostFeatures.hideHostStartButton)
	{
		pushButton->hide();
	}

	if (theHostFeatures.disableSaveFunction)
	{
		actionSave->setEnabled(false);
	}

	int cnt;
	
	switch (theHostFeatures.config_ui.tweakUi)
	{
	case JVX_QT_MAINWIDGET_SCROLL:
		frame_add_me_scroll->setWidget(theNewWidget);
		frame_add_me_noscroll->hide();
		stackedWidget->setCurrentIndex(1);
		break;
	case JVX_QT_MAINWIDGET_EXPAND_FULLSIZE:
		
		// On the second run, there might be a layout already, clear it!
		mlay = frame_add_me_noscroll->layout();
		if (mlay)
			delete mlay;

		myVLayout = new QVBoxLayout();
		myVLayout->addWidget(theNewWidget);
		frame_add_me_noscroll->setLayout(myVLayout);
		frame_add_me_scroll->hide();
		myVLayout->setContentsMargins(0, 0, 0, 0);
		stackedWidget->setCurrentIndex(0);
		break;
	case JVX_QT_MAINWIDGET_NO_EXPAND_CENTER:
		
		// On the second run, there might be a layout already, clear it!
		mlay = frame_add_me_noscroll->layout();
		if (mlay)
			delete mlay;

		myGLayout = new QGridLayout();
		myGLayout->addWidget(theNewWidget, 0, 0, (Qt::AlignHCenter | Qt::AlignVCenter));
		frame_add_me_noscroll->setLayout(myGLayout);
		frame_add_me_scroll->hide();
		myGLayout->setContentsMargins(0, 0, 0, 0);
		stackedWidget->setCurrentIndex(0);
		//cnt = stackedWidget->count();
		//theNewWidget->show();
		break;
	}

	sz.setWidth(0);
	sz.setHeight(0);

	if (theHostFeatures.config_ui.minWidthWindow >= 0)
	{
		sz.setWidth(theHostFeatures.config_ui.minWidthWindow);
	}
	if (theHostFeatures.config_ui.minHeightWindow >= 0)
	{
		sz.setHeight(theHostFeatures.config_ui.minHeightWindow);
	}
	this->setMinimumSize(sz);

	sz.setWidth(QWIDGETSIZE_MAX);
	sz.setHeight(QWIDGETSIZE_MAX);
	if (theHostFeatures.config_ui.maxWidthWindow >= 0)
	{
		sz.setWidth(theHostFeatures.config_ui.maxWidthWindow);
	}
	if (theHostFeatures.config_ui.maxHeightWindow >= 0)
	{
		sz.setHeight(theHostFeatures.config_ui.maxHeightWindow);
	}
	this->setMaximumSize(sz);

	/*
	QSize szz = this->minimumSize();
	std::cout << "A) Min: " << szz.width() << ":" << szz.height() << std::endl;
	*/

	/* Setup the extending widgets */
	setup_widgets();

	/*
	szz = this->minimumSize();
	std::cout << "B) Min: " << szz.width() << ":" << szz.height() << std::endl;
	*/
	return res;
}

jvxErrorType
uMainWindow::boot_specify_slots_specific()
{
	subWidgets.theAudioDialog->acceptSlotConfiguration();
	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::boot_prepare_specific(jvxApiString* errloc) 
{
	// Add a hook to be done after bootup (even after config file read)
	postbootup_specific();

	postbootup_widgets();

	//subWidgets.theAudioDialog->startProperties();

	// ============================================================================
	// Initialize data connections in corresponding widget
	// ============================================================================
	/*
	if (involvedComponents.theDataConnections)
	{
		subWidgets.theConnectionsWidget->setDataConnectionsReference(involvedComponents.theDataConnections);
		updateWindow((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_DATACONNECTIONS_SHIFT);
	}
	*/
	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::boot_start_specific(jvxApiString* errloc) 
{
	subWidgets.theAudioDialog->start();

#ifdef JVX_OS_WINDOWS
	// windows issue
// no idea why, but if I use only showMaximized the window does not show up maximized - QT bug?
	showMinimized();
#endif
	showMaximized();

	this->bootDelayed_specific();

	if (systemParams.auto_start)
	{
		if (allowStart)
		{
			emit emit_autoStart();
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::boot_activate_specific(jvxApiString* errloc) 
{
	finalize_widgets();
	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::shutdown_terminate_specific(jvxApiString* errloc) 
{
	shutdown_widgets();

	// Shutdown specific
	shutdown_specific();

	// ====================================================================
	// Stop main central widget
	//theHostFeatures.automation.if_autoconnect = NULL;
	//theHostFeatures.automation.if_report_automate = NULL;
	theHostFeatures.if_report = NULL;
	theHostFeatures.prv_features = NULL;

	subWidgets.main.theWidget->terminate_submodule();
	mainWindow_UiExtension_host_terminate(subWidgets.main.theWidget);
	subWidgets.main.theWidget = NULL;

	involvedComponents.theHost.hFHost->remove_external_interface(
		reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoop*>(&theEventLoop)), JVX_INTERFACE_EVENTLOOP);

	theEventLoop.stop();
	theEventLoop.terminate();

	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::on_components_before_configure()
{
	// Run the base class load to activate system components
	jvxErrorType res =  JVX_APPHOST_PRODUCT_CLASSNAME::on_components_before_configure();	
	return res;
}
// ====================================================================
// ====================================================================

jvxErrorType 
uMainWindow::shutdown_deactivate_specific(jvxApiString* errloc) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::shutdown_postprocess_specific(jvxApiString* errloc)
{
	pre_shutdown_specific();

	// deactivate_default_components_host(theHostFeatures.lst_ModulesOnStart, involvedHost.hHost); <- all components will be dectivated on shutdown anyway :-)

	// Uncharging the sequencer must happen before removing the sequencer reference - latest option is here
	// subWidgets.theSequencerWidget->remove_all_sequence();

	preshutdown_widgets();

	subWidgets.main.theWidget->inform_about_to_shutdown();

	//subWidgets.theAudioDialog->stopProperties();

	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::shutdown_stop_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}

void 
uMainWindow::report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context)
{
	this->postMessage_inThread(txt, prio);
}
void 
uMainWindow::inform_config_read_start(const char* nmfile, jvxHandle* context)
{
#ifndef JVX_SPLASH_SUPPRESSED
	QSplashScreen* splash = (QSplashScreen*)context;
	splash->showMessage(("Reading configuration <" + (std::string)nmfile + ">...").c_str(), Qt::AlignHCenter, JVX_QT_SPLASH_TEXT_COLOR);
	qApp->processEvents();
#endif
}

void 
uMainWindow::inform_config_read_stop(jvxErrorType resRead, const char* nmFile, jvxHandle* context)
{
	if (resRead != JVX_NO_ERROR)
		std::cout << "Reading config file " << nmFile << " failed, reason: " << jvxErrorType_descr(resRead) << std::endl;
}

void
uMainWindow::report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName)
{	
	subWidgets.main.theWidget->inform_config_read_complete(fName);
}

void
uMainWindow::updateAllWidgetsOnStateChange()
{
	//this->subWidgets.realtimeViewer.props.theWidget->updateAllWidgetsOnStateChange();
	//this->subWidgets.realtimeViewer.plots.theWidget->updateAllWidgetsOnStateChange();
	this->updateWindow();
}

void
uMainWindow::postMessageError(const char* mess)
{
	this->postMessage_inThread(mess, JVX_REPORT_PRIORITY_ERROR);
}

void 
uMainWindow::keyPressEvent(QKeyEvent* event)
{
	if (theHostFeatures.acceptSpaceStart)
	{
		int key = event->key();

		switch (key)
		{

		case Qt::Key_Space:
			//this->control_toggleSequencer();
			on_request_startstop(JVX_START_STOP_INTENDED_TOGGLE);
			break;
		}
	}
}
