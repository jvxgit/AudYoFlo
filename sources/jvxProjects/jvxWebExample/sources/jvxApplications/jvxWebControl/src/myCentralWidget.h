#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myCentralWidget.h"

#include "mainCentral_hostfactory_ww_http.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

// The following helps to identify transfer problems in the
// ereignis websocket transfer: The embedded target may be 
// setup to produce a linear sequence of values of which the right order is checked here
// #define JVX_VERIFY_EREIGNIS_TRANSFER

#define JVX_NUMBER_INT32_ASCANS 4096
#define JVX_NUMBER_INT32_RAWRAM 1024

class myCentralWidget: public mainCentral_hostfactory_ww_http, public Ui::Form_myMainWidget,
	public IjvxConfigurationExtender_report, IjvxMainWindowController_report
{
private:

	Q_OBJECT

	IjvxAccessProperties* propRef_customdvice;
	
	//jvxSize localProcid_ascan;
	jvxSize localProcid_al;

	jvxBool uiReady;
	jvxBool webSocketsReady;

	IjvxConfigurationExtender* cfgExtender;

	QTimer* theAliveTimer;

	jvxUInt32 ascanBuffer32[JVX_NUMBER_INT32_ASCANS];
	QVector<double> ascan_bufx;
	QVector<double> ascan_bufy;

	struct
	{
		jvxData minx;
		jvxData maxx;
		jvxBool autox;
		jvxData miny;
		jvxData maxy;
		jvxBool autoy;
		jvxBool autoupdate;
		jvxBool log10;
	} plotOptionsAscan;

	jvxUInt32 rawramBuffer32[JVX_NUMBER_INT32_RAWRAM];
	QVector<double> rawram_bufx;
	QVector<double> rawram_bufy;

	struct
	{
		jvxData minx;
		jvxData maxx;
		jvxBool autox;
		jvxData miny;
		jvxData maxy;
		jvxBool autoy;
		jvxBool autoupdate;
		jvxBool log10;
	} plotOptionsRawram;
	
public:

	myCentralWidget(QWidget* parent);
	~myCentralWidget();

	virtual void init_submodule(IjvxFactoryHost* theHost) override;
	virtual void terminate_submodule() override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps) override;

	virtual void inform_sequencer_about_to_start() override;

	virtual void inform_sequencer_about_to_stop() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename, jvxInt32 lineno) override;

	virtual bool is_ready() override;

	virtual void inform_update_window(jvxCBitField prio)override;
	virtual void inform_update_window_periodic()override;

	virtual void process_websocket_binary(jvxSize userData, jvxByte* payload, jvxPropertyTransferPriority prio) override;

#ifdef JVX_ADD_WIDGET_WRAPPER
	virtual jvxErrorType reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res) override;
	virtual jvxErrorType reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res) override;
	virtual jvxErrorType reportAllPropertiesAssociateComplete(const char* tag) override;
#endif

	void core_init_alive_retrigger();
	//void core_init_ascan_trigger();

	void replot_ascans();
	void recompute_ascans();
	void reset_ascan_buffers();

	void replot_rawram();
	void recompute_rawram();
	void reset_rawram_buffers();

	virtual jvxErrorType report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr)override;
	virtual jvxErrorType report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION set_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id) override;

signals:


public slots:
	void alive_timer_expired();

	//void trigger_ascans_update();
	void changed_miny_ascan();
	void toggled_autoy_ascan(bool tog);
	void toggled_autox_ascan(bool tog);
	void toggled_log10_ascan(bool tog);
	void changed_minx_ascan();
	void changed_maxy_ascan();
	void changed_maxx_ascan();

	void changed_miny_rawram();
	void toggled_autoy_rawram(bool tog);
	void toggled_autox_rawram(bool tog);
	void toggled_log10_rawram(bool tog);
	void changed_minx_rawram();
	void changed_maxy_rawram();
	void changed_maxx_rawram();
 };

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
