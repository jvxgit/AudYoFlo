#ifndef JVX_MENU_HOST_BRIDGE_COMPONENTS_H__
#define JVX_MENU_HOST_BRIDGE_COMPONENTS_H__

#include "jvx.h"
#include <QtWidgets/QMenu>
#include "jvxQtHostWidgets.h"

class jvx_menu_host_bridge_components : public QObject, public IjvxQtMenuComponentsBridge
{
	Q_OBJECT

	struct oneSetMenuItems
	{
		QMenu* addme_pri_slots = nullptr;
		QMenu* addme_pri = nullptr;
		QMenu* addme_pri_avail = nullptr;
		QMenu* addme_pri_stat = nullptr;
		QMenu* addme_pri_props = nullptr;
		QMenu* addme_pri_commands = nullptr;
		QMenu* addme_pri_conditions = nullptr;
		QMenu* addme_pri_dev_caps = nullptr;
		QAction* addme_pri_information = nullptr;
		jvxComponentType tp_pri = JVX_COMPONENT_UNKNOWN;

		QMenu* addme_sec_sub_slots = nullptr;
		QMenu* addme_sec = nullptr;
		QMenu* addme_sec_avail = nullptr;
		QMenu* addme_sec_stat = nullptr;
		QMenu* addme_sec_props = nullptr;
		QMenu* addme_sec_commands = nullptr;
		QMenu* addme_sec_conditions = nullptr;
		QAction* addme_sec_information = nullptr;
		jvxComponentType tp_sec = JVX_COMPONENT_UNKNOWN;

		oneSetMenuItems()
		{
			reset();
		};

		void reset()
		{
			addme_pri_slots = nullptr;
			addme_pri = nullptr;
			addme_pri_avail = nullptr;
			addme_pri_stat = nullptr;
			addme_pri_props = nullptr;
			addme_pri_commands = nullptr;
			addme_pri_conditions = nullptr;
			addme_pri_information = nullptr;
			addme_pri_dev_caps = nullptr;
			tp_pri = JVX_COMPONENT_UNKNOWN;

			addme_sec_sub_slots = nullptr;
			addme_sec = nullptr;
			addme_sec_avail = nullptr;
			addme_sec_stat = nullptr;
			addme_sec_props = nullptr;
			addme_sec_commands = nullptr;
			addme_sec_conditions = nullptr;
			addme_sec_information = nullptr;
			tp_sec = JVX_COMPONENT_UNKNOWN;

		};
	};

	std::map<jvxComponentType, oneSetMenuItems> theMenuItems;

	configureHost_features* host_features_ref;
	IjvxHost* hHost_ref;
	IjvxToolsHost* hTools_ref;
	QMenu* menuConfiguration_ref;
	jvxComponentIdentification* tpAll_ref;
	jvxComponentIdentification tpAllNext[JVX_COMPONENT_ALL_LIMIT];
	jvx_menu_host_bridge_components_report* report;

	jvxBool* skipStateSelected_ref;
	QMenu* menuOther_Components;

	QWidget* parentWid;

public:

	jvx_menu_host_bridge_components(QWidget* parent);
	~jvx_menu_host_bridge_components();

	virtual void createMenues(configureHost_features* host_features_arg,
		IjvxHost* hHost_arg,
		IjvxToolsHost* hTools_arg,
		QMenu* menuConfiguration_arg,
		jvxComponentIdentification* tpAll_arg,
		jvxBool* skipStateSelected_arg,
		jvx_menu_host_bridge_components_report* report_arg) override;

	virtual void removeMenues() override;

	virtual void updateWindow() override;
	virtual void updateWindow_tools() override;
	virtual void updateWindowSingle(jvxComponentIdentification tp) override;

private:

	void menuUpdate_oneComponentType(QMenu* lstNames, QMenu* lstStates, QMenu* lstProperties, QMenu* lstCommands,
			QMenu* lstConditions, QMenu* lstDeviceCaps, QAction* lstInformation, QMenu* lstSlots,
			const jvxComponentIdentification& tp, jvxBool isPriComp);
	void menuUpdate_tools(QMenu* lstNames);

	void updateWindow_core_single(oneSetMenuItems& item);
	
public slots:
	void action_int_selection_slot();
};

#endif
