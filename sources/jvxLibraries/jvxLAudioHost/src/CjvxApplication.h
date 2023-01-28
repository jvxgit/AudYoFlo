#ifndef JVX_APPLICATION_H
#define JVX_APPLICATION_H

#include <QApplication>
#include <QEvent>

class CjvxApplication : public QApplication
{
    Q_OBJECT
public:
	CjvxApplication(int &argc, char **argv);
    virtual bool notify(QObject*, QEvent*);
};

#endif // MYGUIAPPLICATION_H