import 'package:flutter/material.dart';
import '../ayf_pack_local.dart';
import 'package:provider/provider.dart';
import 'package:collection/collection.dart';

class AudYoFloAudioDeviceFileInputSection {
  double sizeIcon = 55;
  AudYoFloAudioDeviceFileInputSection({required this.sizeIcon});
}

class AudYoFloFileInputControlWidget extends StatelessWidget {
  const AudYoFloFileInputControlWidget(this.identT, this.textShowDrag,
      {Key? key})
      : super(key: key);
  final String textShowDrag;
  final JvxComponentIdentification identT;

  @override
  Widget build(BuildContext context) {
    AudYoFloUiModel theUiModel =
        Provider.of<AudYoFloUiModel>(context, listen: false);

    // Extract and forward the iconSize from the model
    double sizeIcon = 55;
    MapEntry<String, dynamic>? elm = theUiModel
        .widgetSpecificConfigurations.entries
        .firstWhereOrNull((element) => element.key == 'fileinput');
    if (elm != null) {
      if (elm.value is AudYoFloAudioDeviceFileInputSection) {
        sizeIcon = elm.value.sizeIcon;
      }
    }

    // File drop widget is massively depending on platform
    Widget? fileDropWidget;
    if (theUiModel.platformSpec != null) {
      fileDropWidget = theUiModel.platformSpec!
          .allocateFileDropWidget(identT, textShowDrag, sizeIcon);
    }
    return Consumer<AudYoFloUiModel>(builder: (context, notifier, child) {
      return Column(
        children: [
          Flexible(
            flex: 1,
            child: Padding(
                padding: const EdgeInsets.all(8.0), child: fileDropWidget),
          ),
          Flexible(flex: 1, child: AudYoFloFileInputActiveDevicesWidget(identT))
        ],
      );
    });
  }
}
