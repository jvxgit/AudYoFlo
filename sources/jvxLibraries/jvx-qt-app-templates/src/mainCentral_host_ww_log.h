#ifndef __MAINCENTRAL_HOST_WW_LOG_H__
#define __MAINCENTRAL_HOST_WW_LOG_H__

#include "mainCentral_host_ww.h"
#include "jvxQtMaLogFileWriter.h"


class mainCentral_host_ww_log: public mainCentral_host_ww
{

protected:

	jvxQtMaLogFileWriter* theLogFileRef;

public:

	mainCentral_host_ww_log(QWidget* parent);

	void init_submodule(IjvxHost* theHost);
	void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps);
	void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps);
	void inform_update_window(jvxCBitField prio);
	void inform_update_window_periodic();
};

#endif