#include "jvx_property_tree_widget.h"

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtPropertyTreeWidget, jvxQtPropertyTreeWidget, jvx_property_tree_widget);
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtPropertyTreeWidget, jvxQtPropertyTreeWidget);

// #define JVX_WIDGET_TAG_NAME "element_tag"

jvx_property_tree_widget::jvx_property_tree_widget(QWidget* parent)
{
	propRefSelect = NULL;

	widget_wrapper = NULL;;
	widget_wrapper_report = NULL;

}

jvx_property_tree_widget::~jvx_property_tree_widget()
{
}

void 
jvx_property_tree_widget::init(IjvxHost* theHost, jvxCBitField mode, jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd)
{
	if (specPtr)
	{
		IjvxQtPropertyTreeWidget_connect* connectHdl = (IjvxQtPropertyTreeWidget_connect*)specPtr;
		tag_name = connectHdl->tag_name_ptr;
		widget_wrapper = (CjvxQtSaWidgetWrapper*)connectHdl->widget_wrapper_ptr;
		widget_wrapper_report = (IjvxQtSaWidgetWrapper_report*)connectHdl->report_ptr;
	}
	this->setupUi(this);

	if (jvx_bitTest(mode, JVX_LOCAL_PROPERTY_REPORT_SUPERSEDE_SHIFT))
	{
		std::string txt = treeWidget_props->accessibleDescription().toLatin1().data();
		txt += "::REPORT=JVX_WW_REPORT_LOCAL";
		treeWidget_props->setAccessibleDescription(txt.c_str());
	}
	restore_column_expressions_neutral(true);
}

void 
jvx_property_tree_widget::getMyQWidget(QWidget** retWidget, jvxSize id)
{
	switch(id)
	{
	case 0:
		*retWidget = static_cast<QWidget*>(this);
		break;
	}
}

jvxErrorType
jvx_property_tree_widget::request_sub_interface(jvxQtInterfaceType, jvxHandle**)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
jvx_property_tree_widget::return_sub_interface(jvxQtInterfaceType, jvxHandle*)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
jvx_property_tree_widget::addPropertyReference(IjvxAccessProperties* propRef, const std::string& prefixArg, const std::string& identArg)
{
	auto elm = mapAllProps.find(propRef);
	if (elm == mapAllProps.end())
	{
		oneEntryProp newElm;
		
		newElm.ident = identArg;
		newElm.prefix = prefixArg;
		newElm.propRef = propRef;
		propRef->get_reference_component_description(nullptr, nullptr, nullptr, &newElm.cpId);

		mapAllProps[propRef] = newElm;

		jvxCBitField prio = 0;
		jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		update_window_core(prio);

		if (propRefSelect && always_activate_latest)
		{
			unselect_prop_ref();
		}
		if ((propRefSelect == NULL)|| always_activate_latest)
		{
			select_new_prop_ref(newElm);
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}



jvxErrorType
jvx_property_tree_widget::removePropertyReference(IjvxAccessProperties* propRef)
{
	auto elm = mapAllProps.find(propRef);
	if (elm != mapAllProps.end())
	{
		if (propRefSelect == propRef)
		{
			unselect_prop_ref();
		}
		mapAllProps.erase(elm);

		jvxCBitField prio = 0;
		jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		update_window_core(prio);

		if (propRefSelect == NULL)
		{
			if (mapAllProps.size())
			{
				elm = mapAllProps.begin();
				select_new_prop_ref(elm->second);
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

void 
jvx_property_tree_widget::terminate()
{
}

void 
jvx_property_tree_widget::activate()
{

}
	
void 
jvx_property_tree_widget::deactivate()
{
}
	
void 
jvx_property_tree_widget::processing_started()
{

}
	
void 
jvx_property_tree_widget::processing_stopped()
{

}
	
void 
jvx_property_tree_widget::update_window(jvxCBitField prio, const char* propLst)
{
	if(propRefSelect)
	{
		jvxSize rev = JVX_SIZE_UNSELECTED;
		jvxCallManagerProperties callGate;
		propRefSelect->get_revision(&rev, callGate);
		if (rev != last_revision)
		{
			if (latestSelection.propRef)
			{
				oneEntryProp pp = latestSelection;
				unselect_prop_ref();
				select_new_prop_ref(pp);
			}
			last_revision = rev;
		}
		update_window_core(prio);				
	}
}
	
void 
jvx_property_tree_widget::update_window_periodic()
{
	if (widget_wrapper)
	{
		widget_wrapper->trigger_updateWindow_periodic(tag_name);
	}
}

// ==========================================================
/*
void 
jvx_property_tree_widget::set_tokens_widget(const char* token1, const char* token2)
{
}
*/
// ==========================================================
// ==========================================================

jvxErrorType 
jvx_property_tree_widget::reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res_in_call)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
jvx_property_tree_widget::reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res_in_call) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
jvx_property_tree_widget::reportAllPropertiesAssociateComplete(const char* tag)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
jvx_property_tree_widget::reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle* props) 
{
	return JVX_NO_ERROR;
}


// ==========================================================
// ==========================================================

void
jvx_property_tree_widget::update_window_core(jvxCBitField prio)
{	
	jvxSize i;

	std::map< IjvxAccessProperties*, oneEntryProp>::iterator elm;
	jvxSize cnt = 0;
	jvxSize selCnt = JVX_SIZE_UNSELECTED;
	std::string txt = "--";

	if (jvx_bitTest(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT))
	{
		comboBox_component_ref->clear();

		elm = mapAllProps.begin();
		for (; elm != mapAllProps.end(); elm++)
		{
			comboBox_component_ref->addItem(elm->second.ident.c_str());
		}
	}

	elm = mapAllProps.begin();
	for (; elm != mapAllProps.end(); elm++)
	{
		if (elm->second.propRef == propRefSelect)
		{
			jvxApiString astr;
			selCnt = cnt;
			propRefSelect->description_object(&astr);
			txt = astr.std_str();
		}
		cnt++;
	}
	if (JVX_CHECK_SIZE_SELECTED(selCnt))
	{
		comboBox_component_ref->setCurrentIndex(selCnt);
	}
	label_component_id->setText(txt.c_str());
	
	checkBox_actlatest->setChecked(always_activate_latest);

	if(widget_wrapper)
	{
		treeWidget_props->setProperty("search-tree", QVariant(token_search.c_str()));
		widget_wrapper->trigger_updateWindow(tag_name);
	}

	cnt = treeWidget_props->columnCount();
	for (i = 0; i < cnt; i++)
	{
		treeWidget_props->resizeColumnToContents(i);
	}
}

void
jvx_property_tree_widget::select_new_prop_ref(const oneEntryProp& prop)
{
	jvxCBitField prio = 0;
	propRefSelect = prop.propRef;
	setup_column_expressions_active(prop.ident);
	if (widget_wrapper)
	{
		jvxErrorType res = widget_wrapper->associateAutoWidgets(treeWidget_props, propRefSelect,
			widget_wrapper_report,
			tag_name, true, JVX_WW_WIDGET_SEARCH_CURRENT_ONLY,
			prop.prefix);
		if (res == JVX_NO_ERROR)
		{
			latestSelection = prop;
		}
		else
		{
			std::cout << "Failed to associate property tag <" << tag_name << "> with property tree widget." << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to associate property tag <" << tag_name << "> with property tree widget, reason: no wodget wrapper reference specified." << std::endl;
	}
	jvx_bitSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
	update_window_core(prio);
}

void
jvx_property_tree_widget::unselect_prop_ref()
{
	if (widget_wrapper)
	{
		widget_wrapper->deassociateAutoWidgets(tag_name);
		latestSelection.clear();
	}
	restore_column_expressions_neutral(false);
	propRefSelect = NULL;
	update_window();
}

void
jvx_property_tree_widget::restore_column_expressions_neutral(jvxBool storeExpr)
{
	QTreeWidgetItem* it = treeWidget_props->topLevelItem(0);
	if (it)
	{
		if (storeExpr)
		{
			txtCol0 = it->text(0).toLatin1().data();
			txtCol1 = it->text(1).toLatin1().data();
		}

		it->setText(0, "- NONE SELECTED -");
		it->setText(1, "");
	}
}

void
jvx_property_tree_widget::setup_column_expressions_active(const std::string& expr)
{
	QTreeWidgetItem* it = treeWidget_props->topLevelItem(0);
	if (it)
	{
		std::string txt = txtCol0;
		txt = jvx_replaceStrInStr(txt, "PLACEHOLDER_PROP_NAME", expr);
		it->setText(0, txt.c_str());
		txt = txtCol1;
		txt = jvx_replaceStrInStr(txt, "PLACEHOLDER_PROP_TAG", tag_name);
		it->setText(1, txt.c_str());
	}
}

void 
jvx_property_tree_widget::changed_selection_propref(int sel)
{
	if (sel < mapAllProps.size())
	{
		auto elm = mapAllProps.begin();
		std::advance(elm, sel);
		if (propRefSelect != elm->second.propRef)
		{
			unselect_prop_ref();

			select_new_prop_ref(elm->second);
		}
	}
}

void 
jvx_property_tree_widget::changed_activate_latest(bool sel)
{
	always_activate_latest = checkBox_actlatest->isChecked();
	update_window_core((jvxCBitField)0);
}

void 
jvx_property_tree_widget::new_token_search(QString txt)
{
	token_search = txt.toLatin1().data();
	treeWidget_props->setProperty("search-tree", QVariant(token_search.c_str()));
	widget_wrapper->trigger_updateWindow(tag_name);
}

void
jvx_property_tree_widget::changed_selection_hidden(bool isHidden)
{
	treeWidget_props->setProperty("show-hidden", QVariant(isHidden));
	widget_wrapper->trigger_updateWindow(tag_name);
}

void
jvx_property_tree_widget::changed_selection_shorten(bool showShort)
{
	treeWidget_props->setProperty("show-short", QVariant(showShort));
	widget_wrapper->trigger_updateWindow(tag_name);
}

void 
jvx_property_tree_widget::changed_selection_show(bool showDescriptor)
{
	treeWidget_props->setProperty("show-descriptor", QVariant(showDescriptor));
	widget_wrapper->trigger_updateWindow(tag_name);
}

void 
jvx_property_tree_widget::fwd_command_request(const CjvxReportCommandRequest& req)
{
	auto orig = req.origin();
	auto ptrIf = castCommandRequest<CjvxReportCommandRequest_id>(req);
	auto reqTp = req.request();
	auto dtTp = req.datatype();
	auto elm = mapAllProps.end();
	jvxApiString astr;
	switch (dtTp)
	{
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT:
		switch (reqTp)
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY:
			assert(ptrIf);
			ptrIf->ident(&astr);
			elm = mapAllProps.find(propRefSelect);
			if (elm != mapAllProps.end())
			{
				if (elm->second.cpId == orig)
				{
					if (widget_wrapper)
					{
						treeWidget_props->setProperty("search-tree", QVariant(token_search.c_str()));
						widget_wrapper->trigger_updateWindow(tag_name);
					}
					// std::cout << "Request to update property from <" << jvxComponentIdentification_txt(orig) << ">: " << astr.std_str() << std::endl;
				}
			}
			break;
		}
	}
}
