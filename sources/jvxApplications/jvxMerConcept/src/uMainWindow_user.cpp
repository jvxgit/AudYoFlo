#include <QtWidgets/QFileDialog>

#include "uMainWindow.h"

static void setBackgroundLabelColor(QColor col, QLabel* lab)
{
	QPalette pal;
	lab->setAutoFillBackground(true);
	pal = lab->palette();
    QBrush brush(col);
    brush.setStyle(Qt::SolidPattern);
    pal.setBrush(QPalette::All, QPalette::Window, brush);
    lab->setPalette(pal);
}

void
uMainWindow::pushed_Play()
{
	jvxErrorType res = JVX_NO_ERROR;
	switch(myState)
	{
	case JVX_STANDALONE_HOST_STATE_WAITING:
		myState = JVX_STANDALONE_HOST_STATE_PLAYING;
		res = prepareChannels(myState);
		if(res == JVX_NO_ERROR)
		{
			readwrite.theBuffers.set_offline_mode(false);

			rtViewer.theTimer = new QTimer;
			connect(rtViewer.theTimer, SIGNAL(timeout()), this, SLOT(timeout_rtViewer()));
			rtViewer.theTimer->start(JVX_STANDALONE_HOST_TIMEOUT_RTVIEWER);

			this->checkBox_listen->setEnabled(false);

			start_process();
		}
		else
		{
			myState = JVX_STANDALONE_HOST_STATE_WAITING;
			this->postMessage_outThread("Failed to activate channels as required!", JVX_REPORT_PRIORITY_ERROR);
		}
		break;
	case JVX_STANDALONE_HOST_STATE_PLAYING:
		myState = JVX_STANDALONE_HOST_STATE_WAITING;
		rtViewer.theTimer->stop();
		disconnect(rtViewer.theTimer, SIGNAL(timeout()));
		stop_process();
		break;
	}
	this->updateWindow();
}

void 
uMainWindow::pushed_Record()
{
	jvxErrorType res = JVX_NO_ERROR;
	switch(myState)
	{
	case JVX_STANDALONE_HOST_STATE_WAITING:
		myState = JVX_STANDALONE_HOST_STATE_RECORDING;
		res = prepareChannels(myState);
		if(res == JVX_NO_ERROR)
		{
			readwrite.theBuffers.set_offline_mode(false);

			rtViewer.theTimer = new QTimer;
			connect(rtViewer.theTimer, SIGNAL(timeout()), this, SLOT(timeout_rtViewer()));
			rtViewer.theTimer->start(JVX_STANDALONE_HOST_TIMEOUT_RTVIEWER);

			rtPlot.theTimer = new QTimer;
			connect(rtPlot.theTimer, SIGNAL(timeout()), this, SLOT(timeout_rtPlot()));
			rtPlot.theTimer->start(JVX_STANDALONE_HOST_TIMEOUT_RTPLOT);

			this->checkBox_loop->setEnabled(false);
			this->checkBox_listen->setEnabled(false);
			start_process();
		}
		else
		{
			myState = JVX_STANDALONE_HOST_STATE_WAITING;
			this->postMessage_outThread("Failed to activate channels as required!", JVX_REPORT_PRIORITY_ERROR);
		}
		break;
	case JVX_STANDALONE_HOST_STATE_RECORDING:
		myState = JVX_STANDALONE_HOST_STATE_WAITING;
		rtViewer.theTimer->stop();
		disconnect(rtViewer.theTimer, SIGNAL(timeout()));

		rtPlot.theTimer->stop();
		disconnect(rtPlot.theTimer, SIGNAL(timeout()));

		stop_process();
		break;
	}
	this->updateWindow(); 
}

void 
uMainWindow::pushed_Process()
{
	jvxErrorType res = JVX_NO_ERROR;
	switch(myState)
	{
	case JVX_STANDALONE_HOST_STATE_WAITING:
		myState = JVX_STANDALONE_HOST_STATE_PROCESSING;
		res = prepareChannels(myState);
		if(res == JVX_NO_ERROR)
		{
			rtViewer.theTimer = new QTimer;
			connect(rtViewer.theTimer, SIGNAL(timeout()), this, SLOT(timeout_rtViewer()));
			rtViewer.theTimer->start(JVX_STANDALONE_HOST_TIMEOUT_RTVIEWER);

			readwrite.theBuffers.set_offline_mode(true);

			this->checkBox_listen->setEnabled(false);
			this->checkBox_loop->setEnabled(false);
			start_process();
		}
		else
		{
			myState = JVX_STANDALONE_HOST_STATE_WAITING;
			this->postMessage_outThread("Failed to activate channels as required!", JVX_REPORT_PRIORITY_ERROR);
		}
		break;
	case JVX_STANDALONE_HOST_STATE_PROCESSING:
		myState = JVX_STANDALONE_HOST_STATE_WAITING;

		rtViewer.theTimer->stop();
		disconnect(rtViewer.theTimer, SIGNAL(timeout()));

		stop_process();
		break;
	}
	this->updateWindow();
}

void 
uMainWindow::pushed_Listen()
{
	jvxErrorType res = JVX_NO_ERROR;
	switch(myState)
	{
	case JVX_STANDALONE_HOST_STATE_WAITING:
	
		myState = JVX_STANDALONE_HOST_STATE_LISTENING;
		res = prepareChannels(myState);
		if(res == JVX_NO_ERROR)
		{
			this->checkBox_listen->setEnabled(false);
			this->checkBox_loop->setEnabled(false);

			readwrite.theBuffers.set_offline_mode(false);

			rtViewer.theTimer = new QTimer;
			connect(rtViewer.theTimer, SIGNAL(timeout()), this, SLOT(timeout_rtViewer()));
			rtViewer.theTimer->start(JVX_STANDALONE_HOST_TIMEOUT_RTVIEWER);

			start_process();
		}
		else
		{
			myState = JVX_STANDALONE_HOST_STATE_WAITING;
			this->postMessage_outThread("Failed to activate channels as required!", JVX_REPORT_PRIORITY_ERROR);

		}
		break;
	case JVX_STANDALONE_HOST_STATE_LISTENING:
		myState = JVX_STANDALONE_HOST_STATE_WAITING;

		rtViewer.theTimer->stop();
		disconnect(rtViewer.theTimer, SIGNAL(timeout()));

		stop_process();
		break;
	}
	this->updateWindow();
}

void 
uMainWindow::pushed_Save()
{
	QString str = QFileDialog::getSaveFileName(this, tr("Save Output File"), tr(".\\"), tr("Wavefiles (*.wav)"));

	std::string fileName = (std::string)str.toLatin1().constData();
	fileName = jvx_replaceCharacter(fileName, '/', (JVX_SEPARATOR_DIR)[0]);
	fileName = jvx_absoluteToRelativePath(fileName, true);
	this->writeAudioFile(fileName);
}
 
void 
uMainWindow::pushed_Load()
{
	jvxSize i;
	
	if(myState == JVX_STANDALONE_HOST_STATE_WAITING)
	{
		QStringList lst = QFileDialog::getOpenFileNames(this, tr("Open Input File"), tr(".\\"), tr("Wavefiles (*.wav)"));

		for(i = 0; i < lst.size(); i++)
		{
			std::string fileName = (std::string)lst[i].toLatin1().constData();
			fileName = jvx_replaceCharacter(fileName, '/', (JVX_SEPARATOR_DIR)[0]);
			fileName = jvx_absoluteToRelativePath(fileName, true);
		
			this->readAudioFile(fileName);

			this->updatePlotData();
			this->updatePlots();
		}
		this->updateWindow();
	}
}

void 
uMainWindow::pushed_Edit()
{
	QString txt = this->lineEdit->text();
	readwrite.theBuffers.edit_tag_entry(-1, txt.toLatin1().constData());
	this->updateWindow();
}

void 
uMainWindow::pushed_Enhance()
{
	this->toggleStatusAlgorithm_engaged();
	this->updateWindow();
}

void 
uMainWindow::timeout_rtViewer()
{
	IjvxProperties* theProps = NULL;
	IjvxObject* theObj = NULL;
	jvxData container[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];
	jvxBool isValid = false;
	jvxData level_dB;
	std::string txt;
	jvxErrorType res = this->involvedComponents.theHost.hHost->request_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if((res == JVX_NO_ERROR) && theProps)
	{
		// Output average level
		isValid = false;
		if(systemParams.inSlider == 0)
		{
			res = theProps->get_property(&container, 2, JVX_DATAFORMAT_DATA, 12, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true, &isValid, NULL);
		}
		else
		{
			res = theProps->get_property(&container, 2, JVX_DATAFORMAT_DATA, 13, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true, &isValid, NULL);
		}

		if(res == JVX_NO_ERROR)
		{
			level_dB = 10*log10(container[0]);
			txt = jvx_data2String(level_dB,0);
			level_dB += 99;
			level_dB = JVX_MIN(level_dB, 99);
			level_dB = JVX_MAX(level_dB, 0);
			this->progressBar_out_left->setValue(level_dB);
			this->progressBar_out_left->setToolTip(txt.c_str());
			label_level_left->setText(txt.c_str());

			level_dB = 10*log10(container[1]);
			txt = jvx_data2String(level_dB,0);
			level_dB += 99;
			level_dB = JVX_MIN(level_dB, 99);
			level_dB = JVX_MAX(level_dB, 0);
			this->progressBar_out_right->setValue(level_dB);
			this->progressBar_out_right->setToolTip(txt.c_str());
			label_level_right->setText(txt.c_str());
		}

		isValid = false;
		if(systemParams.inSlider == 0)
		{
			res = theProps->get_property(&container, 2, JVX_DATAFORMAT_DATA, 14, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true, &isValid, NULL);
		}
		else
		{
			res = theProps->get_property(&container, 2, JVX_DATAFORMAT_DATA, 15, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true, &isValid, NULL);
		}
		if(res == JVX_NO_ERROR)
		{
			if(container[0] > JVX_STANDALONE_HOST_LEVEL_CLIP)
			{
				setBackgroundLabelColor(Qt::red, label_overload_out_left);
			}
			else
			{
				setBackgroundLabelColor(Qt::green, label_overload_out_left);
			}

			if(container[1] > JVX_STANDALONE_HOST_LEVEL_CLIP)
			{
				setBackgroundLabelColor(Qt::red, label_overload_out_right);
			}
			else
			{
				setBackgroundLabelColor(Qt::green, label_overload_out_right);
			}

			container[0] = 0.0;
			container[1] = 0.0;
			res = theProps->set_property(&container, 2, JVX_DATAFORMAT_DATA, 14, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);
			res = theProps->set_property(&container, 2, JVX_DATAFORMAT_DATA, 15, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);
		}
		this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
	}
}

void 
uMainWindow::timeout_rtPlot()
{
	this->updatePlotData_online();
	this->updatePlots();
}

void 
uMainWindow::newValue_level()
{
	IjvxProperties* theProps = NULL;
	IjvxObject* theObj = NULL;
	jvxData container[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];
	jvxBool isValid = false;
	std::string txt;
	jvxErrorType res = this->involvedComponents.theHost.hHost->request_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if((res == JVX_NO_ERROR) && theProps)
	{
		jvxData levelGain = this->verticalSlider_gain_out->value();
		levelGain -= 89;
		levelGain = pow(10.0, levelGain/20.0);
		if(systemParams.inSlider == 0)
		{
			this->systemParams.in.levelGain_user = levelGain;
			if(!systemParams.in.mute)
			{
				this->systemParams.in.levelGain_inuse = this->systemParams.in.levelGain_user;
			}
			container[0] = this->systemParams.in.levelGain_inuse;
			container[1] = this->systemParams.in.levelGain_inuse;

			res = theProps->set_property(&container, 2, JVX_DATAFORMAT_DATA, 16, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);
		}
		else
		{
			this->systemParams.out.levelGain_user = levelGain;
			if(!systemParams.out.mute)
			{
				this->systemParams.out.levelGain_inuse = this->systemParams.out.levelGain_user;
			}
			container[0] = this->systemParams.out.levelGain_inuse;
			container[1] = this->systemParams.out.levelGain_inuse;

			res = theProps->set_property(&container, 2, JVX_DATAFORMAT_DATA, 17, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);
		}
			
		this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
	}
	this->updateWindow();
}

void 
uMainWindow::toggled_loop()
{
	if(this->readwrite.theBuffers.loop())
	{
		this->readwrite.theBuffers.set_loop(false);
	}
	else
	{
		this->readwrite.theBuffers.set_loop(true);
	}
	this->updateWindow();
}

void 
uMainWindow::toggled_listen()
{
	if(this->systemParams.listen)
	{
		this->systemParams.listen = false;
	}
	else
	{
		this->systemParams.listen = true;
	}
	this->updateWindow();
}

void 
uMainWindow::toggled_mute()
{
	if(systemParams.inSlider == 0)
	{
		if(this->systemParams.in.mute)
		{
			this->systemParams.in.mute = false;
		}
		else
		{
			this->systemParams.in.mute = true;
		}
	}
	else
	{
		if(this->systemParams.out.mute)
		{
			this->systemParams.out.mute = false;
		}
		else
		{
			this->systemParams.out.mute = true;
		}
	}
	
	IjvxProperties* theProps = NULL;
	IjvxObject* theObj = NULL;
	jvxData container[JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS];
	jvxBool isValid = false;
	std::string txt;
	jvxErrorType res = this->involvedComponents.theHost.hHost->request_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if((res == JVX_NO_ERROR) && theProps)
	{
		if(systemParams.inSlider == 0)
		{
			if(this->systemParams.in.mute)
			{
				this->systemParams.in.levelGain_inuse = 0.0;
			}
			else
			{
				this->systemParams.in.levelGain_inuse = this->systemParams.in.levelGain_user;
			}

			container[0] = this->systemParams.in.levelGain_inuse;
			container[1] = this->systemParams.in.levelGain_inuse;

			res = theProps->set_property(&container, 2, JVX_DATAFORMAT_DATA, 16, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);
		}
		else
		{
			if(this->systemParams.in.mute)
			{
				this->systemParams.in.levelGain_inuse = 0.0;
			}
			else
			{
				this->systemParams.in.levelGain_inuse = this->systemParams.in.levelGain_user;
			}

			container[0] = this->systemParams.in.levelGain_inuse;
			container[1] = this->systemParams.in.levelGain_inuse;

			res = theProps->set_property(&container, 2, JVX_DATAFORMAT_DATA, 17, JVX_PROPERTY_CATEGORY_UNSPECIFIC, 0, true);
		}		
		this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
	}
	this->updateWindow();
}

void 
uMainWindow::newSelection_inoutslider()
{
	this->systemParams.inSlider = this->horizontalSlider->value();
	this->updateWindow();
}
