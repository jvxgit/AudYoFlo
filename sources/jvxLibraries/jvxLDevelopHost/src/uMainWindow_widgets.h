#ifndef _UMAINWINDOW_WIDGETS_H__
#define _UMAINWINDOW_WIDGETS_H__

#include "uMainWindow.h"

class QDockWidget_ext: public QDockWidget
{
	Q_OBJECT
private:
	uMainWindow* parRef;
	jvxBool* refReportClose;
	Qt::DockWidgetArea iLiveInArea;

public:
        QDockWidget_ext(const char* txt, uMainWindow* par, jvxBool* refReport, Qt::DockWidgetArea area);
        ~QDockWidget_ext();

        virtual void closeEvent ( QCloseEvent * event );

        public slots:
          void newTopLevel( bool tLevel);
};

#endif
