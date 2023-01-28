#ifndef _ADDITIONAL_ARGS_H__
#define _ADDITIONAL_ARGS_H__

#include "jvx.h"

//#ifdef JVX_OS_MACOSX
//#include "ui_audioAdditionalArgs_mac.h"
//#else
#include "ui_audioAdditionalArgs.h"
//#endif

class configureAudio;
class uMainWindow;

//#ifdef JVX_OS_MACOSX
//class additionalArgsWidget: public QWidget, public Ui::audioAdditionalArgs_mac
//#else
class additionalArgsWidget: public QDialog, public Ui::audioAdditionalArgs
			    //#endif
{
	Q_OBJECT
protected:

	//configureAudio* myParent;
	uMainWindow* myParent;
	jvxComponentIdentification* tpAll;

	struct
	{
		jvxSize min_width;
		jvxSize min_height;
	} config;

public:
	additionalArgsWidget(uMainWindow* parent, jvxComponentIdentification* tpAll);
	void init();
	void refreshPropertyIds(jvxBool tecDevIsGenWrapper);
	QSize sizeHint();

	void refreshPropertyIds_run(jvxBool tecDevIsGenWrapper);
	void updateWindow();
	void updateWindow_periodic();
	virtual void closeEvent(QCloseEvent * event);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

public slots:
	void new_input_lookahead(int);
    void new_input_boost();
	void new_input_loop();
	void new_pause_on_start();
	void new_inc_output_channels();
	void new_dec_output_channels();
	void new_output_channels_text();
	void new_output_srate();
	void new_output_boost();
	void new_format_hw(int);
	void new_check_autohw();
	void open_cpanel();
	void new_output_lookahead(int);
	void new_quality_resampler_in(int);
	void new_quality_resampler_out(int);
	void new_samplerate_hw(int);
	void new_samplerate_hw();
	void new_buffersize_hw(int);
	void new_buffersize_hw();

	void trigger_rewind();
	void trigger_play();
	void trigger_pause();
	void trigger_forward();
	void trigger_restart();

	void new_mode_output(int sel);
	void new_period_silence_stop();
	void clicked_silence_stop_active(bool);

    void new_alsa_check_all_channelsi(bool);
    void new_alsa_check_all_channelso(bool);
    void new_alsa_access_tp(int);
	void new_alsa_data_tp(int);
	void new_alsa_period_i();
	void new_alsa_period_o();

	void new_select_auto_stop();
};

#endif
