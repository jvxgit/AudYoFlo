#ifndef __UMAINWINDOW_SPECIFIC_H__
#define __UMAINWINDOW_SPECIFIC_H__

#include <QMainWindow>
#include "jvx.h"

class uMainWindow;
class configureHost_features;

class uMainWindow_specific: public QMainWindow// , public IjvxAutoDataConnect
{
public:
	uMainWindow_specific();

private:

	uMainWindow* parentRef;
	QMenu* menuAlgorithms;

	IjvxInputConnector* in_au_node;
	IjvxOutputConnector* out_au_node;
	IjvxInputConnector* in_au_device;
	IjvxOutputConnector* out_au_device;
	IjvxConnectionMaster* master_au_device;

	IjvxDataConnectionProcess* theProcess;

	jvxSize the_conn_id;
	jvxSize the_bridge_dev2node;
	jvxSize the_bridge_node2dev;
	//jvxSize the_bridge_master;

	Q_OBJECT

protected:

	jvxComponentIdentification tpAll[JVX_COMPONENT_ALL_LIMIT];

	void updateWindow_specific(jvxCBitField prio);

	void init_specific(uMainWindow* par);

	jvxErrorType connect_specific();

	jvxErrorType bootup_negotiate_specific();

	jvxErrorType postbootup_specific();

	void bootDelayed_specific();

	jvxErrorType shutdown_specific();

	jvxErrorType pre_shutdown_specific();

#ifndef JVX_CONFIGURE_HOST_STATIC_NODE
	static void myComponentFilterCallback(jvxBool* doNotLoad, const char* module_name, jvxComponentType tp, jvxHandle* priv);
#endif

private:

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION request_new_connection_factory_to_be_added(
		IjvxDataConnections* connections, IjvxConnectorFactory* toadd, IjvxConnectionMasterFactory* toaddmaster);
	virtual jvxErrorType JVX_CALLINGCONVENTION request_new_connection_factory_to_be_removed(IjvxDataConnections* connections, IjvxConnectorFactory* toremove, IjvxConnectionMasterFactory* toremovemaster);
	*/

	void display_data_connections(IjvxDataConnections* connections);
public slots:

};

#endif
