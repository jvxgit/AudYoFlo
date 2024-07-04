import 'package:collection/collection.dart';
import 'package:flutter/foundation.dart';
import 'package:ayf_pack/helpers/ayf_helper_lib.dart' as helpers;
import '../ayf_pack_local.dart';

// ================================================
// ================================================
// The actual cache
// ================================================
// ================================================
class AudYoFloHttpConnect {
  bool isConnected = false;
}

abstract class AudYoFloBackendCacheCallback {
  void reportEvent(String reasonHint);
}

abstract class AudYoFloBackendCache
    with ChangeNotifier
    implements AudYoFloBackendCacheBectrlIf, AudYoFloBackendCacheFectrlIf {
  AudYoFloBackendAdapterIf? _be;

  AudYoFloBackendCacheCallback? requestSystemDisconnect;

  AudYoFloHttpConnect httpConnect = AudYoFloHttpConnect();
  // ======================================================================
  // Initialization function
  // ======================================================================

  void initializeWithBackendAdapterReference(
      AudYoFloBackendAdapterIf beAdapterArg) {
    processSection.theRegisteredProcesses.initialize(beAdapterArg);
    componentSection.theSelectedComponents.initialize(beAdapterArg);

    // Store reference to backend adapter
    _be = beAdapterArg;
  }

  // Return reference to backward adapter if required
  AudYoFloBackendAdapterIf? get backendAdapterIf => _be;

  // ========================================================================
  // Member functions
  // ======================================================================

  // Trigger notification in case the cache has changed
  void triggerNotify() {
    notifyListeners();
  }

  // ========================================================================
  // Helper functions
  // ======================================================================
  AudYoFloOneSelectedComponent? findSelectedComponent(
      JvxComponentIdentification cpId) {
    AudYoFloOneSelectedComponent? theSelected;
    MapEntry? entry = componentSection
        .theSelectedComponents.selectedComponents.entries
        .firstWhereOrNull((element) => element.key == cpId);
    if (entry != null) {
      theSelected = entry.value;
    }
    return theSelected;
  }

  // ======================================================================
  // Dedicated caches for all sub elements
  // ======================================================================

  // Hold a structure to a cache for all components
  final AudYoFloComponentCache componentSection = AudYoFloComponentCache();

  // Hold a structure for all processes
  final AudYoFloProcessesCache processSection = AudYoFloProcessesCache();

  // Hold a structure for all sequencer related infos
  final AudYoFloSequencerStateCache sequencerSection =
      AudYoFloSequencerStateCache();

  // Holds the type class list
  Map<jvxComponentTypeClassEnum, List<JvxComponentIdentification>>
      typeClassList = {};

  // Hold a system state id. If the system state id increases we need to restart!
  int ssUpdateId = 1;

  // Cache to hold the device options for each technology
  final AudYoFloDeviceSelectionMapCache deviceOptionMap =
      AudYoFloDeviceSelectionMapCache();

  // Cache to hold all component selection options
  final AudYoFloComponentSelectionMapCache componentOptionMap =
      AudYoFloComponentSelectionMapCache();

  // ======================================================================
  // Function to expose the interfaces
  // ======================================================================

  AudYoFloBackendCacheFectrlIf get cacheToFrontend => this;
  AudYoFloBackendCacheBectrlIf get cacheToBackend => this;

  // ================================================================
  // ================================================================
  // INTERFACE FUNCTIONS INTERFACE FUNCTIONS INTERFACE FUNCTIONS INTERFACE FUNCTIONS
  // ================================================================
  // ================================================================

  // Functions to add components - reported when being selected
  @override
  void reportSelectedComponent(JvxComponentIdentification cpId, String descr,
      {AudYoFloDirectPropertiesIf? directPropRef}) {
    // Add an entry in the list of selected components
    componentSection.theSelectedComponents.addComponent(cpId, descr);
    componentSection.ssUpdateId++;

    // Store typeclass list
    jvxComponentTypeClassEnum cpClass =
        backendAdapterIf!.lookupComponentClass(cpId.cpTp);
    if (cpClass != jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE) {
      MapEntry<jvxComponentTypeClassEnum, List<JvxComponentIdentification>>?
          cls = typeClassList.entries
              .firstWhereOrNull((element) => element.key == cpClass);
      if (cls != null) {
        // There should be no such entry!!
        JvxComponentIdentification? lelm =
            cls.value.firstWhereOrNull((element) => element == cpId);
        assert(lelm == null);
        cls.value.add(cpId);
      } else {
        List<JvxComponentIdentification> lst = [cpId];
        typeClassList[cpClass] = lst;
      }
    }
    // componentOptionMap
    triggerNotify();
  }

  @override
  void reportUnselectedComponent(JvxComponentIdentification cpId) {
    AudYoFloOneSelectedComponent? actComponent = findSelectedComponent(cpId);
    if (actComponent != null) {
      for (var sElm in actComponent.propertyCache.cachedProperties.entries) {
        AudYoFloPropertyContainer theCProp = sElm.value;
        theCProp.dispose();
      }
      actComponent.propertyCache.availProperties = null;
    }
    componentSection.theSelectedComponents.removeComponent(cpId);
    componentSection.ssUpdateId++;

    // ==========================================
    // Remove the device list in cache
    if (checkComponentIdentTechnology(cpId)) {
      deviceOptionMap.selectionOptionCacheMap.removeWhere((key, value) =>
          (key.cpTp == cpId.cpTp) && (key.slotid == cpId.slotid));
    }

    // ===========================================
    // Remove the component cache list for this component type
    if (checkComponentIdentDevice(cpId)) {
      componentOptionMap.selectionOptionCacheMap.removeWhere((key, value) =>
          (key.cpTp == cpId.cpTp) && (key.slotid == cpId.slotid));
    } else {
      componentOptionMap.selectionOptionCacheMap
          .removeWhere((key, value) => (key.cpTp == cpId.cpTp));
    }

    // Cleanup this component in component class list
    bool completeOne = false;
    jvxComponentTypeClassEnum rmMe =
        jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE;
    for (var elmTp in typeClassList.entries) {
      for (var elmTpTp in elmTp.value) {
        if (elmTpTp == cpId) {
          completeOne = true;
          break;
        }
      }
      if (completeOne) {
        elmTp.value.remove(cpId);
        completeOne = false;
        if (elmTp.value.isEmpty) {
          rmMe = elmTp.key;
        }
        break;
      }
    }
    if (rmMe != jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE) {
      typeClassList.remove(rmMe);
    }

    // Post latest change
    triggerNotify();
  }

  @override
  void invalidateComponentListCache(JvxComponentIdentification cpId) {
    if (checkComponentIdentDevice(cpId)) {
      // If device, we need to compare tp and slotid
      MapEntry<
          JvxComponentIdentification,
          AudYoFloOneSelectionOptionEntry<
              AudYoFloOneComponentSelectionOption>>? entry = componentOptionMap
          .selectionOptionCacheMap.entries
          .firstWhereOrNull((element) =>
              (element.key.cpTp == cpId.cpTp) &&
              (element.key.slotid == cpId.slotid));
      if (entry != null) {
        // There was a device list before, just update it
        entry.value.options.optionList.clear();
        entry.value.options.valid = false;
        entry.value.options.ssUpdateId += 1;
      }
    } else {
      MapEntry<
          JvxComponentIdentification,
          AudYoFloOneSelectionOptionEntry<
              AudYoFloOneComponentSelectionOption>>? entry = componentOptionMap
          .selectionOptionCacheMap.entries
          .firstWhereOrNull((element) => (element.key.cpTp == cpId.cpTp));
      if (entry != null) {
        // There was a device list before, just update it
        entry.value.options.optionList.clear();
        entry.value.options.valid = false;
        entry.value.options.ssUpdateId += 1;
      }
    }
  }
  // fectrl

  @override
  void invalidatePropertiesComponent(
      JvxComponentIdentification cpId, List<String> props, bool contentOnly,
      {bool invalidateAll = false}) {
    // Find the component in list of selected components

    AudYoFloOneSelectedComponent? actComponent = findSelectedComponent(cpId);
    if (actComponent != null) {
      // Browse through list of specified properties
      if (invalidateAll) {
        props.clear();
        for (var elmProp in actComponent.propertyCache.cachedProperties.entries)
          props.add(elmProp.value.descriptor);
        //
      }
      for (String elmPS in props) {
        String theProp = elmPS;
        bool contentOnlyThisProp = contentOnly;
        List<String> elements = theProp.split('+');
        if (elements.length == 2) {
          contentOnlyThisProp = false;
          theProp = elements[0];
        }
        AudYoFloPropertyContainer? propContent =
            actComponent.propertyCache.findCachedProperty(theProp);
        if (propContent != null) {
          if (!contentOnlyThisProp) {
            propContent.cache_status.bitClear(jvxPropertyCacheStatusFlagShifts
                .JVX_PROPERTY_DESCRIPTOR_VALID.index);
            propContent.cache_status.bitSet(jvxPropertyCacheStatusFlagShifts
                .JVX_PROPERTY_DESCRIPTOR_INVALID.index);

            // If we update the property descriptor, it is like a new property.
            // Dismiss all attached data
            propContent.attachedSpecific.clear();
          }
          propContent.cache_status.bitClear(jvxPropertyCacheStatusFlagShifts
              .JVX_PROPERTY_CONTENT_VALID.index);
          propContent.cache_status.bitSet(jvxPropertyCacheStatusFlagShifts
              .JVX_PROPERTY_CONTENT_INVALID.index);
          propContent.ssUpdateId++;
        }
      }
    }
  }

  @override
  List<String>? referencePropertyListInCache(JvxComponentIdentification cpId) {
    List<String>? retVal;
    AudYoFloOneSelectedComponent? actComponent = findSelectedComponent(cpId);
    if (actComponent != null) {
      retVal = actComponent.propertyCache.availProperties;
    }
    return retVal;
  }

  @override
  List<AudYoFloPropertyContainer>? referenceValidPropertiesComponents(
      JvxComponentIdentification cpId, List<String> props) {
    List<AudYoFloPropertyContainer>? retVal;
    List<AudYoFloPropertyContainer> collect = [];
    bool foundAll = true;

    // If
    AudYoFloOneSelectedComponent? actComp = findSelectedComponent(cpId);
    if (actComp != null) {
      for (var elmS in props) {
        AudYoFloPropertyContainer? propRef =
            actComp.propertyCache.findCachedProperty(elmS);
        if (propRef != null) {
          // We accept also error to return a value in list
          if (propRef.checkInvalidContent()) {
            foundAll = false;
            break;
          } else {
            collect.add(propRef);
          }
        } else {
          foundAll = false;
          break;
        }
      }
    }

    if (foundAll) {
      retVal = collect;
    }

    return retVal;
  }

  @override
  Future<int> triggerUpdatePropertiesComponent(
      JvxComponentIdentification cpId, List<String> propDescrArg,
      {AyfPropertyReportLevel report =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY,
      bool forceDescriptor = false,
      bool forceContent = false}) async {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    List<String> propsDescr = [];
    List<String> propsContent = [];

    AudYoFloOneSelectedComponent? actComp = findSelectedComponent(cpId);
    if (actComp != null) {
      for (var elmS in propDescrArg) {
        AudYoFloPropertyContainer? propRef =
            actComp.propertyCache.findCachedProperty(elmS);
        if (propRef != null) {
          if (!propRef.checkValidDescriptor() || forceDescriptor) {
            propsDescr.add(elmS);
            propsContent.add(elmS);
          } else {
            if (!propRef.checkValidContent() || forceContent) {
              propsContent.add(elmS);
            }
          }
        } else {
          propsDescr.add(elmS);
          propsContent.add(elmS);
        }
      }
      if (propsDescr.isNotEmpty) {
        errCode = await backendAdapterIf!
            .triggerGetPropertyDescriptionComponent(cpId, propsDescr,
                reportArg: report);
      }
      if (errCode == jvxErrorType.JVX_NO_ERROR) {
        errCode = await backendAdapterIf!.triggerGetPropertyContentComponent(
            cpId, propsContent,
            reportArg: report);
      }
    } else {
      errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    }
    return errCode;
  }

  @override
  Future<int> triggerSetProperties(
      JvxComponentIdentification cpId, List<String> propContents,
      {bool invalidateProperty = true,
      int offset = 0,
      int num = -1,
      bool preventCallMultipleTimes = false,
      AyfPropertyReportLevel report =
          AyfPropertyReportLevel.AYF_BACKEND_REPORT_COMPONENT_PROPERTY}) async {
    int errorCode = jvxErrorType.JVX_NO_ERROR;
    errorCode = await backendAdapterIf!.triggerSetProperties(cpId, propContents,
        invalidateProperty: invalidateProperty,
        offset: offset,
        num: num,
        preventCallMultipleTimes: preventCallMultipleTimes,
        reportArg: report);

    return errorCode;
  }

  // Check that call addresses a technology (in case we request a devices list)
  @override
  bool checkComponentIdentTechnology(JvxComponentIdentification cpId) {
    bool isTechnology = false;
    // Check that the component is a technology
    MapEntry<jvxComponentTypeClassEnum, List<JvxComponentIdentification>>?
        elmTech = typeClassList.entries.firstWhereOrNull((element) =>
            element.key ==
            jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_TECHNOLOGY);
    if (elmTech != null) {
      JvxComponentIdentification? elmCp =
          elmTech.value.firstWhereOrNull((element) => element == cpId);
      if (elmCp != null) {
        isTechnology = true;
      }
    }
    return isTechnology;
  }

  // Check that call addresses a technology (in case we request a devices list)
  @override
  bool checkComponentIdentDevice(JvxComponentIdentification cpId) {
    bool isDevice = false;
    // Check that the component is a technology
    MapEntry<jvxComponentTypeClassEnum, List<JvxComponentIdentification>>?
        elmDev = typeClassList.entries.firstWhereOrNull((element) =>
            element.key == jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_DEVICE);
    if (elmDev != null) {
      JvxComponentIdentification? elmCp =
          elmDev.value.firstWhereOrNull((element) => element == cpId);
      if (elmCp != null) {
        isDevice = true;
      }
    }
    return isDevice;
  }

  // ========================================================================
  // ========================================================================
  // ========================================================================
  // ========================================================================
  // ========================================================================
  // ========================================================================

  // ================================================================
  // Functions to report new processes and removed processes
  @override
  void addProcess(int uId) {
    processSection.theRegisteredProcesses.addProcess(uId);
    //componentSection.ssUpdateId++;
    processSection.ssUpdateId++;
    triggerNotify();
  }

  @override
  void remProcess(int uId) {
    processSection.theRegisteredProcesses.removeProcess(uId);
    //componentSection.ssUpdateId++;
    processSection.ssUpdateId++;
    triggerNotify();
  }

  @override
  void updateProcessStatusCompleteNotify(
      bool processesReadyNew, String reasonIfNot) {
    sequencerSection.processesReady = processesReadyNew;
    sequencerSection.reasonIfNot = reasonIfNot;
    sequencerSection.ssUpdateId++;
    triggerNotify();
  }

  @override
  Future<int> triggerUpdateProcessStatus() async {
    int errCode = await backendAdapterIf!.triggerUpdateProcessStatus();
    return errCode;
  }

  // Update the state if it changes (typically "active")
  @override
  void reportStateSwitchComponent(
      JvxComponentIdentification cpId, jvxStateSwitchEnum ss) {
    if (ss == jvxStateSwitchEnum.JVX_STATE_SWITCH_SELECT) {}
    componentSection.theSelectedComponents.updateComponent(cpId);

    // If a component is activated, load the component selection lists
    // that depends on this component
    if ((ss == jvxStateSwitchEnum.JVX_STATE_SWITCH_ACTIVATE) ||
        (ss == jvxStateSwitchEnum.JVX_STATE_SWITCH_DEACTIVATE)) {
      AudYoFloOneSelectedComponent? theComponentHere =
          findSelectedComponent(cpId);
      if (theComponentHere != null) {
        bool foundOne = true;
        while (foundOne) {
          foundOne = false;
          for (var elm
              in theComponentHere.propertyCache.cachedProperties.entries) {
            // Invalidate
            if (elm.value.invalidateOnStateSwitch(ss)) {
              // If true is returned the single property was also disposed
              foundOne = true;
              elm.value.dispose();
              theComponentHere.propertyCache.cachedProperties.remove(elm.key);
              break;
            }
          }
        }
      }

      triggerNotify();
    } else {
      debugPrint('New stateswitch triggered.');
    }
  }

  @override
  void reportUpdateStatusComponent(JvxComponentIdentification cpId) {
    // We end up here mostly if a device associated to a technology has been selected/unselected

    // Check if the component is a technology
    if (checkComponentIdentTechnology(cpId)) {
      // Find the device list
      MapEntry<JvxComponentIdentification, AudYoFloOneSelectionOptionEntry>?
          entry = deviceOptionMap.selectionOptionCacheMap.entries
              .firstWhereOrNull((element) =>
                  (element.key.cpTp == cpId.cpTp) &&
                  (element.key.slotid == cpId.slotid));
      if (entry != null) {
        entry.value.options.optionList.clear();
        entry.value.options.valid = false;
        entry.value.options.ssUpdateId += 1;
      }
    }
    triggerNotify();
  }

  // ================================================================
  // Process a single sequencer even
  @override
  int reportSequencerEvent(
      jvxSequencerEventType ev, jvxSequencerStatusEnum seqStat) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    String maskStr = ev.eventMask.toRadixString(16);
    debugPrint('Sequencer reports < $ev.description>, event mask = 0x$maskStr');

    if ((ev.eventMask &
            jvxSequencerEventBitfield
                .JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP) !=
        0) {
      if (ev.setp ==
          jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER) {
        // What shall we do here?
      }
    }
    // The sequencer may have been shutdown to close the program. If so,
    // we need to continue shutdown process
    if ((ev.eventMask &
            jvxSequencerEventBitfield.JVX_SEQUENCER_EVENT_PROCESS_TERMINATED) !=
        0) {
      // Then, update the seq status
      // seqStat = ev.seq_state_id!.immediateSequencerStatus();
      seqStat = ev.statSeq;

      // Here, we have seen the PROCESS_TERMINATED event
      // First acknowledge the stop, then it returns the status AFTER acknowledge
      seqStat = backendAdapterIf!.acknowledgeSequencerStop();

      // Finally check for elements in queue
      var retVal = AudYoFloCommandQueueElement.AYF_COMMAND_QUEUE_NONE;
      do {
        retVal = backendAdapterIf!.postSequencerStop();

        // We can run any action when the sequencer shuts down!
        switch (retVal) {
          default:
            break;
        }
      } while (retVal != AudYoFloCommandQueueElement.AYF_COMMAND_QUEUE_NONE);
    }

    if (sequencerSection.seqStatus != seqStat) {
      sequencerSection.seqStatus = seqStat;
      sequencerSection.ssUpdateId++;

      // This trigger will run the notify
      backendAdapterIf!.triggerUpdateProcessStatus();
      // triggerNotify();
    }
    return errCode;
  }

  @override
  String debugPrintProcess(int uId) {
    return processSection.theRegisteredProcesses.debugPrintProcess(uId);
  }

  @override
  int updateIdComponentInCache(JvxComponentIdentification cpId) {
    int retVal = -1;
    AudYoFloOneSelectedComponent? cpElm = findSelectedComponent(cpId);
    if (cpElm != null) {
      retVal = cpElm.propertyCache.ssPropertyCache;
    }
    return retVal;
  }

  @override
  int updateIdPropertyComponentInCache(
      JvxComponentIdentification cpId, String descriptor) {
    int retVal = -1;
    AudYoFloPropertyContainer? propCont;
    AudYoFloOneSelectedComponent? cpElm = findSelectedComponent(cpId);
    if (cpElm != null) {
      propCont = cpElm.propertyCache.findCachedProperty(descriptor);
      if (propCont != null) {
        retVal = propCont.ssUpdateId;
      }
    }
    return retVal;
  }

  @override
  int updateEntrySinglePropertyCache(
      JvxComponentIdentification cpId, AudYoFloPropertyContentBackend newProp) {
    int res = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    AudYoFloOneSelectedComponent? cpElm = findSelectedComponent(cpId);
    if (cpElm != null) {
      res = jvxErrorType.JVX_NO_ERROR;
      AudYoFloPropertyContainer? propCont =
          cpElm.propertyCache.findCachedProperty(newProp.descriptor);
      if (propCont != null) {
        // Exchange this property
        propCont = newProp;
        propCont.ssUpdateId++;
      } else {
        // Attach this property
        cpElm.propertyCache.cachedProperties[newProp.descriptor] = newProp;
      }
    }

    return res;
  }

  @override
  int updatePropertyCacheCompleteNotify(JvxComponentIdentification cpId,
      {AyfPropertyReportLevel report =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY}) {
    int res = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    AudYoFloOneSelectedComponent? cpElm = findSelectedComponent(cpId);
    if (cpElm != null) {
      res = jvxErrorType.JVX_NO_ERROR;

      // We may update also the property cache id for the current component
      if (report ==
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_COMPONENT_PROPERTY) {
        cpElm.propertyCache.ssPropertyCache++;
      }
      if ((report ==
              AyfPropertyReportLevel.AYF_FRONTEND_REPORT_COMPONENT_PROPERTY) ||
          (report ==
              AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY)) {
        triggerNotify();
      }
    }
    return res;
  }

  @override
  List<int> findProcessesMatchComponent(JvxComponentIdentification cpId,
      {List<int>? lstIn}) {
    List<int> lst = [];
    if (lstIn != null) {
      lst = List.from(lstIn);
    }
    for (var elm
        in processSection.theRegisteredProcesses.reportedProcesses.entries) {
      AudYoFloOneConnectedProcess con = elm.value;
      var involvedElementStart = con.involved;
      bool foundElm = false;
      if (involvedElementStart != null) {
        foundElm = involvedElementStart.findProcessMatchComponent(cpId);
      }
      if (foundElm) {
        lst.add(con.uId);
      }
    }
    // con.involved!.next;
    return lst;
  }

  @override
  List<JvxComponentIdentification> findComponentsMatchProcess(int processId,
      {List<JvxComponentIdentification>? lstIn,
      bool processIdLinearAddress = true,
      List<String> lstRegExprMatch = const [],
      List<String>? lstDescriptionsReturn}) {
    List<JvxComponentIdentification> lst = [];
    if (lstIn != null) {
      lst = List.from(lstIn);
    }
    // Collect all data from active components
    if (processIdLinearAddress) {
      if (processId >= 0) {
        if (processId <
            processSection
                .theRegisteredProcesses.reportedProcesses.entries.length) {
          var entry = processSection
              .theRegisteredProcesses.reportedProcesses.entries
              .elementAt(processId);
          AudYoFloOneConnectedProcess con = entry.value;
          con.involved?.findComponentsMatch(
              lst, componentSection.theSelectedComponents.selectedComponents,
              lstCheck: lstRegExprMatch, lstDescriptors: lstDescriptionsReturn);
        } else {
          processId = -1;
        }
      }
      if (processId == -1) {
        componentSection.theSelectedComponents.selectedComponents
            .forEach((k, v) {
          bool addElement = true;
          JvxComponentIdentification cpId = k;
          for (var elm in lst) {
            if (elm == cpId) {
              addElement = false;
              break;
            }
          }

          if (addElement) {
            lst.add(cpId);
          }
        });
      }
    } else {
      var elmP = processSection.theRegisteredProcesses.reportedProcesses.entries
          .firstWhereOrNull((element) => element.key == processId);
      if (elmP != null) {
        AudYoFloOneConnectedProcess con = elmP.value;
        con.involved?.findComponentsMatch(
            lst, componentSection.theSelectedComponents.selectedComponents,
            lstCheck: lstRegExprMatch, lstDescriptors: lstDescriptionsReturn);
      }
    }
    return lst;
  }

  @override
  List<String> availProcessDescriptions({List<String> lst = const []}) {
    processSection.theRegisteredProcesses.reportedProcesses.forEach((k, v) {
      AudYoFloOneConnectedProcess vProc = v;
      String oneEntry = '';
      var uId = vProc.uId;
      var catId = vProc.catId;
      oneEntry = vProc.nameProcess;
      oneEntry += '<$uId>';
      oneEntry += '[$catId]';
      lst.add(oneEntry);
    });
    return lst;
  }

  @override
  String descriptionComponent(JvxComponentIdentification cpTp) {
    String retVal = 'Unknown';
    AudYoFloOneSelectedComponent? comp = findSelectedComponent(cpTp);
    if (comp != null) {
      retVal = comp.description;
    }
    return retVal;
  }

  @override
  String moduleNameComponent(JvxComponentIdentification cpTp) {
    String retVal = 'Unknown';
    AudYoFloOneSelectedComponent? comp = findSelectedComponent(cpTp);
    if (comp != null) {
      retVal = comp.moduleName;
    }
    return retVal;
  }

  @override
  Future<int> triggerActivateComponent(JvxComponentIdentification cpId, int id,
      {bool selectBeforeActivate = true}) async {
    int errCode = await backendAdapterIf!
        .triggerActivateComponent(cpId, id, selectBeforeActivate);

    // All following messages will be reported by events
    return errCode;
  }

  @override
  Future<int> triggerDeactivateComponent(JvxComponentIdentification cpId,
      {bool unselect = true}) async {
    int errCode =
        await backendAdapterIf!.triggerDeactivateComponent(cpId, unselect);
    return errCode;
  }

  // This function returns a list of the requested properties from cache.
  // Each entry in the cache is evaluated if is valid. If any is not in cache
  // the function returns a null list.
  @override
  AudYoFloPropertyContainer? referencePropertyInCache(
      JvxComponentIdentification cpId, String theProp) {
    AudYoFloPropertyContainer? retVal;
    AudYoFloOneSelectedComponent? actComponent = findSelectedComponent(cpId);
    if (actComponent != null) {
      retVal = actComponent.propertyCache.findCachedProperty(theProp);
    }
    return retVal;
  }

  // ======================================================================

  // Property list functions
  @override
  Future<int> triggerUpdatePropertyListComponent(
      JvxComponentIdentification cpId) async {
    int errCode = jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
    if (cpId.cpTp != JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN)
      errCode =
          await backendAdapterIf!.triggerUpdatePropertyListComponent(cpId);
    return errCode;
  }

  @override
  int updatePropertyCacheListCompleteNotify(
      JvxComponentIdentification cpId, List<String> lst,
      {AyfPropertyReportLevel report =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_COMPONENT_PROPERTY}) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    AudYoFloOneSelectedComponent? actComponent = findSelectedComponent(cpId);
    if (actComponent != null) {
      actComponent.propertyCache.availProperties = lst;
      updatePropertyCacheCompleteNotify(cpId, report: report);
    } else {
      errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    }
    return errCode;
  }

  // Get the device list as stored in cache. If the device list is not
  // available return an empty (null) list
  @override
  List<AudYoFloOneDeviceSelectionOption>? referenceDeviceListInCache(
      JvxComponentIdentification cpId) {
    // Check that the type is really a registered technology
    List<AudYoFloOneDeviceSelectionOption>? options;
    if (!checkComponentIdentTechnology(cpId)) {
      /*
      throw AudYoFloException(jvxErrorType.JVX_ERROR_INVALID_SETTING,
          'Function to obtain devices called on a component type other than a technology.');*/
      return null;
    }
    // Here, we have found the technology in list of ids in the technology list

    // Find the device list as stored for this technology
    // A technology is defined by comp type and slotid
    MapEntry<JvxComponentIdentification,
            AudYoFloOneSelectionOptionEntry<AudYoFloOneDeviceSelectionOption>>?
        entry = deviceOptionMap.selectionOptionCacheMap.entries
            .firstWhereOrNull((element) =>
                (element.key.cpTp == cpId.cpTp) &&
                (element.key.slotid == cpId.slotid));
    if (entry != null) {
      if (entry.value.options.valid) {
        // We have identified the right technology and return the VALID device list
        options = entry.value.options.optionList;
      }
      // else: device list is invalid and can not be returned
    }

    return options;
  }

  // Refresh devices list in cache
  @override
  Future<int> triggerUpdateDeviceList(JvxComponentIdentification cpId) async {
    int errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    if (cpId.cpTp != JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      if (backendAdapterIf == null) {
        errCode = jvxErrorType.JVX_ERROR_NOT_READY;
      } else {
        errCode = await backendAdapterIf!.triggerUpdateDeviceList(cpId);
      }
    }
    return errCode;
  }

  // From backend adapter, provide new devices list and inform all listeners
  // Since the notify is triggered, this call must come in asyncronously
  @override
  int updateDeviceListCacheCompleteNotify(JvxComponentIdentification cpId,
      List<AudYoFloOneDeviceSelectionOption> devices) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    MapEntry<JvxComponentIdentification,
            AudYoFloOneSelectionOptionEntry<AudYoFloOneDeviceSelectionOption>>?
        entry = deviceOptionMap.selectionOptionCacheMap.entries
            .firstWhereOrNull((element) =>
                (element.key.cpTp == cpId.cpTp) &&
                (element.key.slotid == cpId.slotid));
    if (entry != null) {
      // There was a device list before, just update it
      entry.value.options.optionList = devices;
      entry.value.options.valid = true;
      entry.value.options.ssUpdateId += 1;
    } else {
      // This is the first time to create a device list
      AudYoFloOneSelectionOptionEntry<AudYoFloOneDeviceSelectionOption>
          newEntry =
          AudYoFloOneSelectionOptionEntry<AudYoFloOneDeviceSelectionOption>();
      newEntry.options.optionList = devices;
      newEntry.options.valid = true;
      newEntry.options.ssUpdateId = 1;
      deviceOptionMap.selectionOptionCacheMap[cpId] = newEntry;
    }

    if (errCode == jvxErrorType.JVX_NO_ERROR) {
      triggerNotify();
    }
    return errCode;
  }

  @override
  int updateIdDeviceListInCache(JvxComponentIdentification cpId) {
    int retVal = -1;
    MapEntry<JvxComponentIdentification,
            AudYoFloOneSelectionOptionEntry<AudYoFloOneDeviceSelectionOption>>?
        entry = deviceOptionMap.selectionOptionCacheMap.entries
            .firstWhereOrNull((element) =>
                (element.key.cpTp == cpId.cpTp) &&
                (element.key.slotid == cpId.slotid));
    if (entry != null) {
      retVal = entry.value.options.ssUpdateId;
    }
    return retVal;
  }

  // Return a selection list from cache. If none exists return an empty list
  @override
  List<AudYoFloOneComponentSelectionOption>? referenceComponentListInCache(
      JvxComponentIdentification cpId) {
    // Check that the type is really a registered technology
    List<AudYoFloOneComponentSelectionOption>? options;
    if (checkComponentIdentDevice(cpId)) {
      // It is a device, hence, we select option based on type and slotid
      MapEntry<
          JvxComponentIdentification,
          AudYoFloOneSelectionOptionEntry<
              AudYoFloOneComponentSelectionOption>>? entry = componentOptionMap
          .selectionOptionCacheMap.entries
          .firstWhereOrNull((element) =>
              (element.key.cpTp == cpId.cpTp) &&
              (element.key.slotid == cpId.slotid));
      if (entry != null) {
        if (entry.value.options.valid) {
          // We have identified the right technology and return the VALID device list
          options = entry.value.options.optionList;
        }
        // else: device list is invalid and can not be returned
      }
    } else {
      // Technology or node, only type is of relevance
      MapEntry<
          JvxComponentIdentification,
          AudYoFloOneSelectionOptionEntry<
              AudYoFloOneComponentSelectionOption>>? entry = componentOptionMap
          .selectionOptionCacheMap.entries
          .firstWhereOrNull((element) => (element.key.cpTp == cpId.cpTp));
      if (entry != null) {
        if (entry.value.options.valid) {
          // We have identified the right technology and return the VALID device list
          options = entry.value.options.optionList;
        }
        // else: device list is invalid and can not be returned
      }
    }

    return options;
  }

  // Trigger an update of the selection list in the backend adapter
  @override
  Future<int> triggerUpdateComponentList(
      JvxComponentIdentification cpId) async {
    bool isDevice = checkComponentIdentDevice(cpId);
    int errCode =
        await backendAdapterIf!.triggerUpdateComponentList(cpId, isDevice);
    return errCode;
  }

// From backend adapter, provide new devices list and inform all listeners
  // Since the notify is triggered, this call must come in asyncronously
  @override
  int updateComponentListCacheCompleteNotify(JvxComponentIdentification cpId,
      List<AudYoFloOneComponentSelectionOption> options, bool isDevice) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    if (isDevice) {
      // If device, we need to compare tp and slotid
      MapEntry<
          JvxComponentIdentification,
          AudYoFloOneSelectionOptionEntry<
              AudYoFloOneComponentSelectionOption>>? entry = componentOptionMap
          .selectionOptionCacheMap.entries
          .firstWhereOrNull((element) =>
              (element.key.cpTp == cpId.cpTp) &&
              (element.key.slotid == cpId.slotid));
      if (entry != null) {
        // There was a device list before, just update it
        entry.value.options.optionList = options;
        entry.value.options.valid = true;
        entry.value.options.ssUpdateId += 1;
      } else {
        // This is the first time to create a device list
        AudYoFloOneSelectionOptionEntry<AudYoFloOneComponentSelectionOption>
            newEntry = AudYoFloOneSelectionOptionEntry<
                AudYoFloOneComponentSelectionOption>();
        newEntry.options.optionList = options;
        newEntry.options.valid = true;
        newEntry.options.ssUpdateId = 1;
        componentOptionMap.selectionOptionCacheMap[cpId] = newEntry;
      }
    } else {
      // Node or technology: only use the type
      MapEntry<
          JvxComponentIdentification,
          AudYoFloOneSelectionOptionEntry<
              AudYoFloOneComponentSelectionOption>>? entry = componentOptionMap
          .selectionOptionCacheMap.entries
          .firstWhereOrNull((element) => (element.key.cpTp == cpId.cpTp));
      if (entry != null) {
        // There was a device list before, just update it
        entry.value.options.optionList = options;
        entry.value.options.valid = true;
        entry.value.options.ssUpdateId += 1;
      } else {
        // This is the first time to create a device list
        AudYoFloOneSelectionOptionEntry<AudYoFloOneComponentSelectionOption>
            newEntry = AudYoFloOneSelectionOptionEntry<
                AudYoFloOneComponentSelectionOption>();
        newEntry.options.optionList = options;
        newEntry.options.valid = true;
        newEntry.options.ssUpdateId = 1;
        componentOptionMap.selectionOptionCacheMap[cpId] = newEntry;
      }
    }

    if (errCode == jvxErrorType.JVX_NO_ERROR) {
      triggerNotify();
    }
    return errCode;
  }

  @override
  void triggerStartSequencer() {
    backendAdapterIf!.triggerStartSequencer();
  }

  // Trigger stop of the sequencer
  @override
  void triggerStopSequencer() {
    backendAdapterIf!.triggerStopSequencer();
  }

  @override
  void triggerToggleSequencer() {
    backendAdapterIf!.triggerStopSequencer();
  }

  @override
  Future<String> transferTextCommand(String textArg, bool jsonOut) {
    return backendAdapterIf!.transferTextCommand(textArg, jsonOut);
  }

  @override
  Future<int> triggerSaveConfig() async {
    return await backendAdapterIf!.triggerSaveConfig();
  }

  @override
  void reportPropertyListSetBackend(
      JvxComponentIdentification cpId, String propLst) {
    // print('Report property set <$propLst>');
    List<String> props = helpers.str2PropList(propLst);

    // Invalidate properties
    invalidatePropertiesComponent(cpId, props, true);
    updatePropertyCacheCompleteNotify(cpId,
        report: AyfPropertyReportLevel.AYF_FRONTEND_REPORT_COMPONENT_PROPERTY);
  }

  @override
  Future<void> triggerClose() async {
    if (backendAdapterIf != null) {
      await backendAdapterIf!.triggerClose();
    }
  }

  @override
  void reportSystemStateRestart() {
    ssUpdateId++;
    // Navigator.pushNamed(context, '/second');
    triggerNotify();
  }
}
