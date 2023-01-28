#define JVX_LOCAL_ASSERT_PROPERTIES_LEAVE true

#include "CjvxQtMixMatrix.h"
#include <iostream>
#include <qpainter.h>
#include "QWheelEvent"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#define EPS_MIN_GAIN 1e-4
#define EPS_MIN_LEVEL 1e-10

// ========================================================================

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtMixMatrixWidget, jvxQtMixMatrix, CjvxQtMixMatrix)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtMixMatrixWidget, jvxQtMixMatrix)

/*
jvxErrorType init_jvxQtMixMatrix(IjvxQtSpecificWidget** returnWidget, QWidget* parent)
{
	if(returnWidget)
	{
		*returnWidget = new CjvxQtMixMatrix(parent);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType terminate_jvxQtMixMatrix(IjvxQtSpecificWidget* returnWidget)
{
	if(returnWidget)
	{
		delete(returnWidget);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
*/
// ====================================================================================

CjvxQtMixMatrix::CjvxQtMixMatrix(QWidget* parent): QWidget(parent)
{
	config.selection_x = JVX_SIZE_UNSELECTED;
	config.selection_y = JVX_SIZE_UNSELECTED;
	buttonPressed = false;
	stereoLockModeOn = false;
	propRef = NULL;
}

CjvxQtMixMatrix::~CjvxQtMixMatrix()
{
}

void
CjvxQtMixMatrix::init(IjvxHost* theHostRef, jvxCBitField mode , jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd)
{
	this->setupUi(this);
	theHost = theHostRef;

	myTableWidget->clear();
	myTableWidget->setRowCount(0);
	myTableWidget->setColumnCount(0);

	theHeaderH = new myHeaderViewH(JVX_QTMIXMATRIX_FIELDWIDTH, JVX_QTMIXMATRIX_FIELDWIDTH_ROTATED_TEXT);
	myTableWidget->setHorizontalHeader(theHeaderH);

	theHeaderV = new myHeaderViewV(JVX_QTMIXMATRIX_FIELDWIDTH, JVX_QTMIXMATRIX_FIELDWIDTH_ROTATED_TEXT);
	myTableWidget->setVerticalHeader(theHeaderV);

	this->setMinimumHeight(JVX_QTMIXMATRIX_WIDGET_HEIGHT);
	this->setMaximumHeight(JVX_QTMIXMATRIX_WIDGET_HEIGHT);
	this->setMinimumWidth(JVX_QTMIXMATRIX_WIDGET_WIDTH);
	this->setMaximumWidth(JVX_QTMIXMATRIX_WIDGET_WIDTH);

	myTableWidget->viewport()->setMouseTracking(true);
	myTableWidget->viewport()->installEventFilter(this);
}

void
CjvxQtMixMatrix::getMyQWidget(QWidget**retWidget, jvxSize id)
{
	if (retWidget)
	{
		if (id == 0)
		{
			*retWidget = static_cast<QWidget*>(this);
		}
		else
		{
			*retWidget = NULL;
		}
	}
}

void
CjvxQtMixMatrix::terminate()
{
	theHost = NULL;
}

jvxErrorType
CjvxQtMixMatrix::addPropertyReference(IjvxAccessProperties* propRefIn, const std::string& prefixArg, const std::string& identArg)
{
	std::string propDescr;
	jvxData valD;
	if (propRef == NULL)
	{
		thePrefix = prefixArg;
		propRef = propRefIn;
		jvxCallManagerProperties callGate;
		if (propRef)
		{
			// Get data to indicate if data shall be taken from the component
			propDescr = jvx_makePathExpr(thePrefix, "/zero_threshold");
			valD = EPS_MIN_GAIN;
			JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef, valD, JVX_DATAFORMAT_DATA, propDescr.c_str(), callGate);
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
CjvxQtMixMatrix::setNamesColorsChannels(QStringList& lstIn, QStringList& lstOut, QList<QColor>& colorsIn, QList<QColor>& colorsOut)
{
	jvxSize i, j;
	valueTuple the1;
	valueTuple the0;
	config.lst_input = lstIn;
	config.lst_output = lstOut;
	config.colors_input = colorsIn;
	config.colors_output = colorsOut;

	the1.gain = 1;
	the1.level = 0;
	the0.gain = 0;
	the0.level = 0;

	// Reset the table widget
	myTableWidget->clear();

	myTableWidget->setRowCount(config.lst_input.size());
	myTableWidget->setColumnCount(config.lst_output.size());

	theHeaderV->setColors(colorsIn);
	theHeaderV->setTexts(lstIn);

	theHeaderH->setColors(colorsOut);
	theHeaderH->setTexts(lstOut);

	std::vector<std::string> lstInStd = jvx_QStringList_to_std(lstOut);
	std::vector<std::string> lstOutStd = jvx_QStringList_to_std(lstIn);
	theContent.resize(lstInStd, lstOutStd, the1, the0);

	for (i = 0; i < theContent.dimX(); i++)
	{
		for (j = 0; j < theContent.dimY(); j++)
		{
			QTableWidgetItem* theW = new QTableWidgetItem(0);
			theW->setText("##");
			myTableWidget->setItem(j, i , theW);
		}
	}
	if (config.selection_x >= theContent.dimX())
	{
		config.selection_x = theContent.dimX() - 1;
	}
	if (config.selection_y >= theContent.dimY())
	{
		config.selection_y = theContent.dimX() - 1;
	}
}

void
CjvxQtMixMatrix::activate()
{
}

void
CjvxQtMixMatrix::deactivate()
{
}

jvxErrorType
CjvxQtMixMatrix::removePropertyReference(IjvxAccessProperties* propRefIn)
{
	if (propRef)
	{
		propRef = NULL;
		thePrefix = "";
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
CjvxQtMixMatrix::processing_started()
{
	seqRunning = true;
}

void
CjvxQtMixMatrix::processing_stopped()
{
	seqRunning = false;
}

void
CjvxQtMixMatrix::update_window(jvxCBitField prio )
{
	jvxSelectionList selList;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isMeasureIr = false;
	jvxData valD = 0;
	jvxInt32 valI32 = 0;
	//jvxStringList myStrLst;
	jvxSize i,j;
	jvxSize selId = JVX_SIZE_UNSELECTED;
	std::string propDescr;
	jvxCBool requiresPull = c_false;
	std::string str;
	jvxCallManagerProperties callGate;
	if(propRef)
	{
		// Get data to indicate if data shall be taken from the component
		propDescr = jvx_makePathExpr(thePrefix, "/config_data_available");

		JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE(propRef, requiresPull, JVX_DATAFORMAT_BOOL, propDescr.c_str(), callGate);
		if (requiresPull)
		{
			transferMatrixNode(false);
			requiresPull = c_false;
			JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef, requiresPull, JVX_DATAFORMAT_BOOL, propDescr.c_str(), callGate);
		}

		propDescr = jvx_makePathExpr(thePrefix, "/main_out");
		JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE(propRef, valD, JVX_DATAFORMAT_DATA, propDescr.c_str(), callGate);
		valD = 20 * log10(valD + EPS_MIN_GAIN);
		valI32 = JVX_DATA2INT32(valD);
		valI32 += JVX_OFFSET_DB_GAIN;
		valI32 = JVX_MAX(valI32, 0);
		valI32 = JVX_MIN(valI32, 100);
		str = jvx_data2String(valD, 1) + " dB";
		horizontalSlider_mainout->setValue(valI32);
		label_mainout_dB->setText(str.c_str());

	}

	if (stereoLockModeOn)
	{
		horizontalSlider_lockstereo->setValue(1);
	}
	else
	{
		horizontalSlider_lockstereo->setValue(0);
	}
	drawContentMatrix();

	setBackgroundLabelColor(Qt::green, label_clipout);
	progressBar_levelin->setValue(0);
	label_in_db->setText("--");
	progressBar_levelout->setValue(0);
	label_out_db->setText("--");


	transferMatrixNode(true);
}

jvxErrorType 
CjvxQtMixMatrix::request_sub_interface(jvxQtInterfaceType, jvxHandle**)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxQtMixMatrix::return_sub_interface(jvxQtInterfaceType, jvxHandle*)
{
	return JVX_ERROR_UNSUPPORTED;
}

void
CjvxQtMixMatrix::update_window_periodic()
{
	std::string propDescr;
	jvxData valD;
	std::string str;
	jvxSize numIdx;
	jvxCallManagerProperties callGate;
	if (propRef)
	{
		setBackgroundLabelColor(Qt::green, label_clipout);
		if ((config.selection_x < theContent.dimX()) && (config.selection_y < theContent.dimY()))
		{
			numIdx = config.selection_x;
			propDescr = jvx_makePathExpr(thePrefix, "/idx_output");
			JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef,
				numIdx, JVX_DATAFORMAT_SIZE, propDescr.c_str(), callGate);

			numIdx = config.selection_y;
			propDescr = jvx_makePathExpr(thePrefix, "/idx_input");
			JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef,
				numIdx, JVX_DATAFORMAT_SIZE, propDescr.c_str(), callGate);

			// Get data to indicate if data shall be taken from the component
			propDescr = jvx_makePathExpr(thePrefix, "/level_out");
			JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE(propRef, valD, JVX_DATAFORMAT_DATA, propDescr.c_str(), callGate);
			valD = 10 * log10(valD + EPS_MIN_LEVEL);
			str = jvx_data2String(valD, 1) + " dB";
			valD += 100;
			valD = JVX_MAX(0, valD);
			valD = JVX_MIN(100, valD);
			progressBar_levelout->setValue(valD);
			label_out_db->setText(str.c_str());
			propDescr = jvx_makePathExpr(thePrefix, "/level_out_max");
			JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE(propRef, valD, JVX_DATAFORMAT_DATA, propDescr.c_str(), callGate);
			if (valD > CLIP_THRESHOLD)
			{
				setBackgroundLabelColor(Qt::red, label_clipout);
			}
			valD = 0;
			JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef, valD, JVX_DATAFORMAT_DATA, propDescr.c_str(), callGate);

			propDescr = jvx_makePathExpr(thePrefix, "/level_in");
			JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE(propRef, valD, JVX_DATAFORMAT_DATA, propDescr.c_str(), callGate);
			valD = 10 * log10(valD + EPS_MIN_LEVEL);
			str = jvx_data2String(valD, 1) + " dB";
			valD += 100;
			valD = JVX_MAX(0, valD);
			valD = JVX_MIN(100, valD);
			progressBar_levelin->setValue(valD);
			label_in_db->setText(str.c_str());
		}
	}
}

void
CjvxQtMixMatrix::drawContentMatrix()
{
	jvxSize i, j;
	jvxData val, val2;
	int valI, valI2;
	std::string str, str2;

	for (i = 0; i < myTableWidget->columnCount(); i++)
	{
		for (j = 0; j < myTableWidget->rowCount(); j++)
		{
			str = "--";
			QTableWidgetItem* theW = myTableWidget->item(j, i);
			assert(theW);
			if (
				(i < theContent.dimX()) &&
				(j < theContent.dimY()))
			{
				val = theContent.value(i, j).gain;
				if (val > EPS_MIN_GAIN)
				{
					val = 20 * log10(val);
					str = jvx_data2String(val, 1);
				}
				else
				{
					str = "off";
				}
			}
			if (
				(i == config.selection_x) &&
				(j == config.selection_y))
			{
				theW->setBackground(QBrush(JVX_COLOR_MATRIX_BGRD_SELECTED));
				theW->setForeground(QBrush(JVX_COLOR_MATRIX_TEXT_SELECTED));
			}
			else
			{
				theW->setBackground(QBrush(JVX_COLOR_MATRIX_BGRD_UNSELECTED));
				theW->setForeground(QBrush(JVX_COLOR_MATRIX_TEXT_UNSELECTED));
			}

			theW->setText(str.c_str());
		}
	}
	str = "--";
	str2 = "--";
	valI = 0;
	valI2 = 0;
	if ((config.selection_x < theContent.dimX()) && (config.selection_y < theContent.dimY()))
	{
		val = theContent.value(config.selection_x, config.selection_y).gain;
		val = 20 * log10(val + EPS_MIN_GAIN);
		valI = JVX_DATA2INT32(val);
		valI += JVX_OFFSET_DB_GAIN;
		valI = JVX_MAX(valI, 0);
		valI = JVX_MIN(valI, 100);
		str = jvx_data2String(val, 1) + " dB";
	}

	horizontalSlider_gain->setValue(valI);
	label_gain_dB->setText(str.c_str());
	lineEdit_gain->setText(str.c_str());
}

void
CjvxQtMixMatrix::clicked_Cell(int idy, int idx)
{
	config.selection_x = idx;
	config.selection_y = idy;
	drawContentMatrix();
}

void
CjvxQtMixMatrix::doubleClicked_Cell(int idy, int idx)
{
	jvxSize i;
	valueTuple theT;

	if((idx < theContent.dimX()) && (idy < theContent.dimY()))
	{
		theT = theContent.value(idx, idy);
		if(theT.gain <= EPS_MIN_GAIN)
		{
			theT.gain = 1.0;
		}
		else
		{
			theT.gain = EPS_MIN_GAIN;
		}
		theContent.setValue(idx, idy, theT);
		transferMatrixNode(true);
		transferMatrixNode(false);
		clicked_Cell(idy, idx);
	}
}

void
CjvxQtMixMatrix::released_Slider()
{
	int val = horizontalSlider_gain->value();
	val -= JVX_OFFSET_DB_GAIN;
	jvxData valD = pow(10.0, (jvxData)val / 20.0);
	if ((config.selection_x < theContent.dimX()) && (config.selection_y < theContent.dimY()))
	{
		valueTuple theT = theContent.value(config.selection_x, config.selection_y);
		theT.gain = valD;
		theContent.setValue(config.selection_x, config.selection_y, theT);
	}
	transferMatrixNode(true, config.selection_x, config.selection_y);
	transferMatrixNode(false, config.selection_x, config.selection_y);
	drawContentMatrix();
}


void
CjvxQtMixMatrix::released_Slider_lockStereo()
{
	int val = horizontalSlider_lockstereo->value();
	stereoLockModeOn = (val != 0);
	update_window();
}

void
CjvxQtMixMatrix::release_Slider_mainout()
{
	jvxCallManagerProperties callGate;
	int val = horizontalSlider_mainout->value();
	val -= JVX_OFFSET_DB_GAIN;
	main_out = pow(10.0, (jvxData)val / 20.0);
	std::string propDescr = jvx_makePathExpr(thePrefix, "/main_out");
	JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef,
		main_out, JVX_DATAFORMAT_DATA, propDescr.c_str(), callGate);
	update_window();
}

void
CjvxQtMixMatrix::editing_Finished_gain()
{
	QString txt = lineEdit_gain->text();
	jvxData val = txt.toData();
	jvxData valD = pow(10.0, (jvxData)val / 20.0);
	if ((config.selection_x < theContent.dimX()) && (config.selection_y < theContent.dimY()))
	{
		valueTuple theT = theContent.value(config.selection_x, config.selection_y);
		theT.gain = valD;
		theContent.setValue(config.selection_x, config.selection_y, theT);
	}
	transferMatrixNode(true, config.selection_x, config.selection_y);
	transferMatrixNode(false, config.selection_x, config.selection_y);
	drawContentMatrix();
}

void
CjvxQtMixMatrix::button_Pushed_alloff()
{
	jvxSize i, j;
	for (i = 0; i < theContent.dimX(); i++)
	{
		for (j = 0; j < theContent.dimY(); j++)
		{
			valueTuple theT = theContent.value(i, j);
			theT.gain = 0;
			theContent.setValue(i, j, theT);
		}
	}

	transferMatrixNode(true, -1, -1);
	transferMatrixNode(false, -1, -1);
	drawContentMatrix();
}

void
CjvxQtMixMatrix::clicked_expandOut()
{
	jvxSize i;
	jvxSize idxY = config.selection_y;
	jvxSize idxX = config.selection_x;
	valueTuple vv;
	jvxData val1 = -1, val2 = -1;
	if (JVX_CHECK_SIZE_SELECTED(idxX) && JVX_CHECK_SIZE_SELECTED(idxY))
	{
		if (stereoLockModeOn)
		{
			if (idxX % 2)
			{
				if ((idxX - 1 < theContent.dimX()) && (idxY < theContent.dimY()))
				{
					vv = theContent.value(idxX - 1, idxY);
					val1 = vv.gain;
				}
				if ((idxX < theContent.dimX()) && (idxY < theContent.dimY()))
				{
					vv = theContent.value(idxX, idxY);
					val2 = vv.gain;
				}
			}
			else
			{
				if ((idxX < theContent.dimX()) && (idxY < theContent.dimY()))
				{
					vv = theContent.value(idxX, idxY);
					val1 = vv.gain;
				}
				if ((idxX + 1 < theContent.dimX()) && (idxY < theContent.dimY()))
				{
					vv = theContent.value(idxX + 1, idxY);
					val2 = vv.gain;
				}
			}
			for (i = 0; i < theContent.dimX(); i++)
			{
				if (i % 2)
				{
					vv = theContent.value(i, idxY);
					if (val2 >= 0)
					{
						vv.gain = val2;
					}
					theContent.setValue(i, idxY, vv);
				}
				else
				{
					vv = theContent.value(i, idxY);
					if (val1 >= 0)
					{
						vv.gain = val1;
					}
					theContent.setValue(i, idxY, vv);
				}
			}
		} // if (stereoLockModeOn)
		else
		{
			if ((idxX < theContent.dimX()) && (idxY < theContent.dimY()))
			{
				vv = theContent.value(idxX, idxY);
				val1 = vv.gain;
			}
			for (i = 0; i < theContent.dimX(); i++)
			{
				vv = theContent.value(i, idxY);
				if (val1 >= 0)
				{
					vv.gain = val1;
				}
				theContent.setValue(i, idxY, vv);
			}
		}
	}
	transferMatrixNode(true, -1, -1);
	transferMatrixNode(false, -1, -1);
	drawContentMatrix();
}

void
CjvxQtMixMatrix::clicked_expandIn()
{
	jvxSize i;
	jvxSize idxY = config.selection_y;
	jvxSize idxX = config.selection_x;
	valueTuple vv;
	jvxData val1 = -1, val2 = -1;
	if (JVX_CHECK_SIZE_SELECTED(idxX) && JVX_CHECK_SIZE_SELECTED(idxY))
	{
		if (stereoLockModeOn)
		{
			if (idxY % 2)
			{
				if ((idxX < theContent.dimX()) && (idxY-1 < theContent.dimY()))
				{
					vv = theContent.value(idxX, idxY-1);
					val1 = vv.gain;
				}
				if ((idxX < theContent.dimX()) && (idxY < theContent.dimY()))
				{
					vv = theContent.value(idxX, idxY);
					val2 = vv.gain;
				}
			}
			else
			{
				if ((idxX < theContent.dimX()) && (idxY < theContent.dimY()))
				{
					vv = theContent.value(idxX, idxY);
					val1 = vv.gain;
				}
				if ((idxX  < theContent.dimX()) && (idxY+1 < theContent.dimY()))
				{
					vv = theContent.value(idxX, idxY+1);
					val2 = vv.gain;
				}
			}
			for (i = 0; i < theContent.dimY(); i++)
			{
				if (i % 2)
				{
					vv = theContent.value(idxX, i);
					if (val2 >= 0)
					{
						vv.gain = val2;
					}
					theContent.setValue(idxX, i, vv);
				}
				else
				{
					vv = theContent.value(idxX, i);
					if (val1 >= 0)
					{
						vv.gain = val1;
					}
					theContent.setValue(idxX, i, vv);
				}
			}
		} // if (stereoLockModeOn)
		else
		{
			if ((idxX < theContent.dimX()) && (idxY < theContent.dimY()))
			{
				vv = theContent.value(idxX, idxY);
				val1 = vv.gain;
			}
			for (i = 0; i < theContent.dimY(); i++)
			{
				vv = theContent.value(idxX, i);
				if (val1 >= 0)
				{
					vv.gain = val1;
				}
				theContent.setValue(idxX, i, vv);
			}
		}
	}
	transferMatrixNode(true, -1, -1);
	transferMatrixNode(false, -1, -1);
	drawContentMatrix();
}

bool
CjvxQtMixMatrix::eventFilter(QObject *object, QEvent *event)
{
	jvxData factor;
	if (event->type() == QEvent::MouseButtonPress)
	{

		if (object == myTableWidget->viewport())
		{
			buttonPressed = true;
		}
		return QObject::eventFilter(object, event);

	}
	else if (event->type() == QEvent::MouseButtonRelease)
	{
		if (object == myTableWidget->viewport())
		{
			buttonPressed = false;
		}
		return QObject::eventFilter(object, event);

	}
	else if (event->type() == QEvent::Wheel)
	{
		if (object == myTableWidget->viewport())
		{
			if (buttonPressed)
			{
				QWheelEvent* wev = static_cast<QWheelEvent*>(event);
				QPoint delta;
				jvxData steps;
#ifdef JVX_OS_WINDOWS
				if (wev->source() == Qt::MouseEventSynthesizedBySystem)
				{
					delta = wev->pixelDelta();
					steps = delta.ry();
				}
				else
				{
#endif

					delta = wev->angleDelta();
					steps = delta.ry() / 120;
#ifdef JVX_OS_WINDOWS
				}
#endif
				factor = pow(10.0, steps / 20);

				modifyLevelIncDec(factor, config.selection_x, config.selection_y);
				drawContentMatrix();
				return(true);
			}
		}
	}

	return QObject::eventFilter(object, event);
}

void
CjvxQtMixMatrix::modifyLevelIncDec(jvxData fac, jvxSize selx, jvxSize sely)
{
	jvxSize j;
	if (stereoLockModeOn)
	{
		jvxSize idxlow = 0;
		jvxSize idxhigh = 0;
		if (selx % 2)
		{
			idxlow = selx - 1;
			idxhigh = selx;
		}
		else
		{
			idxlow = selx;
			idxhigh = selx + 1;
		}
		if (idxlow < theContent.dimX())
		{
			for (j = 0; j < theContent.dimY(); j++)
			{
				valueTuple theT = theContent.value(idxlow, j);
				jvxData val = theT.gain;
				if (val > EPS_MIN_GAIN)
				{
					val *= fac;
				}
				theT.gain = val;
				theContent.setValue(idxlow, j, theT);
				transferMatrixNode(true, idxlow, j);
				transferMatrixNode(false, idxlow, j);
			}
		}
		if (idxhigh < theContent.dimX())
		{
			for (j = 0; j < theContent.dimY(); j++)
			{
				valueTuple theT = theContent.value(idxhigh, j);
				jvxData val = theT.gain;
				if (val > EPS_MIN_GAIN)
				{
					val *= fac;
				}
				theT.gain = val;
				theContent.setValue(idxhigh, j, theT);
				transferMatrixNode(true, idxhigh, j);
				transferMatrixNode(false, idxhigh, j);
			}
		}
	}
	else
	{
		if ((selx < theContent.dimX()) && (sely < theContent.dimY()))
		{
			valueTuple theT = theContent.value(config.selection_x, config.selection_y);
			jvxData val = theT.gain;
			if (val > EPS_MIN_GAIN)
			{
				val *= fac;
			}
			theT.gain = val;
			theContent.setValue(selx, sely, theT);
			transferMatrixNode(true, config.selection_x, config.selection_y);
			transferMatrixNode(false, config.selection_x, config.selection_y);
		}
	}
}


void
CjvxQtMixMatrix::transferMatrixNode(jvxBool toNode, jvxSize idx, jvxSize idy)
{
	jvxSize numIdx = 0;
	jvxSize i, j;
	std::string propDescr;
	jvxCallManagerProperties callGate;
	if (propRef)
	{
#ifdef JVX_CHECK_CONNECTIONS
		propDescr = jvx_makePathExpr( thePrefix, "/number_inputs");
		JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE(theTriples[JVX_COMPONENT_AUDIO_NODE].theProps,
			numIdx, JVX_DATAFORMAT_SIZE, propDescr.c_str());
		assert(numIdx == theContent.dimY());

		propDescr = jvx_makePathExpr(thePrefix, "/number_outputs");
		JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE(propRef,
			numIdx, JVX_DATAFORMAT_SIZE, propDescr.c_str());
		assert(numIdx == theContent.dimX());
#endif

		for (i = 0; i < theContent.dimX(); i++)
		{
			if (JVX_CHECK_SIZE_UNSELECTED(idx) || (i == idx))
			{
				numIdx = i;
				propDescr = jvx_makePathExpr(thePrefix, "/idx_output");
				JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef,
					numIdx, JVX_DATAFORMAT_SIZE, propDescr.c_str(), callGate);

				for (j = 0; j < theContent.dimY(); j++)
				{
					if (JVX_CHECK_SIZE_UNSELECTED(idy) || (j == idy))
					{
						numIdx = j;
						propDescr = jvx_makePathExpr(thePrefix, "/idx_input");
						JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef,
							numIdx, JVX_DATAFORMAT_SIZE, propDescr.c_str(), callGate);

						propDescr = jvx_makePathExpr(thePrefix, "/intersect_gain");
						valueTuple valT = theContent.value(i, j);
						if (toNode)
						{
							JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef,
								valT.gain, JVX_DATAFORMAT_DATA, propDescr.c_str(), callGate);
						}
						else
						{
							JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE(propRef,
								valT.gain, JVX_DATAFORMAT_DATA, propDescr.c_str(), callGate);
							theContent.setValue(i, j, valT);
						}
					}
				}
			}
		}
	}
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
