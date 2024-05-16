import '../ayf_pack_local.dart';

class JvxComponentIdentification {
  JvxComponentTypeEnum cpTp; // = jvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN;
  int slotid; // = -1;
  int slotsubid; // = -1;
  int uid; // = -1;
  JvxComponentIdentification(
      {this.cpTp = JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN,
      this.slotid = -1,
      this.slotsubid = -1,
      this.uid = -1});

  void reset() {
    cpTp = JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN;
    slotid = -1;
    slotsubid = -1;
    uid = -1;
  }

  String get txt {
    String ret = cpTp.txt;
    ret += '<$slotid,$slotsubid>';
    return ret;
  }

  bool operator ==(Object arg) {
    if (arg is JvxComponentIdentification) {
      JvxComponentIdentification argc = arg;
      if ((argc.cpTp == cpTp) &&
          (argc.slotid == slotid) &&
          (argc.slotsubid == slotsubid)) {
        return true;
      }
    }
    return false;
  }
}
