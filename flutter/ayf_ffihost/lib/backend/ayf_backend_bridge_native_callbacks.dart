/*
 * Module top implement synchronous and asynchronous callbacks. Sync callbacks
 * are endpoints within a native ffi call and end up in a static function. 
 * Async callbacks are triggered via the flutter native port API and will be
 * triggered in a queued approach.
 */

import 'dart:ffi';
import 'dart:isolate';
import '../native-ffi/generated/ayf_ffi_gen_bind.dart';
import 'ayf_backend_bridge_native_helpers.dart';
import 'ayf_backend_bridge.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:ayf_pack_native/ayf_pack_native.dart';

// =========================================================================
// Definition of natove callback for sync messages
// =========================================================================
typedef callback_tp = Int32 Function(Pointer<report_callback>);
typedef callback_report = Int32 Function(Pointer<report_callback>);

// =========================================================================
// Class to concentrate all callbacks in the ferncast native bridge component
// =========================================================================
abstract class AudioFlowBackendBridgeCallbacks
    extends AudioFlowBackendBridgeHelpers {
// Generrated part of the ffi native library
  AudioYoFloNative? natLib;

  // Pointer to address the host handle
  Pointer<Void> opaque_host = nullptr;

  void fill_static_callbacks(Pointer<func_pointers> pointers) {
    func_pointers func_ptrs_assign = pointers.ref;

    func_ptrs_assign.cb_sync = Pointer.fromFunction<callback_tp>(
        static_sync_callback, jvxErrorType.JVX_ERROR_INTERNAL);
  }

  int installNativePort() {
    // Install a native port to report to the DART side
    // Good thing: requestExecuteCallback is within class context
    // and can be invoked from every thread
    final interactiveCppRequests = ReceivePort()..listen(asyncCallback);
    int nativePort = interactiveCppRequests.sendPort.nativePort;
    return nativePort;
  }

  // ============================================================
  // Intermediate callback functions as given by the Flutter system.
  // We get access to the bridge object by accessing the main bridge singleton
  // ============================================================
  static int static_sync_callback(Pointer<report_callback> ptr) {
    // Access to the main bridge only via global reference since we are not in build tree
    AudioFlowBackendBridge_ inClass = new AudioFlowBackendBridge_();

    // Return to bridge component
    assert(false);
    return 0;
    //inClass.sync_callback(ptr);
  }

  // ===================================================================
  // Async callback as triggered via the native port
  // ===================================================================
  void asyncCallback(dynamic message) {
    final int cbk_address = message;
    final cbks = Pointer<report_callback>.fromAddress(cbk_address);
    report_callback cbkref = cbks.ref;

    int id1 = cbkref.callback_id;
    int id2 = cbkref.callback_subid;
    int res = jvxErrorType.JVX_NO_ERROR;

    switch (id1) {
      case ffiCallbackIds.JVX_FFI_CALLBACK_ASYNC_REPORT_TEXT:
        printFormattedTextMessage(id2, cbkref.load_fld);
        natLib!.ffi_host_delete(
            cbkref.load_fld, ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
        break;

      case ffiCallbackIds.JVX_FFI_CALLBACK_ASYNC_REPORT_COMMAND_REQUEST:

        // This should not popup here
        assert(false);
        break;

      case ffiCallbackIds.JVX_FFI_CALLBACK_REQUEST_COMMAND:

        // Decode the broadcast type to filtert out rescheduled commands
        jvxReportCommandBroadcastTypeEnum bcEnum =
            decodeBroadcastType(natLib!, cbkref.load_fld);

        // Forward the async message. Re-scheduled messages
        // will be filtered out by the backend function!
        res = natLib!
            .ffi_host_request_command_fwd(opaque_host, cbkref.load_fld, 1);

        // Local processing of the request
        if (res == jvxErrorType.JVX_NO_ERROR) {
          jvxReportCommandRequestEnum reqCom = decodeReqCommand(id2);

          processAsyncCommand(
              reqCom, cbkref.load_fld, cbkref.sz_fld, true, bcEnum);
        }

        // This function deletes the "load_fld" object
        natLib!.ffi_host_delete(cbkref.load_fld,
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_REQUEST_COMMAND);
        break;

      default:
        print('Uncaught asynchronous callback.');
        print(StackTrace.current);
    }
    final delv = Pointer<Void>.fromAddress(cbk_address);
    natLib!.ffi_host_delete(
        delv, ffiDeleteDatatype.JVX_DELETE_DATATYPE_ASYNC_CALLBACK);
  }

  // ============================================================
  // Final mapping goal to forward synchronous and asynchronous callbacks
  // ============================================================
  int sync_callback(Pointer<report_callback> cbks) {
    report_callback cbkref = cbks.ref;

    int id1 = cbkref.callback_id;
    int id2 = cbkref.callback_subid;
    int res = jvxErrorType.JVX_NO_ERROR;

    switch (id1) {
      case ffiCallbackIds.JVX_FFI_CALLBACK_SYNC_REPORT_INTERNALS_CHANGED:
        break;

      case ffiCallbackIds.JVX_FFI_CALLBACK_REQUEST_COMMAND:

        // Forward the sync call to backend library
        res = natLib!
            .ffi_host_request_command_fwd(opaque_host, cbkref.load_fld, 0);

        // The following are tasks that must be run from within the
        // UI host and within the main thread!
        jvxReportCommandRequestEnum reqTp =
            decodeRequestType(natLib!, cbkref.load_fld);
        switch (reqTp) {
          case jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE:

            // Here, we need to run the sequener step IMMEDIATELY!
            natLib!.ffi_sequencer_trigger(opaque_host);
            break;

          default:
            break;
        }

        //natLib.ffi_comm

        if (res == jvxErrorType.JVX_NO_ERROR) {
          // Sync calls must be re-scheduled to enter the UI to keep everything in order
          res = natLib!.ffi_host_request_command_reschedule(cbkref.load_fld);
          /*    
          jvxReportCommandRequestEnum reqCom = decodeReqCommand(id2);
          process_request_command(
              reqCom, cbkref.load_fld, cbkref.sz_fld, false);
          */
        }
        break;

      default:
        res = jvxErrorType.JVX_ERROR_UNSUPPORTED;
        print('Uncaught synchronous callback.');
        print(StackTrace.current);
    }
    return res;
  }

  // =======================================================================
  // Virtual abstract member functions to be re-defined by derived classes
  // =======================================================================
  void processAsyncCommand(
      jvxReportCommandRequestEnum reqCom,
      Pointer<Void> load_fld,
      int sz_fld,
      bool async,
      jvxReportCommandBroadcastTypeEnum bcEnum);
}
