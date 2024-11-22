import 'package:flutter/foundation.dart';
import 'dart:math';
import 'package:collection/collection.dart';
import 'package:stack_trace/stack_trace.dart';
import 'package:intl/intl.dart';
import 'package:collection/collection.dart';
import '../ayf_pack_local.dart';

class AudYoFloAssociatedShowParams {
  String regExpr = '';
  bool showDescriptor = false;
  bool showSelected = false;

  List<String> propsRealtimeUpdate = [];
  List<String> propsSelected = [];
}

class AudYoFloDebugModel with ChangeNotifier {
  late AudYoFloBackendCache be;

  // Parameter for the "debug" widget
  int num_lines_out_max = 128;
  List<String> out_debug = [];
  bool dbgOut = true;
  bool funcOut = false;

  // Parameter for the "component" widget
  // Selection of component in generic property viewer
  JvxComponentIdentification idSelectCp = JvxComponentIdentification();
  int ssUpdateCpId = -1;

  int idSelectProc = -1;

  Map<JvxComponentIdentification, AudYoFloAssociatedShowParams> regExprMap = {};

  void initialize(AudYoFloBackendCache beArg) {
    be = beArg;
    beArg.dbgModel = this;
  }

  void triggerUpdate() {
    notifyListeners();
  }

  void addLineOutConvert(String txt) {
    String token = '';
    for (int ii = 0; ii < txt.length; ii++) {
      String oneChar = txt.substring(ii, ii + 1);
      if (oneChar == '\n') {
        if (token.length > 0) {
          addLineOut(token);
          token = '';
        }
      } else if (oneChar == '\t') {
        token += "   ";
      } else {
        token += oneChar;
      }
    }

    if (token.length > 0) {
      addLineOut(token);
      token = '';
    }
  }

  void addLineOut(String txt) {
    if (out_debug.length == num_lines_out_max) {
      out_debug.removeAt(0);
    }
    DateFormat dateFormat = DateFormat("yyyy-MM-dd HH:mm:ss");
    String dateTime = dateFormat.format(DateTime.now());
    var funcname = '';
    if (funcOut) {
      Trace str = Trace.from(StackTrace.current);
      var ff = str.frames;
      var fr = ff.elementAt(1);
      funcname = '-- ' + fr.toString();
    }
    out_debug.add('${dateTime} ${funcname} -- $txt');
    triggerUpdate();
  }

  void newNumberLines(int num) {
    num_lines_out_max = num;
    num_lines_out_max = max(1, num_lines_out_max);
    while (out_debug.length > num_lines_out_max) {
      out_debug.removeAt(0);
    }
    triggerUpdate();
  }

  void clearAll() {
    out_debug.clear();
    triggerUpdate();
  }

  void verifyComponentSelection(List<JvxComponentIdentification> lst) {
    JvxComponentIdentification? cpSel =
        lst.firstWhereOrNull((element) => element == idSelectCp);
    if (cpSel == null) {
      // If current selection is no longer available, select the first in list
      idSelectCp = lst[0];
      // selectNewComponent(lst[0]); <- This should not be called as the functon is called in build function
    }
  }

  void selectNewComponent(JvxComponentIdentification cpId) {
    idSelectCp = cpId;
    ssUpdateCpId++;
    triggerUpdate();
  }

  String getRegExpressionShow(JvxComponentIdentification cpId) {
    String expr = '';
    var elm =
        regExprMap.entries.firstWhereOrNull((element) => element.key == cpId);
    if (elm != null) {
      expr = elm.value.regExpr;
    }
    return expr;
  }

  void setRegExpressionShow(JvxComponentIdentification cpId, String regExpr) {
    AudYoFloAssociatedShowParams newElm = AudYoFloAssociatedShowParams();
    newElm.regExpr = regExpr;
    mapUpdateNoHash<JvxComponentIdentification, AudYoFloAssociatedShowParams>(
        regExprMap, cpId, newElm, (value) {
      value.regExpr = regExpr;
      return value;
    });
  }

  bool isRealtimeUpdateShow(
      JvxComponentIdentification cpId, String descriptor) {
    bool retVal = false;
    var elm =
        regExprMap.entries.firstWhereOrNull((element) => element.key == cpId);
    if (elm != null) {
      var elmLst = elm.value.propsRealtimeUpdate
          .firstWhereOrNull((element) => element == descriptor);
      if (elmLst != null) {
        retVal = true;
      }
    }
    return retVal;
  }

  void modifyRealtimeUpdateShow(
      JvxComponentIdentification cpId, String descriptor, bool doUpdate) {
    AudYoFloAssociatedShowParams newElm = AudYoFloAssociatedShowParams();
    if (doUpdate) {
      newElm.propsRealtimeUpdate.add(descriptor);
    }

    mapUpdateNoHash<JvxComponentIdentification, AudYoFloAssociatedShowParams>(
        regExprMap, cpId, newElm, (value) {
      var elmIn = value.propsRealtimeUpdate
          .firstWhereOrNull((element) => (element == descriptor));
      if (elmIn != null) {
        value.propsRealtimeUpdate.remove(descriptor);
      }
      if (doUpdate) {
        value.propsRealtimeUpdate.add(descriptor);
      }
      return value;
    });
  }

  bool isSelectedUpdateShow(
      JvxComponentIdentification cpId, String descriptor) {
    bool retVal = false;
    var elm =
        regExprMap.entries.firstWhereOrNull((element) => element.key == cpId);
    if (elm != null) {
      var elmLst = elm.value.propsSelected
          .firstWhereOrNull((element) => element == descriptor);
      if (elmLst != null) {
        retVal = true;
      }
    }
    return retVal;
  }

  void toggleSelectProperty(
      JvxComponentIdentification cpId, String descriptor) {
    AudYoFloAssociatedShowParams newElm = AudYoFloAssociatedShowParams();

    mapUpdateNoHash<JvxComponentIdentification, AudYoFloAssociatedShowParams>(
        regExprMap, cpId, newElm, (value) {
      var elmIn = value.propsSelected
          .firstWhereOrNull((element) => (element == descriptor));
      if (elmIn != null) {
        value.propsSelected.remove(descriptor);
      } else {
        value.propsSelected.add(descriptor);
      }
      return value;
    });
  }

  // =================================================================================

  void setShowSelected(JvxComponentIdentification cpId, bool showSelect) {
    AudYoFloAssociatedShowParams newElm = AudYoFloAssociatedShowParams();
    newElm.showSelected = showSelect;
    mapUpdateNoHash<JvxComponentIdentification, AudYoFloAssociatedShowParams>(
      regExprMap,
      cpId,
      newElm,
      (value) {
        value.showSelected = showSelect;
        return value;
      },
    );
  }

  bool getShowSelected(JvxComponentIdentification cpId) {
    bool retVal = false;
    var elm =
        regExprMap.entries.firstWhereOrNull((element) => element.key == cpId);
    if (elm != null) {
      retVal = elm.value.showSelected;
    }
    return retVal;
  }

  // =================================================================================

  void setShowDescriptor(JvxComponentIdentification cpId, bool showDescriptor) {
    AudYoFloAssociatedShowParams newElm = AudYoFloAssociatedShowParams();
    newElm.showDescriptor = showDescriptor;
    mapUpdateNoHash<JvxComponentIdentification, AudYoFloAssociatedShowParams>(
      regExprMap,
      cpId,
      newElm,
      (value) {
        value.showDescriptor = showDescriptor;
        return value;
      },
    );
  }

  bool getShowDescriptor(JvxComponentIdentification cpId) {
    bool retVal = false;
    var elm =
        regExprMap.entries.firstWhereOrNull((element) => element.key == cpId);
    if (elm != null) {
      retVal = elm.value.showDescriptor;
    }
    return retVal;
  }

  List<String> cleanupEmpty(List<String> inLst) {
    List<String> outLst = [];
    for (var elm in inLst) {
      if (elm.isNotEmpty) {
        outLst.add(elm);
      }
    }
    return outLst;
  }

  void acceptConfigToken(String txt) {
    var lstEntries = txt.split('#');
    lstEntries = cleanupEmpty(lstEntries);
    for (var elm in lstEntries) {
      AudYoFloAssociatedShowParams newElm = AudYoFloAssociatedShowParams();
      JvxComponentIdentification cpId = JvxComponentIdentification();
      var lstOne = elm.split(';');
      lstOne = cleanupEmpty(lstOne);

      for (var elmI in lstOne) {
        var lstOneElm = elmI.split('=');
        if (lstOneElm.length == 2) {
          if (lstOneElm[0] == 'cp') {
            cpId = AudYoFloStringTranslator.componentIdentificationFromString(
                lstOneElm[1]);
            continue;
          }
          if (lstOneElm[0] == 'descr') {
            if (lstOneElm[1] == 'true') {
              newElm.showDescriptor = true;
            }
            continue;
          }
          if (lstOneElm[0] == 'sel') {
            if (lstOneElm[1] == 'true') {
              newElm.showSelected = true;
            }
            continue;
          }
          if (lstOneElm[0] == 'reg') {
            newElm.regExpr = lstOneElm[1];
            continue;
          }
          if (lstOneElm[0] == 'psel') {
            var lstEntries = lstOneElm[1].split(':');
            lstEntries = cleanupEmpty(lstEntries);
            newElm.propsSelected = lstEntries;
            continue;
          }
          if (lstOneElm[0] == 'prtu') {
            var lstEntries = lstOneElm[1].split(':');
            lstEntries = cleanupEmpty(lstEntries);
            newElm.propsRealtimeUpdate = lstEntries;
            continue;
          }
        }
      }

      if (cpId.cpTp != JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
        var elmFound = regExprMap.entries.firstWhereOrNull((element) {
          return element.key == cpId;
        });
        if (elmFound == null) {
          regExprMap[cpId] = newElm;
        }
      }
      // JvxComponentTypeEEnum
      // JvxComponentIdentification cpId
    }
  }

  String provideConfigToken() {
    String ctxtToken = '';
    for (var elm in regExprMap.entries) {
      String oneEntry = 'cp=' + elm.key.txt;
      oneEntry += ';descr=' + elm.value.showDescriptor.toString();
      oneEntry += ';sel=' + elm.value.showSelected.toString();
      oneEntry += ';reg=' + elm.value.regExpr.toString();
      oneEntry += ';psel=';
      for (var elmLst in elm.value.propsSelected) {
        oneEntry += elmLst + ':';
      }
      oneEntry += ';prtu=';
      for (var elmLst in elm.value.propsRealtimeUpdate) {
        oneEntry += elmLst + ':';
      }
      ctxtToken = oneEntry + '#';
    }
    return ctxtToken;
  }
}
