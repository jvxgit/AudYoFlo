#ifndef __JVX_CONNECTIONS_WIDGET_H__
#define __JVX_CONNECTIONS_WIDGET_H__

#include "ui_jvx_connections.h"
#include "jvx.h"
#include "jvxQtHostWidgets.h"

typedef enum
{
	JVX_CONNECTION_WIDGET_IS_A_PROCESS = 0,
	JVX_CONNECTION_WIDGET_IS_A_GROUP = 1
} jvxConnectionWidgetProcessGroupType; 

class jvx_connections_widget: public QWidget, public IjvxQtConnectionWidget, public Ui::ConnectionsForm, public CjvxConnectionDropzone
{
private:
	IjvxHost* theHostRef;
	IjvxQtSpecificWidget_report* theBwdRef;
	IjvxDataConnections* theDataConnections;
	
	struct
	{
		jvxSize id_proc_grp;
		jvxConnectionWidgetProcessGroupType proc_grp_tp;
	} id_select_process_group;

	struct
	{
		jvxSize uid_select_masfac;
		jvxSize id_select_mas;
	} id_selected_master;

	struct
	{
		jvxSize uid_select_confac;
		jvxSize id_select_icon;
		jvxSize id_select_ocon;
	} id_selected_connector;
		
	// jvxConnectionWidgetProcessGroupType proc_grp_tp;
	jvxBool bridge_ded_thread;
	jvxBool bridge_boost_thread;
	jvxBool bridge_entry_chain;
	jvxBool bridge_exit_chain;

	jvxBool process_ic;
	jvxBool process_ess;

	jvxBool rule_ic;
	jvxBool rule_ess;

	jvxBool bridge_show_only_mode;

	jvxBool verbose_mode = false; 

	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS(latest_status_process);
	// ===========================================

	struct
	{
		jvxSize id_rule;
	} id_select_rule;

	struct
	{
		jvxSize id_bridgename;
	} id_select_rule_bridgename;

	jvxConnectionRuleDropzone theConnectionRuleDropzone;

	virtual void closeEvent(QCloseEvent * event)override;

	std::string edit_bridge_name;
	std::string edit_from_fac_wc;
	std::string edit_from_wc;
	jvxComponentIdentification edit_cp_from;
	std::string edit_to_fac_wc;
	std::string edit_to_wc;
	jvxComponentIdentification edit_cp_to;

	struct
	{
		jvxSize min_width;
		jvxSize min_height;
	} config;

	jvxBool inBridgeAlreadyDefined;
	jvxBool outBridgeAlreadyDefined;

	jvxBool allow_select_connector;

	Q_OBJECT
	
protected:

public:

	jvx_connections_widget(QWidget* parent);
	~jvx_connections_widget();

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

	// ==========================================================

	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL) override;
	virtual void getMyQWidget(QWidget** retWidget, jvxSize id = 0) override;
	virtual void terminate()override;
	virtual void activate() override;
	virtual void deactivate()override;
	virtual void processing_started() override;
	virtual void processing_stopped()override;
	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT) override;
	virtual void update_window_periodic() override;

	virtual jvxErrorType check_connections_ready(jvxApiString* astr_ready) override;
	// ==========================================================

	void update_window_core(jvxBitField  oper);
	void ui_redraw_process_list();
	void ui_get_reference_process(IjvxDataConnectionProcess** theProcess_select, IjvxDataConnectionGroup** theGroup_select, jvxSize* select_id_proc);
	void ui_refine_process_list(jvxSize);
	void ui_redraw_factories(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select);
	void ui_redraw_dropzone(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select);
	void ui_refine_dropzone(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select);
	void ui_highlight_dropzone(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select);
	void ui_update_work_buttons(IjvxDataConnectionProcess* theProcess_select, IjvxDataConnectionGroup* theGroup_select);
	
	void ui_redraw_connection_rules();
	void ui_show_connection_rule();
	void reset_rule_dropzone();
	void duplicate_content_bridge(jvxSize id);
	void show_status_test_chain(jvxSize id);

	std::string connectorToExpression(IjvxCommonConnector* con);

public slots:

	void process_item_clicked(QTreeWidgetItem* it, int idx);
	void process_disconnect(); 	
	void process_remove();	
	void dz_process_name_edited();	
	void dz_process_descr_edited();
	void dz_master_clicked(QListWidgetItem*);	
	void dz_icon_clicked(QListWidgetItem*);	
	void dz_ocon_clicked(QListWidgetItem*);	
	void dz_bridge_clicked(QListWidgetItem*);	
	void dz_create_bridge();
	void dz_remove_bridge();
	void dz_create_proc();
	void dz_connect_proc();
	void dz_clear();
	void add_master_dz();
	void add_connector_dz();
	void master_factory_item_clicked(QTreeWidgetItem*, int);
	void connector_factory_item_clicked(QTreeWidgetItem*, int);
	void dz_clear_master();
	void dz_clear_icon();
	void dz_clear_ocon();
	void dz_clear_bridges();
	void dz_switch_process_group(int);
	void dz_bridge_ded_thread();
	void dz_bridge_boost_thread();
	void dz_bridge_entry();
	void dz_bridge_exit();
	void add_masters_dz();
	void add_connectors_dz();

	void select_connection_rule(int id);
	void select_connection_rule_bridge(int id);
	void remove_connection_rule();
	void rdz_select_master_type(int);
	void rdz_select_from_bridge_type(int);
	void rdz_select_to_bridge_type(int);
	void rdz_edit_master_fac_wildcard();
	void rdz_edit_master_wildcard();
	void rdz_edit_master_sid();
	void rdz_edit_master_ssid();
	void rdz_edit_from_fac_wildcard();
	void rdz_edit_from_wildcard();
	void rdz_edit_from_sid();
	void rdz_edit_from_ssid();
	void rdz_edit_to_fac_wildcard();
	void rdz_edit_to_wildcard();
	void rdz_edit_to_sid();
	void rdz_edit_to_ssid();
	void rdz_edit_rulebridgename();
	void rdz_edit_rulename();
	void rdz_add_bridgerule();
	void rdz_add_rule();
	void rdz_rem_bridgerule();
	void rdz_copy();
	void rdz_clear();

	void dz_clicked_interceptors(bool);
	void rdz_clicked_interceptors(bool);

	void dz_clicked_essential(bool);
	void rdz_clicked_essentials(bool);

	void modal_showresult();

	void local_close();
};

#endif // ifndef __JVX_CONNECTIONS_WIDGET_H__
