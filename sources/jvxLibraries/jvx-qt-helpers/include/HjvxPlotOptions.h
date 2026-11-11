#ifndef __HJVXPLOTOPTIONS__H__
#define __HJVXPLOTOPTIONS__H__

extern "C"
{
#include "jvx_windows/jvx_windows.h"
}

class oneColorOption
{
public:
	oneColorOption(const std::string& nmArg, QColor colArg)
	{
		nm = nmArg;
		col = colArg;
	};
	std::string nm;
	QColor col;
};

class oneWidthOption
{
public:
	oneWidthOption(const std::string& nmArg, jvxData widthArg)
	{
		nm = nmArg;
		width = widthArg;
	};	

	std::string nm;
	jvxData width;
};

class oneWinTypeOption
{
public:
	oneWinTypeOption(jvxInt16 winTpArg, const std::string& descrArg)
	{
		winTp = winTpArg;
		descr = descrArg;
	};

	jvxInt16 winTp = JVX_WINDOW_RECT;
	std::string descr;
};

#endif

