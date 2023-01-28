#include "jvx_menu_host_bridge_components.h"
#include "jvx_newvalue_dialog.h"
#include "showInformation.h"
#include "integerProperties.h"
#include "floatProperties.h"
#include "stringFilenameProperties.h"
#include "stringAssignmentProperties.h"

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtMenuComponentsBridge, jvxQtMenuComponentsBridge, jvx_menu_host_bridge_components)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtMenuComponentsBridge, jvxQtMenuComponentsBridge)

// ==========================================================================================

void
jvx_menu_host_bridge_components::createMenues(configureHost_features* host_features_arg,
	IjvxHost* hHost_arg,
	IjvxToolsHost* hTools_arg,
	QMenu* menuConfiguration_arg,
	jvxComponentIdentification* tpAll_arg,
	jvxBool* skipStateSelected_arg,
	jvx_menu_host_bridge_components_report* report_arg)
{
	jvxSize i;
	oneSetMenuItems theItems;
	jvxComponentIdentification cpId;
	jvxComponentType childTp;
	jvxSize numSlotsMax = 0;
	jvxSize numSubSlotsMax = 0;

	host_features_ref = host_features_arg;
	hHost_ref = hHost_arg;
	hTools_ref = hTools_arg;
	menuConfiguration_ref = menuConfiguration_arg;
	
	tpAll_ref = tpAll_arg;
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		tpAllNext[i].reset((jvxComponentType)i, 0, 0);
	}

	skipStateSelected_ref = skipStateSelected_arg;

	report = report_arg;
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		if ((host_features_ref->numSlotsComponents[i] > 0) && (theClassAssociation[i].description))
		{
			cpId.tp = (jvxComponentType)i;

			switch (theClassAssociation[i].comp_class)
			{
			case jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY:

				assert(theClassAssociation[i].comp_sec_type != JVX_COMPONENT_UNKNOWN);
				theItems.reset();

				hHost_ref->number_slots_component_system(cpId, NULL, NULL, NULL, &childTp, &numSlotsMax, &numSubSlotsMax);
				//cpId.slotid = tpAll[i].slotid;
				//involvedHost.hHost->number_slots_component(cpId, NULL, &numSubSlots);

				theItems.addme_pri = new QMenu(menuConfiguration_ref);
				theItems.addme_pri->setObjectName(theClassAssociation[i].config_token);
				theItems.addme_pri->setTitle(theClassAssociation[i].description);
				if (numSlotsMax > 1)
				{
					theItems.addme_pri_slots = new QMenu(theItems.addme_pri);
					theItems.addme_pri_slots->setObjectName("Slots");
					theItems.addme_pri_slots->setTitle("Slots");
				}
				theItems.addme_pri_avail = new QMenu(theItems.addme_pri);
				theItems.addme_pri_avail->setObjectName(((std::string)theClassAssociation[i].config_token + "_avail").c_str());
				theItems.addme_pri_avail->setTitle("Available");
				theItems.addme_pri_stat = new QMenu(theItems.addme_pri);
				theItems.addme_pri_stat->setObjectName(((std::string)theClassAssociation[i].config_token + "_stat").c_str());
				theItems.addme_pri_stat->setTitle("Status");
				theItems.addme_pri_props = new QMenu(theItems.addme_pri);
				theItems.addme_pri_props->setObjectName(((std::string)theClassAssociation[i].config_token + "_props").c_str());
				theItems.addme_pri_props->setTitle("Properties");
				theItems.addme_pri_commands = new QMenu(theItems.addme_pri);
				theItems.addme_pri_commands->setObjectName(((std::string)theClassAssociation[i].config_token + "_commands").c_str());
				theItems.addme_pri_commands->setTitle("Commands");
				theItems.addme_pri_conditions = new QMenu(theItems.addme_pri);
				theItems.addme_pri_conditions->setObjectName(((std::string)theClassAssociation[i].config_token + "_conditions").c_str());
				theItems.addme_pri_conditions->setTitle("Conditions");

				assert(childTp != JVX_COMPONENT_UNKNOWN);
				theItems.addme_pri_dev_caps = new QMenu(theItems.addme_pri);
				theItems.addme_pri_dev_caps->setObjectName(((std::string)theClassAssociation[i].config_token + "_devcaps").c_str());
				theItems.addme_pri_dev_caps->setTitle("Devices");

				theItems.addme_pri_information = new QAction(theItems.addme_pri);
				theItems.addme_pri_information->setObjectName(((std::string)theClassAssociation[i].config_token + "_information").c_str());
				theItems.addme_pri_information->setText("Information");
				connect(theItems.addme_pri_information, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));

				theItems.addme_sec = new QMenu(menuConfiguration_ref);
				theItems.addme_sec->setObjectName(theClassAssociation[i].config_token);
				theItems.addme_sec->setTitle(theClassAssociation[i].description_sec);
				if (numSubSlotsMax > 1)
				{
					theItems.addme_sec_sub_slots = new QMenu(theItems.addme_sec);
					theItems.addme_sec_sub_slots->setObjectName("Subslots");
					theItems.addme_sec_sub_slots->setTitle("Subslots");
				}
				theItems.addme_sec_avail = new QMenu(theItems.addme_sec);
				theItems.addme_sec_avail->setObjectName(((std::string)theClassAssociation[i].config_token + "_availd").c_str());
				theItems.addme_sec_avail->setTitle("Available");
				theItems.addme_sec_stat = new QMenu(theItems.addme_sec);
				theItems.addme_sec_stat->setObjectName(((std::string)theClassAssociation[i].config_token + "_statd").c_str());
				theItems.addme_sec_stat->setTitle("Status");
				theItems.addme_sec_props = new QMenu(theItems.addme_sec);
				theItems.addme_sec_props->setObjectName(((std::string)theClassAssociation[i].config_token + "_propsd").c_str());
				theItems.addme_sec_props->setTitle("Properties");
				theItems.addme_sec_commands = new QMenu(theItems.addme_sec);
				theItems.addme_sec_commands->setObjectName(((std::string)theClassAssociation[i].config_token + "_commandsd").c_str());
				theItems.addme_sec_commands->setTitle("Commands");
				theItems.addme_sec_conditions = new QMenu(theItems.addme_sec);
				theItems.addme_sec_conditions->setObjectName(((std::string)theClassAssociation[i].config_token + "_conditionsd").c_str());
				theItems.addme_sec_conditions->setTitle("Conditions");

				theItems.addme_sec_information = new QAction(theItems.addme_sec);
				theItems.addme_sec_information->setObjectName(((std::string)theClassAssociation[i].config_token + "_informationd").c_str());
				theItems.addme_sec_information->setText("Information");
				connect(theItems.addme_sec_information, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));

				menuConfiguration_ref->addAction(theItems.addme_pri->menuAction());
				if (theItems.addme_pri_slots)
				{
					theItems.addme_pri->addAction(theItems.addme_pri_slots->menuAction());
					theItems.addme_pri->addSeparator();
				}
				theItems.addme_pri->addAction(theItems.addme_pri_avail->menuAction());
				theItems.addme_pri->addAction(theItems.addme_pri_stat->menuAction());
				theItems.addme_pri->addSeparator();
				theItems.addme_pri->addAction(theItems.addme_pri_props->menuAction());
				theItems.addme_pri->addAction(theItems.addme_pri_commands->menuAction());
				theItems.addme_pri->addAction(theItems.addme_pri_conditions->menuAction());
				theItems.addme_pri->addAction(theItems.addme_pri_dev_caps->menuAction());
				theItems.addme_pri->addSeparator();
				theItems.addme_pri->addAction(theItems.addme_pri_information);
				theItems.tp_pri = (jvxComponentType)i;

				menuConfiguration_ref->addAction(theItems.addme_sec->menuAction());
				if (theItems.addme_sec_sub_slots)
				{
					theItems.addme_sec->addAction(theItems.addme_sec_sub_slots->menuAction());
					theItems.addme_sec->addSeparator();
				}
				theItems.addme_sec->addAction(theItems.addme_sec_avail->menuAction());
				theItems.addme_sec->addAction(theItems.addme_sec_stat->menuAction());
				theItems.addme_sec->addSeparator();
				theItems.addme_sec->addAction(theItems.addme_sec_props->menuAction());
				theItems.addme_sec->addAction(theItems.addme_sec_commands->menuAction());
				theItems.addme_sec->addAction(theItems.addme_sec_conditions->menuAction());
				theItems.addme_sec->addSeparator();
				theItems.addme_sec->addAction(theItems.addme_sec_information);
				theItems.tp_sec = theClassAssociation[i].comp_sec_type;

				menuConfiguration_ref->addSeparator();
				theMenuItems[(jvxComponentType)i] = theItems;
				break;

			case jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE:

				assert(theClassAssociation[i].comp_sec_type == JVX_COMPONENT_UNKNOWN);
				theItems.reset();

				hHost_ref->number_slots_component_system(cpId, NULL, NULL, NULL, NULL, &numSlotsMax, &numSubSlotsMax);
				cpId.slotid = tpAll_ref[i].slotid;
				//involvedHost.hHost->number_slots_component(cpId, NULL, &numSubSlots);

				theItems.addme_pri = new QMenu(menuConfiguration_ref);
				theItems.addme_pri->setObjectName(theClassAssociation[i].config_token);
				theItems.addme_pri->setTitle(theClassAssociation[i].description);
				if (numSlotsMax > 1)
				{
					theItems.addme_pri_slots = new QMenu(theItems.addme_pri);
					theItems.addme_pri_slots->setObjectName("Slots");
					theItems.addme_pri_slots->setTitle("Slots");
				}
				theItems.addme_pri_avail = new QMenu(theItems.addme_pri);
				theItems.addme_pri_avail->setObjectName(((std::string)theClassAssociation[i].config_token + "_avail").c_str());
				theItems.addme_pri_avail->setTitle("Available");
				theItems.addme_pri_stat = new QMenu(theItems.addme_pri);
				theItems.addme_pri_stat->setObjectName(((std::string)theClassAssociation[i].config_token + "_stat").c_str());
				theItems.addme_pri_stat->setTitle("Status");
				theItems.addme_pri_props = new QMenu(theItems.addme_pri);
				theItems.addme_pri_props->setObjectName(((std::string)theClassAssociation[i].config_token + "_props").c_str());
				theItems.addme_pri_props->setTitle("Properties");
				theItems.addme_pri_commands = new QMenu(theItems.addme_pri);
				theItems.addme_pri_commands->setObjectName(((std::string)theClassAssociation[i].config_token + "_commands").c_str());
				theItems.addme_pri_commands->setTitle("Commands");
				theItems.addme_pri_conditions = new QMenu(theItems.addme_pri);
				theItems.addme_pri_conditions->setObjectName(((std::string)theClassAssociation[i].config_token + "_conditions").c_str());
				theItems.addme_pri_conditions->setTitle("Conditions");

				theItems.addme_pri_information = new QAction(theItems.addme_pri);
				theItems.addme_pri_information->setObjectName(((std::string)theClassAssociation[i].config_token + "_information").c_str());
				theItems.addme_pri_information->setText("Information");
				connect(theItems.addme_pri_information, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
				theItems.tp_pri = (jvxComponentType)i;

				menuConfiguration_ref->addAction(theItems.addme_pri->menuAction());
				if (theItems.addme_pri_slots)
				{
					theItems.addme_pri->addAction(theItems.addme_pri_slots->menuAction());
					theItems.addme_pri->addSeparator();
				}
				theItems.addme_pri->addAction(theItems.addme_pri_avail->menuAction());
				theItems.addme_pri->addAction(theItems.addme_pri_stat->menuAction());
				theItems.addme_pri->addSeparator();
				theItems.addme_pri->addAction(theItems.addme_pri_props->menuAction());
				theItems.addme_pri->addAction(theItems.addme_pri_commands->menuAction());
				theItems.addme_pri->addAction(theItems.addme_pri_conditions->menuAction());
				theItems.addme_pri->addSeparator();
				theItems.addme_pri->addAction(theItems.addme_pri_information);

				menuConfiguration_ref->addSeparator();
				theMenuItems[(jvxComponentType)i] = theItems;
				break;
			}
		}
	}
	menuOther_Components = new QMenu(menuConfiguration_ref);
	menuOther_Components->setObjectName("OtherComponents");
	menuOther_Components->setTitle(QStringLiteral("Other Components"));
	menuConfiguration_ref->addAction(menuOther_Components->menuAction());

	// All phases passed, staring right away UI elements */
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		jvx_select_next_avail_slot(hHost_ref, (jvxComponentType)i, tpAllNext);
	}
}

// ==========================================================================================

void
jvx_menu_host_bridge_components::removeMenues()
{
	auto elm = theMenuItems.begin();
	for (; elm != theMenuItems.end(); elm++)
	{
		if (elm->second.addme_pri) delete elm->second.addme_pri;
		// All sub menus deleted automatically

		if (elm->second.addme_sec) delete elm->second.addme_sec;
		// All sub menus deleted automatically
	}
	theMenuItems.clear();
	delete menuOther_Components;
	menuOther_Components = NULL;

	host_features_ref = NULL;
	hHost_ref = NULL;
	menuConfiguration_ref = NULL;
	tpAll_ref = NULL;

}

// ==========================================================================================
// ==========================================================================================

jvx_menu_host_bridge_components::jvx_menu_host_bridge_components(QWidget* parent) : QObject(parent)
{
	jvxSize i;
	host_features_ref = NULL;
	hHost_ref = NULL;
	hTools_ref = NULL;
	menuConfiguration_ref = NULL;
	tpAll_ref = NULL;
	
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		tpAllNext[i].reset((jvxComponentType)i, 0, 0);
	}
	report = NULL;

	menuOther_Components = NULL;
	parentWid = parent;
}

jvx_menu_host_bridge_components::~jvx_menu_host_bridge_components()
{

}

void
jvx_menu_host_bridge_components::menuUpdate_oneComponentType(
	QMenu* lstNames,
	QMenu* lstStates, 
	QMenu* lstProperties, 
	QMenu* lstCommands,
	QMenu* lstConditions, 
	QMenu* lstDeviceCaps,
	QAction* lstInformation, QMenu* lstSlots,
	const jvxComponentIdentification& tp, jvxBool isPriComp)
{
	jvxSize i;
	QAction* act = NULL;
	jvxState stat = JVX_STATE_NONE;
	jvxSize idItalicL = JVX_SIZE_UNSELECTED;
	jvxSize idItalicH = JVX_SIZE_UNSELECTED;
	jvxCallManagerProperties callGate;
	IjvxObject* theObject = NULL;
	IjvxProperties* theProps = NULL;
	IjvxSequencerControl* theSeqC = NULL;
	std::string entry;
	jvxSize numSlots = 0;
	jvxSize numSubSlots = 0;
	jvxSize numSlotsMax = 0;
	jvxSize numSubSlotsMax = 0;

	// =================================================
	// Clear all menus for scanner technologies
	// =================================================

	if (lstNames) lstNames->clear();
	if (lstStates) lstStates->clear();
	if (lstProperties) lstProperties->clear();
	if (lstCommands) lstCommands->clear();
	if (lstConditions) lstConditions->clear();
	if (lstDeviceCaps) lstDeviceCaps->clear();
	if(lstSlots) lstSlots->clear();

	jvxSize idx = JVX_SIZE_UNSELECTED;
	jvxSize num = 0;
	jvxApiString fldStr;

	jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
	jvx::propertyAddress::CjvxPropertyAddressLinear ident(idx);

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType res2 = JVX_NO_ERROR;

	hHost_ref->number_slots_component_system(tp, &numSlots, &numSubSlots, NULL, NULL, &numSlotsMax, &numSubSlotsMax);
	if (isPriComp)
	{
		//std::cout << jvxComponentIdentification_txt(tp) << ": slots=" << numSlots << "; slotsmax=" << numSlotsMax << std::endl;
		if (lstSlots)
		{
			lstSlots->clear();
			for (i = 0; i < numSlots; i++)
			{
				act = new QAction(this);

				if (i == tpAll_ref[tp.tp].slotid)
				{
					QFont ft = act->font();
					ft.setBold(true);
					act->setFont(ft);
				}
				act->setText((jvx_size2String(i) + "/" + jvx_size2String(numSlotsMax)).c_str());
				act->setProperty("task", QVariant(120));
				act->setProperty("identification", QVariant((int)tp.tp));
				act->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(i)));
				act->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(JVX_SIZE_UNSELECTED)));
				act->setProperty("subid", QVariant(-1));
				connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
				lstSlots->addAction(act);
				act = NULL;
			}
			if (JVX_CHECK_SIZE_SELECTED(tpAllNext[tp.tp].slotid))
			{
				act = new QAction(this);
				/*
				if (i == idx)
				{
					QFont ft = act->font();
					ft.setBold(true);
					act->setFont(ft);
				}*/
				act->setText("Next");
				act->setProperty("task", QVariant(130));
				act->setProperty("identification", QVariant((int)tp.tp));
				act->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(i)));
				act->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(JVX_SIZE_UNSELECTED)));
				act->setProperty("subid", QVariant(-1));
				connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
				lstSlots->addAction(act);
				act = NULL;
			}

			lstSlots->setTitle(("Slots (" + jvx_size2String(numSlotsMax) + ")").c_str());
		}
	}
	else
	{
		//std::cout << jvxComponentIdentification_txt(tp) << ": subslots=" << numSubSlots << "; subslotsmax=" << numSubSlotsMax << std::endl;
		if (lstSlots)
		{
			lstSlots->clear();
			/*
			std::string slot_hint;
			for (i = 0; i < numSubSlotsMax; i++)
			{
				slot_hint += "*";
			}
			*/

			for (i = 0; i < numSubSlots; i++)
			{
				act = new QAction(this);
				if (i == tpAll_ref[tp.tp].slotsubid)
				{
					QFont ft = act->font();
					ft.setBold(true);
					act->setFont(ft);
				}
				act->setText((jvx_size2String(i) + "/" + jvx_size2String(numSubSlotsMax)).c_str());
				act->setProperty("task", QVariant(120));
				act->setProperty("identification", QVariant((int)tp.tp));
				act->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(JVX_SIZE_UNSELECTED)));
				act->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(i)));
				act->setProperty("subid", QVariant(-1));
				connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
				lstSlots->addAction(act);
				act = NULL;
			}
			if (JVX_CHECK_SIZE_SELECTED(tpAllNext[tp.tp].slotid))
			{
				act = new QAction(this);
				act->setText("Next");
				act->setProperty("task", QVariant(130));
				act->setProperty("identification", QVariant((int)tp.tp));
				act->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(JVX_SIZE_UNSELECTED)));
				act->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(i)));
				act->setProperty("subid", QVariant(-1));
				connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
				lstSlots->addAction(act);
				act = NULL;
			}
			lstSlots->setTitle(("Slots (" + jvx_size2String(numSubSlotsMax) + ")").c_str());

		}
	}

	hHost_ref->number_components_system(tp, &num);
	hHost_ref->selection_component(tp, &idx);

	stat = JVX_STATE_NONE;
	if (JVX_CHECK_SIZE_SELECTED(idx))
	{
		hHost_ref->state_selected_component(tp, &stat);
	}

	// Entry "none"
	act = new QAction(this);
	act->setText("None");
	act->setProperty("task", QVariant(0));
	act->setProperty("identification", QVariant((int)tp.tp));
	act->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(tp.slotid)));
	act->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(tp.slotsubid)));
	act->setProperty("subid", QVariant(-1));
	connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
	lstNames->addAction(act);
	act = NULL;

	if (num > 0)
	{
		for (i = 0; i < num; i++)
		{
			std::string txtShow;
			hHost_ref->description_component_system(tp, i, &fldStr);
			txtShow = fldStr.std_str();

			hHost_ref->module_reference_component_system(tp, i, &fldStr, NULL);
			txtShow += " - ";
			txtShow += fldStr.std_str();

			act = new QAction(this);
			if (i == idx)
			{
				QFont ft = act->font();
				if (stat >= JVX_STATE_ACTIVE)
				{
					ft.setBold(true);
				}
				else
				{
					ft.setBold(false);
				}
				if (stat == JVX_STATE_SELECTED)
				{
					ft.setItalic(true);
				}
				else
				{
					ft.setItalic(false);
				}

				act->setFont(ft);
			}
			act->setText(txtShow.c_str());
			act->setProperty("task", QVariant(0));
			act->setProperty("identification", QVariant((int)tp.tp));
			act->setProperty("subid", QVariant((int)i));
			connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
			lstNames->addAction(act);
			act = NULL;
		}
	}

	

	// Possible next states
	idItalicL = jvx_stateToIndex(stat);
	idItalicH = jvx_stateToIndex(stat);

	if (stat == JVX_STATE_SELECTED)
	{
		idItalicL = jvx_stateToIndex(JVX_STATE_ACTIVE);
		idItalicH = jvx_stateToIndex(JVX_STATE_ACTIVE);
	}
	if (stat == JVX_STATE_ACTIVE)
	{
		idItalicL = jvx_stateToIndex(JVX_STATE_SELECTED);
		idItalicH = jvx_stateToIndex(JVX_STATE_PREPARED);
	}
	for (i = 0; i < JVXSTATE_NUM_USE; i++)
	{
		jvxState statC = jvx_idxToState(i);

		act = new QAction(this);
		QFont ft = act->font();

		ft.setBold(false);
		ft.setItalic(false);

		if (statC == stat)
		{
			ft.setBold(true);
		}
		if (i == idItalicL)
		{
			ft.setItalic(true);
		}
		if (i == idItalicH)
		{
			ft.setItalic(true);
		}
		act->setFont(ft);

		act->setText(jvxState_txt(statC));
		act->setProperty("task", QVariant(10));
		act->setProperty("identification", QVariant((int)tp.tp));
		act->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(tp.slotid)));
		act->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(tp.slotsubid)));
		act->setProperty("subid", QVariant((int)i));
		connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
		lstStates->addAction(act);
		act = NULL;
	}

	res2 = hHost_ref->request_object_selected_component(tp, &theObject);

	if ((res2 == JVX_NO_ERROR) && theObject)
	{
		res = hHost_ref->request_hidden_interface_selected_component(tp, JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
		if ((res == JVX_NO_ERROR) && theProps)
		{
			num = 0;
			res = theProps->number_properties(callGate, &num);
			if (res == JVX_NO_ERROR)
			{
				for (i = 0; i < num; i++)
				{
					ident.idx = i;
					res = theProps->description_property(callGate, theDescr, ident);
					if (res == JVX_NO_ERROR)
					{
						entry = theDescr.descriptor.std_str();
						entry += "--";
						entry += jvxPropertyContext_txt(theDescr.ctxt);
						entry += ": ";
						entry += fldStr.std_str();
						entry += "<" + jvx_validStates2String(theDescr.allowedStateMask) + ">";
						act = new QAction(this);
						QFont ft = act->font();
						/*
						if(i == stat)
						{
						ft.setBold(true);
						}
						if(i == idItalic)
						{
						ft.setItalic(true);
						}*/
						act->setFont(ft);
						act->setText(entry.c_str());
						act->setProperty("task", QVariant(30));
						act->setProperty("identification", QVariant((int)tp.tp));
						act->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(tp.slotid)));
						act->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(tp.slotsubid)));
						act->setProperty("subid", QVariant((int)i));
						connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
						lstProperties->addAction(act);
						act = NULL;

					}
				}
			}
		}
		if (theProps)
		{
			res = hHost_ref->return_hidden_interface_selected_component(tp, JVX_INTERFACE_PROPERTIES, theProps);
		}


		// Obtain sequencer control entries
		res = hHost_ref->request_hidden_interface_selected_component(tp, JVX_INTERFACE_SEQUENCERCONTROL, (jvxHandle**)&theSeqC);
		if ((res == JVX_NO_ERROR) && theSeqC)
		{
			num = 0;
			res = theSeqC->number_commands(&num);
			if (res == JVX_NO_ERROR)
			{
				for (i = 0; i < num; i++)
				{
					jvxUInt64 stateValid = 0;
					jvxSize unqiueId = 0;
					
					res = theSeqC->description_command(i, &unqiueId, &fldStr, &stateValid);
					if (res == JVX_NO_ERROR)
					{
						entry = fldStr.std_str();
						entry += "<" + jvx_validStates2String(stateValid) + ">";
						act = new QAction(this);
						QFont ft = act->font();
						act->setText(entry.c_str());
						act->setProperty("task", QVariant(40));
						act->setProperty("identification", QVariant((int)tp.tp));
						act->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(tp.slotid)));
						act->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(tp.slotsubid)));
						act->setProperty("subid", QVariant((int)unqiueId));
						connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
						lstCommands->addAction(act);
						act = NULL;

					}
				}
			}

			num = 0;
			res = theSeqC->number_conditions(&num);
			if (res == JVX_NO_ERROR)
			{
				for (i = 0; i < num; i++)
				{
					jvxUInt64 stateValid = 0;
					jvxSize unqiueId = 0;

					res = theSeqC->description_condition(i, &unqiueId, &fldStr, &stateValid);
					if (res == JVX_NO_ERROR)
					{
						entry = fldStr.std_str();
						entry += "<" + jvx_validStates2String(stateValid) + ">";
						act = new QAction(this);
						QFont ft = act->font();
						act->setText(entry.c_str());
						act->setProperty("task", QVariant(50));
						act->setProperty("identification", QVariant((int)tp.tp));
						act->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(tp.slotid)));
						act->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(tp.slotsubid)));
						act->setProperty("subid", QVariant((int)unqiueId));
						connect(act, SIGNAL(triggered()), this, SLOT(action_int_selection_slot()));
						lstConditions->addAction(act);
						act = NULL;
					}
				}
			}

		}
		if (theSeqC)
		{
			res = hHost_ref->return_hidden_interface_selected_component(tp, JVX_INTERFACE_SEQUENCERCONTROL, theSeqC);
		}

		if (lstDeviceCaps)
		{
			IjvxTechnology* tPtr = castFromObject<IjvxTechnology>(theObject);
			if (tPtr)
			{
				jvxSize numD = 0;
				jvxApiString astr;
				tPtr->number_devices(&numD);
				for (i = 0; i < numD; i++)
				{
					jvxDeviceCapabilities caps;
					jvxComponentIdentification tpDev;

					tPtr->capabilities_device(i, caps);
					tPtr->location_info_device(i, tpDev);

					QMenu* itt = new QMenu(lstDeviceCaps);
					
					
					lstDeviceCaps->addMenu(itt);

					tPtr->description_device(i, &astr);
					itt->setTitle(astr.c_str());
					
					if (JVX_CHECK_SIZE_SELECTED(tpDev.slotsubid))
					{
						QFont ft = itt->menuAction()->font();
						ft.setBold(true);
						ft.setItalic(true);
						itt->menuAction()->setFont(ft);
					}

					act = new QAction(this);
					act->setText(("Capabilities: " + jvxDeviceCapabilitiesType_txt(caps.capsFlags)).c_str());
					itt->addAction(act);
					
					act = new QAction(this);
					act->setText(("Flow: " + (std::string)jvxDeviceDataFlowType_txt(caps.flow)).c_str());
					itt->addAction(act);
					
					if (JVX_CHECK_SIZE_SELECTED(tpDev.slotsubid))
					{
						act = new QAction(this);
						act->setText(("Slotsubid:" + jvx_size2String(tpDev.slotsubid)).c_str());
						itt->addAction(act);
					}

					act = new QAction(this);
					if (caps.selectable)
					{
						act->setText("Selectable");
					}
					else
					{
						act->setText("Not Selectable");
					}
					itt->addAction(act);

					act = new QAction(this);
					if (caps.selectable)
					{
						act->setText("Singleton");
					}
					else
					{
						act->setText("Not a Singleton");
					}
					itt->addAction(act);

					act = new QAction(this);
					act->setText(("Flags: " + jvx_bitfield162String(caps.flags)).c_str());
					itt->addAction(act);
				}
			}
		}
	}
	if (theObject)
	{
		res2 = hHost_ref->return_object_selected_component(tp, theObject);
	}

	lstInformation->setProperty("task", QVariant(70));
	lstInformation->setProperty("identification", QVariant((int)tp.tp));
	lstInformation->setProperty("identification_slotid", QVariant((int)JVX_SIZE_INT(tp.slotid)));
	lstInformation->setProperty("identification_slotsubid", QVariant((int)JVX_SIZE_INT(tp.slotsubid)));
	lstInformation->setProperty("subid", QVariant(-1));
}

void
jvx_menu_host_bridge_components::updateWindow()
{
	auto elm = theMenuItems.begin();
	for (; elm != theMenuItems.end(); elm++)
	{
		updateWindow_core_single(elm->second);
	}
}

void
jvx_menu_host_bridge_components::updateWindow_core_single(oneSetMenuItems& item)
{
	if (item.addme_pri)
	{
		menuUpdate_oneComponentType(item.addme_pri_avail,
			item.addme_pri_stat, item.addme_pri_props,
			item.addme_pri_commands, item.addme_pri_conditions,
			item.addme_pri_dev_caps,
			item.addme_pri_information, item.addme_pri_slots,
			tpAll_ref[item.tp_pri], true);
	}
	if (item.addme_sec)
	{
		menuUpdate_oneComponentType(item.addme_sec_avail,
			item.addme_sec_stat, item.addme_sec_props,
			item.addme_sec_commands, item.addme_sec_conditions,
			nullptr,
			item.addme_sec_information, item.addme_sec_sub_slots,
			tpAll_ref[item.tp_sec], false);
	}
}

void 
jvx_menu_host_bridge_components::updateWindowSingle(jvxComponentIdentification tp)
{
	auto elm = theMenuItems.find(tp.tp);
	if (elm == theMenuItems.end())
	{
		elm = theMenuItems.begin();
		for (; elm != theMenuItems.end(); elm++)
		{
			if (tp.tp == elm->second.tp_sec)
			{
				break;
			}
		}
	}
	if (elm != theMenuItems.end())
	{
		updateWindow_core_single(elm->second);
	}
}

/**
 * Slot to handle integer selection with different purposes
 */
void
jvx_menu_host_bridge_components::action_int_selection_slot()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType res2 = JVX_NO_ERROR;
	QObject* senderO = this->sender();
	jvxSize idSelect = JVX_SIZE_UNSELECTED;
	jvxSize idSelectedBefore = JVX_SIZE_UNSELECTED;
	jvxBitField stat = JVX_STATE_NONE;
	jvxBitField stat_new = JVX_STATE_NONE;
	jvxState stat0 = JVX_STATE_NONE;
	jvxSize num = 0;
	jvxApiString fldStr;
	IjvxProperties* theProps = NULL;
	std::string description;
	IjvxObject* theObject = NULL;
	jvx_newvalue_dialog* getNewValue = NULL;
	showInformation* showInfo = NULL;
	jvxPropertyContext ctxt = JVX_PROPERTY_CONTEXT_UNKNOWN;
	QVariant varsCB, varsPD;
	jvxCallManagerProperties callGate;
	jvxInt32 valI;
	jvxBool valB;
	QVariant varT;
	QVariant varI;
	std::string showTxt;

	jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
	jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);

	int taskid = 0;
	int id = 0;
	int uId = 0;
	int id_sid = 0;
	int id_ssid = 0;
	jvxComponentType childTp = JVX_COMPONENT_UNKNOWN;

	(static_cast<IjvxObject*>(hHost_ref))->state(&stat0);
	stat = stat0;

	if (stat == JVX_STATE_ACTIVE)
	{
		//uUpdateValue* getNewValue = NULL;
		if (senderO != NULL)
		{
			QAction* act = dynamic_cast<QAction*>(senderO);
			QVariant task = senderO->property("task");
			QVariant var = senderO->property("identification");
			QVariant var_sid = senderO->property("identification_slotid");
			QVariant var_ssid = senderO->property("identification_slotsubid");
			QVariant vars = senderO->property("subid");
			if (task.isValid() && var.isValid() && vars.isValid())
			{
				taskid = task.toInt();
				id = var.toInt();
				uId = vars.toInt();
				assert(id < JVX_COMPONENT_ALL_LIMIT);
				idSelect = vars.toInt();

				switch (taskid)
				{
				case 0:

					// ==========================
					// SELECT NEW COMPONENT
					// ==========================
					if (!host_features_ref->flag_blockModuleEdit[id])
					{
						hHost_ref->selection_component(tpAll_ref[id], &idSelectedBefore);
						if (JVX_CHECK_SIZE_SELECTED(idSelectedBefore) && (idSelectedBefore != idSelect))
						{
							// If new selection, unselect old selection first
							hHost_ref->state_selected_component(tpAll_ref[id], &stat0);
							stat = stat0;
							if (stat == JVX_STATE_ACTIVE)
							{
								res = hHost_ref->deactivate_selected_component(tpAll_ref[id]);
								if (res != JVX_NO_ERROR)
								{
									if (report)
									{
										report->postMessageError((std::string("Failed to deactivate active ") +
											jvxComponentIdentification_txt(tpAll_ref[id])).c_str());
										// H this->postMessage_inThread((std::string("Failed to deactivate active ") + jvxComponentIdentification_txt(tpAll[id])).c_str(), Qt::red);
									}
								}
							}
							hHost_ref->state_selected_component(tpAll_ref[id], &stat0);
							stat = stat0;
							if (stat == JVX_STATE_SELECTED)
							{
								res = hHost_ref->unselect_selected_component(tpAll_ref[id]);
								if (res != JVX_NO_ERROR)
								{
									if (report)
									{
										report->postMessageError((std::string("Failed to unselect selected ") + jvxComponentIdentification_txt(tpAll_ref[id])).c_str());
										// H this->postMessage_inThread((std::string("Failed to unselect selected ") + jvxComponentIdentification_txt(tpAll[id])).c_str(), Qt::red);
									}
								}
							}
						}

						// Select the approriate component
						if (JVX_CHECK_SIZE_SELECTED(idSelect))
						{
							// Keep the slot usage compact: always use the first available slot and 
							// switch the "show" id to the new selection
							jvxComponentTypeClass cpCls = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
							hHost_ref->component_class(tpAll_ref[id].tp, cpCls);
							switch (cpCls)
							{
							case jvxComponentTypeClass::JVX_COMPONENT_TYPE_DEVICE:
								tpAll_ref[id].slotsubid = JVX_SIZE_DONTCARE;
								break;
							case jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE:
								tpAll_ref[id].slotid = JVX_SIZE_DONTCARE;
								tpAll_ref[id].slotsubid = 0;
								break;
							default:
								assert(0);
							}

							res = hHost_ref->select_component(tpAll_ref[id], idSelect);
							if (res != JVX_NO_ERROR)
							{
								if (report)
								{
									report->postMessageError((std::string("Failed to select different ") +
										jvxComponentIdentification_txt(tpAll_ref[id]) +
										" (id #" + jvx_size2String(idSelect) + ")").c_str());
									// H this->postMessage_inThread((std::string("Failed to select different ") + jvxComponentIdentification_txt(tpAll[id]) +
									//	" (id #" + jvx_size2String(idSelect) + ")").c_str(), Qt::red);
								}
							}
							else
							{
								if (*skipStateSelected_ref) // H systemParams.skipStateSelected
								{
									res = hHost_ref->activate_selected_component(tpAll_ref[id]);
									if (res != JVX_NO_ERROR)
									{
										if (report)
										{
											report->postMessageError((std::string("Failed to activate selected ") + jvxComponentIdentification_txt(tpAll_ref[id])).c_str());
											// H this->postMessage_inThread((std::string("Failed to activate selected ") + jvxComponentIdentification_txt(tpAll[id])).c_str(), Qt::red);
										}
									}
								}
							}
						}

						if (report)
						{
							report->updateAllWidgetsOnStateChange();
							// H this->subWidgets.realtimeViewer.props.theWidget->updateAllWidgetsOnStateChange();
							// H this->subWidgets.realtimeViewer.plots.theWidget->updateAllWidgetsOnStateChange();
						}

						jvx_select_next_avail_slot(hHost_ref, tpAll_ref[id].tp, tpAllNext);
					}
					break;
				case 10:

					// =================================
					// SWITCH STATE
					// =================================
					if (!host_features_ref->flag_blockModuleEdit[id])
					{
						stat_new = jvx_idxToState(idSelect);
						hHost_ref->state_selected_component(tpAll_ref[id], &stat0);
						stat = stat0;
						if ((stat == JVX_STATE_SELECTED) && (stat_new == JVX_STATE_ACTIVE))
						{
							res = hHost_ref->activate_selected_component(tpAll_ref[id]);
							if (res != JVX_NO_ERROR)
							{
								if (report)
								{
									report->postMessageError((std::string("Failed to activate selected ") + jvxComponentIdentification_txt(tpAll_ref[id])).c_str());
									// H this->postMessage_inThread((std::string("Failed to activate selected ") + jvxComponentIdentification_txt(tpAll[id])).c_str(), Qt::red);
								}
							}
						}
						if ((stat == JVX_STATE_ACTIVE) && (stat_new == JVX_STATE_SELECTED))
						{
							res = hHost_ref->deactivate_selected_component(tpAll_ref[id]);
							if (res != JVX_NO_ERROR)
							{
								if (report)
								{
									report->postMessageError((std::string("Failed to deactivate selected ") + jvxComponentIdentification_txt(tpAll_ref[id])).c_str());
									// H this->postMessage_inThread((std::string("Failed to deactivate selected ") + jvxComponentIdentification_txt(tpAll[id])).c_str(), Qt::red);
								}
							}
						}

						if (report)
						{
							report->updateAllWidgetsOnStateChange();
							// H this->subWidgets.realtimeViewer.props.theWidget->updateAllWidgetsOnStateChange();
							// H this->subWidgets.realtimeViewer.plots.theWidget->updateAllWidgetsOnStateChange();
						}
					}
					break;

				case 30:

					// ==============================
					// OPEN A DIALOG TO VIEW AND EDIT PROPERTY
					// ==============================

					theProps = NULL;
					theObject = NULL;
					res = hHost_ref->request_object_selected_component(tpAll_ref[id], &theObject);
					res2 = hHost_ref->request_hidden_interface_selected_component(tpAll_ref[id], JVX_INTERFACE_PROPERTIES,
						(jvxHandle**)& theProps);
					if ((res == JVX_NO_ERROR) && (res2 == JVX_NO_ERROR) && theProps && theObject)
					{
						theProps->number_properties(callGate, &num);
						if (res == JVX_NO_ERROR)
						{
							if (JVX_CHECK_SIZE_SELECTED(idSelect) && (idSelect < (jvxInt16)num))
							{					
								ident.idx = idSelect;
								res = theProps->description_property(callGate, theDescr, ident);
								/*idSelect, & cat, & stateMask, & threadingMask, & format, & num, & accessType, & decTp,
									&handleIdx, &ctxt, NULL, &fldStr, NULL, NULL, NULL);*/
								if (res == JVX_NO_ERROR)
								{
									description = "UNKNOWN";
									description = jvxPropertyContext_txt(ctxt);
									description += ": ";
									description += fldStr.std_str();


									switch (theDescr.format)
									{
									case JVX_DATAFORMAT_8BIT:
									case JVX_DATAFORMAT_16BIT_LE:
									case JVX_DATAFORMAT_32BIT_LE:
									case JVX_DATAFORMAT_64BIT_LE:
										if (num == 1)
										{
											integerProperties* theDialog = new integerProperties(
												tpAll_ref[id], idSelect, theDescr.category, 
												theDescr.allowedStateMask, 
												theDescr.allowedThreadingMask,
												theDescr.format, theDescr.num, 
												theDescr.accessType, theDescr.decTp,
												theDescr.globalIdx, theProps, theObject);
											theDialog->init();
											theDialog->exec();
										}
										else
										{
											if (theDescr.decTp == JVX_PROPERTY_DECODER_PLOT_ARRAY)
											{
												// Variant No 2
											}
											else
											{
												// Variant No 3 <- whatever
											}
										}
										break;
									case JVX_DATAFORMAT_DATA:
										if (num == 1)
										{
											floatProperties* theDialog = new floatProperties(
												tpAll_ref[id], idSelect, theDescr.category, theDescr.allowedStateMask, 
												theDescr.allowedThreadingMask, theDescr.format,
												theDescr.num, theDescr.accessType, theDescr.decTp,
												theDescr.globalIdx, theProps, theObject);
											theDialog->init();
											theDialog->exec();
										}
										else
										{
											if (theDescr.decTp == JVX_PROPERTY_DECODER_PLOT_ARRAY)
											{
												// Variant No 2
											}
											else
											{
												// Variant No 3 <- whatever
											}
										}
										break;
									case JVX_DATAFORMAT_SELECTION_LIST:
										// Variant No 4
										break;
									case JVX_DATAFORMAT_VALUE_IN_RANGE:
										// Variant No 5
										break;
									case JVX_DATAFORMAT_STRING:
										if ((theDescr.decTp == JVX_PROPERTY_DECODER_DIRECTORY_SELECT) || 
											(theDescr.decTp == JVX_PROPERTY_DECODER_FILENAME_OPEN) || 
											(theDescr.decTp == JVX_PROPERTY_DECODER_FILENAME_SAVE))
										{
											stringFilenameProperties* theDialog = new stringFilenameProperties(
												tpAll_ref[id], idSelect, theDescr.category, theDescr.allowedStateMask, 
												theDescr.allowedThreadingMask, theDescr.format, theDescr.num, 
												theDescr.accessType, theDescr.decTp, theDescr.globalIdx, theProps, theObject);
											theDialog->init();
											theDialog->exec();
										}
										else
										{
											stringAssignmentProperties* theDialog = new stringAssignmentProperties(
												tpAll_ref[id], idSelect, theDescr.category, theDescr.allowedStateMask, 
												theDescr.allowedThreadingMask, theDescr.format, theDescr.num, 
												theDescr.accessType, theDescr.decTp,
												theDescr.globalIdx, theProps, theObject);
											theDialog->init();
											theDialog->exec();
										}
									}
								}
							}
						}
					}
					if (theObject)
					{
						res = hHost_ref->return_object_selected_component(tpAll_ref[id], theObject);
					}
					if (theProps)
					{
						res2 = hHost_ref->return_hidden_interface_selected_component(tpAll_ref[id], JVX_INTERFACE_PROPERTIES, theProps);
					}
					break;
				
				case 70:

					// =====================================
					// SHOW A MODAL DIALOG TO SHOW COMPONENT INFORMATION
					// =====================================

					theObject = NULL;
					res = hHost_ref->request_object_selected_component(tpAll_ref[id], &theObject);
					if (theObject)
					{
						showInfo = new showInformation(theObject);
						showInfo->init();
						showInfo->exec();
						delete(showInfo);
					}
					break;

				case 100:
					assert(0);
#if 0
					varsCB = senderO->property("callback");
					varsPD = senderO->property("private");
					if (varsCB.isValid() && varsPD.isValid())
					{
						IjvxMainWindowController_report* cbPtr = reinterpret_cast<IjvxMainWindowController_report*>(varsCB.value<void*>());
						jvxHandle* prPtr = varsPD.value<void*>();
						jvxValue var;
						if (prPtr)
						{
							if (act)
							{
								switch (id)
								{
								case 0:
									var.assign(act->isChecked());
									cbPtr->set_variable_edit(prPtr, var, uId);

									cbPtr->get_variable_edit(prPtr, var, uId);
									var.toContent(&valB);
									act->setChecked(valB);
									break;
								case 1:
									varT = act->property("init_txt");
									varI = act->property("current_val");
									valI = varI.toInt();
									getNewValue = new jvx_newvalue_dialog(parentWid, "Specify new value", &valI);
									getNewValue->exec();
									var.assign(valI);
									cbPtr->set_variable_edit(prPtr, var, uId);

									cbPtr->get_variable_edit(prPtr, var, uId);
									var.toContent(&valI);
									act->setProperty("current_val", QVariant(valI));
									showTxt = varT.toString().toLatin1().data();
									showTxt += " -> ";
									showTxt += jvx_int2String(valI);
									act->setText(showTxt.c_str());
									break;
								default:
									break;
								}
							}
							else
							{
								assert(0);
							}
						}
					}
					break;
#endif

				case 120:

					// ============================================
					// SELECT A VALID SLOT
					// ============================================

					if (var_sid.isValid() && var_ssid.isValid())
					{
						id_sid = var_sid.toInt();
						id_ssid = var_ssid.toInt();
						if (JVX_CHECK_SIZE_UNSELECTED(id_ssid))
						{
							tpAll_ref[id].slotid = id_sid;
						}
						else
						{
							tpAll_ref[id].slotsubid = id_ssid;
						}

						hHost_ref->number_slots_component_system((jvxComponentType)id, NULL, NULL, NULL, &childTp);
						if (childTp != JVX_COMPONENT_UNKNOWN)
						{
							tpAll_ref[childTp].slotid = tpAll_ref[id].slotid;
						}
					}
					break;

				case 130:

					// ============================================
					// SELECT NEXT AVAILABLE SLOT
					// ============================================

					if (var_sid.isValid() && var_ssid.isValid())
					{
						id_sid = var_sid.toInt();
						id_ssid = var_ssid.toInt();
						jvx_select_next_avail_slot(hHost_ref, (jvxComponentType)id, tpAllNext);
						if (JVX_CHECK_SIZE_UNSELECTED(id_ssid))
						{
							if (JVX_CHECK_SIZE_SELECTED(tpAllNext[id].slotid))
							{
								tpAll_ref[id] = tpAllNext[id];
							}
						}
						else
						{
							if (JVX_CHECK_SIZE_SELECTED(tpAllNext[id].slotsubid))
							{
								tpAll_ref[id] = tpAllNext[id];
							}
						}
						hHost_ref->number_slots_component_system((jvxComponentType)id, NULL, NULL, NULL, &childTp);
						if (childTp != JVX_COMPONENT_UNKNOWN)
						{
							tpAll_ref[childTp].slotid = tpAll_ref[id].slotid;
						}
					}
					break;

				case 20:
				case 40:
				case 50:
					break;
					assert(0);
					break;
				}
			}
		}
		updateWindow();
	}
}

void
jvx_menu_host_bridge_components::updateWindow_tools()
{
	menuUpdate_tools(menuOther_Components);
}

void
jvx_menu_host_bridge_components::menuUpdate_tools(QMenu* lstNames)
{
	int i;
	jvxSize j;
	jvxApiString fldStr;

	if (hTools_ref)
	{
		lstNames->clear();
		for (i = JVX_COMPONENT_UNKNOWN + 1; i < JVX_COMPONENT_ALL_LIMIT; i++)
		{
			jvxComponentIdentification theTp((jvxComponentType)i, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
			jvxSize num = 0;
			hTools_ref->number_tools(theTp, &num);
			if (num > 0)
			{
				QMenu* newMenu = new QMenu(lstNames);
				newMenu->setTitle(jvxComponentIdentification_txt(theTp).c_str());
				lstNames->addAction(newMenu->menuAction());
				for (j = 0; j < num; j++)
				{
					jvxBool mulInst = false;

					hTools_ref->identification_tool(theTp, j, &fldStr, NULL, &mulInst);
					std::string txt = fldStr.std_str();
					if (mulInst)
						txt += "<m>";
					QAction* act = new QAction(this);
					act->setText(txt.c_str());
					newMenu->addAction(act);
				}
				newMenu = NULL;
			}
		}
	}
}
