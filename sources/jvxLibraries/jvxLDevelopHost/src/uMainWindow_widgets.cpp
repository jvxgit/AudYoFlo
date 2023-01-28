#include "uMainWindow_widgets.h"

QDockWidget_ext::QDockWidget_ext(const char* txt, uMainWindow* par, jvxBool* refReport, Qt::DockWidgetArea area): QDockWidget(txt, par)
{
  parRef = par;
  refReportClose = refReport;
  iLiveInArea = area;
  connect(this, SIGNAL(topLevelChanged  (bool)), this, SLOT(newTopLevel(bool)));
};
QDockWidget_ext::~QDockWidget_ext()
{
};

void QDockWidget_ext::closeEvent ( QCloseEvent * event )
{
  QDockWidget::closeEvent(event);
  *refReportClose = false;
  parRef->updateWindow();
};

void QDockWidget_ext::newTopLevel( bool tLevel)
{
  if(tLevel == false)
    {
      parRef->dockWidgetReturned(iLiveInArea);
    }
};
