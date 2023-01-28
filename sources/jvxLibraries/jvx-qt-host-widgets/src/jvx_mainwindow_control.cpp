#include "jvx_mainwindow_control.h"
#include "jvx_newvalue_dialog.h"

CjvxMainWindowControl::CjvxMainWindowControl()
{
	uniqueIdRegisterFcts = 1;
	menuSystem = NULL;
	menuApplication = NULL;
	parentObj = NULL;
	report = NULL;
	theState = JVX_STATE_NONE;
}

jvxErrorType 
CjvxMainWindowControl::init(IjvxMainWindowControl_connect* rep, QMenu* menAttachSystem, QMenu* menAttachApplication, QWidget* parO)
{
	if (theState == JVX_STATE_NONE)
	{
		theState = JVX_STATE_INIT;
		menuSystem = menAttachSystem;
		menuApplication = menAttachApplication;
		parentObj = parO;
		report = rep;
		return JVX_NO_ERROR;
	}

	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxMainWindowControl::terminate()
{
	if (theState == JVX_STATE_INIT)
	{
		myRegisteredActions.clear();
		menuSystem = NULL;
		menuApplication = NULL;
		parentObj = NULL;
		theState = JVX_STATE_NONE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxMainWindowControl::trigger_operation(
	jvxMainWindowController_trigger_operation_ids id_operation,
	jvxHandle* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification* tp = NULL;

	switch (id_operation)
	{
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_REGISTER_FRAME:
		if (theState != JVX_STATE_INIT)
			return JVX_ERROR_WRONG_STATE;
		if (theData)
		{
			jvxQtFrameAndWidget* widgetFrame = (jvxQtFrameAndWidget*)theData;
			res = register_action_widget(widgetFrame->description, widgetFrame->theConnectionsWidget,
				widgetFrame->theConnectionsFrame);
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_MAINWINDOWCONTROLLER_TRIGGER_UNREGISTER_FRAME:
		if (theState != JVX_STATE_INIT)
			return JVX_ERROR_WRONG_STATE;
		if (theData)
		{
			jvxQtFrameAndWidget* widgetFrame = (jvxQtFrameAndWidget*)theData;
			res = unregister_action_widget(widgetFrame->description, widgetFrame->theConnectionsWidget,
				widgetFrame->theConnectionsFrame);
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	default:
		if (report)
		{
			res = report->forward_trigger_operation(id_operation, theData);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
	}
	return(res);
}

jvxErrorType 
CjvxMainWindowControl::register_functional_callback(
	jvxMainWindowController_register_functional_ids id_operation,
	IjvxMainWindowController_report* bwd,
	jvxHandle* privData,
	const char* contentData,
	jvxSize* id, jvxBool is_parameter, 
	jvxMainWindowController_register_separator_ids sepEnum)
{
	QAction* act = NULL;
	std::string showTxt = contentData;
	jvxBool valB = false;
	jvxInt32 valI;

	switch (sepEnum)
	{
	case JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST:

		if (is_parameter)
		{
			QList<QAction*> lst = menuApplication->actions();
			if (lst.count() != 0)
			{
				menuApplication->addSeparator();
			}
		}
		else
		{
			QList<QAction*> lst = menuSystem->actions();
			if (lst.count() != 0)
			{
				menuSystem->addSeparator();
			}
		}
		break;
	case JVX_REGISTER_FUNCTION_MENU_SEPARATOR:
		if (is_parameter)
		{
			menuApplication->addSeparator();
		}
		else
		{
			menuSystem->addSeparator();
		}
		break;
	}

	switch (id_operation)
	{
	case JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF:
		act = new QAction(parentObj);
		act->setText(contentData);
		act->setProperty("task", QVariant(100));
		act->setProperty("identification", QVariant(0));
		act->setProperty("subid", QVariant(JVX_SIZE_INT32(uniqueIdRegisterFcts)));
		act->setProperty("report", QVariant::fromValue(reinterpret_cast<void*>(bwd)));
		act->setProperty("private", QVariant::fromValue(reinterpret_cast<void*>(privData)));
		act->setCheckable(true);
		connect(act, SIGNAL(triggered()), this, SLOT(action_value_selection_slot()));
		if (is_parameter)
		{
			menuApplication->addAction(act);
		}
		else
		{
			menuSystem->addAction(act);
		}
		*id = uniqueIdRegisterFcts;
		uniqueIdRegisterFcts++;
		myRegisteredActions[*id] = act;
		break;
	case JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE:
		act = new QAction(this);
		act->setText(showTxt.c_str());
		act->setProperty("init_txt", QVariant(QString(contentData)));
		act->setProperty("task", QVariant(100));
		act->setProperty("identification", QVariant(1));
		act->setProperty("subid", QVariant(JVX_SIZE_INT32(uniqueIdRegisterFcts)));
		act->setProperty("report", QVariant::fromValue(reinterpret_cast<void*>(bwd)));
		act->setProperty("private", QVariant::fromValue(reinterpret_cast<void*>(privData)));
		connect(act, SIGNAL(triggered()), this, SLOT(action_value_selection_slot()));
		if (is_parameter)
		{
			menuApplication->addAction(act);
		}
		else
		{
			menuSystem->addAction(act);
		}
		*id = uniqueIdRegisterFcts;
		uniqueIdRegisterFcts++;
		act->setCheckable(false);
		myRegisteredActions[*id] = act;
		break;
	case JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_DATA_VALUE:
		act = new QAction(this);
		act->setText(showTxt.c_str());
		act->setProperty("init_txt", QVariant(QString(contentData)));
		act->setProperty("task", QVariant(100));
		act->setProperty("identification", QVariant(2));
		act->setProperty("subid", QVariant(JVX_SIZE_INT32(uniqueIdRegisterFcts)));
		act->setProperty("report", QVariant::fromValue(reinterpret_cast<void*>(bwd)));
		act->setProperty("private", QVariant::fromValue(reinterpret_cast<void*>(privData)));
		connect(act, SIGNAL(triggered()), this, SLOT(action_value_selection_slot()));
		if (is_parameter)
		{
			menuApplication->addAction(act);
		}
		else
		{
			menuSystem->addAction(act);
		}
		*id = uniqueIdRegisterFcts;
		uniqueIdRegisterFcts++;
		act->setCheckable(false);
		myRegisteredActions[*id] = act;
		break;
	default:
		break;
	}

	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxMainWindowControl::unregister_functional_callback(jvxSize id)
{
	int i;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	QList<QAction *> lst = menuApplication->actions();
	for (i = 0; i < lst.count(); i++)
	{
		QAction* act = lst[i];
		if (act)
		{
			QVariant var = act->property("subid");
			if (var.isValid())
			{
				if (var.toInt() == id)
				{
					menuApplication->removeAction(act);
					break;
				}
			}
		}
	}

	lst = menuApplication->actions();
	for (i = 0; i < lst.count(); i++)
	{
		QAction* act = lst[i];
		if (act)
		{
			QVariant var = act->property("subid");
			if (var.isValid())
			{
				if (var.toInt() == id)
				{
					menuApplication->removeAction(act);
					break;
				}
			}
		}
	}
	lst = menuSystem->actions();
	for (i = 0; i < lst.count(); i++)
	{
		QAction* act = lst[i];
		if (act)
		{
			QVariant var = act->property("subid");
			if (var.isValid())
			{
				if (var.toInt() == id)
				{
					menuSystem->removeAction(act);
					break;
				}
			}
		}
	}

	auto elmE = myRegisteredActions.find(id);
	if (elmE != myRegisteredActions.end())
	{
		myRegisteredActions.erase(elmE);
	}
	return(res);
}

void
CjvxMainWindowControl::action_value_selection_slot()
{
	QObject* senderO = this->sender();
	if (senderO != NULL)
	{
		QAction* act = dynamic_cast<QAction*>(senderO);
		QVariant task = senderO->property("task");
		QVariant var = senderO->property("identification");
		QVariant vars = senderO->property("subid");
		QVariant varsCB, varsPD;
		jvx_newvalue_dialog* getNewValue = NULL;
		jvxInt32 valI;
		jvxData valD;
		jvxBool valB;
		QVariant varT;
		QVariant varI;
		QVariant varD;
		std::string showTxt;

		if (task.isValid() && var.isValid() && vars.isValid())
		{
			int taskid = task.toInt();
			int id = var.toInt();
			int uId = vars.toInt();
			switch (taskid)
			{
			case 100:
				varsCB = senderO->property("report");
				varsPD = senderO->property("private");
				if (varsCB.isValid() && varsPD.isValid())
				{
					IjvxMainWindowController_report* cbPtr = reinterpret_cast<IjvxMainWindowController_report*>(varsCB.value<void*>());
					jvxHandle* prPtr = varsPD.value<void*>();
					jvxValue var;
					if (cbPtr)
					{
						if (act)
						{
							switch (id)
							{
							case 0:
								var.assign(act->isChecked());
								cbPtr->set_variable_edit(prPtr, var, uId);

								updateSingleAction(act);
								cbPtr->get_variable_edit(prPtr, var, uId);
								var.toContent(&valB);
								act->setChecked(valB);
								break;
							case 1:
								varT = act->property("init_txt");
								varI = act->property("current_val");
								valI = varI.toInt();
								getNewValue = new jvx_newvalue_dialog(parentObj, "Specify new value", &valI);
								getNewValue->exec();
								var.assign(valI);
								cbPtr->set_variable_edit(prPtr, var, uId);

								updateSingleAction(act);

								cbPtr->get_variable_edit(prPtr, var, uId);
								var.toContent(&valI);
								act->setProperty("current_val", QVariant(valI));
								showTxt = varT.toString().toLatin1().data();
								showTxt += " -> ";
								showTxt += jvx_int2String(valI);
								act->setText(showTxt.c_str());
								break;
							case 2:
								varT = act->property("init_txt");
								varD = act->property("current_val");
								valD = varD.toData();
								getNewValue = new jvx_newvalue_dialog(parentObj, "Specify new value", &valD, 4);
								getNewValue->exec();
								var.assign(valD);
								cbPtr->set_variable_edit(prPtr, var, uId);

								updateSingleAction(act);

								cbPtr->get_variable_edit(prPtr, var, uId);
								var.toContent(&valD);
								act->setProperty("current_val", QVariant(valD));
								showTxt = varT.toString().toLatin1().data();
								showTxt += " -> ";
								showTxt += jvx_data2String(valD, 4);
								act->setText(showTxt.c_str());
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

jvxErrorType
CjvxMainWindowControl::register_action_widget(const std::string& descr, IjvxQtSpecificWidget_base* widget, IjvxQtWidgetFrameDialog* fwidget,
	jvxMainWindowController_register_separator_ids sepEnum)
{
	std::map<std::string, subWidgetAndAction>::iterator elm = registeredSubwidgets.find(descr);
	if (elm == registeredSubwidgets.end())
	{
		jvxSize cnt = 0;
		subWidgetAndAction newElm;
		
		switch (sepEnum)
		{
		case JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST:
			if (menuSystem->actions().count() != 0)
			{
				menuSystem->addSeparator();
			}
			break;
		case JVX_REGISTER_FUNCTION_MENU_SEPARATOR:
			menuSystem->addSeparator();
			break;
		}

		newElm.action = new QAction(this);
		newElm.action->setObjectName(descr.c_str());
		newElm.action->setCheckable(true);
		newElm.action->setText(descr.c_str());

		connect(newElm.action, SIGNAL(triggered()), this, SLOT(action_triggered()));
		menuSystem->addAction(newElm.action);
		newElm.widget = widget;
		newElm.fWidget = fwidget;
		//newElm.fQWidgets;		
		while (1)
		{
			QDialog* dia = NULL;
			newElm.fWidget->getMyQDialog(&dia, cnt);
			if (dia)
			{
				newElm.fQWidgets.push_back(dia);
			}
			else
			{
				break;
			}
			cnt++;
		}


		registeredSubwidgets[descr] = newElm;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType
CjvxMainWindowControl::unregister_action_widget(const std::string& descr, IjvxQtSpecificWidget_base* widget, IjvxQtWidgetFrameDialog* fwidget)
{
	std::map<std::string, subWidgetAndAction>::iterator elm = registeredSubwidgets.find(descr);
	if (elm != registeredSubwidgets.end())
	{
		if ((elm->second.widget == widget) &&
			(elm->second.fWidget == fwidget))
		{
			disconnect(elm->second.action, SIGNAL(triggered()));
			menuSystem->removeAction(elm->second.action);
			delete(elm->second.action);
		}
		registeredSubwidgets.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

void
CjvxMainWindowControl::updateSingleAction(QAction* act)
{
	QVariant task = act->property("task");
	QVariant var = act->property("identification");
	QVariant vars = act->property("subid");
	QVariant varsCB, varsPD;
	jvxInt32 valI;
	jvxData valD;
	jvxBool valB;
	QVariant varT;
	QVariant varI;
	QVariant varD;
	std::string showTxt;

	if (task.isValid() && var.isValid() && vars.isValid())
	{
		int taskid = task.toInt();
		int id = var.toInt();
		int uId = vars.toInt();
		switch (taskid)
		{
		case 100:
			varsCB = act->property("report");
			varsPD = act->property("private");
			if (varsCB.isValid() && varsPD.isValid())
			{
				IjvxMainWindowController_report* cbPtr = reinterpret_cast<IjvxMainWindowController_report*>(varsCB.value<void*>());
				jvxHandle* prPtr = varsPD.value<void*>();
				jvxValue var;
				if (cbPtr)
				{
					if (act)
					{
						switch (id)
						{
						case 0:
							cbPtr->get_variable_edit(prPtr, var, uId);
							var.toContent(&valB);
							act->setChecked(valB);
							break;
						case 1:
							varT = act->property("init_txt");
							varI = act->property("current_val");

							cbPtr->get_variable_edit(prPtr, var, uId);
							var.toContent(&valI);
							act->setProperty("current_val", QVariant(valI));
							showTxt = varT.toString().toLatin1().data();
							showTxt += " -> ";
							showTxt += jvx_int2String(valI);
							act->setText(showTxt.c_str());
							break;
						case 2:
							varT = act->property("init_txt");
							varD = act->property("current_val");

							cbPtr->get_variable_edit(prPtr, var, uId);
							var.toContent(&valD);
							act->setProperty("current_val", QVariant(valD));
							showTxt = varT.toString().toLatin1().data();
							showTxt += " -> ";
							showTxt += jvx_data2String(valD, 4);
							act->setText(showTxt.c_str());
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

jvxErrorType 
CjvxMainWindowControl::activate()
{
	if (theState == JVX_STATE_INIT)
	{

		std::map<std::string, subWidgetAndAction>::iterator elmw = registeredSubwidgets.begin();
		for (; elmw != registeredSubwidgets.end(); elmw++)
		{
			elmw->second.widget->activate();
		}
		theState = JVX_STATE_ACTIVE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxMainWindowControl::deactivate()
{
	if (theState == JVX_STATE_ACTIVE)
	{
		std::map<std::string, subWidgetAndAction>::iterator elmw = registeredSubwidgets.begin();
		for (; elmw != registeredSubwidgets.end(); elmw++)
		{
			elmw->second.widget->deactivate();
		}
		theState = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxMainWindowControl::update_window(jvxCBitField prio)
{
	std::map<std::string, subWidgetAndAction>::iterator elmw = registeredSubwidgets.begin();
	for (; elmw != registeredSubwidgets.end(); elmw++)
	{
		jvxBool isVis = false;
		for (std::list<QDialog*>::iterator elmi = elmw->second.fQWidgets.begin(); elmi != elmw->second.fQWidgets.end(); elmi++)
		{
			if ((*elmi)->isVisible())
			{
				isVis = true;
				break;
			}
		}

		elmw->second.action->setChecked(isVis);
		elmw->second.widget->update_window(prio);
	}

	std::map<jvxSize, QAction*>::iterator elmA = myRegisteredActions.begin();
	for (; elmA != myRegisteredActions.end(); elmA++)
	{
		updateSingleAction(elmA->second);
	}

	if (menuApplication)
	{
		if (menuApplication->actions().count() == 0)
			menuApplication->setEnabled(false);
		else
			menuApplication->setEnabled(true);
	}

	if (menuSystem)
	{
		if (menuSystem->actions().count() == 0)
			menuSystem->setEnabled(false);
		else
			menuSystem->setEnabled(true);
	}
	

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMainWindowControl::update_window_periodic()
{
	std::map<std::string, subWidgetAndAction>::iterator elmw = registeredSubwidgets.begin();
	for (; elmw != registeredSubwidgets.end(); elmw++)
	{
		elmw->second.widget->update_window_periodic();
	}
	return JVX_NO_ERROR;
}

void
CjvxMainWindowControl::action_triggered()
{
	QObject* send = QObject::sender();
	if (send)
	{
		QString nm = send->objectName();
		std::map<std::string, subWidgetAndAction>::iterator elm = registeredSubwidgets.find(nm.toLatin1().data());
		if (elm != registeredSubwidgets.end())
		{
			jvxBool isVis = false;
			for (std::list<QDialog*>::iterator elmi = elm->second.fQWidgets.begin(); elmi != elm->second.fQWidgets.end(); elmi++)
			{
				if ((*elmi)->isVisible())
				{
					isVis = true;
					break;
				}
			}
			if (isVis)
			{
				for (std::list<QDialog*>::iterator elmi = elm->second.fQWidgets.begin(); elmi != elm->second.fQWidgets.end(); elmi++)
				{
					(*elmi)->hide();
				}
			}
			else
			{
				for (std::list<QDialog*>::iterator elmi = elm->second.fQWidgets.begin(); elmi != elm->second.fQWidgets.end(); elmi++)
				{
					(*elmi)->show();
				}
			}
		}
	}
}
