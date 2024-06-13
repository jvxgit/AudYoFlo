/*
typedef struct
{
	jvxUInt16 proto_family;
} jvxProtocolFamilyHeader; 

// Subtype adv protocol header: extends family header by 2 and 4 byte fields 
typedef struct
{
	jvxProtocolFamilyHeader fam_hdr;
	
	jvxUInt16 purpose;
	jvxUInt32 paketsize;
} jvxProtocolHeader;
*/

import 'dart:typed_data';
import 'package:ayf_pack/ayf_pack_common.dart';

enum JvxProtocolType {
  JVX_PROTOCOL_GLOBAL_ERROR,
  JVX_PROTOCOL_TYPE_AUDIO_DEVICE_LINK,
  JVX_PROTOCOL_TYPE_PROPERTY_STREAM,
  JVX_PROTOCOL_TYPE_GENERIC_MESSAGE,
  JVX_PROTOCOL_TYPE_REMOTE_CALL,
  JVX_PROTOCOL_TYPE_STRING,
  JVX_PROTOCOL_TYPE_RAW_DATA_STREAM,
  JVX_PROTOCOL_TYPE_RAW_PROPERTY_STREAM,
  JVX_PROTOCOL_TYPE_SIMPLE_AUDIO_DEVICE_LINK
}

enum JvxPsType {
  JVX_PS_CONFIGURE_PROPERTY_OBSERVATION,
  JVX_PS_ADD_PROPERTY_TO_OBSERVE,
  JVX_PS_REMOVE_PROPERTY_TO_OBSERVE,
  JVX_PS_TRIGGER_PROPERTY_TRANSFER,
  JVX_PS_SEND_LINEARFIELD,
  JVX_PS_SEND_CIRCFIELD,
  JVX_PS_SEND_INFORM_CHANGE,
  JVX_PS_SEND_INFORM_SYSTEM_CHANGE,
  JVX_PS_SEND_INFORM_PROPERTY_CHANGE,
  JVX_PS_SEND_INFORM_SEQUENCER_EVENT
}

int JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_EXTRACT_MASK = 0xC000;
int JVX_PROTOCOL_ADVLINK_MESSAGE_COMMAND_EXTRACT_MASK = 0x3FFF;

int JVX_ADV_DECRIPTION_LENGTH = 32;

int JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER = 0x4000;
int JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST = 0x8000;
int JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_SPECIFY = 0xC000;
int JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE = 0x0000;

// ===========================================================================
// ===========================================================================

enum JvxBinSocketConnection {
  JVX_BIN_SOCKET_CONNECTION_NONE,
  JVX_BIN_SOCKET_CONNECTION_INIT,
  JVX_BIN_SOCKET_CONNECTION_STARTED,
}

// ===========================================================================
// ===========================================================================
class callByReferenceOffset {
  int offset = 0;
}

class JvxProtocolHelper {
  bool isLittleEndian = true;
  int extract_lsbyte(int val, int bitwidth) {
    if (isLittleEndian) {
      return val & 0xFF;
    }
    assert(false);
    return 0;
  }

  int insert_lsbyte(int val, int toInsert, int bitwidth) {
    if (isLittleEndian) {
      val = val | ((toInsert & 0xFF) << (bitwidth - 8));
      return val;
    }
    assert(false);
    return 0;
  }

  int shift_forward(int val) {
    if (isLittleEndian) {
      return val >> 8;
    }
    assert(false);
    return 0;
  }

  int shift_backward(int val) {
    if (isLittleEndian) {
      return val << 8;
    }
    assert(false);
    return 0;
  }

  int read_ui8(Uint8List lst, callByReferenceOffset cbR) {
    int val = 0;
    val = insert_lsbyte(val, lst.elementAt(cbR.offset), 8);
    cbR.offset++;
    return val;
  }

  List<int> write_ui8(List<int> lst, int val8) {
    lst.add(extract_lsbyte(val8, 8));
    return lst;
  }

  int read_ui16(Uint8List lst, callByReferenceOffset cbR) {
    int val = 0;
    val = insert_lsbyte(val, lst.elementAt(cbR.offset), 16);
    val = shift_forward(val);
    cbR.offset++;
    val = insert_lsbyte(val, lst.elementAt(cbR.offset), 16);
    cbR.offset++;
    return val;
  }

  List<int> write_ui16(List<int> lst, int val16) {
    lst.add(extract_lsbyte(val16, 16));
    val16 = shift_forward(val16);
    lst.add(extract_lsbyte(val16, 16));
    return lst;
  }

  // =========================================================================================

  int read_ui32(Uint8List lst, callByReferenceOffset cbR) {
    int val = 0;
    val = insert_lsbyte(val, lst.elementAt(cbR.offset), 32);
    val = shift_forward(val);
    cbR.offset++;
    val = insert_lsbyte(val, lst.elementAt(cbR.offset), 32);
    val = shift_forward(val);
    cbR.offset++;
    val = insert_lsbyte(val, lst.elementAt(cbR.offset), 32);
    val = shift_forward(val);
    cbR.offset++;
    val = insert_lsbyte(val, lst.elementAt(cbR.offset), 32);
    cbR.offset++;
    return val;
  }

  List<int> write_ui32(List<int> lst, int val32) {
    lst.add(extract_lsbyte(val32, 32));
    val32 = shift_forward(val32);
    lst.add(extract_lsbyte(val32, 32));
    val32 = shift_forward(val32);
    lst.add(extract_lsbyte(val32, 32));
    val32 = shift_forward(val32);
    lst.add(extract_lsbyte(val32, 32));
    return lst;
  }
}

class jvxProtocolFamilyHeader extends JvxProtocolHelper {
  int proto_family_ui16 = 0;

  List<int> serialize() {
    List<int> newLst = [];
    newLst = write_ui16(newLst, proto_family_ui16);
    return newLst;
  }

  static int sizeofType() {
    return 2;
  }

  void fromIntList(Uint8List lst, {callByReferenceOffset? cbRArg}) {
    callByReferenceOffset cbR = callByReferenceOffset();
    if (cbRArg != null) {
      cbR = cbRArg;
    }
    proto_family_ui16 = read_ui16(lst, cbR);
  }
}

class jvxProtocolHeader extends JvxProtocolHelper {
  jvxProtocolFamilyHeader loc_header = jvxProtocolFamilyHeader();
  int purpose_ui16 = 0;
  int paketsize_ui32 = 0;
  List<int> serialize() {
    List<int> fromSuper = loc_header.serialize();
    fromSuper = write_ui16(fromSuper, purpose_ui16);
    fromSuper = write_ui32(fromSuper, paketsize_ui32);
    return fromSuper;
  }

  static int sizeofType() {
    return jvxProtocolFamilyHeader.sizeofType() + 2 + 4;
  }

  void fromIntList(Uint8List lst, {callByReferenceOffset? cbRArg}) {
    callByReferenceOffset cbR = callByReferenceOffset();
    if (cbRArg != null) {
      cbR = cbRArg;
    }
    loc_header.fromIntList(lst, cbRArg: cbR);
    purpose_ui16 = read_ui16(lst, cbR);
    paketsize_ui32 = read_ui32(lst, cbR);
  }
}

class JvxPropertyConfigurePropertySend extends JvxProtocolHelper {
  jvxProtocolHeader loc_header = jvxProtocolHeader();
  int tick_msec_ui16 = 1000;
  int ping_count_ui16 = 10;
  int user_specific_ui32 = 0;

  List<int> serialize() {
    List<int> fromSuper = loc_header.serialize();
    fromSuper = write_ui16(fromSuper, tick_msec_ui16);
    fromSuper = write_ui16(fromSuper, ping_count_ui16);
    fromSuper = write_ui32(fromSuper, user_specific_ui32);
    return fromSuper;
  }

  static int sizeofType() {
    return jvxProtocolHeader.sizeofType() + 2 + 2 + 4;
  }
}

class jvxPropertyPropertyObserveHeader extends JvxProtocolHelper {
  jvxProtocolHeader loc_header = jvxProtocolHeader();

  // Speccify the component to address
  int component_type_ui16 = 0;
  int component_slot_ui16 = 0;
  int component_subslot_ui16 = 0;

  // Specify the state in which the property stream is active. Can and should be a bitfield to activate for multiple states.
  int state_active_ui16 = jvxState.JVX_STATE_ACTIVE |
      jvxState.JVX_STATE_PREPARED |
      jvxState.JVX_STATE_PROCESSING;

  // Offset in field if selected filed portion shall be addressed
  int property_offset_ui16 =
      jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE.index;

  // Property format
  int property_format_ui8 = jvxDataFormatEnum.JVX_DATAFORMAT_DATA.index;

  // Property decoder hint type
  int property_dec_hint_tp_ui8 =
      jvxPropertyDecoderHintTypeEnum.JVX_PROPERTY_DECODER_NONE.index;

  // Expected number of elements
  int property_num_elements_ui32 = -1;

  // We may add an integer here which is only transferred back but not used on the backend side!!
  int user_specific_ui32 = 0;

  // The following two properties allow to segment a field extract in case we have a 2-D field to be transferred
  // param_0 : extension X-dimension
  // param_1: extension Y-dimension
  int param0_ui32 = -1;
  int param1_ui32 = -1;

  // Condition for update, type JvxPropertyStreamCondUpdate
  // JvxPropertyStreamCondUpdate.JVX_PROP_STREAM_UPDATE_TIMEOUT: update after a timeout period
  // JvxPropertyStreamCondUpdate.JVX_PROP_STREAM_UPDATE_ON_CHANGE: update if property has changed
  // JvxPropertyStreamCondUpdate.JVX_PROP_STREAM_UPDATE_ON_REQUEST: update of request
  // JvxPropertyStreamCondUpdate.JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN <- reports only the origin
  int cond_update_ui16 =
      JvxPropertyStreamCondUpdate.JVX_PROP_STREAM_UPDATE_TIMEOUT.index;

  // Valid for transfer modes
  // -> "JVX_PROP_STREAM_UPDATE_ON_CHANGE": This argument is the timeout in msecs to restart property transfer
  // -> "JVX_PROP_STREAM_UPDATE_TIMEOUT": This argument is the modulo number to time down the update period.
  int param_cond_update_ui16 = 0;

  // Seems to be no longer in use..
  int cnt_report_disconnect_ui16 = 0;

  // Flag to activate flow control
  int requires_flow_control_ui16 = 0;

  // Space to fill up to 32 bits
  int reserved1_ui16 = 0;

  // Priority: Type JvxPropertyTransferPriority
  // JvxPropertyTransferPriority.JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO,
  // JvxPropertyTransferPriority.JVX_PROP_CONNECTION_TYPE_HIGH_PRIO
  // JVX_PROP_CONNECTION_TYPE_HIGH_PRIO is routed to a UDP transfer which will not be decoded by a websocket connection!!
  int priority_ui16 = 0;

  // Space to fill up to 32 bits
  int reserved2_ui16 = 0;

  // Port number for high prio UDP streaming in case of priority "JVX_PROP_CONNECTION_TYPE_HIGH_PRIO"
  int port_ui32 = 0;

  // Segmentation of property streaming in case of hogh priority streaming of circular buffer transfers
  int num_emit_min_ui32 = 0;
  int num_emit_max_ui32 = 0;

  List<int> serialize() {
    List<int> fromSuper = loc_header.serialize();
    fromSuper = write_ui16(fromSuper, component_type_ui16);
    fromSuper = write_ui16(fromSuper, component_slot_ui16);
    fromSuper = write_ui16(fromSuper, component_subslot_ui16);
    fromSuper = write_ui16(fromSuper, state_active_ui16);
    fromSuper = write_ui16(fromSuper, property_offset_ui16);
    fromSuper = write_ui8(fromSuper, property_format_ui8);
    fromSuper = write_ui8(fromSuper, property_dec_hint_tp_ui8);
    fromSuper = write_ui32(fromSuper, property_num_elements_ui32);
    fromSuper = write_ui32(fromSuper, user_specific_ui32);
    fromSuper = write_ui32(fromSuper, param0_ui32);
    fromSuper = write_ui32(fromSuper, param1_ui32);
    fromSuper = write_ui16(fromSuper, cond_update_ui16);
    fromSuper = write_ui16(fromSuper, param_cond_update_ui16);
    fromSuper = write_ui16(fromSuper, cnt_report_disconnect_ui16);
    fromSuper = write_ui16(fromSuper, requires_flow_control_ui16);
    fromSuper = write_ui16(fromSuper, reserved1_ui16);
    fromSuper = write_ui16(fromSuper, priority_ui16);
    fromSuper = write_ui16(fromSuper, reserved2_ui16);
    fromSuper = write_ui32(fromSuper, port_ui32);
    fromSuper = write_ui32(fromSuper, num_emit_min_ui32);
    fromSuper = write_ui32(fromSuper, num_emit_max_ui32);
    return fromSuper;
  }

  static int sizeofType() {
    return jvxProtocolHeader.sizeofType() +
        5 * 2 +
        2 * 1 +
        4 * 4 +
        7 * 2 +
        3 * 4;
  }
}

// ================================================================================

class jvxPropertyConfigurePropertySend_response extends JvxProtocolHelper {
  jvxProtocolHeader loc_header = jvxProtocolHeader();

  int errcode_ui16 = 0;
  int reserved_ui16 = 0;

  int user_specific_ui32 = 0;

  void fromIntList(Uint8List lst, {callByReferenceOffset? cbRArg}) {
    callByReferenceOffset cbR = callByReferenceOffset();
    if (cbRArg != null) {
      cbR = cbRArg;
    }
    loc_header.fromIntList(lst, cbRArg: cbR);
    errcode_ui16 = read_ui16(lst, cbR);
    reserved_ui16 = read_ui16(lst, cbR);
    user_specific_ui32 = read_ui32(lst, cbR);
  }
}

class jvxPropertyPropertyObserveHeader_response extends JvxProtocolHelper {
  jvxProtocolHeader loc_header = jvxProtocolHeader();

  int errcode_ui16 = jvxErrorType.JVX_NO_ERROR;
  int stream_id_ui16 = -1;
  int user_specific_ui32 = 0;

  int component_type_ui16 =
      jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE.index;
  int component_slot_ui16 = -1;
  int component_subslot_ui16 = -1;

  void fromIntList(Uint8List lst, {callByReferenceOffset? cbRArg}) {
    callByReferenceOffset cbR = callByReferenceOffset();
    if (cbRArg != null) {
      cbR = cbRArg;
    }
    loc_header.fromIntList(lst, cbRArg: cbR);
    errcode_ui16 = read_ui16(lst, cbR);
    stream_id_ui16 = read_ui16(lst, cbR);
    user_specific_ui32 = read_ui32(lst, cbR);

    component_type_ui16 = read_ui16(lst, cbR);
    component_slot_ui16 = read_ui16(lst, cbR);
    component_subslot_ui16 = read_ui16(lst, cbR);
  }

  static int sizeofType() {
    return jvxProtocolHeader.sizeofType() + 2 * 2 + 1 * 4 + 3 * 2;
  }
}
