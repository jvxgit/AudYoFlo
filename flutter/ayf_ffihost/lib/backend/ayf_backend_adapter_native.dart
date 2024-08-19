import 'dart:ffi';
import 'dart:math';
import 'dart:typed_data';
import 'dart:async';

//import 'dart:html';
import 'package:flutter/cupertino.dart';

import '../properties/ayf_properties_from_native.dart';
import 'package:ffi/ffi.dart';
import 'dart:convert';
import 'package:collection/collection.dart';
import 'package:fixnum/fixnum.dart' as fn;

import 'package:ayf_pack/ayf_pack.dart';
import 'package:ayf_pack_native/ayf_pack_native.dart';

import '../native-ffi/generated/ayf_ffi_gen_bind.dart';
import '../systemstate/ayf_connected_processes.dart';
import './ayf_backend_adapter.dart';

typedef Future<void> shutdownFunction();

class AudYoFloNativePropertyHandle {
  JvxComponentIdentification cpId = JvxComponentIdentification();
  AudioYoFloNative natLib;
  Pointer<one_property_comp> propRef = nullptr;
  Pointer<Void> opaqueHost;

  AudYoFloNativePropertyHandle(
      {required this.cpId,
      required this.propRef,
      required this.natLib,
      required this.opaqueHost});
}

class AudYoFloNativeHandles {
  AudYoFloNativePropertyHandle? prop;
  Pointer<component_ident> ident;
  AudYoFloNativeHandles({required this.ident});
}

class AudYoFloBackendAdapterNative extends AudYoFloBackendAdapterIf
    with AudYoFloBackendAdapter {
  int cfg_gran_state_report = 5;
  late AudioYoFloNative natLib;
  Pointer<Void> opaque_host = nullptr;

  AudYoFloCompileFlags flags = AudYoFloCompileFlags();
  late AudYoFloBackendCacheBectrlIf theBeCacheNative;
  shutdownFunction? shutdown;

  String latestError = '';

  Completer<void>? completeSequencer;

  // Retain a list of deferred actions to be run on certain conditions, e.g.,
  // to trigger shutdown of the app.
  List<AudYoFloCommandQueueElement> queuedOperations = [];

  ayfBackendAccessEnum accBackend = ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI;
  //ayfBackendAccessEnum accBackend =
  //    ayfBackendAccessEnum.JVX_ACCESS_BACKEND_JSON;
  // ==============================================================

  AudYoFloBackendAdapterNative();

  @override
  String get lastError => latestError;

  //! Return the currently detected compile flags
  @override
  AudYoFloCompileFlags compileFlags() {
    return flags;
  }

  void initializeNative(
      AudioYoFloNative ptrNatlib,
      Pointer<Void> opaque_host_hdl,
      AudYoFloBackendCache theBeCacheArg,
      shutdownFunction shutdownArg) {
    natLib = ptrNatlib;
    opaque_host = opaque_host_hdl;

    // Set cross references
    theBeCacheArg.initializeWithBackendAdapterReference(this);

    // Extract interface interface
    theBeCacheNative = theBeCacheArg;

    // Pass reference for backward cross referencing
    super.initializeWithBackendCacheReference(theBeCacheNative);
    shutdown = shutdownArg;

    // Find out how the backend has been compiled
    Pointer<Int32> dataFormatSpecPtr = calloc<Int32>();
    Pointer<Int32> numSFieldsBitFieldPtr = calloc<Int32>();
    int res = natLib.ffi_get_compile_flags(
        opaque_host_hdl, dataFormatSpecPtr, numSFieldsBitFieldPtr);
    flags.compileNum32BitBitfield = numSFieldsBitFieldPtr.elementAt(0).value;
    flags.compileDataTypeSpec =
        jvxDataTypeSpecEEnum.fromInt(numSFieldsBitFieldPtr.value);
    calloc.free(dataFormatSpecPtr);
    calloc.free(numSFieldsBitFieldPtr);
    assert(res == jvxErrorType.JVX_NO_ERROR);

    int reportSet = 1;
    /*
     * This can no longer be chosen, property changes should always be reported.
     * HK, 24.02.2023
     * 
    AudYoFloBackendConfig cfg = theBeCacheArg.config;
    int reportSet = 0;
    if (cfg.reportPropertyOnSet) {
      reportSet = 1;
    }
    */

    int reportSyncDirect = 1;

    /*
     * This can no longer be chosen, sync message should never be reported 
     * through the flutter code as this can not be realized in the plugin.
     * Reporting direct means: always bypass Flutter code.
     * HK, 24.02.2023
     * 
    if (cfg.reportSyncDirect) {
      reportSyncDirect = 1;
    }
    */
    res = natLib.ffi_set_operation_flags(
        opaque_host_hdl, reportSet, reportSyncDirect);
    assert(res == jvxErrorType.JVX_NO_ERROR);
  }

  // =========================================================
  @override
  Future<int> triggerStartSequencer() async {
    int res = natLib.ffi_sequencer_start(opaque_host, cfg_gran_state_report);
    if (res == jvxErrorType.JVX_NO_ERROR) {
      // Run the first sequencer step immediately!!
      res = natLib.ffi_sequencer_trigger(opaque_host);
    }
    return res;
  }

  @override
  Future<int> triggerStopSequencer() async {
    int res = natLib.ffi_sequencer_stop(opaque_host);
    if (res == jvxErrorType.JVX_NO_ERROR) {
      // Run the first sequencer step immediately!!
      res = natLib.ffi_sequencer_trigger(opaque_host);

      // Wait for sequencer to complete
      if (completeSequencer != null) {
        await completeSequencer?.future;

        // Remove completer
        completeSequencer = null;
      }
    }
    return res;
  }

  @override
  Future<int> triggerToggleSequencer() async {
    jvxSequencerStatusEnum stat = await updateStatusSequencer();

    if (stat == jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE) {
      triggerStartSequencer();
    } else {
      triggerStopSequencer();
    }
    return jvxErrorType.JVX_NO_ERROR;
  }

  @override
  Future<jvxSequencerStatusEnum> updateStatusSequencer() async {
    jvxSequencerStatusEnum stat =
        jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;
    if (opaque_host != nullptr) {
      int statv = natLib.ffi_sequencer_status(opaque_host);
      stat = jvxSequencerStatusEEnum.fromInt(statv);
    }
    return stat;
  }

  // ============================================================

  @override
  Future<String> transferTextCommand(String textArg, bool jsonReturn) async {
    String retStr = '';
    int arg = 0;
    if (jsonReturn) arg = 1;
    Pointer<Utf8> ptrU8 = textArg.toNativeUtf8();
    Pointer<Int8> cmd = ptrU8.cast<Int8>();
    Pointer<Int8> textRet = natLib.ffi_transfer_command(opaque_host, cmd, arg);
    ptrU8 = textRet.cast<Utf8>();
    if (ptrU8 != null) {
      retStr = ptrU8.toDartString();
      natLib.ffi_host_delete(textRet.cast<Void>(),
          ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
    }
    return retStr;
  }

  String translateComponent(JvxComponentIdentification elm) {
    String ret = 'None';

    // This function does not access the list of handles as it may request the
    // description of a non-active component
    Pointer<component_ident> cpId = calloc<component_ident>(1);
    component_ident cpIdRef = cpId.ref;
    cpIdRef.tp = elm.cpTp.index;
    cpIdRef.slotid = elm.slotid;
    cpIdRef.slotsubid = elm.slotsubid;
    cpIdRef.uId = elm.uid;

    Pointer<Int8> descrPtr =
        natLib.ffi_component_description(opaque_host, cpId);
    if (descrPtr != nullptr) {
      Pointer<Utf8> dPtr = descrPtr.cast<Utf8>();
      String oneEntry = dPtr.toDartString();
      ret = oneEntry;
      natLib.ffi_host_delete(
          dPtr.cast<Void>(), ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
    }

    calloc.free(cpId);
    return ret;
  }

  @override
  int translateEnumString(
      String selection, String formatName, AudYoFloCompileFlags flags) {
    Pointer<Utf8> selPtr = selection.toNativeUtf8();
    Pointer<Utf8> formPtr = formatName.toNativeUtf8();
    return natLib.ffi_translate_enum_string(
        opaque_host, formPtr.cast<Int8>(), selPtr.cast<Int8>());
  }

  /*
   * This function is called by the sequencer controller to acknowledge shutdown.
   * We do not need this in web connection but it is used in native host
   */ ///=========================================================================
  @override
  jvxSequencerStatusEnum acknowledgeSequencerStop() {
    jvxSequencerStatusEnum statOnLeave =
        jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;
    natLib.ffi_sequencer_ack_stop(opaque_host);
    statOnLeave = jvxSequencerStatusEEnum
        .fromInt(natLib.ffi_sequencer_status(opaque_host));
    return statOnLeave;
  }

  @override
  AudYoFloCommandQueueElement postSequencerStop() {
    AudYoFloCommandQueueElement ret =
        AudYoFloCommandQueueElement.AYF_COMMAND_QUEUE_NONE;
    /*
    if (queuedOperations
        .contains(jvxFernliveCommandQueueElement.FERNLIVE_SHUTDOWN)) {
      queuedOperations.removeWhere((element) =>
          element == jvxFernliveCommandQueueElement.FERNLIVE_SHUTDOWN);
      ret = jvxFernliveCommandQueueElement.FERNLIVE_SHUTDOWN;
      
    }
    */
    return ret;
  }

  /*
  void requestUpdateProperties(
      JvxComponentIdentification cpId, String ident, bool contentOnly) {
    List<String> props = [ident];
    List<String> propLst = str2PropList(ident);
    if (propLst.length > 0) {
      props = propLst;
    }

    theBeCacheNative.invalidatePropertiesComponent(cpId, props, contentOnly);
  }
  */

  AudYoFloNativePropertyHandle? requestNativePropertyReference(
      JvxComponentIdentification cpId) {
    AudYoFloNativePropertyHandle? theComp;
    // asd
    Pointer<component_ident> cpTpPtr = calloc<component_ident>(1);
    component_ident idTpCp = cpTpPtr.ref;
    idTpCp.tp = cpId.cpTp.index;
    idTpCp.slotid = cpId.slotid;
    idTpCp.slotsubid = cpId.slotsubid;
    idTpCp.uId = cpId.uid;

    Pointer<one_property_comp> propHdl =
        natLib.ffi_properties_request_handle(opaque_host, cpTpPtr);
    if (propHdl != nullptr) {
      theComp = AudYoFloNativePropertyHandle(
          cpId: cpId,
          propRef: propHdl,
          natLib: natLib,
          opaqueHost: opaque_host);
    }
    calloc.free(cpTpPtr);
    return theComp;
  }

  void returnNativePropertyReference(
      JvxComponentIdentification cpId, AudYoFloNativePropertyHandle propRef) {
    // This function deletes the propRef struct
    natLib.ffi_properties_return_handle(opaque_host, propRef.propRef);
  }

  @override
  AudYoFloOneConnectedProcess allocateProcess(int uId) {
    AudYoFloOneComponentInProcessNative inProcLocal =
        AudYoFloOneComponentInProcessNative();
    AudYoFloOneConnectedProcess theProc = AudYoFloOneConnectedProcess(uId);

    Pointer<one_process> hdl =
        natLib.ffi_process_handle_for_uid(opaque_host, uId);

    if (hdl != nullptr) {
      one_process procRef = hdl.ref;
      if (procRef.name != nullptr) {
        Pointer<Utf8> nmPtr = procRef.name.cast<Utf8>();
        theProc.nameProcess = nmPtr.toDartString();
        natLib.ffi_host_delete(nmPtr.cast<Void>(),
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
        theProc.catId = procRef.category;
      }

      if (procRef.iterator != nullptr) {
        AudYoFloOneComponentInProcess oneInvolved =
            AudYoFloOneComponentInProcessNative();
        oneInvolved.fill(AudYoFloBackendSpecificNative(
            natLib, opaque_host, procRef.iterator));
        theProc.involved = oneInvolved;
      }
      natLib.ffi_host_delete(
          hdl.cast<Void>(), ffiDeleteDatatype.JVX_DELETE_DATATYPE_ONE_PROCESS);
    }
    return theProc;
  }

  @override
  int processSeqEvent(
      jvxSequencerEventType ev, jvxSequencerStatusEnum seqStat) {
    return theBeCacheNative.reportSequencerEvent(ev, seqStat);
  }

  void addProcess(int uId) {
    return theBeCacheNative.addProcess(uId);
  }

  void remProcess(int uId) {
    return theBeCacheNative.remProcess(uId);
  }

  @override
  String debugPrintProcess(int uId) {
    return theBeCacheNative.debugPrintProcess(uId);
  }

  void requestUpdateProcessStatus() async {
    await theBeCacheNative.triggerUpdateProcessStatus();
  }

  @override
  Future<int> triggerUpdateProcessStatus() async {
    int errCode = jvxErrorType.JVX_NO_ERROR;

    errCode = await Future.delayed(Duration.zero, () async {
      int errCode = jvxErrorType.JVX_NO_ERROR;
      if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
        // Native readout of the property descriptors
        Pointer<process_ready> procReadyPtr =
            natLib.ffi_process_system_ready(opaque_host);
        if (procReadyPtr != nullptr) {
          process_ready procReadyRef = procReadyPtr.ref;
          bool processesReadyNew =
              (procReadyRef.err == jvxErrorType.JVX_NO_ERROR);
          String reasonIfNot = '';
          if (procReadyRef.reason_if_not != nullptr) {
            reasonIfNot =
                procReadyRef.reason_if_not.cast<Utf8>().toDartString();
            natLib.ffi_host_delete(procReadyRef.reason_if_not.cast<Void>(),
                ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
          }
          natLib.ffi_host_delete(procReadyPtr.cast<Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_PROCESS_READY);
          theBeCacheNative.updateProcessStatusCompleteNotify(
              processesReadyNew, reasonIfNot);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INTERNAL;
        }
      } else {
        assert(false);
      }

      return errCode;
    });
    return errCode;
  }

  @override
  dynamic createComponentPropertyRef(
      JvxComponentIdentification cpId, dynamic optParam /* ignored */) {
    Pointer<component_ident> cpTpPtr = calloc<component_ident>(1);
    component_ident idTpCp = cpTpPtr.ref;
    idTpCp.tp = cpId.cpTp.index;
    idTpCp.slotid = cpId.slotid;
    idTpCp.slotsubid = cpId.slotsubid;
    idTpCp.uId = cpId.uid;

    // calloc.free(cpTpPtr);
    AudYoFloNativeHandles newHandlePair = AudYoFloNativeHandles(ident: cpTpPtr);
    newHandlePair.prop = requestNativePropertyReference(cpId);
    return newHandlePair;
  }

  void destroyComponentPropertyReference(
      JvxComponentIdentification cpId, dynamic removeme) {
    AudYoFloNativeHandles storedHandlePair = removeme;
    if (storedHandlePair.prop != null) {
      // Release property reference only if it is not null
      AudYoFloNativePropertyHandle propRef = storedHandlePair.prop!;
      returnNativePropertyReference(cpId, propRef);
    }

    // But always release the ident handle
    calloc.free(storedHandlePair.ident);
  }

  void reportStateSwitchComponent(
      JvxComponentIdentification cpId, jvxStateSwitchEnum ss) {
    return theBeCacheNative.reportStateSwitchComponent(cpId, ss);
  }

  // Function called on event <JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT>
  void reportUpdateStatusComponent(JvxComponentIdentification cpId) {
    return theBeCacheNative.reportUpdateStatusComponent(cpId);
  }

  // =================================================================

  // Translate component type enum to component class
  @override
  jvxComponentTypeClassEnum lookupComponentClass(JvxComponentTypeEnum tp) {
    jvxComponentTypeClassEnum cpClass =
        jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE;
    int cpClassId = natLib.ffi_lookup_component_class(opaque_host, tp.index);
    if (cpClassId >= jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE.index) {
      cpClass = jvxComponentTypeClassEEnum.fromInt(cpClassId);
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
      Pointer<Int8> strPtr = natLib.ffi_encode_component_identification(
          cpId.cpTp.index, cpId.slotid, cpId.slotsubid);
      if (strPtr.address != 0) {
        Pointer<Utf8> ptrU8 = strPtr.cast<Utf8>();
        address = ptrU8.toDartString();
        natLib.ffi_host_delete(strPtr.cast<Void>(),
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
      }

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
  Future<int> triggerActivateComponent(
      JvxComponentIdentification cpId, int id, bool select) async {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    jvxComponentTypeClassEnum cpCls = lookupComponentClass(cpId.cpTp);
    if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
      Pointer<component_ident> ptrId = nullptr;
      if (select) {
        // Select currently selected component
        switch (cpCls) {
          case jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_DEVICE:
            ptrId = natLib.ffi_select_component(
                opaque_host, cpId.cpTp.index, cpId.slotid, cpId.slotsubid, id);
            break;
          case jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NODE:
          case jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_TECHNOLOGY:
            ptrId = natLib.ffi_select_component(
                opaque_host, cpId.cpTp.index, cpId.slotid, cpId.slotsubid, id);
            break;
          default:
            assert(false);
        }
      } else {
        // The required pointer must be taken from the list of native handles
        MapEntry? entryProps = componentReferences.entries
            .firstWhereOrNull((element) => element.key == cpId);
        if (entryProps != null) {
          ptrId = entryProps.value.ident;
        }
      }

      // Step 2: Activate selected component
      if (ptrId != nullptr) {
        errCode = natLib.ffi_activate_selected_component(opaque_host, ptrId);
      }
      if (!select) {
        // If we have temporarily allocated a component ident pointer delete it here
        natLib.ffi_host_delete(ptrId.cast<Void>(),
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_ONE_COMPONENT_IDENT);
      }
    } else {
      // JSON CONTROL

      String address = 'unknown';
      Pointer<Int8> strPtr = natLib.ffi_encode_component_identification(
          cpId.cpTp.index, cpId.slotid, cpId.slotsubid);
      if (strPtr.address != 0) {
        Pointer<Utf8> ptrU8 = strPtr.cast<Utf8>();
        address = ptrU8.toDartString();
        natLib.ffi_host_delete(strPtr.cast<Void>(),
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
      }
      String cmd = 'act(' + address + ', activate, ' + id.toString() + ');';
      String retText = await transferTextCommand(cmd, true);
      Map valueMap = json.decode(retText);

      MapEntry? entry_ecode = valueMap.entries
          .firstWhereOrNull((element) => element.key == 'return_code');
      if (entry_ecode != null) {
        errCode = jvxErrorTypeEInt.fromStringSingle(entry_ecode.value);
      }
    }

    return errCode;
  }

  // ===========================================================================
  // DEACTIVATE COMPONENT DEACTIVATE COMPONENT DEACTIVATE COMPONENT DEACTIVATE COMPONENT
  // ===========================================================================

  @override
  Future<int> triggerDeactivateComponent(
      JvxComponentIdentification cpId, bool unselect) async {
    int errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    jvxComponentTypeClassEnum cpCls = lookupComponentClass(cpId.cpTp);
    if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
      // Get the native component ident from the list
      Pointer<component_ident> ptrId = nullptr;
      MapEntry? entryProps = componentReferences.entries
          .firstWhereOrNull((element) => element.key == cpId);
      if (entryProps != null) {
        ptrId = entryProps.value.ident;

        if (ptrId != nullptr) {
          errCode =
              natLib.ffi_deactivate_selected_component(opaque_host, ptrId);
        }

        // If we check the errorcode we will never get rid of selected devices
        // if (errCode == jvxErrorType.JVX_NO_ERROR) {
        if (unselect) {
          errCode = natLib.ffi_unselect_selected_component(opaque_host, ptrId);
        }
        //}

        // Note that the component_ident handle is stored within the list and will be removed
        // once the unselected component is reported back via the report callback!
      } // if (entryProps != null)
    } else {
      // JSON CONTROL
      assert(false);
      String address = 'unknown';
      Pointer<Int8> strPtr = natLib.ffi_encode_component_identification(
          cpId.cpTp.index, cpId.slotid, cpId.slotsubid);
      if (strPtr.address != 0) {
        Pointer<Utf8> ptrU8 = strPtr.cast<Utf8>();
        address = ptrU8.toDartString();
        natLib.ffi_host_delete(strPtr.cast<Void>(),
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
      }
      String cmd = 'act(' + address + ', deactivate);';
      String retText = await transferTextCommand(cmd, true);
      Map valueMap = json.decode(retText);

      MapEntry? entry_ecode = valueMap.entries
          .firstWhereOrNull((element) => element.key == 'return_code');
      if (entry_ecode != null) {
        errCode = jvxErrorTypeEInt.fromStringSingle(entry_ecode.value);
      }
    }

    return errCode;
  }

  @override
  Future<int> triggerGetPropertyContentComponent(
      JvxComponentIdentification cpId, List<String> propDescrLst,
      {AyfPropertyReportLevel reportArg =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY}) {
    Future<int> getPropertyComponent = Future(() async {
      int errCode = jvxErrorType.JVX_NO_ERROR;
      if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
        MapEntry? entry = componentReferences.entries
            .firstWhereOrNull((element) => element.key == cpId);
        if (entry != null) {
          AudYoFloNativeHandles oneNativeHandlePair = entry.value;
          AudYoFloNativePropertyHandle? propRef = oneNativeHandlePair.prop;
          if (propRef != null) {
            for (var elmS in propDescrLst) {
              AudYoFloPropertyContainer? propUpdate =
                  theBeCacheNative.referencePropertyInCache(cpId, elmS);
              if (propUpdate != null) {
                if (propUpdate is AudYoFloPropertyContentBackend) {
                  int errUpdate = AudYoFloPropertyContentFromNative
                      .updatePropertyContentFromNative(
                          propUpdate, elmS, propRef);
                  if (errUpdate == jvxErrorType.JVX_ERROR_POSTPONE) {
                    assert(false);
                  }
                } else {
                  assert(false);
                }
              }
            }
          } else {
            errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
          }
        } else {
          errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
        }
      } else {
        String address = 'unknown';
        Pointer<Int8> strPtr = natLib.ffi_encode_component_identification(
            cpId.cpTp.index, cpId.slotid, cpId.slotsubid);
        if (strPtr.address != 0) {
          Pointer<Utf8> ptrU8 = strPtr.cast<Utf8>();
          address = ptrU8.toDartString();
          natLib.ffi_host_delete(strPtr.cast<Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
        }
        String propDescr = "";
        if (propDescrLst.length > 1) {
          propDescr = "[";
          for (int i = 0; i < propDescrLst.length; i++) {
            propDescr += propDescrLst[i];
            if (i != propDescrLst.length - 1) {
              propDescr += ", ";
            }
          }
          propDescr += "]";
        } else {
          if (propDescrLst.length == 1) {
            propDescr = propDescrLst[0];
          }
        }
        if (propDescr.isNotEmpty) {
          String cmd = 'act(' + address + ', get_property, ' + propDescr + ');';
          String retText = await transferTextCommand(cmd, true);
          Map valueMap = json.decode(retText);

          String? errStr = getStringEntryValueMap(valueMap, 'return_code');
          if (errStr != null) {
            errCode = jvxErrorTypeEInt.fromStringSingle(errStr);
          }

          Map? subSec = getMapValueMap(valueMap, 'properties');
          if (subSec != null) {
            // Multiple fields returned
            for (var elmS in propDescrLst) {
              Map? subSubSec = getMapValueMap(subSec, elmS);
              if (subSubSec != null) {
                // Update the field here!
                AudYoFloPropertyContainer? propUpdate =
                    theBeCacheNative.referencePropertyInCache(cpId, elmS);
                if (propUpdate != null) {
                  if (propUpdate is AudYoFloPropertyContentBackend) {
                    int errUpdate = AudYoFloPropertyContentFromJson
                        .updatePropertyContentFromJsonMap(
                            propUpdate, subSubSec, this);
                    if (errUpdate == jvxErrorType.JVX_ERROR_POSTPONE) {
                      assert(false);
                    }
                  } else {
                    assert(false);
                  }
                }
              }
            }
          } else {
            if (propDescrLst.length == 1) {
              // Single field returned
              String elmS = propDescr;
              AudYoFloPropertyContainer? propUpdate =
                  theBeCacheNative.referencePropertyInCache(cpId, elmS);
              if (propUpdate != null) {
                if (propUpdate is AudYoFloPropertyContentBackend) {
                  AudYoFloPropertyContentFromJson
                      .updatePropertyContentFromJsonMap(
                          propUpdate, valueMap, this);
                }
              }
            }
          }
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
        }
      }
      theBeCacheNative.updatePropertyCacheCompleteNotify(cpId,
          report: reportArg);

      return errCode;
    });
    return getPropertyComponent;
  }

  @override
  Future<int> triggerGetPropertyDescriptionComponent(
      JvxComponentIdentification cpId, List<String> propDescrLst,
      {AyfPropertyReportLevel reportArg =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_NO_REPORT,
      int numDigits = 2}) async {
    int errCode = jvxErrorType.JVX_NO_ERROR;

    errCode = await Future.delayed(Duration.zero, () async {
      if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
        // Native readout of the property descriptors

        // Find entry in component cache list
        MapEntry? entry = componentReferences.entries
            .firstWhereOrNull((element) => element.key == cpId);
        if (entry != null) {
          // If this component is in cache list go ahead with the property reference
          AudYoFloNativeHandles oneHandlePair = entry.value;
          AudYoFloNativePropertyHandle? propRef = oneHandlePair.prop;

          if (propRef != null) {
            // Update all properties in provided string list
            for (var elmS in propDescrLst) {
              // Try to use existing container in cache
              AudYoFloPropertyContainer? propFromCache =
                  theBeCacheNative.referencePropertyInCache(cpId, elmS);
              if (propFromCache != null) {
                if (propFromCache is AudYoFloPropertyContentBackend) {
                  // An element already exists: update the property content
                  int resL = AudYoFloPropertyContentFromNative
                      .updatePropertyDescriptorFromNative(
                          this, propFromCache, propRef, flags, true);
                  if (resL != jvxErrorType.JVX_NO_ERROR) {
                    String t1 = cpId.txt;
                    String t2 = propFromCache.descriptor;
                    String t3 = jvxErrorTypeEInt.toStringSingle(resL);
                    print(
                        "Failed to update single property with descriptor $t2 on component <$t1>, reason: <$t3>.");
                    // assert(false);
                  }
                } else {
                  assert(false);
                }
              } else {
                // Create new component and receive descriptor
                AudYoFloPropertyContentBackend updateProp =
                    AudYoFloPropertyContentFromNative.createPropertyFromNative(
                        this, elmS, propRef, flags, numDigits);

                /*
              int resL = updateProp.descriptorFromJsonMap(this, elmS);
              if (resL == jvxErrorType.JVX_NO_ERROR) {
                */
                int resL = theBeCacheNative.updateEntrySinglePropertyCache(
                    cpId, updateProp);
                assert(resL == jvxErrorType.JVX_NO_ERROR);
              }
            }
          }
        }
      } else {
        String address = 'unknown';

        Pointer<Int8> strPtr = natLib.ffi_encode_component_identification(
            cpId.cpTp.index, cpId.slotid, cpId.slotsubid);
        if (strPtr.address != 0) {
          Pointer<Utf8> ptrU8 = strPtr.cast<Utf8>();
          address = ptrU8.toDartString();
          natLib.ffi_host_delete(strPtr.cast<Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
        }

        String propDescr = "";
        if (propDescrLst.length > 1) {
          propDescr = "[";
          for (int i = 0; i < propDescrLst.length; i++) {
            propDescr += propDescrLst[i];
            if (i != propDescrLst.length - 1) {
              propDescr += ", ";
            }
          }
          propDescr += "]";
        } else {
          if (propDescrLst.length == 1) {
            propDescr = propDescrLst[0];
          }
        }
        if (propDescr.isNotEmpty) {
          String cmd =
              'show(' + address + ', properties, fullp|x, ' + propDescr + ');';
          String retText = await transferTextCommand(cmd, true);
          Map valueMap = json.decode(retText);

          errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
          String? errCodeExprPtr =
              getStringEntryValueMap(valueMap, 'return_code');
          if (errCodeExprPtr != null) {
            // Convert error code
            String errCodeExpr = errCodeExprPtr;
            errCode = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
          }

          Map? subSec = getMapValueMap(valueMap, 'properties');
          if (subSec != null) {
            for (var elmS in propDescrLst) {
              Map? subSubSec = getMapValueMap(subSec, elmS);
              if (subSubSec == null) {
                assert(false);
              } else {
                AudYoFloPropertyContainer? propFromCache =
                    theBeCacheNative.referencePropertyInCache(cpId, elmS);
                if (propFromCache != null) {
                  if (propFromCache is AudYoFloPropertyContentBackend) {
                    // An element already exists: update the property content
                    int resL = AudYoFloPropertyContentFromJson
                        .updatePropertyDescriptionFromJson(
                            cpId,
                            this,
                            propFromCache,
                            elmS,
                            subSubSec,
                            flags,
                            true,
                            numDigits);
                    assert(resL == jvxErrorType.JVX_NO_ERROR);
                  } else {
                    assert(false);
                  }
                } else {
                  AudYoFloPropertyContentBackend updateProp =
                      AudYoFloPropertyContentFromJson.createPropertyFromJsonMap(
                          cpId, this, elmS, subSubSec, flags, numDigits);

                  /*
              int resL = updateProp.descriptorFromJsonMap(this, elmS);
              if (resL == jvxErrorType.JVX_NO_ERROR) {
                */
                  int resL = theBeCacheNative.updateEntrySinglePropertyCache(
                      cpId, updateProp);
                  if (resL != jvxErrorType.JVX_NO_ERROR) {
                    assert(false);
                  }
                }
              }
            }
          }
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
        }
      }
      if ((errCode == jvxErrorType.JVX_NO_ERROR) &&
          ((reportArg ==
                  AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY) ||
              (reportArg ==
                  AyfPropertyReportLevel
                      .AYF_FRONTEND_REPORT_COMPONENT_PROPERTY))) {
        theBeCacheNative.updatePropertyCacheCompleteNotify(cpId,
            report: reportArg);
      }
      return errCode;
    });
    return errCode;
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
    List<String> propContents = [];
    AyfBackendReportPropertySetEnum repEnum = AyfBackendReportPropertySetEnum
        .AYF_BACKEND_REPORT_SET_PROPERTY_INACTIVE;

    // We may reduce the number of "sets" to be issued
    if (preventCallMultipleTimes) {
      //print(
      //    'Entering <triggerSetProperties> with <preventCallMultipleTimes> true');

      for (var propDescr in propContentsArg) {
        AudYoFloPropertyContainer? propFromCache =
            theBeCacheNative.referencePropertyInCache(cpId, propDescr);
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

    // We need to run the update in a delayed manner. Otherwise we could end up
    // in calling this function in the build thread and the notify causes an exception
    // since it is not allowed to run notify within the build thread!!!
    // We may allow the delay to be larger to reduce the number of calls to the backend
    // at this point!
    return await Future<int>.delayed(
        Duration.zero /*Duration(milliseconds: 100)*/, () async {
      int errCode = jvxErrorType.JVX_NO_ERROR;
      if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
        MapEntry? entry = componentReferences.entries
            .firstWhereOrNull((element) => element.key == cpId);
        if (entry != null) {
          // If this component is in cache list go ahead with the property reference
          AudYoFloNativeHandles oneHandlePair = entry.value;
          AudYoFloNativePropertyHandle? propRef = oneHandlePair.prop;

          if (propRef != null) {
            if (reportArg ==
                AyfPropertyReportLevel
                    .AYF_BACKEND_REPORT_COMPONENT_PROPERTY_COLLECT) {
              errCode = natLib.ffi_properties_set_collect_start(
                  opaque_host, propRef.propRef);
            }

            for (var propDescr in propContents) {
              AudYoFloPropertyContainer? propFromCache =
                  theBeCacheNative.referencePropertyInCache(cpId, propDescr);
              if (propFromCache != null) {
                if (propFromCache is AudYoFloPropertyContentBackend) {
                  if (propFromCache is AudYoFloPropertyValueInRangeNative) {
                    AudYoFloPropertyValueInRangeNative contVal = propFromCache;

                    Pointer<Void> ptrSet = contVal.fldPtr.cast<Void>();
                    assert(ptrSet != nullptr);

                    int address = ptrSet.address;
                    address += contVal.szNativeElement * offset;
                    ptrSet = Pointer<Void>.fromAddress(address);

                    // Limit the number of elements due to offset
                    int numLocal = contVal.fldSz - offset;

                    // Limit the number of elements due to specification
                    if (num > 0) {
                      numLocal = min(num, numLocal);
                    }

                    errCode = natLib.ffi_properties_set_value_range(
                        opaque_host,
                        propRef.propRef,
                        propFromCache.descriptor.toNativeUtf8().cast<Int8>(),
                        1,
                        /* contentOnly */
                        offset,
                        ptrSet,
                        numLocal,
                        contVal.minVal,
                        contVal.maxVal,
                        repEnum.index);
                  } else if (propFromCache
                      is AudYoFloPropertyMultiContentNative) {
                    AudYoFloPropertyMultiContentNative contMulti =
                        propFromCache;

                    Pointer<Void> ptrSet = contMulti.fldPtr.cast<Void>();
                    if (ptrSet != nullptr) {
                      int address = ptrSet.address;
                      address += contMulti.szNativeElement * offset;
                      ptrSet = Pointer<Void>.fromAddress(address);

                      // Limit the number of elements due to offset
                      int numLocal = contMulti.fldSz - offset;

                      // Limit the number of elements due to specification
                      if (num > 0) {
                        numLocal = min(num, numLocal);
                      }

                      errCode = natLib.ffi_properties_set_content_property_fld(
                          opaque_host,
                          propRef.propRef,
                          propFromCache.descriptor.toNativeUtf8().cast<Int8>(),
                          1,
                          /* contentOnly */
                          offset,
                          ptrSet,
                          contMulti.jvxFormat.index,
                          numLocal,
                          repEnum.index);
                    } else {
                      print(
                          'Requesting property <${propFromCache.descriptor}> of type <${propFromCache.dataType.name}}> not possible due to nullptr.');
                    }
                  } else if (propFromCache
                      is AudYoFloPropertySelectionListBackend) {
                    AudYoFloPropertySelectionListBackend contSel =
                        propFromCache as AudYoFloPropertySelectionListBackend;

                    if (contSel.selection is AudYoFloBitFieldNative) {
                      AudYoFloBitFieldNative sel =
                          contSel.selection as AudYoFloBitFieldNative;

                      Pointer<Uint32> targetPtr = sel.raw;
                      int address = targetPtr.address;
                      address += offset *
                          sel.compileNum32BitBitfield *
                          sizeOf<Uint32>();
                      targetPtr = Pointer<Uint32>.fromAddress(address);

                      int numLocal =
                          (sel.numEntries - offset); // Corrected by HK
                      if (num > 0) {
                        numLocal = min(num, numLocal);
                      }

                      errCode = natLib.ffi_properties_set_selection_list(
                          opaque_host,
                          propRef.propRef,
                          propFromCache.descriptor.toNativeUtf8().cast<Int8>(),
                          1,
                          offset,
                          /* the offset may even be beyond 0 if property can deal with it */
                          targetPtr,
                          numLocal,
                          sel.compileNum32BitBitfield,
                          repEnum.index);
                    } else {
                      assert(false);
                    }
                  } else if (propFromCache
                      is AudYoFloPropertySingleStringBackend) {
                    AudYoFloPropertySingleStringBackend contStr = propFromCache;
                    errCode = natLib.ffi_properties_set_single_string(
                        opaque_host,
                        propRef.propRef,
                        propFromCache.descriptor.toNativeUtf8().cast<Int8>(),
                        contStr.value.toNativeUtf8().cast<Int8>(),
                        repEnum.index);
                  } else {
                    print(
                        'Setting property <${propFromCache.descriptor}> of type <${propFromCache.dataType.name}}> is currently not supported.');
                  }
                  if (propFromCache.setInProgress ==
                      jvxPropertyProgressStates.JVX_PROPERTY_PROGRESS_WAIT) {
                    propFromCache.setInProgress =
                        jvxPropertyProgressStates.JVX_PROPERTY_PROGRESS_NONE;
                  }
                } else {
                  assert(false);
                }
              } else {
                // Property not found
              }
            }

// If we use reportPropertyOnSet this will not be required
            if (invalidateProperty) {
              theBeCacheNative.invalidatePropertiesComponent(
                  cpId, propContents, true);
            }
            if (reportArg ==
                AyfPropertyReportLevel
                    .AYF_BACKEND_REPORT_COMPONENT_PROPERTY_COLLECT) {
              errCode = natLib.ffi_properties_set_collect_stop(
                  opaque_host, propRef.propRef);
            }

            if (reportArg !=
                AyfPropertyReportLevel.AYF_BACKEND_REPORT_COMPONENT_PROPERTY) {
              // If we use reportPropertyOnSet this will not be required
              theBeCacheNative.updatePropertyCacheCompleteNotify(cpId,
                  report: reportArg);
              // theBeCacheNative.updatePropertiesInCacheComplete(report: report);
            }
          }
        }
      }
      return errCode;
    });
  }

  // Trigger backend to update the list of properties in cache
  @override
  Future<int> triggerUpdatePropertyListComponent(
      JvxComponentIdentification cpId,
      {AyfPropertyReportLevel reportArg = AyfPropertyReportLevel
          .AYF_FRONTEND_REPORT_COMPONENT_PROPERTY}) async {
    // The update property list in cache is typically executed with a delay
    // since here is the point where we have to request each single property.
    // We are only allowed to notify the listeners if the result is not within
    // the build function.
    return await Future<int>.delayed(Duration.zero, () async {
      int errCode = jvxErrorType.JVX_NO_ERROR;
      if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
        MapEntry? entry = componentReferences.entries
            .firstWhereOrNull((element) => element.key == cpId);
        if (entry != null) {
          // If this component is in cache list go ahead with the property reference
          AudYoFloNativeHandles oneHandlePair = entry.value;
          AudYoFloNativePropertyHandle? propRef = oneHandlePair.prop;
          if (propRef != null) {
            List<String> lstProps = [];
            int num = natLib.ffi_properties_number_properties(
                propRef.opaqueHost, propRef.propRef);
            for (int idx = 0; idx < num; idx++) {
              Pointer<one_property_full_plus> propDescrPtr = propRef.natLib
                  .ffi_properties_descriptor_property_lin(
                      propRef.opaqueHost,
                      propRef.propRef,
                      idx,
                      jvxPropertyDescriptorEnum
                          .JVX_PROPERTY_DESCRIPTOR_FULL_PLUS);
              if (propDescrPtr != nullptr) {
                one_property_full_plus propDescrRef = propDescrPtr.ref;
                lstProps.add(
                    propDescrRef.full.descriptor.cast<Utf8>().toDartString());
                propRef.natLib.ffi_host_delete(
                    propDescrPtr.cast<Void>(),
                    ffiDeleteDatatype
                        .JVX_DELETE_DATATYPE_ONE_PROPERTY_DESCRIPTOR_FULL_PLUS);
              }
            }
            theBeCacheNative.updatePropertyCacheListCompleteNotify(
                cpId, lstProps,
                report: reportArg);
          } else {
            errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
          }
        } else {
          errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
        }
      } else {
        String address = 'unknown';

        Pointer<Int8> strPtr = natLib.ffi_encode_component_identification(
            cpId.cpTp.index, cpId.slotid, cpId.slotsubid);
        if (strPtr.address != 0) {
          Pointer<Utf8> ptrU8 = strPtr.cast<Utf8>();
          address = ptrU8.toDartString();
          natLib.ffi_host_delete(strPtr.cast<Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
        }

        String propDescr = "/*";
        String cmd =
            'show(' + address + ', properties, list, ' + propDescr + ');';
        String retText = await transferTextCommand(cmd, true);
        Map valueMap = json.decode(retText);

        errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
        String? errCodeExprPtr =
            getStringEntryValueMap(valueMap, 'return_code');
        if (errCodeExprPtr != null) {
          // Convert error code
          String errCodeExpr = errCodeExprPtr;
          errCode = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
        }
        if (errCode == jvxErrorType.JVX_NO_ERROR) {
          List? subSec = getMapValueList(valueMap, 'properties');
          if (subSec != null) {
            List<String> lstProps = [];
            for (var elmS in subSec) lstProps.add(elmS);
            theBeCacheNative.updatePropertyCacheListCompleteNotify(
                cpId, lstProps);
          }
        }
      }

      return errCode;
    });
  }

  @override
  Future<int> triggerUpdateDeviceList(JvxComponentIdentification cpId) async {
    // The update property list in cache is typically executed with a delay
    // since here is the point where we have to request each single property.
    // We are only allowed to notify the listeners if the result is not within
    // the build function.
    return await Future<int>.delayed(Duration.zero, () async {
      int errCode = jvxErrorType.JVX_NO_ERROR;

      if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
        // Get the native opaque handle for the component
        MapEntry? entry = componentReferences.entries
            .firstWhereOrNull((element) => element.key == cpId);
        if (entry != null) {
          // If we found it
          AudYoFloNativeHandles oneNativeHandlePair = entry.value;
          Pointer<component_ident> cpPtr = oneNativeHandlePair.ident;
          component_ident cpRref = cpPtr.ref;
          // New device list
          List<AudYoFloOneDeviceSelectionOption> devices = [];

          // Obtain the number of components
          int num = natLib.ffi_component_number_devices(opaque_host, cpPtr);
          for (int idx = 0; idx < num; idx++) {
            AudYoFloOneDeviceSelectionOption newDevice =
                AudYoFloOneDeviceSelectionOption();

            // Native call to update
            Pointer<device_capabilities> devCapPtr = natLib
                .ffi_component_capabilities_device(opaque_host, cpPtr, idx);
            if (devCapPtr != nullptr) {
              device_capabilities devCapRef = devCapPtr.ref;

              // Fill in the values as received
              newDevice.caps.fld = fn.Int32(devCapRef.caps);
              newDevice.flow =
                  jvxDeviceDataFlowTypeEEnum.fromInt(devCapRef.flow);
              newDevice.description =
                  devCapRef.description.cast<Utf8>().toDartString();
              newDevice.descriptor =
                  devCapRef.descriptor.cast<Utf8>().toDartString();
              newDevice.flags.fld = fn.Int32(devCapRef.flags);
              newDevice.selectable = (devCapRef.selectable != 0);
              newDevice.optionIdx = idx;
              newDevice.state.fld = fn.Int32(devCapRef.stat);

              // newDevice.state = devCapRef.

              // The device is always technology + 1
              newDevice.devIdent.cpTp =
                  JvxComponentTypeEEnum.fromInt(cpRref.tp + 1);

              // The slotid value is always helpful to identify the technology
              newDevice.devIdent.slotid = cpRref.slotid;
              newDevice.devIdent.slotsubid = devCapRef.slotsubid;
              devices.add(newDevice);

              if (devCapRef.description != nullptr) {
                natLib.ffi_host_delete(devCapRef.description.cast<Void>(),
                    ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
              }

              if (devCapRef.descriptor != nullptr) {
                natLib.ffi_host_delete(devCapRef.descriptor.cast<Void>(),
                    ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
              }
              // Release the memory
              natLib.ffi_host_delete(devCapPtr.cast<Void>(),
                  ffiDeleteDatatype.JVX_DELETE_DATATYPE_DEVICE_CAPS);

              // Update the list and notify all!!
            }
          }

          theBeCacheNative.updateDeviceListCacheCompleteNotify(cpId, devices);
        } else {
          errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
        }
      } else {
        assert(
            false); // was implemented but not tested, reactivate comment if desired
        /*
        // if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI)
        String address = 'unknown';

        Pointer<Int8> strPtr = natLib.ffi_encode_component_identification(
            cpId.cpTp.index, cpId.slotid, cpId.slotsubid);
        if (strPtr.address != 0) {
          Pointer<Utf8> ptrU8 = strPtr.cast<Utf8>();
          address = ptrU8.toDartString();
          natLib.ffi_host_delete(strPtr.cast<Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
        }

        String propDescr = "/ *"; // <- added " "
        String cmd = 'show(' + address + ', devices);';
        String retText = transferTextCommand(cmd, true);
        Map valueMap = json.decode(retText);

        errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
        String? errCodeExprPtr =
            helpers.getStringEntryValueMap(valueMap, 'return_code');
        if (errCodeExprPtr != null) {
          // Convert error code
          String errCodeExpr = errCodeExprPtr;
          errCode = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
        }
        if (errCode == jvxErrorType.JVX_NO_ERROR) {
          List? subSec = helpers.getMapValueList(valueMap, 'properties');
          if (subSec != null) {
            List<String> lstProps = [];
            for (var elmS in subSec) lstProps.add(elmS);
            theBeCacheNative.updatePropertyCacheListCompleteNotify(cpId, lstProps);
          }
        }

        */
      }

      return errCode;
    });
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

      if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
        // For this command we do not need a specific component ident
        Pointer<component_ident> cpIdPtr = calloc<component_ident>(1);
        component_ident cpIdRef = cpIdPtr.ref;
        cpIdRef.tp = cpId.cpTp.index;
        cpIdRef.slotid = cpId.slotid;

        // Sub id and uid do not have any impact
        cpIdRef.slotsubid = cpId.slotsubid;
        cpIdRef.uId = cpId.uid;

        // New device list
        List<AudYoFloOneComponentSelectionOption> options = [];

        // Get the slot population for this component
        List<int> ssids = [];
        Pointer<ss_list> sslistPtr =
            natLib.ffi_component_slot_allocation(opaque_host, cpIdPtr);
        if (sslistPtr != nullptr) {
          ss_list sslistRef = sslistPtr.ref;
          Int32List? lst;
          if (sslistRef.slots != nullptr) {
            lst = sslistRef.slots.asTypedList(sslistRef.num);
          }
          if (sslistRef.subslots != nullptr) {
            lst = sslistRef.subslots.asTypedList(sslistRef.num);
          }
          if (lst != null) {
            for (var sgl in lst) {
              ssids.add(sgl);
            }
          }
        }

        // Obtain the number of components
        int num = natLib.ffi_component_number_components(opaque_host, cpIdPtr);
        for (int idx = 0; idx < num; idx++) {
          AudYoFloOneComponentSelectionOption newOption =
              AudYoFloOneComponentSelectionOption();

          // Find the associated sots
          for (int sidx = 0; sidx < ssids.length; sidx++) {
            if (ssids[sidx] == idx) newOption.sids.add(sidx);
          }

          // Native call to update entry
          Pointer<selection_option> optPtr =
              natLib.ffi_component_selection_option(opaque_host, cpIdPtr, idx);
          if (optPtr != nullptr) {
            selection_option optRef = optPtr.ref;

            // Fill in the values as received
            newOption.description =
                optRef.description.cast<Utf8>().toDartString();
            newOption.descriptor =
                optRef.descriptor.cast<Utf8>().toDartString();

            // Stil missing: the slots
            newOption.optionIdx = idx;

            // The device is always technology + 1
            options.add(newOption);

            if (optRef.description != nullptr) {
              natLib.ffi_host_delete(optRef.description.cast<Void>(),
                  ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
            }

            if (optRef.descriptor != nullptr) {
              natLib.ffi_host_delete(optRef.descriptor.cast<Void>(),
                  ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
            }
            // Release the memory
            natLib.ffi_host_delete(optPtr.cast<Void>(),
                ffiDeleteDatatype.JVX_DELETE_DATATYPE_SELECTION_OPTION);
          }
        }

        calloc.free(cpIdPtr);

        // Update the list and notify all!!
        theBeCacheNative.updateComponentListCacheCompleteNotify(
            cpId, options, isDevice);
      } else {
        assert(
            false); // was implemented but not tested, reactivate comment if desired
        /*
        // if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI)
        String address = 'unknown';

        Pointer<Int8> strPtr = natLib.ffi_encode_component_identification(
            cpId.cpTp.index, cpId.slotid, cpId.slotsubid);
        if (strPtr.address != 0) {
          Pointer<Utf8> ptrU8 = strPtr.cast<Utf8>();
          address = ptrU8.toDartString();
          natLib.ffi_host_delete(strPtr.cast<Void>(),
              ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
        }

        String propDescr = "/ *"; <- added " "
        String cmd = 'show(' + address + ', options);';
        String retText = transferTextCommand(cmd, true);
        Map valueMap = json.decode(retText);

        errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
        String? errCodeExprPtr =
            helpers.getStringEntryValueMap(valueMap, 'return_code');
        if (errCodeExprPtr != null) {
          // Convert error code
          String errCodeExpr = errCodeExprPtr;
          errCode = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
        }
        if (errCode == jvxErrorType.JVX_NO_ERROR) {
          List? subSec = helpers.getMapValueList(valueMap, 'properties');
          if (subSec != null) {
            List<String> lstProps = [];
            for (var elmS in subSec) lstProps.add(elmS);
            theBeCacheNative.updatePropertyCacheListCompleteNotify(cpId, lstProps);
          }
        }
        */
      }

      return errCode;
    });
  }

  // =====================================================================
  // Api call to save current setup in config file
  // =====================================================================

  @override
  Future<int> triggerSaveConfig() async {
    return await Future<int>.delayed(Duration.zero, () async {
      int errCode = jvxErrorType.JVX_NO_ERROR;

      if (accBackend == ayfBackendAccessEnum.JVX_ACCESS_BACKEND_FFI) {
        errCode = natLib.ffi_config_save(opaque_host);
      } else {
        assert(false);
      }
      return errCode;
    });
  }

  @override
  void reportPropertyListSetBackend(
      JvxComponentIdentification cpId, String propLst) {
    theBeCacheNative.reportPropertyListSetBackend(cpId, propLst);
  }

  @override
  Future<void> triggerClose() async {
    if (shutdown != null) {
      await shutdown!();
    }
  }

  @override
  Future<int> triggerStartPropertyStream(
      List<AudYoFloPropertyStreamDefintion> propDescriptions,
      void Function(
              JvxComponentIdentification,
              JvxPropertyStreamMessageType msgT,
              String propertyDescriptor,
              dynamic data,
              int offset)
          cb,
      {int desired_timeout_msec = 1000,
      int desired_ping_granularity = 10}) async {
    return jvxErrorType.JVX_ERROR_UNSUPPORTED;
  }

  Future<int> triggerStopPropertyStream() async {
    return jvxErrorType.JVX_ERROR_UNSUPPORTED;
  }

  void triggerComponentStateSwitch(jvxStateSwitchEnum ss, String compName,
      JvxComponentIdentification cpId) async {
    if (ss == jvxStateSwitchEnum.JVX_STATE_SWITCH_SELECT) {
      JvxComponentIdentification cpIdUse =
          JvxComponentIdentification.from(cpId);

      // cpIdUse.slotsubid = 0;

      jvxComponentTypeClassEnum cpClass = lookupComponentClass(cpIdUse.cpTp);
      if (cpClass == jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_DEVICE) {
        // We need to transform a device into a technology to list all devices
        cpIdUse.cpTp = JvxComponentTypeEEnum.fromInt(cpIdUse.cpTp.index - 1);

        // Return the name of available devices
        var devLst = theBeCache.referenceDeviceListInCache(cpIdUse);
        if (devLst == null) {
          int result = await triggerUpdateDeviceList(cpIdUse);
          if (result == jvxErrorType.JVX_NO_ERROR) {
            devLst = theBeCache.referenceDeviceListInCache(cpIdUse);
          }
        }

        // In the list check for the right names
        if (devLst != null) {
          var elm = devLst
              .firstWhereOrNull((element) => element.description == compName);
          if (elm == null) {
            dbgPrint('Unable to activate device with ident <$compName>.');
          } else {
            triggerActivateComponent(cpId, elm.optionIdx, true);
          }
        }
      }
    }
  }
}
