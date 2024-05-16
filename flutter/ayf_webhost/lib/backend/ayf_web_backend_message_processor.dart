import 'package:ayf_pack/ayf_pack.dart';
import 'dart:convert' as convert;
import '../ayf_web_backend_helpers.dart';
import '../ayf_web_backend_constants.dart';
import '../ayf_web_backend_typedefs.dart';
import './ayf_web_backend_adapter.dart';
import './ayf_web_backend_anomalies.dart';

class AudYoFloOneConnectedProcessWeb extends AudYoFloOneConnectedProcess {
  AudYoFloWebBackendHelpers helper;
  AudYoFloWebBackendAnomalies report;
  AudYoFloOneConnectedProcessWeb(int uId, this.helper, this.report)
      : super(uId);
  void fill(dynamic arg) {
    Map<String, dynamic> elm = arg as Map<String, dynamic>;
    String? descrStr = extractStringFromJson(elm, 'descriptor');
    // String? isfromruleStr = extractStringFromJson(elm, 'is_from_rule');
    // String? statusStr = extractStringFromJson(elm, 'status');
    // String? lasttestStr  = extractStringFromJson(elm, 'last_test');
    //String? readyStr = extractStringFromJson(elm, 'ready');
    var processDetails = getMapValueMap(elm, 'process_path');
    catId = getIntEntryValueMap(elm, 'category_id');
    if (descrStr != null) {
      nameProcess = descrStr!;
    }
    if (processDetails != null) {
      involved = AudYoFloOneComponentInProcessWeb(helper, report);
      involved!.fill(processDetails);
    }
  }
}

class AudYoFloOneComponentInProcessWeb extends AudYoFloOneComponentInProcess {
  AudYoFloWebBackendHelpers helper;
  AudYoFloWebBackendAnomalies report;
  AudYoFloOneComponentInProcessWeb(this.helper, this.report);

  @override
  void fill(dynamic arg) {
    Map<dynamic, dynamic> jsonMap = arg as Map<dynamic, dynamic>;

    String? cpIdStr =
        extractStringFromJson(jsonMap, 'component_identification');
    int uid = getIntEntryValueMap(jsonMap, 'uid');
    String? modNameStr =
        extractStringFromJson(jsonMap, 'component_identification');
    String? cpNameStr = extractStringFromJson(jsonMap, 'description');
    String? connNmStr = extractStringFromJson(jsonMap, 'context');
    var nextMap = getMapValueList(jsonMap, 'next');

    if ((cpIdStr != null) &&
        (modNameStr != null) &&
        (cpNameStr != null) &&
        (connNmStr != null)) {
      cpId = helper.translateStringComponentIdentification(cpIdStr, uid);
      nameComponent = cpNameStr;
      nameModule = modNameStr;
      nameConnector = connNmStr;
      if (nextMap != null) {
        for (var nComp in nextMap) {
          AudYoFloOneComponentInProcessWeb newInvolved =
              AudYoFloOneComponentInProcessWeb(helper, report);
          newInvolved.fill(nComp);
          attach(newInvolved);
        }
      }
    }
  }
}

class AudYoFloWebStateProcessor {
  AudYoFloWebBackendHelpers? helper;
  AudYoFloWebBackendAnomalies? report;

  AudYoFloCompileFlags flags = AudYoFloCompileFlags();

  String dataTypeToken = "jvxData";

  jvxWebHostConnectionState connectState =
      jvxWebHostConnectionState.JVX_CONECTION_NONE;
  String backendGitTag = 'unknown';
  String backendDatetag = 'unknown';
  AudYoFloBackendAdapterWeb? parentRef;
  // AudYoFloBackendCacheBectrlIf? _theBeCacheReference;

  Map<JvxComponentTypeEnum, jvxComponentTypeClassEnum> lookupClassTypes = {};

  // If a new process pops up, we need to store all information in a list
  // Then, later, the cache will allocate a new process handle and then reuse the
  // information
  Map<int, AudYoFloOneConnectedProcessWeb> pendingProcesses = {};

  void initializeWebProcessor(
      AudYoFloBackendAdapterWeb beArg,
      AudYoFloWebBackendHelpers helperArg,
      AudYoFloWebBackendAnomalies reportArg) {
    parentRef = beArg;
    dataTypeToken = "jvxData";
    connectState = jvxWebHostConnectionState.JVX_CONECTION_NONE;
    backendGitTag = 'unknown';
    backendDatetag = 'unknown';
    helper = helperArg;
    report = reportArg;
  }

  void terminateWebProcessor() {
    lookupClassTypes.clear();
    pendingProcesses.clear();
  }

  Future<bool> processResponseState(jvxWebHostOperationResponseType tp,
      Map<dynamic, dynamic> jsonResponse, ExtractedFromJson? extrRet) async {
    bool retVal = false;
    switch (connectState) {
      // This is the processing of the initial connection interaction
      case jvxWebHostConnectionState.JVX_CONECTION_NONE:
        int errCode = jvxErrorType.JVX_NO_ERROR;
        String? typeStr = extractStringFromJson(jsonResponse, "return_code");
        if (typeStr != null) {
          errCode = jvxErrorTypeEInt.fromStringSingle(typeStr);
          if (errCode == jvxErrorType.JVX_NO_ERROR) {
            var subMap = getMapValueMap(jsonResponse, "version");
            if (subMap != null) {
              var me_tag = extractStringFromJson(subMap, 'me_tag');
              var git_tag = extractStringFromJson(subMap, 'git_tag');
              var date_tag = extractStringFromJson(subMap, 'date_tag');
              var num32BitField = extractStringFromJson(subMap, 'num32BitFld');
              var dtTp = extractStringFromJson(subMap, 'dtTp');
              if (me_tag != null) {
                if (me_tag == 'jvx-rt-750619') {
                  if ((num32BitField != null) && (dtTp != null)) {
                    flags.compileNum32BitBitfield = int.parse(num32BitField);
                    flags.compileDataTypeSpec =
                        jvxDataTypeSpecEEnum.fromString(dtTp);
                    dataTypeToken += '(' + dtTp + ')';

                    if (git_tag != null) {
                      backendGitTag = git_tag;
                    }
                    if (date_tag != null) {
                      backendDatetag = date_tag;
                    }
                    connectState = jvxWebHostConnectionState
                        .JVX_CONECTION_FIRST_CONTACT_DONE;
                    retVal = true;
                  }
                }
              }
            }
          }
        }
        break;
      case jvxWebHostConnectionState.JVX_CONECTION_FIRST_CONTACT_DONE:

        // ==============================================================
        // Extract response with return_code and message context
        // ==============================================================

        int errCode = jvxErrorType.JVX_NO_ERROR;
        String? typeStr = extractStringFromJson(jsonResponse, "return_code");
        if (typeStr != null) {
          errCode = jvxErrorTypeEInt.fromStringSingle(typeStr);
        }
        if (extrRet != null) {
          typeStr = extractStringFromJson(jsonResponse, "call_context");
          if (typeStr != null) {
            extrRet.ctxt = typeStr;
          }
        }
        connectState = jvxWebHostConnectionState.JVX_CONECTION_WEBSOCKET_ON;

        // ==============================================================
        // Extract system status information
        // ==============================================================

        var secSystem = getMapValueMap(jsonResponse, 'system');
        if (secSystem != null) {
          // ===================================================================
          // Scan the involved components
          // ===================================================================

          var secComponents = getListValueMap(secSystem, 'components');
          if (secComponents != null) {
            // Build up component type class list at first
            for (var elm in secComponents) {
              var cpTpStr = extractStringFromJson(elm, 'component_type');
              var cpTpClassStr =
                  extractStringFromJson(elm, 'component_type_class');
              if ((cpTpStr != null) && (cpTpClassStr != null)) {
                JvxComponentTypeEnum cpTp =
                    helper!.translateStringComponentType(cpTpStr);
                jvxComponentTypeClassEnum cpCls =
                    helper!.translateStringComponentTypeClass(cpTpClassStr);
                lookupClassTypes[cpTp] = cpCls;
              }
            }

            // Find the active components
            for (var elm in secComponents) {
              var cpTpStr = extractStringFromJson(elm, 'component_type');
              if (cpTpStr != null) {
                JvxComponentTypeEnum cpTp =
                    helper!.translateStringComponentType(cpTpStr);
                var secActive = getListValueMap(elm, 'active_components');
                int slotId = 0;
                if (secActive != null) {
                  for (var elmA in secActive) {
                    reportOneComponent(elmA);

                    var cpCls = parentRef!.lookupComponentClass(cpTp);
                    if (cpCls ==
                        jvxComponentTypeClassEnum
                            .JVX_COMPONENT_TYPE_TECHNOLOGY) {
                      var secDevices = getListValueMap(elmA, 'devices');
                      if (secDevices != null) {
                        for (var elmD in secDevices) {
                          reportOneComponent(elmD);
                        }
                      }
                    }
                  }
                }
              }
            }
          }

          // =============================================================
          // =============================================================
          var secSequencer = getListValueMap(secSystem, 'sequencer');
          if (secSequencer != null) {
            for (var elm in secSequencer) {
              Map<String, dynamic> mp = elm;
              /*
              int uid = getIntEntryValueMap(mp, 'uid');
              AudYoFloOneConnectedProcessWeb newProcess =
                  AudYoFloOneConnectedProcessWeb(uid);
              createPendingProcessFromJson(newProcess, elm);
              */
            }
          }

          // =============================================================
          // =============================================================
          var secConnections =
              getListValueMap(secSystem, 'connection_processes');
          if (secConnections != null) {
            for (var elm in secConnections) {
              Map<String, dynamic> mp = elm;
              int uid = getIntEntryValueMap(mp, 'uid');
              AudYoFloOneConnectedProcessWeb newProcess =
                  AudYoFloOneConnectedProcessWeb(uid, helper!, report!);
              createPendingProcessFromJson(newProcess, elm);
            }
          }
        }
        break;
      case jvxWebHostConnectionState.JVX_CONECTION_WEBSOCKET_ON:
        if (tp == jvxWebHostOperationResponseType.JVX_MESSAGE_WEBSOCKET) {
          var reqStr = extractStringFromJson(jsonResponse, "req");
          if (reqStr != null) {
            // This message is a command request message!!
            jvxReportCommandRequestEnum reqTp =
                jvxReportCommandRequestEEnum.fromInt(parentRef!
                    .translateEnumString(reqStr, "jvxReportCommandRequest", flags));
            var typeStr = extractStringFromJson(jsonResponse, "type");
            var mapOrigin = getMapValueMap(jsonResponse, "origin");
            var mapSpecific = getMapValueMap(jsonResponse, "specific");
            if ((typeStr != null) && (mapOrigin != null)) {
              var cpIdStr =
                  extractStringFromJson(mapOrigin, "component_identification");
              var uidCp = getIntEntryValueMap(mapOrigin, "uid");

              if (cpIdStr != null) {
                String? identStr;
                String ident = 'unknown';
                int uidRep = -1;
                jvxStateSwitchEnum ssTp =
                    jvxStateSwitchEnum.JVX_STATE_SWITCH_NONE;
                JvxComponentIdentification cpId = helper!
                    .translateStringComponentIdentification(cpIdStr, uidCp);

                jvxReportCommandDataTypeEnum reqDtTp =
                    jvxReportCommandDataTypeEEnum.fromInt(parentRef!
                        .translateEnumString(
                            typeStr, "jvxReportCommandDataType", flags));
                String specTxt = "";
                if (mapSpecific != null) {
                  switch (reqDtTp) {
                    case jvxReportCommandDataTypeEnum
                        .JVX_REPORT_COMMAND_TYPE_IDENT:
                      identStr = extractStringFromJson(mapSpecific, "ident");
                      if (identStr != null) {
                        ident = identStr;
                      }
                      specTxt = "ident: $ident";
                      break;
                    case jvxReportCommandDataTypeEnum
                        .JVX_REPORT_COMMAND_TYPE_UID:
                      uidRep = getIntEntryValueMap(mapSpecific, "uid");
                      specTxt = "uId: $uidRep";
                      break;
                    case jvxReportCommandDataTypeEnum
                        .JVX_REPORT_COMMAND_TYPE_SS:
                      String? ssStr =
                          extractStringFromJson(mapSpecific, "sswitch");
                      if (ssStr != null) {
                        ssTp = jvxStateSwitchEEnum.fromInt(parentRef!
                            .translateEnumString(ssStr, "jvxStateSwitch", flags));
                      }
                      specTxt = ssTp.toString();
                      specTxt = "sswitch: $specTxt";
                      break;
                    default:
                      break;
                  }
                }
                String bcText = 'no-broadcast-info';
                if (parentRef!.dbgOut) {
                  String txt =
                      'Incoming ASYNC command requested <${reqTp.txt}> from component <${cpId.txt}> - $bcText - $specTxt';

                  parentRef!.dbgModel?.addLineOut(txt);
                  switch (reqTp) {
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC:
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST:
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT_LIST:
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT:
                      if (parentRef!.dbgOut) {
                        parentRef!.dbgModel?.addLineOut(
                            'Component <${cpId.txt}> requests to react to new status.');
                      }
                      parentRef!.theBeCache!.reportUpdateStatusComponent(cpId);
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
                      if (parentRef!.dbgOut) {
                        parentRef!.dbgModel?.addLineOut(
                            'Component was born: $ident component <${cpId.txt}>');
                      }
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE:
                      if (parentRef!.dbgOut) {
                        parentRef!.dbgModel?.addLineOut(
                            'Component about to die: $ident component <${cpId.txt}>');
                      }
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_COMPONENT:
                      if (parentRef!.dbgOut) {
                        parentRef!.dbgModel?.addLineOut(
                            'Component was born: $ident component <${cpId.txt}>');
                      }
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_COMPONENT:
                      if (parentRef!.dbgOut) {
                        parentRef!.dbgModel?.addLineOut(
                            'Component was born: $ident component <${cpId.txt}>');
                      }
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED:
                      if (parentRef!.dbgOut) {
                        parentRef!.dbgModel
                            ?.addLineOut('Process connected with uid $uidRep');
                      }
                      AudYoFloOneConnectedProcessWeb newProcess =
                          AudYoFloOneConnectedProcessWeb(
                              uidRep, helper!, report!);

                      int errCodeLocal =
                          await parentRef!.requestInfoProcess(newProcess);
                      if (errCodeLocal == jvxErrorType.JVX_NO_ERROR) {}
                      // Here, we require more info!!
                      /*
                      newProcess.descrStr =
                          extractStringFromJson(elm, 'descriptor');
                      newProcess.uid = uidRep;
                      newProcess.isfromruleStr =
                          extractStringFromJson(elm, 'is_from_rule');
                      newProcess.statusStr =
                          extractStringFromJson(elm, 'status');
                      newProcess.lasttestStr =
                          extractStringFromJson(elm, 'last_test');
                      newProcess.readyStr = extractStringFromJson(elm, 'ready');
                      newProcess.pathEntry =
                          getMapValueMap(elm, 'process_path');
                      */
                      if (newProcess.uId >= 0) {
                        pendingProcesses[newProcess.uId!] = newProcess;
                        parentRef!.theBeCache!.addProcess(uidRep);
                        parentRef!.triggerUpdateProcessStatus();

                        txt = parentRef!.debugPrintProcess(uidRep);
                        parentRef!.dbgModel?.addLineOutConvert(txt);
                      }
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED:
                      if (parentRef!.dbgOut) {
                        parentRef!.dbgModel?.addLineOut(
                            'Process to be disconnected with uid $uidRep');
                      }
                      parentRef!.theBeCache!.remProcess(uidRep);
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN:
                      if (parentRef!.dbgOut) {
                        parentRef!.dbgModel
                            ?.addLineOut('Tested chain with uid $uidRep');
                      }
                      parentRef!.theBeCache!.triggerUpdateProcessStatus();
                      break;

                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN_RUN:
                      break;

                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH:
                      if (parentRef!.dbgOut) {
                        parentRef!.dbgModel?.addLineOut(
                            'Component State Switch -> ${ssTp.txt} component <${cpId.txt}>');
                      }
                      switch (ssTp) {
                        case jvxStateSwitchEnum.JVX_STATE_SWITCH_SELECT:
                          String cpName = cpId.txt;
                          String? cpNameStr =
                              await parentRef!.requestOptionsComponent(cpId);
                          if (cpNameStr != null) {
                            cpName = cpNameStr;
                          }
                          parentRef!.theBeCache!
                              .reportSelectedComponent(cpId, cpName);
                          break;
                        case jvxStateSwitchEnum.JVX_STATE_SWITCH_UNSELECT:
                          parentRef!.theBeCache!
                              .reportUnselectedComponent(cpId);
                          break;
                        default:

                          // Indicate that the state changed
                          parentRef!.theBeCache!
                              .reportStateSwitchComponent(cpId, ssTp);
                          break;
                      }

                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE:
                      // runAutoStart();
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_EVENT:
                      if (mapSpecific != null) {
                        jvxSequencerEventType ev = jvxSequencerEventType();
                        jvxSequencerStatusEnum seqStat =
                            jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;
                        String? evDescrPtr =
                            extractStringFromJson(mapSpecific, 'seqev-descr');
                        String? seqQTypePtr =
                            extractStringFromJson(mapSpecific, 'seqev-qtp');
                        String? seqETypePtr =
                            extractStringFromJson(mapSpecific, 'seqev-etp');
                        int stpId =
                            getIntEntryValueMap(mapSpecific, 'seqev-stpid');
                        int seqId =
                            getIntEntryValueMap(mapSpecific, 'seqev-seqid');
                        int stpFId =
                            getIntEntryValueMap(mapSpecific, 'seqev-stpfid');
                        String? fErrorPtr =
                            extractStringFromJson(mapSpecific, 'seqev-ferror');
                        int seqEvStateId =
                            getIntEntryValueMap(mapSpecific, 'seqev-state_id');
                        String? evMaskPtr =
                            extractStringFromJson(mapSpecific, 'seqev-mask');
                        String? seqStatePtr =
                            extractStringFromJson(mapSpecific, 'seq-state');
                        if ((evDescrPtr != null) &&
                            (seqQTypePtr != null) &&
                            (seqETypePtr != null) &&
                            (fErrorPtr != null) &&
                            (seqStatePtr != null) &&
                            (evMaskPtr != null)) {
                          seqStat = jvxSequencerStatusEEnum.fromInt(parentRef!
                              .translateEnumString(
                                  seqStatePtr, 'jvxSequencerStatus', flags));

                          ev.description = evDescrPtr;
                          ev.fId = stpFId;
                          ev.qtp = jvxSequencerQueueTypeEEnum.fromInt(parentRef!
                              .translateEnumString(
                                  seqQTypePtr, 'jvxSequencerQueueType', flags));
                          ev.indicateFirstError = (fErrorPtr == "yes");
                          ev.eventMask = int.parse(evMaskPtr);
                          ev.seq_state_id = seqEvStateId;
                          ev.sequenceId = seqId;
                          ev.stepId = stpId;
                          ev.setp = jvxSequencerElementTypeEEnum.fromInt(
                              parentRef!.translateEnumString(
                                  seqETypePtr, 'jvxSequencerElementType', flags));
                        }

                        parentRef!.theBeCache!
                            .reportSequencerEvent(ev, seqStat);
                      }
                      /*
                  var evPtr =
                      natLib!.ffi_req_command_decode_sequencer_event(load_fld);
                  int status = natLib!.ffi_sequencer_status(opaque_host);
                  jvxSequencerStatusEnum seqStat =
                      jvxSequencerStatusEEnum.fromInt(status);
                  jvxSequencerEventType ev = jvxSequencerEventTypeNative
                      .decodeSequencerEventType(evPtr);

                  one_sequencer_event evRef = evPtr.ref;
                  ffi.Pointer<ffi.Void> dPtr =
                      evRef.description.cast<ffi.Void>();

                  natLib!.ffi_host_delete(dPtr.cast<ffi.Void>(),
                      ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
                  natLib!.ffi_host_delete(evPtr.cast<ffi.Void>(),
                      ffiDeleteDatatype.JVX_DELETE_DATATYPE_ONE_SEQ_EVENT);

                  jvxFernliveCommandQueueElement? whattodo =
                      theBeAdapter.processSeqEvent(ev, seqStat);
                  if (whattodo ==
                      jvxFernliveCommandQueueElement.FERNLIVE_SHUTDOWN) {
                    finalizeShutdown();
                  }

                  // If the
                  //theBeAdapter.triggerUpdateProcessStatus();
                  */
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_CALLBACK:
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_DISCONNECT_COMPLETE:
                      parentRef!.theBeCache!.triggerUpdateProcessStatus();
                      break;

                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY:
                      parentRef!.reportPropertyListSetBackend(cpId, ident);
                      break;
                    case jvxReportCommandRequestEnum
                        .JVX_REPORT_COMMAND_REQUEST_REPORT_TEST_SUCCESS:
                      break;
                  }
                }
              }
            }
          }
        }
        break;
      default:
        break;
    }
    return retVal;
  }

  void createPendingProcessFromJson(
      AudYoFloOneConnectedProcessWeb newProcess, Map<String, dynamic> elm) {
    newProcess.fill(elm);

    if (newProcess.uId >= 0) {
      // Store all data related to the process
      pendingProcesses[newProcess.uId] = newProcess;
      parentRef!.theBeCache!.addProcess(newProcess.uId);
      parentRef!.theBeCache!.triggerUpdateProcessStatus();
    }
  }

  void reportOneComponent(Map<dynamic, dynamic> elm) {
    var cpIdStr = extractStringFromJson(elm, 'component_identification');
    var descrStr = extractStringFromJson(elm, 'description');
    var stateStr = extractStringFromJson(elm, 'state');
    var uidStr = extractStringFromJson(elm, 'uid');
    if ((cpIdStr != null) &&
        (descrStr != null) &&
        (stateStr != null) &&
        (uidStr != null)) {
      int uid = int.parse(uidStr);

      // ====================================================
      // Report every single component to backend
      // ====================================================
      String txt = '';
      JvxComponentIdentification cpId =
          helper!.translateStringComponentIdentification(cpIdStr, uid);
      int stat = helper!.translateStringState(stateStr);
      if (stat >= jvxState.JVX_STATE_SELECTED) {
        txt = 'Initial connect: Reporting select component ' + cpId.txt;
        parentRef!.dbgModel?.addLineOut(txt);
        parentRef!.theBeCache!.reportSelectedComponent(cpId, descrStr);
      }
      if (stat >= jvxState.JVX_STATE_ACTIVE) {
        txt = 'Initial connect: Reporting state switch ' +
            jvxStateSwitchEnum.JVX_STATE_SWITCH_ACTIVATE.toString() +
            ', component ' +
            cpId.txt;
        parentRef!.dbgModel?.addLineOut(txt);
        parentRef!.theBeCache!.reportStateSwitchComponent(
            cpId, jvxStateSwitchEnum.JVX_STATE_SWITCH_ACTIVATE);
      }
      if (stat >= jvxState.JVX_STATE_PREPARED) {
        txt = 'Initial connect: Reporting state switch ' +
            jvxStateSwitchEnum.JVX_STATE_SWITCH_PREPARE.toString() +
            ', component ' +
            cpId.txt;
        parentRef!.dbgModel?.addLineOut(txt);
        parentRef!.theBeCache!.reportStateSwitchComponent(
            cpId, jvxStateSwitchEnum.JVX_STATE_SWITCH_PREPARE);
      }
      if (stat >= jvxState.JVX_STATE_PROCESSING) {
        txt = 'Initial connect: Reporting state switch ' +
            jvxStateSwitchEnum.JVX_STATE_SWITCH_START.toString() +
            ', component ' +
            cpId.txt;
        parentRef!.dbgModel?.addLineOut(txt);
        parentRef!.theBeCache!.reportStateSwitchComponent(
            cpId, jvxStateSwitchEnum.JVX_STATE_SWITCH_START);
      }
    }
  }
}
