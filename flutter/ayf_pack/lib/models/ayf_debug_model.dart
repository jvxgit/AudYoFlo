import 'package:flutter/foundation.dart';
import 'dart:math';
import 'package:stack_trace/stack_trace.dart';
import 'package:intl/intl.dart';
import 'package:collection/collection.dart';
import '../ayf_pack_local.dart';

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

  void initialize(AudYoFloBackendCache beArg) {
    be = beArg;
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
}
