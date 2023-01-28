#include <QtGui/QKeyEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QSplashScreen>

//#define DEFAULT_NAME_CONFIG_FILE ".session.rtproc"
//#define POSTFIX_DONT_LOAD_CONFIG "--no-config"
#include "uMainWindow.h"
#include "jvx-qt-helpers.h"

#define SMOOTHING_ALPHA 0.6
#define UPDATE_PERIOD 1000

uMainWindow::uMainWindow(): epLeft(SMOOTHING_ALPHA), epRight(SMOOTHING_ALPHA)
{
	JVX_REGISTER_QT_METATYPES

	devices.theSocketFamily = NULL;

	devices.deviceLeft = NULL;
	devices.deviceRight = NULL;

	devices.idLeft = JVX_SIZE_UNSELECTED;
	devices.idRight = JVX_SIZE_UNSELECTED;

	filters.numAvailableFilters = 0;

	devices.started = false;

	JVX_INITIALIZE_MUTEX(devices.saveSendLeft);
	JVX_INITIALIZE_MUTEX(devices.saveSendRight);
}

uMainWindow::~uMainWindow(void)
{
	JVX_TERMINATE_MUTEX(devices.saveSendLeft);
	JVX_TERMINATE_MUTEX(devices.saveSendRight);
}

jvxErrorType
uMainWindow::initialize(QApplication* hdlApp, char* clStr[], int clNum)
{
	jvxApiString errMess;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	char tt[JVX_MAXSTRING];
	setupUi(this);
	myTimer = NULL;

	jvxComponentIdentification cpType;
	jvxBool multObj;
	QSplashScreen*spl = NULL;

#ifndef JVX_SPLASH_SUPPRESSED

	spl = jvx_start_splash_qt();
	if(spl)
	{
		spl->showMessage("Starting host...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	commLine.assign_args((const char**)&clStr[1], clNum - 1, clStr[0]);
	devices.num = 0;

	// Start device family
	res = jvx_connect_client_family_initialize_st(&devices.theSocketFamily, JVX_CONNECT_SOCKET_TYPE_PCAP);
	assert(res == JVX_NO_ERROR);

	res = jvx_connect_client_family_number_interfaces_st(devices.theSocketFamily, &devices.num);
	assert(res == JVX_NO_ERROR);
	assert(devices.num >= 2);

	for(i = 0; i < devices.num; i++)
	{ 
		memset(tt, 0, JVX_MAXSTRING);
		jvx_connect_client_family_description_interfaces_st(devices.theSocketFamily, tt, JVX_MAXSTRING-1, i);
		comboBox_ifs_left->addItem(tt);
		comboBox_ifs_right->addItem(tt);
	}
	
	devices.idLeft = 0;
	devices.idRight = 1;

	devices.laneLeft.association = JVX_PACKETFILTER_LEFT;
	devices.laneRight.association = JVX_PACKETFILTER_RIGHT;

	devices.laneLeft.thisisme = this;
	devices.laneRight.thisisme = this;

	myCallbacks.common.callback_provide_data_and_length = callback_provide_data_and_length;
	myCallbacks.common.callback_report_data_and_read = callback_report_data_and_read;
	myCallbacks.common.callback_report_error = callback_report_error;
	myCallbacks.common.callback_report_shutdown_complete = callback_report_shutdown_complete;
	myCallbacks.common.callback_report_specific = callback_report_specific;
	myCallbacks.common.callback_report_startup_complete = callback_report_startup_complete;

	myCallbacks.callback_report_connect_outgoing = callback_report_connect_outgoing;
	myCallbacks.callback_report_disconnect_outgoing = callback_report_disconnect_outgoing;
	myCallbacks.callback_report_unsuccesful_outgoing = callback_report_unsuccesful_outgoing;

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Configuring factory host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	res = this->boot_configure(&errMess, static_cast<IjvxCommandLine*>(&commLine), static_cast<IjvxReport*>(this), NULL, NULL, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Initializing factory host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	res = boot_initialize(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Activating factory host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
		
#endif

	res = boot_activate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if(spl)
	{
		spl->showMessage("Preparing factory host ...", Qt::AlignHCenter, QColor(0, 176, 240));
		qApp->processEvents();
	}
#endif

	res = boot_prepare(&errMess, spl);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	updateWindow();

#ifndef JVX_SPLASH_SUPPRESSED
	jvx_stop_splash_qt(spl, this);
#endif

	return JVX_NO_ERROR;
}

void
uMainWindow::updateWindow()
{
	if (devices.started)
	{
		pushButton_startstop->setText("Stop");
		comboBox_ifs_left->setEnabled(false);
		comboBox_ifs_right->setEnabled(false);
	}
	else
	{
		pushButton_startstop->setText("Start");
		comboBox_ifs_left->setEnabled(true); 
		comboBox_ifs_right->setEnabled(true);

		comboBox_ifs_left->setCurrentIndex(JVX_SIZE_INT(devices.idLeft));
		comboBox_ifs_right->setCurrentIndex(JVX_SIZE_INT(devices.idRight));

		lineEdit_lost_left->setText("--");
		lineEdit_lost_right->setText("--");
	}
}

void 
uMainWindow::reportOneInPackage(jvxHandle* buf, jvxSize szFld, laneAssociation assoc)
{
	jvxTick tcnt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	switch (assoc)
	{
	case JVX_PACKETFILTER_LEFT:
		epLeft.update_in(szFld, tcnt);
		queues.leftrightqueue.add_one_packet(0, (jvxByte*)buf, szFld, NULL, JVX_SIZE_UNSELECTED);
		break;
	case JVX_PACKETFILTER_RIGHT:
		epRight.update_in(szFld, tcnt);
		queues.rightleftqueue.add_one_packet(0, (jvxByte*)buf, szFld, NULL, JVX_SIZE_UNSELECTED);
		break;
	}
}

// =========================================================================
// SLOTS SLOTS SLOTS SLOTS SLOTS SLOTS SLOTS SLOTS SLOTS SLOTS SLOTS SLOTS
// =========================================================================

void
uMainWindow::updateWindowPeriodic()
{
	jvxData showmeDat;
	jvxSize showmeSz;
	jvxSize showmeSz2;
	std::string txt;

	epLeft.data_in(showmeDat, showmeSz);

	txt = jvx_data2String(showmeDat, 1);
	lineEdit_epL_in_Rate->setText(txt.c_str());

	txt = jvx_size2String(showmeSz);
	lineEdit_epL_in_numPackets->setText(txt.c_str());

	epLeft.data_out(showmeDat, showmeSz);

	txt = jvx_data2String(showmeDat, 1);
	lineEdit_epL_out_Rate->setText(txt.c_str());

	txt = jvx_size2String(showmeSz);
	lineEdit_epL_out_numPackets->setText(txt.c_str());

	queues.leftrightqueue.data_admin(showmeSz, showmeSz2);
	txt = jvx_size2String(showmeSz) + "/" + jvx_size2String(showmeSz2);
	lineEdit_lost_left->setText(txt.c_str());

	// ===================================================================

	epRight.data_in(showmeDat, showmeSz);

	txt = jvx_data2String(showmeDat, 1);
	lineEdit_epR_in_Rate->setText(txt.c_str());

	txt = jvx_size2String(showmeSz);
	lineEdit_epR_in_numPackets->setText(txt.c_str());

	epRight.data_out(showmeDat, showmeSz);

	txt = jvx_data2String(showmeDat, 1);
	lineEdit_epR_out_Rate->setText(txt.c_str());

	txt = jvx_size2String(showmeSz);
	lineEdit_epR_out_numPackets->setText(txt.c_str());

	queues.rightleftqueue.data_admin(showmeSz, showmeSz2);
	txt = jvx_size2String(showmeSz) + "/" + jvx_size2String(showmeSz2);
	lineEdit_lost_right->setText(txt.c_str());

}

void
uMainWindow::select_cb_ifs_left(int idx)
{
	jvxSize idLeftOld = devices.idLeft;
	devices.idLeft = idx;
	if (devices.idLeft == devices.idRight)
	{
		devices.idRight = idLeftOld;
	}
	updateWindow();
}

void
uMainWindow::select_cb_ifs_right(int idx)
{
	jvxSize idRightOld = devices.idRight;
	devices.idRight = idx;
	if (devices.idLeft == devices.idRight)
	{
		devices.idLeft = idRightOld;
	}
	updateWindow();
}

void
uMainWindow::pushed_start()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize context_id = 0;
	std::list<oneFilterInstance>::iterator elm = filters.fRules.begin();

	if (devices.started)
	{

		// Stop everything
		res = jvx_connect_client_trigger_shutdown_st(devices.deviceLeft);
		assert(res == JVX_NO_ERROR);
		res = jvx_connect_client_trigger_shutdown_st(devices.deviceRight);
		assert(res == JVX_NO_ERROR);

		// Stop everything
		res = jvx_connect_client_optional_wait_for_shutdown_st(devices.deviceLeft);
		assert(res == JVX_NO_ERROR);
		res = jvx_connect_client_optional_wait_for_shutdown_st(devices.deviceRight);
		assert(res == JVX_NO_ERROR);

		res = jvx_connect_client_stop_queue_st(devices.deviceLeft);
		assert(res == JVX_NO_ERROR);
		res = jvx_connect_client_stop_queue_st(devices.deviceRight);
		assert(res == JVX_NO_ERROR);

		res = jvx_connect_client_interface_terminate_st(devices.deviceLeft);
		assert(res == JVX_NO_ERROR);
		res = jvx_connect_client_interface_terminate_st(devices.deviceRight);
		assert(res == JVX_NO_ERROR);

		myTimer->stop();
		disconnect(myTimer, SIGNAL(timeout()));
		delete(myTimer);
		myTimer = NULL;

		queues.leftrightqueue.terminate();
		queues.rightleftqueue.terminate();

		devices.started = false;

		context_id = 0;
		elm = filters.fRules.begin();
		for (; elm != filters.fRules.end(); elm++)
		{
			(*elm).thRule_core->postprocess_rule(context_id);
		}

		context_id = 1;
		elm = filters.fRules.begin();
		for (; elm != filters.fRules.end(); elm++)
		{
			(*elm).thRule_core->postprocess_rule(context_id);
		}

	}
	else
	{
		context_id = 0;
		elm = filters.fRules.begin();
		for (; elm != filters.fRules.end(); elm++)
		{
			// LEFT TO RIGHT
			(*elm).thRule_core->preprare_rule(context_id, 
				static_cast<IjvxOnePacketQueue_queue*>(&queues.leftrightqueue), static_cast<IjvxOnePacketQueue_send*>(&queues.leftrightqueue),
				static_cast<IjvxOnePacketQueue_queue*>(&queues.rightleftqueue), static_cast<IjvxOnePacketQueue_send*>(&queues.rightleftqueue));
		}

		context_id = 1;
		elm = filters.fRules.begin();
		for (; elm != filters.fRules.end(); elm++)
		{
			(*elm).thRule_core->preprare_rule(context_id, 
				static_cast<IjvxOnePacketQueue_queue*>(&queues.rightleftqueue), static_cast<IjvxOnePacketQueue_send*>(&queues.rightleftqueue),
				static_cast<IjvxOnePacketQueue_queue*>(&queues.leftrightqueue), static_cast<IjvxOnePacketQueue_send*>(&queues.leftrightqueue));
		}

		JVX_GET_TICKCOUNT_US_SETREF(&tStamp);

		myTimer = new QTimer(this);
		connect(myTimer, SIGNAL(timeout()), this, SLOT(updateWindowPeriodic()));
		myTimer->start(UPDATE_PERIOD);

		epLeft.reset();
		epRight.reset();

		queues.leftrightqueue.initialize(this, CjvxOneQueueGateway::JVX_PACKETFILTER_LEFT_TO_RIGHT, 
			JVX_NUMBER_ENTRIES_QUEUE, JVX_MTU_SIZE_MAX, JVX_DEFAULT_TIMEOUT_PACKET_DELAY_MS, 
			static_cast<IjvxOnePacketQueue_send*>(&queues.rightleftqueue), "Left To Right Queue");
		queues.rightleftqueue.initialize(this, CjvxOneQueueGateway::JVX_PACKETFILTER_RIGHT_TO_LEFT, 
			JVX_NUMBER_ENTRIES_QUEUE, JVX_MTU_SIZE_MAX, JVX_DEFAULT_TIMEOUT_PACKET_DELAY_MS, 
			static_cast<IjvxOnePacketQueue_send*>(&queues.leftrightqueue), "Right To Left Queue");

		jvx_connect_spec_pcap cfgPcap;

		cfgPcap.timeoutConnect_msec = 1000;
		cfgPcap.receive_mode = 0;
		jvx_bitSet(cfgPcap.receive_mode, JVX_PCAP_RECEIVE_OUTPUT_RAW_SHIFT);
		jvx_bitSet(cfgPcap.receive_mode, JVX_PCAP_RECEIVE_FILTER_NO_SELF_CAPTURE_SHIFT);

		// Start everything
		res = jvx_connect_client_interface_initialize_st(devices.theSocketFamily, &devices.deviceLeft, &myCallbacks, &devices.laneLeft, 100, devices.idLeft);
		assert(res == JVX_NO_ERROR);
		res = jvx_connect_client_interface_initialize_st(devices.theSocketFamily, &devices.deviceRight, &myCallbacks, &devices.laneRight, 100, devices.idRight);
		assert(res == JVX_NO_ERROR);

		// Start everything

		res = jvx_connect_client_start_queue_st(devices.deviceLeft, JVX_MTU_SIZE_MAX, JVX_PCAP_NUM_FLDS_QUEUE);
		assert(res == JVX_NO_ERROR);
		res = jvx_connect_client_start_queue_st(devices.deviceRight, JVX_MTU_SIZE_MAX, JVX_PCAP_NUM_FLDS_QUEUE);
		assert(res == JVX_NO_ERROR);

		res = jvx_connect_client_interface_connect_st(devices.deviceLeft, &cfgPcap, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_PCAP);
		assert(res == JVX_NO_ERROR);
		res = jvx_connect_client_interface_connect_st(devices.deviceRight, &cfgPcap, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_PCAP);
		assert(res == JVX_NO_ERROR);

		res = jvx_connect_client_set_thread_priority_st(devices.deviceLeft, JVX_PRIORITY_REALTIME);
		assert(res == JVX_NO_ERROR);
		res = jvx_connect_client_set_thread_priority_st(devices.deviceRight, JVX_PRIORITY_REALTIME);
		assert(res == JVX_NO_ERROR);

		devices.started = true;
	}
	updateWindow();
}

void 
uMainWindow::closeEvent(QCloseEvent* event)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (involvedComponents.theHost.hFHost)
	{
		jvxApiString errMess;
		res = shutdown_postprocess(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

		res = shutdown_deactivate(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

		res = shutdown_terminate(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}
		commLine.unassign_args();

	}

	// On Mac, closeEvent may happen twice
	event->accept();
	
}

void
uMainWindow::fatalStop(const char* str1, const char* str2)
{
	QMessageBox msgBox(QMessageBox::Critical, str1, str2);
	msgBox.exec();
	exit(-1);
}

jvxErrorType
uMainWindow::filter_packet_left_to_right(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_add_tag, jvxSize if_tag_add, jvxBool* packet_remove)
{
	jvxSize context_id = 0;
	std::list<oneFilterInstance>::iterator elm = filters.fRules.begin();
	for (; elm != filters.fRules.end(); elm++)
	{
		(*elm).thRule_core->process_rule(fld, numBytesUsed, ptr_add_tag, if_tag_add, context_id, packet_remove);
		if (*packet_remove == true)
		{
			break;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::filter_packet_right_to_left(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_add_tag, jvxSize if_tag_add, jvxBool* packet_remove)
{
	jvxSize context_id = 1;
	std::list<oneFilterInstance>::iterator elm = filters.fRules.begin();
	for (; elm != filters.fRules.end(); elm++)
	{
		(*elm).thRule_core->process_rule(fld, numBytesUsed, ptr_add_tag, if_tag_add, context_id, packet_remove);
		if (*packet_remove == true)
		{
			break;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::send_to_left(jvxByte* fld, jvxSize fldSize)
{
	jvx_connect_send_spec_pcap wheretosend;	
	jvxSize szFld = fldSize;
	jvxTick tcnt = 0;
	
	// If we allow unordered send, we need to protect the send
	JVX_LOCK_MUTEX(devices.saveSendLeft);

	// Secure send..
	tcnt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	wheretosend.sendRaw = true;
	
	jvxErrorType res = jvx_connect_client_collect_st(devices.deviceLeft, fld, &szFld, &wheretosend, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_PCAP_TARGET);
	assert(res == JVX_NO_ERROR);

	res = jvx_connect_client_transmit_st(devices.deviceLeft);
	assert(res == JVX_NO_ERROR);

	assert(szFld == fldSize);
	JVX_UNLOCK_MUTEX(devices.saveSendLeft);
	epLeft.update_out(fldSize, tcnt);

	return JVX_NO_ERROR;
}

jvxErrorType
uMainWindow::send_to_right(jvxByte* fld, jvxSize fldSize)
{
	jvx_connect_send_spec_pcap wheretosend;
	jvxSize szFld = fldSize;
	jvxTick tcnt = 0;
	
	// If we allow unordered send, we need to protect the send
	JVX_LOCK_MUTEX(devices.saveSendRight);

	tcnt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

	wheretosend.sendRaw = true;
	
	jvxErrorType res = jvx_connect_client_collect_st(devices.deviceRight, fld, &szFld, &wheretosend, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_PCAP_TARGET);
	assert(res == JVX_NO_ERROR);

	res = jvx_connect_client_transmit_st(devices.deviceRight);
	assert(res == JVX_NO_ERROR);
	assert(szFld == fldSize);

	JVX_UNLOCK_MUTEX(devices.saveSendRight);

	epRight.update_out(fldSize, tcnt);

	return JVX_NO_ERROR;
}

// =============================================================================
// SOCKET CALLBACKS
// =============================================================================
jvxErrorType 
uMainWindow::callback_report_startup_complete(jvxHandle* privateData, jvxSize ifidx)
{
	oneLane* theHandle = (oneLane*)privateData;
	if (theHandle)
	{
		switch (theHandle->association)
		{
		case JVX_PACKETFILTER_LEFT:
			std::cout << "Left device, function " << __FUNCTION__ << std::endl;
			break;
		case JVX_PACKETFILTER_RIGHT:
			std::cout << "Right device, function " << __FUNCTION__ << std::endl;
			break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::callback_report_shutdown_complete(jvxHandle* privateData, jvxSize ifidx)
{
	oneLane* theHandle = (oneLane*)privateData;
	if (theHandle)
	{
		switch (theHandle->association)
		{
		case JVX_PACKETFILTER_LEFT:
			std::cout << "Left device, function " << __FUNCTION__ << std::endl;
			break;
		case JVX_PACKETFILTER_RIGHT:
			std::cout << "Right device, function " << __FUNCTION__ << std::endl;
			break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::callback_provide_data_and_length(jvxHandle* privateData, jvxSize channelId, char**, jvxSize*, jvxSize* offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	oneLane* theHandle = (oneLane*)privateData;
	if (theHandle)
	{
		switch (theHandle->association)
		{
		case JVX_PACKETFILTER_LEFT:
			std::cout << "Left device, function " << __FUNCTION__ << std::endl;
			break;
		case JVX_PACKETFILTER_RIGHT:
			std::cout << "Right device, function " << __FUNCTION__ << std::endl;
			break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::callback_report_data_and_read(jvxHandle* privateData, jvxSize channelId, char*, jvxSize sz, jvxSize offset, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis)
{
	oneLane* theHandle = (oneLane*)privateData;
	if (theHandle)
	{
		switch (theHandle->association)
		{
		case JVX_PACKETFILTER_LEFT:
			std::cout << "Left device, function " << __FUNCTION__ << std::endl;
			break;
		case JVX_PACKETFILTER_RIGHT:
			std::cout << "Right device, function " << __FUNCTION__ << std::endl;
			break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::callback_report_error(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx)
{
	oneLane* theHandle = (oneLane*)privateData;
	if (theHandle)
	{
		switch (theHandle->association)
		{
		case JVX_PACKETFILTER_LEFT:
			std::cout << "Left device, function " << __FUNCTION__ << std::endl;
			break;
		case JVX_PACKETFILTER_RIGHT:
			std::cout << "Right device, function " << __FUNCTION__ << std::endl;
			break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::callback_report_specific(jvxHandle* privateData, jvx_connect_specific_sub_type idWhat, jvxHandle* fld, jvxSize szFld, jvxSize ifidx)
{
	oneLane* theHandle = (oneLane*)privateData;
	if (theHandle)
	{
		switch (idWhat)
		{
		case JVX_CONNECT_SPECIFIC_SUBTYPE_PCAP_RAW:
			theHandle->thisisme->reportOneInPackage(fld, szFld, theHandle->association);

			break;
		default:
			switch (theHandle->association)
			{
			case JVX_PACKETFILTER_LEFT:
				std::cout << "Left device, function " << __FUNCTION__ << std::endl;
				break;
			case JVX_PACKETFILTER_RIGHT:
				std::cout << "Right device, function " << __FUNCTION__ << std::endl;
				break;
			}

		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::callback_report_unsuccesful_outgoing(jvxHandle* privateData, jvx_connect_fail_reason reason, jvxSize ifidx)
{
	oneLane* theHandle = (oneLane*)privateData;
	if (theHandle)
	{
		switch (theHandle->association)
		{
		case JVX_PACKETFILTER_LEFT:
			std::cout << "Left device, function " << __FUNCTION__ << std::endl;
			break;
		case JVX_PACKETFILTER_RIGHT:
			std::cout << "Right device, function " << __FUNCTION__ << std::endl;
			break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::callback_report_connect_outgoing(jvxHandle* privateData, jvxSize ifidx)
{
	oneLane* theHandle = (oneLane*)privateData;
	if (theHandle)
	{
		switch (theHandle->association)
		{
		case JVX_PACKETFILTER_LEFT:
			std::cout << "Left device, function " << __FUNCTION__ << std::endl;
			break;
		case JVX_PACKETFILTER_RIGHT:
			std::cout << "Right device, function " << __FUNCTION__ << std::endl;
			break;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::callback_report_disconnect_outgoing(jvxHandle* privateData, jvxSize ifidx)
{
	oneLane* theHandle = (oneLane*)privateData;
	if (theHandle)
	{
		switch (theHandle->association)
		{
		case JVX_PACKETFILTER_LEFT:
			std::cout << "Left device, function " << __FUNCTION__ << std::endl;
			break;
		case JVX_PACKETFILTER_RIGHT:
			std::cout << "Right device, function " << __FUNCTION__ << std::endl;
			break;
		}
	}
	return(JVX_NO_ERROR);
}


jvxErrorType 
uMainWindow::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	std::cout << txt << std::endl;
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::report_internals_have_changed(const jvxComponentIdentification&, IjvxObject*, jvxPropertyCategoryType cat,
	jvxSize propId, bool, jvxSize offs, jvxSize num, const jvxComponentIdentification&, jvxPropertyCallPurpose callpurpose)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::report_command_request(
	jvxCBitField request, 
	jvxHandle* caseSpecificData,
	jvxSize szSpecificData)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::request_command(const CjvxReportCommandRequest& request) 
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::interface_sub_report(IjvxSubReport** subReport)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
uMainWindow::report_take_over_property(const jvxComponentIdentification&  thisismytype, IjvxObject* thisisme,
	jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
	jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification&  tpTo,
	jvxPropertyCallPurpose callpurpose)
{
	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::report_os_specific(jvxSize commandId, void* context)
{
	return(JVX_NO_ERROR);
}

// ==================================================================================

jvxErrorType
uMainWindow::boot_specify_slots_specific()
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::boot_initialize_specific(jvxApiString* errloc)
{
	involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_PACKETFILTER_RULE, false);
	involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_CONFIG_PROCESSOR, false);

	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::boot_prepare_specific(jvxApiString* errloc)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::boot_start_specific(jvxApiString* errloc)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::boot_activate_specific(jvxApiString* errloc)
{
	jvxSize i;
	jvxErrorType res = involvedComponents.theTools.hTools->number_tools(JVX_COMPONENT_PACKETFILTER_RULE, &filters.numAvailableFilters);
	assert(res == JVX_NO_ERROR);

	for (i = 0; i < filters.numAvailableFilters; i++)
	{
		jvxApiString descr;
		jvxApiString descror;
		involvedComponents.theTools.hTools->identification_tool(JVX_COMPONENT_PACKETFILTER_RULE, i, &descr, &descror, NULL);
		comboBox_filterrules->addItem(descr.c_str());
	}	
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::shutdown_terminate_specific(jvxApiString* errloc)
{
	involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_CONFIG_PROCESSOR, false);
	involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_PACKETFILTER_RULE, false);
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::shutdown_deactivate_specific(jvxApiString* errloc)
{
	comboBox_filterrules->clear();
	filters.numAvailableFilters = 0;
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::shutdown_postprocess_specific(jvxApiString* errloc)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::shutdown_stop_specific(jvxApiString* errloc)
{
	return(JVX_NO_ERROR);
}

void 
uMainWindow::report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context)
{
}

void 
uMainWindow::inform_config_read_start(const char* nmfile, jvxHandle* context)
{
}

void 
uMainWindow::inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context)
{
}


void 
uMainWindow::get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{

}

void 
uMainWindow::get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data)
{

}

void 
uMainWindow::put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName)
{
}

void 
uMainWindow::put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{
}

void 
uMainWindow::report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName)
{

}

void
uMainWindow::select_frule(int id)
{
	filters.idSelect = id;
}

void
uMainWindow::add_rule()
{
	oneFilterInstance theNewInst;
	if (filters.idSelect < filters.numAvailableFilters)
	{
		theNewInst.theRule_obj = NULL;
		theNewInst.thRule_core = NULL;
		theNewInst.theRule_properties = NULL;

		involvedComponents.theTools.hTools->instance_tool(JVX_COMPONENT_PACKETFILTER_RULE, &theNewInst.theRule_obj, filters.idSelect, NULL);
		if (theNewInst.theRule_obj)
		{
			theNewInst.theRule_obj->request_specialization(reinterpret_cast<jvxHandle**>(&theNewInst.thRule_core), NULL, NULL, NULL);
			if (theNewInst.thRule_core)
			{
				theNewInst.thRule_core->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theNewInst.theRule_properties));

				filters.fRules.push_back(theNewInst);
			}
		}
		updateWindow();
	}
}

void
uMainWindow::rem_rule()
{

}
