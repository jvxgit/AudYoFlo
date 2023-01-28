#ifndef __HJVXQTHELPERS_H__
#define __HJVXQTHELPERS_H__

#include <QLabel>
#include <QSplashScreen>

#define JVX_QSTRING_2_STDSTRING(qstr) qstr.toLatin1().data()

void setBackgroundLabelColor(QColor col, QLabel* lab);

std::vector<std::string> jvx_QStringList_to_std(QStringList& lstIn);

QStringList jvx_QStringList_to_std(std::vector<std::string> & lstIn);

void jvx_replaceInAccessibleDescription(QWidget* fromHere, const std::string& from, const std::string& to);
void jvx_undoReplaceInAccessibleDescription(QWidget* fromHere);

QRect jvx_get_screen_geometry_qt();

QSplashScreen* jvx_start_splash_qt(jvxData scaleSplash = 0.8, const char* = ":/images/images/splash.png", int ft_size=24, jvxBool it_font = true);
void jvx_stop_splash_qt(QSplashScreen* splash, QWidget* widg);

#endif
