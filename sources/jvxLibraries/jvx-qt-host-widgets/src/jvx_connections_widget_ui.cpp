#include "jvx_connections_widget.h"
#include "CjvxJson.h"

#include "jvx_connections_widget_const.h"

void
jvx_connections_widget::ui_redraw_process_list()
{
	std::string tooltip = "--";
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize i;
	jvxSize numP = 0;
	jvxSize numG = 0;
	jvxApiString pName;
	jvxState stat = JVX_STATE_NONE;
	jvxSize uId = 0;
	jvxSize catId = JVX_SIZE_UNSELECTED;
	jvxSize uIdDep = JVX_SIZE_UNSELECTED;
	jvxBool testOk = true;
	jvxBool essential = false;
	IjvxDataConnectionProcess* theProcess = NULL;
	IjvxDataConnectionGroup* theGroup = NULL;
	QTreeWidgetItem* it_select = NULL;
	jvxBool preventConfig = false;
	jvxSize ruleId;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	treeWidget_connections->clear();
	QTreeWidgetItem* newItem = new QTreeWidgetItem(treeWidget_connections);
	newItem->setText(0, "--");
	newItem->setText(1, "None selected");
	newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROCESS_UID, QVariant(-1));
	newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_WIDGET_ID, QVariant(-1));
	newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_CONN_TYPE, QVariant(QVariant(JVX_CONNECTION_WIDGET_IS_A_PROCESS))); // <- Default is process

	it_select = newItem;

	if (theDataConnections)
	{
		theDataConnections->number_connections_process(&numP);
		for (i = 0; i < numP; i++)
		{
			theDataConnections->reference_connection_process(i, &theProcess);
			assert(theProcess);

			if (i == id_select_process_group.id_proc_grp)
			{
				it_select = newItem;
			}

			theProcess->descriptor_connection(&pName);
			theProcess->misc_connection_parameters(&ruleId, nullptr);
			theProcess->category_id(&catId);
			theProcess->depends_on_process(&uIdDep);
			newItem = new QTreeWidgetItem(treeWidget_connections);
			theDataConnections->uid_for_reference(theProcess, &uId);
			theProcess->marked_essential_for_start(&essential);
			newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROCESS_UID, QVariant(JVX_SIZE_INT(uId)));
			newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_WIDGET_ID, QVariant(JVX_SIZE_INT(i)));
			newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_CONN_TYPE, QVariant(JVX_CONNECTION_WIDGET_IS_A_PROCESS));
			newItem->setText(0, ("Process #" + jvx_size2String(i)).c_str());
			if (JVX_CHECK_SIZE_SELECTED(ruleId))
			{
				newItem->setText(1, (pName.std_str() + "<A" + jvx_size2String(ruleId) + ">").c_str());
			}
			else
			{
				newItem->setText(1, pName.c_str());
			}
			if(essential)
			{
				newItem->setText(2, "<E>");
			}
			else
			{
				newItem->setText(2, "--");
			}
			newItem->setText(3, jvx_size2String(uId).c_str());
			if (JVX_CHECK_SIZE_SELECTED(catId))
			{
				newItem->setText(4, jvx_size2String(catId).c_str());
			}
			else
			{
				newItem->setText(4, "--");
			}
			if (JVX_CHECK_SIZE_SELECTED(uIdDep))
			{
				newItem->setText(5, jvx_size2String(uIdDep).c_str());
			}
			else
			{
				newItem->setText(5, "--");
			}

			newItem->setExpanded(true);

			QTreeWidgetItem* newSubItem = new QTreeWidgetItem(newItem);
			theProcess->status(&stat);
			
			theProcess->status_chain(&testOk JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
			if (stat >= JVX_STATE_ACTIVE)
			{
				std::string txt;
				newSubItem->setText(0, "connected");
				/*
				CjvxJsonElementList jsonElmLst;
				resL = theProcess->transfer_forward_chain(JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON, &jsonElmLst JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				assert(resL == JVX_NO_ERROR);


				jsonElmLst.printToStringView(txt, JVX_NO_ERROR);
				*/if (testOk == false)
				{
					txt = "Status -> Error.";
				}
				else
				{
					txt = "Status -> Ok.";
				}
				tooltip = txt;
			}
			else
			{
				newSubItem->setText(0, "not connected");
			}
			newItem->setToolTip(1, tooltip.c_str());
			theDataConnections->return_reference_connection_process(theProcess);
		}

		// ====================================================================================

		theDataConnections->number_connections_group(&numG);
		for (i = 0; i < numG; i++)
		{
			theDataConnections->reference_connection_group(i, &theGroup);
			assert(theGroup);

			if (i == id_select_process_group.id_proc_grp)
			{
				it_select = newItem;
			}

			theGroup->descriptor_connection(&pName);
			theGroup->misc_connection_parameters(& ruleId, nullptr);
			newItem = new QTreeWidgetItem(treeWidget_connections);
			theDataConnections->uid_for_reference(theGroup, &uId);
			newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROCESS_UID, QVariant(JVX_SIZE_INT(uId)));
			newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_WIDGET_ID, QVariant(JVX_SIZE_INT(i)));
			newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_CONN_TYPE, QVariant(JVX_CONNECTION_WIDGET_IS_A_GROUP));
			newItem->setText(0, ("Group #" + jvx_size2String(i)).c_str());
			if (JVX_CHECK_SIZE_SELECTED(ruleId))
			{
				newItem->setText(1, (pName.std_str() + "<A" + jvx_size2String(ruleId) + ">").c_str());
			}
			else
			{
				newItem->setText(1, pName.c_str());
			}
			newItem->setExpanded(true);

			QTreeWidgetItem* newSubItem = new QTreeWidgetItem(newItem);
			theGroup->status(&stat);
			if (stat >= JVX_STATE_ACTIVE)
			{
				std::string txt;
				newSubItem->setText(0, "connected");
				tooltip = "connected";
			}
			else
			{
				newSubItem->setText(0, "not connected");
				tooltip = "not connected";
			}
			newItem->setToolTip(1, tooltip.c_str());
			theDataConnections->return_reference_connection_group(theGroup);
		}
	}
	if (it_select)
	{
		treeWidget_connections->setCurrentItem(it_select);
	}
}

void 
jvx_connections_widget::ui_get_reference_process(IjvxDataConnectionProcess** theProcess_select, IjvxDataConnectionGroup** theGroup_select, jvxSize* select_id_proc)
{
	jvxSize numP = 0;
	jvxSize numG = 0;

	if(theProcess_select)
		*theProcess_select = NULL;

	if(theGroup_select)
		*theGroup_select = NULL;

	// ===================================================================
	// Get the reference to the selected process if any
	// ===================================================================
	if (theDataConnections)
	{
		theDataConnections->number_connections_process(&numP);
		theDataConnections->number_connections_group(&numG);
	}

	// Select one option in range of valid options
	if (JVX_CHECK_SIZE_SELECTED(id_select_process_group.id_proc_grp))
	{
		switch (id_select_process_group.proc_grp_tp)
		{
		case JVX_CONNECTION_WIDGET_IS_A_PROCESS:
			if (numP == 0)
			{
				id_select_process_group.id_proc_grp = JVX_SIZE_UNSELECTED;
			}
			else
			{
				id_select_process_group.id_proc_grp = JVX_MIN(id_select_process_group.id_proc_grp, (numP)-1);
			}

			// If a process has been selected, get the reference and show contents/properties
			if (id_select_process_group.id_proc_grp < numP)
			{
				theDataConnections->reference_connection_process(id_select_process_group.id_proc_grp, theProcess_select);
			}
			*select_id_proc = (int)id_select_process_group.id_proc_grp + 1;
			break;
		case JVX_CONNECTION_WIDGET_IS_A_GROUP:
			if (numG == 0)
			{
				id_select_process_group.id_proc_grp = JVX_SIZE_UNSELECTED;
			}
			else
			{
				id_select_process_group.id_proc_grp = JVX_MIN(id_select_process_group.id_proc_grp, (numP + numG) - 1);
			}
			if (id_select_process_group.id_proc_grp < numG)
			{
				theDataConnections->reference_connection_group(id_select_process_group.id_proc_grp, theGroup_select);
			}
			*select_id_proc = (int)id_select_process_group.id_proc_grp + numP + 1;
			break;
		}
	}

	// what is used at different places...
	// None option is there too
}

void
jvx_connections_widget::ui_refine_process_list(jvxSize select_id_proc)
{
	jvxSize i;
	pushButton_procremove->setEnabled(false);
	pushButton_procdisc->setEnabled(false);
	jvxSize cnt = treeWidget_connections->topLevelItemCount();
	for (i = 0; i < cnt; i++)
	{
		QTreeWidgetItem* it = treeWidget_connections->topLevelItem(JVX_SIZE_INT(i));
		if (it)
		{
			if (i == select_id_proc)
			{
				it->setBackground(0, QBrush(Qt::gray));
			}
			else
			{
				it->setBackground(0, QBrush(Qt::white));
			}

			QVariant var = it->data(0, JVX_USER_ROLE_TREEWIDGET_PROCESS_UID);
			QVariant pvar = it->data(0, JVX_USER_ROLE_TREEWIDGET_CONN_TYPE);
			jvxSize uId = JVX_SIZE_UNSELECTED;
			assert(var.isValid());
			assert(pvar.isValid());

			uId = (jvxSize)var.toInt();
			jvxConnectionWidgetProcessGroupType pTp = (jvxConnectionWidgetProcessGroupType)pvar.toInt();

			IjvxDataConnectionProcess* theProc = NULL;
			IjvxDataConnectionGroup* theGrp = NULL;
			jvxState stat = JVX_STATE_NONE;

			if (pTp == JVX_CONNECTION_WIDGET_IS_A_PROCESS)
			{
				theDataConnections->reference_connection_process_uid(uId, &theProc);
				if (theProc)
				{
					theProc->status(&stat);
					theProc->status_chain(NULL JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
				}
			}
			else
			{
				theDataConnections->reference_connection_group_uid(uId, &theGrp);
				if (theGrp)
				{
					theGrp->status(&stat);
				}
			}

			switch (stat)
			{
			case JVX_STATE_SELECTED:
				it->setBackground(1, QBrush(Qt::yellow));
				it->setForeground(1, QBrush(Qt::black));
				break;
			case JVX_STATE_ACTIVE:
				it->setBackground(1, QBrush(Qt::blue));
				it->setForeground(1, QBrush(Qt::white));
				break;
			case JVX_STATE_PREPARED:
			case JVX_STATE_PROCESSING:
				it->setBackground(1, QBrush(Qt::green));
				it->setForeground(1, QBrush(Qt::red));
				break;
			default:
				it->setBackground(1, QBrush(Qt::white));
				it->setForeground(1, QBrush(Qt::black));
				break;
			}

			if (i == select_id_proc)
			{
				switch (stat)
				{
				case JVX_STATE_INIT:
					pushButton_procremove->setEnabled(true);
					break;
				case JVX_STATE_ACTIVE:
					pushButton_procdisc->setEnabled(true);
					break;
				case JVX_STATE_PREPARED:
				case JVX_STATE_PROCESSING:
					break;
				default:
					break;
				}
			}
			if (theProc)
			{
				theDataConnections->return_reference_connection_process(theProc);
				theProc = NULL;
			}
			if(theGrp)
			{
				theDataConnections->return_reference_connection_group(theGrp);
				theProc = NULL;
			}
		}
	}
	for (i = 0; i < treeWidget_connections->columnCount(); i++)
	{
		treeWidget_connections->resizeColumnToContents(JVX_SIZE_INT(i));
	}
}

void
jvx_connections_widget::ui_redraw_factories(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select)
{
	jvxSize i,j;
	jvxSize numMF = 0;
	jvxSize numCF = 0;
	jvxSize numCM = 0;
	jvxSize numIC = 0;
	jvxSize numOC = 0;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxComponentIdentification tpId;
	jvxApiString strMF;
	jvxApiString strMFd;
	jvxApiString condescr;
	std::string txt;

	treeWidget_masterfactories->clear();
	theDataConnections->number_connection_master_factories(&numMF);
	for (i = 0; i < numMF; i++)
	{
		IjvxObject* obj = nullptr;
		IjvxConnectionMasterFactory* theFac = NULL;
		theDataConnections->reference_connection_master_factory(i, &theFac, &uId);
		jvx_request_interfaceToObject(theFac, &obj, &tpId, &strMF, &strMFd);

		QTreeWidgetItem* newItem = new QTreeWidgetItem(treeWidget_masterfactories);
		txt = "Factory #" + jvx_size2String(i);
		newItem->setText(0, txt.c_str());
		
		txt = strMF.std_str();
		newItem->setText(1, txt.c_str());

		// ================================================================================
		// Trying to use "Nickname" instead of descriptor
		// ================================================================================
		IjvxProperties* props = reqInterfaceObj<IjvxProperties>(obj);
		if (props)
		{
			jvxApiString astr;
			jvxCallManagerProperties callgate;
			jPAD ident("/system/nickname");

			if (props->get_property(callgate, jPRIO<jvxApiString>(astr), ident) == JVX_NO_ERROR)
			{
				if (!astr.std_str().empty())
				{
					txt = astr.std_str();
					auto ft = newItem->font(1);
					ft.setItalic(true);
					newItem->setFont(1, ft);
					newItem->setText(1, txt.c_str());
				}
			}
			retInterfaceObj<IjvxProperties>(obj, props);
		}

		txt = strMFd.std_str();
		newItem->setToolTip(1, txt.c_str());

		txt = jvxComponentIdentification_txt(tpId);
		newItem->setText(2, txt.c_str());

		newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_MF_UID, QVariant(JVX_SIZE_INT(uId)));
		newItem->setExpanded(true);

		theFac->number_connector_masters(&numCM);
		for (j = 0; j < numCM; j++)
		{
			IjvxConnectionMaster* master = NULL;
			jvxApiString masdescr;
			IjvxDataConnectionProcess* theProcess_loc = NULL;
			theFac->reference_connector_master(j, &master);
			master->descriptor_master(&masdescr);
			QTreeWidgetItem* newSubItem = new QTreeWidgetItem(newItem);
			newSubItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_MF_UID, QVariant(JVX_SIZE_INT(uId)));
			newSubItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_F_ID, QVariant(JVX_SIZE_INT(j)));
			master->associated_process(&theProcess_loc);

			txt = "Master #" + jvx_size2String(j);
			newSubItem->setText(0, txt.c_str());
			txt = masdescr.std_str();
			newSubItem->setText(1, txt.c_str());
			if (theProcess_loc == NULL)
			{
				newSubItem->setBackground(1, QBrush(Qt::green));
			}
			else
			{
				if (theProcess_loc == theProcess_select)
				{
					newSubItem->setBackground(0, QBrush(Qt::gray));
				}
				else
				{
					newSubItem->setBackground(0, QBrush(Qt::white));
				}
				newSubItem->setBackground(1, QBrush(Qt::red));
			}
			theFac->return_reference_connector_master(master);
		}

		theDataConnections->return_reference_connection_master_factory(theFac);
	}
	for (i = 0; i < treeWidget_masterfactories->columnCount(); i++)
	{
		treeWidget_masterfactories->resizeColumnToContents(JVX_SIZE_INT(i));
	}
	
	// ============================================================================

	treeWidget_connectorfactories->clear();
	theDataConnections->number_connection_factories(&numCF);
	for (i = 0; i < numCF; i++)
	{
		IjvxObject* obj = nullptr;
		IjvxConnectorFactory* theFac = NULL;
		theDataConnections->reference_connection_factory(i, &theFac, &uId);
		jvx_request_interfaceToObject(theFac, &obj, &tpId, &strMF, &strMFd);

		QTreeWidgetItem* newItem = new QTreeWidgetItem(treeWidget_connectorfactories);
		txt = "Factory #" + jvx_size2String(i);
		newItem->setText(0, txt.c_str());

		// Position "1": Descriptor/module name
		txt = strMF.std_str();
		newItem->setText(1, txt.c_str());

		// ================================================================================
		// Trying to use "Nickname" instead of descriptor
		// ================================================================================
		IjvxProperties* props = reqInterfaceObj<IjvxProperties>(obj);
		if (props)
		{
			jvxApiString astr;
			jvxCallManagerProperties callgate;
			jPAD ident("/system/nickname");

			if (props->get_property(callgate, jPRIO<jvxApiString>(astr), ident) == JVX_NO_ERROR)
			{
				if (!astr.std_str().empty())
				{
					txt = astr.std_str();
					auto ft = newItem->font(1);
					ft.setItalic(true);
					newItem->setFont(1, ft);
					newItem->setText(1, txt.c_str());
				}
			}
			retInterfaceObj<IjvxProperties>(obj, props);
		}

		// Add the description as tooltip
		txt = strMFd.std_str();
		newItem->setToolTip(1, txt.c_str());

		// Position "2"
		txt = jvxComponentIdentification_txt(tpId);
		newItem->setText(2, txt.c_str());

		newItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_CF_UID, QVariant(JVX_SIZE_INT(uId)));
		newItem->setExpanded(true);

		theFac->number_input_connectors(&numIC);
		for (j = 0; j < numIC; j++)
		{
			IjvxInputConnectorSelect* icS = NULL;
			IjvxInputConnector* ic = NULL;
			IjvxOutputConnector* oc = NULL;
			IjvxDataConnectionCommon* assComm = NULL;
			jvxApiString masdescr;
			jvxBool isAvail = false;
			theFac->reference_input_connector(j, &icS);
			icS->descriptor_connector(&condescr);
			QTreeWidgetItem* newSubItem = new QTreeWidgetItem(newItem);
			newSubItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_CF_UID, QVariant(JVX_SIZE_INT(uId)));
			newSubItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_IC_ID, QVariant(JVX_SIZE_INT(j)));

			// Check if a (possible) select connector also is a full connector and work with it
			ic = icS->reference_icon();
			if (ic)
			{
				ic->associated_connection_icon(&assComm);
				ic->connected_ocon(&oc);
			}

			txt = "Input Connector #" + jvx_size2String(j);
			newSubItem->setText(0, txt.c_str());

			// ================================================================================
			// Show connection counterpart in tooltip
			// ================================================================================

			if (oc)
			{
				jvxApiString astr;
				oc->descriptor_connector(&astr);
				jvx_commonConnectorToObjectDescription(oc, txt, astr.std_str());
				newSubItem->setToolTip(1, txt.c_str());
			}

			txt = condescr.std_str();
			newSubItem->setText(1, txt.c_str());
			if (assComm == NULL)
			{
				newSubItem->setBackground(1, QBrush(Qt::green));
			}
			else
			{
				if (static_cast<IjvxDataConnectionCommon*>(theProcess_select) == assComm)
				{
					newSubItem->setBackground(0, QBrush(Qt::gray));
				}
				else
				{
					newSubItem->setBackground(0, QBrush(Qt::white));
				}

				newSubItem->setBackground(1, QBrush(Qt::red));
			}
			theFac->return_reference_input_connector(icS);
		}

		theFac->number_output_connectors(&numOC);
		for (j = 0; j < numOC; j++)
		{
			IjvxOutputConnectorSelect* ocS = NULL;
			IjvxOutputConnector* oc = NULL;
			IjvxInputConnector* ic = NULL;
			IjvxDataConnectionCommon* assComm = NULL;
			jvxApiString masdescr;
			jvxBool isAvail = false;
			theFac->reference_output_connector(j, &ocS);
			ocS->descriptor_connector(&condescr);
			QTreeWidgetItem* newSubItem = new QTreeWidgetItem(newItem);
			newSubItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_CF_UID, QVariant(JVX_SIZE_INT(uId)));
			newSubItem->setData(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_OC_ID, QVariant(JVX_SIZE_INT(j)));

			// Check if a (possible) select connector also is a full connector and work with it
			oc = ocS->reference_ocon();
			if (oc)
			{
				oc->associated_connection_ocon(&assComm);
				oc->connected_icon(&ic);
			}
						
			// ================================================================================
			// Show connection counterpart in tooltip
			// ================================================================================
			if (ic)
			{
				jvxApiString astr;
				ic->descriptor_connector(&astr);
				jvx_commonConnectorToObjectDescription(ic, txt, astr.std_str());
				newSubItem->setToolTip(1, txt.c_str());
			}

			txt = "Output Connector #" + jvx_size2String(j);
			newSubItem->setText(0, txt.c_str());
			txt = condescr.std_str();
			newSubItem->setText(1, txt.c_str());
			if (assComm == NULL)
			{
				newSubItem->setBackground(1, QBrush(Qt::green));
			}
			else
			{
				if (static_cast<IjvxDataConnectionCommon*>(theProcess_select) == assComm)
				{
					newSubItem->setBackground(0, QBrush(Qt::gray));
				}
				else
				{
					newSubItem->setBackground(0, QBrush(Qt::white));
				}

				newSubItem->setBackground(1, QBrush(Qt::red));
			}
			theFac->return_reference_output_connector(ocS);
		}
		theDataConnections->return_reference_connection_factory(theFac);
	}//for (i = 0; i < numCF; i++)
	for (i = 0; i < treeWidget_connectorfactories->columnCount(); i++)
	{
		treeWidget_connectorfactories->resizeColumnToContents(JVX_SIZE_INT(i));
	}
}

void
jvx_connections_widget::ui_redraw_dropzone(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select)
{
	jvxSize i;
	jvxState stat = JVX_STATE_NONE;
	IjvxConnectionMaster* theMaster = NULL;
	IjvxConnectionMasterFactory* masFac = NULL;
	jvxComponentIdentification tpId;
	jvxApiString strMF;
	jvxApiString mas_descr;
	jvxApiString con_descr;
	std::string txt;
	std::string txtin;
	std::string txtout;
	jvxSize numB = 0;
	jvxSize cnt = 0;
	IjvxConnectorBridge* bridg = NULL;
	jvxBool ic_act = false;

	std::list<std::string> inConnectors;
	std::list<std::string> outConnectors;
	// =========================================================================
	lineEdit_dz_procname->setEnabled(true);
	lineEdit_dz_procdescr->setEnabled(true);
	listWidget_dz_master->setEnabled(true);
	listWidget_dz_icon->setEnabled(true);
	listWidget_dz_outcon->setEnabled(true);
	listWidget_dz_bridges->setEnabled(true);
	pushButton_dz_create_bridge->setEnabled(true);
	pushButton_dz_create_proc->setEnabled(true);
	comboBox_proc_grp_mode->setEnabled(true);
	checkBox_dz_ic->setEnabled(true);
	pushButton_connect_proc->setEnabled(false);
	pushButton_clear_dz->setEnabled(true);
	pushButton_dz_clear_master->setEnabled(true);
	pushButton_dz_clear_icon->setEnabled(true);
	pushButton_dz_clear_ocon->setEnabled(true);
	pushButton_dz_clear_bridge->setEnabled(true);

	listWidget_dz_master->clear();
	listWidget_dz_bridges->clear();
	listWidget_dz_icon->clear();
	listWidget_dz_outcon->clear();

	pushButton_addconnector->setEnabled(false);
	pushButton_setmaster->setEnabled(false);

	IjvxDataConnectionGroup* vPointer = NULL;

	if (theProcess_select || theGroup_select)
	{
		if (theProcess_select)
		{
			theProcess_select->descriptor_connection(&strMF);
		}
		else
		{
			theGroup_select->descriptor_connection(&strMF);
		}
		lineEdit_dz_procname->setText(strMF.c_str());
		lineEdit_dz_procname->setEnabled(false);

		if (theProcess_select)
		{
			theProcess_select->descriptor(&strMF);
		}
		else
		{
			theGroup_select->descriptor(&strMF);
		}

		lineEdit_dz_procdescr->setText(strMF.c_str());
		lineEdit_dz_procdescr->setEnabled(false);

		if (theProcess_select)
		{
			theProcess_select->status(&stat);
			theProcess_select->status_chain(NULL JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		}
		else
		{
			theGroup_select->status(&stat);
		}

		pushButton_connect_proc->setEnabled(true);
		pushButton_dz_create_proc->setEnabled(false);
		comboBox_proc_grp_mode->setEnabled(false);

		if (theProcess_select)
		{
			theProcess_select->interceptors_active(&ic_act);
		}
		else
		{ 
			theGroup_select->interceptors_active(&ic_act);
		}
		
		checkBox_dz_ic->setEnabled(false);
		checkBox_dz_ic->setChecked(ic_act);

		if (theProcess_select)
		{
			comboBox_proc_grp_mode->setCurrentIndex(JVX_CONNECTION_WIDGET_IS_A_PROCESS);
		}
		else
		{
			comboBox_proc_grp_mode->setCurrentIndex(JVX_CONNECTION_WIDGET_IS_A_GROUP);
		}
		if (stat >= JVX_STATE_ACTIVE)
		{
			/*
			listWidget_dz_master->setEnabled(false);
			listWidget_dz_icon->setEnabled(false);
			listWidget_dz_outcon->setEnabled(false);
			listWidget_dz_bridges->setEnabled(false);
			*/
			pushButton_connect_proc->setEnabled(false);
			pushButton_dz_create_bridge->setEnabled(false);
			pushButton_clear_dz->setEnabled(false);

			pushButton_dz_clear_master->setEnabled(false);
			pushButton_dz_clear_icon->setEnabled(false);
			pushButton_dz_clear_ocon->setEnabled(false);
			pushButton_dz_clear_bridge->setEnabled(false);

			if (theProcess_select)
			{
				vPointer = static_cast<IjvxDataConnectionGroup*>(theProcess_select);
				theProcess_select->associated_master(&theMaster);
				if (theMaster)
				{
					theMaster->parent_master_factory(&masFac);
					jvx_request_interfaceToObject(masFac, NULL, &tpId, &strMF);
					theMaster->descriptor_master(&mas_descr);

					txt = mas_descr.std_str() + ":" + jvxComponentIdentification_txt(tpId) + ":" + strMF.std_str();

					listWidget_dz_master->addItem(txt.c_str());
				}
				listWidget_dz_master->setEnabled(true);

			}
			else
			{
				vPointer = theGroup_select;
				listWidget_dz_master->clear();
				listWidget_dz_master->setEnabled(false);
			}
				
			vPointer->number_bridges(&numB);
			for (i = 0; i < numB; i++)
			{
				vPointer->reference_bridge(i, &bridg);
				if (bridg)
				{
					jvxBool ded_thread = false;
					jvxBool boost_thread = false;
					jvxBool interceptor = false;
					IjvxOutputConnector* conFrom = NULL;
					IjvxInputConnector* conTo = NULL;
					IjvxConnectorFactory* my_parent = NULL;
					bridg->descriptor_bridge(&strMF);
					bridg->properties_bridge(&ded_thread, &boost_thread, &interceptor);
					std::string nmb = strMF.std_str();
					if (ded_thread)
					{
						nmb += "+t";
						if (boost_thread)
						{
							nmb += "+b";
						}
					}

					QListWidgetItem* newItem = new QListWidgetItem(listWidget_dz_bridges);
					QFont ft = newItem->font();
					if (interceptor)
					{
						ft.setItalic(true);
					}
					else
					{
						ft.setItalic(false);
					}

					newItem->setData(JVX_USER_ROLE_LISTWIDGET_ID_BRIDGE_ID, QVariant(JVX_SIZE_INT(i)));
					newItem->setText(nmb.c_str());

					bridg->reference_connect_from(&conFrom);

					txtout = connectorToExpression(conFrom);
					outConnectors.push_back(txtout);

					txt = txtout;
					newItem->setToolTip(txt.c_str());

					bridg->return_reference_connect_from(conFrom);

					bridg->reference_connect_to(&conTo);

					txtin = connectorToExpression(conTo);
					inConnectors.push_back(txtin);

					txt = txtout + "-->" + txtin;

					newItem->setToolTip(txt.c_str());

					bridg->return_reference_connect_to(conTo);

					listWidget_dz_bridges->addItem(newItem);

					vPointer->return_reference_bridge(bridg);
				}
			}

			cnt = 0;
			std::list<std::string>::iterator elmI = inConnectors.begin();
			for (; elmI != inConnectors.end(); elmI++)
			{
				QListWidgetItem* newItem = new QListWidgetItem(listWidget_dz_icon);
				newItem->setData(JVX_USER_ROLE_LISTWIDGET_ID_ICON_ID, QVariant(JVX_SIZE_INT(cnt)));
				newItem->setText(elmI->c_str());
				listWidget_dz_icon->addItem(newItem);
				cnt++;
			}
			std::list<std::string>::iterator elmO = outConnectors.begin();
			for (; elmO != outConnectors.end(); elmO++)
			{
				QListWidgetItem* newItem = new QListWidgetItem(listWidget_dz_outcon);
				newItem->setData(JVX_USER_ROLE_LISTWIDGET_ID_OCON_ID, QVariant(JVX_SIZE_INT(cnt)));
				newItem->setText(elmO->c_str());
				listWidget_dz_outcon->addItem(newItem);
				cnt++;
			}
			
		} // if (stat >= JVX_STATE_ACTIVE)
	} // if (theProcess_select || theGroup_select)
	else
	{
		lineEdit_dz_procname->setText(process_name.c_str());
		lineEdit_dz_procdescr->setText(process_description.c_str());

		comboBox_proc_grp_mode->setCurrentIndex(id_select_process_group.proc_grp_tp);

		checkBox_dz_ic->setEnabled(true);
		checkBox_dz_ic->setChecked(process_ic);
	}

	jvx_cleanup_candidates_dropzone(theDataConnections);

	if (id_select_process_group.proc_grp_tp == JVX_CONNECTION_WIDGET_IS_A_PROCESS)
	{
		listWidget_dz_master->setEnabled(true);

		// Here if the process is not selected or not connected
		cnt = 0;
		std::list<lst_elm_master>::iterator elmM = lst_masters.begin();
		for (; elmM != lst_masters.end(); elmM++)
		{
			IjvxConnectionMasterFactory* theMasFac = NULL;
			IjvxConnectionMaster* theMas = NULL;

			QListWidgetItem* newItem = new QListWidgetItem(listWidget_dz_master);
			newItem->setData(JVX_USER_ROLE_LISTWIDGET_ID_MASTER_ID, QVariant(JVX_SIZE_INT(cnt)));

			theDataConnections->reference_connection_master_factory_uid(elmM->identify.mas_fac_uid, &theMasFac);
			assert(theMasFac);

			theMasFac->reference_connector_master(elmM->identify.mas_id, &theMas);
			jvx_request_interfaceToObject(theMasFac, NULL, &tpId, &strMF);

			assert(theMas);
			theMas->descriptor_master(&mas_descr);

			txt = mas_descr.std_str() + ":" + jvxComponentIdentification_txt(tpId) + ":" + strMF.std_str();
			theMasFac->return_reference_connector_master(theMas);
			theDataConnections->return_reference_connection_master_factory(theMasFac);

			newItem->setText(txt.c_str());
			listWidget_dz_master->addItem(newItem);
			cnt++;
		}
		QFont ft = label_dz_master->font();
		pushButton_add_allmasters->setEnabled(true);
		ft.setItalic(false);
		label_dz_master->setFont(ft);
	}
	else
	{
		listWidget_dz_master->setEnabled(false);
		pushButton_add_allmasters->setEnabled(false);
		QFont ft = label_dz_master->font();
		ft.setItalic(true);
		label_dz_master->setFont(ft);
	}

	inBridgeAlreadyDefined = false;
	outBridgeAlreadyDefined = false;

	cnt = 0;
	std::list<lst_elm_bidges>::iterator elmB = lst_bridges.begin();
	for (; elmB != lst_bridges.end(); elmB++)
	{
		IjvxConnectorFactory* theFac = NULL;
		IjvxOutputConnectorSelect* ocon = NULL;
		IjvxInputConnectorSelect* icon = NULL;
		std::string bn = elmB->bridge_name;
		std::string ext;

		QListWidgetItem* newItem = new QListWidgetItem(listWidget_dz_bridges);
		newItem->setData(JVX_USER_ROLE_LISTWIDGET_ID_BRIDGE_ID, QVariant(JVX_SIZE_INT(cnt)));

		if (JVX_CHECK_SIZE_UNSELECTED(elmB->identify_out.fac_uid))
		{
			ext = "IN";
			inBridgeAlreadyDefined = true;
		}
		if (JVX_CHECK_SIZE_UNSELECTED(elmB->identify_in.fac_uid))
		{
			ext = "OUT";
			outBridgeAlreadyDefined = true;
		}

		if (elmB->boostThread)
		{
			if (ext.empty())
			{
				ext = "B";
			}
			else
			{
				ext += ",B";
			}
		}
		if (elmB->dedicatedThread)
		{
			if (ext.empty())
			{
				ext = "T";
			}
			else
			{
				ext += ",T";
			}
		}

		bn += " (" + ext + ")";
		newItem->setText(bn.c_str());

		if (JVX_CHECK_SIZE_SELECTED(elmB->identify_out.fac_uid))
		{
			theDataConnections->reference_connection_factory_uid(elmB->identify_out.fac_uid, &theFac);
			assert(theFac);
			theFac->reference_output_connector(elmB->identify_out.ocon_id, &ocon);
			assert(ocon);
			jvx_request_interfaceToObject(theFac, NULL, &tpId, &strMF);
			ocon->descriptor_connector(&con_descr);
			txtin = con_descr.std_str() + ":" + jvxComponentIdentification_txt(tpId) + ":" + strMF.std_str();
			inConnectors.push_back(txtin);
			txt = txtin;
			theFac->return_reference_output_connector(ocon);
			theDataConnections->return_reference_connection_factory(theFac);
			theFac = NULL;
		}
		else
		{
			txtin = "|";
		}

		if (JVX_CHECK_SIZE_SELECTED(elmB->identify_in.fac_uid))
		{
			theDataConnections->reference_connection_factory_uid(elmB->identify_in.fac_uid, &theFac);
			assert(theFac);
			theFac->reference_input_connector(elmB->identify_in.icon_id, &icon);
			assert(icon);
			jvx_request_interfaceToObject(theFac, NULL, &tpId, &strMF);
			icon->descriptor_connector(&con_descr);
			txtout = con_descr.std_str() + ":" + jvxComponentIdentification_txt(tpId) + ":" + strMF.std_str();
			outConnectors.push_back(txtout);
			txt = txtin + "-->" + txtout;
			theFac->return_reference_input_connector(icon);
			theDataConnections->return_reference_connection_factory(theFac);
			theFac = NULL;
		}
		else
		{
			txt = txtin + "-->|";
		}

		newItem->setToolTip(txt.c_str());

		listWidget_dz_bridges->addItem(newItem);
		cnt++;
	}

	cnt = 0;
	std::list<lst_elm_icons>::iterator elmIC = lst_inputs.begin();
	for (; elmIC != lst_inputs.end(); elmIC++)
	{
		IjvxConnectorFactory* theFac = NULL;
		IjvxInputConnectorSelect* icon = NULL;

		QListWidgetItem* newItem = new QListWidgetItem(listWidget_dz_icon);
		newItem->setData(JVX_USER_ROLE_LISTWIDGET_ID_ICON_ID, QVariant(JVX_SIZE_INT(cnt)));

		theDataConnections->reference_connection_factory_uid(elmIC->identify.fac_uid, &theFac);
		assert(theFac);
		theFac->reference_input_connector(elmIC->identify.icon_id, &icon);
		assert(icon);
		jvx_request_interfaceToObject(theFac, NULL, &tpId, &strMF);
		icon->descriptor_connector(&con_descr);
		txtout = con_descr.std_str() + ":" + jvxComponentIdentification_txt(tpId) + ":" + strMF.std_str();
		theFac->return_reference_input_connector(icon);
		theDataConnections->return_reference_connection_factory(theFac);
		theFac = NULL;
		newItem->setText(txtout.c_str());
		listWidget_dz_icon->addItem(newItem);
		cnt++;
	}

	cnt = 0;
	std::list<lst_elm_ocons>::iterator elmOC = lst_outputs.begin();
	for (; elmOC != lst_outputs.end(); elmOC++)
	{
		IjvxConnectorFactory* theFac = NULL;
		IjvxOutputConnectorSelect* ocon = NULL;

		QListWidgetItem* newItem = new QListWidgetItem(listWidget_dz_outcon);
		newItem->setData(JVX_USER_ROLE_LISTWIDGET_ID_OCON_ID, QVariant(JVX_SIZE_INT(cnt)));

		theDataConnections->reference_connection_factory_uid(elmOC->identify.fac_uid, &theFac);
		assert(theFac);
		theFac->reference_output_connector(elmOC->identify.ocon_id, &ocon);
		assert(ocon);
		jvx_request_interfaceToObject(theFac, NULL, &tpId, &strMF);
		ocon->descriptor_connector(&con_descr);
		txtout = con_descr.std_str() + ":" + jvxComponentIdentification_txt(tpId) + ":" + strMF.std_str();
		theFac->return_reference_output_connector(ocon);
		theDataConnections->return_reference_connection_factory(theFac);
		theFac = NULL;
		newItem->setText(txtout.c_str());
		listWidget_dz_outcon->addItem(newItem);
		cnt++;
	}
	/*
	std::list<std::string>::iterator elmI = inConnectors.begin();
	for (; elmI != inConnectors.end(); elmI++)
	{
	listWidget_dz_icon->addItem(elmI->c_str());
	}
	std::list<std::string>::iterator elmO = outConnectors.begin();
	for (; elmO != outConnectors.end(); elmO++)
	{
	listWidget_dz_outcon->addItem(elmO->c_str());
	}
	*/
}

void
jvx_connections_widget::ui_highlight_dropzone(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select)
{
	jvxSize numb = 0;
	jvxSize i;
	IjvxDataConnectionCommon* comm_proc = NULL;
	if (JVX_CHECK_SIZE_SELECTED(id_select_bridge))
	{
		if (theProcess_select)
		{
			comm_proc = theProcess_select;
		}
		if (theGroup_select)
		{
			comm_proc = theGroup_select;
		}

		if (comm_proc)
		{
			comm_proc->number_bridges(&numb);
			if (id_select_bridge < numb)
			{
				IjvxConnectorBridge* theB = NULL;
				IjvxOutputConnector* ocon = NULL;
				IjvxInputConnector* icon = NULL;
				jvxBool ded = false;
				jvxBool boost = false;
				jvxBool interceptor = false;

				comm_proc->reference_bridge(id_select_bridge, &theB);
				if (theB)
				{
					jvxBool foundone = false;
					std::string txt;
					theB->reference_connect_from(&ocon);
					txt = connectorToExpression(ocon);
					for (i = 0; i < listWidget_dz_outcon->count(); i++)
					{
						QListWidgetItem* it = listWidget_dz_outcon->item((int)i);
						QFont ft = it->font();
						std::string str = it->text().toLatin1().data();
						if (txt == str)
						{
							foundone = true;
							ft.setBold(true);
						}
						else
						{
							ft.setBold(false);
						}
						it->setFont(ft);
					}
					if (!foundone)
					{
						checkBox_bridge_entry->setChecked(true);
					}
					else
					{
						checkBox_bridge_entry->setChecked(false);
					}

					foundone = false;
					theB->reference_connect_to(&icon);
					txt = connectorToExpression(icon);
					for (i = 0; i < listWidget_dz_icon->count(); i++)
					{
						QListWidgetItem* it = listWidget_dz_icon->item((int)i);
						QFont ft = it->font();
						std::string str = it->text().toLatin1().data();
						if (txt == str)
						{
							foundone = true;
							ft.setBold(true);
						}
						else
						{
							ft.setBold(false);
						}
						it->setFont(ft);
					}
					if (!foundone)
					{
						checkBox_bridge_exit->setChecked(true);
					}
					else
					{
						checkBox_bridge_exit->setChecked(false);
					}
					theB->properties_bridge(&ded, &boost, &interceptor);
					checkBox_bridge_thread->setChecked(ded);
					checkBox_bridge_boost->setChecked(boost);

					theB->return_reference_connect_from(ocon);
					theB->return_reference_connect_to(icon);
				}
				comm_proc->return_reference_bridge(theB);
			}
		}
	}
}

void
jvx_connections_widget::ui_refine_dropzone(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select)
{
	jvxSize cnt;

	cnt = 0;
	if (
		(theProcess_select == NULL) &&
		(theGroup_select == NULL))
	{
		// This funcion with control only if no process/group has been selected
		std::list<lst_elm_master>::iterator elmma = lst_masters.begin();
		for (; elmma != lst_masters.end(); elmma++)
		{
			QListWidgetItem* it = listWidget_dz_master->item((int)cnt);
			if (it)
			{
				// it might be zero if we are in group edit mode
				QFont ft = it->font();
				if (cnt == id_select_master)
				{
					ft.setBold(true);
				}
				else
				{
					ft.setBold(false);
				}
				it->setFont(ft);
			}
			cnt++;
		}

		// Switch bridge id back to in-range
		if (JVX_CHECK_SIZE_SELECTED(id_select_bridge))
		{
			int idx = JVX_MIN((int)lst_bridges.size() - 1, (int)id_select_bridge);
			id_select_bridge = (jvxSize)idx;
		}

		// Show bridge
		if (JVX_CHECK_SIZE_SELECTED(id_select_bridge))
		{
			std::list<lst_elm_bidges>::iterator elmB = lst_bridges.begin();
			std::advance(elmB, id_select_bridge);
			if (elmB != lst_bridges.end())
			{
				cnt = 0;
				std::list<lst_elm_icons>::iterator elmic = lst_inputs.begin();
				for (; elmic != lst_inputs.end(); elmic++)
				{
					assert(cnt < listWidget_dz_icon->count());
					QListWidgetItem* it = listWidget_dz_icon->item((int)cnt);

					QFont ft = it->font();
					if (
						(elmic->identify.fac_uid == elmB->identify_in.fac_uid) &&
						(elmic->identify.icon_id == elmB->identify_in.icon_id))
					{
						ft.setBold(true);
					}
					else
					{
						ft.setBold(false);
					}
					it->setFont(ft);
					cnt++;
				}

				cnt = 0;
				std::list<lst_elm_ocons>::iterator elmoc = lst_outputs.begin();
				for (; elmoc != lst_outputs.end(); elmoc++)
				{
					assert(cnt < listWidget_dz_outcon->count());
					QListWidgetItem* it = listWidget_dz_outcon->item((int)cnt);

					QFont ft = it->font();
					if (
						(elmoc->identify.fac_uid == elmB->identify_out.fac_uid) &&
						(elmoc->identify.ocon_id == elmB->identify_out.ocon_id))
					{
						ft.setBold(true);
					}
					else
					{
						ft.setBold(false);
					}
					it->setFont(ft);
					cnt++;
				}
			}
		}

		cnt = 0;
		std::list<lst_elm_ocons>::iterator elmoc = lst_outputs.begin();
		for (; elmoc != lst_outputs.end(); elmoc++)
		{
			QListWidgetItem* it = listWidget_dz_outcon->item((int)cnt);
			if (id_select_ocon == cnt)
			{
				it->setBackground(QBrush(Qt::gray));
			}
			else
			{
				it->setBackground(QBrush(Qt::white));
			}
			cnt++;

		}
		cnt = 0;
		std::list<lst_elm_icons>::iterator elmic = lst_inputs.begin();
		for (; elmic != lst_inputs.end(); elmic++)
		{
			QListWidgetItem* it = listWidget_dz_icon->item((int)cnt);
			if (id_select_icon == cnt)
			{
				it->setBackground(QBrush(Qt::gray));
			}
			else
			{
				it->setBackground(QBrush(Qt::white));
			}
			cnt++;
		}

		if (id_select_process_group.proc_grp_tp == JVX_CONNECTION_WIDGET_IS_A_PROCESS)
		{
			checkBox_bridge_thread->setEnabled(false);
			checkBox_bridge_thread->setChecked(false);
			checkBox_bridge_boost->setEnabled(false);
			checkBox_bridge_boost->setChecked(false);
			checkBox_bridge_entry->setEnabled(false);
			checkBox_bridge_entry->setChecked(false);
			checkBox_bridge_exit->setEnabled(false);
			checkBox_bridge_exit->setChecked(false);
			listWidget_dz_icon->setEnabled(true);
			listWidget_dz_outcon->setEnabled(true);
		}
		else
		{
			checkBox_bridge_thread->setEnabled(true);
			checkBox_bridge_thread->setChecked(bridge_ded_thread);
			checkBox_bridge_boost->setEnabled(true);
			checkBox_bridge_boost->setChecked(bridge_boost_thread);
			if (inBridgeAlreadyDefined)
			{
				checkBox_bridge_entry->setEnabled(false);
				bridge_entry_chain = false;
			}
			else
			{
				checkBox_bridge_entry->setEnabled(true);
			}
			checkBox_bridge_entry->setChecked(bridge_entry_chain);
			if (outBridgeAlreadyDefined)
			{
				checkBox_bridge_exit->setEnabled(false);
				bridge_exit_chain = false;
			}
			else
			{
				checkBox_bridge_exit->setEnabled(true);
			}
			checkBox_bridge_exit->setChecked(bridge_exit_chain);
			listWidget_dz_icon->setEnabled(true);
			listWidget_dz_outcon->setEnabled(true);
			if (bridge_entry_chain)
			{
				listWidget_dz_outcon->setEnabled(false);
			}
			if (bridge_exit_chain)
			{
				listWidget_dz_icon->setEnabled(false);
			}
		}

		comboBox_proc_grp_mode->setCurrentIndex(id_select_process_group.proc_grp_tp);
	}
}

void
jvx_connections_widget::ui_update_work_buttons(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select)
{
	jvxState stat = JVX_STATE_NONE;
	pushButton_addconnector->setEnabled(true);
	pushButton_setmaster->setEnabled(true);
	listWidget_dz_bridges->setEnabled(true);
	listWidget_dz_icon->setEnabled(true);
	listWidget_dz_outcon->setEnabled(true);
	listWidget_dz_master->setEnabled(true);
	allow_select_connector = true;
	bridge_show_only_mode = false;
	checkBox_bridge_boost->setEnabled(true);
	checkBox_bridge_thread->setEnabled(true);
	checkBox_bridge_entry->setEnabled(true);
	checkBox_bridge_exit->setEnabled(true);

	if (theProcess_select)
	{
		theProcess_select->status(&stat);
		theProcess_select->status_chain( NULL JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		if (stat >= JVX_STATE_ACTIVE)
		{
			pushButton_addconnector->setEnabled(false);
			pushButton_setmaster->setEnabled(false);
			bridge_show_only_mode = true;
			// listWidget_dz_bridges->setEnabled(false); // allow scrolling
			//listWidget_dz_icon->setEnabled(false);
			//listWidget_dz_outcon->setEnabled(false);
			allow_select_connector = false;
			listWidget_dz_master->setEnabled(false);
			checkBox_bridge_boost->setEnabled(false);
			checkBox_bridge_thread->setEnabled(false);
			checkBox_bridge_entry->setEnabled(false);
			checkBox_bridge_exit->setEnabled(false);
		}
	}
	if (theGroup_select)
	{
		theGroup_select->status(&stat);
		if (stat >= JVX_STATE_ACTIVE)
		{
			pushButton_addconnector->setEnabled(false);
			pushButton_setmaster->setEnabled(false);
			bridge_show_only_mode = true;
			//listWidget_dz_bridges->setEnabled(false);// allow scrolling
			//listWidget_dz_icon->setEnabled(false);
			//listWidget_dz_outcon->setEnabled(false);
			allow_select_connector = false;
			listWidget_dz_master->setEnabled(false);
			checkBox_bridge_boost->setEnabled(false);
			checkBox_bridge_thread->setEnabled(false);
			checkBox_bridge_entry->setEnabled(false);
			checkBox_bridge_exit->setEnabled(false);
		}
	}

	if (id_select_process_group.proc_grp_tp == JVX_CONNECTION_WIDGET_IS_A_PROCESS)
	{
		if (JVX_CHECK_SIZE_SELECTED(id_selected_master.uid_select_masfac))
		{
			if (JVX_CHECK_SIZE_SELECTED(id_selected_master.id_select_mas))
			{
				IjvxConnectionMasterFactory* theMasFac = NULL;
				IjvxConnectionMaster* theMas = NULL;
				theDataConnections->reference_connection_master_factory_uid(id_selected_master.uid_select_masfac, &theMasFac);
				if (theMasFac)
				{
					theMasFac->reference_connector_master(id_selected_master.id_select_mas, &theMas);
					if (theMas)
					{
						jvxBool isAvail = false;

						theMas->available_master(&isAvail);
						if (isAvail)
						{
							jvxBool alreadyThere = false;
							std::list<lst_elm_master>::iterator elm = lst_masters.begin();
							for (; elm != lst_masters.end(); elm++)
							{
								if (
									(elm->identify.mas_fac_uid == id_selected_master.uid_select_masfac) &&
									(elm->identify.mas_id == id_selected_master.id_select_mas))
								{
									alreadyThere = true;
									break;
								}
							}
							if (alreadyThere)
							{
								pushButton_setmaster->setEnabled(false);
							}
						}
						else
						{
							pushButton_setmaster->setEnabled(false);
						}
						theMasFac->return_reference_connector_master(theMas);
					}
					else
					{
						pushButton_setmaster->setEnabled(false);
					}
					theDataConnections->return_reference_connection_master_factory(theMasFac);
				}
				else
				{
					pushButton_setmaster->setEnabled(false);
				}
			}
			else
			{
				pushButton_setmaster->setEnabled(false);
			}
		}
		else
		{
			pushButton_setmaster->setEnabled(false);
		}
	}
	else
	{
		listWidget_dz_master->setEnabled(false);
		pushButton_setmaster->setEnabled(false);
	}

	if (JVX_CHECK_SIZE_SELECTED(id_selected_connector.uid_select_confac))
	{
		if (JVX_CHECK_SIZE_SELECTED(id_selected_connector.id_select_icon))
		{
			IjvxConnectorFactory* theConFac = NULL;
			IjvxInputConnectorSelect* theIcons = NULL;
			theDataConnections->reference_connection_factory_uid(id_selected_connector.uid_select_confac, &theConFac);
			if (theConFac)
			{
				theConFac->reference_input_connector(id_selected_connector.id_select_icon, &theIcons);
				if (theIcons)
				{
					jvxBool isAvail = false;
					IjvxInputConnector* iconc = theIcons->reference_icon();
					if (iconc && (iconc->available_to_connect_icon() == JVX_NO_ERROR))
					{

					}
					else
					{
						pushButton_addconnector->setEnabled(false);
					}
					theConFac->return_reference_input_connector(theIcons);
				}
				else
				{
					pushButton_addconnector->setEnabled(false);
				}
				theDataConnections->return_reference_connection_factory(theConFac);
			}
			else
			{
				pushButton_addconnector->setEnabled(false);
			}
		}
		else if (JVX_CHECK_SIZE_SELECTED(id_selected_connector.id_select_ocon))
		{
			IjvxConnectorFactory* theConFac = NULL;
			IjvxOutputConnectorSelect* theOcons = NULL;
			theDataConnections->reference_connection_factory_uid(id_selected_connector.uid_select_confac, &theConFac);
			if (theConFac)
			{
				theConFac->reference_output_connector(id_selected_connector.id_select_ocon, &theOcons);
				if (theOcons)
				{
					jvxBool isAvail = false;
					IjvxOutputConnector* oconc = theOcons->reference_ocon();
					if (oconc && (oconc->available_to_connect_ocon() == JVX_NO_ERROR))
					{

					}
					else
					{
						pushButton_addconnector->setEnabled(false);
					}
					theConFac->return_reference_output_connector(theOcons);
				}
				else
				{
					pushButton_addconnector->setEnabled(false);
				}
				theDataConnections->return_reference_connection_factory(theConFac);
			}
			else
			{
				pushButton_addconnector->setEnabled(false);
			}
		}
		else
		{
			pushButton_addconnector->setEnabled(false);
		}
	}
	else
	{
		pushButton_addconnector->setEnabled(false);
	}
}

void
jvx_connections_widget::update_window_core(jvxBitField oper, const char* propLst )
{
	jvxErrorType resL = JVX_NO_ERROR;
	std::string tooltip = "--";

	IjvxDataConnectionProcess* theProcess_select = NULL;
	IjvxDataConnectionGroup* theGroup_select = NULL;
	jvxSize select_id_proc_grp = 0;

	// ===================================================================
	// ===================================================================
	ui_get_reference_process(&theProcess_select, &theGroup_select, &select_id_proc_grp);
	
	// ===================================================================
	// Rebuild process list
	// ===================================================================
	if (jvx_bitTest(oper, REDRAW_PROCESS_LIST_SHIFT))
	{
		ui_redraw_process_list();
	}

	// ===================================================================
	// ===================================================================

	if (jvx_bitTest(oper, REDRAW_PROCESS_LIST_ID_SHIFT))
	{
		ui_refine_process_list(select_id_proc_grp);

	}

	if(jvx_bitTest(oper, REDRAW_PROCESS_MASTER_FAC_SHIFT))
	{
		ui_redraw_factories(theProcess_select, theGroup_select);
	}

	if (jvx_bitTest(oper, REDRAW_PROCESS_DROPZONE_FULL_SHIFT))
	{		
		ui_redraw_dropzone(theProcess_select, theGroup_select);
	}

	
	if (jvx_bitTest(oper, REDRAW_PROCESS_DROPZONE_REFINE_SHIFT))
	{
		ui_refine_dropzone(theProcess_select, theGroup_select);
	}

	if (jvx_bitTest(oper, REDRAW_PROCESS_DROPZONE_HIGHLIGHT_SHIFT))
	{
		ui_highlight_dropzone(theProcess_select, theGroup_select);
	}

	ui_update_work_buttons(theProcess_select, theGroup_select);

	// ====================================================================
	if (jvx_bitTest(oper, REDRAW_PROCESS_REDRAW_CONNECTION_RULES_SHIFT))
	{
		ui_redraw_connection_rules();
	}

	if (jvx_bitTest(oper, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT))
	{
		ui_show_connection_rule();
	}

	if (theProcess_select)
	{
		show_status_test_chain(id_select_process_group.id_proc_grp);		
	}

	if (theProcess_select)
	{
		theDataConnections->return_reference_connection_process(theProcess_select);
	}
	theProcess_select = NULL;

	if (theGroup_select)
	{
		theDataConnections->return_reference_connection_group(theGroup_select);
	}
	theGroup_select = NULL;
}

void
jvx_connections_widget::ui_redraw_connection_rules()
{
	jvxSize num = 0;
	jvxSize i;
	IjvxDataConnectionRule* myRule = NULL;
	jvxApiString str;
	jvxBool isDefault = false;
	theDataConnections->number_connection_rules(&num);
	comboBox_rulename->clear();
	comboBox_rulename->addItem("None");
	for (i = 0; i < num; i++)
	{
		theDataConnections->reference_connection_rule(i, &myRule);
		myRule->description_rule(&str, &isDefault);
		if (isDefault)
		{
			comboBox_rulename->addItem((str.std_str() + "<D>").c_str());
		}
		else
		{
			comboBox_rulename->addItem(str.c_str());
		}
		theDataConnections->return_reference_connection_rule(myRule);
		myRule = NULL;
	}
	if (id_select_rule.id_rule >= num)
	{
		id_select_rule.id_rule = JVX_SIZE_UNSELECTED;
	}
	if (JVX_CHECK_SIZE_SELECTED(id_select_rule.id_rule))
	{
		comboBox_rulename->setCurrentIndex((int)id_select_rule.id_rule + 1);
	}
	else
	{
		comboBox_rulename->setCurrentIndex(0);
	}
}

void
jvx_connections_widget::ui_show_connection_rule()
{
	jvxSize j;
	jvxSize numB = 0;
	jvxBool isDefault = false;
	if (JVX_CHECK_SIZE_SELECTED(id_select_rule.id_rule))
	{
		jvxSize num = 0;
		IjvxDataConnectionRule* myRule = NULL;
		jvxApiString str1, str2;
		jvxComponentIdentification cpTp;
		jvxErrorType res = JVX_NO_ERROR;

		comboBox_ruleMasterTp->setEnabled(false);
		lineEdit_ruleMasterNameWC->setEnabled(false);
		lineEdit_ruleMaster->setEnabled(false);
		lineEdit_mastersid->setEnabled(false);
		lineEdit_masterssid->setEnabled(false);
		lineEdit_ruleName->setEnabled(false);
		lineEdit_fromBridgeWC->setEnabled(false);
		lineEdit_fromBridgeWCC->setEnabled(false);
		comboBox_fromBridge->setEnabled(false);
		lineEdit_fromBridgesid->setEnabled(false);
		lineEdit_fromBridgessid->setEnabled(false);
		lineEdit_toBridgeWC->setEnabled(false);
		lineEdit_toBridgeWCC->setEnabled(false);
		comboBox_toBridge->setEnabled(false);
		lineEdit_toBridgesid->setEnabled(false);
		lineEdit_toBridgessid->setEnabled(false);
		lineEdit_ruleBridgename->setEnabled(false);
		pushButton_remRule->setEnabled(true);
		pushButton_addRule->setEnabled(false);
		pushButton_remRuleBridge->setEnabled(false);
		pushButton_addRuleBridge->setEnabled(false);

		theDataConnections->number_connection_rules(&num);
		if (id_select_rule.id_rule < num)
		{
			res = theDataConnections->reference_connection_rule(id_select_rule.id_rule, &myRule);
			assert(res == JVX_NO_ERROR);

			myRule->get_master(&cpTp, &str1, &str2);

			comboBox_ruleMasterTp->setCurrentIndex((int)cpTp.tp);
			lineEdit_ruleMasterNameWC->setText(str1.c_str());
			lineEdit_ruleMaster->setText(str2.c_str());

			lineEdit_mastersid->setText(jvx_size2String(cpTp.slotid, 2).c_str());
			lineEdit_masterssid->setText(jvx_size2String(cpTp.slotsubid, 2).c_str());

			str1.clear();
			myRule->description_rule(&str1, &isDefault);
			if (isDefault)
			{
				lineEdit_ruleName->setText((str1.std_str() + "<D>").c_str());
			}
			else
			{
				lineEdit_ruleName->setText(str1.c_str());
			}
			myRule->number_bridges(&numB);
			if (id_select_rule_bridgename.id_bridgename > numB)
			{
				id_select_rule_bridgename.id_bridgename = 0;
			}

			comboBox_ruleBridges->clear();
			for (j = 0; j < numB; j++)
			{
				str1.clear();
				myRule->get_bridge(j, &str1, NULL, NULL, NULL, NULL, NULL, NULL);
				comboBox_ruleBridges->addItem(str1.c_str());
				if (j == id_select_rule_bridgename.id_bridgename)
				{
					lineEdit_ruleBridgename->setText(str1.c_str());

					str1.clear();
					str2.clear();

					myRule->get_bridge(j, NULL, &cpTp, &str1, &str2, NULL, NULL, NULL);

					lineEdit_fromBridgeWC->setText(str1.c_str());
					lineEdit_fromBridgeWCC->setText(str2.c_str());
					comboBox_fromBridge->setCurrentIndex(cpTp.tp);
					lineEdit_fromBridgesid->setText(jvx_size2String(cpTp.slotid, 2).c_str());
					lineEdit_fromBridgessid->setText(jvx_size2String(cpTp.slotsubid, 2).c_str());

					// =======================================
					str1.clear();
					str2.clear();

					myRule->get_bridge(j, NULL,NULL, NULL, NULL,  &cpTp, &str1, &str2);

					lineEdit_toBridgeWC->setText(str1.c_str());
					lineEdit_toBridgeWCC->setText(str2.c_str());
					comboBox_toBridge->setCurrentIndex(cpTp.tp);
					lineEdit_toBridgesid->setText(jvx_size2String(cpTp.slotid, 2).c_str());
					lineEdit_toBridgessid->setText(jvx_size2String(cpTp.slotsubid, 2).c_str());

				}
			}

			if (numB > 0)
			{
				comboBox_ruleBridges->setCurrentIndex((int)id_select_rule_bridgename.id_bridgename);
			}
			res = theDataConnections->return_reference_connection_rule(myRule);
			assert(res == JVX_NO_ERROR);
			myRule = NULL;
		}
	}
	else
	{
		comboBox_ruleMasterTp->setEnabled(true);
		lineEdit_ruleMasterNameWC->setEnabled(true);
		lineEdit_ruleMaster->setEnabled(true);
		lineEdit_mastersid->setEnabled(true);
		lineEdit_masterssid->setEnabled(true);
		lineEdit_ruleName->setEnabled(true);
		lineEdit_fromBridgeWC->setEnabled(true);
		lineEdit_fromBridgeWCC->setEnabled(true);
		comboBox_fromBridge->setEnabled(true);
		lineEdit_fromBridgesid->setEnabled(true);
		lineEdit_fromBridgessid->setEnabled(true);
		lineEdit_toBridgeWC->setEnabled(true);
		lineEdit_toBridgeWCC->setEnabled(true);
		comboBox_toBridge->setEnabled(true);
		lineEdit_toBridgesid->setEnabled(true);
		lineEdit_toBridgessid->setEnabled(true);
		lineEdit_ruleBridgename->setEnabled(true);
		pushButton_remRule->setEnabled(true);
		pushButton_addRule->setEnabled(true);
		pushButton_remRuleBridge->setEnabled(true);
		pushButton_addRuleBridge->setEnabled(true);

		comboBox_ruleBridges->clear();

		lineEdit_ruleMasterNameWC->setText("");
		lineEdit_ruleMaster->setText("");
		lineEdit_mastersid->setText("");
		lineEdit_masterssid->setText("");
		lineEdit_ruleName->setText("");
		lineEdit_fromBridgeWC->setText("");
		lineEdit_fromBridgeWCC->setText("");
		comboBox_fromBridge->setCurrentIndex(0);
		lineEdit_fromBridgesid->setText("");
		lineEdit_fromBridgessid->setText("");
		lineEdit_toBridgeWC->setText("");
		lineEdit_toBridgeWCC->setText("");
		comboBox_toBridge->setCurrentIndex(0);
		lineEdit_toBridgesid->setText("");
		lineEdit_toBridgessid->setText("");
		lineEdit_ruleBridgename->setText("");

		comboBox_ruleMasterTp->setCurrentIndex((int)theConnectionRuleDropzone.cpMaster.tp);
		lineEdit_ruleMasterNameWC->setText(theConnectionRuleDropzone.masterFactoryWildcard.c_str());
		lineEdit_ruleMaster->setText(theConnectionRuleDropzone.masterWildcard.c_str());

		lineEdit_mastersid->setText(jvx_size2String(theConnectionRuleDropzone.cpMaster.slotid, 2).c_str());
		lineEdit_masterssid->setText(jvx_size2String(theConnectionRuleDropzone.cpMaster.slotsubid, 2).c_str());

		lineEdit_ruleName->setText(theConnectionRuleDropzone.rule_name.c_str());

		numB = theConnectionRuleDropzone.bridges.size();
		if (id_select_rule_bridgename.id_bridgename > numB)
		{
			id_select_rule_bridgename.id_bridgename = 0;
		}

		comboBox_ruleBridges->clear();
		std::list<jvxConnectionRuleBridge>::iterator  elm = theConnectionRuleDropzone.bridges.begin();
		j = 0;
		for (; elm != theConnectionRuleDropzone.bridges.end(); elm++)
		{
			comboBox_ruleBridges->addItem(elm->bridge_name.c_str());
		}

		if (numB > 0)
		{
			comboBox_ruleBridges->setCurrentIndex((int)id_select_rule_bridgename.id_bridgename);
		}

		lineEdit_ruleBridgename->setText(edit_bridge_name.c_str());

		// =======================================

		lineEdit_fromBridgeWCC->setText(edit_from_wc.c_str());
		lineEdit_fromBridgeWC->setText(edit_from_fac_wc.c_str());
		comboBox_fromBridge->setCurrentIndex(edit_cp_from.tp);
		lineEdit_fromBridgesid->setText(jvx_size2String(edit_cp_from.slotid, 2).c_str());
		lineEdit_fromBridgessid->setText(jvx_size2String(edit_cp_from.slotsubid, 2).c_str());

		// =======================================

		lineEdit_toBridgeWCC->setText(edit_to_wc.c_str());
		lineEdit_toBridgeWC->setText(edit_to_fac_wc.c_str());
		comboBox_toBridge->setCurrentIndex(edit_cp_to.tp);
		lineEdit_toBridgesid->setText(jvx_size2String(edit_cp_to.slotid, 2).c_str());
		lineEdit_toBridgessid->setText(jvx_size2String(edit_cp_to.slotsubid, 2).c_str());
	}
}

std::string 
jvx_connections_widget::connectorToExpression(IjvxCommonConnector* con)
{
	IjvxConnectorFactory* my_parent = NULL;
	jvxComponentIdentification tpId;
	jvxApiString str;
	jvxApiString descr;
	std::string txt = "failed";
	con->parent_factory(&my_parent);
	jvx_request_interfaceToObject(my_parent, NULL, &tpId, &str);
	con->descriptor_connector(&descr);
	txt = descr.std_str() + ":" + jvxComponentIdentification_txt(tpId) + ":" + str.std_str();
	return txt;
}
