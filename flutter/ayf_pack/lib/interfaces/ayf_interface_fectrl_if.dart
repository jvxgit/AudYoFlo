import '../ayf_pack_local.dart';

abstract class AudYoFloBackendCacheFectrlIf {
  // Explicitely set a property to be invalid
  void invalidatePropertiesComponent(
      JvxComponentIdentification cpId, List<String> props, bool contentOnly,
      {bool invalidateAll = false});

  // Return the list of properties with valid content
  List<AudYoFloPropertyContainer>? referenceValidPropertiesComponents(
      JvxComponentIdentification cpId, List<String> props);

  // Update one or more properties to be stored in cache.
  Future<int> triggerUpdatePropertiesComponent(
      JvxComponentIdentification cpId, List<String> propDescr,
      {AyfPropertyReportLevel report =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY,
      bool forceDescriptor = false,
      bool forceContent = false});

  // Send a list of new property values to backend. The properties will be
  // invalidated on set since a new value must be read for read-back before
  // exposing to UI
  Future<int> triggerSetProperties(
      JvxComponentIdentification cpId, List<String> propContents,
      {bool invalidateProperty = true,
      int offset = 0,
      int num = -1,
      bool preventCallMultipleTimes = false,
      AyfPropertyReportLevel report =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_NO_REPORT});

  // =========================================================================

  void triggerStartSequencer();

  // Trigger stop of the sequencer
  void triggerStopSequencer();

  void triggerToggleSequencer();

  Future<String> transferTextCommand(String textArg, bool jsonOut);

  // Get a list of strings to list all properties for a specific component
  // The list is returned as null if there is no valid property list
  // If it is null, you can call <triggerUpdatePropertyListComponent> to create a
  // list
  List<String>? referencePropertyListInCache(JvxComponentIdentification cpId);

  // Trigger an update of the property list in cache.
  Future<int> triggerUpdatePropertyListComponent(
      JvxComponentIdentification cpId);

// ==========================================================================
  // We had 2 interfaces at this point earlier
  // ==========================================================================

  // ================================================================
  // HELPER FUNCTIONS
  List<JvxComponentIdentification> findComponentsMatchProcess(int processId,
      {List<JvxComponentIdentification>? lstIn,
      bool processIdLinearAddress = true,
      List<String> lstRegExprMatch = const [],
      List<String>? lstDescriptionsReturn});

  List<int> findProcessesMatchComponent(JvxComponentIdentification cpId,
      {List<int>? lstIn});

  List<String> availProcessDescriptions({List<String> lst});

  String descriptionComponent(JvxComponentIdentification cpTp);

  String moduleNameComponent(JvxComponentIdentification cpTp);

  // =======================================================================
  // Activate a new component specified by the id.
  // The activated component will be reported via the "report" callbacks
  Future<int> triggerActivateComponent(JvxComponentIdentification cpId, int id,
      {bool selectBeforeActivate = true});

  Future<int> triggerDeactivateComponent(JvxComponentIdentification cpId,
      {bool unselect = true});

  // =======================================================================

  // Function to update the process status. The backend will drive this call
  // in async manner and generate a notify on the backend cache.
  Future<int> triggerUpdateProcessStatus();

  // =======================================================================

  int updateIdComponentInCache(JvxComponentIdentification cpId);

  int updateIdPropertyComponentInCache(
      JvxComponentIdentification cpId, String propDescr);

  int updateIdDeviceListInCache(JvxComponentIdentification cpId);

  // =========================================================================

  // Function to receive a device list from cache. If no device list present, you can call
  // triggerUpdateDeviceList.
  List<AudYoFloOneDeviceSelectionOption>? referenceDeviceListInCache(
      JvxComponentIdentification cpId);

  // Function to trigger a device list readout to cache
  Future<int> triggerUpdateDeviceList(JvxComponentIdentification cpId);

  // =========================================================================

  // Function to receive a componnt list from cache. If the component list is null
  // you can trigger an update using the function triggerUpdateComponentList.
  List<AudYoFloOneComponentSelectionOption>? referenceComponentListInCache(
      JvxComponentIdentification cpId);

  // Function to update the component list in cache
  Future<int> triggerUpdateComponentList(JvxComponentIdentification cpId);

  // =========================================================================
  Future<int> triggerSaveConfig();
}
