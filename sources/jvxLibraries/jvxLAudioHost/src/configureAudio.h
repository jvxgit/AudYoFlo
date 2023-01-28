#ifndef __CONFIGUREAUDIO_H__
#define __CONFIGUREAUDIO_H__

#include "jvx.h"

//#ifdef JVX_OS_MACOSX
//#include "ui_configureAudio_mac.h"
//#else
#include "ui_configureAudio.h"
//#endif

#include "additionalArgs.h"
#include "jvx-helpers.h"
#include <QtCore/QTimer>

#include "jvxQtComponentWidgets.h"
#include "templates/jvxProperties/CjvxAccessProperties_direct.h"

class uMainWindow;


// In case the object that is a delegate needs to be allocate by the caller of the delegate functions,
// we need to allow creation functions of the delegate super class
#ifdef JVX_AUDIO_HOST_WITH_NETWORK_ACCESS
extern IjvxQtComponentWidget* IjvxQtComponentWidget_init_device();
extern void IjvxQtComponentWidget_terminate_device(IjvxQtComponentWidget*);
#endif

// ==============================================================
typedef enum
{
	JVX_UI_SHOW_MODE_GENW_SHIFT = 0,
	JVX_UI_SHOW_MODE_GENW_ANY_SHIFT = 1,
	JVX_UI_SHOW_MODE_ALLOTHER_SHIFT = 2
} jvxUiShowMode;

class configureAudio: public QDialog, public Ui::ConfigureAudio
{
	friend class additionalArgsWidget;

	struct props_onechan
	{
		jvxSize mapped_to = JVX_SIZE_UNSELECTED;
		jvxData gain = 1.0;
		jvxBool direct = false;
		jvxBool valid = false;
	};

private:
	struct
	{
		std::vector<jvxSize> id_technology;
		std::vector<std::vector<jvxSize> > id_device;
		jvxSize id_inchan;
		jvxSize id_outchan;

		/*
		struct
		{
			int width;
			int height;
		} sizeSmall;

		struct
		{
			int width;
			int height;
		} inUse;*/
		std::vector<props_onechan> in_channel_mapper;
		std::vector<props_onechan> out_channel_mapper;

		jvxBool in_all;
		jvxBool out_all;
	}runtime;

	/*
	struct
	{
		jvx_componentIdPropsCombo theTechProps;
		jvx_componentIdPropsCombo theDevProps;
	} properties;
	*/

	struct
	{
		std::string artist;
		std::string title;
	} localSettings;

	struct
	{
		jvxSize period_ms;
		jvxSize min_width;
		jvxSize min_height;
	} config;

	jvxBitField featureClass_tech;
	jvxBitField featureClass_dev;

	IjvxAccessProperties* currentPropsDev;
	IjvxAccessProperties* currentPropsTech;

	QTimer* timerViewUpdate;
	int progress;
	jvxTimeStampData tStampRef;

	jvxComponentIdentification* tpAll;

	//jvxBool properties_started;

	//jvxBool tecDevIsGenWrapperFirst;
	//jvxBool tecDevIsGenWrapper;
	jvxCBitField modeShow;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;


public:
	configureAudio(uMainWindow* thePa, jvxComponentIdentification* tpAll);

	~configureAudio();

	jvxErrorType init();

	//jvxErrorType startProperties();
	//jvxErrorType stopProperties();
	jvxErrorType refreshPropertyIds();
	jvxErrorType refreshPropertyIds_run();

	void start();

	void startProcessing(jvxInt32 period_ms);
	void stopProcessing();

	void inform_internals_have_changed(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, 
		const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose purpose);
	
	void inform_component_select(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps);
	void inform_component_unselect(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps);

	void inform_component_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps);
	void inform_component_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps);

	void updateWindow();

	void updateVariables_whileRunning();

	virtual void closeEvent ( QCloseEvent * event );

	void updateVariablesSequencerNotification();
	void acceptSlotConfiguration();

	void reactivateComponent(const jvxComponentIdentification& cpTp);

protected:

	uMainWindow* myParent;

private:

	void setAllEnabled(bool val);
	void checkMode();
	void deactivate_slot(const jvxComponentIdentification& tpId, const jvxComponentIdentification& tpIdS);
	void activate_slot(const jvxComponentIdentification& tpId, jvxComponentIdentification& tpIdS);

public:
	Q_OBJECT

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

public slots:
	void select_technology(int);
	void add_infile();
	void rem_infile();
	void add_outfile();
	void rem_outfile();
 	void select_device(int);
  	void up_inchan();
 	void down_inchan();
 	void up_outchan();
  	void down_outchan();
	void select_format(int);
 	void selection_infiles(QTreeWidgetItem*,int);
 	void selection_outfiles(QTreeWidgetItem*,int);
	void selection_inchans(QTreeWidgetItem*,int);
 	void selection_outchans(QTreeWidgetItem*,int);
	void edit_samplerate();
    void edit_buffersize();
	void new_input_gain();
	void toggle_inall();
	void toggle_outall();
	void new_output_gain();
	void toggle_direct();
	void toggle_autooff(bool);

	void add_dummy_in();
	void add_dummy_out();
	void rem_dummy_in();
	void rem_dummy_out();

	void slider_in_reset();
	void slider_out_reset();

	void technology_use_button();
	void device_use_button();

	void timerExpired();

	void new_name_outwav_artist();
	void new_name_outwav_title();
	void button_outwav_add();
	void button_outwav_apply();

	void pushed_inallon();
	void pushed_inalloff();
	void pushed_outallon();
	void pushed_outalloff();

	void new_dev_slot(int);
	void new_tech_slot(int id);

	void select_rate(int);
	void select_bsize(int);

	void toggled_fillfromlast_out(bool tog);
	void toggled_fillfromlast_in(bool tog);
};

#endif
