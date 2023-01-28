#include "jvx-qt-helpers.h"

#include <QLabel>
#include <QDesktopWidget>
#include <QWindow>
#include <QScreen>

void setBackgroundLabelColor(QColor col, QLabel* lab)
{
	QPalette pal;
	lab->setAutoFillBackground(true);
	pal = lab->palette();
    QBrush brush(col);
    brush.setStyle(Qt::SolidPattern);
    pal.setBrush(QPalette::All, QPalette::Window, brush);
    lab->setPalette(pal);
}

std::vector<std::string> jvx_QStringList_to_std(QStringList& lstIn)
{
	jvxSize i;
	std::vector<std::string> lst;
	for (i = 0; i < lstIn.size(); i++)
	{
		lst.push_back(lstIn[(int)i].toLatin1().data());
	}
	return(lst);
}

QStringList jvx_QStringList_to_std(std::vector<std::string> & lstIn)
{
	jvxSize i;
	QStringList lst;
	for (i = 0; i < lstIn.size(); i++)
	{
		lst.append(lstIn[i].c_str());
	}
	return(lst);
}

void jvx_replaceInAccessibleDescription(QWidget* fromHere, const std::string& from, const std::string& to)
{
	jvxSize i;
	QList<QWidget *> widgets;
	widgets = fromHere->findChildren<QWidget *>();
	for (i = 0; i < widgets.count(); i++)
	{
		QString aDescr = widgets[(int)i]->accessibleDescription();
		widgets[(int)i]->setProperty("JVX_ACCESSIBLE_DESCRIPTION_MODIFY_ORIG", QVariant(aDescr));
		std::string accDescr = aDescr.toLatin1().data();
		accDescr = jvx_replaceStrInStr(accDescr, from, to);
		widgets[(int)i]->setAccessibleDescription(accDescr.c_str());
	}
}

void jvx_undoReplaceInAccessibleDescription(QWidget* fromHere)
{
	jvxSize i;
	QList<QWidget *> widgets;
	widgets = fromHere->findChildren<QWidget *>();
	for (i = 0; i < widgets.count(); i++)
	{
		QVariant var = widgets[(int)i]->property("JVX_ACCESSIBLE_DESCRIPTION_MODIFY_ORIG");
		QString valStr;
		if (var.isValid())
		{
			valStr = var.toString();
		}
		if (!valStr.isEmpty())
		{
			widgets[(int)i]->setAccessibleDescription(valStr);
			widgets[(int)i]->setProperty("JVX_ACCESSIBLE_DESCRIPTION_MODIFY_ORIG", {});
		}
	}
}

QRect jvx_get_screen_geometry_qt()
{
	QDesktopWidget widget;
	QScreen* scr = widget.windowHandle()->screen();
	QRect desktopRect = scr->geometry();
	return desktopRect;
}

QSplashScreen* jvx_start_splash_qt(jvxData scaleSplash, const char* pixmapf, int ft_size, jvxBool it_font)
{
	QSplashScreen* splash = NULL;
	QRect desktopRect = jvx_get_screen_geometry_qt();
	QPixmap pixmap;
	if (pixmap.load(pixmapf))
	{
		QSize szPix = pixmap.size();
		int ymin = JVX_MIN(scaleSplash*desktopRect.height(), pixmap.height());
		int xmin = JVX_MIN(scaleSplash*desktopRect.width(), pixmap.width());
		splash = new QSplashScreen(pixmap.scaled(QSize((int)xmin, (int)ymin), Qt::KeepAspectRatio));
		splash->show();
		QFont ft = splash->font();
		ft.setPixelSize(ft_size);
		if (it_font)
		{
			ft.setItalic(true);
		}
		splash->setFont(ft);
	}
	else
	{
		std::cout << "Failed to load splash pixmap <" << pixmapf << ">." << std::endl;
	}
	return splash;
}

void jvx_stop_splash_qt(QSplashScreen* splash, QWidget* widg)
{
	if (splash)
	{
		splash->finish(widg);
		delete(splash);
	}
}

