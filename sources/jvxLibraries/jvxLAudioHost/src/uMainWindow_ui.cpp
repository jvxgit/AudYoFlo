#include "jvx-qt-helpers.h"
#include "uMainWindow.h"
#include "jvxAudioHost_common.h"

void
uMainWindow::load_icon(QLabel* lab, const std::string coretxt)
{
	QPixmap iconP;
	std::string whattoload;
	std::string whattoloade;
	bool loaded = false;
	bool setSizeHalf = false;
	whattoload = theHostFeatures.config_ui.foldericons + coretxt;
	if (theHostFeatures.config_ui.smallicons)
	{
		whattoloade = whattoload + theHostFeatures.config_ui.smallposticons + "." + theHostFeatures.config_ui.endingicons;
		loaded = iconP.load(whattoloade.c_str());
		if (!loaded)
		{
			std::cout << "Failed to load small icon <" << whattoloade << std::endl;
		}
		else
		{
			setSizeHalf = true;
		}
	}
	if (!loaded)
	{
		whattoloade = whattoload + "." + theHostFeatures.config_ui.endingicons;
		loaded = iconP.load(whattoloade.c_str());
		if (!loaded)
		{
			std::cout << "Failed to load icon <" << whattoloade << std::endl;
		}
	}
	
	lab->setPixmap(iconP);
	if (setSizeHalf)
	{
		lab->setMinimumSize(QSize(theHostFeatures.config_ui.iconw_small, 
			theHostFeatures.config_ui.iconh_small));
		lab->setMaximumSize(QSize(theHostFeatures.config_ui.iconw_small, 
			theHostFeatures.config_ui.iconh_small));
	}
	else
	{
		lab->setMinimumSize(QSize(theHostFeatures.config_ui.iconw_normal, 
			theHostFeatures.config_ui.iconh_normal));
		lab->setMaximumSize(QSize(theHostFeatures.config_ui.iconw_normal, 
			theHostFeatures.config_ui.iconh_normal));
	}
	/*
	QSize szz = this->minimumSize();
	std::cout << "Min: " << szz.width() << ":" << szz.height() << std::endl;
	QRect rr = this->geometry();
	std::cout << "Current: " << rr.width() << ":" << rr.height() << std::endl;
	*/
}

void
uMainWindow::updateWindow(jvxCBitField prio)
{
	jvxSize i;
	jvxBool checkAudioDialog = false;
	jvxBool checkAudioArgs = false;
	jvxBool checkConnectionDialog = false;
	jvxBool checkSequencerDialog = false;

	jvxErrorType res;
	IjvxObject* theObj = NULL;
	jvxBool iamready = false;
	jvxBool weareready = true;
	jvxSize num = 0;
	jvxBool markedForProcess = false;
	jvxState stat = JVX_STATE_NONE;
	std::string txt_error_ready;
	jvxApiString  reasonIfNot;
	jvxComponentIdentification tpLoc;
	jvxSize numSubSlots = 0;
	jvxSize numSlots = 0;

	/*
	load_icon(label_dev, "icon_audiodev");
	load_icon(label_algo, "icon_algorithm");
	*/
	load_icon(label_seq, "icon_sequencer");

	if (!involvedComponents.theHost.hFHost)
	{
		return;
	}

	if (subWidgets.theAudioDialog && subWidgets.theAudioArgs)
	{
		subWidgets.theAudioDialog->refreshPropertyIds();
		subWidgets.theAudioDialog->updateWindow();
		if (subWidgets.theAudioDialog->isVisible())
		{
			checkAudioDialog = true;
		}

		// subWidgets.theAudioArgs->refreshPropertyIds();<- this is part of the audio dialog already
		//subWidgets.theAudioArgs->updateWindow();<- this is part of the audio dialog already
		if (subWidgets.theAudioArgs->isVisible())
		{
			checkAudioArgs = true;
		}
	}
	actionOpen_Audio_Configuration->setChecked(checkAudioDialog);
	actionOpen_Audio_Arguments->setChecked(checkAudioArgs);
	/*
		if (subWidgets.main.theWidget)
		{
			subWidgets.main.theWidget->inform_update_window(prio);
		}
		* This update happens a little later in line 252 *
	*/

	// ===================================================================
	// ===================================================================
	theControl.update_window();

	// ===================================================================
	jvxSequencerStatus seqStat = JVX_SEQUENCER_STATUS_NONE;
	if (subWidgets.theSequencerWidget)
	{
		subWidgets.theSequencerWidget->status_process(&seqStat, NULL);
	}

	if (seqStat == JVX_SEQUENCER_STATUS_NONE)
	{
		// =================================================================

		// Check if audio devices are ready
#if 0
		iamready = true;
		tpLoc.reset(JVX_COMPONENT_AUDIO_DEVICE);
		numSubSlots = 0;
		involvedHost.hHost->number_slots_component(tpLoc, &numSlots, &numSubSlots);
		if (numSubSlots == 0)
		{
			iamready = false;
		}
		for (i = 0; i < numSubSlots; i++)
		{
			jvxBool iamreadyLoc = false;
			tpLoc.slotsubid = i;
			res = involvedHost.hHost->is_ready_component(tpLoc/*tpAll[JVX_COMPONENT_AUDIO_DEVICE]*/, &iamreadyLoc, &reasonIfNot);
			iamready = iamready & iamreadyLoc;

			if (!iamreadyLoc)
			{
				if (!txt_error_ready.empty())
				{
					txt_error_ready += "; ";
				}
				txt_error_ready += jvxComponentIdentification_txt(tpLoc) + ": ";
				txt_error_ready += reasonIfNot.std_str();
			}
		}

		if (iamready)
		{
			load_icon(label_dev_ready, "icon_ready");
		}
		else
		{
			weareready = false;
			load_icon(label_dev_ready, "icon_notready");
			//setBackgroundLabelColor(Qt::red, this->label_audiodevice);
		}


		// Check if audio node is ready
		iamready = true;
		tpLoc.reset(JVX_COMPONENT_AUDIO_NODE);
		numSubSlots = 0;
		involvedHost.hHost->number_slots_component(tpLoc, &numSlots, &numSubSlots);
		if (numSubSlots == 0)
		{
			iamready = false;
		}
		for (i = 0; i < numSubSlots; i++)
		{
			jvxBool iamreadyLoc = false;
			tpLoc.slotsubid = i;
			res = involvedHost.hHost->is_ready_component(tpLoc/*tpAll[JVX_COMPONENT_AUDIO_DEVICE]*/, &iamreadyLoc, &reasonIfNot);
			iamready = iamready & iamreadyLoc;
			if (!iamreadyLoc)
			{
				if (!txt_error_ready.empty())
				{
					txt_error_ready += "; ";
				}
				txt_error_ready += jvxComponentIdentification_txt(tpLoc) + ": ";
				txt_error_ready += reasonIfNot.std_str();
			}
		}

		//iamready = false;
		//res = involvedComponents.theHost.hFHost->is_ready_component(tpAll[JVX_COMPONENT_AUDIO_NODE], &iamready, &reasonIfNot);
		if (iamready)
		{
			load_icon(label_algo_ready, "icon_ready");
			//setBackgroundLabelColor(Qt::green, this->label_algorithm);
		}
		else
		{
			weareready = false;
			load_icon(label_algo_ready, "icon_notready");
			//setBackgroundLabelColor(Qt::red, this->label_algorithm);
		}
#endif

		jvxApiString astr_ready;
		res = subWidgets.theConnectionsWidget->check_connections_ready(&astr_ready);
		if (res == JVX_NO_ERROR)
		{
			allowStart = true;
			load_icon(label_seq_ready, "icon_ready");
			txt_error_ready = "";
			setBackgroundLabelColor(Qt::green, label_status_processes);
		}
		else
		{
			allowStart = false;
			load_icon(label_seq_ready, "icon_notready");
			txt_error_ready = astr_ready.std_str();
			weareready = false;
			setBackgroundLabelColor(Qt::blue, label_status_processes);
		}
		
		/*
		// Check if all processes are ready
		IjvxDataConnections* theDataConnections = NULL;
		IjvxDataConnectionProcess* theProc = NULL;
		jvxState statProc = JVX_STATE_NONE;
		jvxBool lastTestOk = false;
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
		jvxBool allTestsOk = true;
		std::vector<std::string> errMessagesConnect;
		jvxBool incompleteChain = false;

		//std::cout << "Hier!" << std::endl;
		res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));
		assert(theDataConnections);
		theDataConnections->number_connections_process(&num);
		for (i = 0; i < num; i++)
		{
			theDataConnections->reference_connection_process(i, &theProc);
			if (theProc)
			{
				theProc->status(&statProc);
				theProc->status_chain(&lastTestOk JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
				if (statProc < JVX_STATE_ACTIVE)
				{
					incompleteChain = true;
					//std::cout << "- Not Ready -" << std::endl;
				}
				else
				{
					if (!lastTestOk)
					{
						//std::cout << "- Error -" << std::endl;
						allTestsOk = false;
					}
					else
					{
						//std::cout << "- No Error -" << std::endl;
					}
				}
				theDataConnections->return_reference_connection_process(theProc);
				theProc = NULL;
			}
		}

		if (!allTestsOk)
		{
			weareready = false;
			for (i = 0; i < errMessagesConnect.size(); i++)
			{
				this->report_simple_text_message(errMessagesConnect[i].c_str(), JVX_REPORT_PRIORITY_ERROR);
			}
		}
		if (allTestsOk)
		{
			if (incompleteChain)
			{
				setBackgroundLabelColor(Qt::blue, label_status_processes);
			}
			else
			{
				setBackgroundLabelColor(Qt::green, label_status_processes);
			}
		}
		else
		{
			setBackgroundLabelColor(Qt::red, label_status_processes);
		}*/

		QIcon icon;
		if (weareready)
		{
			pushButton->setEnabled(true);
			icon.addFile(QStringLiteral(":/images/images/icon_start.png"), QSize(), QIcon::Normal, QIcon::Off);
		}
		else
		{
			pushButton->setEnabled(false);
			icon.addFile(QStringLiteral(":/images/images/icon_start_notready.png"), QSize(), QIcon::Normal, QIcon::Off);
		}
		pushButton->setIcon(icon);
		pushButton->setIconSize(QSize(48, 48));
		label_error_reason->setText(txt_error_ready.c_str());
		QPalette pa = label_error_reason->palette();
		pa.setColor(QPalette::WindowText, Qt::red);
		label_error_reason->setPalette(pa);

	} //if (seqStat == JVX_SEQUENCER_STATUS_NONE)
	else
	{
//		label_dev_ready->setPixmap(QPixmap(QString::fromUtf8(":/images/images/icon_proc.png")));
//		label_algo_ready->setPixmap(QPixmap(QString::fromUtf8(":/images/images/icon_proc.png")));
		label_seq_ready->setPixmap(QPixmap(QString::fromUtf8(":/images/images/icon_proc.png")));

		QIcon icon;
		icon.addFile(QStringLiteral(":/images/images/icon_stop.png"), QSize(), QIcon::Normal, QIcon::Off);
		pushButton->setIcon(icon);
		pushButton->setIconSize(QSize(48, 48));

		txt_error_ready = "Sequencer Running";
		label_error_reason->setText(txt_error_ready.c_str());
		QPalette pa = label_error_reason->palette();
		pa.setColor(QPalette::WindowText, Qt::green);
		label_error_reason->setPalette(pa);
	}// else: if(stat <= JVX_STATE_ACTIVE)

	

	//lineEdit_error_ready->setText(txt_error_ready.c_str());
	this->updateWindow_specific(prio);

	subWidgets.bridgeMenuComponents->updateWindow();
	subWidgets.bridgeMenuComponents->updateWindow_tools();

	actionAuto_Start->setChecked(genQtAudioHost::host_params.autostart.value);
}

// Slots for userinteractions
void
uMainWindow::trigger_openAudioConfiguration()
{
	if(subWidgets.theAudioDialog->isVisible())
	{
		subWidgets.theAudioDialog->hide();
	}
	else
	{
		subWidgets.theAudioDialog->show();
	}
	this->updateWindow(0);

}

void
uMainWindow::trigger_openAudioArgs()
{
	if (subWidgets.theAudioArgs->isVisible())
	{
		subWidgets.theAudioArgs->hide();
	}
	else
	{
		subWidgets.theAudioArgs->show();
	}
	this->updateWindow(0);

}

void
uMainWindow::trigger_openVideoConfiguration()
{
	std::cout << __FUNCTION__ <<  "Not yet implemented" << std::endl;
}

