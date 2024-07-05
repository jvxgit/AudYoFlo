import 'package:provider/provider.dart';
import 'package:flutter/material.dart';
import '../../ayf_pack_local.dart';

import 'package:collection/collection.dart';

class AudYoFloAudioDeviceRenderSection {
  String imageNameSoundIo = '';
  String imageNameSoundI = '';
  String imageNameSoundO = '';

  AudYoFloAudioDeviceRenderSection(
      {required this.imageNameSoundIo,
      required this.imageNameSoundI,
      required this.imageNameSoundO});
}

class AudYoFloOneDeviceSliver extends StatelessWidget {
  final AudYoFloOneDeviceSelectionOptionWithId option;
  final bool fromActiveList;
  final AudYoFloSelectionComponentUi? reportTarget;

  AudYoFloOneDeviceSliver(this.option, this.fromActiveList, this.reportTarget);

  Widget iconForDeviceOption(
      AudYoFloOneDeviceSelectionOption dev, AudYoFloUiModel uiModel) {
    String imageNameSoundIo = '';
    String imageNameSoundI = '';
    String imageNameSoundO = '';

    MapEntry<String, dynamic>? elm = uiModel
        .widgetSpecificConfigurations.entries
        .firstWhereOrNull((element) => element.key == 'soundIo');
    if (elm != null) {
      if (elm.value is AudYoFloAudioDeviceRenderSection) {
        AudYoFloAudioDeviceRenderSection elmSoundIo =
            elm.value as AudYoFloAudioDeviceRenderSection;
        imageNameSoundIo = elmSoundIo.imageNameSoundIo;
        imageNameSoundI = elmSoundIo.imageNameSoundI;
        imageNameSoundO = elmSoundIo.imageNameSoundO;
      }
    }

    if ((dev.caps.bitTest(jvxDeviceCapabilityTypeShift
                .JVX_DEVICE_CAPABILITY_INPUT_SHIFT.index) &&
            dev.caps.bitTest(jvxDeviceCapabilityTypeShift
                .JVX_DEVICE_CAPABILITY_OUTPUT_SHIFT.index)) ||
        dev.caps.bitTest(jvxDeviceCapabilityTypeShift
            .JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT.index)) {
      if (imageNameSoundIo.isEmpty) {
        return const Icon(
          Icons.report_problem,
          color: Colors.green,
          size: 30.0,
        );
      }
      return Image.asset(imageNameSoundIo);
    } else {
      if (dev.caps.bitTest(jvxDeviceCapabilityTypeShift
          .JVX_DEVICE_CAPABILITY_INPUT_SHIFT.index)) {
        if (imageNameSoundI.isEmpty) {
          return const Icon(
            Icons.report_problem,
            color: Colors.green,
            size: 30.0,
          );
        }
        return Image.asset(imageNameSoundI);
      }
    }
    if (imageNameSoundO.isEmpty) {
      return const Icon(
        Icons.report_problem,
        color: Colors.green,
        size: 30.0,
      );
    }
    return Image.asset(imageNameSoundO);
  }

  @override
  Widget build(BuildContext context) {
    // Produce Device Name

    AudYoFloUiModel theUiCache =
        Provider.of<AudYoFloUiModel>(context, listen: false);

    String showDeviceName = option.option.description;
    int idx = showDeviceName.indexOf(RegExp(r'<*>'));
    if (idx != -1) {
      showDeviceName = showDeviceName.substring(1, showDeviceName.length - 1);
    }
    deviceSelectionDragData dData =
        deviceSelectionDragData(fromActiveList, option.idx);

    // Show device capabailities
    Widget icon = iconForDeviceOption(option.option, theUiCache);
    Widget? statusWidget;

    // Highlight the master device
    String masterText = '';
    if ((option.option.devIdent.slotsubid == 0) &&
        (option.option.devIdent.slotid == 0)) {
      masterText = "M";
    }

    String selectable = '';
    if (!option.option.selectable) {
      selectable = 'x';
    }

    if (option.option.state
        .bitTest(jvxStateShiftEnum.JVX_STATE_SELECTED.index)) {
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.red, child: Icon(Icons.warning, size: 20)));
    } else if (option.option.state
        .bitTest(jvxStateShiftEnum.JVX_STATE_ACTIVE.index)) {
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.green, child: Icon(Icons.stop, size: 20)));
    } else if (option.option.state
        .bitTest(jvxStateShiftEnum.JVX_STATE_PREPARED.index)) {
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.yellow, child: Icon(Icons.pause, size: 20)));
    } else if (option.option.state
        .bitTest(jvxStateShiftEnum.JVX_STATE_PROCESSING.index)) {
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.blue, child: Icon(Icons.play_arrow, size: 20)));
    } else {
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.grey,
              child: Icon(Icons.check_box_outline_blank, size: 20)));
    }

    return GestureDetector(
      onTap: () {
        // here, we run a callback to the next higher layer in the widget hierarchy to
        // update the widget which is in parallel to the device selection construct

        if (reportTarget != null) {
          reportTarget!.reportSelectionComponentUi(option.option.devIdent);
        }
      },
      child: Card(
          child: Row(
              //mainAxisAlignment: MainAxisAlignment.start, children: [
              children: [
            Draggable<deviceSelectionDragData>(
                feedback: SizedBox(height: 30, width: 60, child: icon),
                data: dData,
                child: SizedBox(height: 30, width: 60, child: icon)),
            Expanded(
                child: Padding(
                    padding: const EdgeInsets.all(8.0),
                    child: Text(showDeviceName))),
            Padding(
                padding: const EdgeInsets.all(8.0),
                child: Container(color: Colors.red, child: Text(masterText))),
            statusWidget,
            Padding(padding: const EdgeInsets.all(8.0), child: Text(selectable))
          ])),
    );
  }
}
