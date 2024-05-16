import 'package:provider/provider.dart';
import 'package:flutter/material.dart';
import '../../ayf_pack_local.dart';

import 'package:collection/collection.dart';

class AudYoFloHeadtrackerDeviceRenderSection {
  String imageNameHeadtracker;

  AudYoFloHeadtrackerDeviceRenderSection({required this.imageNameHeadtracker});
}

class AudYoFloOneHeadtrackerDeviceSliver extends StatelessWidget {
  final AudYoFloOneSelectionOptionWithId option;
  final bool fromActiveList;
  final AudYoFloSingleSelectionComponent? reportTarget;

  AudYoFloOneHeadtrackerDeviceSliver(
      this.option, this.fromActiveList, this.reportTarget);

  Widget iconForDeviceOption(
      AudYoFloOneDeviceSelectionOption dev, AudYoFloUiModel uiModel) {
    String imageNameHeadtracker = '';

    MapEntry<String, dynamic>? elm = uiModel
        .widgetSpecificConfigurations.entries
        .firstWhereOrNull((element) => element.key == 'headtracker');
    if (elm != null) {
      if (elm.value is AudYoFloHeadtrackerDeviceRenderSection) {
        AudYoFloHeadtrackerDeviceRenderSection elmSoundIo =
            elm.value as AudYoFloHeadtrackerDeviceRenderSection;
        imageNameHeadtracker = elmSoundIo.imageNameHeadtracker;
      }
    }

    return Image.asset(imageNameHeadtracker);
  }

  @override
  Widget build(BuildContext context) {
    // Produce Device Name

    AudYoFloUiModel theUiCache =
        Provider.of<AudYoFloUiModel>(context, listen: false);

    String showDeviceName = option.option.description;

    deviceSelectionDragData dData =
        deviceSelectionDragData(fromActiveList, option.idx);

    // Show device capabailities
    Widget icon = iconForDeviceOption(option.option, theUiCache);
    Widget? statusWidget;

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
          reportTarget!.reportSelectionComponent(option.option.devIdent);
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
            statusWidget,
            Padding(padding: const EdgeInsets.all(8.0), child: Text(selectable))
          ])),
    );
  }
}
