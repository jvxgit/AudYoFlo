#ifndef __MJVX_CENTRAL_WIDGET_MACROS_H__
#define __MJVX_CENTRAL_WIDGET_MACROS_H__

#define JVX_MAIN_CENTRAL_DECLARE_TREE_WIDGET(propertyTreeWidget) \
	struct \
	{ \
		IjvxQtPropertyTreeWidget* widget; \
		QWidget* qwidget; \
	} propertyTreeWidget;

#define JVX_MAIN_CENTRAL_ATTACH_TREE_WIDGET(propertyTreeWidget, tabWidget_app) \
	{ \
		jvxCBitField mode = 0; \
		IjvxQtPropertyTreeWidget_connect connectStr; \
		jvx_bitZSet(mode, JVX_LOCAL_PROPERTY_REPORT_SUPERSEDE_SHIFT); \
		connectStr.widget_wrapper_ptr = &myWidgetWrapper; \
		connectStr.report_ptr = static_cast<IjvxQtSaWidgetWrapper_report*>(this); \
		connectStr.tag_name_ptr = "element_tag"; \
		init_jvxQtPropertyTreeWidget(&propertyTreeWidget.widget, this); \
		propertyTreeWidget.widget->init(theHost, mode, &connectStr, static_cast<IjvxQtSpecificWidget_report*>(this)); \
		propertyTreeWidget.widget->getMyQWidget(&propertyTreeWidget.qwidget); \
		tabWidget_app->addTab(propertyTreeWidget.qwidget, "Generic Property Viewer"); \
	}

#define JVX_MAIN_CENTRAL_DETACH_TREE_WIDGET(propertyTreeWidget, tabWidget_app) \
	{ \
		jvxSize i; \
		for (i = 0; i < tabWidget_app->count(); i++) \
		{ \
			QWidget* retW = tabWidget_app->widget(i); \
			if (retW == propertyTreeWidget.qwidget) \
			{ \
				tabWidget_app->removeTab(i); \
				break; \
			} \
		} \
		terminate_jvxQtPropertyTreeWidget(propertyTreeWidget.widget); \
		propertyTreeWidget.widget = NULL; \
		propertyTreeWidget.qwidget = NULL; \
	}

#define JVX_MAIN_CENTRAL_TREE_WIDGET_ACTIVE(propRef, propertyTreeWidget) \
	{ \
		jvxApiString astr; \
		jvxComponentIdentification tpL; \
		if(propRef) \
		{ \
			/*propRef->description_object(&astr);*/ \
			propRef->get_reference_component_description(nullptr, &astr, nullptr, &tpL); \
			astr = astr.std_str() + " - " + jvxComponentIdentification_txt(tpL); \
			propertyTreeWidget.widget->addPropertyReference(propRef, "", astr.std_str()); \
		} \
	}

#define JVX_MAIN_CENTRAL_TREE_WIDGET_INACTIVE(propRef, propertyTreeWidget) \
	if(propRef) \
	{ \
		propertyTreeWidget.widget->removePropertyReference(propRef); \
	}

#define JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW(propertyTreeWidget, prio) \
	propertyTreeWidget.widget->update_window(prio);

#define JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW_LST(propertyTreeWidget, argLst) \
	{ \
		jvxCBitField prio; \
		jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT); \
		propertyTreeWidget.widget->update_window(prio, argLst); \
	}

#define JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW_PERIODIC(propertyTreeWidget) \
	propertyTreeWidget.widget->update_window_periodic();
	
// ===================================================================================
	
#define JVX_MAIN_CENTRAL_ATTACH_HOA_DIRECTIVITY_WIDGET(openGLWidget, frame_glW) \
	openGLWidget = new CjvxMyGlWidget(this); \
	QWidget* widgetRef = NULL; \
	openGLWidget->getWidgetReferences(&widgetRef); \
	assert(widgetRef); \
	QLayout* lay = frame_glW->layout(); \
	assert(lay == NULL); \
	QHBoxLayout* layN = new QHBoxLayout; \
	layN->insertWidget(0, widgetRef); \
	frame_glW->setLayout(layN);

#define JVX_MAIN_CENTRAL_DETACH_HOA_DIRECTIVITY_WIDGET(openGLWidget) \
	delete openGLWidget; \
	openGLWidget = NULL;
	
#define JVX_MAIN_CENTRAL_ACTIVATE_HOA_DIRECTIVITY_WIDGET(openGLWidget, tab_glw, thePropRef_loc) \
	myWidgetWrapper.associateAutoWidgets(tab_glw, thePropRef_loc, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "locNode"); \
	openGLWidget->setConnectLinks(thePropRef_loc, "expose_plot_if", jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL);
	
#define JVX_MAIN_CENTRAL_DEACTIVATE_HOA_DIRECTIVITY_WIDGET(openGLWidget) \
	openGLWidget->clearConnectLinks(); \
	myWidgetWrapper.deassociateAutoWidgets("locNode");
	
#define JVX_MAIN_CENTRAL_START_HOA_DIRECTIVITY_WIDGET(res, openGLWidget) \
	res = this->openGLWidget->inform_about_to_start();
	
#define JVX_MAIN_CENTRAL_STOP_HOA_DIRECTIVITY_WIDGET(res, openGLWidget) \
	res = openGLWidget->inform_stopped();


#endif
	