#ifndef __HJVXCOMPONENENTTOOLSQT_H__
#define __HJVXCOMPONENENTTOOLSQT_H__

#include <QComboBox>

#define JVX_USER_ROLE_COMPONENT_ID (Qt::UserRole + 1)
#define JVX_USER_ROLE_COMPONENT_SLOT_ID (Qt::UserRole + 2)
#define JVX_USER_ROLE_COMPONENT_SLOTSUB_ID (Qt::UserRole + 3)

void jvx_qt_listComponentsInComboBox_(bool addressesStandardComponents, QComboBox* combo, IjvxHost* hostRef, jvxComponentIdentification& myCp);

void jvx_qt_listProcessesInComboBox_(bool addressesStandardComponents, QComboBox* combo, IjvxHost* hostRef, jvxComponentIdentification& myCp, 
	const std::string& matchProcExpr);

void jvx_qt_listComponentsInComboBox(bool addressesStandardComponents, QComboBox* combo, IjvxHost* hostRef, jvxComponentIdentification myCp,
	jvxComponentIdentification* spSelect);

void jvx_qt_listPropertiesInComboBox(jvxComponentIdentification tp, jvxPropertyContext ctxtShow, jvxInt16 whatToShow, QComboBox* combo, IjvxHost* hostRef, jvxSize& indexShow, jvxSize* indexComponent, jvxRealtimeViewerFilterShow filter);

void jvx_qt_propertiesIndexShowForIndexComponent(jvxComponentIdentification tp, IjvxHost* hostRef, jvxSize& indexShow, jvxComponentIdentification indexComponent, 
	jvxRealtimeViewerFilterShow filter);

#endif
