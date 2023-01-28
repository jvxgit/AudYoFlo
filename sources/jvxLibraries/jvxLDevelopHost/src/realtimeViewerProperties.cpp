#include "realtimeViewer_helpers.h"
#include "realtimeViewerProperties.h"
#include "realtimeViewerOneProperty.h"

#include "jvx-helpers.h"
#include "uMainWindow_defines.h"
#include "jvx-qt-helpers.h"
#include <QScrollBar>

#define REPORT_ERROR(hdlOut, txt) hdlOut->postMessage_inThread(txt, JVX_QT_COLOR_ERROR);

/**
 * Set the margins for shown properties.
 *///===========================================================
static void
setMargins(QLayout* layout)
{
	QMargins marg;
	marg.setTop(JVX_MARGIN_RT_VIEWER_TOP);
	marg.setLeft(JVX_MARGIN_RT_VIEWER_LEFT);
	marg.setRight(JVX_MARGIN_RT_VIEWER_RIGHT);
	marg.setBottom(JVX_MARGIN_RT_VIEWER_BOTTOM);
	layout->setContentsMargins(marg);
	layout->setSpacing(JVX_MARGIN_RT_VIEWER_SPACING);
}

/**
 * Set the margins for shown properties.
 *///===========================================================
realtimeViewerProperties::realtimeViewerProperties(QWidget* parent, CjvxRealtimeViewer* theRef, IjvxHost* hostRef, std::string& descr, 
	jvxSize sectionId, IjvxReport* report, jvxComponentIdentification*tpAllI): QWidget(parent),
	realtimeViewer_base("realtimeViewerOneProperty", hostRef, theRef, report), tpAll(tpAllI)
{
	//this->selectionUser.tp = jvxComponentIdentification( JVX_COMPONENT_UNKNOWN, 0, 0); // All happening in slot 0,0
	this->selectionUser.ctxt = JVX_PROPERTY_CONTEXT_UNKNOWN;

	selectionUser.propertyIdShow = JVX_SIZE_UNSELECTED;
	selectionUser.propertyIdComponent = JVX_SIZE_UNSELECTED;
	selectionUser.numberEntriesPerLine = 1;
	selectionUser.whatToShow = 0; // 0: description, 1: descriptor, 2: name

	references.sectionId = (jvxInt16)sectionId;

	connect(this, SIGNAL(emit_removeMe_inThread(int, int, int)), this, SLOT(removeMe_inThread(int, int, int)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_updateWindow_redraw_all()), this, SLOT(updateWindow_redraw_all()), Qt::QueuedConnection);

	myCallback.callback_item = callbackUpdateField;
	myCallback.privData = reinterpret_cast<jvxHandle*>(this);

	this->setProperty("BASE_REALTIMEVIEWER", qVariantFromValue(reinterpret_cast<void*>(static_cast<realtimeViewer_base*>(this))));

//	this->selectionUser.description = "No description";
//	selectionUser.idInsert = -1;
}

realtimeViewerProperties::~realtimeViewerProperties()
{
	cleanBeforeDelete();

	// Remove all sub widgets AND unregister references
	this->updateWindow_detach_ui();

	this->references.theRef->_set_update_callback_view_section(references.sectionId, NULL);
}

// ================================================================================================

/**
 * Init this property section
 *///====================================================================
void
realtimeViewerProperties::init()
{
	jvxErrorType res = JVX_NO_ERROR;

	this->setupUi(this);

	this->comboBox_context->addItem("All"); // JVX_PROPERTY_CONTEXT_UNKNOWN 
	this->comboBox_context->addItem("Parameter"); // JVX_PROPERTY_CONTEXT_PARAMETER 
	this->comboBox_context->addItem("Info"); // JVX_PROPERTY_CONTEXT_INFO 
	this->comboBox_context->addItem("Result"); // JVX_PROPERTY_CONTEXT_RESULT 
	this->comboBox_context->addItem("Command"); // JVX_PROPERTY_CONTEXT_COMMAND 
	this->comboBox_context->addItem("ViewBuf");// JVX_PROPERTY_CONTEXT_VIEWBUFFER


	// Set the callback for the update callbacks
	res = this->references.theRef->_set_update_callback_view_section(references.sectionId, &myCallback);
	assert(res == JVX_NO_ERROR);

	// For a delayed scroll update
	connect(this, SIGNAL(emit_updateScroll_delayed()), this, SLOT(updateScroll_delayed()), Qt::QueuedConnection);

	// Update all window parts
	this->updateWindow_detach_ui();
	this->updateWindow_attach_ui();

	this->updateWindow();
}

void realtimeViewerProperties::cleanBeforeDelete()
{
	jvxRealtimeViewerPropertySection* myPropSec = NULL;
	jvxSize szPropSec = 0;
	jvxRealtimePrivateMemoryDecoderEnum decIdSec = JVX_REALTIME_PRIVATE_MEMORY_DECODER_NONE;

	references.theRef->_request_property_section(references.sectionId, reinterpret_cast<jvxHandle**>(&myPropSec), &szPropSec, (jvxInt16*)&decIdSec);
	if(myPropSec)
	{
		QScrollBar* theScrollv = scrollArea->verticalScrollBar();
		myPropSec->scrollY = theScrollv->value();

		QScrollBar* theScrollh = scrollArea->horizontalScrollBar();
		myPropSec->scrollX = theScrollh->value();
	}
}

// ===========================================================
/**
 * Update window in case a modification has been made that does not require any
 * redraw of the setup properties.
 *///====================================================================
void
realtimeViewerProperties::updateWindow()
{
	jvxSize i;
	
	// Create a specific list which holds the right comp id
	jvxComponentIdentification tpAllC[JVX_COMPONENT_ALL_LIMIT];
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		tpAllC[i] = tpAll[i];
	}
	tpAllC[selectionUser.tpV.tp] = selectionUser.tpV;

	jvx_qt_listComponentsInComboBox_(false, comboBox_components, references.theHostRef, selectionUser.tpV);

	jvx_qt_listPropertiesInComboBox(selectionUser.tpV, selectionUser.ctxt, selectionUser.whatToShow, comboBox_properties, 
		references.theHostRef, selectionUser.propertyIdShow, 
		&selectionUser.propertyIdComponent, JVX_PROPERTIES_FILTER_PROPERTIES_RTV);
	this->comboBox_context->setCurrentIndex(selectionUser.ctxt);

	this->comboBox_whatToShow->setCurrentIndex(selectionUser.whatToShow);

	label_showallnum->setText(jvx_int2String(selectionUser.numberEntriesPerLine).c_str());
}

void
realtimeViewerProperties::updateWindow_update(jvxBool fullUpdate)
{
	jvxSize i, j;
	jvxSize numGs = 0, numIs = 0;
	QFrame* theFrame = NULL;
	QHBoxLayout* newHLayout = NULL;
	jvxBool requiresRedraw;

	QLayout* theLayout = this->widget_contents->layout();
	QVBoxLayout* theVLayout = static_cast<QVBoxLayout*>(theLayout);
	if(theVLayout)
	{
		setMargins(theVLayout);

		// Update all subfields
		QLayoutItem* it = NULL;
		int cntV = theVLayout->count();
		for(i = 0; i < (jvxSize)cntV; i++)
		{
			it = theVLayout->itemAt((int)i);
			if(it)
			{
				QWidget* theW = it->widget(); // <- this is the frame

				theLayout = theW->layout();
				QHBoxLayout* theHLayout = static_cast<QHBoxLayout*>(theLayout);
				if(theHLayout)
				{
					QLayoutItem* itH = NULL;
					int cntH = theHLayout->count();
					for(j = 0; j < (jvxSize)cntH; j++)
					{
						itH = theHLayout->itemAt((int)j);
						if(itH)
						{
							QWidget* theWH = itH->widget();

							if(theWH)
							{
								QVariant var = theWH->property("BASE_REALTIMEVIEWER");
								jvxHandle* theClass = NULL;
								realtimeViewer_base::jvxRtvMainClassType theType = realtimeViewer_base::JVX_RTV_MAIN_CLASS_NONE;

								if(var.isValid())
								{
									realtimeViewer_base* thePointer = reinterpret_cast<realtimeViewer_base*>(var.value<void *>());
									thePointer->get_pointer_main_class(&theClass, &theType);
									if(theClass)
									{
										switch(theType)
										{
										case realtimeViewer_base::JVX_RTV_MAIN_CLASS_RTV_ONE_PROPERTY:
											requiresRedraw = false;
											if(fullUpdate)
											{
												requiresRedraw = true;
											}
											else
											{
												(reinterpret_cast<realtimeViewerOneProperty*>(theClass))->updateWindow_contents(requiresRedraw, fullUpdate);
											}

											if(requiresRedraw)
											{
												(reinterpret_cast<realtimeViewerOneProperty*>(theClass))->deleteUiElementsInWidget(); // delete all ui sub elements in widget - withoput destroying the widget
												(reinterpret_cast<realtimeViewerOneProperty*>(theClass))->cleanContent(); // only checks that all ui element references are gone
												(reinterpret_cast<realtimeViewerOneProperty*>(theClass))->updateWindow_construct();
												(reinterpret_cast<realtimeViewerOneProperty*>(theClass))->updateWindow_contents(requiresRedraw, false);
											}
											break;
										default:
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void
realtimeViewerProperties::updateWindow_detach_ui()
{
	jvxSize i, j;
	jvxSize numGs = 0, numIs = 0;
	QFrame* theFrame = NULL;
	QHBoxLayout* newHLayout = NULL;


	QLayout* theLayout = this->widget_contents->layout();
	QVBoxLayout* theVLayout = static_cast<QVBoxLayout*>(theLayout);
	if (theVLayout)
	{
		setMargins(theVLayout);


		// Remove all existing items
		QLayoutItem* it = NULL;
		while ((it = theVLayout->takeAt(0)))
		{
			if (it)
			{
				QWidget* theW = it->widget(); // <- this is the frame

				theLayout = theW->layout();
				QHBoxLayout* theHLayout = static_cast<QHBoxLayout*>(theLayout);
				if (theHLayout)
				{
					QLayoutItem* itH = NULL;
					while ((itH = theHLayout->takeAt(0)))
					{
						if (itH)
						{
							QWidget* theWH = itH->widget();

							if (theWH)
							{
								QVariant var = theWH->property("BASE_REALTIMEVIEWER");
								if (var.isValid())
								{
									realtimeViewer_base* thePointer = reinterpret_cast<realtimeViewer_base*>(var.value<void *>());
									if (thePointer)
									{
										thePointer->cleanBeforeDelete();
									}
								}
								delete(theWH);
							}
						}
						delete(itH);
					}
					delete(theW);
				}
				delete(it);
			}
		}
	}
}

void
realtimeViewerProperties::updateWindow_redraw_all()
{
	updateWindow_detach_ui();
	updateWindow_attach_ui();
}
/**
 * Update window such that all shown properties are redrawn. This is always necessary if
 * a new property has been added.
 *///====================================================================
void
realtimeViewerProperties::updateWindow_attach_ui()
{
	jvxSize i, j;
	jvxSize numGs = 0, numIs = 0;
	QFrame* theFrame = NULL;
	QHBoxLayout* newHLayout = NULL;


	QLayout* theLayout = this->widget_contents->layout();
	QVBoxLayout* theVLayout = static_cast<QVBoxLayout*>(theLayout);

	if (theVLayout)
	{
		theVLayout->setSizeConstraint(QLayout::SetFixedSize);

		// Next, add new items
		references.theRef->_number_groups_in_section(references.sectionId, &numGs);

		for(i = 0 ; i < numGs; i++)
		{
			theFrame = new QFrame(this);
			newHLayout = new QHBoxLayout();

			setMargins(newHLayout);

			QPushButton* newButton = new QPushButton("X", theFrame);
			jvxSize cnt = 1;
			newButton->setProperty("group-id", QVariant((int)i));
			connect(newButton, SIGNAL(clicked()), this, SLOT(removeGroup()));
			newButton->setMaximumHeight(20);
			newButton->setMinimumHeight(20);
			newButton->setMaximumWidth(20);
			newButton->setMinimumWidth(20);
			newHLayout->addWidget(newButton, 1, Qt::AlignLeft);

			references.theRef->_number_items_in_group_in_section(references.sectionId, i, &numIs);
			for(j = 0; j < numIs; j++)
			{
				//QPushButton* newButton2 = new QPushButton("Du", widget_contents);
				realtimeViewerOneProperty* theNewElement = new realtimeViewerOneProperty(this, references.theHostRef, references.theRef, references.report, references.sectionId, i, j);

				QWidget* theW = static_cast<QWidget*>(theNewElement);
				QVariant var = theW->property("BASE_REALTIMEVIEWER");
				if(var.isValid())
				{
					theW = NULL;
				}
				jvxSize width = 1;
				theNewElement->init();
				theNewElement->setProperty("group-id", QVariant((int)i));
				theNewElement->setProperty("item-id", QVariant((int)i));
				newHLayout->addWidget(theNewElement, (int)width, Qt::AlignLeft);
				cnt += width;
			}
			QPushButton* newButton3 = new QPushButton("+", theFrame);
			newButton3->setProperty("group-id", QVariant((int)i));
			connect(newButton3, SIGNAL(clicked()), this, SLOT(addElement()));
			newButton3->setMaximumHeight(20);
			newButton3->setMinimumHeight(20);
			newButton3->setMaximumWidth(20);
			newButton3->setMinimumWidth(20);
			newHLayout->addWidget(newButton3, 1, Qt::AlignLeft);

			theFrame->setLayout(newHLayout);
			theVLayout->addWidget(theFrame);
		}

		theFrame = new QFrame(this);
		newHLayout = new QHBoxLayout();
		setMargins(newHLayout);

		QPushButton* newButton4 = new QPushButton("++", theFrame);
		newButton4->setProperty("group-id", QVariant((int)i));
		connect(newButton4, SIGNAL(clicked()), this, SLOT(addGroup()));
		newButton4->setMaximumHeight(20);
		newButton4->setMinimumHeight(20);
		newButton4->setMaximumWidth(20);
		newButton4->setMinimumWidth(20);
		newHLayout->addWidget(newButton4, 1, Qt::AlignLeft);
		theFrame->setLayout(newHLayout);
		theVLayout->addWidget(theFrame);
	}
	else
	{
		references.report->report_simple_text_message("Failed to receive reference for layout in <realtimeViewerProperties::updateWindow_redraw_all>", JVX_REPORT_PRIORITY_ERROR);
	}
	emit emit_updateScroll_delayed();
}

void 
realtimeViewerProperties::updateScroll_delayed()
{
	jvxRealtimeViewerPropertySection* myPropSec = NULL;
	jvxSize szPropSec = 0;
	jvxRealtimePrivateMemoryDecoderEnum decIdSec = JVX_REALTIME_PRIVATE_MEMORY_DECODER_NONE;

	references.theRef->_request_property_section(references.sectionId, reinterpret_cast<jvxHandle**>(&myPropSec), &szPropSec, (jvxInt16*)&decIdSec);
	if(myPropSec)
	{
		QScrollBar* theScrollv = scrollArea->verticalScrollBar();
		theScrollv->setValue(myPropSec->scrollY);

		QScrollBar* theScrollh = scrollArea->horizontalScrollBar();
		theScrollh->setValue(myPropSec->scrollX);
	}
}

/**
 * New selection of a property id in combo box
 *///====================================================================
void
realtimeViewerProperties::newSelection_component(int val)
{
	if (val < comboBox_components->count())
	{
		QVariant var1 = comboBox_components->itemData(val, JVX_USER_ROLE_COMPONENT_ID);
		QVariant var2 = comboBox_components->itemData(val, JVX_USER_ROLE_COMPONENT_SLOT_ID);
		QVariant var3 = comboBox_components->itemData(val, JVX_USER_ROLE_COMPONENT_SLOTSUB_ID);
		if (
			(var1.isValid()) &&
			(var2.isValid()) &&
			(var3.isValid()))
		{
			selectionUser.tpV.tp = (jvxComponentType)var1.toInt();
			selectionUser.tpV.slotid = var2.toInt();
			selectionUser.tpV.slotsubid = var3.toInt();
		}
	}

	this->updateWindow();
}

/**
 * New selection of a property id in combo box
 *///====================================================================
void
realtimeViewerProperties::newSelection_property(int val)
{
	selectionUser.propertyIdShow = val;
	this->updateWindow();
}

/**
 * New selection of a property id in combo box
 *///====================================================================
void 
realtimeViewerProperties::newSelection_context(int val)
{
	selectionUser.ctxt = (jvxPropertyContext) val;
	this->updateWindow();
}

/**
 * New selection of descriptor/description flag
 *///====================================================================
void 
realtimeViewerProperties::activate_whatToShow(int val)
{
	selectionUser.whatToShow = val;
	this->updateWindow();
}

/**
 * Remove a complete group of items
 *///====================================================================
void
realtimeViewerProperties::removeGroup()
{
	jvxErrorType res = JVX_NO_ERROR;
	QObject* obj = sender();
	QVariant var = obj->property("group-id");
	int idRemove = var.toInt();

	this->updateWindow_detach_ui();
	res = this->references.theRef->_remove_group_in_section(references.sectionId, idRemove);
	if(res != JVX_NO_ERROR)
	{
		references.report->report_simple_text_message("Failed to call <_remove_group_in_section> in <realtimeViewerProperties::removeGroup>", JVX_REPORT_PRIORITY_ERROR);
	}
	this->updateWindow_attach_ui();
}

/**
 * Add a new realtime property
 *///====================================================================
void
realtimeViewerProperties::addElement()
{
	jvxErrorType res = JVX_NO_ERROR;

	QObject* obj = sender();
	QVariant var = obj->property("group-id");
	int idGroup = var.toInt();
	std::string descr = "no name";
	if(JVX_CHECK_SIZE_SELECTED(selectionUser.propertyIdComponent))
	{
		// Try to add an item and if successful, update all windows
		jvxRealtimeViewerPropertyItem theItem;
		static_initData(&theItem, sizeof(jvxRealtimeViewerPropertyItem),  JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM);

		updateWindow_detach_ui();

		res = references.theRef->_insert_item_in_group_in_section(references.sectionId, idGroup, selectionUser.tpV, selectionUser.propertyIdComponent, 
			JVX_SIZE_UNSELECTED, descr, &theItem, sizeof(jvxRealtimeViewerPropertyItem),  JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM);
		if(res != JVX_NO_ERROR)
		{
			references.report->report_simple_text_message("Failed to call <_insert_item_in_group_in_section> in <realtimeViewerProperties::removeGroup>", JVX_REPORT_PRIORITY_ERROR);
		}
		updateWindow_attach_ui();
	}
	else
	{
		references.report->report_simple_text_message("Adding item failed since there is no selection of a property.", JVX_REPORT_PRIORITY_WARNING);
	}
}

/**
 * Add a new group for properties to control.
 *///====================================================================
void
realtimeViewerProperties::addGroup()
{
	jvxErrorType res = JVX_NO_ERROR;

	QObject* obj = sender();
	QVariant var = obj->property("group-id");
	int idRemove = var.toInt();

	this->updateWindow_detach_ui();
		
	// Try to add a group and if successful, update all windows
	res = references.theRef->_insert_group_in_section(references.sectionId, JVX_SIZE_UNSELECTED, "property-group", JVX_REALTIME_VIEWER_GROUP_PROPERTY, NULL, 0, JVX_REALTIME_PRIVATE_MEMORY_DECODER_NONE);
	if(res != JVX_NO_ERROR)
	{
		references.report->report_simple_text_message("Failed to call <_insert_group_in_section> in <realtimeViewerProperties::removeGroup>", JVX_REPORT_PRIORITY_ERROR);
	}

	this->updateWindow_attach_ui();
}

/**
 * Function call from within the property to indicate that someone has pushed a remove button
 *///====================================================================
void
realtimeViewerProperties::removeMe_delayed(jvxSize secId, jvxSize grpId, jvxSize itId)
{
	// Actual remove process comes in delayed not to pull out an object the function call lives in
	emit emit_removeMe_inThread((int)secId, (int)grpId, (int)itId);
}

/**
 * Actually remove the item. If successful, update the windows
 *///====================================================================
void
realtimeViewerProperties::removeMe_inThread(int secId, int grpId, int itId)
{
	jvxErrorType res = JVX_NO_ERROR;

	this->updateWindow_detach_ui();
	res = references.theRef->_remove_item_in_group_in_section(secId, grpId, itId);
	if(res != JVX_NO_ERROR)
	{
		references.report->report_simple_text_message("Failed to call <_remove_item_in_group_in_section> in <realtimeViewerProperties::removeGroup>", JVX_REPORT_PRIORITY_ERROR);
	}
	this->updateWindow_attach_ui();
}


// ==========================================================================
// Callbacks
// ==========================================================================

/**
 * Callback to trigger a property update. This static function is the entry point and
 * will link to the version within the class
 *///======================================================================================
jvxErrorType
realtimeViewerProperties::callbackUpdateField(jvxSize sectionId, jvxSize groupId, jvxSize itemId, jvxHandle* privateData)
{
	jvxErrorType res = JVX_NO_ERROR;
	realtimeViewerProperties* this_pointer = reinterpret_cast<realtimeViewerProperties*>(privateData);
	if(this_pointer)
	{
		res = this_pointer->cbUpdateField(sectionId, groupId, itemId);
			/*tpViewItem, groupId, itemId, tpComp, category, handleId);*/
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

/**
 * Callback to trigger a property update. Check all connected widgets wether they are the ones
 * to be triggered. If so, make an update.
 *///======================================================================================
jvxErrorType
realtimeViewerProperties::cbUpdateField(jvxSize sectionId, jvxSize groupId, jvxSize itemId)
{
	int i,j;
	QLayout* theLayout = this->widget_contents->layout();
	QVBoxLayout* theVLayout = static_cast<QVBoxLayout*>(theLayout);
	QLayoutItem* theItem = NULL;
	QLayoutItem* theNewItem = NULL;
	realtimeViewerOneProperty* theOneProp = NULL;

	int num = theVLayout->count();
	int num2;
	for(i = 0; i < num; i++)
	{
		theItem = theVLayout->itemAt(i);
		if(theItem)
		{
			QWidget* theWidget = theItem->widget();
			QLayout* newLayout = theWidget->layout();
			QHBoxLayout* theHLayout = static_cast<QHBoxLayout*>(newLayout);

			num2 = theHLayout->count();
			for(j = 0; j < num2; j++)
			{
				theNewItem = theHLayout->itemAt(j);
				QWidget* theNewWidget = theNewItem->widget();
				theOneProp = dynamic_cast<realtimeViewerOneProperty*>(theNewWidget);
				if(theOneProp)
				{
					theOneProp->updatePropertyIfValid(sectionId, groupId, itemId);
				}
			}
		}
	}

	return(JVX_NO_ERROR);
}

void
realtimeViewerProperties::askForCompleteRebuild()
{	
	emit emit_updateWindow_redraw_all();
}

void 
realtimeViewerProperties::buttonPushed_printall()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvx_propertyReferenceTriple theTriple;
	jvxSize num = 0;
	
	std::string txt;
	jvxCallManagerProperties callGate;

	jvx_initPropertyReferenceTriple(&theTriple);

	jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, selectionUser.tpV);

	if(jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		theTriple.theProps->number_properties(callGate, &num);
		for(i = 0; i < num; i++)
		{
			jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
			jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);

			theTriple.theProps->description_property(callGate, theDescr, ident);

			txt = "Prop #" + jvx_size2String(i) + ": <" + theDescr.name.std_str() + ">-<" + 
				theDescr.descriptor.std_str() + ">:\"" + theDescr.description.std_str() + "\"";
			//references.report->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
			std::cout << txt << std::flush;
			txt = "   -> ";
			txt += jvxDataFormat_txt((jvxSize)theDescr.format);
			txt += ", ";
			txt += jvx_size2String(theDescr.num);
			txt += ", ";
			txt += jvxPropertyAccessType_txt((jvxSize)theDescr.accessType);
			txt += ", ";
			txt += jvxPropertyDecoderHintType_txt((jvxSize)theDescr.decTp);
			//references.report->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
			std::cout << txt << std::endl;

		}// for(i = 0; i < num; i++)
	}
}

void 
realtimeViewerProperties::buttonPushed_showall()
{
	jvxSize i;
	jvx_propertyReferenceTriple theTriple;
	jvxSize num = 0;
	
	bool conditionForAdd = false;
	std::string descr;
	jvxCallManagerProperties callGate;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize cnt = 0;
	jvxSize numGroups = JVX_SIZE_UNSELECTED;

	// Remove all open properties
	updateWindow_detach_ui();

	// Remove all entries
	while (1)
	{
		res = references.theRef->_remove_group_in_section(references.sectionId, 0);
		if (res != JVX_NO_ERROR)
		{
			break;
		}
	}

	// Try to obtain all properties of selected component
	jvx_initPropertyReferenceTriple(&theTriple);
	jvx_getReferencePropertiesObject(references.theHostRef, &theTriple, selectionUser.tpV);

	if(jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		theTriple.theProps->number_properties(callGate, &num);
		for(i = 0; i < num; i++)
		{
			jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
			jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);
			theTriple.theProps->description_property(callGate, theDescr, ident);

			conditionForAdd = jvx_applyPropertyFilter(theDescr.format, theDescr.num, theDescr.decTp, JVX_PROPERTIES_FILTER_PROPERTIES_RTV);
			
			if(conditionForAdd)
			{
				if(selectionUser.ctxt != JVX_PROPERTY_CONTEXT_UNKNOWN)
				{					
					if(theDescr.ctxt != selectionUser.ctxt)
					{
						conditionForAdd = false;
					}
				}
			}

			if(conditionForAdd)
			{
				if((cnt % selectionUser.numberEntriesPerLine) == 0)
				{
					numGroups++;
					res = references.theRef->_insert_group_in_section(references.sectionId, JVX_SIZE_UNSELECTED, "property-group", JVX_REALTIME_VIEWER_GROUP_PROPERTY, NULL, 0, JVX_REALTIME_PRIVATE_MEMORY_DECODER_NONE);
				}

				jvxRealtimeViewerPropertyItem theItem;
				static_initData(&theItem, sizeof(jvxRealtimeViewerPropertyItem),  JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM);
				res = references.theRef->_insert_item_in_group_in_section(references.sectionId, numGroups, selectionUser.tpV, 
					i, JVX_SIZE_UNSELECTED, descr, &theItem, sizeof(jvxRealtimeViewerPropertyItem),  JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM);
				cnt++;
			}
		}// for(i = 0; i < num; i++)	
	}
	updateWindow_attach_ui();
}
 
void 
realtimeViewerProperties::selectionNumberPerLine()
{
	selectionUser.numberEntriesPerLine = horizontalSlider_showallnum->value();
	updateWindow();
}
