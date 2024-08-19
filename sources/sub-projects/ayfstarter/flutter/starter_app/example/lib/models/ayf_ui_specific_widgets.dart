import 'package:flutter/material.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'ayf_ui_specific.dart';
import 'package:flutter/foundation.dart' show kIsWeb;
import 'package:ayf_pack/main-widgets/ayf_audio_settings_widget.dart';

// ============================================================================
// Callback to allocate an audio device sliver in the list of active/inactive
// devices lists
Widget? allocateAudioDeviceSliver(AudYoFloOneDeviceSelectionOptionWithId option,
        bool fromActiveList, AudYoFloSelectionComponentUi? reportW) =>
    AudYoFloOneDeviceSliver(option, fromActiveList, reportW);

// ============================================================================
// Callback to allocate a headtracker device sliver in the list of active/
// inactibe devices list
Widget? allocateHeadtrackerDeviceSliver(
        AudYoFloOneDeviceSelectionOptionWithId option,
        bool fromActiveList,
        AudYoFloSelectionComponentUi? reportW) =>
    AudYoFloOneHeadtrackerDeviceSliver(option, fromActiveList, reportW);

BottomAppBar? allocatorBottomAppBar() {
  if (kIsWeb) {
    return AudYoFloPlayConnectBottomAppBar();
  } else {
    return AudYoFloBottomAppBar();
  }
}

class AudYoFloPlayConnectBottomAppBar extends BottomAppBar {
  AudYoFloPlayConnectBottomAppBar()
      : super(
            child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
          children: [AudYoFloPlaypannel(), AudYoFloPlaypannel()],
        ));
}

// ============================================================================
// ============================================================================
// This class extends the common UiModel part and adds application specific
// configuration entries.
// ============================================================================
class AudYoFloUiModelSpecificWithWidget extends AudYoFloUiModelSpecific {
  JvxComponentIdentification cpTpPriTech = JvxComponentIdentification();
  JvxComponentIdentification cpTpPriDev = JvxComponentIdentification();

  // ==========================================================================
  // Define the core widget composition here: The function configures all application
  // specific widgets here. Then, if all specific widgets were added, the base class
  // call may be used to attach the application unspecific widgets - which are typically
  // only the debug output and the component property viewer widget.
  // ==========================================================================

  AudYoFloUiModelSpecificWithWidget(String routeInit) : super(routeInit) {
    allocateBottomAppBar = allocatorBottomAppBar;
  }

  @override
  void createTabsAndViews(Map<String, String> entriesCfgWidgets) {
    // ======================================================================
    // Add the primary audio device widget
    // ======================================================================

    String imageNameAudioIoPri =
        'packages/ayf_pack/images/ayf_icons/ayf-audio-settings.png';
    String textAudioIoPri = 'ASIO I/O';
    if (cpTpPriTech.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpTpPriTech = widgetConfigCpId(
          entriesCfgWidgets,
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY,
              slotid: 0,
              slotsubid: 0),
          'priaudiot');
    }

    if (cpTpPriDev.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpTpPriDev = widgetConfigCpId(
          entriesCfgWidgets,
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE,
              slotid: cpTpPriTech.slotid,
              slotsubid: 0),
          'priaudiod');
    }

    theAllocatedTabs.add(RotatedBox(
        quarterTurns: -1,
        child: Tooltip(
            message: '${cpTpPriTech.txt}--${cpTpPriDev.txt}',
            child: Column(children: [
              Image.asset(imageNameAudioIoPri,
                  filterQuality: FilterQuality.medium,
                  //'images/fernlive_icons/fernlive-mixer.png',
                  width: navButtonSizeMain,
                  height: navButtonSizeMain),
              Text(textAudioIoPri)
            ]))));
    theAllocatedTabViews.add(AudYoFloAudioSettingsWidget(
        cpTpPriTech,
        'Audio Technologies',
        cpTpPriDev,
        'Audio Devices',
        'soundIo',
        (JvxComponentIdentification selCpId) =>
            AudYoFloAudioDeviceInputOutputChannelWidget(selCpId),
        allocateAudioDeviceSliver));
    // Here, the base class function is called. This call allows the base class
    // to attach the "standard widgets" debug and components. If you do not call these functions
    // the debug widgets will not be viisible.
    super.createTabsAndViews(entriesCfgWidgets);
  }
}
