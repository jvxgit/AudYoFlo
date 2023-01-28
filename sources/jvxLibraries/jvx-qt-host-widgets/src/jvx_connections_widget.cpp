#include <QtWidgets/QMessageBox>

#include "jvx_connections_widget.h"
#include "CjvxJson.h"
#include "jvx_connect_results.h"
#include "jvx_connections_widget_const.h"

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtConnectionWidget, jvxQtConnectionWidget, jvx_connections_widget)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtConnectionWidget, jvxQtConnectionWidget)

jvx_connections_widget::jvx_connections_widget(QWidget* parent) : QWidget(parent)
{
	theHostRef = NULL;
	theBwdRef = NULL;
	theDataConnections = NULL;

	process_name = "My new connection";
	process_description = "newConn";

	id_select_process_group.id_proc_grp = JVX_SIZE_UNSELECTED;

	id_selected_master.uid_select_masfac = JVX_SIZE_UNSELECTED;
	id_selected_master.id_select_mas = JVX_SIZE_UNSELECTED;

	id_selected_connector.uid_select_confac = JVX_SIZE_UNSELECTED;
	id_selected_connector.id_select_icon = JVX_SIZE_UNSELECTED;
	id_selected_connector.id_select_ocon = JVX_SIZE_UNSELECTED;

	id_select_rule.id_rule = JVX_SIZE_UNSELECTED;
	id_select_rule_bridgename.id_bridgename  = JVX_SIZE_UNSELECTED;

	id_select_process_group.proc_grp_tp = JVX_CONNECTION_WIDGET_IS_A_PROCESS;

	// proc_grp_tp = JVX_CONNECTION_WIDGET_IS_A_PROCESS;
	bridge_ded_thread = false;
	bridge_boost_thread = false;;
	bridge_entry_chain = false;
	bridge_exit_chain = false;

	process_ic = false;
	process_ess = true;

	rule_ic = false;
	rule_ess = true;

	bridge_show_only_mode = false;

	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS_INIT(latest_status_process);
	config.min_width = 800;
	config.min_height = 600;
}

jvx_connections_widget::~jvx_connections_widget()
{
	this->latest_status_process->clear();
}

QSize
jvx_connections_widget::sizeHint() const
{
	return QSize((int)config.min_width, (int)config.min_height);
}

QSize
jvx_connections_widget::minimumSizeHint() const
{
	return QSize((int)config.min_width, (int)config.min_height);
}

void
jvx_connections_widget::init(IjvxHost* theHost, jvxCBitField mode, jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd)
{
	jvxSize i;
	theHostRef = theHost;
	theBwdRef = bwd;
	theDataConnections = NULL;

	this->setupUi(this);

	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		comboBox_ruleMasterTp->addItem(jvxComponentType_txt(i));
		comboBox_fromBridge->addItem(jvxComponentType_txt(i));
		comboBox_toBridge->addItem(jvxComponentType_txt(i));
	}
	comboBox_fromBridge->addItem("--");
	comboBox_toBridge->addItem("--");
	this->setWindowTitle("Data Connections");
}

void 
jvx_connections_widget::local_close()
{
	parentWidget()->close();
}

void
jvx_connections_widget::getMyQWidget(QWidget** retWidget, jvxSize id)
{
	switch (id)
	{
	case 0:
		*retWidget = static_cast<QWidget*>(this);
		break;
	default:
		*retWidget = NULL;
	}
}

void
jvx_connections_widget::terminate()
{
	theHostRef = NULL;
	theBwdRef = NULL;
	theDataConnections = NULL;
}

void
jvx_connections_widget::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theHostRef->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, (jvxHandle**)&theDataConnections);
	if (res != JVX_NO_ERROR)
	{
		QMessageBox msgBox(QMessageBox::Critical, "Fatal Error", "No access to JVX data connections interface!");
		msgBox.exec();
		exit(-1);
	}
}

void
jvx_connections_widget::deactivate()
{
	if (theDataConnections)
	{
		// theDataConnections->remove_all_connection(); <- this is already done in app host code
		theHostRef->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, (jvxHandle*)theDataConnections);
		theDataConnections = NULL;
	}
}

void
jvx_connections_widget::processing_started()
{
	show_status_test_chain(id_select_process_group.id_proc_grp);
}

void
jvx_connections_widget::processing_stopped()
{
	show_status_test_chain(id_select_process_group.id_proc_grp);
}

void
jvx_connections_widget::update_window(jvxCBitField prio)
{
	jvxBitField btf;
	jvx_cleanup_candidates_dropzone(theDataConnections);
	jvx_bitFull(btf);
	update_window_core(btf);

	// Show the current connection status if requested from external
	// show_status_test_chain(id_select_process_group.id_proc_grp);
}

void
jvx_connections_widget::update_window_periodic()
{
}

void
jvx_connections_widget::process_item_clicked(QTreeWidgetItem* it, int idx)
{
	jvxSize numP = 0;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxSize eId = JVX_SIZE_UNSELECTED;
	IjvxDataConnectionProcess* theProcess = NULL;
	jvxState stat = JVX_STATE_NONE;
	QVariant varuId = it->data(0, JVX_USER_ROLE_TREEWIDGET_PROCESS_UID);
	QVariant vareId = it->data(0, JVX_USER_ROLE_TREEWIDGET_WIDGET_ID);
	QVariant varpId = it->data(0, JVX_USER_ROLE_TREEWIDGET_CONN_TYPE);
	jvxBitField btf;
	jvxApiString nm;
	jvxBool testOk = true;

	//jvxApiString tstResult;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	if (varuId.isValid() && vareId.isValid() && varpId.isValid())
	{

		uId = varuId.toInt();
		eId = vareId.toInt();
		id_select_process_group.proc_grp_tp = (jvxConnectionWidgetProcessGroupType)varpId.toInt();
		id_select_process_group.id_proc_grp = eId;
		jvx_bitZSet(btf, REDRAW_PROCESS_LIST_ID_SHIFT);
		jvx_bitSet(btf, REDRAW_PROCESS_MASTER_FAC_SHIFT);
		jvx_bitSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
		jvx_bitSet(btf, REDRAW_PROCESS_DROPZONE_REFINE_SHIFT);
		id_select_bridge = JVX_SIZE_UNSELECTED;
		listWidget_connections->setEnabled(true);

		this->update_window_core(btf);
	}
	else
	{
		listWidget_connections->setEnabled(false);
	}

}

void
jvx_connections_widget::show_status_test_chain(jvxSize idSelect)
{
	IjvxDataConnectionProcess* theProcess = NULL;
	jvxBool testOk = false;
	jvxState stat = JVX_STATE_NONE;
	std::string txtA;
	jvxApiString nmChain;
	QColor col(Qt::red);
	jvxErrorType res = JVX_NO_ERROR;
	QListWidgetItem* item = NULL;
	if (JVX_CHECK_SIZE_SELECTED(idSelect))
	{
		theDataConnections->reference_connection_process(idSelect, &theProcess);
		if (theProcess)
		{

			theProcess->descriptor_connection(&nmChain);
			txtA = "Most recent test of chain <" + nmChain.std_str();
			txtA += ">: ";

			latest_status_process->clear();
			theProcess->status(&stat);
			theProcess->status_chain( &testOk JVX_CONNECTION_FEEDBACK_REF_CALL_A(latest_status_process));
			if (stat == JVX_STATE_ACTIVE)
			{
				if (testOk == false)
				{
					txtA += "Status -> Error.";
					col = Qt::red;
				}
				else
				{
					txtA += "Status -> Ok.";
					col = Qt::black;
				}
				item = new QListWidgetItem(txtA.c_str());
				item->setForeground(QBrush(col));
				listWidget_connections->addItem(item);
				listWidget_connections->setCurrentRow(listWidget_connections->count() - 1);
			}
			else if(stat < JVX_STATE_ACTIVE)
			{
				txtA += " -> INCOMPLETE.";
				col = Qt::blue;
				item = new QListWidgetItem(txtA.c_str());
				item->setForeground(QBrush(col));
				listWidget_connections->addItem(item);
				listWidget_connections->setCurrentRow(listWidget_connections->count() - 1);
			}
			else
			{ 
				txtA += " -> RUNNING.";
				col = Qt::blue;
				item = new QListWidgetItem(txtA.c_str());
				item->setForeground(QBrush(col));
				listWidget_connections->addItem(item);
				listWidget_connections->setCurrentRow(listWidget_connections->count() - 1);
			}
			
			theDataConnections->return_reference_connection_process(theProcess);
		}
	}
}

void
jvx_connections_widget::process_disconnect()
{
	IjvxDataConnectionProcess* theProcess_rem = NULL;
	IjvxDataConnectionGroup* theGroup_rem = NULL;
	jvxSize select_id_proc_grp = JVX_SIZE_UNSELECTED;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxConnectionWidgetProcessGroupType pTp = id_select_process_group.proc_grp_tp;
	ui_get_reference_process(&theProcess_rem, &theGroup_rem, &select_id_proc_grp);

	if (theProcess_rem)
	{
		theProcess_rem->remove_connection();
		theProcess_rem->deassociate_master();
		theDataConnections->return_reference_connection_process(theProcess_rem);
	}
	if (theGroup_rem)
	{
		theGroup_rem->remove_connection();
		theDataConnections->return_reference_connection_group(theGroup_rem);
	}	

	this->update_window();

	if (theBwdRef)
	{
		theBwdRef->report_widget_specific(JVX_QTWIDGET_SPECIFIC_REPORT_UDPATE_WINDOW, NULL);
	}
}

void
jvx_connections_widget::process_remove()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxDataConnectionProcess* theProcess_rem = NULL;
	IjvxDataConnectionGroup* theGroup_rem = NULL;
	jvxSize select_id_proc_grp = JVX_SIZE_UNSELECTED;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxConnectionWidgetProcessGroupType pTp = id_select_process_group.proc_grp_tp;
	ui_get_reference_process(&theProcess_rem, &theGroup_rem, &select_id_proc_grp);

	if (theProcess_rem)
	{
		theDataConnections->uid_for_reference(theProcess_rem, &uId);
		theProcess_rem->remove_connection();
		theDataConnections->return_reference_connection_process(theProcess_rem);
		res = theDataConnections->remove_connection_process(uId);
		assert(res == JVX_NO_ERROR);
	}
	if (theGroup_rem)
	{
		theDataConnections->uid_for_reference(theGroup_rem, &uId);
		theGroup_rem->remove_connection();
		theDataConnections->return_reference_connection_group(theGroup_rem);
		res = theDataConnections->remove_connection_group(uId);
		assert(res == JVX_NO_ERROR);
	}

	id_select_process_group.proc_grp_tp = JVX_CONNECTION_WIDGET_IS_A_PROCESS;
	id_select_process_group.id_proc_grp = JVX_SIZE_UNSELECTED;
	this->update_window();
}

void 
jvx_connections_widget::dz_process_name_edited()
{
	process_name = lineEdit_dz_procname->text().toLatin1().data();
}

void
jvx_connections_widget::dz_process_descr_edited()
{
	process_description = lineEdit_dz_procdescr->text().toLatin1().data();
}

void 
jvx_connections_widget::dz_master_clicked(QListWidgetItem* it)
{
	QVariant var = it->data(JVX_USER_ROLE_LISTWIDGET_ID_MASTER_ID);
	jvxBitField btf;
	id_select_master = JVX_SIZE_UNSELECTED;
	if (var.isValid())
	{
		id_select_master = var.toInt();

		jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_REFINE_SHIFT);
		update_window_core(btf);
	}
}

void 
jvx_connections_widget::dz_icon_clicked(QListWidgetItem* it)
{
	QVariant var = it->data(JVX_USER_ROLE_LISTWIDGET_ID_ICON_ID);
	jvxBitField btf;
	id_select_icon = JVX_SIZE_UNSELECTED;
	if (allow_select_connector)
	{
		if (var.isValid())
		{
			id_select_icon = var.toInt();

			jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_REFINE_SHIFT);
			update_window_core(btf);
		}
	}
}

void 
jvx_connections_widget::dz_ocon_clicked(QListWidgetItem* it)
{
	QVariant var = it->data(JVX_USER_ROLE_LISTWIDGET_ID_OCON_ID);
	jvxBitField btf;
	id_select_ocon = JVX_SIZE_UNSELECTED;
	if (allow_select_connector)
	{
		if (var.isValid())
		{
			id_select_ocon = var.toInt();

			jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_REFINE_SHIFT);
			update_window_core(btf);
		}
	}
}

void 
jvx_connections_widget::dz_bridge_clicked(QListWidgetItem* it)
{
	QVariant var = it->data(JVX_USER_ROLE_LISTWIDGET_ID_BRIDGE_ID);
	jvxBitField btf;

	if (!bridge_show_only_mode)
	{
		id_select_bridge = JVX_SIZE_UNSELECTED;
		if (var.isValid())
		{
			id_select_bridge = var.toInt();

			jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_REFINE_SHIFT);
			update_window_core(btf);
		}
	}
	else
	{
		id_select_bridge = JVX_SIZE_UNSELECTED;
		if (var.isValid())
		{
			id_select_bridge = var.toInt();
			jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_HIGHLIGHT_SHIFT);
			update_window_core(btf);
		}
	}
}

void 
jvx_connections_widget::dz_create_bridge()
{
	jvxBitField btf;
	QString bName = lineEdit_bridgename->text();
	std::string txt = bName.toLatin1().data();
	jvxBool allowCreate = true;
	std::string txtUse;
	if (txt.empty())
	{
		txt = "My New Bridge";
	}
	std::list<lst_elm_bidges>::iterator elm;
	jvxSize cnt = JVX_SIZE_UNSELECTED;
	while (1)
	{
		allowCreate = true;
		txtUse = txt;
		if (JVX_CHECK_SIZE_SELECTED(cnt))
		{
			txtUse += "(" + jvx_size2String(cnt) + ")";
		}
		elm = lst_bridges.begin();
		for (; elm != lst_bridges.end(); elm++)
		{
			if (elm->bridge_name == txtUse)
			{
				allowCreate = false;
				break;
			}
		}
		if (allowCreate)
			break;
		cnt++;
	}
	
	lst_elm_bidges newElm;
	newElm.bridge_name = txtUse;

	std::list<lst_elm_icons>::iterator elmIC = lst_inputs.begin();
	std::list<lst_elm_ocons>::iterator elmOC = lst_outputs.begin();

	if ((id_select_process_group.proc_grp_tp == JVX_CONNECTION_WIDGET_IS_A_GROUP) && (JVX_CHECK_SIZE_SELECTED(id_select_icon) && bridge_entry_chain))
	{
		std::advance(elmIC, id_select_icon);

		elm = lst_bridges.begin();
		for (; elm != lst_bridges.end(); elm++)
		{
			if (
				(elm->identify_in.fac_uid == elmIC->identify.fac_uid) &&
				(elm->identify_in.icon_id == elmIC->identify.icon_id))
			{
				allowCreate = false;
				break;
			}
			if (
				(elm->identify_out.fac_uid == JVX_SIZE_UNSELECTED) &&
				(elm->identify_out.ocon_id == JVX_SIZE_UNSELECTED))
			{
				allowCreate = false;
				break;
			}
		}
		if (allowCreate)
		{
			newElm.identify_in.fac_uid = elmIC->identify.fac_uid;
			newElm.identify_in.icon_id = elmIC->identify.icon_id;
			newElm.identify_out.fac_uid = JVX_SIZE_UNSELECTED;
			newElm.identify_out.ocon_id = JVX_SIZE_UNSELECTED;
		}
	}
	else if ((id_select_process_group.proc_grp_tp == JVX_CONNECTION_WIDGET_IS_A_GROUP) && JVX_CHECK_SIZE_SELECTED(id_select_ocon) && bridge_exit_chain)
	{
		std::advance(elmOC, id_select_ocon);

		elm = lst_bridges.begin();
		for (; elm != lst_bridges.end(); elm++)
		{
			if (
				(elm->identify_in.fac_uid == JVX_SIZE_UNSELECTED) &&
				(elm->identify_in.icon_id == JVX_SIZE_UNSELECTED))
			{
				allowCreate = false;
				break;
			}
			if (
				(elm->identify_out.fac_uid == elmOC->identify.fac_uid) &&
				(elm->identify_out.ocon_id == elmOC->identify.ocon_id))
			{
				allowCreate = false;
				break;
			}
		}
		if (allowCreate)
		{
			newElm.identify_in.fac_uid = JVX_SIZE_UNSELECTED;
			newElm.identify_in.icon_id = JVX_SIZE_UNSELECTED;
			newElm.identify_out.fac_uid = elmOC->identify.fac_uid;
			newElm.identify_out.ocon_id = elmOC->identify.ocon_id;
		}
	}
	else if (
		(JVX_CHECK_SIZE_SELECTED(id_select_ocon)) &&
		(JVX_CHECK_SIZE_SELECTED(id_select_icon)))
	{
		std::advance(elmIC, id_select_icon);
		std::advance(elmOC, id_select_ocon);

		elm = lst_bridges.begin();
		for (; elm != lst_bridges.end(); elm++)
		{
			if (
				(elm->identify_in.fac_uid == elmIC->identify.fac_uid) &&
				(elm->identify_in.icon_id == elmIC->identify.icon_id))
			{
				allowCreate = false;
				break;
			}
			if (
				(elm->identify_out.fac_uid == elmOC->identify.fac_uid) &&
				(elm->identify_out.ocon_id == elmOC->identify.ocon_id))
			{
				allowCreate = false;
				break;
			}
		}
		if (allowCreate)
		{
			newElm.identify_in.fac_uid = elmIC->identify.fac_uid;
			newElm.identify_in.icon_id = elmIC->identify.icon_id;
			newElm.identify_out.fac_uid = elmOC->identify.fac_uid;
			newElm.identify_out.ocon_id = elmOC->identify.ocon_id;
		}
	}
	else
	{
		allowCreate = false;
	}
	if (allowCreate)
	{
		newElm.dedicatedThread = bridge_ded_thread;
		newElm.boostThread = bridge_boost_thread;
		lst_bridges.push_back(newElm);

		id_select_bridge = lst_bridges.size() - 1;
		jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
		jvx_bitSet(btf, REDRAW_PROCESS_DROPZONE_REFINE_SHIFT);
		update_window_core(btf);
	}
}

void 
jvx_connections_widget::dz_remove_bridge()
{
	jvxBitField btf;
	if (
		(JVX_CHECK_SIZE_SELECTED(id_select_bridge)))
	{
		std::list<lst_elm_bidges>::iterator elm = lst_bridges.begin();
		std::advance(elm, id_select_bridge);
		lst_bridges.erase(elm);
		id_select_bridge = JVX_MIN(id_select_bridge, lst_bridges.size() - 1);
		jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
		jvx_bitSet(btf, REDRAW_PROCESS_DROPZONE_REFINE_SHIFT);
		update_window_core(btf);
	}
}

void 
jvx_connections_widget::dz_create_proc()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxSize numP = 0;
	jvxSize numG = 0;
	std::string newName;
	std::string newDescr;
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_process_group.id_proc_grp))
	{
		jvxSize cnt = JVX_SIZE_UNSELECTED;
		while (1)
		{
			newName = process_name;
			newDescr = process_description;

			if (newName.empty())
			{
				if (id_select_process_group.proc_grp_tp == JVX_CONNECTION_WIDGET_IS_A_PROCESS)
				{
					newName = "My New Process";
				}
				else
				{
					newName = "My New Group";
				}
			}

			if (JVX_CHECK_SIZE_SELECTED(cnt))
			{
				newName += "(" + jvx_size2String(cnt) + ")";
			}
			cnt++;
			if (id_select_process_group.proc_grp_tp == JVX_CONNECTION_WIDGET_IS_A_PROCESS)
			{
				res = theDataConnections->create_connection_process(newName.c_str(), &uId, process_ic, process_ess, verbose_mode, false);
			}
			else
			{
				res = theDataConnections->create_connection_group(newName.c_str(), &uId, newDescr.c_str(), verbose_mode);
			}
			if (res != JVX_ERROR_ALREADY_IN_USE)
			{
				break;
			}
		}
		if(res == JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
			
			if (id_select_process_group.proc_grp_tp == JVX_CONNECTION_WIDGET_IS_A_PROCESS)
			{
				IjvxDataConnectionProcess* theProc = NULL;
				theDataConnections->reference_connection_process_uid(uId, &theProc);	
				if (theProc)
				{
					res = connect_process_from_dropzone(theDataConnections, theProc JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}

				theDataConnections->return_reference_connection_process(theProc);
				theProc = NULL;
				if (res != JVX_NO_ERROR)
				{
					theDataConnections->remove_connection_process(uId);
				}
				else
				{
					theDataConnections->number_connections_process(&numP);
					id_select_process_group.id_proc_grp = (jvxSize)((int)numP - 1);
					id_select_process_group.proc_grp_tp = JVX_CONNECTION_WIDGET_IS_A_PROCESS;

					lst_bridges.clear();
					id_select_bridge = JVX_SIZE_UNSELECTED;
					jvx_cleanup_candidates_dropzone(theDataConnections);

					jvxBitField btf;
					jvx_bitFull(btf);
					update_window_core(btf);

					if (theBwdRef)
					{
						theBwdRef->report_widget_specific(JVX_QTWIDGET_SPECIFIC_REPORT_UDPATE_WINDOW, NULL);
					}
				}
			}
			else
			{
				IjvxDataConnectionGroup* theGroup = NULL;
				theDataConnections->reference_connection_group_uid(uId, &theGroup);
				if (theGroup)
				{
					res = connect_group_from_dropzone(theDataConnections, theGroup JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}

				theDataConnections->return_reference_connection_group(theGroup);
				theGroup = NULL;
				if (res != JVX_NO_ERROR)
				{
					theDataConnections->remove_connection_group(uId);
				}
				else
				{
					theDataConnections->number_connections_process(&numP);
					theDataConnections->number_connections_group(&numG);
					id_select_process_group.id_proc_grp = (jvxSize)((int)numG - 1);
					id_select_process_group.proc_grp_tp = JVX_CONNECTION_WIDGET_IS_A_GROUP;
					lst_bridges.clear();
					id_select_bridge = JVX_SIZE_UNSELECTED;
					jvx_cleanup_candidates_dropzone(theDataConnections);

					jvxBitField btf;
					jvx_bitFull(btf);
					update_window_core(btf);

					if (theBwdRef)
					{
						theBwdRef->report_widget_specific(JVX_QTWIDGET_SPECIFIC_REPORT_UDPATE_WINDOW, NULL);
					}
				}
			}
		}

		if (res != JVX_NO_ERROR)
		{
			std::cout << "Unable to create process " << newName << std::endl;
		}
	}
}

void 
jvx_connections_widget::dz_connect_proc()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numP = 0;
	jvxSize numG = 0;
	if (JVX_CHECK_SIZE_SELECTED(id_select_process_group.id_proc_grp))
	{
		switch (id_select_process_group.proc_grp_tp)
		{
		case JVX_CONNECTION_WIDGET_IS_A_PROCESS:
			theDataConnections->number_connections_process(&numP);
			if (id_select_process_group.id_proc_grp < numP)
			{
				IjvxDataConnectionProcess* theProc = NULL;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb)
					theDataConnections->reference_connection_process(id_select_process_group.id_proc_grp, &theProc);
				if (theProc)
				{
					res = connect_process_from_dropzone(theDataConnections, theProc JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res != JVX_NO_ERROR)
					{
						std::string txtA = "Failed to connect from dropzone, reason: <";
						txtA += jvxErrorType_descr(res);
						txtA += ">.";
						
						QListWidgetItem* item = new QListWidgetItem(txtA.c_str());
						listWidget_connections->addItem(item);
						listWidget_connections->setCurrentRow(listWidget_connections->count() - 1);
					}
				}
				theDataConnections->return_reference_connection_process(theProc);
			}
			break;

		case JVX_CONNECTION_WIDGET_IS_A_GROUP:
			theDataConnections->number_connections_group(&numG);
			if (id_select_process_group.id_proc_grp < numG)
			{
				IjvxDataConnectionGroup* theGroup = NULL;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb)
					theDataConnections->reference_connection_group(id_select_process_group.id_proc_grp, &theGroup);
				if (theGroup)
				{
					res = connect_group_from_dropzone(theDataConnections, theGroup JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				theDataConnections->return_reference_connection_group(theGroup);
			}
			break;
		}
		if (res == JVX_NO_ERROR)
		{
			// Set into the initial position
			id_select_process_group.proc_grp_tp = JVX_CONNECTION_WIDGET_IS_A_PROCESS;
			id_select_process_group.id_proc_grp = JVX_SIZE_UNSELECTED;

			lst_bridges.clear();
			id_select_bridge = JVX_SIZE_UNSELECTED;
			jvx_cleanup_candidates_dropzone(theDataConnections);
			jvxBitField btf;
			jvx_bitFull(btf);
			update_window_core(btf);

			if (theBwdRef)
			{
				theBwdRef->report_widget_specific(JVX_QTWIDGET_SPECIFIC_REPORT_UDPATE_WINDOW, NULL);
			}
		}
	}
}

void 
jvx_connections_widget::dz_clear()
{
	dz_clear_icon();
	dz_clear_ocon();
	dz_clear_master();
	dz_clear_bridges();
}

void 
jvx_connections_widget::add_master_dz()
{
	jvxBitField btf;
	if (JVX_CHECK_SIZE_SELECTED(id_selected_master.uid_select_masfac))
	{
		IjvxConnectionMasterFactory* theMasFac = NULL;
		IjvxConnectionMaster* theMas = NULL;
		theDataConnections->reference_connection_master_factory_uid(id_selected_master.uid_select_masfac, &theMasFac);
		if (theMasFac)
		{
			if (JVX_CHECK_SIZE_SELECTED(id_selected_master.id_select_mas))
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
						if (!alreadyThere)
						{
							lst_elm_master newElm;
							newElm.identify.mas_fac_uid = id_selected_master.uid_select_masfac;
							newElm.identify.mas_id = id_selected_master.id_select_mas;
							lst_masters.push_back(newElm);
							jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
							update_window_core(btf);
						}
					}
					
					theMasFac->return_reference_connector_master(theMas);
				}
				theDataConnections->return_reference_connection_master_factory(theMasFac);
			}
		}
	}
}

void 
jvx_connections_widget::add_connector_dz()
{
	jvxBitField btf;
	if (JVX_CHECK_SIZE_SELECTED(id_selected_connector.uid_select_confac))
	{

		IjvxConnectorFactory* theConFac = NULL;
		IjvxInputConnector* theI = NULL;
		IjvxOutputConnector* theO = NULL;
		theDataConnections->reference_connection_factory_uid(id_selected_connector.uid_select_confac, &theConFac);
		if (theConFac)
		{
			if (JVX_CHECK_SIZE_SELECTED(id_selected_connector.id_select_icon))
			{
				theConFac->reference_input_connector(id_selected_connector.id_select_icon, &theI);
				if (theI)
				{
					IjvxDataConnectionCommon* ref = NULL;
					theI->associated_common_icon(&ref);
					if (ref == NULL)
					{
						jvxBool alreadythere = false;
						std::list<lst_elm_icons>::iterator elmI = lst_inputs.begin();
						for (; elmI != lst_inputs.end(); elmI++)
						{
							if (
								(elmI->identify.fac_uid == id_selected_connector.uid_select_confac) &&
								(elmI->identify.icon_id == id_selected_connector.id_select_icon))
							{
								alreadythere = true;
								break;
							}
						}

						if (!alreadythere)
						{
							lst_elm_icons newElm;
							newElm.identify.fac_uid = id_selected_connector.uid_select_confac;
							newElm.identify.icon_id = id_selected_connector.id_select_icon;
							lst_inputs.push_back(newElm);
							jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
							update_window_core(btf);
						}
					}

					theConFac->return_reference_input_connector(theI);
				}

			}
			if (JVX_CHECK_SIZE_SELECTED(id_selected_connector.id_select_ocon))
			{
				theConFac->reference_output_connector(id_selected_connector.id_select_ocon, &theO);
				if (theO)
				{
					IjvxDataConnectionCommon* ref = NULL;
					theO->associated_common_ocon(&ref);
					if (ref == NULL)
					{
						jvxBool alreadythere = false;
						std::list<lst_elm_ocons>::iterator elmO = lst_outputs.begin();
						for (; elmO != lst_outputs.end(); elmO++)
						{
							if (
								(elmO->identify.fac_uid  == id_selected_connector.uid_select_confac) && 
								(elmO->identify.ocon_id == id_selected_connector.id_select_ocon))
							{
								alreadythere = true;
								break;
							}
						}

						if (!alreadythere)
						{
							lst_elm_ocons newElm;
							newElm.identify.fac_uid = id_selected_connector.uid_select_confac;
							newElm.identify.ocon_id = id_selected_connector.id_select_ocon;
							lst_outputs.push_back(newElm);
							jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
							update_window_core(btf);
						}
					}

					theConFac->return_reference_output_connector(theO);
				}
			}

			theDataConnections->return_reference_connection_factory(theConFac);
		}
	}
}

void 
jvx_connections_widget::master_factory_item_clicked(QTreeWidgetItem* it, int col)
{
	id_selected_master.uid_select_masfac = JVX_SIZE_UNSELECTED;
	id_selected_master.id_select_mas = JVX_SIZE_UNSELECTED;

	QVariant varuId = it->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_MF_UID);
	QVariant varid = it->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_F_ID);
	if (varuId.isValid() && varid.isValid())
	{
		id_selected_master.uid_select_masfac = varuId.toInt();
		id_selected_master.id_select_mas = varid.toInt();
	}

	jvxBitField btf;
	this->update_window_core(btf);
}

void 
jvx_connections_widget::connector_factory_item_clicked(QTreeWidgetItem* it, int col)
{
	id_selected_connector.uid_select_confac = JVX_SIZE_UNSELECTED;
	id_selected_connector.id_select_icon = JVX_SIZE_UNSELECTED;
	id_selected_connector.id_select_ocon = JVX_SIZE_UNSELECTED;

	QVariant varuId = it->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_CF_UID);
	QVariant varidic = it->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_IC_ID);
	QVariant varidoc = it->data(0, JVX_USER_ROLE_TREEWIDGET_PROPERTY_OC_ID);
	if (varuId.isValid())
	{
		if (varidic.isValid())
		{
			id_selected_connector.uid_select_confac = varuId.toInt();
			id_selected_connector.id_select_icon = varidic.toInt();
		}
		if (varidoc.isValid())
		{
			id_selected_connector.uid_select_confac = varuId.toInt();
			id_selected_connector.id_select_ocon = varidoc.toInt();
		}
	}

	std::cout << "Uid = " << id_selected_connector.uid_select_confac << ": idic = " << id_selected_connector.id_select_icon << ": idoc = " << id_selected_connector.id_select_ocon << std::endl;
	jvxBitField btf;
	this->update_window_core(btf);
}

void 
jvx_connections_widget::dz_clear_master()
{
	jvxBitField btf;
	jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);

	lst_masters.clear();
	update_window_core(btf);
}

void 
jvx_connections_widget::dz_clear_icon()
{
	jvxBitField btf;
	jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);

	lst_inputs.clear();

	update_window_core(btf);
}

void
jvx_connections_widget::dz_clear_bridges()
{
	jvxBitField btf;
	jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);

	lst_bridges.clear();
	id_select_bridge = JVX_SIZE_UNSELECTED;
	update_window_core(btf);
}

void 
jvx_connections_widget::dz_clear_ocon()
{
	jvxBitField btf;
	jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);

	lst_outputs.clear();

	update_window_core(btf);
}

void
jvx_connections_widget::add_masters_dz()
{
	jvxSize i,j;
	jvxSize numMaF = 0;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	theDataConnections->number_connection_master_factories(&numMaF);
	for (i = 0; i < numMaF; i++)
	{
		IjvxConnectionMasterFactory* theMaFac = NULL;
		uId = JVX_SIZE_UNSELECTED;
		theDataConnections->reference_connection_master_factory(i, &theMaFac, &uId);
		assert(theMaFac);

		jvxSize numMa = 0;
		theMaFac->number_connector_masters(&numMa);
		for (j = 0; j < numMa; j++)
		{
			IjvxConnectionMaster* theMa = NULL;
			jvxBool isAvail = false;
			theMaFac->reference_connector_master(j, &theMa);
			assert(theMa);
			theMa->available_master(&isAvail);
			if (isAvail)
			{
				lst_elm_master newElm;
				jvxBool alreadyThere = false;
				newElm.identify.mas_fac_uid = uId;
				newElm.identify.mas_id = j;
				std::list<lst_elm_master>::iterator elmMa = lst_masters.begin();
				for (; elmMa != lst_masters.end(); elmMa++)
				{
					if (
						(elmMa->identify.mas_fac_uid == newElm.identify.mas_fac_uid) &&
						(elmMa->identify.mas_id == newElm.identify.mas_id))
					{
						alreadyThere = true;
						break;
					}
				}
				if (!alreadyThere)
				{
					lst_masters.push_back(newElm);
				}
			}
			theMaFac->return_reference_connector_master(theMa);
		}
		theDataConnections->return_reference_connection_master_factory(theMaFac);
	}
	jvxBitField btf;
	jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
	update_window_core(btf);
}

void
jvx_connections_widget::add_connectors_dz()
{
	jvxBitField btf;

	jvxSize i, j;
	jvxSize numCoF = 0;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	theDataConnections->number_connection_factories(&numCoF);
	for (i = 0; i < numCoF; i++)
	{
		IjvxConnectorFactory* theCoFac = NULL;
		uId = JVX_SIZE_UNSELECTED;
		theDataConnections->reference_connection_factory(i, &theCoFac, &uId);
		assert(theCoFac);

		jvxSize numIc = 0;
		jvxSize numOc = 0;

		theCoFac->number_input_connectors(&numIc);
		for (j = 0; j < numIc; j++)
		{
			IjvxInputConnector* theIc = NULL;
			IjvxDataConnectionCommon* com = NULL;
			theCoFac->reference_input_connector(j, &theIc);
			assert(theIc);
			theIc->associated_common_icon(&com);
			if (!com)
			{
				lst_elm_icons newElm;
				jvxBool alreadyThere = false;
				newElm.identify.fac_uid = uId;
				newElm.identify.icon_id = j;
				std::list<lst_elm_icons>::iterator elmIc = lst_inputs.begin();
				for (; elmIc != lst_inputs.end(); elmIc++)
				{
					if (
						(elmIc->identify.fac_uid == newElm.identify.fac_uid) &&
						(elmIc->identify.icon_id == newElm.identify.icon_id))
					{
						alreadyThere = true;
						break;
					}
				}
				if (!alreadyThere)
				{
					lst_inputs.push_back(newElm);
				}
			}
			theCoFac->return_reference_input_connector(theIc);
		}

		theCoFac->number_output_connectors(&numOc);
		for (j = 0; j < numOc; j++)
		{
			IjvxOutputConnector* theOc = NULL;
			IjvxDataConnectionCommon* com = NULL;
			theCoFac->reference_output_connector(j, &theOc);
			assert(theOc);
			theOc->associated_common_ocon(&com);
			if (!com)
			{
				lst_elm_ocons newElm;
				jvxBool alreadyThere = false;
				newElm.identify.fac_uid = uId;
				newElm.identify.ocon_id = j;
				std::list<lst_elm_ocons>::iterator elmOc = lst_outputs.begin();
				for (; elmOc != lst_outputs.end(); elmOc++)
				{
					if (
						(elmOc->identify.fac_uid == newElm.identify.fac_uid) &&
						(elmOc->identify.ocon_id == newElm.identify.ocon_id))
					{
						alreadyThere = true;
						break;
					}
				}
				if (!alreadyThere)
				{
					lst_outputs.push_back(newElm);
				}
			}
			theCoFac->return_reference_output_connector(theOc);
		}
		theDataConnections->return_reference_connection_factory(theCoFac);
	}

	// Update UI
	jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
	update_window_core(btf);
}

void
jvx_connections_widget::closeEvent(QCloseEvent * event)
{
	QWidget::closeEvent(event);
	if (theBwdRef)
	{
		theBwdRef->report_widget_closed(static_cast<QWidget*>(this));
	}
}

void
jvx_connections_widget::modal_showresult()
{
	jvx_connect_results* modalShow = new jvx_connect_results(this);
	modalShow->setModal(true);
	modalShow->create(latest_status_process);
	modalShow->show();

}

void
jvx_connections_widget::dz_switch_process_group(int idsel)
{
	id_select_process_group.proc_grp_tp = (jvxConnectionWidgetProcessGroupType)idsel;
	lst_bridges.clear();
	ui_redraw_dropzone(NULL, NULL);
	ui_refine_dropzone(NULL, NULL);
}

void
jvx_connections_widget::dz_bridge_ded_thread()
{
	bridge_ded_thread = !bridge_ded_thread;
	ui_refine_dropzone(NULL, NULL);
}

void
jvx_connections_widget::dz_bridge_boost_thread()
{
	bridge_boost_thread = !bridge_boost_thread;
	ui_refine_dropzone(NULL, NULL);
}

void
jvx_connections_widget::dz_bridge_entry()
{
	bridge_entry_chain = !bridge_entry_chain;
	if (bridge_entry_chain)
		bridge_exit_chain = false;
	ui_refine_dropzone(NULL, NULL);
}

void
jvx_connections_widget::dz_bridge_exit()
{
	bridge_exit_chain = !bridge_exit_chain;
	if (bridge_exit_chain)
		bridge_entry_chain = false;
	ui_refine_dropzone(NULL, NULL);
}


void
jvx_connections_widget::dz_clicked_interceptors(bool tog)
{
	jvxBitField btf;
	process_ic = tog;
	jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
	update_window_core(btf);
}

void
jvx_connections_widget::dz_clicked_essential(bool tog)
{
	jvxBitField btf;
	process_ess = tog;
	jvx_bitZSet(btf, REDRAW_PROCESS_DROPZONE_FULL_SHIFT);
	update_window_core(btf);
}

jvxErrorType 
jvx_connections_widget::check_connections_ready(jvxApiString* astr_ready)
{
	jvxSize numP = 0;
	IjvxDataConnectionProcess* theProcess = NULL;
	jvxApiString descr;
	jvxErrorType res = JVX_NO_ERROR;
	
	if (theDataConnections)
	{
		res = theDataConnections->ready_for_start(astr_ready);
	}
	else
	{
		astr_ready->assign("Error: Data processing subsystem not ready.");
		res = JVX_ERROR_NOT_READY;
	}
	return res;
}
