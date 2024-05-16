//import 'dart:html';
import 'package:fixnum/fixnum.dart' as fn;
import 'package:flutter/cupertino.dart';
import 'dart:convert';
import 'package:collection/collection.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:http/http.dart' as http;
import '../ayf_web_backend_constants.dart';
import '../ayf_web_backend_typedefs.dart';
import 'ayf_web_backend_message_processor.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

import '../ayf_web_backend_helpers.dart';
import './ayf_web_backend_anomalies.dart';

import 'dart:async';
import 'dart:io';

// ===========================================================================
// ===========================================================================
// ===========================================================================
class AyfHtmlAudioHost extends AyfHost {
  // Allocate the audio host with instance Native Audio Host
  static AyfHost _instance = AyfHtmlAudioHost();
  static AyfHost get instance => _instance;

  List<String> cmdArgs = [];
  @override
  String descriptionHost() {
    return 'AyfHtmlAudioHost';
  }

  @override
  Future<bool> initialize(AudYoFloBackendCache beCache,
      AudYoFloDebugModel? dbgModel, Map<String, dynamic> args) {
    String addrTo = '127.0.0.1';
    int portId = 80;
    var elm =
        args.entries.firstWhereOrNull((element) => element.key == 'cmdArgs');
    if (elm != null) {
      if (elm.value is List<String>) {
        cmdArgs = elm.value as List<String>;
      }
      for (var idx = 0; idx < cmdArgs.length; idx++) {
        if (cmdArgs[idx] == '--connect-addr') {
          idx++;
          if (idx < cmdArgs.length) {
            addrTo = cmdArgs[idx];
          }
          continue;
        }
        if (cmdArgs[idx] == '--connect-port') {
          idx++;
          if (idx < cmdArgs.length) {
            int? portIdScan = int.tryParse(cmdArgs[idx]);
            if (portIdScan != null) {
              portId = portIdScan;
            }
          }
          continue;
        }
      }
    }
    return _adapter.initialize(beCache, dbgModel, addrTo, portId);
  }
  // _bridge.initialize(
  //     cmdArgsLst, beCache, dbgModel, libraryPath, executableInBinFolder);

  @override
  void terminate() => _adapter.terminate();
  void periodicTriggerSlow() => _adapter.periodic_trigger_slow();

  // Hold the global instance
  AudYoFloBackendAdapterWeb _adapter = AudYoFloBackendAdapterWeb();
  // AudioFlowBackendBridge_ get beBridge => _instance;
  AudYoFloCompileFlags get compFlags => _adapter.compileFlags();

  @override
  String get lastError => _adapter.latestError;
}

typedef Future<void> shutdownFunction();

class AudYoFloBackendAdapterWeb extends AudYoFloBackendAdapterIf
    with
        AudYoFloWebStateProcessor,
        AudYoFloWebBackendAnomalies,
        AudYoFloWebBackendHelpers {
  // ========================================================================
  // Variables
  // ========================================================================

  // ==============================================================
  int cfg_gran_state_report = 5;

  shutdownFunction? shutdown;

  // Retain a list of deferred actions to be run on certain conditions, e.g.,
  // to trigger shutdown of the app.
  List<AudYoFloCommandQueueElement> queuedOperations = [];

  ayfBackendAccessEnum accBackend = ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI;
  //ayfBackendAccessEnum accBackend =
  //    ayfBackendAccessEnum.JVX_ACCESS_BACKEND_JSON;
  String latestError = '';

  Completer<void>? completeWebSocketTransaction;

  WebSocketChannel? channel;

  int msgId = 1;

  String contextPending = '';

  String httpTarget = '';
  String wsTarget = '';

  AudYoFloDebugModel? dbgModel;
  bool dbgOut = true;
  AudYoFloBackendCache? theBeCache;
  // ==============================================================
  // ========================================================================
  // ========================================================================

  // ========================================================================
  // Coupling to host functions
  // ========================================================================
  void periodic_trigger_slow() {}

  void terminate() {}

  Future<bool> initialize(AudYoFloBackendCache beCache,
      AudYoFloDebugModel? dbgModelArg, String addrTo, int portId) async {
    bool retVal = false;
    latestError = '';

    /*
    String val = "{\"req\": \"cmdreq-rep-seq-evt\", \"type\": \"cmdreqtp-seq\""
        ", \"origin\": "
        "{"
        "\"component_identification\": \"unknown<JVX_SIZE_UNSELECTED,JVX_SIZE_UNSELECTED>\", "
        "\"uid\": -1"
        "}, "
        "\"specific\": "
        "{"
        "\"seqev-descr\": \"Startup of sequencer thread successfully completed\", "
        "\"seqev-qtp\": \"queue_type_none\", "
        "\"seqev-etp\": \"none\", "
        "\"seqev-stpid\": -1, "
        "\"seqev-seqid\": -1, "
        "\"seqev-stpfid\": -1, "
        "\"seqev-ferror\": \"yes\", "
        "\"seqev-state\": 0"
        "}"
        "}";
    Map<String, dynamic> mm = jsonDecode(val);
    */

    dbgModel = dbgModelArg;
    theBeCache = beCache;

    // Here, we start the http connection
    theBeCache!.initializeWithBackendAdapterReference(this);

    // Assign backend cache in base class -
    // initializeWithBackendCacheReference(beCache);

    // Set parent, report and helper reference
    initializeWebProcessor(this, this, this);

    // Set
    initializeHelper(this);

    // Link the debug model
    dbgModel?.initialize(beCache);

    // Start HTML connection and web socket
    // First command: curl 127.0.0.1:8000
    httpTarget = addrTo + ':' + portId.toString();
    String address = '/jvx/host/system/version';
    var url = Uri.http(httpTarget, address);

    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    int errCode = await webRequestHttpGet(url, urlRequest);
    latestError = urlRequest.lastError;
    if (errCode == jvxErrorType.JVX_NO_ERROR) {
      retVal = await processResponseState(
          jvxWebHostOperationResponseType.JVX_RESPONSE_INIT_CONNECT,
          urlRequest.jsonMap,
          null);
    } else {
      retVal = false;
    }

    if (retVal == true) {
      if (connectState ==
          jvxWebHostConnectionState.JVX_CONECTION_FIRST_CONTACT_DONE) {
        // Start the Websocket connection
        // Web socket connect
        wsTarget = 'ws://' + addrTo + ':' + portId.toString();
        address = '/jvx/host/ws/';
        try {
          channel = WebSocketChannel.connect(Uri.parse(wsTarget + address));
        } catch (exc) {
          print('$exc');
        }

        channel!.stream.listen(onMessage,
            onDone: onConnectionDone, onError: onConnectionError);

        /*
       * Create your own Future to map callbacks into await statements:
       * https://api.dart.dev/stable/2.10.5/dart-async/Completer-class.html
       * */
        contextPending = 'AYF_WS_HOST_CONNECT:' + msgId.toString();
        msgId++;

        String handshake = 'AYF_WS_HOST_CONNECT';
        if (contextPending.isNotEmpty) {
          handshake += '#' + contextPending;
        }

        completeWebSocketTransaction = Completer<void>();
        channel!.sink.add(handshake);
      }
      await completeWebSocketTransaction!.future;
    }
    return retVal;
  }

  @override
  void reportStatusDisconnect(int idReason, int subId, String description) {
    super.reportStatusDisconnect(idReason, subId, description);
    latestError = 'Websocket was diconnected.';
    if (theBeCache!.requestSystemDisconnect != null) {
      theBeCache!.requestSystemDisconnect!.reportEvent(latestError);
    }
  }

  void onMessage(dynamic body) async {
    String retBody = body;
    ExtractedFromJson extracted = ExtractedFromJson();
    // print('onMessage: $retBody');

    var jsonMap = jsonDecode(retBody);
    await processResponseState(
        jvxWebHostOperationResponseType.JVX_MESSAGE_WEBSOCKET,
        jsonMap,
        extracted);

    if (extracted.ctxt.isNotEmpty) {
      if (extracted.ctxt == contextPending) {
        if (!completeWebSocketTransaction!.isCompleted) {
          completeWebSocketTransaction!.complete(null);
        }
      }
    }
  }

  void onConnectionDone() {
    print(
        'Ws connection reports a disconnect. Informing app to switch to disconnect status.');
    report!.reportStatusDisconnect(0, 0, 'Websocket disconnected');
  }

  void onConnectionError(error) {
    print('Ws connection reports a connection error, reason:  <$error>');
    report!.reportStatusDisconnect(1, 0, error);
  }

  // ========================================================================
  // ========================================================================
  // ========================================================================

  AudYoFloBackendAdapterWeb();

  @override
  AudYoFloCompileFlags compileFlags() {
    return flags;
  }

  // =========================================================
  @override
  Future<int> triggerStartSequencer() async {
    // curl -d "" -X POST 127.0.0.1:8000/jvx/host/sequencer/start

    int retVal = jvxErrorType.JVX_NO_ERROR;
    String cmd = '/jvx/host/sequencer/start';
    var url = Uri.http(httpTarget, cmd);

    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpPost(url, '', urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      String? errCodeExprPtr =
          getStringEntryValueMap(urlRequest.jsonMap, 'return_code');
      if (errCodeExprPtr != null) {
        // Convert error code
        String errCodeExpr = errCodeExprPtr;
        retVal = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
      }
    }
    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          2, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  @override
  Future<int> triggerStopSequencer() async {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    String cmd = '/jvx/host/sequencer/stop';
    var url = Uri.http(httpTarget, cmd);

    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpPost(url, '', urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      String? errCodeExprPtr =
          getStringEntryValueMap(urlRequest.jsonMap, 'return_code');
      if (errCodeExprPtr != null) {
        // Convert error code
        String errCodeExpr = errCodeExprPtr;
        retVal = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
      }
    }
    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          3, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  @override
  Future<int> triggerToggleSequencer() async {
    return await Future<int>.delayed(Duration.zero, () async {
      int errCode = jvxErrorType.JVX_NO_ERROR;

      // To be implemented!
      assert(false);
      return errCode;
    });
  }

  @override
  Future<jvxSequencerStatusEnum> updateStatusSequencer() async {
    jvxSequencerStatusEnum stat =
        jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;
    int retVal = jvxErrorType.JVX_NO_ERROR;
    String cmd = '/jvx/host/sequencer/status';
    var url = Uri.http(httpTarget, cmd);

    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpGet(url, urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      String? errCodeExprPtr =
          getStringEntryValueMap(urlRequest.jsonMap, 'return_code');
      if (errCodeExprPtr != null) {
        // Convert error code
        String errCodeExpr = errCodeExprPtr;
        retVal = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
      }
    }
    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          4, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return stat;
  }

  // ============================================================

  @override
  Future<String> transferTextCommand(String textArg, bool jsonReturn) async {
    String retStr = '';
    int retVal = await Future<int>.delayed(Duration.zero, () async {
      int errCode = jvxErrorType.JVX_NO_ERROR;

      // To be implemented!
      assert(false);
      return errCode;
    });

    // if(retVal) //
    return retStr;
  }

  /*
   * This function is called by the sequencer controller to acknowledge shutdown.
   * We do not need this in web connection but it is used in native host
   */ ///=========================================================================
  @override
  jvxSequencerStatusEnum acknowledgeSequencerStop() {
    // This resets the sequencer status in the frontend. In native code, the acknowledge
    // must be invoked here, in http, the acknowledge is done fully in the backend.
    return jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;
  }

  @override
  AudYoFloCommandQueueElement postSequencerStop() {
    // This function is called after the sequencer was terminated. We may
    // add functional elements in this queue in the future. Currently,
    // there is no function for this.
    AudYoFloCommandQueueElement ret =
        AudYoFloCommandQueueElement.AYF_COMMAND_QUEUE_NONE;
    return ret;
  }

  @override
  AudYoFloOneConnectedProcess allocateProcess(int uId) {
    var elm = pendingProcesses.entries
        .firstWhereOrNull((element) => element.key == uId);
    if (elm == null) {
      assert(false);
    }

    AudYoFloOneConnectedProcess retVal = elm!.value;
    pendingProcesses.remove(uId);
    return retVal;
  }

  /*
  @override
  int processSeqEvent(
      jvxSequencerEventType ev, jvxSequencerStatusEnum seqStat) {
    return theBeCache!.reportSequencerEvent(ev, seqStat);
  }
*/
  void addProcess(int uId) {
    return theBeCache!.addProcess(uId);
  }

  void remProcess(int uId) {
    return theBeCache!.remProcess(uId);
  }

  @override
  String debugPrintProcess(int uId) {
    return theBeCache!.debugPrintProcess(uId);
  }

  Future<int> triggerUpdateProcessStatus() async {
    // -> processSystemReady
    // -> reasonIfNot
    // from (IjvxDataConnections*)->ready_for_start(astr);
    // curl 127.0.0.1:8000/jvx/host/connections/ready
    int retVal = jvxErrorType.JVX_NO_ERROR;
    String cmd = '/jvx/host/connections/ready';
    var url = Uri.http(httpTarget, cmd);

    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpGet(url, urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      retVal = AudYoFloSequencerStatusFromJson.updateSequencerStatus(
          urlRequest.jsonMap, theBeCache!);
    }

    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          5, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  // =================================================================

  // Translate component type enum to component class
  @override
  jvxComponentTypeClassEnum lookupComponentClass(JvxComponentTypeEnum tp) {
    jvxComponentTypeClassEnum cpClass =
        jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE;
    var elm = lookupClassTypes.entries
        .firstWhereOrNull((element) => element.key == tp);
    if (elm != null) {
      cpClass = elm.value;
    } else {
      assert(false);
    }
    return cpClass;
  }

  // Here is a good explanation of the async/await functionality:
  // https://stackoverflow.com/questions/53249859/async-await-not-waiting-before-future-dart
  @override
  Future<int> triggerUpdateAvailComponents(JvxComponentIdentification cpId,
      List<AudYoFloOneComponentOptions> updateMe) async {
    //List<FerncastOneComponentOptions> updateMe = [];
    updateMe.clear();

    // This is the core function call to update the status of a component
    Future<int> updateComponents = Future(() async {
      // Convert the component identification into a string
      int errCode = jvxErrorType.JVX_NO_ERROR;
      String address = 'unknown';

      // Generate a text command
      String cmd = 'show(' + address + ');';
      int stat = jvxState.JVX_STATE_NONE;

      // Request command on backend (immediate emit)
      String retText = await transferTextCommand(cmd, true);

      // Process the returned result
      Map valueMap = json.decode(retText);

      // Extract the relevant information
      MapEntry? entry_ecode = valueMap.entries
          .firstWhereOrNull((element) => element.key == 'return_code');
      if (entry_ecode != null) {
        errCode = jvxErrorTypeEInt.fromStringSingle(entry_ecode.value);
      }

      // Next, return the load part for this command
      MapEntry? entry_load = valueMap.entries
          .firstWhereOrNull((element) => element.key == 'all_components');
      if (entry_load != null) {
        for (Map singleSection in entry_load.value) {
          String name = 'Unknown';
          String state = 'none';

          MapEntry? singleEntryNm = singleSection.entries
              .firstWhereOrNull((element) => element.key == 'description');
          if (singleEntryNm != null) {
            name = singleEntryNm.value;
          }
          MapEntry? singleEntryState = singleSection.entries
              .firstWhereOrNull((element) => element.key == 'state');
          if (singleEntryState != null) {
            state = singleEntryState.value;
            stat = jvxStateEInt.fromStringSingle(state);
          }
          AudYoFloOneComponentOptions newCom =
              AudYoFloOneComponentOptions(name: name, stat: stat);
          updateMe.add(newCom);
        }
      }
      // theBeCache.updateAvailableComponentList(cpId, availComps);

      return errCode;
    });
    return updateComponents;
  }

  // ===========================================================================
  // ACTIVATE COMPONENT ACTIVATE COMPONENT ACTIVATE COMPONENT ACTIVATE COMPONENT
  // ===========================================================================

  @override
  Future<int> triggerActivateComponent(JvxComponentIdentification cpId, int id,
      bool selectBeforeActivate) async {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    String address = translateCompType(cpId.cpTp);
    String cmd = '/jvx/host/components/' + address;

    // If selectBeforeActivate is true, the call to activate requires an
    // id argument. Otherwise we live with no id argujent but function will fail if
    // component is not yet SELECTED.
    cmd += "/activate";
    Map<String, String> queryParams = {};
    queryParams['slotid'] = cpId.slotid.toString();
    queryParams['slotsubid'] = cpId.slotsubid.toString();
    if (selectBeforeActivate) {
      queryParams['id'] = id.toString();
    }
    var url = Uri.http(httpTarget, cmd, queryParams);
    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpPost(url, '', urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      String? errCodeExprPtr =
          getStringEntryValueMap(urlRequest.jsonMap, 'return_code');
      if (errCodeExprPtr != null) {
        // Convert error code
        String errCodeExpr = errCodeExprPtr;
        retVal = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
      }
    }

    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          6, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }

    return retVal;
  }

  // If a new component was selected, we request the options of components at first and
  // then, select the component which was indeed selected.
  Future<String?> requestOptionsComponent(
      JvxComponentIdentification cpId) async {
    String? retVal;
    int errCode = jvxErrorType.JVX_NO_ERROR;
    String address = translateCompType(cpId.cpTp);
    String cmd = '/jvx/host/components/' + address;
    cmd += "/options";
    Map<String, String> queryParams = {};
    queryParams['slotid'] = cpId.slotid.toString();
    queryParams['slotsubid'] = cpId.slotsubid.toString();
    var url = Uri.http(httpTarget, cmd, queryParams);
    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    errCode = await webRequestHttpGet(url, urlRequest);
    latestError = urlRequest.lastError;
    if (errCode == jvxErrorType.JVX_NO_ERROR) {

      var lstOptions = getListValueMap(urlRequest.jsonMap, 'options');
      if (lstOptions != null) {
        for (var oneOption in lstOptions) {
          bool foundHere = false;
          var subslotlst = getListValueMap(oneOption, 'subslots');
          if (subslotlst != null) {
            for (var oneslotsubid in subslotlst) {
              if (oneslotsubid == cpId.slotsubid) {
                retVal = extractStringFromJson(oneOption, 'description');
                foundHere = true;
                break;
              }
            }
          } else {
            var slotlst = getListValueMap(oneOption, 'slots');
            if (slotlst != null) {
              for (var oneslotid in slotlst) {
                if (oneslotid == cpId.slotid) {
                  retVal = extractStringFromJson(oneOption, 'description');
                  foundHere = true;
                  break;
                }
              }
            }
          }
          if (foundHere) {
            break;
          }
        }
      }
    }

    if (errCode != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          7, 0, jvxErrorTypeEInt.toStringSingle(errCode));
    }
    return retVal;
  }
  // curl 127.0.0.1:8000/jvx/host/components/audio_device/options

  // ===================================================================
  // This function is in use if a process is reported later than in the first contact!
  // It will be called if a new process is reported and allocate a handle
  // for the new process. Then, it triggers the backend to obtain the path - hence, the
  // involved components in the chain.
  Future<int> requestInfoProcess(AudYoFloOneConnectedProcessWeb newProc) async {
    int retVal = jvxErrorType.JVX_NO_ERROR;

    // curl 127.0.0.1:8000?uid=12
    String cmd = '/jvx/host/connections/path';
    Map<String, String> queryParams = {};
    queryParams['uid'] = newProc.uId.toString();
    var url = Uri.http(httpTarget, cmd, queryParams);
    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpGet(url, urlRequest);
    latestError = urlRequest.lastError;

    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      var secConnections =
          getMapValueList(urlRequest.jsonMap, 'connection_processes');
      if (secConnections != null) {
        for (var elm in secConnections) {
          Map<String, dynamic> mp = elm;
          int uid = getIntEntryValueMap(mp, 'uid');
          AudYoFloOneConnectedProcessWeb newProcess =
              AudYoFloOneConnectedProcessWeb(uid, this, report!);
          createPendingProcessFromJson(newProcess, elm);
        }
      }
    }

    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          8, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }
  // ===========================================================================
  // DEACTIVATE COMPONENT DEACTIVATE COMPONENT DEACTIVATE COMPONENT DEACTIVATE COMPONENT
  // ===========================================================================

  @override
  Future<int> triggerDeactivateComponent(
      JvxComponentIdentification cpId, bool unselect) async {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    String address = translateCompType(cpId.cpTp);
    String cmd = '/jvx/host/components/' + address;
    if (unselect) {
      cmd += "/unselect";
    } else {
      cmd += "/deactivate";
    }
    Map<String, String> queryParams = {};
    queryParams['slotid'] = cpId.slotid.toString();
    queryParams['slotsubid'] = cpId.slotsubid.toString();
    var url = Uri.http(httpTarget, cmd, queryParams);
    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpPost(url, '', urlRequest);
    latestError = urlRequest.lastError;

    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          9, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  @override
  Future<int> triggerGetPropertyContentComponent(
      JvxComponentIdentification cpId, List<String> propDescrLst,
      {AyfPropertyReportLevel reportArg =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY}) async {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    // The update property list in cache is typically executed with a delay
    // since here is the point where we have to request each single property.
    // We are only allowed to notify the listeners if the result is not within
    // the build function.
    String address = translateCompType(cpId.cpTp);
    String cmd = '/jvx/host/components/' + address + '/multi';
    Map<String, String> queryParams = {};
    queryParams['slotid'] = cpId.slotid.toString();
    queryParams['slotsubid'] = cpId.slotsubid.toString();
    String multParam = '\[';
    int cnt = 0;
    for (; cnt < propDescrLst.length; cnt++) {
      if (cnt > 0) {
        multParam += ',';
      }
      multParam += propDescrLst[cnt];
    }
    multParam += '\]';
    queryParams['multi'] = multParam;
    var url = Uri.http(httpTarget, cmd, queryParams);
    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpGet(url, urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      Map? subSec = getMapValueMap(urlRequest.jsonMap, 'properties');
      if (subSec != null) {
        for (var elmS in propDescrLst) {
          Map? subSubSec = getMapValueMap(subSec, elmS);
          if (subSubSec == null) {
            // Property was not returned in message - would this really happen?
          } else {
            AudYoFloPropertyContainer? propUpdate =
                theBeCache?.referencePropertyInCache(cpId, elmS);
            if (propUpdate != null) {
              if (propUpdate is AudYoFloPropertyContentBackend) {
                int resL = AudYoFloPropertyContentFromJson
                    .updatePropertyContentFromJsonMap(propUpdate, subSubSec, theBeCache!.backendAdapterIf!);
              }
            } else {
              // Whatever the reason is to end up here..
            }
          }
        }
      }
    }
    theBeCache!.updatePropertyCacheCompleteNotify(cpId, report: reportArg);
    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          10, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  @override
  Future<int> triggerGetPropertyDescriptionComponent(
      JvxComponentIdentification cpId, List<String> propDescrLst,
      {AyfPropertyReportLevel reportArg =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_NO_REPORT, int numDigits = 2}) async {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    // The update property list in cache is typically executed with a delay
    // since here is the point where we have to request each single property.
    // We are only allowed to notify the listeners if the result is not within
    // the build function.
    String address = translateCompType(cpId.cpTp);
    String cmd = '/jvx/host/components/' + address + '/multi';
    Map<String, String> queryParams = {};
    queryParams['purpose'] = 'fullp|x';
    queryParams['slotid'] = cpId.slotid.toString();
    queryParams['slotsubid'] = cpId.slotsubid.toString();
    String multParam = '\[';
    int cnt = 0;
    for (; cnt < propDescrLst.length; cnt++) {
      if (cnt > 0) {
        multParam += ',';
      }
      multParam += propDescrLst[cnt];
    }
    multParam += '\]';
    queryParams['multi'] = multParam;
    var url = Uri.http(httpTarget, cmd, queryParams);
    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpGet(url, urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      Map? subSec = getMapValueMap(urlRequest.jsonMap, 'properties');
      if (subSec != null) {
        for (var elmS in propDescrLst) {
          Map? subSubSec = getMapValueMap(subSec, elmS);
          if (subSubSec == null) {
            // This case happens if the property does not exist!
            // We will not add the property to the list.

            //
          } else {
            // ==================================================
            // Find out if property is already in the cache
            // ==================================================
            AudYoFloPropertyContainer? propFromCache =
                theBeCache!.referencePropertyInCache(cpId, elmS);
            if (propFromCache != null) {
              if (propFromCache is AudYoFloPropertyContentBackend) {
                // ==================================================
                // An element already exists: update the property content
                // ==================================================
                int resL = AudYoFloPropertyContentFromJson
                    .updatePropertyDescriptionFromJson(cpId, this,
                        propFromCache, elmS, subSubSec, flags, true, numDigits);
                assert(resL == jvxErrorType.JVX_NO_ERROR);
              } else {
                assert(false);
              }
            } else {
              // ==================================================
              // Here, we have seen a new property!
              // ==================================================

              AudYoFloPropertyContentBackend updateProp =
                  AudYoFloPropertyContentFromJson.createPropertyFromJsonMap(
                      cpId, this, elmS, subSubSec, flags, numDigits);

              // ==================================================
              // Here, we add the property to the cache!
              // ==================================================
              int resL =
                  theBeCache!.updateEntrySinglePropertyCache(cpId, updateProp);
              if (resL != jvxErrorType.JVX_NO_ERROR) {
                assert(false);
              }
            }
          }
        }

        // ==================================================
        // Here, we report that property update was completed
        // ==================================================

        if ((retVal == jvxErrorType.JVX_NO_ERROR) &&
            ((report ==
                    AyfPropertyReportLevel
                        .AYF_FRONTEND_REPORT_SINGLE_PROPERTY) ||
                (report ==
                    AyfPropertyReportLevel
                        .AYF_FRONTEND_REPORT_COMPONENT_PROPERTY))) {
          theBeCache!
              .updatePropertyCacheCompleteNotify(cpId, report: reportArg);
        }
      }
    }
    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          11, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  @override
  Future<int> triggerSetProperties(
      JvxComponentIdentification cpId, List<String> propContentsArg,
      {bool invalidateProperty = true,
      int offset = 0,
      int num = -1,
      bool preventCallMultipleTimes = false,
      AyfPropertyReportLevel reportArg =
          AyfPropertyReportLevel.AYF_BACKEND_REPORT_COMPONENT_PROPERTY}) async {
    // List of properties
    int retVal = jvxErrorType.JVX_NO_ERROR;
    List<String> propContents = [];
    AyfBackendReportPropertySetEnum repEnum = AyfBackendReportPropertySetEnum
        .AYF_BACKEND_REPORT_SET_PROPERTY_INACTIVE;

    // We may reduce the number of "sets" to be issued
    if (preventCallMultipleTimes) {
      //print(
      //    'Entering <triggerSetProperties> with <preventCallMultipleTimes> true');

      for (var propDescr in propContentsArg) {
        AudYoFloPropertyContainer? propFromCache =
            theBeCache!.referencePropertyInCache(cpId, propDescr);
        if (propFromCache != null) {
          if (propFromCache is AudYoFloPropertyContentBackend) {
            if (propFromCache.setInProgress ==
                jvxPropertyProgressStates.JVX_PROPERTY_PROGRESS_NONE) {
              propFromCache.setInProgress =
                  jvxPropertyProgressStates.JVX_PROPERTY_PROGRESS_WAIT;
              propContents.add(propDescr);
            } else {
              //print(
              //    'Set for property <$propDescr> is skipped due to previous pending operation');
            }
          } else {
            assert(false);
          }
        }
      }
      if (propContents.isEmpty) {
        //print(
        //    'Leaving <triggerSetProperties> with <preventCallMultipleTimes> true');
        return jvxErrorType.JVX_NO_ERROR;
      } else {
        //print(
        //    'Running <triggerSetProperties> with <preventCallMultipleTimes> true');
      }
    } else {
      propContents = propContentsArg;
    }
    // ==============================================================

    if ((reportArg ==
            AyfPropertyReportLevel.AYF_BACKEND_REPORT_COMPONENT_PROPERTY) ||
        (reportArg ==
            AyfPropertyReportLevel
                .AYF_BACKEND_REPORT_COMPONENT_PROPERTY_COLLECT)) {
      repEnum = AyfBackendReportPropertySetEnum
          .AYF_BACKEND_REPORT_SET_PROPERTY_ACTIVE;
    }

    String address = translateCompType(cpId.cpTp);
    String cmd = '/jvx/host/components/' + address + '/multi';
    Map<String, String> queryParams = {};
    queryParams['slotid'] = cpId.slotid.toString();
    queryParams['slotsubid'] = cpId.slotsubid.toString();
    if (reportArg ==
        AyfPropertyReportLevel.AYF_BACKEND_REPORT_COMPONENT_PROPERTY_COLLECT) {
      queryParams['collect'] = 'yes';
    }
    if (repEnum ==
        AyfBackendReportPropertySetEnum
            .AYF_BACKEND_REPORT_SET_PROPERTY_ACTIVE) {
      queryParams['report'] = 'yes';
    }
    String multParam = '\[';
    int cnt = 0;
    for (; cnt < propContents.length; cnt++) {
      String onePropReq = propContents[cnt] + ',';
      AudYoFloPropertyContainer? propFromCache =
          theBeCache!.referencePropertyInCache(cpId, propContents[cnt]);
      if (propFromCache != null) {
        var valTxt = '';
        var valTxtPtr =
            AudYoFloPropertyContentToString.convertPropertyContentToString(
                propFromCache, true, offset: offset, num: num,
                precision: 4, );
        if (valTxtPtr != null) {
          valTxt = valTxtPtr;
        }
        //propertiesToString(propFromCache, offset, num);
        onePropReq += valTxt;
        if (offset > 0) {
          onePropReq += ',' + offset.toString();
        }
        onePropReq = '(' + onePropReq + ')';
      } else {
        assert(false);
      }
      if (cnt > 0) {
        multParam += ',';
      }
      multParam += onePropReq;
    }
    multParam += '\]';
    var url = Uri.http(httpTarget, cmd, queryParams);

    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpPost(url, multParam, urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      var returnedProps = getMapValueList(urlRequest.jsonMap, 'properties');
      if (returnedProps != null) {
        for (var elmP in returnedProps) {
          String? descriptor = extractStringFromJson(elmP, 'descriptor');
          String? errCodeExprPtr =
              getStringEntryValueMap(urlRequest.jsonMap, 'return_code');
          if ((descriptor != null) && (errCodeExprPtr != null)) {
            bool sucSet = false;
            // Convert error code
            String errCodeExpr = errCodeExprPtr;
            int retValLoc = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
            if (retValLoc == jvxErrorType.JVX_NO_ERROR) {
              for (var elmIP in propContents) {
                if (descriptor! == elmIP) {
                  sucSet = true;
                  break;
                }
              }
            }
            if (sucSet == false) {
              print(
                  'Setting property <$descriptor!> failed, reason: <$errCodeExpr>.');
            }
          } else {
            report!.reportStatusErrorProtocol(
                12, 1, jvxErrorTypeEInt.toStringSingle(retVal));
          }
        }
      }

      if (preventCallMultipleTimes) {
        for (var propDescr in propContentsArg) {
          AudYoFloPropertyContainer? propFromCache =
            theBeCache!.referencePropertyInCache(cpId, propDescr);
            if(propFromCache != null) {
              if (propFromCache is AudYoFloPropertyContentBackend) {
              if (propFromCache!.setInProgress ==
                      jvxPropertyProgressStates.JVX_PROPERTY_PROGRESS_WAIT) {
                    propFromCache.setInProgress =
                        jvxPropertyProgressStates.JVX_PROPERTY_PROGRESS_NONE;
                  }
        }
            }
        }
      }
      // If we use reportPropertyOnSet this will not be required
                  if (invalidateProperty) {
                    theBeCache!.invalidatePropertiesComponent(
                        cpId, propContents, true);
                  }
      if (reportArg !=
          AyfPropertyReportLevel.AYF_BACKEND_REPORT_COMPONENT_PROPERTY) {
        // If we use reportPropertyOnSet this will not be required
        theBeCache!.updatePropertyCacheCompleteNotify(cpId, report: reportArg);
        // theBeCacheNative.updatePropertiesInCacheComplete(report: report);
      }
    }
    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          12, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  // Trigger backend to update the list of properties in cache
  @override
  Future<int> triggerUpdatePropertyListComponent(
      JvxComponentIdentification cpId,
      {AyfPropertyReportLevel reportArg = AyfPropertyReportLevel
          .AYF_FRONTEND_REPORT_COMPONENT_PROPERTY}) async {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    // The update property list in cache is typically executed with a delay
    // since here is the point where we have to request each single property.
    // We are only allowed to notify the listeners if the result is not within
    // the build function.
    String address = translateCompType(cpId.cpTp);
    String cmd = '/jvx/host/components/' + address;
    Map<String, String> queryParams = {};
    queryParams['purpose'] = 'list';
    queryParams['slotid'] = cpId.slotid.toString();
    queryParams['slotsubid'] = cpId.slotsubid.toString();
    var url = Uri.http(httpTarget, cmd, queryParams);

    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpGet(url, urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      String? errCodeExprPtr =
          getStringEntryValueMap(urlRequest.jsonMap, 'return_code');
      if (errCodeExprPtr != null) {
        // Convert error code
        String errCodeExpr = errCodeExprPtr;
        retVal = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
      }
      if (retVal == jvxErrorType.JVX_NO_ERROR) {
        var subSec = getMapValueList(urlRequest.jsonMap, 'properties');
        List<String> lstProps = [];
        if (subSec != null) {
          for (var elmS in subSec) {
            if (elmS is String) {
              lstProps.add(elmS);
            }
          }
          theBeCache!.updatePropertyCacheListCompleteNotify(cpId, lstProps,
              report: reportArg);
        }
      }
    }

    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          13, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  @override
  Future<int> triggerUpdateDeviceList(JvxComponentIdentification cpId) async {
    // The update property list in cache is typically executed with a delay
    // since here is the point where we have to request each single property.
    // We are only allowed to notify the listeners if the result is not within
    // the build function.
    // /jvx/host/components/audio_technology/devices
    int retVal = jvxErrorType.JVX_NO_ERROR;
    // The update property list in cache is typically executed with a delay
    // since here is the point where we have to request each single property.
    // We are only allowed to notify the listeners if the result is not within
    // the build function.
    String address = translateCompType(cpId.cpTp);
    String cmd = '/jvx/host/components/' + address;
    cmd += "/devices";
    Map<String, String> queryParams = {};
    queryParams['slotid'] = cpId.slotid.toString();
    queryParams['slotsubid'] = cpId.slotsubid.toString();
    var url = Uri.http(httpTarget, cmd, queryParams);

    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpGet(url, urlRequest);
    latestError = urlRequest.lastError;
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      retVal = AudYoFloDevicelistFromJson.updateDeviceList(
          urlRequest.jsonMap, cpId, theBeCache!, theBeCache!.backendAdapterIf!);
    }

    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          14, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  @override
  Future<int> triggerUpdateComponentList(
      JvxComponentIdentification cpId, bool isDevice) async {
    // The update property list in cache is typically executed with a delay
    // since here is the point where we have to request each single property.
    // We are only allowed to notify the listeners if the result is not within
    // the build function.
    return await Future<int>.delayed(Duration.zero, () async {
      int errCode = jvxErrorType.JVX_NO_ERROR;

      // To be implemented!
      assert(false);
      return errCode;
    });
  }

  // =====================================================================
  // Api call to save current setup in config file
  // =====================================================================

  @override
  Future<int> triggerSaveConfig() async {
    // file(config, write)
    // curl -d "" -X POST 127.0.0.1:8000/jvx/host/system/file
    int retVal = jvxErrorType.JVX_NO_ERROR;
    // The update property list in cache is typically executed with a delay
    // since here is the point where we have to request each single property.
    // We are only allowed to notify the listeners if the result is not within
    // the build function.
    String cmd = '/jvx/host/system/file';
    var url = Uri.http(httpTarget, cmd);
    AudYoFloUrlRequest urlRequest = AudYoFloUrlRequest();
    retVal = await webRequestHttpPost(url, '', urlRequest);
    latestError = urlRequest.lastError;
    if (retVal != jvxErrorType.JVX_NO_ERROR) {
      report!.reportStatusErrorProtocol(
          15, 0, jvxErrorTypeEInt.toStringSingle(retVal));
    }
    return retVal;
  }

  @override
  void reportPropertyListSetBackend(
      JvxComponentIdentification cpId, String propLst) {
    theBeCache!.reportPropertyListSetBackend(cpId, propLst);
  }

  @override
  Future<void> triggerClose() async {
    if (shutdown != null) {
      await shutdown!();
    }
  }

  @override
  String get lastError => latestError;
}
