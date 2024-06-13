import '../ayf_pack_local.dart';

abstract class AudYoFloBackendTranslator {
  //String translateComponent(jvxComponentIdentification elm);
  // Translate an enum string into an int (and from there into a real enum).
  // We indeed involve the backend for this since the lookup is stored in the
  // backend. However, in the future the enum lookup should NOT involve a web
  // connection if using a web control.
  int translateEnumString(
      String selection, String formatName, AudYoFloCompileFlags flags);

  // Obtain compile falgs
  AudYoFloCompileFlags compileFlags();
}

/*
 * Interface to address the backend from GUI or backend cache
 */ //========================================================
abstract class AudYoFloBackendAdapterIf extends AudYoFloBackendTranslator {
  // ==================================
  //// Interface invoked from application
  // ==================================

  // Trigger start of the sequencer
  Future<int> triggerStartSequencer();

  // Trigger stop of the sequencer
  Future<int> triggerStopSequencer();

  // If sequencer is stopped, start the sequencer,
  // if sequencer is started, stop the sequencer
  Future<int> triggerToggleSequencer();

  // Obtain the current sequencer status
  Future<jvxSequencerStatusEnum> updateStatusSequencer();

  // ==================================

  // Transfer a single text command. The result may be returned delayed.
  Future<String> transferTextCommand(String textArg, bool jsonOut);

  // ==================================

  // Forward of cache update functions towards the backend
  // https://stackoverflow.com/questions/53249859/async-await-not-waiting-before-future-dart
  Future<int> triggerUpdateAvailComponents(JvxComponentIdentification cpId,
      List<AudYoFloOneComponentOptions> updateMe);

  // ===============================================================================
  // Change component states
  // ===============================================================================

  // Forward of cache update functions towards the backend
  Future<int> triggerActivateComponent(
      JvxComponentIdentification cpId, int id, bool select);

  // Forward of cache update functions towards the backend
  Future<int> triggerDeactivateComponent(
      JvxComponentIdentification cpId, bool unselect);

  // ===============================================================================

  // Trigger call to read out the current process status. This call will be completed
  // with an update of the cache value and a notify
  Future<int> triggerUpdateProcessStatus();

  // ===============================================================================

  // Forward of cache update functions towards the backend
  Future<int> triggerUpdateDeviceList(JvxComponentIdentification cpId);

  // Forward of cache update functions towards the backend
  Future<int> triggerUpdateComponentList(
      JvxComponentIdentification cpId, bool isDevice);

  // Forward of cache update functions towards the backend
  Future<int> triggerSaveConfig();

  void reportPropertyListSetBackend(
      JvxComponentIdentification cpId, String propLst);
  // !! OPTIMIZE THOSE OUT!!

  //=========================================================================
  // These functions are run whenever the sequencer has completed. Then, we need to
  // check an action queue to find pending requests
  //=========================================================================

  // Acknowledge the sequencer stop event. In the end, the stop must be
  // acknowledged to reset the state in the backend. We may change this.
  // We do not need this in web connection but it is used in native host
  jvxSequencerStatusEnum acknowledgeSequencerStop();

  // Immediate request of the sequencer status. We need this to find out if the seq
  // status has changed
  // jvxSequencerStatusEnum requestSequencerStatus_();

  // This step is run after the command to stop the sequencer
  // has been triggered, the end of sequencer was acknowledged.
  // We may run steps from a queue, therefore, this function returns
  // jvxFernliveCommandQueueElement elements until there is none left.
  AudYoFloCommandQueueElement postSequencerStop();

  //=========================================================================

  // Immediate request of the process - CAN WE OPTIMIZE THIS OUT?
  AudYoFloOneConnectedProcess allocateProcess(int uId);

  String debugPrintProcess(int uId);

  Future<void> triggerClose();

  //void reportUpdateProcessStatus();
  // !! OPTIMIZE THOSE OUT!!

  // ============================================================================
  // ============================================================================

  // Translate component type to a component type class
  jvxComponentTypeClassEnum lookupComponentClass(JvxComponentTypeEnum tp);

  // Forward of cache update functions towards the backend
  Future<int> triggerGetPropertyDescriptionComponent(
      JvxComponentIdentification cpId, List<String> propDescr,
      {AyfPropertyReportLevel reportArg =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_COMPONENT_PROPERTY});

  // Forward of cache update functions towards the backend
  Future<int> triggerGetPropertyContentComponent(
      JvxComponentIdentification cpId, List<String> propDescr,
      {AyfPropertyReportLevel reportArg =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_SINGLE_PROPERTY});

  // Forward of cache update functions towards the backend
  Future<int> triggerSetProperties(
      JvxComponentIdentification cpId, List<String> propContents,
      {bool invalidateProperty = true,
      int offset = 0,
      int num = -1,
      bool preventCallMultipleTimes = false,
      AyfPropertyReportLevel reportArg =
          AyfPropertyReportLevel.AYF_BACKEND_REPORT_COMPONENT_PROPERTY});

  // Forward of cache update functions towards the backend
  Future<int> triggerUpdatePropertyListComponent(
      JvxComponentIdentification cpId,
      {AyfPropertyReportLevel reportArg =
          AyfPropertyReportLevel.AYF_FRONTEND_REPORT_COMPONENT_PROPERTY});

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
      int desired_ping_granularity = 10});

  Future<int> triggerStopPropertyStream();

  String get lastError;
}
