#include "CjvxQtProject.h"

// ========================================================================

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtSpecificHWidget, jvxQtProject, CjvxQtProject)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtSpecificHWidget, CjvxQtProject)

CjvxQtProject::CjvxQtProject(QWidget* parent) : QWidget(parent)
{
	// If a resource shall be included
	Q_INIT_RESOURCE(jvx_signals);
}

CjvxQtProject::~CjvxQtProject()
{
}

// =========================================================================

// Interface <IjvxQtSpecificWidget_h_base>
void 
CjvxQtProject::init(IjvxHost* theHostRef, jvxCBitField mode, jvxHandle* specPtr, IjvxQtSpecificWidget_report* bwd)
{
	this->setupUi(this);
	theHost = theHostRef;
}

// --> Interface <IjvxQtSpecificHWidget>
jvxErrorType 
CjvxQtProject::addPropertyReference(IjvxAccessProperties* propRef, const std::string& prefixArg, const std::string& identifierArg)
{
	if (theProps == nullptr)
	{
		theProps = propRef;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxQtProject::removePropertyReference(IjvxAccessProperties* propRef)
{
	if (theProps == propRef)
	{
		theProps = nullptr;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// =========================================================================
// =========================================================================

// --> Interface <IjvxQtSpecificWidget_base>
void
CjvxQtProject::getMyQWidget(QWidget** retWidget, jvxSize id )
{
	switch (id)
	{
	case 0:
		if (retWidget)
		{
			*retWidget = static_cast<QWidget*>(this);
		}
		break;
	default:
		if (retWidget)
		{
			*retWidget = nullptr;
		}
	}
}

void 
CjvxQtProject::terminate()
{
	IjvxHost* theHost = nullptr;
	IjvxAccessProperties* theProps = nullptr;
}

void
CjvxQtProject::activate()
{
}

void 
CjvxQtProject::deactivate()
{
}

void 
CjvxQtProject::processing_started()
{
}

void 
CjvxQtProject::processing_stopped()
{
}

void 
CjvxQtProject::update_window(jvxCBitField prio)
{
}

void 
CjvxQtProject::update_window_periodic()

{
}

