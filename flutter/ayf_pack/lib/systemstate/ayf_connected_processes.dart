import 'package:collection/collection.dart';
import '../ayf_pack_local.dart';

class AudYoFloOneConnectedProcess {
  int uId = -1;
  int catId = -1;
  String nameProcess = '';
  AudYoFloOneComponentInProcess? involved;
  AudYoFloOneConnectedProcess(this.uId);

  String debugPrintOneIterator(
      AudYoFloOneComponentInProcess involved, String tab) {
    String ret = '';
    var name = involved.nameComponent;
    var mod = involved.nameModule;
    var cname = involved.nameConnector;
    var cpIdName = involved.cpId.txt;
    ret = '$tab -> $name -> $mod -> $cpIdName -> $cname\n';
    tab = tab + '\t';
    for (int ii = 0; ii < involved.next.length; ii++) {
      ret += '$tab -> Branch: $ii\n';
      ret += debugPrintOneIterator(involved.next.elementAt(ii), tab);
    }
    return ret;
  }

  String debugPrintProcess() {
    String ret = 'Process connected: $nameProcess\n';
    if (involved != null) {
      ret += debugPrintOneIterator(involved!, '');
    }
    return ret;
  }
}

abstract class AudYoFloOneComponentInProcess {
  JvxComponentIdentification cpId = JvxComponentIdentification();
  List<AudYoFloOneComponentInProcess> next = [];
  String nameComponent = '';
  String nameModule = '';
  String nameConnector = '';

  void attach(AudYoFloOneComponentInProcess cpIProc) {
    next.add(cpIProc);
  }

  void fill(dynamic implHandle);

  // Return if the passed cpIdentitification is affected by this object
  // and any following.
  bool findComponentIdentification(JvxComponentIdentification cpIdArg) {
    bool overallResult = false;
    if (cpId == cpIdArg) {
      overallResult = true;
    } else {
      for (var elm in next) {
        overallResult = elm.findComponentIdentification(cpIdArg);
        if (overallResult) {
          break;
        }
      }
    }
    return overallResult;
  }

  void findComponentsMatch(List<JvxComponentIdentification> lst,
      Map<JvxComponentIdentification, AudYoFloOneSelectedComponent> allCps) {
    var entry =
        allCps.entries.firstWhereOrNull((element) => (cpId == element.key));
    if (entry != null) {
      bool addElement = true;
      for (var elm in lst) {
        if (elm == cpId) {
          addElement = false;
          break;
        }
      }
      if (addElement) {
        lst.add(cpId);
      }
    }
    for (var elm in next) {
      elm.findComponentsMatch(lst, allCps);
    }
  }
}

class AudYoFloConnectedProcesses {
  Map reportedProcesses = new Map<int, AudYoFloOneConnectedProcess>();
  AudYoFloConnectedProcesses();
  /*
  late fernLiveNative natLib;
  late Pointer<Void> opaqueHost;
*/
  AudYoFloBackendAdapterIf? be;

  void fullClear() {
    reportedProcesses.clear();
  }

  void initialize(AudYoFloBackendAdapterIf? beArg) {
    be = beArg;
  }

  int addProcess(int uId) {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    MapEntry? entry = reportedProcesses.entries
        .firstWhereOrNull((element) => element.key == uId);
    if (entry != null) {
      retVal = retVal = jvxErrorType.JVX_ERROR_ALREADY_IN_USE;
    } else {
      // There is no handle for the process yet. Hence, allocate a new object
      AudYoFloOneConnectedProcess? theProcPtr = be?.allocateProcess(uId);
      if (theProcPtr != null) {
        AudYoFloOneConnectedProcess theProc = theProcPtr;
        reportedProcesses[uId] = theProc;
      }
    }
    return retVal;
  }

  int removeProcess(int uId) {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    MapEntry? entry = reportedProcesses.entries
        .firstWhereOrNull((element) => element.key == uId);
    if (entry == null) {
      retVal = retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    } else {
      reportedProcesses.remove(uId);
    }

    return retVal;
  }

  String debugPrintProcess(int uId) {
    String ret = '';
    MapEntry<dynamic, dynamic> entry =
        reportedProcesses.entries.firstWhere((element) {
      AudYoFloOneConnectedProcess proc = element.value;
      return ((uId == proc.uId) ? true : false);
    });
    if (entry != null) {
      AudYoFloOneConnectedProcess proc = entry.value;
      ret = proc.debugPrintProcess();
    }
    return ret;
  }
}
