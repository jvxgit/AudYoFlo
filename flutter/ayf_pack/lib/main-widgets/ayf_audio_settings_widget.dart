import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

class AudYoFloAudioSettingsWidget extends StatefulWidget {
  const AudYoFloAudioSettingsWidget(
      this.identT,
      this.tagTechnologies,
      this.identD,
      this.tagDevices,
      this.deviceTypeTag,
      this.callbackDeviceWidget,
      this.callbackActiveInactiveDeviceSliver);
  final JvxComponentIdentification identT;
  final JvxComponentIdentification identD;
  final String tagTechnologies;
  final String tagDevices;
  final String deviceTypeTag;

  final Widget? Function(JvxComponentIdentification selCpId)
      callbackDeviceWidget;
  final Widget? Function(
      AudYoFloOneDeviceSelectionOptionWithId option,
      bool fromActiveList,
      AudYoFloSelectionComponentUi?) callbackActiveInactiveDeviceSliver;
  @override
  _AudYoFloAudioSettingsWidgetState createState() =>
      _AudYoFloAudioSettingsWidgetState();
}

class _AudYoFloAudioSettingsWidgetState
    extends State<AudYoFloAudioSettingsWidget> {
  AudYoFloStringWithId dropdownValuePrimDev = AudYoFloStringWithId('None', -1);
  AudYoFloBackendCache? theBeCache;
  AudYoFloBackendCacheFectrlIf? beCacheFe;
  AudYoFloUiModel? theUiModel;

  @override
  Widget build(BuildContext context) {
    theBeCache = Provider.of<AudYoFloBackendCache>(context, listen: false);
    theUiModel = Provider.of<AudYoFloUiModel>(context, listen: false);

    // obtain the frontend interface
    beCacheFe = theBeCache!.cacheToFrontend;

    return Selector<AudYoFloBackendCache, int>(
        selector: (context, beModel) => theBeCache!.componentSection.ssUpdateId,
        shouldRebuild: (previous, next) => (next != previous) ? true : false,
        builder: (context, notifier, child) {
          if (theUiModel!.showAudioDevTech) {
            // ================================================================
            return Container(
                color: Colors.red,
                child: Column(children: [
                  Flexible(
                      flex: 0,
                      child: Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Row(
                          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                          children: [
                            Flexible(
                                flex: 1, child: Text(widget.tagTechnologies)),
                            Flexible(
                              flex: 1,
                              child: AudYoFloComponentSelectionComboBoxWidget(
                                  widget.identT),
                            ),
                            Flexible(flex: 1, child: Text(widget.tagDevices)),
                            Flexible(
                              flex: 1,
                              child: AudYoFloComponentSelectionComboBoxWidget(
                                  widget.identD),
                            )
                          ],
                        ),
                      )),
                  Flexible(
                    flex: 10,
                    child: AudYoFloDeviceSelectionWidget(
                        widget.identT,
                        widget.tagDevices,
                        widget.callbackDeviceWidget,
                        widget.callbackActiveInactiveDeviceSliver),
                  )
                ]));
          }

          // No additional information: show only audio device lists
          return AudYoFloDeviceSelectionWidget(
              widget.identT,
              widget.tagDevices,
              widget.callbackDeviceWidget,
              widget.callbackActiveInactiveDeviceSliver);
        });
  }
}
