#ifndef __CJVXQTCLASS_QT_H__
#define __CJVXQTCLASS_QT_H__

#include "jvx.h"
template <class T> class CjvxQtClass_qt
{
protected:
	struct
	{
		IjvxProperties* theProps;
		T* ptr_shareWithUi;
		JVX_THREAD_ID idQtThread;
	} _common_set_qt;

public:

	CjvxQtClass_qt(IjvxProperties* theProps)
	{
		_common_set_qt.theProps = theProps;
		_common_set_qt.idQtThread = JVX_GET_CURRENT_THREAD_ID();
	};

	~CjvxQtClass_qt()
	{
	};

	virtual void setReferenceShareWithUi(T* share)
	{
		_common_set_qt.ptr_shareWithUi = share;
	};

	/* Abstract member function which MUST be redefined - at least this one */
	virtual void setupWidget() = 0;

	virtual void updateWindow_sync(jvxState theState, jvxBool update_delayed) = 0;

};

#endif
