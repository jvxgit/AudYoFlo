#ifndef JVX_QT_AUDIOHOST_HOOK_ENTRIES_H__
#define JVX_QT_AUDIOHOST_HOOK_ENTRIES_H__

#include "jvxQtHostHookEntries.h"

// Entry for host enxtension functions
extern jvxErrorType IjvxQtComponentWidget_init(IjvxQtComponentWidget** onReturn, jvxBitField featureClass, const jvxComponentIdentification& tp);
extern jvxErrorType IjvxQtComponentWidget_terminate(IjvxQtComponentWidget* returnme, jvxBitField featureClass, const jvxComponentIdentification& tp);

#endif
