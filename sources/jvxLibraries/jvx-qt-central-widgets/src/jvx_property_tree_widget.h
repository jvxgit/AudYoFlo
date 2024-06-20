#ifndef __JVX_PROPERTY_TREE_WIDGET_H__
#define __JVX_PROPERTY_TREE_WIDGET_H__

#include "ui_jvx_property_tree_widget.h"
#include "jvx.h"
#include "jvxQtCentralWidgets.h"
#include "CjvxQtSaWidgetWrapper.h" 

class jvx_property_tree_widget : public QWidget, 
	public IjvxQtPropertyTreeWidget,
	public IjvxQtSaWidgetWrapper_report,
	public Ui::jvx_property_tree
{
private:

	Q_OBJECT

	class oneProcess
	{
	public:
		jvxSize uid;
		std::string description;
		std::list<jvxComponentIdentification> involved;
	};

	class oneEntryProp
	{
	public:
		IjvxAccessProperties* propRef;
		std::string ident;
		std::string prefix;
		jvxComponentIdentification cpId;
		oneEntryProp()
		{
			clear();
		};
		void clear()
		{
			propRef = NULL;
		};
	};

	std::map< IjvxAccessProperties*, oneEntryProp> mapAllPropsStore;
	std::map< std::string, oneEntryProp> mapAllPropsShow;

	std::map<jvxSize, oneProcess> mapAllProcesses;

	IjvxAccessProperties* propRefSelect;
	//std::string token1Copy;
	//std::string token2Copy;

	std::string txtCol0;
	std::string txtCol1;

	CjvxQtSaWidgetWrapper* widget_wrapper;
	IjvxQtSaWidgetWrapper_report* widget_wrapper_report;
	std::string tag_name;

	jvxBool always_activate_latest = false;
	jvxSize last_revision = JVX_SIZE_UNSELECTED;
	oneEntryProp latestSelection;

	std::string token_search;

	IjvxHost* theHostRef = nullptr;
	jvxSize selProcess = JVX_SIZE_DONTCARE;

public:

	jvx_property_tree_widget(QWidget* parent);
	~jvx_property_tree_widget();
	
	// ==========================================================

	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL) override;
	virtual void getMyQWidget(QWidget** retWidget, jvxSize id = 0) override;
	virtual void terminate()override;
	virtual void activate() override;
	virtual void deactivate()override;
	virtual void processing_started() override;
	virtual void processing_stopped()override;
	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT, const char* propLst = nullptr) override;
	virtual void update_window_periodic() override;

	virtual jvxErrorType request_sub_interface(jvxQtInterfaceType, jvxHandle**) override;
	virtual jvxErrorType return_sub_interface(jvxQtInterfaceType, jvxHandle*) override;

	// ==========================================================

	virtual jvxErrorType addPropertyReference(IjvxAccessProperties* propRef, const std::string& prefixArg, const std::string& identArg) override;
	virtual jvxErrorType removePropertyReference(IjvxAccessProperties* propRef) override;

	// ==========================================================

	// virtual void set_tokens_widget(const char* token1, const char* token2) override;

	// ==========================================================

	virtual jvxErrorType reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res_in_call) override;
	virtual jvxErrorType reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res_in_call) override;
	virtual jvxErrorType reportAllPropertiesAssociateComplete(const char* tag) override;
	virtual jvxErrorType reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle* props) override;

	virtual void fwd_command_request(const CjvxReportCommandRequest& req) override;
	void reconstructShowRefs();

private:
	void select_new_prop_ref(const oneEntryProp& prop);
	void unselect_prop_ref();

	void restore_column_expressions_neutral(jvxBool storeExpr);
	void setup_column_expressions_active(const std::string& expr);

	void update_window_core(jvxCBitField prio);

	void fillProcessRecursively(oneProcess& theProc, IjvxConnectionIterator* it);
public slots:
	void changed_selection_propref(int sel);
	void changed_selection_process(int selProp);
	void changed_selection_hidden(bool isHidden);
	void changed_selection_shorten(bool showShort);
	void changed_selection_show(bool showDescriptor);
	void changed_activate_latest(bool sel);
	void new_token_search(QString);
};

#endif