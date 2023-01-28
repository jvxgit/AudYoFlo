#ifndef __ULOADSAVEDIALOG_H__
#define __ULOADSAVEDIALOG_H__

#include "ui_loadsaveFileDialog.h"
#include "jvx.h"
// ======================================================================================
// ======================================================================================
// ======================================================================================
// ======================================================================================

//#include "uMainWindow_defines.h"

class uLoadSaveDialog: public QDialog, public Ui::Dialog
{
	Q_OBJECT

private:
	
	jvxBool* stopVariable;


public:
	uLoadSaveDialog(QWidget* parent);

	~uLoadSaveDialog();
	void setupUi();
	void setStopVariable(jvxBool* stop);
	void setText(std::string & fileName);			
	void setProgress(jvxData val);
	virtual void accept();
};

#endif