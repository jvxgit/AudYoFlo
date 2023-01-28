#include "myCentralWidget.h"
#include "jvxVertLabel.h"
#include <QTableWidget>
#include "jvx-helpers-product.h"
#include "typedefs/jvxTools/TjvxGenericWrapperApi.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif



void
myCentralWidget::init_submodule(IjvxHost* theHost)
{
	this->setupUi(this);
	mainCentral_host::init_submodule(theHost);

	currentlyShown.inChannels = 0;
	currentlyShown.outChannels = 0;

	myWidgetMixMatrix = NULL;
	theQWidget = NULL;
	init_jvxQtMixMatrix(&myWidgetMixMatrix, static_cast<QWidget*>(this));
	myWidgetMixMatrix->init(theHost);
	myWidgetMixMatrix->getMyQWidget(&theQWidget);

	QGridLayout* myLayout = static_cast<QGridLayout*>(theMainFrame->layout());
	assert(myLayout == NULL);
	myLayout = new QGridLayout();
	myLayout->addWidget(theQWidget);
	theMainFrame->setLayout(myLayout);
}

void
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
	mainCentral_host::inform_active(tp, propRef);
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:

		thePropRef_algo = propRef;
		myWidgetMixMatrix->addPropertyReference(thePropRef_algo, "/CjvxAuNMixMatrix/mixmatrix");
		break;

	case JVX_COMPONENT_AUDIO_DEVICE:
		thePropRef_dev = propRef;
		break;
	default:
		break;
	}
}


void
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		myWidgetMixMatrix->removePropertyReference(thePropRef_algo);
		thePropRef_algo = NULL;
		break;

	case JVX_COMPONENT_AUDIO_DEVICE:
		thePropRef_dev = NULL;
		break;

	default:
		break;
	}
	mainCentral_host::inform_inactive(tp, propRef);
}

void
myCentralWidget::inform_update_window(jvxCBitField prio)
{
	std::string propDescr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 numInChannels = -1;
	jvxInt32 numOutChannels = -1;
	jvxCallManagerProperties callGate;
	mainCentral_host::inform_update_window(prio);

	if (thePropRef_algo)
	{
		// Do something
		// Get number of input channels
		// Get number of output channels
		propDescr = "/JVX_AUDIONODE_INPUT_CHANNELS";
		JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE((thePropRef_algo),
			numInChannels, JVX_DATAFORMAT_32BIT_LE, propDescr.c_str(), callGate);

		propDescr = "/JVX_AUDIONODE_OUTPUT_CHANNELS";
		JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE((thePropRef_algo),
			numOutChannels, JVX_DATAFORMAT_32BIT_LE, propDescr.c_str(), callGate);

		rebuildMixMatrix(numInChannels, numOutChannels);
	}
	myWidgetMixMatrix->update_window();
}

void
myCentralWidget::inform_update_window_periodic()
{
	mainCentral_host::inform_update_window_periodic();
	myWidgetMixMatrix->update_window_periodic();
}

void
myCentralWidget::rebuildMixMatrix(jvxInt32 numInChannels, jvxInt32 numOutChannels)
{
	jvxSize i;
	std::string propDescr;
	QStringList lstIn;
	QStringList lstOut;

	QList<QColor> colorsIn;
	QList<QColor> colorsOut;
	QColor col;

	jvxSelectionList selList;
	int cnt = 0;
	jvxAssignedFileTags tags;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;

	if (thePropRef_dev)
	{
		propDescr = "/system/sel_input_channels";
		JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE_FULL((thePropRef_dev),
			selList, JVX_DATAFORMAT_SELECTION_LIST, propDescr.c_str(), callGate);
		for (i = 0; i < selList.strList.ll(); i++)
		{
			if (jvx_bitTest(selList.bitFieldSelected(), i))
			{
				std::string name = selList.strList.std_str_at(i);
				std::string nmChan;
				jvxGenericWrapperChannelType purp;
				int idInDevice;

				jvxErrorType res = jvx_genw_decodeChannelName(name, purp, nmChan, idInDevice);
				if (res != JVX_NO_ERROR)
				{
					nmChan = name;
					col = Qt::black;
				}
				else
				{
					switch (purp)
					{
					case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:
						jvx_initTagName(tags);
						jvx_tagExprToTagStruct(nmChan, tags, name);
						if (JVX_EVALUATE_BITFIELD(tags.tagsSet))
						{
							nmChan = jvx_tagStructToUserView(tags);
						}
						else
						{
							nmChan = name;
						}
						col = Qt::blue;
						break;
					case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:
						col = Qt::green;
						break;
					case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:
						col = Qt::black;
						break;
					case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:
						col = Qt::magenta;
						break;
					default:
						assert(0);
					}
				}
				nmChan += "|" + jvx_int2String(idInDevice);
				nmChan = jvx_shortenStringName(20, nmChan, 2);
				lstIn.append(nmChan.c_str());
				colorsIn.append(col);
				cnt++;
			}
		}
	}
	for (/*cnt */; cnt < numInChannels; cnt++)
	{
		lstIn.append(("unknown #" + jvx_int2String(cnt)).c_str());
	}

	cnt = 0;
	if (thePropRef_dev)
	{
		propDescr = "/system/sel_output_channels";
		JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE_FULL((thePropRef_dev),
			selList, JVX_DATAFORMAT_SELECTION_LIST, propDescr.c_str(), callGate);

			for (i = 0; i < selList.strList.ll(); i++)
			{
				if (jvx_bitTest(selList.bitFieldSelected(), i))
				{
					std::string name = selList.strList.std_str_at(i);
					std::string nmChan;


					jvxGenericWrapperChannelType purp;
					int idInDevice;
					col = Qt::black;
					jvxErrorType res = jvx_genw_decodeChannelName(name, purp, nmChan, idInDevice);
					if (res != JVX_NO_ERROR)
					{
						nmChan = name;
					}
					else
					{
						switch (purp)
						{
						case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:
							jvx_initTagName(tags);
							jvx_tagExprToTagStruct(nmChan, tags, name);
							if (JVX_EVALUATE_BITFIELD( tags.tagsSet))
							{
								nmChan = jvx_tagStructToUserView(tags);
							}
							else
							{
								nmChan = name;
							}
							col = Qt::blue;
							break;
						case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:
							col = Qt::green;
							break;
						case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:
							col = Qt::black;
							break;
						case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:
							col = Qt::magenta;
							break;
						default:
							assert(0);
						}
					}
					nmChan += "|" + jvx_int2String(idInDevice);
					nmChan = jvx_shortenStringName(20, nmChan,2);
					lstOut.append(nmChan.c_str());
					colorsOut.append(col);
					cnt++;
				}
			}
	}

	for (/*cnt*/; cnt < numOutChannels; cnt++)
	{
		lstOut.push_back(("unknown #" + jvx_int2String(cnt)).c_str());
	}

	myWidgetMixMatrix->setNamesColorsChannels(lstIn, lstOut, colorsIn, colorsOut);
}


#ifdef JVX_PROJECT_NAMESPACE
}
#endif
