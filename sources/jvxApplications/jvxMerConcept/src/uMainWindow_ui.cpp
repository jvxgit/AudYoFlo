#include "uMainWindow.h"

void 
uMainWindow::updateWindow()
{
	jvxBool engaged = false;
	jvxErrorType res = JVX_NO_ERROR;

	pushButton_play->setEnabled(false);
	pushButton_record->setEnabled(false);
	pushButton_process->setEnabled(false);
	pushButton_listen->setEnabled(false);
	pushButton_load->setEnabled(false);
	pushButton_save->setEnabled(false);
	pushButton_edit->setEnabled(false);

	switch(myState)
	{
		case JVX_STANDALONE_HOST_STATE_NONE:

			pushButton_play->setStyleSheet("background-color:gray;");
			pushButton_record->setStyleSheet("background-color:gray;");
			pushButton_process->setStyleSheet("background-color:gray;");
			pushButton_listen->setStyleSheet("background-color:gray;");
			pushButton_load->setStyleSheet("background-color:gray;");
			pushButton_save->setStyleSheet("background-color:gray;");
			pushButton_edit->setStyleSheet("background-color:gray;");

			break;
		case JVX_STANDALONE_HOST_STATE_WAITING:
			
			pushButton_play->setStyleSheet("background-color:green;");
			pushButton_record->setStyleSheet("background-color:green;");
			pushButton_process->setStyleSheet("background-color:green;");
			pushButton_listen->setStyleSheet("background-color:green;");
			pushButton_load->setStyleSheet("background-color:green;");
			pushButton_save->setStyleSheet("background-color:green;");
			pushButton_edit->setStyleSheet("background-color:green;");

			pushButton_play->setEnabled(true);
			pushButton_record->setEnabled(true);
			pushButton_process->setEnabled(true);
			pushButton_listen->setEnabled(true);
			pushButton_load->setEnabled(true);
			pushButton_save->setEnabled(true);
			pushButton_edit->setEnabled(true);

			break;
		case JVX_STANDALONE_HOST_STATE_PLAYING:

			pushButton_play->setStyleSheet("background-color:red;");
			pushButton_record->setStyleSheet("background-color:gray;");
			pushButton_process->setStyleSheet("background-color:gray;");
			pushButton_listen->setStyleSheet("background-color:gray;");
			pushButton_load->setStyleSheet("background-color:gray;");
			pushButton_save->setStyleSheet("background-color:gray;");
			pushButton_edit->setStyleSheet("background-color:gray;");

			pushButton_play->setEnabled(true);

			break;
		case JVX_STANDALONE_HOST_STATE_RECORDING:

			pushButton_play->setStyleSheet("background-color:gray;");
			pushButton_record->setStyleSheet("background-color:red;");
			pushButton_process->setStyleSheet("background-color:gray;");
			pushButton_listen->setStyleSheet("background-color:gray;");
			pushButton_load->setStyleSheet("background-color:gray;");
			pushButton_save->setStyleSheet("background-color:gray;");
			pushButton_edit->setStyleSheet("background-color:gray;");

			pushButton_record->setEnabled(true);

			break;
		case JVX_STANDALONE_HOST_STATE_PROCESSING:

			pushButton_play->setStyleSheet("background-color:gray;");
			pushButton_record->setStyleSheet("background-color:gray;");
			pushButton_process->setStyleSheet("background-color:red;");
			pushButton_listen->setStyleSheet("background-color:gray;");
			pushButton_load->setStyleSheet("background-color:gray;");
			pushButton_save->setStyleSheet("background-color:gray;");
			pushButton_edit->setStyleSheet("background-color:gray;");
			pushButton_process->setEnabled(true);

			break;
		case JVX_STANDALONE_HOST_STATE_LISTENING:

			pushButton_play->setStyleSheet("background-color:gray;");
			pushButton_record->setStyleSheet("background-color:gray;");
			pushButton_process->setStyleSheet("background-color:gray;");
			pushButton_listen->setStyleSheet("background-color:red;");
			pushButton_load->setStyleSheet("background-color:gray;");
			pushButton_save->setStyleSheet("background-color:gray;");
			pushButton_edit->setStyleSheet("background-color:gray;");

			pushButton_listen->setEnabled(true);

			break;

		case JVX_STANDALONE_HOST_STATE_SAVING:

			pushButton_play->setStyleSheet("background-color:gray;");
			pushButton_record->setStyleSheet("background-color:gray;");
			pushButton_process->setStyleSheet("background-color:gray;");
			pushButton_listen->setStyleSheet("background-color:red;");
			pushButton_load->setStyleSheet("background-color:gray;");
			pushButton_save->setStyleSheet("background-color:gray;");
			pushButton_edit->setStyleSheet("background-color:gray;");

			pushButton_listen->setEnabled(true);
			break;
	}

	pushButton_enhance->setStyleSheet("background-color:gray;");
	res = checkStatusAlgorithm_engaged(engaged);
	if(res == JVX_NO_ERROR)
	{
		if(engaged)
		{
			pushButton_enhance->setStyleSheet("background-color:red;");
		}
		else
		{
			pushButton_enhance->setStyleSheet("background-color:green;");
		}
	}

	jvxSize num = 0;
	jvxSize i;
	jvxInt32 sel = -1;
	CjvxOneAudioBuffer::jvxAudioBuffertype tp;
	std::string description;
	std::string filename;
	std::string length;
	jvxData valD = 0;
	jvxInt32 valI = 0;

	this->treeWidget->clear();
	readwrite.theBuffers.number_entries(num);
	readwrite.theBuffers.selection(sel);
	for(i = 0; i < num; i++)
	{
		QTreeWidgetItem* topL = new QTreeWidgetItem;

		readwrite.theBuffers.entry_type(i, tp);
		readwrite.theBuffers.entry_description(i, description);
		readwrite.theBuffers.entry_filename(i, filename);
		readwrite.theBuffers.entry_length_seconds(i, valD);

		if(valD < 60)
		{
			length = jvx_data2String(valD, 0) + "sec";
		}
		else if(valD >= 60)
		{
			valI = floor(valD/60);
			length =  jvx_int2String(valI) + "min::" + jvx_data2String((valD-valI * 60),0) + "sec";
		}
		if(tp == CjvxOneAudioBuffer::JVX_AUDIOBUFFER_FILE)
		{
			topL->setText(0, filename.c_str());
		}
		else
		{
			topL->setText(0, ("Entry #" + jvx_int2String((int)i)).c_str());
		}
		topL->setText(1, description.c_str());
		topL->setText(2, length.c_str());

		QFont ft = topL->font(0);					
		if(i == sel)
		{
			ft.setBold(true);
			topL->setFont(0, ft);
			topL->setFont(1, ft);
			topL->setFont(2, ft);
			QBrush br (Qt::black);
			topL->setForeground(0, br);
		}
		else
		{
			ft.setBold(false);
			topL->setFont(0, ft);
			topL->setFont(1, ft);
			topL->setFont(2, ft);
			QBrush br (Qt::gray);
			topL->setForeground(0, br);
		}

		topL->setData(0, Qt::UserRole, QVariant(i));
		topL->setData(1, Qt::UserRole, QVariant(i));
		topL->setData(2, Qt::UserRole, QVariant(i));
		treeWidget->addTopLevelItem(topL);
		treeWidget->setCurrentItem(topL);
	}

	IjvxProperties* theProps = NULL;
	IjvxObject* theObj = NULL;
	jvxData container[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];
	jvxBool isValid = false;
	jvxData level_dB = 99; 
	std::string txt;
	res = this->involvedComponents.theHost.hHost->request_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if((res == JVX_NO_ERROR) && theProps)
	{
		// Output average level
		isValid = false;
		if(systemParams.inSlider == 0)
		{
			res = theProps->get_property(&container, 2, JVX_DATAFORMAT_DATA, 16, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true, &isValid, NULL);
			if((res == JVX_NO_ERROR ) && (isValid))
			{
				systemParams.in.levelGain_inuse = container[0];
			}
			this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
		}
		else
		{
			res = theProps->get_property(&container, 2, JVX_DATAFORMAT_DATA, 17, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true, &isValid, NULL);
			if((res == JVX_NO_ERROR ) && (isValid))
			{
				systemParams.out.levelGain_inuse = container[0];
			}
			this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
		}

	}

	if(systemParams.inSlider == 0)
	{
		level_dB = 20*log10(systemParams.in.levelGain_inuse);
	}
	else
	{
		level_dB = 20*log10(systemParams.out.levelGain_inuse);
	}
	level_dB += 89;
	level_dB = JVX_MIN(99, level_dB);
	level_dB = JVX_MAX(0, level_dB);
	this->verticalSlider_gain_out->setValue(level_dB);

	if(systemParams.inSlider == 0)
	{
		this->checkBox_mute->setChecked(this->systemParams.in.mute);
	}
	else
	{
		this->checkBox_mute->setChecked(this->systemParams.out.mute);
	}

	this->checkBox_listen->setChecked(systemParams.listen);
	this->checkBox_loop->setChecked(readwrite.theBuffers.loop());

	this->horizontalSlider->setValue(systemParams.inSlider);
}

void 
uMainWindow::newSelection_currentItem(QTreeWidgetItem* it,int column)
{
	QVariant idVEntry = -1;
	idVEntry = it->data(0, Qt::UserRole);

	readwrite.theBuffers.set_selection(idVEntry.toInt());

	this->updatePlotData();
	this->updatePlots();

	this->updateWindow();
}