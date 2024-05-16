import 'package:flutter/material.dart';
import 'dart:async';
import '../ayf_pack_local.dart';

class AudYoFloHostTimer {
  AudYoFloPeriodicCallbacks? callbacks;

  int timeoutFastMsecs = 50;
  int timeoutSlowMsecs = 1000;
  late Timer timerPeriodicFast;
  late Timer timerPeriodicSlow;

  AudYoFloHostTimer(
      {this.timeoutFastMsecs = 50,
      this.timeoutSlowMsecs = 1000,
      this.callbacks});

  void fastCallback(Timer _) {
    // Callback for fast periodic update
    int timeoutNew = -1;
    int? timeoutVar = callbacks?.reportFast();
    if (timeoutVar != null) {
      if (timeoutVar != timeoutFastMsecs) {
        timerPeriodicFast.cancel();
        timeoutFastMsecs = timeoutVar;
        timerPeriodicFast = Timer.periodic(
            Duration(milliseconds: timeoutFastMsecs), fastCallback);
      }
    }
  }

  void slowCallback(Timer _) {
    // Callback for slow periodic update
    int timeoutNew = -1;
    int? timeoutVar = callbacks?.reportSlow();
    if (timeoutVar != null) {
      if (timeoutVar != timeoutSlowMsecs) {
        timerPeriodicSlow.cancel();
        timeoutSlowMsecs = timeoutVar;
        timerPeriodicSlow = Timer.periodic(
            Duration(milliseconds: timeoutSlowMsecs), slowCallback);
      }
    }
  }

  bool initTimer() {
    timerPeriodicFast =
        Timer.periodic(Duration(milliseconds: timeoutFastMsecs), fastCallback);

    timerPeriodicSlow =
        Timer.periodic(Duration(milliseconds: timeoutSlowMsecs), slowCallback);

    return true;
  }

  @override
  bool termTimer() {
    timerPeriodicFast.cancel();
    timerPeriodicSlow.cancel();
    return true;
  }
}
