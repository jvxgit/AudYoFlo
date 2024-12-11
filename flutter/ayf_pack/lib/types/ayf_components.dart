import '../ayf_pack_local.dart';

class JvxComponentIdentification {
  JvxComponentTypeEnum cpTp; // = jvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN;
  int slotid; // = -1;
  int slotsubid; // = -1;
  int uid; // = -1;

  static JvxComponentIdentification from(JvxComponentIdentification cpFrom) {
    JvxComponentIdentification returnme = JvxComponentIdentification();
    returnme.cpTp = cpFrom.cpTp;
    returnme.slotid = cpFrom.slotid;
    returnme.slotsubid = cpFrom.slotsubid;
    returnme.uid = cpFrom.slotsubid;
    return returnme;
  }

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

  String get txtshort {
    String ret = "";
    if (slotid > 0) {
      ret = "<$slotid";
      if (slotsubid > 0) {
        ret = "$ret,$slotsubid>";
      } else {
        ret = "$ret>";
      }
    } else {
      if (slotsubid > 0) {
        ret = "<0,$slotsubid>";
      }
    }
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
