#ifndef __JVX_NS_H__
#define __JVX_NS_H__

// RTProc namespace definition in case all modules must be put into a
// namespace (when linking static libraries)
#ifdef JVX_CUSTOM_NAMESPACE
#define JVX_NAMESPACE_BEGIN namespace JVX_CUSTOM_NAMESPACE {
#define JVX_NAMESPACE_END }
#define JVX_NAMESPACE_USE(a) JVX_CUSTOM_NAMESPACE::a

#ifdef QT_NAMESPACE
#define JVX_UI_USE(a) QT_NAMESPACE::JVX_UI_NAMESPACE::a
#else
// I would like to derive RTPROC_UI_NAMESPACE from RTPROC_CUSTOM_NAMESPACE
// but this will not be possible since the pre processor wont allow
#define JVX_UI_USE(a) JVX_UI_NAMESPACE::a
#endif

#else // QT_NAMESPACE
#define JVX_NAMESPACE_BEGIN
#define JVX_NAMESPACE_END
#define JVX_NAMESPACE_USE(a) a
#define JVX_UI_USE(a)
#endif // endif

#endif
