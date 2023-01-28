#ifndef __CONFIGUREAUDIO_UIEXTENSION_H__
#define __CONFIGUREAUDIO_UIEXTENSION_H__

/* 
 * Delegate interface for extension of the host for socket connections
 *///================================================================
 class QFrame;
 
JVX_INTERFACE IjvxQtComponentWidget
{
public:
	
	IjvxQtComponentWidget(){}; // IjvxQtComponentWidget
	
	virtual ~IjvxQtComponentWidget(){};

	virtual void init_submodule(IjvxHost* theHost) = 0;
	virtual void terminate_submodule() = 0;

	virtual void start_show(IjvxAccessProperties* propRef, QFrame* placeHere, const char* propPrefix) = 0;
	virtual void stop_show(QFrame* placeHere) = 0;

	virtual void updateWindow() = 0;
	virtual void updateWindow_periodic() = 0;
};

#endif
