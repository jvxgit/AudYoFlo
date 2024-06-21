import '../ayf_pack_local.dart';

abstract class AudYoFloBackendCacheBectrlIf {
  // ====================================================================
  // ====================================================================
  // Report a new component that pops up in the system due to a select
  // operation.
  void reportSelectedComponent(JvxComponentIdentification cpId, String descr,
      {AudYoFloDirectPropertiesIf? directPropRef});

  // Report a component that has been removed from the system due to an
  // unselect operation
  void reportUnselectedComponent(JvxComponentIdentification cpId);

  // Function to invalidate the component list in cache. This function is
  // principally called if a component that matches the type/id is selected
  // or unselected. In particular, it is important to invalidate since the
  // location with slots and subslots is affected.
  void invalidateComponentListCache(JvxComponentIdentification cpId);

  // ==========================================================================
  // We had 2 interfaces at this point earlier
  // ==========================================================================

  // Function to add a new process uId. This function is called from backend
  // if a new process is reported asynchronously.
  void addProcess(int uId);

  // Function to remove a process uId. This function is called from backend
  // if a removed process is reported asynchronously.
  void remProcess(int uId);

  // ====================================================================

  // Function to update the process status. The process status allows to
  // decide if processing may be started. The backend will forward this
  // request in async manner and generate a notify on the backend cache.
  // This function can be addressed from frontend as well as the backend
  Future<int> triggerUpdateProcessStatus();

  // Function to conclude the update the status of the system. This function
  // notifies all listeners
  void updateProcessStatusCompleteNotify(bool procStatus, String reasonIfNot);

  // ====================================================================
  // ====================================================================

  // Report if the state of a component has changed.
  void reportStateSwitchComponent(
      JvxComponentIdentification cpId, jvxStateSwitchEnum ss);

  // Report that the status of a component has changed. A status change is
  // reported if the event <JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT>
  // comes from the connected backend. This event is reported if the INTERNAL
  // status of a component has changed, it is independent of the component
  // state switch.
  void reportUpdateStatusComponent(JvxComponentIdentification cpId);

  // Report a sequencer event. This report comes from the backend adapter
  // directly and may be enter from a websocket connection or an async local
  // event.
  int reportSequencerEvent(
      jvxSequencerEventType ev, jvxSequencerStatusEnum seqStat);

  // ====================================================================
  // ====================================================================

  List<AudYoFloPropertyContainer>? referenceValidPropertiesComponents(
      JvxComponentIdentification cpId, List<String> props);

  // Properties can
  Future<int> triggerUpdatePropertiesComponent(
      JvxComponentIdentification cpId, List<String> propDescr,
      {AyfPropertyReportLevel report =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY,
      bool forceDescriptor = false,
      bool forceContent = false});

  // ====================================================================
  // ====================================================================

  // Update one property entry in the property cache. This call will only
  // update the content, it does NOT invoke a notify event but it increases the
  // update id of the property.
  int updateEntrySinglePropertyCache(
      JvxComponentIdentification cpId, AudYoFloPropertyContentBackend newProp);

  // Acknowledge an update of the latest property interaction. It may increase
  // the component specific update id (argument report) and it calls notify
  // to push a signal with an updated backend cache.
  int updatePropertyCacheCompleteNotify(JvxComponentIdentification cpId,
      {AyfPropertyReportLevel report =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY});

  // Get a reference to a property from the backend to refresh the actual
  // content. The caller of this function is responsible to inform the app via
  // emission of notify events
  AudYoFloPropertyContainer? referencePropertyInCache(
      JvxComponentIdentification cpId, String descriptor);

  // ====================================================================
  // ====================================================================
  // Function to report a new version of the property list in cache.
  // This call will trigger the notify function.
  int updatePropertyCacheListCompleteNotify(
      JvxComponentIdentification cpId, List<String> lst,
      {AyfPropertyReportLevel report =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_COMPONENT_PROPERTY});

  // Function to invalidate the list of properties as given in the argument
  // props. The properties may be invalidated regarding the content of the
  // property (lite version) or the overall property. This function does
  // not trigger a notify. Properties can be invaliodated from frontend as well
  // as from backend. Typically, an invalidation ends with a call to
  // <updatePropertyCacheCompleteNotify> to trigger a notify.
  void invalidatePropertiesComponent(
      JvxComponentIdentification cpId, List<String> props, bool contentOnly,
      {bool invalidateAll = false});

  // ====================================================================
  // ====================================================================

  // Function to update the device list in cache and notify the listeners.
  // The device list previously stored in cache is replaced by the passed
  // list. Note that a device list is obtained for a technology, the cpId
  // is specified by means of the type and the slot id.
  int updateDeviceListCacheCompleteNotify(JvxComponentIdentification cpId,
      List<AudYoFloOneDeviceSelectionOption> options);

  // I added this function today: we need to get reference to the list of
  // devices also from the backend adapter if, e.g., a technology requests
  // default activation of a specific device
  List<AudYoFloOneDeviceSelectionOption>? referenceDeviceListInCache(
      JvxComponentIdentification cpId);

  // Function to update the list of selectable components and notify the
  // listeners. The list of components depends on the component type for
  // technologies and nodes and on the slotid for the device components.
  // A previously stored list is replaced by the call.
  int updateComponentListCacheCompleteNotify(JvxComponentIdentification cpId,
      List<AudYoFloOneComponentSelectionOption> options, bool isDevice);

  // ====================================================================
  // ====================================================================

  // Function to report list of properties that have recently been modified
  // This call reuires the config to enable report on every property set
  void reportPropertyListSetBackend(
      JvxComponentIdentification cpId, String propLst);

  // ====================================================================
  // ====================================================================

  void triggerClose();

  // If some calls this function, the host needs to restart!!
  void reportSystemStateRestart();

  // More kind of helper functions

  // Function to check if a component is of type "technology".
  bool checkComponentIdentTechnology(JvxComponentIdentification cpId);

  // Function to check if a component is of type "device".
  bool checkComponentIdentDevice(JvxComponentIdentification cpId);

  // Function to create a string to represent a process.
  String debugPrintProcess(int uId);
}
