#ifndef __REALTIMEVIEWER_HELPERS_H__
#define __REALTIMEVIEWER_HELPERS_H__

#include "jvx.h"

#include "common/CjvxRealtimeViewer.h"

#include "realtimeViewer_typedefs.h"

class QLineEdit_ext;
class QComboBox_ext;
class QSlider_ext;
class QwtPlotCurve;
class QCheckBox_ext;
class QComboBox;
class QFrame;

#if defined (QT_NAMESPACE)
namespace QT_NAMESPACE {
#endif

class QLabel;
class QPushButton;
class QCheckBox;

#if defined (QT_NAMESPACE)
}
#endif

class jvxRealtimeViewerPropertyDescriptor: public jvx::propertyDescriptor::CjvxPropertyDescriptorFull
{
public:
	std::string origin;
	std::string propPurpose;
	std::string objDescription;
	jvxComponentIdentification tp = JVX_COMPONENT_UNKNOWN;
	jvxAccessProtocol latest_read = JVX_ACCESS_PROTOCOL_OK;
	jvxAccessProtocol latest_write = JVX_ACCESS_PROTOCOL_OK;
	jvxSize inPropIdx = JVX_SIZE_UNSELECTED;
};

typedef struct
{
	jvxRealtimeViewerPropertyDescriptor descriptory;
	jvxRealtimeViewerPropertyDescriptor descriptorx;
	jvxSize color;
	jvxSize linestyle;
	jvxSize show;
	jvxSize width;
	jvxSize idPropX;
	QwtPlotCurve* thePlotCurve;
	jvxData* dataX;
	jvxData* dataY;
	jvxSize numValues;

	jvxData maxX;
	jvxData maxY;
	jvxData minX;
	jvxData minY;
	jvxBool minmaxValid;
} jvxRealtimeViewerPropertyPlotItem;

typedef struct
{
	jvxRealtimeViewerPropertyDescriptor descriptor;

	jvxSize length;
	jvxSize num_chans;
	jvxBitField selection;

	jvxSize numElmsCircBuffer;
	jvxSize szCircBuffer;
	jvxExternalBuffer* ptrCircBuffer;
	jvxBool isValid;
	QwtPlotCurve** thePlotCurves;
	jvxData* dataInc;
	jvxData** dataCpy;

	jvxSize lineWidth;
	jvxSize idChannelX;
	JVX_MUTEX_HANDLE* safeAccessHdl;
	jvxData maxX;
	jvxData maxY;
	jvxData minX;
	jvxData minY;
	jvxBool minmaxValid;
	jvxBool expectRescaleX;

} jvxRealtimeViewerPropertyCircPlotItem;

typedef struct
{
	jvxRealtimeViewerPropertyDescriptor descriptor;
	jvxData ymax;
	jvxData xmax;
	jvxData ymin;
	jvxData xmin;
	jvxInt16 autox;
	jvxInt16 autoy;
	std::string xaxistxt;
	std::string yaxistxt;
} jvxRealtimeViewerPropertyPlotGroup;

typedef struct
{
	jvxRealtimeViewerPropertyDescriptor descriptor;
	bool setup_complete;
	QLineEdit_ext* line_edit;
	QComboBox_ext* combo_box;
	QComboBox_ext* combo_box_access_rwcd;
	QComboBox_ext* combo_box_access_what;
	QComboBox* combo_box_config;
	QFrame* frame_tags;
	QSlider_ext* slider;
	QCheckBox_ext* checkbox_onoff;

#if defined (QT_NAMESPACE)
	std::vector<QT_NAMESPACE::QLabel*> label_flags;
	std::vector<QT_NAMESPACE::QLabel*> label_entries;
QT_NAMESPACE::QLabel* label_min;
	QT_NAMESPACE::QLabel* label_max;
	QT_NAMESPACE::QLabel* label_valid;
	QT_NAMESPACE::QPushButton* button_1;
	QT_NAMESPACE::QPushButton* button_2;
	QT_NAMESPACE::QPushButton* button_3;
	QT_NAMESPACE::QPushButton* button_4;
	//QT_NAMESPACE::QCheckBox* checkbox_1;
#else
	std::vector<QLabel*> label_flags;
	std::vector<QLabel*> label_entries;
	QLabel* label_min;
	QLabel* label_max;
	QLabel* label_valid;
	QPushButton* button_1;
	QPushButton* button_2;
	QPushButton* button_3;
	QPushButton* button_4;
	//QCheckBox* checkbox_1;
#endif
	jvxSize selection;
	jvxCBool validOnCreation;
	jvxContext ctxt;
	jvxSize sel_access;
} jvxRealtimeViewerPropertyItem;

typedef struct
{
	jvxInt16 scrollY;
	jvxInt16 scrollX;
} jvxRealtimeViewerPropertySection;

jvxErrorType static_putConfiguration(IjvxConfigProcessor* theReader, jvxConfigData* readEntriesFrom, jvxHandle* privateDataItem, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeId, jvxInt16 itemId);
jvxErrorType static_getConfiguration(IjvxConfigProcessor* theReader, jvxConfigData* writeEntriesTo, jvxHandle* privateDataItem, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeId, jvxInt16 itemId);

jvxErrorType static_initData(jvxHandle* privateData, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeHint);
jvxErrorType static_allocateData(jvxHandle** privateData, jvxSize* sz, jvxRealtimePrivateMemoryDecoderEnum decodeHint);
jvxErrorType static_copyData(jvxHandle* copyTo, jvxHandle* copyFrom, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeId);
jvxErrorType static_deallocateData(jvxHandle* removeMyContent, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeId);

#endif

