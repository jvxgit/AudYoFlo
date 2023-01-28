#include <QtGui/QKeyEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include "uMainWindow.h"
#include "uMainWindow_widgets.h"
#include "jvxQtHostHookEntries.h"
#include "jvxHost_config.h"

uMainWindow::uMainWindow()
{
	jvxSize i;

	this->subWidgets.main.theWidget = NULL;
	subWidgets.main.timer = NULL;
	this->subWidgets.messages.theWidget = NULL;
	this->subWidgets.messages.theWidgetD = NULL;
	
	subWidgets.bridgeMenuComponents = NULL;

	systemParams.encryptionConfigFiles = false;
	systemParams.skipStateSelected = true;
	systemParams.auto_start = false;

	id_autoc_id = JVX_SIZE_UNSELECTED;
	id_rtview_period_props_ms = JVX_SIZE_UNSELECTED;
	id_rtview_period_plots_ms = JVX_SIZE_UNSELECTED;
	id_period_seq_ms = JVX_SIZE_UNSELECTED;
	id_rtview_period_maincentral_ms = JVX_SIZE_UNSELECTED;
	id_skip_ssel_id = JVX_SIZE_UNSELECTED;
	id_enc_config_id = JVX_SIZE_UNSELECTED;

	tpAll[JVX_COMPONENT_UNKNOWN].reset(JVX_COMPONENT_UNKNOWN);
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		tpAll[i].reset((jvxComponentType)i, 0, 0);
		// tpAllNext[i].reset((jvxComponentType)i, 0, 0);
	}

	confHostFeatures = static_cast<configureHost_features*>(&theHostFeatures);
}

uMainWindow::~uMainWindow(void)
{
}

void
uMainWindow::postMessage_outThread(const char* txt, QColor col)
{
	QString txtq = txt;
	emit emit_postMessage(txtq, col);
}

void
uMainWindow::postMessage_inThread(QString txt, QColor col)
{
	if (this->subWidgets.messages.theWidget)
	{
		this->subWidgets.messages.theWidget->postMessage_inThread(txt, col);
	}
	else
	{
		std::cout << "Message posted after termination of message widget: " << txt.toStdString() << std::endl;
	}
}

jvxErrorType
uMainWindow::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	switch(prio)
	{
	case JVX_REPORT_PRIORITY_ERROR:
		postMessage_outThread(txt, JVX_QT_COLOR_ERROR);
		break;
	case JVX_REPORT_PRIORITY_WARNING:
		postMessage_outThread(txt, JVX_QT_COLOR_WARNING);
		break;
	case JVX_REPORT_PRIORITY_INFO:
		postMessage_outThread(txt, JVX_QT_COLOR_INFORM);
		break;
	case JVX_REPORT_PRIORITY_SUCCESS:
		postMessage_outThread(txt, JVX_QT_COLOR_SUCCESS);
		break;
	default:
		postMessage_outThread(txt, Qt::black);
		break;
	}
	return(JVX_NO_ERROR);
}

// =========================================================================

jvxErrorType 
uMainWindow::interface_sub_report(IjvxSubReport** subReport)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
uMainWindow::request_command(const CjvxReportCommandRequest& request)
{
	jvxErrorType res = JVX_NO_ERROR;
	CjvxReportCommandRequest* ptr = NULL;

	if (request.immediate())
	{
		switch (request.request())
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE:
			if (subWidgets.sequencer.theWidget)
			{
				res = subWidgets.sequencer.theWidget->immediate_sequencer_step();
			}
			break;
		}
	}
	else
	{
		switch (request.datatype())
		{
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE:
			ptr = new CjvxReportCommandRequest_rm(request);
			break;
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT:
			ptr = new CjvxReportCommandRequest_id(request);
			break;
		case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID:
			ptr = new CjvxReportCommandRequest_uid(request);
			break;
		default:
			ptr = new CjvxReportCommandRequest(request);
			break;
		}
		emit emit_request_command(ptr);
	}
	return res;
}

void
uMainWindow::request_command_inThread(CjvxReportCommandRequest* request)
{
	jvxReportCommandDataType tp = request->datatype();
	const CjvxReportCommandRequest_rm* iface_rm = NULL;
	jvxReportCommandRequest req = request->request();
	jvxComponentIdentification cp;
	jvxCBitField prio = 0;
	IjvxObject* theObj = NULL;
	IjvxInterfaceFactory* theFac = NULL;
	jvxComponentIdentification cpId = request->origin();
	jvxSize schedId = JVX_SIZE_UNSELECTED;
	jvxErrorType resL = JVX_NO_ERROR;
	switch (tp)
	{
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_BASE:
		switch (req)
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST:
			cp = request->origin();
			if (subWidgets.bridgeMenuComponents)
			{
				subWidgets.bridgeMenuComponents->updateWindowSingle(cpId);
			}
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

	delete request;
}
// =========================================================================

jvxErrorType
uMainWindow::report_command_request(
	jvxCBitField request, 
	jvxHandle* caseSpecificData, 
	jvxSize szSpecificData)
{
	emit emit_report_command_request(request, caseSpecificData, szSpecificData);
	return(JVX_NO_ERROR);
}

void
uMainWindow::report_command_request_inThread(jvxCBitField request, 
	jvxHandle* caseSpecificData, 
	jvxSize szSpecificData)
{
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT))
	{
		control_stopSequencer();
	}
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SUBROUTINE_SHIFT))
	{
		control_contSequencer();
	}
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER_SHIFT))
	{
		control_startSequencer();
	}
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT))
	{
		this->updateWindow(request);
	}
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT))
	{
		this->updateWindow(request);
		this->subWidgets.realtimeViewer.props.theWidget->updateWindow_update(false);
	}
	if (jvx_cbitTest(request, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT))
	{
		this->updateWindow(request);
		this->subWidgets.realtimeViewer.props.theWidget->updateWindow_update(true);
	}

	if (
		(jvx_cbitTest(request, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT)))
	{
		IjvxDataConnections* theDataConnections = NULL;
		IjvxDataConnectionProcess* theRef = NULL;
		IjvxConnectionMaster* theMas = NULL;
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
		jvxBool testOk = false;

		if (involvedComponents.theHost.hFHost)
		{
			jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));
			if ((res == JVX_NO_ERROR) && theDataConnections)
			{
				jvxSize uId = (intptr_t)caseSpecificData;
				jvxApiString nmChain;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

				theDataConnections->reference_connection_process_uid(uId, &theRef);
				if (theRef)
				{
					// Run test on chain, the result is stored internally and will be shown in next "updateWindow"
					jvxErrorType resL = theRef->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

					theRef->status_chain(&testOk JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
					if (!testOk)
					{
						theRef->descriptor_connection(&nmChain);
						this->report_simple_text_message(("Problem testing chain " + nmChain.std_str() + ", please review connection manager.").c_str(), JVX_REPORT_PRIORITY_ERROR);
					}
					theDataConnections->return_reference_connection_process(theRef);
				}
				else
				{
					std::cout << "Failed to find connection process with ID = " << uId << std::endl;
				}
				involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theDataConnections));
				theDataConnections = NULL;
			}
		}
		this->updateWindow(request);
		if (this->subWidgets.realtimeViewer.props.theWidget)
		{
			this->subWidgets.realtimeViewer.props.theWidget->updateWindow_update(false);
		}
	}
}


jvxErrorType
uMainWindow::report_internals_have_changed(const jvxComponentIdentification& tpFrom, IjvxObject* theObj, jvxPropertyCategoryType cat, 
	jvxSize propId, bool onlyContent, jvxSize offset_start, jvxSize numElements,
	const jvxComponentIdentification& tpTo,	jvxPropertyCallPurpose callpurpose)
{
	// Check config lines..
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	IjvxPropertyPool* thePropPool = NULL;
	jvxBool fullUpdateRequired = false;
/*
	// Follow deployment of properties as specified in the configuration lines
	IjvxConfigurationLine* theConfigLine = NULL;


	// Config parameters to config line
	res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLine));

	if((res == JVX_NO_ERROR) && theConfigLine)
	{
		theConfigLine->report_property_was_set(tpFrom, cat, propId, onlyContent, tpTo);


		this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle*>(theConfigLine));
	}
	*/

	// Config parameters to config line
	res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTY_POOL, reinterpret_cast<jvxHandle**>(&thePropPool));

	if ((res == JVX_NO_ERROR) && thePropPool)
	{
		resL = thePropPool->report_property_changed(propId, cat, onlyContent, offset_start, numElements, tpFrom);
		if (resL == JVX_NO_ERROR)
		{
			fullUpdateRequired = true;
		}
		this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTY_POOL, reinterpret_cast<jvxHandle*>(thePropPool));
	}

	// If we have modified more than one parameter, we need to update full UI
	if (fullUpdateRequired)
	{
		jvxCBitField request;
		jvx_bitZSet(request, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
		this->report_command_request(
			request, NULL, 0);
	}
	else
	{
		// Report to update all UIs since important stuff has changed
		report_internals_have_changed_outThread(tpFrom, theObj, cat, propId, onlyContent, tpTo, callpurpose);
	}
	return(JVX_NO_ERROR);
}


jvxErrorType
uMainWindow::report_take_over_property(const jvxComponentIdentification& tpFrom, IjvxObject* objRef,
                                       jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
									   jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvx_propertyReferenceTriple theTriple;
	jvxCallManagerProperties callGate;
	jvx_initPropertyReferenceTriple(&theTriple);
	jvx_getReferencePropertiesObject(this->involvedHost.hHost, &theTriple, tpTo);

	if(jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		res = theTriple.theProps->request_takeover_property(callGate,
			tpFrom, objRef,
			fld, numElements, format, offsetStart, onlyContent,
			cat, propId);
		jvx_returnReferencePropertiesObject(this->involvedHost.hHost, &theTriple, tpTo);
	}
	return(res);
}


void
uMainWindow::report_internals_have_changed_outThread(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId, 
	bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurp)
{
	emit emit_report_internals_have_changed_inThread(tp, theObj, cat, propId, onlyContent, tpTo, callpurp);
}

void
uMainWindow::report_internals_have_changed_inThread( jvxComponentIdentification tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId, 
	bool onlyContent,  jvxComponentIdentification tpTo, jvxPropertyCallPurpose callpurp)
{
	if(callpurp == JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
	{
		if(onlyContent)
		{
			this->subWidgets.realtimeViewer.props.theWidget->updateWindow_content();
		}
		else
		{
			// Currently no difference
			this->subWidgets.realtimeViewer.props.theWidget->updateWindow_rebuild(JVX_SIZE_UNSELECTED);
		}
	}

	if(subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->inform_internals_have_changed(tp, theObj, cat, propId, onlyContent, tpTo, callpurp);
	}
}

jvxErrorType
uMainWindow::report_os_specific(jvxSize commandId, void* context)
{
	return(JVX_ERROR_UNSUPPORTED);
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
uMainWindow::pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp)
{
	jvx_componentIdPropsCombo newRefElm;
	switch (ss)
	{
	case JVX_STATE_SWITCH_UNSELECT:
		jvx_componentIdentification_properties_find(
			theRegisteredComponentReferenceTriples,
			tp, newRefElm);

		jvx_componentIdentification_properties_erase(
			involvedHost.hHost,
			theRegisteredComponentReferenceTriples,
			tp);
		break;
	case JVX_STATE_SWITCH_DEACTIVATE:

		jvx_componentIdentification_properties_find(
			theRegisteredComponentReferenceTriples,
			tp, newRefElm);

		if (subWidgets.main.theWidget)
		{
			subWidgets.main.theWidget->inform_inactive(tp, newRefElm.accProps);
		}
		break;
	default:
		break;
	}

	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc)
{
	jvx_componentIdPropsCombo newRefElm;
	switch (ss)
	{
	case JVX_STATE_SWITCH_SELECT:
		jvx_componentIdentification_properties_direct_add(
			involvedHost.hHost,
			theRegisteredComponentReferenceTriples,
			tp, &newRefElm);
		break;
	case JVX_STATE_SWITCH_ACTIVATE:

		jvx_componentIdentification_properties_find(
			theRegisteredComponentReferenceTriples,
			tp, newRefElm);

		if (subWidgets.main.theWidget)
		{
			subWidgets.main.theWidget->inform_active(tp, newRefElm.accProps);
		}
		break;
	default:
		break;
	}
	return(JVX_NO_ERROR);
}

/* TODO
void
uMainWindow::triggered_showsecret_fld()
{
	subWidgets.main.theWidget->showSecretField();
}
*/

jvxErrorType
uMainWindow::forward_trigger_operation(
		jvxMainWindowController_trigger_operation_ids id_operation,
		jvxHandle* theData)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvxCBitField bfld = 0;
	switch(id_operation)
	{
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_UPDATE_WINDOW:
		updateWindow();
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_START:
		res = this->control_startSequencer();
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SEQ_STOP:
		res = this->control_stopSequencer();
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_CONT:
		res = this->control_contSequencer();
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_HIDE_EXPERT_UI:
		//subWidgets.sequences.theWidgetD->hide();
		subWidgets.sequencer.theWidgetD->hide();
		subWidgets.realtimeViewer.props.theWidgetD->hide();
		subWidgets.realtimeViewer.plots.theWidgetD->hide();
		subWidgets.connections.theWidgetD->hide();
		subWidgets.messages.theWidgetD->hide();
		menuConfiguration->menuAction()->setVisible(false);
		menuView->menuAction()->setVisible(false);
		menuRuntime_Parameters->menuAction()->setVisible(false);
		menuSettings->menuAction()->setVisible(false);
		lockKeyShortcuts = true;
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_SHOW_EXPERT_UI:
		//subWidgets.sequences.theWidgetD->show();
		subWidgets.sequencer.theWidgetD->show();
		subWidgets.realtimeViewer.props.theWidgetD->show();
		subWidgets.realtimeViewer.plots.theWidgetD->show();
		subWidgets.connections.theWidgetD->show();
		subWidgets.messages.theWidgetD->show();
		menuConfiguration->menuAction()->setVisible(true);
		menuView->menuAction()->setVisible(true);
		menuRuntime_Parameters->menuAction()->setVisible(true);
		menuSettings->menuAction()->setVisible(true);
		lockKeyShortcuts = false;

		res = JVX_NO_ERROR;
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
		this->openConfigFile_core((const char*)theData, false);
		res = JVX_NO_ERROR;
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_OPEN_CONFIG_OVLAY:
		this->openConfigFile_core((const char*)theData, true);
		res = JVX_NO_ERROR;
		break;

	case JVX_MAINWINDOWCONTROLLER_TRIGGER_UPDATE_VIEWER:
		if (subWidgets.realtimeViewer.props.theWidget)
		{
			subWidgets.realtimeViewer.props.theWidget->_trigger_viewer();
		}
		if (subWidgets.realtimeViewer.plots.theWidget)
		{
			subWidgets.realtimeViewer.plots.theWidget->_trigger_viewer();
		}
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_IMMEDIATE_TEST_ALL_CHAINS:
		jvx_bitSet(bfld, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT);
		this->report_command_request_inThread(bfld, NULL, 0);
		break;

	default:
		break;
	}

	return(res);
}

void 
uMainWindow::report_sequencer_callback(jvxSize fId)
{
	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->inform_sequencer_callback(fId);
	}
}

void 
uMainWindow::report_sequencer_stopped()
{
	sequencer_stopped();
}

void 
uMainWindow::report_sequencer_error(const char* str1, const char* str2)
{
	reportSequencerError_inThread(str1, str2);
}

void
uMainWindow::report_sequencer_update_timeout()
{
}



