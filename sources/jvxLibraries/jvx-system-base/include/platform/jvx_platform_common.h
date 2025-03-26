#if !defined( JVX_SIZE_IS_UINT64) && !defined( JVX_SIZE_IS_UINT32)
#define JVX_DECLARE_QT_METATYPES_1 Q_DECLARE_METATYPE(jvxSize)
#else
#define JVX_DECLARE_QT_METATYPES_1 /* Q_DECLARE_METATYPE(jvxSize)*/
#endif

#ifndef JVX_CBOOL_IS_UINT16
#define JVX_DECLARE_QT_METATYPES_2 Q_DECLARE_METATYPE(jvxCBool)
#else
#define JVX_DECLARE_QT_METATYPES_2  /* Q_DECLARE_METATYPE(jvxCBool)*/
#endif

#ifndef JVX_CBITFIELD_IS_UINT64
#define JVX_DECLARE_QT_METATYPES_3 Q_DECLARE_METATYPE(jvxCBitField)
#else
#define JVX_DECLARE_QT_METATYPES_3  /* Q_DECLARE_METATYPE(jvxCBitField)*/
#endif

#define JVX_SEPARATOR_DIR_ALL_PLATFORMS "/"

#define JVX_DECLARE_QT_METATYPES \
JVX_DECLARE_QT_METATYPES_1 \
JVX_DECLARE_QT_METATYPES_2 \
JVX_DECLARE_QT_METATYPES_3 \
Q_DECLARE_METATYPE(jvxBool)\
Q_DECLARE_METATYPE(jvxInt16)\
Q_DECLARE_METATYPE(jvxUInt16)\
Q_DECLARE_METATYPE(jvxInt64)\
Q_DECLARE_METATYPE(jvxUInt64)\
Q_DECLARE_METATYPE(jvxInt32)\
Q_DECLARE_METATYPE(jvxUInt32)\
Q_DECLARE_METATYPE(jvxReportPriority)\
Q_DECLARE_METATYPE(jvxComponentType)\
Q_DECLARE_METATYPE(jvxComponentIdentification)\
Q_DECLARE_METATYPE(IjvxObject*)\
Q_DECLARE_METATYPE(jvxPropertyCategoryType)\
Q_DECLARE_METATYPE(jvxPropertyCallPurpose) \
Q_DECLARE_METATYPE(jvxSequencerQueueType) \
Q_DECLARE_METATYPE(CjvxReportCommandRequest*)

#define JVX_REGISTER_QT_METATYPES \
qRegisterMetaType<jvxSize>("jvxSize"); \
qRegisterMetaType<jvxCBool>("jvxCBool"); \
qRegisterMetaType<jvxCBitField>("jvxCBitField");\
qRegisterMetaType<jvxBool>("jvxBool");\
qRegisterMetaType<jvxInt16>("jvxInt16");\
qRegisterMetaType<jvxUInt16>("jvxUInt16");\
qRegisterMetaType<jvxInt32>("jvxInt32");\
qRegisterMetaType<jvxUInt32>("jvxUInt32");\
qRegisterMetaType<jvxInt64>("jvxInt64");\
qRegisterMetaType<jvxUInt64>("jvxUInt64");\
qRegisterMetaType<jvxReportPriority>("jvxReportPriority");\
qRegisterMetaType<jvxComponentType>("jvxComponentType");\
qRegisterMetaType<jvxComponentIdentification>("jvxComponentIdentification");\
qRegisterMetaType<IjvxObject*>("IjvxObject*");\
qRegisterMetaType<jvxPropertyCategoryType>("jvxPropertyCategoryType");\
qRegisterMetaType<bool>("bool");\
qRegisterMetaType<jvxPropertyCallPurpose>("jvxPropertyCallPurpose"); \
qRegisterMetaType<jvxSequencerQueueType>("jvxSequencerQueueType"); \
qRegisterMetaType<CjvxReportCommandRequest*>("CjvxReportCommandRequest*");

