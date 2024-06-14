import '../ayf_pack_local.dart';

enum JvxPropertyTransferPriority {
  JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO,
  JVX_PROP_CONNECTION_TYPE_HIGH_PRIO
}

enum JvxPropertyStreamCondUpdate {
  JVX_PROP_STREAM_UPDATE_TIMEOUT,
  JVX_PROP_STREAM_UPDATE_ON_CHANGE,
  JVX_PROP_STREAM_UPDATE_ON_REQUEST,
  JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN
}

enum JvxPropertyStreamMessageType {
  JVX_PROP_STREAM_SEND_LINEARFIELD,
  JVX_PROP_STREAM_SEND_CIRCFIELD
}

class AudYoFloPropertyStreamDefintion {
  JvxComponentIdentification target = JvxComponentIdentification();
  int stateMaskActive = jvxState.JVX_STATE_ACTIVE |
      jvxState.JVX_STATE_PREPARED |
      jvxState.JVX_STATE_PROCESSING;
  int offset = 0;
  jvxDataFormatEnum format = jvxDataFormatEnum.JVX_DATAFORMAT_DATA;
  jvxPropertyDecoderHintTypeEnum dec_hint_tp =
      jvxPropertyDecoderHintTypeEnum.JVX_PROPERTY_DECODER_NONE;
  int num_elements = -1;
  int param0 = -1;
  int param1 = -1;
  JvxPropertyStreamCondUpdate cond_update =
      JvxPropertyStreamCondUpdate.JVX_PROP_STREAM_UPDATE_TIMEOUT;
  int param_cond_update = 0;
  bool requires_flow_control = false;
  JvxPropertyTransferPriority priority =
      JvxPropertyTransferPriority.JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO;
  int port = 0;
  int num_emit_min = 0;
  int num_emit_max = 0;
  String propertyDescriptor = "/not/described/properly";
}
