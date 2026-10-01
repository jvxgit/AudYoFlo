#ifndef __TJVXTYPES_STR_H__
#define __TJVXTYPES_STR_H__

#define JVXCONFIGSECTION_NUM 8
#define JVXSTATE_NUM 8

extern jvxTextHelpers jvxSizeSpecialValues[JVX_SIZE_NUM_EXTRA_CASES_WITH_0];
extern jvxSize valuesSizeSpecialValues[JVX_SIZE_NUM_EXTRA_CASES_WITH_0];
extern jvxTextHelpers jvxAccessProtocol_str[JVX_ACCESS_PROTOCOL_LIMIT];
extern jvxTextHelpers jvxPropertyAccessType_str[JVX_PROPERTY_ACCESS_LIMIT];
extern jvxTextHelpers jvxPropertyInvalidateType_str[JVX_PROPERTY_INVALIDATE_LIMIT];
extern jvxTextHelpers jvxPropertyCategoryType_str[JVX_PROPERTY_CATEGORY_LIMIT];
extern jvxTextHelpers jvxPropertyContext_str[JVX_PROPERTY_CONTEXT_LIMIT];
extern jvxTextHelpers jvxPropertyDecoderHintType_str[JVX_PROPERTY_DECODER_LIMIT];
extern jvxCBitField jvxPropertySetType_map[JVXPROPERRTY_SETTYPE_NUM];
extern jvxTextHelpers jvxConfigSection_str[JVXCONFIGSECTION_NUM];
extern jvxTextHelpers jvxState_str[JVXSTATE_NUM];
extern jvxCBitField jvxState_map[JVXSTATE_NUM];
extern jvxTextHelpers jvxStateSwitch_str[JVX_STATE_SWITCH_LIMIT];

extern jvxTextHelpers jvxInterfaceType_str[JVX_INTERFACE_LIMIT];
extern jvxTextHelpers  jvxSequencerStatus_str[JVX_SEQUENCER_STATUS_LIMIT];
extern jvxTextHelpers jvxSequencerQueueType_str[JVX_SEQUENCER_QUEUE_TYPE_LIMIT];
extern jvxTextHelpers jvxSequencerEventType_str[JVXSEQUENCEREVENT_NUM];
extern jvxCBitField jvxSequencerEventType_map[JVXSEQUENCEREVENT_NUM];
extern jvxTextHelpers jvxSequencerElementType_str[JVX_SEQUENCER_TYPE_COMMAND_LIMIT];
extern jvxTextHelpers jvxCommandRequestFlag_str[JVX_REPORT_REQUEST_LIMIT];
extern jvxTextHelpers jvxUpdateWindowFlags_str[JVX_UPDATE_WINDOW_LIMIT];
extern jvxTextHelpers jvxFeatureClass_str[JVXFEATURE_CLASS_NUM];
extern jvxTextHelpers jvxPropertySetType_str[JVXPROPERRTY_SETTYPE_NUM];
extern jvxTextHelpers jvxReportPriority_str[JVX_REPORT_PRIORITY_LIMIT];
extern const char* jvxComponentSubType_str[];
extern jvxTextHelpers jvxPropertyStreamCondUpdate_str[JVX_PROP_STREAM_UPDATE_LIMIT];
extern jvxTextHelpers jvxDeviceCapabilities_str[(int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_LIMIT_SHIFT];
extern jvxTextHelpers jvxDeviceDataFlowType_str[(int)jvxDeviceDataFlowType::JVX_DEVICE_DATAFLOW_LIMIT];
extern jvxTextHelpers jvxReportCommandRequest_str[(int)jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_LIMIT];
extern jvxTextHelpers jvxReportCommandDataType_str[(int)jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_LIMIT];
extern jvxTextHelpers jvxSocketsConnectionType_str[(int)jvxSocketsConnectionType::JVX_SOCKET_TYPE_LIMIT];
extern jvxTextHelpers jvxRemoteConnectionQuality_str[(int)jvxRemoteConnectionQuality::JVX_REMOTE_CONNECTION_QUALITY_LIMIT];
extern jvxTextHelpers jvxDataTypeSpec_str[JVX_DATA_TYPE_SPEC_LIMIT];

jvxSize jvxSizeSpecialValues_decode(const char* txt);
const char* jvxAccessProtocol_txt(jvxSize id);
jvxAccessProtocol jvxAccessProtocol_decode(const char* txt);
const char* jvxPropertyAccessType_txt(jvxSize id);
jvxPropertyAccessType jvxPropertyAccessType_decode(const char* txt);
const char* jvxPropertyInvalidateType_txt(jvxSize id);
const char* jvxPropertyExtendedInfoType_txt(jvxSize id);
const char* jvxSectionType_txt(jvxSize id);
const char* jvxState_txt(jvxState stat);
std::string jvxState_dec(jvxSize stat);
jvxSize jvxState_decode(std::string in, jvxBool& err);
const char* jvxStateSwitch_txt(jvxSize id);
const char* jvxInterfaceType_txt(jvxSize id);
const char* jvxPropertyCategoryType_txt(jvxSize id);
jvxPropertyCategoryType jvxPropertyCategoryType_decode(const char* txt);
const char* jvxPropertyContext_txt(jvxSize id);
jvxPropertyContext jvxPropertyContext_decode(const char* txt);
const char* jvxPropertyDecoderHintType_txt(jvxSize id);
jvxPropertyDecoderHintType jvxPropertyDecoderHintType_decode(const char* txt);
const char* jvxSequencerStatus_txt(jvxSize id);
jvxSequencerStatus jvxSequencerStatus_decode(const char* txt);
const char* jvxSequencerQueueType_txt(jvxSize id);
const char* jvxSequencerEventType_txt(jvxCBitField bits);
const char* jvxSequencerElementType_txt(jvxSize id);
const char* jvxCommandRequestFlag_txt(jvxSize id_shift);
const char* jvxUpdateWindowFlags_txt(jvxSize id_shift);
const char* jvxFeatureClass_txt_shift(jvxSize id_shift);
const char* jvxPropertySetType_txt(jvxSize id);
const char* jvxReportPriority_txt(jvxSize id);
const char* jvxComponentAccessType_txt(jvxSize id);
const char* jvxPropertyStreamCondUpdate_txt(jvxSize id);
std::string jvxDeviceCapabilitiesType_txt(jvxCBitField val);
const char* jvxDeviceDataFlowType_txt(jvxDeviceDataFlowType flow);
const char* jvxReportCommandRequest_txt(jvxReportCommandRequest coReq);
const char* jvxReportCommandDataType_txt(jvxReportCommandDataType coTp);
const char* jvxSocketsConnectionType_txt(jvxSocketsConnectionType soTp);
jvxSocketsConnectionType jvxSocketsConnectionType_decode(const char* txt);
const char* jvxRemoteConnectionQuality_txt(jvxRemoteConnectionQuality soTp);
jvxRemoteConnectionQuality jvxRemoteConnectionQuality_decode(const char* txt);
const char* jvxDataTypeSpec_txt(jvxSize id);

#endif
