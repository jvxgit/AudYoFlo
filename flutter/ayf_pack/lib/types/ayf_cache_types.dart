import '../ayf_pack_local.dart';

// ================================================
// ================================================
// Define some cache subsections
// ================================================
// ================================================

class AudYoFloStateCache {
  bool valid = false;
  int ssUpdateId = 0;
}

class AudYoFloComponentCache extends AudYoFloStateCache {
  // Here, we store all selected components
  AudYoFloSelectedComponents theSelectedComponents =
      AudYoFloSelectedComponents();
  void fullClear() {
    theSelectedComponents.selectedComponents.clear();
  }
}

// Sequencer cache
class AudYoFloSequencerStateCache extends AudYoFloStateCache {
  jvxSequencerStatusEnum seqStatus =
      jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;
  bool processesReady = false;
  String reasonIfNot = '';

  void fullClear() {
    seqStatus = jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;
    processesReady = false;
    reasonIfNot = '';
  }
}

/*
class AudYoFloComponentStateCache extends AudioFlowStateCache {
  Map<JvxComponentIdentification, AudYoFloOneComponentSelectionOptions>
      componentCacheMap = {};
}
*/

class AudYoFloDeviceSelectionMapCache {
  // Node and device: Option for type
  // Technology: Oe entry for type + slotid
  Map<JvxComponentIdentification,
          AudYoFloOneSelectionOptionEntry<AudYoFloOneDeviceSelectionOption>>
      selectionOptionCacheMap = {};
  void fullClear() {
    selectionOptionCacheMap.clear();
  }
}

class AudYoFloComponentSelectionMapCache {
  // Node and device: Option for type
  // Technology: Oe entry for type + slotid
  Map<JvxComponentIdentification,
          AudYoFloOneSelectionOptionEntry<AudYoFloOneComponentSelectionOption>>
      selectionOptionCacheMap = {};

  void fullClear() {
    selectionOptionCacheMap.clear();
  }
}

// System cache
class AudYoFloProcessesCache extends AudYoFloStateCache {
  // Here, we store all processes
  AudYoFloConnectedProcesses theRegisteredProcesses =
      AudYoFloConnectedProcesses();

  void fullClear() {
    theRegisteredProcesses.fullClear();
  }
}
