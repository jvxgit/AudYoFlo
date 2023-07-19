#include "realtimeViewerOneProperty.h"
#include "realtimeViewerProperties.h"

#include "jvx-helpers.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>

#include "uMainWindow_defines.h"
#include <cassert>

#include "realtimeViewer_widgets.h"
#include "realtimeViewer_helpers.h"

#define JVX_NUM_ENTRIES_FLAGTAG 16
//#define JVX_VERBOSE_OUTPUT

const char* entriesGroupL[16] =
{
	"L0",
	"L1",
	"L2",
	"L3",
	"L4",
	"L5",
	"L6",
	"L7",
	"L8",
	"L9",
	"L10",
	"L11",
	"L12",
	"L13",
	"ACCESS",
	"CONFIG"
};

const char* entriesGroupC[4] =
{
	"FULL", "OVERLAY", "ACCESS", "CONFIG"
};

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

// ============================================================================================
// ============================================================================================

realtimeViewerOneProperty::realtimeViewerOneProperty(realtimeViewerProperties* parent,
						     IjvxHost* hostRef,
						     CjvxRealtimeViewer* rtvRef,
						     IjvxReport* report,
						     jvxSize sectionId,
						     jvxSize groupId,
						     jvxSize itemId):
	QWidget(parent), realtimeViewer_base("realtimeViewerOneProperty", hostRef, rtvRef, report)
{
	local.parentRef = parent;

	references.sectionId = sectionId;
	references.groupId = groupId;
	references.itemId = itemId;

	// Set reference for low level function calls
	this->setProperty("BASE_REALTIMEVIEWER", QVariant::fromValue(reinterpret_cast<void*>(static_cast<realtimeViewer_base*>(this))));
};

realtimeViewerOneProperty::~realtimeViewerOneProperty()
{
	cleanBeforeDelete();
	cleanContent();
}

void
realtimeViewerOneProperty::init()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool requiresRedraw = false;
	setupUi(this);

	itemProps.tpV.reset(JVX_COMPONENT_UNKNOWN);
	itemProps.inPropId = 0;

	if(references.theRef)
	{
		res = references.theRef->_description_item_in_group_in_section(
			references.sectionId, references.groupId, references.itemId,
			&itemProps.inPropId, &itemProps.tpV, itemProps.description);
		if(res != JVX_NO_ERROR)
		{
			//
			itemProps.tpV = JVX_COMPONENT_UNKNOWN;
		}
	}
	if(!(itemProps.tpV.tp == JVX_COMPONENT_UNKNOWN))
	{
		do
		{

			cleanContent();
			updateWindow_construct();
			updateWindow_contents(requiresRedraw);
		}
		while(requiresRedraw);
		
		updateWindow_update();
	}

};

void
realtimeViewerOneProperty::deleteUiElementsInWidget()
{
	jvxSize i;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;

#ifdef JVX_VERBOSE_OUTPUT
	std::cout << "Del<" << references.sectionId << "," << references.groupId << "," << references.itemId << ">" << std::endl;
#endif
	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if (theItemData)
	{
		for (i = 0; i < theItemData->label_flags.size(); i++)
		{
			delete(theItemData->label_flags[i]);
		}
		theItemData->label_flags.clear();

		for (i = 0; i < theItemData->label_entries.size(); i++)
		{
			delete(theItemData->label_entries[i]);
		}
		theItemData->label_entries.clear();

		if (theItemData->button_1)
		{
			delete(theItemData->button_1);
		}
		theItemData->button_1 = NULL;

		if (theItemData->button_2)
		{
			delete(theItemData->button_2);
		}
		theItemData->button_2 = NULL;

		if (theItemData->button_3)
		{
			delete(theItemData->button_3);
		}
		theItemData->button_3 = NULL;

		if (theItemData->button_4)
		{
			delete(theItemData->button_4);
		}
		theItemData->button_4 = NULL;
		//theItemData->checkbox_1 = NULL;

		if (theItemData->combo_box)
		{
			delete(theItemData->combo_box);
		}
		theItemData->combo_box = NULL;

		if (theItemData->label_max)
		{
			delete(theItemData->label_max);
		}
		theItemData->label_max = NULL;

		if (theItemData->label_min)
		{
			delete(theItemData->label_min);
		}
		theItemData->label_min = NULL;

		if (theItemData->label_valid)
		{
			delete(theItemData->label_valid);
		}
		theItemData->label_valid = NULL;

		if (theItemData->line_edit)
		{
			delete(theItemData->line_edit);
		}
		theItemData->line_edit = NULL;

		if (theItemData->slider)
		{
			delete(theItemData->slider);
		}
		theItemData->slider = NULL;
		
		if (theItemData->combo_box_access_rwcd)
		{
			delete theItemData->combo_box_access_rwcd;
		}
		theItemData->combo_box_access_rwcd = NULL;

		if (theItemData->combo_box_access_what)
		{
			delete theItemData->combo_box_access_what;
		}
		theItemData->combo_box_access_what = NULL;

		if (theItemData->combo_box_config)
		{
			delete theItemData->combo_box_config;
		}
		theItemData->combo_box_config = NULL;

		if (theItemData->frame_tags)
		{
			delete theItemData->frame_tags;
		}
		theItemData->frame_tags = NULL;

		if (theItemData->checkbox_onoff)
		{
			delete theItemData->checkbox_onoff;
		}
		theItemData->checkbox_onoff = NULL;

		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
			reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::cleanBeforeDelete()
{
	jvxRealtimeViewerPropertyItem* theItemData = NULL;
#ifdef JVX_VERBOSE_OUTPUT
	std::cout << "Cln<" << references.sectionId << "," << references.groupId << "," << references.itemId << ">" << std::endl;
#endif
	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
#ifdef JVX_VERBOSE_OUTPUT
		std::cout << "Cln: Item data:" << theItemData << std::endl;
#endif
		// We need to only remopve the flags, everything else is handled by QT
		theItemData->label_flags.clear();
		theItemData->label_entries.clear();

		theItemData->button_1 = NULL;
		theItemData->button_2 = NULL;
		theItemData->button_3 = NULL;
		theItemData->button_4 = NULL;
		//theItemData->checkbox_1 = NULL;

		theItemData->combo_box = NULL;
		theItemData->label_max = NULL;
		theItemData->label_min = NULL;
		theItemData->label_valid = NULL;
		theItemData->line_edit = NULL;
		theItemData->setup_complete = false;
		theItemData->slider = NULL;
		theItemData->combo_box_access_rwcd = NULL;
		theItemData->combo_box_access_what = NULL;
		theItemData->combo_box_config = NULL;
		theItemData->frame_tags = NULL;
		theItemData->checkbox_onoff = NULL;

		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
			reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::cleanContent()
{
	jvxSize i;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;

#ifdef JVX_VERBOSE_OUTPUT
	std::cout << "Check<" << references.sectionId << "," << references.groupId << "," << references.itemId << ">" << std::endl;
#endif

	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
#ifdef JVX_VERBOSE_OUTPUT
		std::cout << "Cln: Item data:" << theItemData << std::endl;
#endif
		assert(theItemData->label_flags.size() == 0);
		assert(theItemData->label_entries.size() == 0);
		assert(theItemData->button_1 == NULL);
		assert(theItemData->button_2 == NULL);
		assert(theItemData->button_3 == NULL);
		assert(theItemData->button_4 == NULL);
		assert(theItemData->combo_box == NULL);
		assert(theItemData->label_max == NULL);
		assert(theItemData->label_min == NULL);
		assert(theItemData->label_valid == NULL);
		assert(theItemData->line_edit == NULL);
		assert(theItemData->slider == NULL);
		assert(theItemData->combo_box_access_rwcd == NULL);
		assert(theItemData->combo_box_access_what == NULL);
		assert(theItemData->combo_box_config == NULL);
		assert(theItemData->frame_tags == NULL);
		assert(theItemData->checkbox_onoff == NULL);

		theItemData->setup_complete = false;
		
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
			reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::updateWindow_construct()
{
	jvxBool iamanewlayout = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString fldStr, fldStr2;

	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;

	jvxSelectionList selList;
	jvxSize i;
	jvxComponentIdentification objTp;

	QLineEdit* newElement = NULL;
	QLabel* newLabel = NULL;
	QLabel* oneFlagLabel = NULL;
	QLabel* oneEntryLabel = NULL;
	jvxCallManagerProperties callGate;

	jvxPropertyContext ctxt = JVX_PROPERTY_CONTEXT_UNKNOWN;
//	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;

	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
		if(theItemData->setup_complete == false)
		{
			jvx_initPropertyReferenceTriple(&theTriple);


			if(itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
			{
				res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
			}

			if((res == JVX_NO_ERROR) && theTriple.theProps)
			{
				jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);
				res = theTriple.theObj->request_specialization(NULL, &objTp, NULL);
				assert(res == JVX_NO_ERROR);

				res = theTriple.theObj->description( &fldStr);
				if(res == JVX_NO_ERROR)
				{
					theItemData->descriptor.objDescription = jvxComponentIdentification_txt(objTp);
					theItemData->descriptor.objDescription += "::";
					theItemData->descriptor.objDescription += fldStr.std_str();
				}

				theItemData->descriptor.origin = "";
				ident.idx = itemProps.inPropId;
				res = theTriple.theProps->description_property(callGate, theItemData->descriptor,
					ident);
				if(res == JVX_NO_ERROR)
				{

					theItemData->descriptor.propPurpose = std::string(jvxPropertyContext_txt(ctxt));
					res = theTriple.theProps->get_property_extended_info(
						callGate,
						&fldStr, JVX_PROPERTY_INFO_ORIGIN,
						jPAGID(theItemData->descriptor.globalIdx,
						theItemData->descriptor.category));

					if (res == JVX_NO_ERROR)
					{
						theItemData->descriptor.origin = fldStr.std_str();
					}
				}

				QGridLayout* theLayout = NULL;
				QLayout* layout = this->frame_content->layout();
				theLayout = dynamic_cast<QGridLayout*>(layout);
				if(theLayout == NULL)
				{
					theLayout =	new QGridLayout;
					iamanewlayout = true;
				}

				if(jvx_applyPropertyFilter(theItemData->descriptor.format, theItemData->descriptor.num, theItemData->descriptor.decTp, JVX_PROPERTIES_FILTER_PROPERTIES_RTV))
				{
					int idx_lastrow = 1;
					int idx_maxwidth = 2;

					newLabel = new QLabel(this);
					newLabel->setText(theItemData->descriptor.description.c_str());
					std::string lStr = "OBJECT=" + theItemData->descriptor.objDescription + "; DESCRIPTOR=" +
						theItemData->descriptor.description.std_str() + "; PURPOSE=" + theItemData->descriptor.propPurpose;
					if (!theItemData->descriptor.origin.empty())
					{
						lStr += "ORIGIN=" + theItemData->descriptor.origin;
					}
					newLabel->setToolTip(lStr.c_str());
					// All cases handled in separate if statements

					theItemData->validOnCreation = true;
					switch(theItemData->descriptor.format)
					{
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_64BIT_LE:
					case JVX_DATAFORMAT_DATA:
					case JVX_DATAFORMAT_SIZE:
					case JVX_DATAFORMAT_STRING:
						theLayout->addWidget(newLabel, 0, 0, 1, 2);
						theItemData->line_edit = new QLineEdit_ext(this);
						if (theItemData->descriptor.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
						{
							theItemData->line_edit->setReadOnly(true);
						}

						theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));
						
						theLayout->addWidget(theItemData->line_edit, 1, 0, 1, 1);

						theItemData->label_valid = new QLabel(this);
						theItemData->label_valid->setMinimumSize(QSize(10, 10));
						theItemData->label_valid->setMaximumSize(QSize(10, 10));
						theItemData->label_valid->setAutoFillBackground(true);
						theItemData->label_valid->setFrameShape(QFrame::StyledPanel);
						if(theItemData->descriptor.isValid)
						{
							setBackgroundLabelColor(Qt::green, theItemData->label_valid);
						}
						else
						{
							setBackgroundLabelColor(Qt::red, theItemData->label_valid);
						}
						theLayout->addWidget(theItemData->label_valid, 1, 1, 1, 1);
						idx_lastrow = 2;
						idx_maxwidth = 2;

						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
						theLayout->addWidget(newLabel, 0, 0, 1, 5);
						theItemData->label_min = new QLabel(this);
						theLayout->addWidget(theItemData->label_min, 1, 0, 1, 1);
						theItemData->slider = new QSlider_ext(this);
						theItemData->slider->setMinimum(0);
						theItemData->slider->setMaximum(100);
						theItemData->slider->setMinimumWidth(80);

						theLayout->addWidget(theItemData->slider, 1, 1, 1, 1);
						theItemData->label_max = new QLabel(this);
						theLayout->addWidget(theItemData->label_max, 1, 2, 1, 1);
						theItemData->line_edit = new QLineEdit_ext(this);
						theLayout->addWidget(theItemData->line_edit, 1, 3, 1, 1);
						theItemData->slider->setMaximum(100);
						theItemData->slider->setMinimum(0);

						theItemData->label_valid = new QLabel(this);
						theItemData->label_valid->setMinimumSize(QSize(10, 10));
						theItemData->label_valid->setMaximumSize(QSize(10, 10));
						theItemData->label_valid->setAutoFillBackground(true);
						theItemData->label_valid->setFrameShape(QFrame::StyledPanel);
						if(theItemData->descriptor.isValid)
						{
							setBackgroundLabelColor(Qt::green, theItemData->label_valid);
						}
						else
						{
							setBackgroundLabelColor(Qt::red, theItemData->label_valid);
						}
						theLayout->addWidget(theItemData->label_valid, 1, 4, 1, 1);
						idx_lastrow = 2;
						idx_maxwidth = 5;

						break;
					case JVX_DATAFORMAT_STRING_LIST:
						theLayout->addWidget(newLabel, 0, 0, 1, 1);
						theItemData->label_valid = new QLabel(this);
						theItemData->label_valid->setMinimumSize(QSize(10, 10));
						theItemData->label_valid->setMaximumSize(QSize(10, 10));
						theItemData->label_valid->setAutoFillBackground(true);
						theItemData->label_valid->setFrameShape(QFrame::StyledPanel);
						if(theItemData->descriptor.isValid)
						{
							setBackgroundLabelColor(Qt::green, theItemData->label_valid);
						}
						else
						{
							setBackgroundLabelColor(Qt::red, theItemData->label_valid);
						}
						theLayout->addWidget(theItemData->label_valid, 1, 0, 1, 1);
						idx_lastrow = 2;
						idx_maxwidth = 1;

						// What could we want to do with a string list??
						break;
					case JVX_DATAFORMAT_16BIT_LE:
						switch(theItemData->descriptor.decTp)
						{
						case JVX_PROPERTY_DECODER_FORMAT_IDX:
						case JVX_PROPERTY_DECODER_SUBFORMAT_IDX:
							theLayout->addWidget(newLabel, 0, 0, 1, 2);

							theItemData->line_edit = new QLineEdit_ext(this);
							if(theItemData->descriptor.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
							{
								theItemData->line_edit->setReadOnly(true);
								theItemData->line_edit->setToolTip("<read-only>");
							}
							theLayout->addWidget(theItemData->line_edit, 1, 0, 1, 1);

							theItemData->label_valid = new QLabel(this);
							theItemData->label_valid->setMinimumSize(QSize(10, 10));
							theItemData->label_valid->setMaximumSize(QSize(10, 10));
							theItemData->label_valid->setAutoFillBackground(true);
							theItemData->label_valid->setFrameShape(QFrame::StyledPanel);
							if(theItemData->descriptor.isValid)
							{
								setBackgroundLabelColor(Qt::green, theItemData->label_valid);
							}
							else
							{
								setBackgroundLabelColor(Qt::red, theItemData->label_valid);
							}
							theLayout->addWidget(theItemData->label_valid, 1, 1, 1, 1);
							idx_lastrow = 2;
							idx_maxwidth = 2;

							break;
						case JVX_PROPERTY_DECODER_SIMPLE_ONOFF:

							theLayout->addWidget(newLabel, 0, 0, 1, 2);
							
							theItemData->checkbox_onoff = new QCheckBox_ext(this);
							theItemData->checkbox_onoff->setEnabled(true);
							if (theItemData->descriptor.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
							{
								theItemData->checkbox_onoff->setEnabled(false);
								theItemData->checkbox_onoff->setToolTip("<read-only>");
							}
							theLayout->addWidget(theItemData->checkbox_onoff, 1, 0, 1, 1);

							theItemData->label_valid = new QLabel(this);
							theItemData->label_valid->setMinimumSize(QSize(10, 10));
							theItemData->label_valid->setMaximumSize(QSize(10, 10));
							theItemData->label_valid->setAutoFillBackground(true);
							theItemData->label_valid->setFrameShape(QFrame::StyledPanel);
							if (theItemData->descriptor.isValid)
							{
								setBackgroundLabelColor(Qt::green, theItemData->label_valid);
							}
							else
							{
								setBackgroundLabelColor(Qt::red, theItemData->label_valid);
							}
							theLayout->addWidget(theItemData->label_valid, 1, 1, 1, 1);
							idx_lastrow = 2;
							idx_maxwidth = 2;
							break;
						default:
							theLayout->addWidget(newLabel, 0, 0, 1, 2);
							theItemData->line_edit = new QLineEdit_ext(this);
							if(theItemData->descriptor.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
							{
								theItemData->line_edit->setReadOnly(true);
								theItemData->line_edit->setToolTip("<read-only>");
							}
							theLayout->addWidget(theItemData->line_edit, 1, 0, 1, 1);

							theItemData->label_valid = new QLabel(this);
							theItemData->label_valid->setMinimumSize(QSize(10, 10));
							theItemData->label_valid->setMaximumSize(QSize(10, 10));
							theItemData->label_valid->setAutoFillBackground(true);
							theItemData->label_valid->setFrameShape(QFrame::StyledPanel);
							if(theItemData->descriptor.isValid)
							{
								setBackgroundLabelColor(Qt::green, theItemData->label_valid);
							}
							else
							{
								setBackgroundLabelColor(Qt::red, theItemData->label_valid);
							}
							theLayout->addWidget(theItemData->label_valid, 1, 1, 1, 1);
							idx_lastrow = 2;
							idx_maxwidth = 2;

							break;
						}
						break;
					case JVX_DATAFORMAT_SELECTION_LIST:

						res = theTriple.theProps->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
							jPAGID(theItemData->descriptor.globalIdx, theItemData->descriptor.category));

						theItemData->label_valid = new QLabel(this);
						theItemData->label_valid->setMinimumSize(QSize(10, 10));
						theItemData->label_valid->setMaximumSize(QSize(10, 10));
						theItemData->label_valid->setAutoFillBackground(true);
						theItemData->label_valid->setFrameShape(QFrame::StyledPanel);
						
						std::vector<std::string> entries;
						for (i = 0; i < selList.strList.ll(); i++)
						{
							entries.push_back(selList.strList.std_str_at(i));
						}
						
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
						{
							theItemData->validOnCreation = true;
							setBackgroundLabelColor(Qt::green, theItemData->label_valid);
						}
						else
						{
							theItemData->validOnCreation = false;
							setBackgroundLabelColor(Qt::red, theItemData->label_valid);
						}

						switch(theItemData->descriptor.decTp)
						{
						case JVX_PROPERTY_DECODER_BITFIELD:
							theLayout->addWidget(newLabel, 0, 0, 1, (int)entries.size() + 1);
							for(i = 0; i < entries.size(); i++)
							{
								oneFlagLabel = new QLabel(this);
								oneFlagLabel->setMinimumSize(QSize(10, 10));
								oneFlagLabel->setMaximumSize(QSize(10, 10));
								oneFlagLabel->setAutoFillBackground(true);
								oneFlagLabel->setFrameShape(QFrame::StyledPanel);
								if(jvx_bitTest(selList.bitFieldSelected(), i))
								{
									setBackgroundLabelColor(Qt::white, oneFlagLabel);
								}
								else
								{
									setBackgroundLabelColor(Qt::gray, oneFlagLabel);
								}

								//oneFlagLabel->setStyle(NULL);
								oneEntryLabel = new QLabel(this);
								oneEntryLabel->setText(entries[i].c_str());

								theItemData->label_flags.push_back(oneFlagLabel);
								theItemData->label_entries.push_back(oneEntryLabel);

								theLayout->addWidget(oneFlagLabel, 1, (int)i, 1, 1, Qt::AlignHCenter);
								theLayout->addWidget(oneEntryLabel, 2, (int)i, 1, 1, Qt::AlignHCenter);
							}
							idx_lastrow = 3;
							idx_maxwidth = i+1;

							theLayout->addWidget(theItemData->label_valid, 2, (int)i, 1, 1);
							break;
						case JVX_PROPERTY_DECODER_SINGLE_SELECTION:
							theLayout->addWidget(newLabel, 0, 0, 1, 2);
							theItemData->combo_box = new QComboBox_ext(this);
							for(i = 0; i < entries.size(); i++)
							{
								theItemData->combo_box->addItem(entries[i].c_str());
							}

							theLayout->addWidget(theItemData->combo_box, 1, 0, 1, 1, Qt::AlignHCenter);
							theLayout->addWidget(theItemData->label_valid, 1, 1, 1, 1);
							idx_lastrow = 2;
							idx_maxwidth = 2;


							break;
						case JVX_PROPERTY_DECODER_MULTI_SELECTION:
						case JVX_PROPERTY_DECODER_NONE:

							theLayout->addWidget(newLabel, 0, 0, 1, 2);

							theItemData->combo_box = new QComboBox_ext(this);
							for(i = 0; i < entries.size(); i++)
							{
								theItemData->combo_box->addItem(entries[i].c_str());
							}

							theLayout->addWidget(theItemData->combo_box, 1, 0, 1, 2, Qt::AlignHCenter);
							theLayout->addWidget(theItemData->label_valid, 1, 2, 1, 1);

							theItemData->button_1 = new QPushButton(this);
							theItemData->button_1->setText("All On");
							theItemData->button_1->setMaximumWidth(40);
							theItemData->button_1->setMaximumHeight(40);
							theItemData->button_2 = new QPushButton(this);
							theItemData->button_2->setText("All Off");
							theItemData->button_2->setMaximumWidth(40);
							theItemData->button_2->setMaximumHeight(40);

							connect(theItemData->button_1, SIGNAL(clicked()), this, SLOT(button_select_all()));
							connect(theItemData->button_2, SIGNAL(clicked()), this, SLOT(button_unselect_all()));

							theLayout->addWidget(theItemData->button_1, 2, 0, 1, 1);
							theLayout->addWidget(theItemData->button_2, 2, 1, 1, 1);
							idx_lastrow = 3;
							idx_maxwidth = 2;


							break;

						case JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:

							theLayout->addWidget(newLabel, 0, 0, 1, 5);

							theItemData->combo_box = new QComboBox_ext(this);
							theLayout->addWidget(theItemData->combo_box, 1, 0, 1, 5, Qt::AlignHCenter);
							for(i = 0; i < entries.size(); i++)
							{
								theItemData->combo_box->addItem(entries[i].c_str());
							}

							if(entries.size() == 0)
							{
								theItemData->selection = JVX_SIZE_UNSELECTED;
							}
							else
							{
								if(theItemData->selection >= entries.size())
								{
									theItemData->selection = JVX_SIZE_UNSELECTED;
								}
							}

							theLayout->addWidget(theItemData->label_valid, 1, 5, 1, 1);

							theItemData->button_1 = new QPushButton(this);
							theItemData->button_1->setText("All On");
							theItemData->button_1->setMaximumWidth(40);
							theItemData->button_1->setMaximumHeight(40);
							theItemData->button_2 = new QPushButton(this);
							theItemData->button_2->setText("All off");
							theItemData->button_2->setMaximumWidth(40);
							theItemData->button_2->setMaximumHeight(40);
							theItemData->button_3 = new QPushButton(this);
							theItemData->button_3->setText("up");
							theItemData->button_3->setMaximumWidth(40);
							theItemData->button_3->setMaximumHeight(40);
							theItemData->button_4 = new QPushButton(this);
							theItemData->button_4->setText("down");
							theItemData->button_4->setMaximumWidth(40);
							theItemData->button_4->setMaximumHeight(40);
							/*
							theItemData->checkbox_1 = new QCheckBox(this);
							theItemData->checkbox_1->setText("");
							theItemData->checkbox_1->setMaximumWidth(20);
							theItemData->checkbox_1->setMaximumHeight(20);
							*/

							connect(theItemData->button_1, SIGNAL(clicked()), this, SLOT(button_select_all()));
							connect(theItemData->button_2, SIGNAL(clicked()), this, SLOT(button_unselect_all()));
							connect(theItemData->button_3, SIGNAL(clicked()), this, SLOT(button_up()));
							connect(theItemData->button_4, SIGNAL(clicked()), this, SLOT(button_down()));
							//connect(theItemData->checkbox_1, SIGNAL(clicked(bool)), this, SLOT(checkbox_toggled(bool)));

							theLayout->addWidget(theItemData->button_1, 2, 0, 1, 1);
							theLayout->addWidget(theItemData->button_2, 2, 1, 1, 1);
							theLayout->addWidget(theItemData->button_3, 2, 2, 1, 1);
							theLayout->addWidget(theItemData->button_4, 2, 3, 1, 1);
							//theLayout->addWidget(theItemData->checkbox_1, 2, 4, 1, 1);

							idx_lastrow = 3;
							idx_maxwidth = 4;

							break;
						case JVX_PROPERTY_DECODER_INPUT_FILE_LIST:

							theLayout->addWidget(newLabel, 0, 0, 1, 2);

							theItemData->combo_box = new QComboBox_ext(this);
							for(i = 0; i < entries.size(); i++)
							{
								theItemData->combo_box->addItem(entries[i].c_str());
							}

							theLayout->addWidget(theItemData->combo_box, 1, 0, 1, 2, Qt::AlignHCenter);
							theLayout->addWidget(theItemData->label_valid, 1, 2, 1, 1);

							theItemData->button_1 = new QPushButton(this);
							theItemData->button_1->setText("+");
							theItemData->button_1->setMaximumWidth(40);
							theItemData->button_1->setMaximumHeight(40);
							theItemData->button_2 = new QPushButton(this);
							theItemData->button_2->setText("-");
							theItemData->button_2->setMaximumWidth(40);
							theItemData->button_2->setMaximumHeight(40);

							connect(theItemData->button_1, SIGNAL(clicked()), this, SLOT(button_select_all()));
							connect(theItemData->button_2, SIGNAL(clicked()), this, SLOT(button_unselect_all()));

							theLayout->addWidget(theItemData->button_1, 2, 0, 1, 1);
							theLayout->addWidget(theItemData->button_2, 2, 1, 1, 1);
							idx_lastrow = 3;
							idx_maxwidth = 2;


							break;

						case JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST:
							theLayout->addWidget(newLabel, 0, 0, 1, 2);

							theItemData->combo_box = new QComboBox_ext(this);
							for(i = 0; i < entries.size(); i++)
							{
								theItemData->combo_box->addItem(entries[i].c_str());
							}

							theLayout->addWidget(theItemData->combo_box, 1, 0, 1, 2, Qt::AlignHCenter);
							theLayout->addWidget(theItemData->label_valid, 1, 2, 1, 1);

							theItemData->button_1 = new QPushButton(this);
							theItemData->button_1->setText("+");
							theItemData->button_1->setMaximumWidth(40);
							theItemData->button_1->setMaximumHeight(40);
							theItemData->button_2 = new QPushButton(this);
							theItemData->button_2->setText("-");
							theItemData->button_2->setMaximumWidth(40);
							theItemData->button_2->setMaximumHeight(40);

							connect(theItemData->button_1, SIGNAL(clicked()), this, SLOT(button_select_all()));
							connect(theItemData->button_2, SIGNAL(clicked()), this, SLOT(button_unselect_all()));

							theLayout->addWidget(theItemData->button_1, 2, 0, 1, 1);
							theLayout->addWidget(theItemData->button_2, 2, 1, 1, 1);
							idx_lastrow = 3;
							idx_maxwidth = 3;
							break;
						default:
							assert(0);
						}
							
						theItemData->validOnCreation = theItemData->descriptor.isValid;
						break;
					}//switch(propParams.format)

					// ======================================================================
					QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); 
					
					theItemData->frame_tags = new QFrame(this);
					QGridLayout* myLayout = new QGridLayout(theItemData->frame_tags);
					theItemData->frame_tags->setLayout(myLayout);
					myLayout->setContentsMargins(1, 1, 1, 1);
					//myFrame->setSizePolicy(sizePolicy);
					QPalette lpal;
					QBrush brush(Qt::yellow);
					brush.setStyle(Qt::SolidPattern);
					lpal.setBrush(QPalette::Active, QPalette::Window, brush);
					lpal.setBrush(QPalette::Inactive, QPalette::Base, brush);
					lpal.setBrush(QPalette::Inactive, QPalette::Window, brush);
					lpal.setBrush(QPalette::Disabled, QPalette::Base, brush);
					lpal.setBrush(QPalette::Disabled, QPalette::Window, brush);
					theItemData->frame_tags->setPalette(lpal);
					theItemData->frame_tags->setAutoFillBackground(true);
					theItemData->frame_tags->setFrameShape(QFrame::StyledPanel);
					theItemData->frame_tags->setFrameShadow(QFrame::Raised);

					theItemData->combo_box_access_what = new QComboBox_ext(this, 3);
					theItemData->combo_box_access_what->addItem("r");
					theItemData->combo_box_access_what->addItem("w");
					theItemData->combo_box_access_what->addItem("c");
					theItemData->combo_box_access_what->addItem("d");
					myLayout->addWidget(theItemData->combo_box_access_what, 0, 0, 1, 1, Qt::AlignHCenter);

					theItemData->combo_box_access_rwcd = new QComboBox_ext(this, 1);

					//theItemData->combo_box_access->setSizePolicy(sizePolicy);
					for (i = 0; i < 16; i++)
					{
						theItemData->combo_box_access_rwcd->addItem(entriesGroupL[i]);
					}
					//res = theTriple.theProps->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST, theItemData->descriptor.globalIdx, theItemData->descriptor.category, 0, false, &isValid, &theItemData->descriptor.accessType);
					myLayout->addWidget(theItemData->combo_box_access_rwcd, 0, 1, 1, 1, Qt::AlignHCenter);

					theItemData->combo_box_config = new QComboBox_ext(this, 2);
					//theItemData->combo_box_config->setSizePolicy(sizePolicy);
					for (i = 0; i < 4; i++)
					{
						theItemData->combo_box_config->addItem(entriesGroupC[i]);
					}
					myLayout->addWidget(theItemData->combo_box_config, 0, 2, 1, 1, Qt::AlignHCenter);

					//myFrame->setMinimumWidth(200);
					//myFrame->setMinimumHeight(200);
					theLayout->addWidget(theItemData->frame_tags, idx_lastrow, 0, 1, idx_maxwidth, Qt::AlignHCenter);

#if 0
					theItemData->combo_box_access = new QComboBox_ext(this, true);
					QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
					theItemData->combo_box_access->setSizePolicy(sizePolicy);
					for (i = 0; i < 15; i++)
					{
						theItemData->combo_box_access->addItem(entriesGroupL[i]);
					}
					//res = theTriple.theProps->get_property(callGate, jPRG( &selList, 1, JVX_DATAFORMAT_SELECTION_LIST, theItemData->descriptor.globalIdx, theItemData->descriptor.category, 0, false, &isValid, &theItemData->descriptor.accessType);

					theLayout->addWidget(theItemData->combo_box_access, idx_lastrow, 0, 1, idx_maxwidth-1, Qt::AlignHCenter);					
					
					theItemData->label_access = new QLabel(this);
					theItemData->label_access->setText("Access");
					theLayout->addWidget(theItemData->label_access, idx_lastrow, idx_maxwidth - 1, 1, 1, Qt::AlignHCenter);

					// ===============================================================================
					idx_lastrow++;
					theItemData->combo_box_config = new QComboBox_ext(this, true);
					theItemData->combo_box_config->setSizePolicy(sizePolicy);
					for (i = 0; i < 3; i++)
					{
						theItemData->combo_box_config->addItem(entriesGroupC[i]);
					}

					theLayout->addWidget(theItemData->combo_box_config, idx_lastrow, 0, 1, idx_maxwidth - 1, Qt::AlignHCenter);

					theItemData->label_config = new QLabel(this);
					theItemData->label_config->setText("Config");
					theLayout->addWidget(theItemData->label_config, idx_lastrow, idx_maxwidth - 1, 1, 1, Qt::AlignHCenter);

					theLayout->setContentsMargins(2, 2, 2, 2);
					theLayout->setVerticalSpacing(1);
#endif
				}
				else
				{

					newLabel = new QLabel(this);
					newLabel->setText("- not supported in viewer -");
					QFont qf = newLabel->font();
					qf.setItalic(true);
					newLabel->setFont(qf);

					newLabel->setToolTip((theItemData->descriptor.objDescription + ":" + theItemData->descriptor.description.std_str()).c_str());
					theLayout->addWidget(newLabel, 0, 0, 1, 2);

					theItemData->label_valid = new QLabel(this);
					theItemData->label_valid->setMinimumSize(QSize(10, 10));
					theItemData->label_valid->setMaximumSize(QSize(10, 10));
					theItemData->label_valid->setAutoFillBackground(true);
					theItemData->label_valid->setFrameShape(QFrame::StyledPanel);
					setBackgroundLabelColor(Qt::red, theItemData->label_valid);
					theLayout->addWidget(theItemData->label_valid, 1, 1, 1, 1);

					theLayout->addWidget(newLabel, 0, 0, 1, 1);
					theItemData->validOnCreation = false;
				}

				if(iamanewlayout)
				{
					this->frame_content->setLayout(theLayout);
				}

				res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
			}
			else
			{
				// It seems that there is no active component, make this an invalid item
				QGridLayout* theLayout = new QGridLayout;

				newLabel = new QLabel(this);
				newLabel->setText("- no active component -");
				QFont qf = newLabel->font();
				qf.setItalic(true);
				newLabel->setFont(qf);

				theLayout->addWidget(newLabel, 0, 0, 1, 2);

				theItemData->label_valid = new QLabel(this);
				theItemData->label_valid->setMinimumSize(QSize(10, 10));
				theItemData->label_valid->setMaximumSize(QSize(10, 10));
				theItemData->label_valid->setAutoFillBackground(true);
				theItemData->label_valid->setFrameShape(QFrame::StyledPanel);
				setBackgroundLabelColor(Qt::red, theItemData->label_valid);
				theLayout->addWidget(theItemData->label_valid, 1, 1, 1, 1);

				theLayout->addWidget(newLabel, 0, 0, 1, 1);
				this->frame_content->setLayout(theLayout);
				theItemData->validOnCreation = false;
			}
			theItemData->setup_complete = true;
 		}
		else
		{
			assert(0);
			this->local.parentRef->askForCompleteRebuild();
		}
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
			reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::updateWindow_contents(jvxBool& requiresRedraw, jvxBool fullUpdate)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxSelectionList selList;
	jvxSize i;
	jvxInt8 int8Val = 0;
	jvxInt16 int16Val = 0;
	jvxInt32 int32Val = 0;
	jvxInt64 int64Val = 0;
	jvxData dblVal = 0;
	jvxSize intSz = 0;
	jvxData fltVal = 0;
	QString txt;	
	jvxValueInRange valInRange;
	//bool foundit = false;
	std::string entry;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;

	QFont ft ;
	QPalette pal;

	jvx_initPropertyReferenceTriple(&theTriple);

	requiresRedraw = false;

	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
		callGate.on_get.prop_access_type = &theItemData->descriptor.accessType;
		if(itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			if(jvx_applyPropertyFilter(theItemData->descriptor.format, theItemData->descriptor.num, theItemData->descriptor.decTp, JVX_PROPERTIES_FILTER_PROPERTIES_RTV))
			{
				// All cases handled in separate if statements
				switch(theItemData->descriptor.format)
				{
				case JVX_DATAFORMAT_8BIT:
					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset(true);
					res = theTriple.theProps->get_property(callGate, jPRG( &int8Val, 1, theItemData->descriptor.format),ident, trans);
					if(res == JVX_NO_ERROR)
					{
						txt = jvx_int2String(int8Val).c_str();
						theItemData->line_edit->setText(txt);
					}

					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						theItemData->descriptor.isValid = true;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						theItemData->line_edit->setReadOnly(false);
						QFont ft = theItemData->line_edit->font();
						ft.setItalic(false);
						theItemData->line_edit->setFont(ft);
						QPalette pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::black);
						theItemData->line_edit->setPalette(pal);
					}
					else
					{
						theItemData->descriptor.isValid = false;
						theItemData->line_edit->setReadOnly(true);
						QFont ft = theItemData->line_edit->font();
						ft.setItalic(true);
						theItemData->line_edit->setFont(ft);
						QPalette pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::gray);
						theItemData->line_edit->setPalette(pal);

						
					}
					
					theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));
					break;

				case JVX_DATAFORMAT_16BIT_LE:
					ident.reset(theItemData->descriptor.globalIdx,
						theItemData->descriptor.category);
					trans.reset(true);
					res = theTriple.theProps->get_property(callGate, jPRG( &int16Val, 1, theItemData->descriptor.format),ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						theItemData->descriptor.isValid = true;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						switch(theItemData->descriptor.decTp)
						{
						case JVX_PROPERTY_DECODER_FORMAT_IDX:
						case JVX_PROPERTY_DECODER_SUBFORMAT_IDX:
							if (theItemData->descriptor.decTp == JVX_PROPERTY_DECODER_FORMAT_IDX)
							{
								txt = jvxDataFormat_txt(int16Val);
							}
							else
							{
								txt = jvxDataFormatGroup_txt(int16Val);
							}
							theItemData->line_edit->setText(txt);
							if (theItemData->descriptor.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
							{
								theItemData->line_edit->setReadOnly(true);
								QFont ft = theItemData->line_edit->font();
								ft.setItalic(true);
								theItemData->line_edit->setFont(ft);
								QPalette pal = theItemData->line_edit->palette();
								pal.setColor(QPalette::Text, Qt::gray);
								theItemData->line_edit->setPalette(pal);
							}
							else
							{
								theItemData->line_edit->setReadOnly(false);
								QFont ft = theItemData->line_edit->font();
								ft.setItalic(false);
								theItemData->line_edit->setFont(ft);
								QPalette pal = theItemData->line_edit->palette();
								pal.setColor(QPalette::Text, Qt::black);
								theItemData->line_edit->setPalette(pal);
							}
							theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));

							break;

						case JVX_PROPERTY_DECODER_SIMPLE_ONOFF:
							if (int16Val == c_false)
							{
								theItemData->checkbox_onoff->setChecked(false);
							}
							else
							{
								theItemData->checkbox_onoff->setChecked(true);
							}


							if (theItemData->descriptor.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
							{
								theItemData->checkbox_onoff->setEnabled(false);
							}
							else
							{
								theItemData->checkbox_onoff->setEnabled(true);
							}
							theItemData->checkbox_onoff->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));
							break;

						default:
							txt = jvx_int2String(int16Val).c_str();
							theItemData->line_edit->setText(txt);
							if (theItemData->descriptor.accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
							{
								theItemData->line_edit->setReadOnly(true);
								QFont ft = theItemData->line_edit->font();
								ft.setItalic(true);
								theItemData->line_edit->setFont(ft);
								QPalette pal = theItemData->line_edit->palette();
								pal.setColor(QPalette::Text, Qt::gray);
								theItemData->line_edit->setPalette(pal);
							}
							else
							{
								theItemData->line_edit->setReadOnly(false);
								QFont ft = theItemData->line_edit->font();
								ft.setItalic(false);
								theItemData->line_edit->setFont(ft);
								QPalette pal = theItemData->line_edit->palette();
								pal.setColor(QPalette::Text, Qt::black);
								theItemData->line_edit->setPalette(pal);
							}
							theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));

							break;
						}
					}
					else
					{
						theItemData->descriptor.isValid = false;

						txt = "--";
						int16Val = c_false;
						switch (theItemData->descriptor.decTp)
						{
						case JVX_PROPERTY_DECODER_FORMAT_IDX:
							
							theItemData->line_edit->setText(txt);
							theItemData->line_edit->setReadOnly(true);
							ft = theItemData->line_edit->font();
							ft.setItalic(true);
							theItemData->line_edit->setFont(ft);
							pal = theItemData->line_edit->palette();
							pal.setColor(QPalette::Text, Qt::gray);
							theItemData->line_edit->setPalette(pal);
							theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));

							break;

						case JVX_PROPERTY_DECODER_SIMPLE_ONOFF:
							theItemData->checkbox_onoff->setChecked(false);

							theItemData->checkbox_onoff->setEnabled(false);
							theItemData->checkbox_onoff->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));
							break;

						default:
							txt = jvx_int2String(int16Val).c_str();
							theItemData->line_edit->setText(txt);
							
							theItemData->line_edit->setReadOnly(true);
							ft = theItemData->line_edit->font();
							ft.setItalic(true);
							theItemData->line_edit->setFont(ft);
							pal = theItemData->line_edit->palette();
							pal.setColor(QPalette::Text, Qt::gray);
							theItemData->line_edit->setPalette(pal);
							theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));

							break;
						}
					}					
					break;
				case JVX_DATAFORMAT_32BIT_LE:
					ident.reset(theItemData->descriptor.globalIdx,
						theItemData->descriptor.category);
					trans.reset(true);
					res = theTriple.theProps->get_property(callGate, jPRG( &int32Val, 1, theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str()
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						theItemData->descriptor.isValid = true;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						txt = jvx_int2String(int32Val).c_str();
						theItemData->line_edit->setText(txt);
					}
					else
					{
						theItemData->descriptor.isValid = false;
						txt = "--";
					}

					if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
					{
						theItemData->line_edit->setReadOnly(true);
						ft = theItemData->line_edit->font();
						ft.setItalic(true);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::gray);
						theItemData->line_edit->setPalette(pal);
					}
					else
					{
						theItemData->line_edit->setReadOnly(false);
						ft = theItemData->line_edit->font();
						ft.setItalic(false);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::black);
						theItemData->line_edit->setPalette(pal);
					}
					theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));
					break;

				case JVX_DATAFORMAT_64BIT_LE:

					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset(true);
					res = theTriple.theProps->get_property(callGate, jPRG( &int64Val, 1, theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str()
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						theItemData->descriptor.isValid = true;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						txt = jvx_int2String(int64Val).c_str();
						theItemData->line_edit->setText(txt);
					}
					else
					{
						theItemData->descriptor.isValid = false;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						txt = "--";
					}

					if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
					{
						theItemData->line_edit->setReadOnly(true);
						ft = theItemData->line_edit->font();
						ft.setItalic(true);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::gray);
						theItemData->line_edit->setPalette(pal);
					}
					else
					{
						theItemData->line_edit->setReadOnly(false);
						ft = theItemData->line_edit->font();
						ft.setItalic(false);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::black);
						theItemData->line_edit->setPalette(pal);
					}
					theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));
					break;

				case JVX_DATAFORMAT_DATA:

					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset(true);

					res = theTriple.theProps->get_property(callGate, jPRG( &dblVal, 1, theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str(),
						//JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category), 
						theTriple.theProps))
					{
						theItemData->descriptor.isValid = true;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						txt = jvx_data2String(dblVal, JVX_NUM_DIGITS_PARAMS).c_str();
						theItemData->line_edit->setText(txt);
					}
					else
					{
						theItemData->descriptor.isValid = false;
						txt = "--";
					}

					if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
					{
						theItemData->line_edit->setReadOnly(true);
						ft = theItemData->line_edit->font();
						ft.setItalic(true);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::gray);
						theItemData->line_edit->setPalette(pal);
					}
					else
					{
						theItemData->line_edit->setReadOnly(false);
						ft = theItemData->line_edit->font();
						ft.setItalic(false);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::black);
						theItemData->line_edit->setPalette(pal);
					}
					theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));
					break;

				case JVX_DATAFORMAT_SIZE:
					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset(true);
					res = theTriple.theProps->get_property(callGate, jPRG( &intSz, 1, theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						theItemData->descriptor.isValid = true;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						txt = jvx_size2String(intSz).c_str();
						theItemData->line_edit->setText(txt);
					}
					else
					{
						theItemData->descriptor.isValid = false;
						txt = "--";
					}

					if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
					{
						theItemData->line_edit->setReadOnly(true);
						ft = theItemData->line_edit->font();
						ft.setItalic(true);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::gray);
						theItemData->line_edit->setPalette(pal);
					}
					else
					{
						theItemData->line_edit->setReadOnly(false);
						ft = theItemData->line_edit->font();
						ft.setItalic(false);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::black);
						theItemData->line_edit->setPalette(pal);
					}
					theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));
					break;
				case JVX_DATAFORMAT_STRING:
					callGate.s_push();
					callGate.context = &theItemData->ctxt;
					res = jvx_pullPropertyString(entry, theTriple.theObj, theTriple.theProps, theItemData->descriptor.globalIdx, 
						theItemData->descriptor.category, callGate);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str()
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						theItemData->descriptor.isValid = true;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						theItemData->line_edit->setText(entry.c_str());
					}
					else
					{
						theItemData->descriptor.isValid = false;
						txt = "--";
					}
					callGate.s_pop();

					if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
					{
						theItemData->line_edit->setReadOnly(true);
						ft = theItemData->line_edit->font();
						ft.setItalic(true);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::gray);
						theItemData->line_edit->setPalette(pal);
					}
					else
					{
						theItemData->line_edit->setReadOnly(false);
						ft = theItemData->line_edit->font();
						ft.setItalic(false);
						theItemData->line_edit->setFont(ft);
						pal = theItemData->line_edit->palette();
						pal.setColor(QPalette::Text, Qt::black);
						theItemData->line_edit->setPalette(pal);
					}
					theItemData->line_edit->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));					
					break;
				case JVX_DATAFORMAT_VALUE_IN_RANGE:

					// Only full update since we need to compute slider position
					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset();
					res = theTriple.theProps->get_property(callGate, jPRG( &valInRange, 1, theItemData->descriptor.format), 
						ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						//JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						theItemData->descriptor.isValid = true;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						txt = jvx_data2String(valInRange.minVal, -JVX_NUM_DIGITS_PARAMS).c_str();
						theItemData->label_min->setText(txt);
						txt = jvx_data2String(valInRange.maxVal, -JVX_NUM_DIGITS_PARAMS).c_str();
						theItemData->label_max->setText(txt);
						txt = jvx_data2String(valInRange.val(), -JVX_NUM_DIGITS_PARAMS).c_str();
						theItemData->line_edit->setText(txt);
						jvxData diff = valInRange.maxVal - valInRange.minVal;
						int idx = JVX_DATA2INT16((valInRange.val()- valInRange.minVal)/diff * 100.0);
						if((idx >= 0) && (idx <= 100))
						{
							theItemData->slider->setValue(idx);
						}
					}
					else
					{
						theItemData->descriptor.isValid = false;
						txt = "--";
					}

					if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
					{
						theItemData->slider->setEnabled(false);
					}
					else
					{
						theItemData->slider->setEnabled(true);
					}

					theItemData->slider->setToolTip(jvxPropertyAccessType_txt(theItemData->descriptor.accessType));
					break;

				case JVX_DATAFORMAT_STRING_LIST:

					// Nothing realized for this case
					break;

				case JVX_DATAFORMAT_SELECTION_LIST:

					if(fullUpdate)
					{
						requiresRedraw = true;
						res = JVX_ERROR_ABORT;
					}
					else
					{
						ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
						trans.reset(true);
						res = theTriple.theProps->get_property(callGate, jPRG( &selList, 1, theItemData->descriptor.format), ident, trans);
					}
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str()
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						theItemData->descriptor.isValid = true;
						if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_INVALID)
						{
							theItemData->descriptor.isValid = false;
						}

						switch(theItemData->descriptor.decTp)
						{
						case JVX_PROPERTY_DECODER_BITFIELD:
							if(selList.strList.ll() != theItemData->label_flags.size())
							{
								requiresRedraw = true;
							}
							else
							{
								for(i = 0; i < theItemData->label_flags.size(); i++)
								{
									if(jvx_bitTest(selList.bitFieldSelected(), i))
									{
										setBackgroundLabelColor(Qt::green, theItemData->label_flags[i]);
									}
									else
									{
										setBackgroundLabelColor(Qt::gray, theItemData->label_flags[i]);
									}
								}
							}

							break;

						case JVX_PROPERTY_DECODER_SINGLE_SELECTION:
							
							if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
							{
								pal = theItemData->combo_box->palette();
								pal.setColor(QPalette::Text, Qt::gray);
								theItemData->combo_box->setPalette(pal);
							}
							else
							{
								pal = theItemData->combo_box->palette();
								pal.setColor(QPalette::Text, Qt::black);
								theItemData->combo_box->setPalette(pal);
							}

							if(selList.strList.ll() != theItemData->combo_box->count())
							{
								requiresRedraw = true;
							}
							else
							{
								jvxSize firstEntry = JVX_SIZE_UNSELECTED;
								for(i = 0; i < (jvxSize)theItemData->combo_box->count(); i++)
								{
									QFont myCustomFont;

									if(jvx_bitTest(selList.bitFieldSelected(), i))
									{
										if(JVX_CHECK_SIZE_UNSELECTED( firstEntry))
											firstEntry = i;
										myCustomFont.setBold(true);
										myCustomFont.setItalic(false);
									}
									else
									{
										myCustomFont.setBold(false);
										myCustomFont.setItalic(true);
									}
#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(true);
#endif
									theItemData->combo_box->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(false);
#endif
								}
								if (JVX_CHECK_SIZE_SELECTED(firstEntry))
								{
									theItemData->combo_box->setCurrentIndex((int)firstEntry);
								}
								else
								{
									if(theItemData->combo_box->count())
										theItemData->combo_box->setCurrentIndex((int)0);
								}
							}
							break;
						case JVX_PROPERTY_DECODER_MULTI_SELECTION:
						case JVX_PROPERTY_DECODER_NONE:
							if(selList.strList.ll() != theItemData->combo_box->count())
							{
								requiresRedraw = true;
							}
							else
							{
								if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
								{
									pal = theItemData->combo_box->palette();
									pal.setColor(QPalette::Text, Qt::gray);
									theItemData->combo_box->setPalette(pal);
								}
								else
								{
									pal = theItemData->combo_box->palette();
									pal.setColor(QPalette::Text, Qt::black);
									theItemData->combo_box->setPalette(pal);
								}

								jvxSize firstEntry = JVX_SIZE_UNSELECTED;
								for (i = 0; i < (jvxSize)theItemData->combo_box->count(); i++)
								{
									QFont myCustomFont;
									if (jvx_bitTest(selList.bitFieldSelected(), i))
									{
										if (JVX_CHECK_SIZE_UNSELECTED(firstEntry))
											firstEntry = i;
										myCustomFont.setBold(true);
										myCustomFont.setItalic(false);
									}
									else
									{
										myCustomFont.setBold(false);
										myCustomFont.setItalic(true);
									}
#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(true);
#endif
									theItemData->combo_box->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(false);
#endif
								}
								if (JVX_CHECK_SIZE_SELECTED(firstEntry))
								{
									theItemData->combo_box->setCurrentIndex((int)firstEntry);
								}
								else
								{
									if (theItemData->combo_box->count())
										theItemData->combo_box->setCurrentIndex((int)0);
								}
							}

							break;

						case JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:
							if(selList.strList.ll() != theItemData->combo_box->count())
							{
								requiresRedraw = true;
							}
							else
							{
								if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
								{
									pal = theItemData->combo_box->palette();
									pal.setColor(QPalette::Text, Qt::gray);
									theItemData->combo_box->setPalette(pal);
								}
								else
								{
									pal = theItemData->combo_box->palette();
									pal.setColor(QPalette::Text, Qt::black);
									theItemData->combo_box->setPalette(pal);
								}

								for(i = 0; i < (jvxSize)theItemData->combo_box->count(); i++)
								{
									QFont myCustomFont;

									if(jvx_bitTest(selList.bitFieldSelected(), i))
									{
										myCustomFont.setBold(true);
										myCustomFont.setItalic(false);
									}
									else
									{
										myCustomFont.setBold(false);
										myCustomFont.setItalic(true);
									}

#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(true);
#endif
									theItemData->combo_box->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(false);
#endif

								}

								for(i = 0; i < (jvxSize)theItemData->combo_box->count(); i++)
								{
									const QColor color(Qt::white);
									const QModelIndex idx = theItemData->combo_box->model()->index((int)i, 0);
									theItemData->combo_box->model()->setData(idx, color, Qt::BackgroundRole);
								}

								theItemData->button_3->setEnabled(false);
								theItemData->button_4->setEnabled(false);
								if(JVX_CHECK_SIZE_SELECTED(theItemData->selection) && (theItemData->selection < (jvxInt16)selList.strList.ll()))
								{
									theItemData->button_3->setEnabled(true);
									theItemData->button_4->setEnabled(true);
									theItemData->combo_box->setCurrentIndex((int)theItemData->selection);
									const QColor color(Qt::red);
									const QModelIndex idx = theItemData->combo_box->model()->index((int)theItemData->selection, 0);
									theItemData->combo_box->model()->setData(idx, color, Qt::BackgroundRole);

								}
							}
							break;

						case JVX_PROPERTY_DECODER_INPUT_FILE_LIST:
							if(selList.strList.ll() != theItemData->combo_box->count())
							{
								requiresRedraw = true;
							}
							else
							{
								if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
								{
									pal = theItemData->combo_box->palette();
									pal.setColor(QPalette::Text, Qt::gray);
									theItemData->combo_box->setPalette(pal);
								}
								else
								{
									pal = theItemData->combo_box->palette();
									pal.setColor(QPalette::Text, Qt::black);
									theItemData->combo_box->setPalette(pal);
								}

								for(i = 0; i < (jvxSize)theItemData->combo_box->count(); i++)
								{
									QFont myCustomFont;

									myCustomFont.setBold(false);
									myCustomFont.setItalic(false);
#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(true);
#endif
									theItemData->combo_box->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(false);
#endif
								}
							}

							for(i = 0; i < (jvxSize)theItemData->combo_box->count(); i++)
							{
								const QColor color(Qt::white);
								const QModelIndex idx = theItemData->combo_box->model()->index((int)i, 0);
								theItemData->combo_box->model()->setData(idx, color, Qt::BackgroundRole);
							}
							if(JVX_CHECK_SIZE_SELECTED(theItemData->selection) && (theItemData->selection < (jvxInt16)selList.strList.ll()))
							{
								theItemData->combo_box->setCurrentIndex((int)theItemData->selection);
								const QColor color(Qt::red);
								const QModelIndex idx = theItemData->combo_box->model()->index((int)theItemData->selection, 0);
								theItemData->combo_box->model()->setData(idx, color, Qt::BackgroundRole);
							}
							break;

						case JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST:
							if(selList.strList.ll() != theItemData->combo_box->count())
							{
								requiresRedraw = true;
							}
							else
							{
								if (JVX_CHECK_STATUS_PROPERTY_ONLY_READ(theItemData->descriptor.accessType))
								{
									pal = theItemData->combo_box->palette();
									pal.setColor(QPalette::Text, Qt::gray);
									theItemData->combo_box->setPalette(pal);
								}
								else
								{
									pal = theItemData->combo_box->palette();
									pal.setColor(QPalette::Text, Qt::black);
									theItemData->combo_box->setPalette(pal);
								}

								for(i = 0; i < (jvxSize)theItemData->combo_box->count(); i++)
								{
									QFont myCustomFont;

									myCustomFont.setBold(false);
									myCustomFont.setItalic(false);
#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(true);
#endif
									theItemData->combo_box->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
									theItemData->combo_box->setEditable(false);
#endif
								}
							}
							for(i = 0; i < (jvxSize)theItemData->combo_box->count(); i++)
							{
								const QColor color(Qt::white);
								const QModelIndex idx = theItemData->combo_box->model()->index((int)i, 0);
								theItemData->combo_box->model()->setData(idx, color, Qt::BackgroundRole);
							}
							if(JVX_CHECK_SIZE_SELECTED(theItemData->selection) && (theItemData->selection < (jvxInt16)selList.strList.ll()))
							{
								theItemData->combo_box->setCurrentIndex((int)theItemData->selection);
								const QColor color(Qt::red);
								const QModelIndex idx = theItemData->combo_box->model()->index((int)theItemData->selection, 0);
								theItemData->combo_box->model()->setData(idx, color, Qt::BackgroundRole);
							}
							break;

						default:
							assert(0);
						}
					}
					else
					{
						theItemData->descriptor.isValid = false;
						break;
					}
					break;
				}//switch(propParams.format)

				if (theItemData->descriptor.isValid)
				{
					setBackgroundLabelColor(Qt::green, theItemData->label_valid);
				}
				else
				{
					setBackgroundLabelColor(Qt::red, theItemData->label_valid);
				}

				theItemData->combo_box_access_what->setCurrentIndex(theItemData->sel_access);

				jvxAccessRightFlags_rwcd access_rights_rwcd = 0;
				jvxConfigModeFlags config_mode_flags = 0;
				int idxProps = 0;
				res = theTriple.theProps->get_meta_flags(callGate,
					&access_rights_rwcd, &config_mode_flags,
					jPAGID(theItemData->descriptor.globalIdx, theItemData->descriptor.category));
				if (res == JVX_NO_ERROR)
				{
					if (theItemData->combo_box_access_rwcd)
					{
						jvxAccessRightFlags access_rcwd = JVX_EXTRACT_READ_FLAGS(access_rights_rwcd);
						switch (theItemData->sel_access)
						{
						case 0:
							access_rcwd = JVX_EXTRACT_READ_FLAGS(access_rights_rwcd);
							break;
						case 1:
							access_rcwd = JVX_EXTRACT_WRITE_FLAGS(access_rights_rwcd);
							break;
						case 2:
							access_rcwd = JVX_EXTRACT_CREATE_FLAGS(access_rights_rwcd);
							break;
						case 3:
							access_rcwd = JVX_EXTRACT_DESTROY_FLAGS(access_rights_rwcd);
							break;
						}
						QFont myCustomFont;
						jvxSize firstEntry = JVX_SIZE_UNSELECTED;
						for (i = 0; i < 16; i++)
						{
							if ((1 << i) & access_rcwd)
							{
								if (JVX_CHECK_SIZE_UNSELECTED(firstEntry))
									firstEntry = i;
								myCustomFont.setBold(true);
								myCustomFont.setItalic(false);
							}
							else
							{
								myCustomFont.setBold(false);
								myCustomFont.setItalic(true);
							}
#ifdef JVX_OS_LINUX
							theItemData->combo_box_access_rwcd->setEditable(true);
#endif
							theItemData->combo_box_access_rwcd->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
							theItemData->combo_box_access_rwcd->setEditable(false);
#endif
						}
						if (JVX_CHECK_SIZE_SELECTED(firstEntry))
						{
							theItemData->combo_box_access_rwcd->setCurrentIndex(firstEntry);
						}
						else
						{
							if (theItemData->combo_box_access_rwcd->count())
								theItemData->combo_box_access_rwcd->setCurrentIndex((int)0);
						}
					}

					if(theItemData->combo_box_config)
					{
						QFont myCustomFont;
						jvxSize firstEntry = JVX_SIZE_UNSELECTED;
						for (i = 0; i < 4; i++)
						{
							if ((1 << i) & config_mode_flags)
							{
								if (JVX_CHECK_SIZE_UNSELECTED(firstEntry))
									firstEntry = i;
								myCustomFont.setBold(true);
								myCustomFont.setItalic(false);
							}
							else
							{
								myCustomFont.setBold(false);
								myCustomFont.setItalic(true);
							}
#ifdef JVX_OS_LINUX
							theItemData->combo_box_config->setEditable(true);
#endif
							theItemData->combo_box_config->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
							theItemData->combo_box_config->setEditable(false);
#endif
						}
						if (JVX_CHECK_SIZE_SELECTED(firstEntry))
						{
							theItemData->combo_box_config->setCurrentIndex(firstEntry);
						}
						else
						{
							if (theItemData->combo_box_config->count())
								theItemData->combo_box_config->setCurrentIndex((int)0);
						}						
					}
				}

			}//if(jvxApplyPropertyFilter(propParams.format, propParams.num, propParams.decTp, JVX_PROPERTIES_FILTER_PROPERTIES_RTV))
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);

		}//if((res == JVX_NO_ERROR) && theTriple.theProps)
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));

	}//if(theItemData)
	// If really a lot has changed, redraw the complete widget
	
	/*
	if(requiresRedraw)
	{
		this->local.parentRef->askForCompleteRebuild();
		references.report->report_simple_text_message("Request to redraw realtime viewer properties", JVX_REPORT_PRIORITY_INFO);
	}*/
}

void
realtimeViewerOneProperty::updateWindow_update()
{
	bool update = false;
	references.theRef->_update_item_in_group_in_section(
		references.sectionId, references.groupId, references.itemId, &update);
	this->checkBox->setChecked(update);
}

void
realtimeViewerOneProperty::newSelection_update(bool checked)
{
	this->references.theRef->_set_update_item_in_group_in_section(
		references.sectionId, references.groupId, references.itemId, checked);
	updateWindow_update();

}

void
realtimeViewerOneProperty::buttonPushed_remove()
 {
	 local.parentRef->removeMe_delayed(references.sectionId, references.groupId, references.itemId);
 }

void
realtimeViewerOneProperty::newSelection_combo(jvxBitField bitset, jvxBool startedWithRightClick, jvxSize mode)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;
	jvxSelectionList theList;
	jvxSelectionList theOldList;
	jvxSize newSelection = 0;
	jvxSize i;
	jvxBool requiresRedraw = false;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	jvx_initPropertyReferenceTriple(&theTriple);
	jvxBool isValid = false;

	theList.bitFieldSelected() = bitset;

	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
		if(itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			if (mode)
			{
				jvxAccessRightFlags_rwcd access_rights_rwcd = 0;
				jvxAccessRightFlags_rwcd tt, tt2 = 0;
				jvxAccessRightFlags access_rights = 0;
				jvxConfigModeFlags config_mode_flags = 0;
				jvxSize idx = jvx_bitfieldSelection2Id(bitset, JVX_NUM_ENTRIES_FLAGTAG);
				if (JVX_CHECK_SIZE_SELECTED(idx))
				{
					if (mode == 3)
					{
						theItemData->sel_access = theItemData->combo_box_access_what->currentIndex();
					}
					else
					{
						res = theTriple.theProps->get_meta_flags(callGate, 
							&access_rights_rwcd, &config_mode_flags,
							jPAGID(theItemData->descriptor.globalIdx, theItemData->descriptor.category));

						switch (mode)
						{
						case 1:
							switch (theItemData->sel_access)
							{
							case 0:
								access_rights = JVX_EXTRACT_READ_FLAGS(access_rights_rwcd);
								break;
							case 1:
								access_rights = JVX_EXTRACT_WRITE_FLAGS(access_rights_rwcd);
								break;
							case 2:
								access_rights = JVX_EXTRACT_CREATE_FLAGS(access_rights_rwcd);
								break;
							case 3:
								access_rights = JVX_EXTRACT_DESTROY_FLAGS(access_rights_rwcd);
								break;
							}

							if (startedWithRightClick)
							{
								access_rights = ~access_rights;
							}
							else
							{
								access_rights = access_rights ^ ((jvxAccessRightFlags)1 << idx);
							}
							switch (theItemData->sel_access)
							{
							case 0:
								access_rights_rwcd = JVX_REPLACE_READ_FLAGS(access_rights_rwcd, access_rights);
								break;
							case 1:
								access_rights_rwcd = JVX_REPLACE_WRITE_FLAGS(access_rights_rwcd, access_rights);
								break;
							case 2:
								access_rights_rwcd = JVX_REPLACE_CREATE_FLAGS(access_rights_rwcd, access_rights);
								break;
							case 3:
								access_rights_rwcd = JVX_REPLACE_DESTROY_FLAGS(access_rights_rwcd, access_rights);
								break;
							}
							break;
						case 2:
							if (startedWithRightClick)
							{
								config_mode_flags = ~config_mode_flags;
							}
							else
							{
								config_mode_flags = config_mode_flags ^ ((jvxAccessRightFlags)1 << idx);
							}
							break;

						}
						res = theTriple.theProps->set_meta_flags(callGate,
							&access_rights_rwcd, &config_mode_flags,
							jPAGID(theItemData->descriptor.globalIdx, theItemData->descriptor.category));
					}
				}
			}
			else
			{
				if (theItemData->descriptor.format == JVX_DATAFORMAT_SELECTION_LIST)
				{
					switch (theItemData->descriptor.decTp)
					{
					case JVX_PROPERTY_DECODER_SINGLE_SELECTION:
						ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
						trans.reset(true);
						res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1, theItemData->descriptor.format), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
							(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
							//JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
							, theTriple.theProps))
						{
							jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
								theItemData->descriptor.globalIdx,
								theItemData->descriptor.category);
							jvx::propertyDetail::CjvxTranferDetail trans(true);

							res = theTriple.theProps->set_property(callGate,
								jPRG(&theList, 1, theItemData->descriptor.format), 
								ident, trans);
						}
						break;
					case JVX_PROPERTY_DECODER_MULTI_SELECTION:

						ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
						trans.reset(true);
						res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1, theItemData->descriptor.format), ident, trans);
						if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
							(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str()
							// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx,theItemData->descriptor.category)
							, theTriple.theProps))
						{
							// This includes the toggle

							jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
								theItemData->descriptor.globalIdx, 
								theItemData->descriptor.category);
							jvx::propertyDetail::CjvxTranferDetail trans(true);

							theList.bitFieldSelected() = theList.bitFieldSelected() ^ theOldList.bitFieldSelected();
							res = theTriple.theProps->set_property(callGate,
								jPRG(&theList, 1, theItemData->descriptor.format),
								ident, trans);
						}
						break;
					case JVX_PROPERTY_DECODER_INPUT_FILE_LIST:

						if (startedWithRightClick)
						{
							newSelection = JVX_SIZE_UNSELECTED;
							for (i = 0; i < sizeof(jvxBitField) * 8; i++)
							{
								if (jvx_bitTest(bitset, i))
								{
									newSelection = i;
									break;
								}
							}

							if (theItemData->selection == newSelection)
							{
								theItemData->selection = JVX_SIZE_UNSELECTED;
							}
							else
							{
								theItemData->selection = newSelection;
							}
						}
						else
						{
							theItemData->selection = JVX_SIZE_UNSELECTED;
						}
						break;
					case JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST:

						if (startedWithRightClick)
						{
							newSelection = JVX_SIZE_UNSELECTED;
							for (i = 0; i < sizeof(jvxBitField) * 8; i++)
							{
								if (jvx_bitTest(bitset, i))
								{
									newSelection = i;
									break;
								}
							}
							if (theItemData->selection == newSelection)
							{
								theItemData->selection = JVX_SIZE_UNSELECTED;
							}
							else
							{
								theItemData->selection = newSelection;
							}
						}
						else
						{
							theItemData->selection = JVX_SIZE_UNSELECTED;
						}
						break;
					case JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:

						if (startedWithRightClick)
						{
							newSelection = JVX_SIZE_UNSELECTED;
							for (i = 0; i < sizeof(jvxBitField) * 8; i++)
							{
								if (jvx_bitTest(bitset, i))
								{
									newSelection = i;
									break;
								}
							}
							if (theItemData->selection == newSelection)
							{
								theItemData->selection = JVX_SIZE_UNSELECTED;
							}
							else
							{
								theItemData->selection = newSelection;
							}
						}
						else
						{
							ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
							trans.reset(true);
							res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1, theItemData->descriptor.format), ident, trans);
							if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
								(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str()
								// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx,theItemData->descriptor.category)
								, theTriple.theProps))
							{
								ident.reset(
									theItemData->descriptor.globalIdx,
									theItemData->descriptor.category);
								trans.reset(true);

								// This includes the toggle
								theList.bitFieldSelected() = theList.bitFieldSelected() ^ theOldList.bitFieldSelected();
								res = theTriple.theProps->set_property(callGate,
									jPRG(&theList, 1, theItemData->descriptor.format)
									, ident, trans);
							}
							theItemData->selection = JVX_SIZE_UNSELECTED;
						}
						break;
					default:
						std::cout << "Triggered selection list property of type <" << jvxPropertyDecoderHintType_txt(theItemData->descriptor.decTp) <<
							">, probably, you have forgotten to specify a valid decoder hint type." << std::endl;
					}
				}
			}
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}
		updateWindow_contents(requiresRedraw);
		if(requiresRedraw)
		{
			cleanContent();
			updateWindow_construct();
		}
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::newPosition_slider(int val)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;
	jvx_initPropertyReferenceTriple(&theTriple);
	jvxValueInRange valRange;
	jvxBool isValid = false;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
		if(itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			if(theItemData->descriptor.format == JVX_DATAFORMAT_VALUE_IN_RANGE)
			{
				ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
				trans.reset();
				res = theTriple.theProps->get_property(callGate, jPRG( &valRange, 1,theItemData->descriptor.format), ident, trans);
				if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
					(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
					// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
					, theTriple.theProps))
				{
					jvxData diff = valRange.maxVal - valRange.minVal;
					jvxData newVal = (jvxData)val/100.0 * diff + valRange.minVal;
					valRange.val() = newVal;

					jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
						theItemData->descriptor.globalIdx,
						theItemData->descriptor.category);
					jvx::propertyDetail::CjvxTranferDetail trans(true);

					theItemData->line_edit->setText(jvx_data2String(newVal, JVX_NUM_DIGITS_PARAMS).c_str());
					res = theTriple.theProps->set_property(callGate,
						jPRG(&valRange, 1,theItemData->descriptor.format), 
						ident, trans);
				}
			}
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}
		updateWindow_update();
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void 
realtimeViewerOneProperty::toggled_checkBox(bool checkState)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;
	jvx_initPropertyReferenceTriple(&theTriple);
	//	jvxString str;
	std::string strC;

	jvxInt8 int8Val = 0;
	jvxInt16 int16Val = 0;
	jvxInt32 int32Val = 0;
	jvxInt64 int64Val = 0;
	jvxData dblVal = 0;
	jvxData fltVal = 0;

	jvxValueInRange valRange;
	jvxCallManagerProperties callGate;

	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if (theItemData)
	{
		if (itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		if ((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			switch (theItemData->descriptor.format)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				if (theItemData->descriptor.decTp == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
				{
					jvxInt16 val = c_false;
					if (checkState)
						val = c_true;

					jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
						theItemData->descriptor.globalIdx,
						theItemData->descriptor.category);
					jvx::propertyDetail::CjvxTranferDetail trans(true);

					res = theTriple.theProps->set_property(callGate,
						jPRG(&val, 1, theItemData->descriptor.format), 
						ident, trans);
				}
				else
				{
					assert(0);
				}
				break;
			default:
				assert(0);
			}
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}
		updateWindow_update();
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::newText_lineEdit(QString txt, jvxBool& doReset)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;
	jvx_initPropertyReferenceTriple(&theTriple);
//	jvxString str;
	std::string strC;

	jvxInt8 int8Val = 0;
	jvxInt16 int16Val = 0;
	jvxInt32 int32Val = 0;
	jvxSize intSz = 0;
	jvxInt64 int64Val = 0;
	jvxData dblVal = 0;
	jvxData fltVal = 0;

	jvxValueInRange valRange;
	jvxCallManagerProperties callGate;

	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
		if(itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
				theItemData->descriptor.globalIdx,
				theItemData->descriptor.category);
			jvx::propertyDetail::CjvxTranferDetail trans(true);

			switch(theItemData->descriptor.format)
			{
			case JVX_DATAFORMAT_8BIT:
				int8Val = txt.toInt();
				res = theTriple.theProps->set_property(callGate,
					jPRG(&int8Val, 1,theItemData->descriptor.format), 
					ident, trans);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				int16Val = txt.toInt();
				res = theTriple.theProps->set_property(callGate,
					jPRG(&int16Val, 1,theItemData->descriptor.format), 
					ident, trans);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				int32Val = txt.toInt();
				res = theTriple.theProps->set_property(callGate,
					jPRG(&int32Val, 1,theItemData->descriptor.format), 
					ident, trans);
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				int64Val = txt.toInt();
				res = theTriple.theProps->set_property(callGate,
					jPRG(&int64Val, 1,theItemData->descriptor.format), 
					ident, trans);
				break;
			case JVX_DATAFORMAT_SIZE:
				if (txt == "unselected")
				{
					intSz = JVX_SIZE_UNSELECTED;
				}
				intSz = txt.toInt();
				res = theTriple.theProps->set_property(callGate,
					jPRG(&intSz, 1, theItemData->descriptor.format), 
					ident, trans);
				break;
			case JVX_DATAFORMAT_DATA:
				dblVal = txt.toData();
				res = theTriple.theProps->set_property(callGate,
					jPRG(&dblVal, 1,theItemData->descriptor.format), 
					ident, trans);
				break;
			case JVX_DATAFORMAT_STRING:
				strC = txt.toLatin1().constData();
				jvx_pushPropertyString(strC, theTriple.theObj, theTriple.theProps, theItemData->descriptor.globalIdx, theItemData->descriptor.category, 
					callGate);
				if (theItemData->descriptor.accessType == JVX_PROPERTY_ACCESS_WRITE_ONLY)
				{
					doReset = true;
				}
				break;
			case JVX_DATAFORMAT_VALUE_IN_RANGE:
				valRange.val() = txt.toData();
				res = theTriple.theProps->set_property(callGate,
					jPRG(&valRange, 1,theItemData->descriptor.format), 
					ident, trans);
				break;
			default:
				assert(0);
			}
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}
		updateWindow_update();
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::updatePropertyIfValid(jvxSize sectionId, jvxSize groupId, jvxSize itemId)
{
	jvxBool requiresRedraw = false;
	if(
		(sectionId == references.sectionId) &&
		(groupId == references.groupId) &&
		(itemId == references.itemId))
	{
		updateWindow_contents(requiresRedraw);
		if(requiresRedraw)
		{
			deleteUiElementsInWidget();
			cleanContent();
			updateWindow_construct();
		}
	}
}

void
realtimeViewerOneProperty::button_select_all()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;
	jvxSelectionList theList;
	jvxSelectionList theOldList;
	std::vector<std::string> lstNew;
	jvx_initPropertyReferenceTriple(&theTriple);
	theList.bitFieldSelected() = 0;
	jvxBool requiresRedraw = false;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
		if(itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			if(theItemData->descriptor.format == JVX_DATAFORMAT_SELECTION_LIST)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
					theItemData->descriptor.globalIdx,
					theItemData->descriptor.category);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				switch(theItemData->descriptor.decTp)
				{
				case JVX_PROPERTY_DECODER_MULTI_SELECTION:
				case JVX_PROPERTY_DECODER_NONE:

					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset(true);
					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx,theItemData->descriptor.category)
						, theTriple.theProps))
					{
						// This includes the toggle
						theList.bitFieldSelected() = ((jvxBitField)1 << (jvxUInt32)theOldList.strList.ll())-1;
						res = theTriple.theProps->set_property(callGate,
							jPRG(&theList, 1,theItemData->descriptor.format), 
							ident, trans);
					}
					break;

				case JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:

					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset(true);
					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format), 
						ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						// This includes the toggle
						theList.bitFieldSelected() = ((jvxBitField)1 << (jvxUInt32)theOldList.strList.ll())-1;
						res = theTriple.theProps->set_property(callGate,
							jPRG(&theList, 1,theItemData->descriptor.format), 
							ident, trans);
					}
					break;
				case JVX_PROPERTY_DECODER_INPUT_FILE_LIST:

					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset();
					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						for (i = 0; i < theOldList.strList.ll(); i++)
						{
							lstNew.push_back(theOldList.strList.std_str_at(i));
						}

						QStringList lst = QFileDialog::getOpenFileNames(this, tr("Open Input File"), tr(".\\"), tr("(*.*)"));

						for (i = 0; i < (jvxSize)lst.size(); i++)
						{
							std::string fileName = (std::string)lst[(int)i].toLatin1().constData();
							fileName = jvx_replaceCharacter(fileName, '/', (JVX_SEPARATOR_DIR)[0]);
							fileName = jvx_absoluteToRelativePath(fileName, true);

							lstNew.push_back(fileName);
						}

						theList.bitFieldSelected() = ((jvxBitField)1 << (jvxUInt32)lstNew.size()) - 1;
						theList.strList.assign(lstNew);
						trans.contentOnly = false;
						res = theTriple.theProps->set_property(callGate,
							jPRG(&theList, 1, theItemData->descriptor.format), 
							ident, trans);
					}
					
					break;
				default:
					assert(0);
				}
			}
			else
			{
				assert(0);
			}
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}
		updateWindow_contents(requiresRedraw);
		if(requiresRedraw)
		{
			cleanContent();
			updateWindow_construct();
		}
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::button_unselect_all()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;
	jvxSelectionList theList;
	jvxSelectionList theOldList;
	std::vector<std::string> lstNew;
	jvx_initPropertyReferenceTriple(&theTriple);
	jvxBool isValid = false;
	jvxBool isReadOnly = false;
	theList.bitFieldSelected() = 0;
	jvxBool requiresRedraw = false;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
		if(itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			if(theItemData->descriptor.format == JVX_DATAFORMAT_SELECTION_LIST)
			{
				ident.reset(
					theItemData->descriptor.globalIdx,
					theItemData->descriptor.category);
				trans.reset(true);

				switch(theItemData->descriptor.decTp)
				{
				case JVX_PROPERTY_DECODER_MULTI_SELECTION:

					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset(true);
					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						// This includes the toggle
						theList.bitFieldSelected() = 0;
						res = theTriple.theProps->set_property(callGate,
							jPRG(&theList, 1,theItemData->descriptor.format), 
							ident, trans);
					}
					break;

				case JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:

					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset(true);
					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format),ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						// This includes the toggle
						theList.bitFieldSelected() = 0;
						res = theTriple.theProps->set_property(callGate,
							jPRG(&theList, 1,theItemData->descriptor.format), 
							ident, trans);
					}
					break;

				case JVX_PROPERTY_DECODER_INPUT_FILE_LIST:

					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset();
					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						if (JVX_CHECK_SIZE_SELECTED(theItemData->selection))
						{
							for (i = 0; i < theOldList.strList.ll(); i++)
							{
								if (i != theItemData->selection)
								{
									lstNew.push_back(theOldList.strList.std_str_at(i));
								}
							}
							theItemData->selection = JVX_SIZE_UNSELECTED;
						}

						theList.bitFieldSelected() = (jvxBitField)-1;
						theList.strList.assign(lstNew);
						trans.contentOnly = false;
						res = theTriple.theProps->set_property(callGate,
							jPRG(&theList, 1, theItemData->descriptor.format),ident, trans);
					}
					break;
				case JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST:
					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset(false);
					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format),ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						if (JVX_CHECK_SIZE_SELECTED(theItemData->selection))
						{
							for (i = 0; i < theOldList.strList.ll(); i++)
							{
								if (i != theItemData->selection)
								{
									lstNew.push_back(theOldList.strList.std_str_at(i));
								}
							}
							theItemData->selection = JVX_SIZE_UNSELECTED;
						}

						theList.bitFieldSelected() = (jvxBitField)-1;
						theList.strList.assign(lstNew);
						trans.contentOnly = false;
						res = theTriple.theProps->set_property(callGate,
							jPRG(&theList, 1, theItemData->descriptor.format), 
							ident, trans);
					}
					break;
				default:
					assert(0);
				}
			}
			else
			{
				assert(0);
			}
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}
		updateWindow_contents(requiresRedraw);
		if(requiresRedraw)
		{
			cleanContent();
			updateWindow_construct();
		}
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::button_up()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;
	jvxCallManagerProperties callGate;
	jvx_initPropertyReferenceTriple(&theTriple);
	jPAGID ident;
	jPD trans;
	jvxBool isValid = false;
	jvxBool isReadOnly = false;
	std::vector<std::string> replaceList;
	jvxSelectionList theOldList;
	jvxSelectionList theNewList;
	theNewList.bitFieldSelected() = 0;

	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);

	if(theItemData)
	{
		if(itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			if(theItemData->descriptor.format == JVX_DATAFORMAT_SELECTION_LIST)
			{
				switch(theItemData->descriptor.decTp)
				{
				case JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:
					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset();
					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						if(JVX_CHECK_SIZE_SELECTED(theItemData->selection) && (theItemData->selection >= 1) && (theItemData->selection < (jvxInt16)theOldList.strList.ll()))
						{
							jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
								theItemData->descriptor.globalIdx,
								theItemData->descriptor.category);
							jvx::propertyDetail::CjvxTranferDetail trans(false);

							theNewList.bitFieldSelected() = 0;
							theNewList.bitFieldExclusive = 0;

							for(i = 0; i < theOldList.strList.ll(); i++)
							{
								if(i == theItemData->selection)
								{
									replaceList.push_back(theOldList.strList.std_str_at(theItemData->selection-1));
									if( jvx_bitTest(theOldList.bitFieldSelected(), (theItemData->selection-1)))
									{
										theNewList.bitFieldSelected()  |=  (jvxBitField)1 << ((jvxUInt32)theItemData->selection);
									}
									if( jvx_bitTest(theOldList.bitFieldExclusive, (theItemData->selection-1)))
									{
										theNewList.bitFieldExclusive  |=  (jvxBitField)1 << ((jvxUInt32)theItemData->selection);
									}
								}
								else
								{
									if(i == theItemData->selection-1)
									{
										replaceList.push_back(theOldList.strList.std_str_at(theItemData->selection));
										if( jvx_bitTest(theOldList.bitFieldSelected(), (theItemData->selection)))
										{
											theNewList.bitFieldSelected()  |=  (jvxBitField)1 << ((jvxUInt32)theItemData->selection-1);
										}
										if( jvx_bitTest(theOldList.bitFieldExclusive, (theItemData->selection)))
										{
											theNewList.bitFieldExclusive  |=  (jvxBitField)1 << ((jvxUInt32)theItemData->selection-1);
										}
									}
									else
									{
										replaceList.push_back(theOldList.strList.std_str_at(i));
										theNewList.bitFieldSelected()  |= theOldList.bitFieldSelected() & ((jvxBitField)1 << ((jvxUInt32)i));
										theNewList.bitFieldExclusive |= theOldList.bitFieldSelected() & ((jvxBitField)1 << ((jvxUInt32)i));
									}
								}
							}
							theItemData->selection--;
							theNewList.strList.assign(replaceList);
							res = theTriple.theProps->set_property(callGate,
								jPRG(&theNewList, 1,theItemData->descriptor.format), 
								ident, trans);
						}
					}

					break;
				default:
					assert(0);
				}
			}
			else
			{
				assert(0);
			}
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}
		cleanContent();
		updateWindow_construct();

		//this->local.parentRef->updateWindow_redraw_addgroup(references.groupId);
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));
	}
}

void
realtimeViewerOneProperty::button_down()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;

	jvx_initPropertyReferenceTriple(&theTriple);
	jvxBool isValid = false;
	jvxBool isReadOnly = false;
	std::vector<std::string> replaceList;
	jvxSelectionList theOldList;
	jvxSelectionList theNewList;
	theNewList.bitFieldSelected() = 0;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;

	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);

	if(theItemData)
	{
		if(itemProps.tpV.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			if(theItemData->descriptor.format == JVX_DATAFORMAT_SELECTION_LIST)
			{
				switch(theItemData->descriptor.decTp)
				{
				case JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:
					ident.reset(theItemData->descriptor.globalIdx, theItemData->descriptor.category);
					trans.reset();
					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format), ident, trans);
					if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, 
						(theItemData->descriptor.objDescription + theItemData->descriptor.descriptor.std_str()).c_str() 
						// JVX_PROPERTY_DESCRIBE_IDX_CAT(theItemData->descriptor.globalIdx, theItemData->descriptor.category)
						, theTriple.theProps))
					{
						if((JVX_CHECK_SIZE_SELECTED(theItemData->selection)) && (theItemData->selection < (jvxInt16)(theOldList.strList.ll()-1)))
						{
							ident.reset(
								theItemData->descriptor.globalIdx,
								theItemData->descriptor.category);
							trans.reset(false);

							theNewList.bitFieldSelected() = 0;
							theNewList.bitFieldExclusive = 0;

							for(i = 0; i < theOldList.strList.ll(); i++)
							{
								if(i == theItemData->selection)
								{
									replaceList.push_back(theOldList.strList.std_str_at(theItemData->selection+1));
									if( jvx_bitTest(theOldList.bitFieldSelected(),(theItemData->selection+1)))
									{
										theNewList.bitFieldSelected()  |=  (jvxBitField)1 << ((jvxUInt32)theItemData->selection);
									}
									if(jvx_bitTest( theOldList.bitFieldExclusive, (theItemData->selection+1)))
									{
										theNewList.bitFieldExclusive  |=  (jvxBitField)1 << ((jvxUInt32)theItemData->selection);
									}
								}
								else
								{
									if(i == theItemData->selection+1)
									{
										replaceList.push_back(theOldList.strList.std_str_at(theItemData->selection));
										if( jvx_bitTest(theOldList.bitFieldSelected(), (theItemData->selection)))
										{
											theNewList.bitFieldSelected()  |=  (jvxBitField)1 << ((jvxUInt32)theItemData->selection+1);
										}
										if(jvx_bitTest( theOldList.bitFieldExclusive, (theItemData->selection)))
										{
											theNewList.bitFieldExclusive  |=  (jvxBitField)1 << ((jvxUInt32)theItemData->selection+1);
										}
									}
									else
									{
										replaceList.push_back(theOldList.strList.std_str_at(i));
										theNewList.bitFieldSelected()  |= theOldList.bitFieldSelected() & ((jvxBitField)1 << ((jvxUInt32)i));
										theNewList.bitFieldExclusive |= theOldList.bitFieldSelected() & ((jvxBitField)1 << ((jvxUInt32)i));
									}
								}
							}
							theItemData->selection++;
							theNewList.strList.assign(replaceList);
							res = theTriple.theProps->set_property(callGate,
								jPRG(&theNewList, 1,theItemData->descriptor.format),
								ident, trans);
						}
					}

					break;
				default:
					assert(0);
				}
			}
			else
			{
				assert(0);
			}
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tpV);
		}

		cleanContent();
		updateWindow_construct();

		//this->local.parentRef->updateWindow_redraw_addgroup(references.groupId);
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));
	}
}

/*
void
realtimeViewerOneProperty::checkbox_toggled(bool tog)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvx_propertyReferenceTriple theTriple;
	jvxRealtimeViewerPropertyItem* theItemData = NULL;
	jvxSelectionList theOldList;
	jvxStringList theStrList;
	theOldList.strList = &theStrList;

	jvx_initPropertyReferenceTriple(&theTriple);
	jvxBool isValid = false;

	references.theRef->_request_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId,
		reinterpret_cast<jvxHandle**>(&theItemData), NULL, NULL);
	if(theItemData)
	{
		if(itemProps.tp != JVX_COMPONENT_UNKNOWN)
		{
			res = jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tp);
		}

		if((res == JVX_NO_ERROR) && theTriple.theProps)
		{
			if(theItemData->descriptor.format == JVX_DATAFORMAT_SELECTION_LIST)
			{
				switch(theItemData->descriptor.decTp)
				{
				case JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:

					res = theTriple.theProps->get_property(callGate, jPRG( &theOldList, 1,theItemData->descriptor.format, theItemData->descriptor.globalIdx, theItemData->descriptor.category, 0, true, &isValid, &theItemData->descriptor.accessType);
					if(isValid && (theItemData->descriptor.accessType != JVX_PROPERTY_ACCESS_READ_ONLY))
					{
						if((theItemData->selection >= 0) && (theItemData->selection < theOldList.strList.ll()))
						{
							// This includes the toggle
							if(tog)
							{
								theOldList.bitFieldSelected() |= ((jvxBitField)1 << theItemData->selection);
							}
							else
							{
								theOldList.bitFieldSelected() = theOldList.bitFieldSelected() ^ ((jvxBitField)1 << theItemData->selection);
							}
						}
						res = theTriple.theProps->set_property(callGate,&theOldList, 1,theItemData->descriptor.format, theItemData->descriptor.globalIdx, theItemData->descriptor.category, 0, true);
					}
					break;
				default:
					assert(0);
				}
			}
			res = jvx_returnReferencePropertiesObject(references.theHostRef, &theTriple, itemProps.tp);
		}
		updateWindow_contents();
		references.theRef->_return_property_item_in_group_in_section(references.sectionId, references.groupId, references.itemId, reinterpret_cast<jvxHandle*>(theItemData));
	}
}
*/
