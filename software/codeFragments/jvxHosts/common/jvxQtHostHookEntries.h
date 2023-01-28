#ifndef JVX_QT_HOST_HOOK_ENTRIES_H__
#define JVX_QT_HOST_HOOK_ENTRIES_H__

// Entry functions for main central widget interface
extern jvxErrorType  mainWindow_UiExtension_host_init(mainWindow_UiExtension_host** widgetOnReturn, QWidget* parent);
extern jvxErrorType  mainWindow_UiExtension_host_terminate(mainWindow_UiExtension_host* returnme);

extern jvxErrorType  mainWindow_UiExtension_hostfactory_init(mainWindow_UiExtension_hostfactory** widgetOnReturn, QWidget* parent);
extern jvxErrorType  mainWindow_UiExtension_hostfactory_terminate(mainWindow_UiExtension_hostfactory* returnme);

#endif
