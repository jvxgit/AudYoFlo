import 'package:flutter/material.dart';
import 'package:ffi/ffi.dart';
import 'dart:ffi' as ffi;
import 'dart:io' show File, Platform;
import '../native-ffi/generated/ayf_ffi_gen_bind.dart';
import 'ayf_backend_bridge_native_callbacks.dart';
import 'ayf_backend_adapter_native.dart';
import 'ayf_backend_bridge_native_helpers.dart';
import '../types/ayf_sequencer_types.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:collection/collection.dart';
import 'dart:async';

class AyfNativeAudioHost extends AyfHost {
  // Allocate the audio host with instance Native Audio Host
  static AyfHost _instance = AyfNativeAudioHost();
  static AyfHost get instance => _instance;

  @override
  String descriptionHost() {
    return 'AyfNativeAudioHost';
  }

  @override
  Future<bool> initialize(AudYoFloBackendCache beCache,
      AudYoFloDebugModel? dbgModel, Map<String, dynamic> args) async {
    List<String> cmdArgsLst = [];
    String libraryPath = "not-found";
    int executableInBinFolder = 0;

    var elm =
        args.entries.firstWhereOrNull((element) => element.key == 'cmdArgs');
    if (elm != null) {
      if (elm.value is List<String>) {
        cmdArgsLst = elm.value as List<String>;
      }
    }

    elm = args.entries.firstWhereOrNull((element) => element.key == 'libPath');
    if (elm != null) {
      if (elm.value is String) {
        libraryPath = elm.value as String;
      }
    }

    elm = args.entries.firstWhereOrNull((element) => element.key == 'exeInBin');
    if (elm != null) {
      if (elm.value is int) {
        executableInBinFolder = elm.value as int;
      }
    }

    return _bridge.initialize(
        cmdArgsLst, beCache, dbgModel, libraryPath, executableInBinFolder);
  }

  @override
  void terminate() => _bridge.terminate();
  void periodicTriggerSlow() => _bridge.periodic_trigger_slow();

  // Hold the global instance
  AudioFlowBackendBridge_ _bridge = AudioFlowBackendBridge_();
  // AudioFlowBackendBridge_ get beBridge => _instance;
  AudYoFloCompileFlags get compFlags => _bridge.theBeAdapter.compileFlags();

  @override
  String get lastError => _bridge.theBeAdapter.lastError;
}

// ==========================================================
// Derive this class from the Callback part and come back via
// overriden callbacks
// ==========================================================
class AudioFlowBackendBridge_ extends AudioFlowBackendBridgeCallbacks {
// ==========================================================
  // The dynamic library which is connected via ffi
  late ffi.DynamicLibrary dylib;

  // ==========================================================
  // Copies version of the runtime arguments
  List<String> cmdArgs = [];

  // Global debug switch
  // late FernliveBackendCache theBackendCache;
  AudYoFloBackendAdapterNative theBeAdapter = AudYoFloBackendAdapterNative();
  AudYoFloDebugModel? theDebugModel;

  // ===================================================
  // The bridge is a singleton that exists only once.
  // The singleton functionality is indicated by the "factory"
  // keyword - every additional call to the constructor returns the
  // same object.
  // ===================================================

  AudioFlowBackendBridge_._privateConstructor() {
    print("Creating <AudYoFloBackendBridge>.");
  }
  static final AudioFlowBackendBridge_ _instance =
      AudioFlowBackendBridge_._privateConstructor();
  factory AudioFlowBackendBridge_() {
    return _instance;
  }

  // ==================================================================
  // Startup the host bridge interface in ffi domain
  // ==================================================================

  Future<bool> initialize(
      List<String> cmdArgsLst,
      AudYoFloBackendCache beCache,
      AudYoFloDebugModel? dbgModel,
      String libraryPath,
      int executableInBinFolder) async {
    int result = 0;

    cmdArgs = cmdArgsLst;
    //theBackendCache = beCache;
    theDebugModel = dbgModel;

    if (!File(libraryPath).existsSync()) {
      print('Unable to find library <${libraryPath}>');
      assert(false);
    }

    debugPrint('Loading backend library <$libraryPath>');
    dylib = ffi.DynamicLibrary.open(libraryPath);
    natLib = AudioYoFloNative(dylib);

    // ===========================================================
    // Initialize the native functions to control the application
    // ===========================================================

    // Install the methodCall callback that reports specific aspects that
    // are not covered by flutter by default.
    // WindowsAyfNative.initialize(_fernNativeCallback);
    // ===========================================================

    // Initialize the DART API on the native side. In there, all
    // function to control dart part from within the native code are passed
    // the parameter NativeApi.initializeApiDLData
    final initializeApi = dylib.lookupFunction<
        ffi.IntPtr Function(ffi.Pointer<ffi.Void>),
        int Function(ffi.Pointer<ffi.Void>)>("InitDartApiDL");
    initializeApi(ffi.NativeApi.initializeApiDLData);

    // Install the native port
    final int nativePort = installNativePort();

    // Allocate the backend handle
    retOpaqueHandle retVal = natLib!.ffi_allocate_backend_handle();

    // Store the host handle
    opaque_host = retVal.opaque_hdl;

    // Cross-link the required system required
    theBeAdapter.initializeNative(
        natLib!, opaque_host, beCache, triggeredClose);
    theDebugModel?.initialize(beCache);

    // ====================================================================
    // Provide a list of command line arguments
    // ====================================================================
    List<String> lstArgv = [];
    String runExec = Platform.resolvedExecutable; //;Platform.script.path;
    lstArgv.add(runExec);
    int numEntries = cmdArgsLst.length;
    for (var i = 0; i < numEntries; i++) {
      print("Run Arg [ ${i} ] = ${cmdArgsLst[i]}");
      lstArgv.add(cmdArgsLst[i]);
    }

    // Copy into ffi C domain
    numEntries = lstArgv.length;
    ffi.Pointer<ffi.Pointer<ffi.Int8>> lstArgvC =
        calloc<ffi.Pointer<ffi.Int8>>(numEntries);
    for (var i = 0; i < numEntries; i++) {
      ffi.Pointer<Utf8> ptrU8 = lstArgv[i].toNativeUtf8();
      lstArgvC[i] = ptrU8.cast<ffi.Int8>();
    }

    // Allocate the struct and fill it with synchronous callback references
    ffi.Pointer<func_pointers> pointers = calloc<func_pointers>(1);
    fill_static_callbacks(pointers);

    // Call the initialize function and pass the callback struct
    result = natLib!.ffi_host_initialize(opaque_host, lstArgvC, numEntries,
        pointers, nativePort, executableInBinFolder);

    // Free list of arguments
    calloc.free(lstArgvC);

    // ===========================================================
    // Initialize the native part
    // ===========================================================

    // The following sets the min/max pixel size and the maximized
    // start of the window
    // initializeNative();

    // ===========================================================
    return true;
  }

  // ============================================================
  // Terminate the ffi backend. I think this function is not called
  // since there is just not termination signal
  // ============================================================
  void terminate() {
    int result = 0;
    if (opaque_host != ffi.nullptr) {
      result = natLib!.ffi_host_terminate(opaque_host);
      result = natLib!.ffi_deallocate_backend_handle(opaque_host);
      opaque_host = ffi.nullptr;
    }
    if (dylib != null) {
      // DynamicLibrary.close() <- does not exist at the moment :-(
    }
  }

  // ===========================================================
  // Some native function implementations
  // ===========================================================
  //
  //Run functions to setup window resolution etc
  /*
  Future initializeNative() async {
    Size size = await DesktopWindow.getWindowSize();
    print(size);
    await DesktopWindow.setWindowSize(Size(500, 500));

    await DesktopWindow.setMinWindowSize(Size(800, 800));
    await DesktopWindow.setMaxWindowSize(Size(800, 800));

    await DesktopWindow.resetMaxWindowSize();
    await DesktopWindow.toggleFullScreen();
    bool isFullScreen = await DesktopWindow.getFullScreen();
    await DesktopWindow.setFullScreen(true);
    isFullScreen = await DesktopWindow.getFullScreen();
    await DesktopWindow.setFullScreen(true);
  }
*/
  Future<void> triggeredClose() async {
    jvxSequencerStatusEnum statusSeq =
        jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;

    statusSeq = await theBeAdapter.updateStatusSequencer();

    if (statusSeq != jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE) {
      assert(theBeAdapter.completeSequencer != null);

      // Wait for sequencer to complete
      await theBeAdapter.triggerStopSequencer();

      // Wait for sequencer to complete
      await theBeAdapter.completeSequencer?.future;

      // Remove completer
      theBeAdapter.completeSequencer = null;
    }

    // Finalize the shutdown
    finalizeShutdown();
  }

  // Callback in which the close event is reported to the flutter host in windows
  /*
  Future<dynamic> _fernNativeCallback(MethodCall methodCall) async {
    String command = methodCall.method;
    var args = methodCall.arguments;
    Map valueMap = json.decode(args);
    if (command == 'trigger_wm_close') {
      jvxSequencerStatusEnum statusSeq =
          jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;

      statusSeq = theBeAdapter.statusSequencer();

      if (statusSeq != jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE) {
        theBeAdapter.queuedOperations
            .add(jvxFernliveCommandQueueElement.FERNLIVE_SHUTDOWN);
        theBeAdapter.triggerStopSequencer();
      } else {
        finalizeShutdown();
      }

      // At the moment, I have no clue how to pass the result back to
      // the caller. Somehow, there is no docs on this call direction.
      return 123;
    }
  }
  */

  void finalizeShutdown() {
    terminate();
    // WindowsAyfNative.acknowldge_wm_close('wm_close_ok');
  }

  //========================================================================
  // Overriden callback functions to report messages
  //========================================================================
  void processAsyncCommand(
      jvxReportCommandRequestEnum reqCom,
      ffi.Pointer<ffi.Void> load_fld,
      int sz_fld,
      bool async,
      jvxReportCommandBroadcastTypeEnum bcEnum) {
    int uId = 0;
    String dbgTxt;
    bool dbgOut = false;
    bool? dbgOutL = theDebugModel?.dbgOut;
    if (dbgOutL != null) dbgOut = dbgOutL;

    JvxComponentIdentification cpId =
        AudioFlowBackendBridgeHelpers.decodeCpIdReqCommand(natLib!, load_fld);
    jvxStateSwitchEnum ss = jvxStateSwitchEnum.JVX_STATE_SWITCH_NONE;

    // =================================================================
    // =================================================================
    String txt = '';
    String bcText = bcEnum.txt;
    if (dbgOut) {
      if (async) {
        txt =
            'Incoming ASYNC command requested <${reqCom.txt}> from component <${cpId.txt}> - $bcText';
      } else {
        txt =
            'Incoming SYNC command requested <${reqCom.txt}> from component <${cpId.txt}> - $bcText';
      }
      theDebugModel?.addLineOut(txt);
    }
    // =================================================================
    // =================================================================

    switch (reqCom) {
      case jvxReportCommandRequestEnum.JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC:
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST:
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT_LIST:
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT:
        theBeAdapter.reportUpdateStatusComponent(cpId);
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED:
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE:
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES:
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_RESCHEDULE_MAIN:
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE:
        ffi.Pointer<ffi.Int8> ptrIdent =
            natLib!.ffi_req_command_decode_ident_allocate_char_array(load_fld);
        if (ptrIdent.address != 0) {
          ffi.Pointer<Utf8> ptrU8 = ptrIdent.cast<Utf8>();
          String ident = ptrU8.toDartString();
          natLib!.ffi_host_delete(ptrIdent.cast<ffi.Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
          if (dbgOut) {
            theDebugModel?.addLineOut(
                'Component was born: ${ident} component <${cpId.txt}>');
          }
        }
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE:
        ffi.Pointer<ffi.Int8> ptrIdent =
            natLib!.ffi_req_command_decode_ident_allocate_char_array(load_fld);
        if (ptrIdent.address != 0) {
          ffi.Pointer<Utf8> ptrU8 = ptrIdent.cast<Utf8>();
          String ident = ptrU8.toDartString();
          natLib!.ffi_host_delete(ptrIdent.cast<ffi.Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
          if (dbgOut) {
            theDebugModel?.addLineOut(
                'Component about to die: ${ident} component <${cpId.txt}>');
          }
        }
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_COMPONENT:
        ffi.Pointer<ffi.Int8> ptrIdent =
            natLib!.ffi_req_command_decode_ident_allocate_char_array(load_fld);
        if (ptrIdent.address != 0) {
          ffi.Pointer<Utf8> ptrU8 = ptrIdent.cast<Utf8>();
          String ident = ptrU8.toDartString();
          natLib!.ffi_host_delete(ptrIdent.cast<ffi.Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
          if (dbgOut) {
            theDebugModel?.addLineOut(
                'Component was born: ${ident} component <${cpId.txt}>');
          }
        }
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_COMPONENT:
        ffi.Pointer<ffi.Int8> ptrIdent =
            natLib!.ffi_req_command_decode_ident_allocate_char_array(load_fld);
        if (ptrIdent.address != 0) {
          ffi.Pointer<Utf8> ptrU8 = ptrIdent.cast<Utf8>();
          String ident = ptrU8.toDartString();
          natLib!.ffi_host_delete(ptrIdent.cast<ffi.Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
          if (dbgOut) {
            theDebugModel?.addLineOut(
                'Component was born: ${ident} component <${cpId.txt}>');
          }
        }
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED:
        uId = decodeUId(natLib!, load_fld);
        if (dbgOut) {
          theDebugModel?.addLineOut('Process connected with uid ${uId}');
        }
        theBeAdapter.addProcess(uId);
        theBeAdapter.requestUpdateProcessStatus();

        dbgTxt = theBeAdapter.debugPrintProcess(uId);
        theDebugModel?.addLineOutConvert(dbgTxt);
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED:
        uId = decodeUId(natLib!, load_fld);
        if (dbgOut) {
          theDebugModel
              ?.addLineOut('Process to be disconnected with uid ${uId}');
        }
        theBeAdapter.remProcess(uId);
        break;
      case jvxReportCommandRequestEnum.JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN:
        uId = decodeUId(natLib!, load_fld);
        if (dbgOut) {
          theDebugModel?.addLineOut('Tested chain with uid ${uId}');
        }
        theBeAdapter.requestUpdateProcessStatus();
        break;

      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH:
        ss = decodeSswitch(natLib!, load_fld);

        if (dbgOut) {
          theDebugModel?.addLineOut(
              'Component State Switch -> ${ss.txt} component <${cpId.txt}>');
        }
        switch (ss) {
          case jvxStateSwitchEnum.JVX_STATE_SWITCH_SELECT:
            theBeAdapter.reportSelectedComponent(cpId);
            break;
          case jvxStateSwitchEnum.JVX_STATE_SWITCH_UNSELECT:
            theBeAdapter.reportUnselectedComponent(cpId);
            break;
          default:

            // Indicate that the state changed
            theBeAdapter.reportStateSwitchComponent(cpId, ss);
            break;
        }
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE:
        // theBeAdapter.runOnConfigure(); <- move to SYSTEM READY
        break;

      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_SYSTEM_READY:
        theBeAdapter.runOnSystemReady();
        break;

      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_EVENT:
        if (opaque_host != ffi.nullptr) {
          var evPtr = natLib!.ffi_req_command_decode_sequencer_event(load_fld);
          int status = natLib!.ffi_sequencer_status(opaque_host);
          jvxSequencerStatusEnum seqStat =
              jvxSequencerStatusEEnum.fromInt(status);
          jvxSequencerEventType ev =
              jvxSequencerEventTypeNative.decodeSequencerEventType(evPtr);

          one_sequencer_event evRef = evPtr.ref;
          ffi.Pointer<ffi.Void> dPtr = evRef.description.cast<ffi.Void>();

          natLib!.ffi_host_delete(dPtr.cast<ffi.Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
          natLib!.ffi_host_delete(evPtr.cast<ffi.Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_ONE_SEQ_EVENT);

          if ((ev.eventMask &
                  jvxSequencerEventBitfield
                      .JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE) !=
              0) {
            // This event indicates a JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE
            //Replace completer if there is an old one
            theBeAdapter.completeSequencer = Completer<void>();
          }

          if ((ev.eventMask &
                  jvxSequencerEventBitfield
                      .JVX_SEQUENCER_EVENT_PROCESS_TERMINATED) !=
              0) {
            // This event indicates a JVX_SEQUENCER_EVENT_PROCESS_TERMINATED
            // Signal the completer
            theBeAdapter.completeSequencer?.complete(null);
          }

          int errCode = theBeAdapter.processSeqEvent(ev, seqStat);
          if (errCode != jvxErrorType.JVX_NO_ERROR) {
            print('Error reported while reporting sequencer event!');
          }
          /*
          if (whattodo == jvxFernliveCommandQueueElement.FERNLIVE_SHUTDOWN) {
            finalizeShutdown();
          }
          */
        } else {
          print('Nullptr in call');
        }
        // If the
        //theBeAdapter.triggerUpdateProcessStatus();
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_CALLBACK:
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_DISCONNECT_COMPLETE:
        theBeAdapter.requestUpdateProcessStatus();
        break;

      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY:
        ffi.Pointer<ffi.Int8> ptrIdent =
            natLib!.ffi_req_command_decode_ident_allocate_char_array(load_fld);
        if (ptrIdent.address != 0) {
          ffi.Pointer<Utf8> ptrU8 = ptrIdent.cast<Utf8>();
          String propLst = ptrU8.toDartString();
          natLib!.ffi_host_delete(ptrIdent.cast<ffi.Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
          theBeAdapter.reportPropertyListSetBackend(cpId, propLst);
        }
        break;
      case jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_COMPONENT_STATESWITCH:
        ffi.Pointer<ffi.Int8> ptrIdent =
            natLib!.ffi_req_command_decode_ident_allocate_char_array(load_fld);
        if (ptrIdent.address != 0) {
          ffi.Pointer<Utf8> ptrU8 = ptrIdent.cast<Utf8>();
          String compName = ptrU8.toDartString();
          natLib!.ffi_host_delete(ptrIdent.cast<ffi.Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
          ss = decodeSswitch(natLib!, load_fld);
          theBeAdapter.triggerComponentStateSwitch(ss, compName, cpId);
          // theBeAdapter.triggerActivateComponent(cpId, id, select)
          // cpId

          dbgPrint('Tracked request to switch state');
        }

        break;
    }
  }

  void periodic_trigger_slow() {
    jvxSequencerStatusEnum seq_stat =
        jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;
    if (opaque_host != ffi.nullptr) {
      seq_stat = jvxSequencerStatusEEnum
          .fromInt(natLib!.ffi_sequencer_status(opaque_host));

      if (seq_stat != jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE) {
        natLib!.ffi_sequencer_trigger(opaque_host);
      }
      natLib!.ffi_periodic_trigger(opaque_host);
    }
  }
}
