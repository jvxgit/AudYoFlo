import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import './ayf_main_layout.dart';
import '../ayf_pack_local.dart';

class AudYoFloMainAppWidget extends StatefulWidget {
  AudYoFloMainAppWidget();
  @override
  _AudYoFloMainAppWidgetStates createState() => _AudYoFloMainAppWidgetStates();
}

class _AudYoFloMainAppWidgetStates extends State<AudYoFloMainAppWidget>
    with AudYoFloBackendCacheCallback {
  String myCbTag = 'Main';

  BuildContext? latestContext;
  AudYoFloBackendCache? beCache;
  AudYoFloUiModel? uiModel;
  // FocusNode _myFocusNode = FocusNode();

  @override
  void reportEvent(String reasonHint) {
    if (uiModel != null) {
      if (uiModel!.initRoute != "/") {
        if (beCache != null) {
          // Clear the backend cache!!
          beCache!.componentOptionMap.fullClear();
          beCache!.deviceOptionMap.fullClear();
          beCache!.componentSection.fullClear();
          beCache!.processSection.fullClear();
          beCache!.sequencerSection.fullClear();
          beCache!.typeClassList.clear();
          beCache!.httpConnect.isConnected = false;
        }
        Navigator.pushNamed(context, uiModel!.initRoute);
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    beCache = Provider.of<AudYoFloBackendCache>(context, listen: false);
    uiModel = Provider.of<AudYoFloUiModel>(context, listen: false);
    beCache!.requestSystemDisconnect = this;
    latestContext = context;

    // =======================================================

    final Size size = MediaQuery.of(context).size;
    return AudYoFloMainLayout(size: size);
  }

  @override
  void initState() {
    super.initState();
    //AudioFlowBackendBridge bridge = AudioFlowBackendBridge();
  }

  @override
  void didUpdateWidget(AudYoFloMainAppWidget oldWidget) {
    super.didUpdateWidget(oldWidget);
  }

  @override
  void dispose() {
    super.dispose();
  }
}
