import '../ayf_pack_local.dart';

class AudYoFloOneSelectionOptionBase {
  String description = 'not-set';
  String descriptor = 'not-set';
  int optionIdx = -1;
}

// Class tp hold the status of a single device to be shown in
// device list.
class AudYoFloOneDeviceSelectionOption extends AudYoFloOneSelectionOptionBase {
  jvxDeviceDataFlowTypeEnum flow =
      jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_UNKNOWN;
  AudYoFloCBitField32 caps = AudYoFloCBitField32();
  AudYoFloCBitField32 flags = AudYoFloCBitField32();
  bool selectable = false;
  JvxComponentIdentification devIdent = JvxComponentIdentification();
  AudYoFloCBitField32 state = AudYoFloCBitField32();
  bool singleton = false;
}

class AudYoFloOneDeviceSelectionOptionWithId {
  AudYoFloOneDeviceSelectionOption option;
  int idx;
  AudYoFloOneDeviceSelectionOptionWithId(this.option, this.idx);
}

class AudYoFloOneDeviceSelectionOptionWithIdAndProcess
    extends AudYoFloOneDeviceSelectionOptionWithId {
  AudYoFloOneConnectedProcess? proc;
  List<AudYoFloOneComponentWithDetail> relChainComponents = [];
  AudYoFloOneDeviceSelectionOptionWithIdAndProcess(
      AudYoFloOneDeviceSelectionOption option, int idx)
      : super(option, idx) {}
}

class AudYoFloOneComponentSelectionOption
    extends AudYoFloOneSelectionOptionBase {
  List<int> sids = [];
}

class AudYoFloOneSelectionOptionCache<T> extends AudYoFloStateCache {
  List<T> optionList = [];
}

class AudYoFloOneSelectionOptionEntry<T> {
  // This is the list of devices for a technology
  AudYoFloOneSelectionOptionCache<T> options =
      AudYoFloOneSelectionOptionCache<T>();

  // This is the list of options
  //AudYoFloOneSelectionOptionCache<T> options =
  //    AudYoFloOneSelectionOptionCache<T>();
}
