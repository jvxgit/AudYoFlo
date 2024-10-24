import 'package:flutter/material.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'ayf_ui_specific.dart';

import 'package:ayf_pack/main-widgets/ayf_audio_settings_widget.dart';

import 'package:flutter/foundation.dart' show kIsWeb;

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
  /*
  if (kIsWeb) {
    */
  return AudYoFloPlayConnectBottomAppBar();
  /*
  } else {
    return AudYoFloBottomAppBar();
  }
  */
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

  JvxComponentIdentification cpTpSecTech = JvxComponentIdentification();
  JvxComponentIdentification cpTpSecDev = JvxComponentIdentification();

  JvxComponentIdentification cpTpHtPriTech = JvxComponentIdentification();
  JvxComponentIdentification cpTpHtPriDev = JvxComponentIdentification();

  JvxComponentIdentification cpTpHtSecTech = JvxComponentIdentification();
  JvxComponentIdentification cpTpHtSecDev = JvxComponentIdentification();

  JvxComponentIdentification cpTpFIoTech = JvxComponentIdentification();

  // ==========================================================================
  // Define the core widget composition here: The function configures all application
  // specific widgets here. Then, if all specific widgets were added, the base class
  // call may be used to attach the application unspecific widgets - which are typically
  // only the debug output and the component property viewer widget.
  // ==========================================================================

  AudYoFloUiModelSpecificWithWidget(String routeInit) : super(routeInit) {
    allocateBottomAppBar = allocatorBottomAppBar;
    initRoute = "/connect";
  }

  @override
  void createTabsAndViews(Map<String, String> entriesCfgWidgets) {
    // Tabs are added to a list along multiple layers of the class hierarchie
    // Since we dont know if we are the first here, we can not clear the list
    // when entering the call. It is the task of the caller to run clear before

    // ======================================================================
    // Add the Binaural rendering widget
    // ======================================================================

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

    // ======================================================================
    // Add the secondary audio device widget
    // ======================================================================

    String imageNameAudioIoSec =
        'packages/ayf_pack/images/ayf_icons/ayf-audio-settings.png';
    String textAudioIoSec = 'WASAPI I/O';

    if (cpTpSecTech.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpTpSecTech = widgetConfigCpId(
          entriesCfgWidgets,
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY,
              slotid: 3,
              slotsubid: 0),
          'secaudiot');
    }

    if (cpTpSecDev.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpTpSecDev = widgetConfigCpId(
          entriesCfgWidgets,
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE,
              slotid: cpTpSecTech.slotid,
              slotsubid: 0),
          'secaudiod');
    }

    theAllocatedTabs.add(RotatedBox(
        quarterTurns: -1,
        child: Tooltip(
            message: '${cpTpSecTech.txt}--${cpTpSecDev.txt}',
            child: Column(children: [
              Image.asset(imageNameAudioIoSec,
                  filterQuality: FilterQuality.medium,
                  //'images/fernlive_icons/fernlive-mixer.png',
                  width: navButtonSizeMain,
                  height: navButtonSizeMain),
              Text(textAudioIoSec)
            ]))));
    theAllocatedTabViews.add(AudYoFloAudioSettingsWidget(
        cpTpSecTech,
        'Audio Technologies',
        cpTpSecDev,
        'Audio Devices',
        'soundIo',
        (JvxComponentIdentification selCpId) =>
            AudYoFloAudioDeviceInputOutputChannelWidget(selCpId),
        allocateAudioDeviceSliver));

    // ======================================================================
    // Add the headtracker device widget [RS-232]
    // ======================================================================
    String imageNameHeadTracker =
        'packages/ayf_pack/images/ayf_icons/ayf-headtracker-settings.png';
    String textAudioIoHt = 'H-Tracker Rs232';

    if (cpTpHtPriTech.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpTpHtPriTech = widgetConfigCpId(
          entriesCfgWidgets,
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_TECHNOLOGY,
              slotid: 0,
              slotsubid: 0),
          'headtrackrs232t');
    }

    if (cpTpHtPriDev.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpTpHtPriDev = widgetConfigCpId(
          entriesCfgWidgets,
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_DEVICE,
              slotid: cpTpHtPriTech.slotid,
              slotsubid: 0),
          'headtrackrs232d');
    }

    theAllocatedTabs.add(RotatedBox(
        quarterTurns: -1,
        child: Tooltip(
            message: '${cpTpHtPriTech.txt}--${cpTpHtPriDev.txt}',
            child: Column(children: [
              Image.asset(imageNameHeadTracker,
                  filterQuality: FilterQuality.medium,
                  //'images/fernlive_icons/fernlive-mixer.png',
                  width: navButtonSizeMain,
                  height: navButtonSizeMain),
              Text(textAudioIoHt)
            ]))));
    theAllocatedTabViews.add(AudYoFloAudioSettingsWidget(
        cpTpHtPriTech,
        'Head Tracker Technologies',
        cpTpHtPriDev,
        'Rs232 Head Tracker Devices',
        'headtracker',
        (JvxComponentIdentification selCpId) => null,
        allocateHeadtrackerDeviceSliver));

    // ======================================================================
    // Add the headtracker device widget [ART-UDP]
    // ======================================================================
    textAudioIoHt = 'ART Tracker';
    if (cpTpHtSecTech.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpTpHtSecTech = widgetConfigCpId(
          entriesCfgWidgets,
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_TECHNOLOGY,
              slotid: 1,
              slotsubid: 0),
          'headtrackartt');
    }

    if (cpTpHtSecDev.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpTpHtSecDev = widgetConfigCpId(
          entriesCfgWidgets,
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_DEVICE,
              slotid: cpTpHtSecTech.slotid,
              slotsubid: 0),
          'headtrackartd');
    }

    theAllocatedTabs.add(RotatedBox(
        quarterTurns: -1,
        child: Tooltip(
            message: '${cpTpHtSecTech.txt}--${cpTpHtSecDev.txt}',
            child: Column(children: [
              Image.asset(imageNameHeadTracker,
                  filterQuality: FilterQuality.medium,
                  //'images/fernlive_icons/fernlive-mixer.png',
                  width: navButtonSizeMain,
                  height: navButtonSizeMain),
              Text(textAudioIoHt)
            ]))));
    theAllocatedTabViews.add(AudYoFloAudioSettingsWidget(
        cpTpHtSecTech,
        'Head Tracker Technologies',
        cpTpHtSecDev,
        'ART Head Tracker Devices',
        'headtracker',
        (JvxComponentIdentification selCpId) => null,
        allocateHeadtrackerDeviceSliver));

    // ======================================================================
    // Add the file input widget
    // ======================================================================
    String imageNameFileIO =
        'packages/ayf_pack/images/ayf_icons/ayf-file-input.png';
    String textFileIo = 'File Input';

    if (cpTpFIoTech.cpTp == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpTpFIoTech = widgetConfigCpId(
          entriesCfgWidgets,
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY,
              slotid: 1,
              slotsubid: 0),
          'fileiot');
    }
    theAllocatedTabs.add(RotatedBox(
        quarterTurns: -1,
        child: Tooltip(
            message: cpTpFIoTech.txt,
            child: Column(children: [
              Image.asset(imageNameFileIO,
                  filterQuality: FilterQuality.medium,
                  //'images/fernlive_icons/fernlive-mixer.png',
                  width: navButtonSizeMain,
                  height: navButtonSizeMain),
              Text(textFileIo)
            ]))));
    theAllocatedTabViews.add(AudYoFloFileInputControlWidget(
      cpTpFIoTech,
      "Drag Input File Here",
    ));
    /*AudYoFloAudioSettingsWidget(
        cpTpFIoTech,
        'File I/O Technologies',
        cpTpFIoDev,
        'File I/O Devices',
        'headtracker',
        (JvxComponentIdentification selCpId) => null,
        allocateHeadtrackerDeviceSliver));*/

    // Here, the base class function is called. This call allows the base class
    // to attach the "standard widgets" debug and components. If you do not call these functions
    // the debug widgets will not be viisible.
    super.createTabsAndViews(entriesCfgWidgets);
  }
}
