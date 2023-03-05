#include "jvx-qt-helpers.h"

void jvx_qt_listComponentsInComboBox_(bool addressesStandardComponents, QComboBox* combo, IjvxHost* hostRef, jvxComponentIdentification& myCp)
{
	jvxSize i, j, k;
	IjvxObject* theObj = NULL;
	jvxComponentIdentification cpTp;
	std::string txt;
	jvxSize szSlots = 0;
	jvxSize szSubSlotId = 0;
	jvxComponentType parTp = JVX_COMPONENT_UNKNOWN;
	jvxApiString strApi;
	combo->clear();
	jvxSize cnt = 0;
	jvxSize idSelect = JVX_SIZE_UNSELECTED;
	jvxErrorType resL = JVX_NO_ERROR;
	QFont myCustomFont_bold;

	myCustomFont_bold.setBold(true);
	myCustomFont_bold.setItalic(false);
	for (i = JVX_COMPONENT_UNKNOWN; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		cpTp.tp = (jvxComponentType)i;
		cpTp.slotid = JVX_SIZE_DONTCARE;
		cpTp.slotsubid = JVX_SIZE_DONTCARE;

		txt = jvxComponentType_txt(cpTp.tp);
		combo->addItem(txt.c_str());
#ifdef JVX_OS_LINUX
		combo->setEditable(true);
#endif
		combo->setItemData(cnt, QVariant((int)cpTp.tp), JVX_USER_ROLE_COMPONENT_ID);
		combo->setItemData(cnt, QVariant(JVX_SIZE_INT(cpTp.slotid)), JVX_USER_ROLE_COMPONENT_SLOT_ID);
		combo->setItemData(cnt, QVariant(JVX_SIZE_INT(cpTp.slotsubid)), JVX_USER_ROLE_COMPONENT_SLOTSUB_ID);

#ifdef JVX_OS_LINUX
		combo->setEditable(false);
#endif
		if (myCp == cpTp)
		{
			idSelect = cnt;
		}
		cnt++;

		hostRef->role_component_system(cpTp.tp, &parTp, nullptr, nullptr);
		hostRef->number_slots_component_system(cpTp, &szSlots, NULL, nullptr, nullptr);

		if (parTp == JVX_COMPONENT_UNKNOWN)
		{
			for (j = 0; j < szSlots; j++)
			{
				cpTp.slotid = j;
				cpTp.slotsubid = 0;
				resL = hostRef->description_selected_component(cpTp, &strApi);
				if (resL == JVX_NO_ERROR)
				{
					txt = "-->" + strApi.std_str() + "<" + jvx_size2String(cpTp.slotid) + "-" + jvx_size2String(cpTp.slotsubid) + ">";
					combo->addItem(txt.c_str());
#ifdef JVX_OS_LINUX
					combo->setEditable(true);
#endif
					combo->setItemData((int)cnt, QVariant(myCustomFont_bold), Qt::FontRole);
#ifdef JVX_OS_LINUX
					combo->setEditable(false);
#endif
#ifdef JVX_OS_LINUX
					combo->setEditable(true);
#endif				
					combo->setItemData(cnt, QVariant((int)cpTp.tp), JVX_USER_ROLE_COMPONENT_ID);
					combo->setItemData(cnt, QVariant(JVX_SIZE_INT(cpTp.slotid)), JVX_USER_ROLE_COMPONENT_SLOT_ID);
					combo->setItemData(cnt, QVariant(JVX_SIZE_INT(cpTp.slotsubid)), JVX_USER_ROLE_COMPONENT_SLOTSUB_ID);
#ifdef JVX_OS_LINUX
					combo->setEditable(false);
#endif

					if (
						(myCp.tp == cpTp.tp) &&
						(myCp.slotid == cpTp.slotid) &&
						(myCp.slotsubid == cpTp.slotsubid))
					{
						idSelect = cnt;
					}
					cnt++;
				}
			}
		}
		else
		{
			for (j = 0; j < szSlots; j++)
			{
				cpTp.slotid = j;
				hostRef->role_component_system(cpTp.tp, &parTp, nullptr, nullptr),
				hostRef->number_slots_component_system(cpTp, NULL, &szSubSlotId, nullptr, nullptr);
				for (k = 0; k < szSlots; k++)
				{
					cpTp.slotid = j;
					cpTp.slotsubid = k;
					resL = hostRef->description_selected_component(cpTp, &strApi);
					if (resL == JVX_NO_ERROR)
					{
						txt = "-->" + strApi.std_str() + "<" + jvx_size2String(cpTp.slotid) + "-" + jvx_size2String(cpTp.slotsubid) + ">";
#ifdef JVX_OS_LINUX
						combo->setEditable(true);
#endif
						combo->setItemData((int)cnt, QVariant(myCustomFont_bold), Qt::FontRole);
#ifdef JVX_OS_LINUX
						combo->setEditable(false);
#endif
#ifdef JVX_OS_LINUX
						combo->setEditable(true);
#endif				

						combo->addItem(txt.c_str());
#ifdef JVX_OS_LINUX
						combo->setEditable(true);
#endif
						combo->setItemData(cnt, QVariant((int)cpTp.tp), JVX_USER_ROLE_COMPONENT_ID);
						combo->setItemData(cnt, QVariant(JVX_SIZE_INT(cpTp.slotid)), JVX_USER_ROLE_COMPONENT_SLOT_ID);
						combo->setItemData(cnt, QVariant(JVX_SIZE_INT(cpTp.slotsubid)), JVX_USER_ROLE_COMPONENT_SLOTSUB_ID);
#ifdef JVX_OS_LINUX
						combo->setEditable(false);
#endif

						if (
							(myCp.tp == cpTp.tp) &&
							(myCp.slotid == cpTp.slotid) &&
							(myCp.slotsubid == cpTp.slotsubid))
						{
							idSelect = cnt;
						}
						cnt++;
					}
				}
			}
		}
	}

	if (JVX_CHECK_SIZE_SELECTED(idSelect))
	{
		combo->setCurrentIndex(idSelect);
	}
	else
	{
		myCp.slotid = JVX_SIZE_UNSELECTED;
		myCp.slotsubid = JVX_SIZE_UNSELECTED;
		myCp.tp = JVX_COMPONENT_UNKNOWN;
		combo->setCurrentIndex(0);
	}
}

void jvx_qt_listProcessesInComboBox_(bool addressesStandardComponents, QComboBox* combo, IjvxHost* hostRef, 
	jvxComponentIdentification& myCp, const std::string& procMatch)
{
	jvxSize i;
	IjvxObject* theObj = NULL;
	jvxComponentIdentification cpTp;
	std::string txt;
	jvxSize szSlots = 0;
	jvxSize szSubSlotId = 0;
	jvxComponentType parTp = JVX_COMPONENT_UNKNOWN;
	jvxApiString strApi;
	combo->clear();
	jvxSize uIdLocal = JVX_SIZE_UNSELECTED;
	jvxSize idSelect = JVX_SIZE_UNSELECTED;
	jvxErrorType resL = JVX_NO_ERROR;
	QFont myCustomFont_bold;
	IjvxDataConnections* theConnections = NULL;
	IjvxDataConnectionProcess* theConnectionProcess = NULL;
	jvxSize numProcs = 0;

	myCustomFont_bold.setBold(true);
	myCustomFont_bold.setItalic(false);

	hostRef->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theConnections));
	if (theConnections)
	{
		theConnections->number_connections_process(&numProcs);
		for (i = 0; i < numProcs; i++)
		{
			theConnections->reference_connection_process(i, &theConnectionProcess);
			if (theConnectionProcess)
			{
				jvxApiString strRet;
				jvxBool processMatches = false;
				theConnectionProcess->match_master(&processMatches, myCp, procMatch.c_str());
				if (processMatches)
				{
					theConnectionProcess->descriptor_connection(&strRet);
					QString qStr = strRet.c_str();
					combo->addItem(qStr);
				}
				theConnections->return_reference_connection_process(theConnectionProcess);
				theConnectionProcess = NULL;
			}
		}
		hostRef->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theConnections));
	}
	combo->setCurrentIndex(0);
}

/*
void jvx_qt_listGroupsInComboBox_(bool addressesStandardComponents, QComboBox* combo, IjvxHost* hostRef,
	jvxComponentIdentification& myCp, const std::string& procMatch)
{
	jvxSize i, j, k;
	IjvxObject* theObj = NULL;
	jvxComponentIdentification cpTp;
	std::string txt;
	jvxSize szSlots = 0;
	jvxSize szSubSlotId = 0;
	jvxComponentType parTp = JVX_COMPONENT_UNKNOWN;
	jvxApiString strApi;
	combo->clear();
	jvxSize uIdLocal = JVX_SIZE_UNSELECTED;
	jvxSize idSelect = JVX_SIZE_UNSELECTED;
	jvxErrorType resL = JVX_NO_ERROR;
	QFont myCustomFont_bold;
	IjvxDataConnections* theConnections = NULL;
	IjvxDataConnectionGroup* theConnectionGroup = NULL;
	jvxSize numProcs = 0;

	myCustomFont_bold.setBold(true);
	myCustomFont_bold.setItalic(false);

	hostRef->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theConnections));
	if (theConnections)
	{
		theConnections->number_connections_group(&numProcs);
		for (i = 0; i < numProcs; i++)
		{
			theConnections->reference_connection_group(i, &theConnectionGroup);
			if (theConnectionGroup)
			{
				jvxApiString strRet;
				jvxBool processMatches = false;
				theConnectionGroup->match_master(&processMatches, myCp, procMatch.c_str());
				if (processMatches)
				{
					theConnectionProcess->descriptor_connection(&strRet, NULL);
					QString qStr = strRet.c_str();
					combo->addItem(qStr);
				}
				theConnections->return_reference_connection_process(theConnectionProcess);
				theConnectionProcess = NULL;
			}
		}
		hostRef->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theConnections));
	}
	combo->setCurrentIndex(0);
}
*/

void jvx_qt_listComponentsInComboBox(bool addressesStandardComponents, QComboBox* combo, IjvxHost* hostRef, jvxComponentIdentification myCp, 
	jvxComponentIdentification* spSelect)
{
	jvxSize i;
	IjvxObject* theObj = NULL;
	jvxComponentIdentification cpTp;

	combo->clear();
	for(i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		combo->addItem(jvxComponentType_txt(i));
	}

	for(i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		QFont myCustomFont;

		// Ok, we know that this component has been selected
		theObj = NULL;

		cpTp = spSelect[i];
		jvx_getReferenceComponentType(cpTp, theObj, addressesStandardComponents, hostRef);

		if(theObj)
		{
			myCustomFont.setBold(true);
			myCustomFont.setItalic(false);
			jvx_returnReferenceComponentType(cpTp, theObj, addressesStandardComponents, hostRef);
		}
		else
		{
			myCustomFont.setBold(false);
			myCustomFont.setItalic(true);
		}
#ifdef JVX_OS_LINUX
		combo->setEditable(true);
#endif
		combo->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
		combo->setEditable(false);
#endif
	}
	combo->setCurrentIndex((int)myCp.tp);
}

void jvx_qt_listPropertiesInComboBox(jvxComponentIdentification tp, jvxPropertyContext ctxtShow, jvxInt16 whatToShow, QComboBox* combo, IjvxHost* hostRef, jvxSize& indexShow, jvxSize* indexComponent, jvxRealtimeViewerFilterShow filter)
{
	jvxSize i;
	jvx_propertyReferenceTriple theTriple;
	jvxSize num = 0;

	jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
	jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);

	jvxCallManagerProperties callGate; 
	bool conditionForAdd = false;
	std::string entry;
	QFont myCustomFont;
	jvxBool showActiveOnly = false;

	jvx_initPropertyReferenceTriple(&theTriple);
	combo->clear();

	jvx_getReferencePropertiesObject(hostRef, &theTriple, tp);

	if(theTriple.theProps)
	{
		theTriple.theProps->number_properties(callGate, &num);
		if(num > 0)
		{
			for(i = 0; i < num; i++)
			{
				entry = "UNKNOWN";
				ident.idx = i;
				theTriple.theProps->description_property(callGate, theDescr, ident);

				switch (whatToShow)
				{
				case 1:
					entry = theDescr.name.std_str();
					break;
				case 2:
					entry = theDescr.description.std_str();
					break;
				default:
					entry = theDescr.descriptor.std_str();
				}
				conditionForAdd = jvx_applyPropertyFilter(theDescr.format, theDescr.num, theDescr.decTp, filter);

				if(conditionForAdd)
				{
					if(ctxtShow != JVX_PROPERTY_CONTEXT_UNKNOWN)
					{
						showActiveOnly = true;
						if(theDescr.ctxt != ctxtShow)
						{
							conditionForAdd = false;
						}
					}
				}
				if(conditionForAdd)
				{
					combo->addItem(entry.c_str());
					combo->setItemData(combo->count()-1, QVariant((int)i), Qt::UserRole);

					myCustomFont.setBold(true);
					myCustomFont.setItalic(false);
#ifdef JVX_OS_LINUX
					combo->setEditable(true);
#endif
					combo->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
					combo->setEditable(false);
#endif
				}
				else
				{
					if(!showActiveOnly)
					{
						combo->addItem(entry.c_str());
						combo->setItemData(combo->count()-1, QVariant((int)i), Qt::UserRole);

						myCustomFont.setBold(false);
						myCustomFont.setItalic(true);
#ifdef JVX_OS_LINUX
						combo->setEditable(true);
#endif
						combo->setItemData((int)i, QVariant(myCustomFont), Qt::FontRole);
#ifdef JVX_OS_LINUX
						combo->setEditable(false);
#endif
					}
				}
			}
			if(combo->count() >= 1)
			{
				if(indexShow >= (jvxSize)combo->count())
				{
					indexShow = 0;
				}
				combo->setCurrentIndex((int)indexShow);
				if(indexComponent)
				{
					QVariant var = combo->itemData((int)indexShow, Qt::UserRole);
					*indexComponent = var.toInt();
				}
			}
			else
			{
				indexShow = (jvxSize)-1;
				if(indexComponent)
				{
					*indexComponent = (jvxSize)-1;
				}
			}
		}
		else
		{
			indexShow = (jvxSize)-1;
			if(indexComponent)
			{
				*indexComponent = (jvxSize)-1;
			}
		}
		jvx_returnReferencePropertiesObject(hostRef, &theTriple, tp);
	}
	else
	{
		indexShow = (jvxSize)-1;
		if(indexComponent)
		{
			*indexComponent = (jvxSize)-1;
		}
	}
}

void jvx_qt_propertiesIndexShowForIndexComponent(jvxComponentIdentification tp, IjvxHost* hostRef, jvxSize& indexShow, jvxComponentIdentification myCp, jvxRealtimeViewerFilterShow filter)
{
	jvxSize i;
	jvx_propertyReferenceTriple theTriple;
	jvxSize num = 0;
	jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
	jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);
	jvxCallManagerProperties callGate;

	bool conditionForAdd = false;
	std::string entry;
	int cnt = 0;
	indexShow = -1;

	jvx_initPropertyReferenceTriple(&theTriple);
	jvx_getReferencePropertiesObject(hostRef, &theTriple, tp);

	if(theTriple.theProps)
	{
		theTriple.theProps->number_properties(callGate, &num);
		if(num > 0)
		{
			for(i = 0; i < num; i++)
			{
				ident.idx = i;
				theTriple.theProps->description_property(callGate, theDescr, ident);

				/*
				if(fldStr)
				{
					entry = fldStr->bString;
					entry = jvxPropertyContext_prefix(ctxt) + entry;
					theTriple.theObj->deallocate(fldStr);
				}
				*/
				conditionForAdd = jvx_applyPropertyFilter(theDescr.format, theDescr.num, theDescr.decTp, filter);

				if(conditionForAdd)
				{
					if(i == myCp.tp)
					{
						indexShow = cnt;
					}
				}
				cnt++;
			}
		}
		jvx_returnReferencePropertiesObject(hostRef, &theTriple, tp);
	}
}

