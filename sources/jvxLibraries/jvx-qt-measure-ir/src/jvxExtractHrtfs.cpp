#include "jvxExtractHrtfs.h"
#include "QClipboard"
#include "jvx-qcp-qt-helpers.h"
#include <math.h>

#define JVX_LOG10_EPS 1e-10

jvxExtractHrtfs::jvxExtractHrtfs(QWidget* parent) : QWidget(parent)
{
	refMeasure = NULL;
}

jvxExtractHrtfs::~jvxExtractHrtfs()
{
}

JVX_FFT_TOOLS_DEFINE_FFT_SIZES

void
jvxExtractHrtfs::init(IjvxQtAcousticMeasurement* refMeasureArg)
{
	jvxSize i;

	refMeasure = refMeasureArg;

	this->setupUi(static_cast<QWidget*>(this));
	
	this->setWindowTitle("Acoustic Measument - extract hrtfs");

	if (refMeasure)
	{
		refMeasure->register_data_processor("Extract HRTFs",
			JVX_ACOUSTIC_MEASURE_TASK_PASS_TWO_MEASURED_IRS,
			static_cast<IjvxQtAcousticMeasurement_process*>(this), 0);
	}

	// Have 2 plots in one diagram
	qcp_display_eq_td_left->addGraph();
	qcp_display_eq_td_left->graph(0)->setVisible(true);

	qcp_display_eq_td_right->addGraph();
	qcp_display_eq_td_right->graph(0)->setVisible(true);

}

void
jvxExtractHrtfs::terminate()
{
	if (refMeasure)
	{
		refMeasure->unregister_data_processor(
			JVX_ACOUSTIC_MEASURE_TASK_EQUALIZE,
			static_cast<IjvxQtAcousticMeasurement_process*>(this), 0);
	}
	refMeasure = NULL;
}

jvxErrorType
jvxExtractHrtfs::process_data(jvxMeasurementDataProcessorTask task, jvxHandle* fld, jvxSize idTag)
{
	if (task != jvxMeasurementDataProcessorTask::JVX_ACOUSTIC_MEASURE_TASK_PASS_TWO_MEASURED_IRS)
	{
		return JVX_ERROR_UNSUPPORTED;
	}

	jvxMeasurementTaskPass2Ir* dataTransfer = (jvxMeasurementTaskPass2Ir*)fld;

	setData1.irMeasured.resize(dataTransfer->ir_data1_len);
	memcpy(setData1.irMeasured.data(), dataTransfer->ir_data1, sizeof(jvxData) * dataTransfer->ir_data1_len);
	setData1.nameChannel = dataTransfer->ir_data1_chan_name;
	setData1.nameMeasurement = dataTransfer->ir_data1_meas_name;
	setData1.rate = dataTransfer->ir_data1_rate;

	setData2.irMeasured.resize(dataTransfer->ir_data2_len);
	memcpy(setData2.irMeasured.data(), dataTransfer->ir_data2, sizeof(jvxData) * dataTransfer->ir_data2_len);
	setData2.nameChannel = dataTransfer->ir_data2_chan_name;
	setData2.nameMeasurement = dataTransfer->ir_data2_meas_name;
	setData2.rate = dataTransfer->ir_data2_rate;

	compute_auto();
	replot_hrtfs();
	update_window(); 

	return JVX_NO_ERROR;
}

QWidget* 
jvxExtractHrtfs::my_widget()
{
	return static_cast<QWidget*>(this);
}

void
jvxExtractHrtfs::update_window()
{
	jvxSize i;

	label_chan_ir_1->setText(setData1.nameChannel.c_str());
	label_meas_ir_1->setText(setData1.nameMeasurement.c_str());

	label_chan_ir_2->setText(setData2.nameChannel.c_str());
	label_meas_ir_2->setText(setData2.nameMeasurement.c_str());

	checkBox_autoExtract->setChecked(this->autoComputeStart);
	lineEdit_srate->setText(jvx_size2String(setData1.rate).c_str());
	lineEdit_l_ir->setText(jvx_size2String(setData1.irMeasured.size()).c_str());
	lineEdit_l_hrtfs->setText(jvx_size2String(this->lHrtfs).c_str());
	lineEdit_thres->setText(jvx_data2String(startThresholdHrtf, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
	lineEdit_displace->setText(jvx_size2String(this->showDisplacement).c_str());
	lineEdit_showoffset->setText(jvx_size2String(this->offsetHrtfs).c_str());
	if (autoComputeStart)
	{
		lineEdit_showoffset->setEnabled(false);
	}
	else
	{
		lineEdit_showoffset->setEnabled(true);
	}

	lineEdit_copy->setText(hrtfBufShow.c_str());
}

void
jvxExtractHrtfs::compute_auto()
{
	jvxSize i;
	jvxData maxAbs = 0;
	jvxSize idxStart1 = 0;
	jvxSize idxStart2 = 0;
	if (autoComputeStart)
	{
		maxAbs = 0;
		for (i = 0; i < setData1.irMeasured.size(); i++)
		{
			if (abs(setData1.irMeasured[i]) > maxAbs)
			{
				maxAbs = abs(setData1.irMeasured[i]);
			}
		}

		jvxData thres = maxAbs * startThresholdHrtf;
		for (i = 0; i < setData1.irMeasured.size(); i++)
		{
			if (abs(setData1.irMeasured[i]) > thres)
			{
				idxStart1 = i;
				break;
			}
		}

		maxAbs = 0;
		for (i = 0; i < setData2.irMeasured.size(); i++)
		{
			if (abs(setData2.irMeasured[i]) > maxAbs)
			{
				maxAbs = abs(setData2.irMeasured[i]);
			}
		}

		thres = maxAbs * startThresholdHrtf;
		for (i = 0; i < setData2.irMeasured.size(); i++)
		{
			if (abs(setData2.irMeasured[i]) > thres)
			{
				idxStart2 = i;
				break;
			}
		}
		offsetHrtfs = JVX_MIN(idxStart1, idxStart2);
	}

	offsetHrtfs = JVX_MAX(0, ((int)offsetHrtfs - showDisplacement));	
}

void
jvxExtractHrtfs::replot_hrtfs()
{
	std::vector<std::vector<jvxData> > vecs;
	std::vector<jvxData> vecleft;
	vecleft.resize(lHrtfs);
	std::vector<jvxData> vecright;
	vecright.resize(lHrtfs);

	leftDataY.resize(lHrtfs);
	rightDataY.resize(lHrtfs);
	leftRightDataX.resize(lHrtfs);
	jvxData xlimmin = offsetHrtfs;
	jvxData xlimmax = offsetHrtfs + lHrtfs;
	jvxData ylimminl = +mat_epsMax;
	jvxData ylimmaxl = -mat_epsMax;
	jvxData ylimminr = +mat_epsMax;
	jvxData ylimmaxr = -mat_epsMax;

	jvxSize copyMin1 = JVX_MIN(lHrtfs, setData1.irMeasured.size() - offsetHrtfs);
	jvxSize copyMin2 = JVX_MIN(lHrtfs, setData2.irMeasured.size() - offsetHrtfs);

	switch (viewOrder)
	{
	case jvxIrOrderViewHrtf::JVX_IR_VIEW_LEFT_FIRST:
		memcpy(leftDataY.data(), setData1.irMeasured.data() + offsetHrtfs, sizeof(jvxData) * copyMin1);
		memcpy(vecleft.data(), setData1.irMeasured.data() + offsetHrtfs, sizeof(jvxData) * copyMin1);
		memcpy(rightDataY.data(), setData2.irMeasured.data() + offsetHrtfs, sizeof(jvxData) * copyMin2);
		memcpy(vecright.data(), setData2.irMeasured.data() + offsetHrtfs, sizeof(jvxData) * copyMin2);
		break;
	default:
		memcpy(leftDataY.data(), setData2.irMeasured.data() + offsetHrtfs, sizeof(jvxData) * copyMin2);
		memcpy(vecleft.data(), setData2.irMeasured.data() + offsetHrtfs, sizeof(jvxData) * copyMin2);
		memcpy(rightDataY.data(), setData1.irMeasured.data() + offsetHrtfs, sizeof(jvxData) * copyMin1);
		memcpy(vecright.data(), setData1.irMeasured.data() + offsetHrtfs, sizeof(jvxData) * copyMin1);
		break;
	}

	for (int i = 0; i < leftRightDataX.size(); i++)
	{
		leftRightDataX[i] = (offsetHrtfs + i);
		if (leftDataY[i] < ylimminl)
		{
			ylimminl = leftDataY[i];
		}
		if (rightDataY[i] < ylimminr)
		{
			ylimminr = rightDataY[i];
		}
		if (leftDataY[i] > ylimmaxl)
		{
			ylimmaxl = leftDataY[i];
		}
		if (rightDataY[i] > ylimmaxr)
		{
			ylimmaxr = rightDataY[i];
		}
	}

	qcp_display_eq_td_left->graph(0)->setData(leftRightDataX, leftDataY, true);
	qcp_display_eq_td_left->xAxis->setRange(xlimmin, xlimmax);
	qcp_display_eq_td_left->yAxis->setRange(ylimminl, ylimmaxl);
	qcp_display_eq_td_left->replot();

	qcp_display_eq_td_right->graph(0)->setData(leftRightDataX, rightDataY, true);
	qcp_display_eq_td_right->xAxis->setRange(xlimmin, xlimmax);
	qcp_display_eq_td_right->yAxis->setRange(ylimminr, ylimmaxr);
	qcp_display_eq_td_right->replot();

	vecs.push_back(vecleft);
	vecs.push_back(vecright);
	hrtfBufShow = jvx_data2NumericExpressionString(vecs, JVX_DATA_2STRING_CONST_FORMAT_G);
}

void
jvxExtractHrtfs::update_parameter_show()
{
	compute_auto();
	replot_hrtfs();
	update_window();
}

void
jvxExtractHrtfs::toggle_autoextract(bool tog)
{
	autoComputeStart = tog;
	update_parameter_show();
}

void
jvxExtractHrtfs::changed_displacement()
{
	int newVal = lineEdit_displace->text().toInt();
	this->showDisplacement = newVal;

	this->update_parameter_show();
}

void 
jvxExtractHrtfs::change_l_hrtf()
{
	lHrtfs = lineEdit_l_hrtfs->text().toInt();
	this->update_parameter_show();
}

void 
jvxExtractHrtfs::change_show_offset()
{
	offsetHrtfs = lineEdit_showoffset->text().toInt();
	this->update_parameter_show();
}

void
jvxExtractHrtfs::change_threshold_offset()
{
	startThresholdHrtf = lineEdit_thres->text().toInt();
	this->update_parameter_show();
}

void 
jvxExtractHrtfs::trigger_clipboard_ir()
{
	QClipboard* clipboard = QGuiApplication::clipboard();
	clipboard->setText(hrtfBufShow.c_str());
}



