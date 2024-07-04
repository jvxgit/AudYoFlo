import 'package:provider/provider.dart';
import 'package:flutter/material.dart';
import 'package:collection/collection.dart';
import '../../ayf_pack_local.dart';

//import '../ayf_devices_active_inactive_widget.dart';
//import '../audio-devices/ayf_audio_device_channel_selection_widget.dart';

class AudYoFloDeviceSelectionWidget extends StatefulWidget {
  final JvxComponentIdentification identT;
  final String tagDevices;
  final Widget? Function(JvxComponentIdentification selCpId)
      callbackDeviceWidget;
  final Widget? Function(
      AudYoFloOneDeviceSelectionOptionWithId option,
      bool fromActiveList,
      AudYoFloSingleSelectionComponent?) callbackActiveInactiveDeviceSliver;

  AudYoFloDeviceSelectionWidget(this.identT, this.tagDevices,
      this.callbackDeviceWidget, this.callbackActiveInactiveDeviceSliver);
  @override
  State<StatefulWidget> createState() {
    return _AudYoFloDeviceSelectionWidgetStates();
  }
}

class _AudYoFloDeviceSelectionWidgetStates
    extends State<AudYoFloDeviceSelectionWidget>
    with
        AudYoFloSingleSelectionComponent,
        AudYoFloOneTechnologyDevicesOnChange {
  List<AudYoFloOneDeviceSelectionOption>? pContent;
  JvxComponentIdentification selCpId = JvxComponentIdentification(
      cpTp: JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN);

  // ===========================================================================
  // Implements the callback in case a device was selected in a sub widget
  // At the moment, this callback is called if a device was chosen in the list
  // of active devices.
  @override
  void reportSelectionComponent(JvxComponentIdentification cpId) {
    setState(() {
      selCpId = cpId;
    });
  }

  // ===========================================================================
  // ===========================================================================
  @override
  void updateDevices(List<AudYoFloOneDeviceSelectionOption> devs) {
    // Pass in the current devices
    pContent = devs;
  }

  @override
  Widget build(BuildContext context) {
    cpId = widget.identT;
    return localBuild(context);
  }

  @override
  Widget innerWidget(BuildContext context) {
    // ========================================================================
    // Here, we allocate the widget that show the component specific widget
    Widget? devWidget = widget.callbackDeviceWidget(selCpId);
    // ========================================================================

    if ((selCpId.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) ||
        (devWidget == null)) {
      return //Container(color: Colors.blue, child: Text('Success'));
          AudYoFloDeviceActiveInactiveWidget(pContent, widget.identT,
              widget.tagDevices, widget.callbackActiveInactiveDeviceSliver,
              reportWidget: this);
    } else {
      return Column(
        children: [
          Flexible(
              flex: 1,
              child: AudYoFloDeviceActiveInactiveWidget(pContent, widget.identT,
                  widget.tagDevices, widget.callbackActiveInactiveDeviceSliver,
                  reportWidget: this)),
          Flexible(
              flex: 1,
              //child: AudYoFloInputOutputChannelWidget(selCpId)),
              child: devWidget)
        ],
      );
    }
  }
}

// ==============================================================================

