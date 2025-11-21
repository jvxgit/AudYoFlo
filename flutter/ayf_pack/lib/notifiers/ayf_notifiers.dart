//import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

// =======================================================
// =======================================================
// =======================================================

// Report periodic timer events to the widgets in the tree. We use
// the PeriodicCallbacks reference to transform a callback into
// a PeriodicNotifier in the fernline_main_widget

abstract class AudYoFloPeriodicCallbacks {
  int reportFast();
  int reportSlow();
}

// Report this element to the listening widgets
class AudYoFloPeriodicNotifier with ChangeNotifier {
  int cntFast = 0;
  int cntSlow = 0;
  // bool fast = false; <- this should be used anymore, check the counters instead!!!

  AudYoFloPeriodicNotifier({bool fast = true});

  void notify() {
    notifyListeners();
  }
}
