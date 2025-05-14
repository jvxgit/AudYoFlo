// Module to provide some native to flutter conversion helpers
import 'package:ffi/ffi.dart';
import 'dart:ffi';

import 'package:ayf_pack/ayf_pack.dart';
import '../native-ffi/generated/ayf_ffi_gen_bind.dart';

// Abstract class with conversion functions
abstract class AudioFlowBackendBridgeHelpers {
  // Decode a component identification (type, slot, subslot, uid)
  static JvxComponentIdentification decodeCpIdReqCommand(
      AudioYoFloNative natLib, Pointer<Void> load_fld) {
    Pointer<component_ident> cpIdPtr = calloc<component_ident>(1);

    int resLoc = natLib.ffi_req_command_decode_origin(load_fld, cpIdPtr);

    var ret = JvxComponentIdentification();
    component_ident cpIdRef = cpIdPtr.ref;

    ret.cpTp = JvxComponentTypeEEnum.fromInt(cpIdRef.tp);
    ret.slotid = cpIdRef.slotid;
    ret.slotsubid = cpIdRef.slotsubid;
    ret.uid = cpIdRef.uId;
    calloc.free(cpIdPtr);

    return ret;
  }

  // Function to decode uId for processes
  int decodeUId(AudioYoFloNative natLib, Pointer<Void> load_fld) {
    Pointer<Size> uIdPtr = calloc<Size>(1);
    int resLoc = natLib.ffi_req_command_decode_uid(load_fld, uIdPtr);
    int uId = uIdPtr.value;
    calloc.free(uIdPtr);
    return uId;
  }

// Function to decode state switches
  jvxStateSwitchEnum decodeSswitch(
      AudioYoFloNative natLib, Pointer<Void> load_fld) {
    Pointer<Int> ssPtr = calloc<Int>(1);
    int resLoc = natLib.ffi_req_command_decode_sswitch(load_fld, ssPtr);
    int ssi = ssPtr.value;
    calloc.free(ssPtr);
    return jvxStateSwitchEEnum.fromInt(ssi);
  }

  // Function to decode state switches
  jvxReportCommandRequestEnum decodeRequestType(
      AudioYoFloNative natLib, Pointer<Void> load_fld) {
    Pointer<Int> reqPtr = calloc<Int>(1);
    int resLoc = natLib.ffi_req_command_decode_request_type(load_fld, reqPtr);
    int reqi = reqPtr.value;
    calloc.free(reqPtr);
    return jvxReportCommandRequestEEnum.fromInt(reqi);
  }

  // Function to decode command requests
  jvxReportCommandRequestEnum decodeReqCommand(int rci) {
    return jvxReportCommandRequestEEnum.fromInt(rci);
  }

  // Function to decode command requests
  jvxReportCommandBroadcastTypeEnum decodeBroadcastType(
      AudioYoFloNative natLib, Pointer<Void> load_fld) {
    Pointer<Int> bcPtr = calloc<Int>(1);
    int resLoc = natLib.ffi_req_command_decode_broadcast(load_fld, bcPtr);
    int bcId = bcPtr.value;
    calloc.free(bcPtr);
    return jvxReportCommandBroadcastTypeEEnum.fromInt(bcId);
  }
}
