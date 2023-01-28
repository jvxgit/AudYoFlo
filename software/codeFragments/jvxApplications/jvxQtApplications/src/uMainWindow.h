#ifndef __UMAINWINDOW_H__
#define __UMAINWINDOW_H__

#include "ui_mainApplication.h"
#include "jvx.h"
#include "codeFragments/commandline/CjvxCommandline.h"
#include <QtGui/QCloseEvent>

class uMainWindow: public QMainWindow, public Ui::MainWindow,
	public CjvxCommandline
{
	Q_OBJECT
private:
	jvxBool  bootupComplete;
public:
	uMainWindow();
	~uMainWindow();
	jvxErrorType initSystem(QApplication* hdlApp, char*[], int);
	void closeEvent(QCloseEvent* event);

public slots:
	void close_app();
	void boot_delayed();
	void save();
	void open_preferences();
	void save_as();
	void open();
signals:
	void emit_close_app();
	void emit_boot_delayed();
};

#endif