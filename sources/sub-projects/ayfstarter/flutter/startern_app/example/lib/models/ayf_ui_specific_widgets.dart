import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:ayf_pack_video/ayf_pack_video.dart';
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
  // ==========================================================================
  // Define the core widget composition here: The function configures all application
  // specific widgets here. Then, if all specific widgets were added, the base class
  // call may be used to attach the application unspecific widgets - which are typically
  // only the debug output and the component property viewer widget.
  // ==========================================================================
  bool activatePrimaryAudio = false;

  AudYoFloConfigComponentForWidgetStartupTD cpTpBaresip =
      AudYoFloConfigComponentForWidgetStartupTD(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY,
              slotid: 2,
              slotsubid: 0),
          'baresipaudiot',
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE,
              slotid: 2,
              slotsubid: 0),
          'baresipaudiod');

  AudYoFloConfigComponentForWidgetStartupTD cpTpPriAudio =
      AudYoFloConfigComponentForWidgetStartupTD(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY,
              slotid: 0,
              slotsubid: 0),
          'priaudiot',
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE,
              slotid: 0,
              slotsubid: 0),
          'priaudiod');

  AudYoFloConfigComponentForWidgetStartupTD cpTpSecAudio =
      AudYoFloConfigComponentForWidgetStartupTD(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY,
              slotid: 1,
              slotsubid: 0),
          'secaudiot',
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE,
              slotid: 1,
              slotsubid: 0),
          'secaudiod');

  AudYoFloConfigComponentForWidgetStartup cpTpFIo =
      AudYoFloConfigComponentForWidgetStartup(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY,
              slotid: 2,
              slotsubid: 0),
          'fileiot');

  AudYoFloConfigComponentForWidgetStartupTD cpTpVideo =
      AudYoFloConfigComponentForWidgetStartupTD(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_TECHNOLOGY,
              slotid: 0,
              slotsubid: 0),
          'videot',
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_DEVICE,
              slotid: 0,
              slotsubid: 0),
          'videod');

  AudYoFloConfigComponentForWidgetStartup cpTpVRen =
      AudYoFloConfigComponentForWidgetStartup(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_NODE,
              slotid: 0,
              slotsubid: 0),
          'vrendn');

  AudYoFloUiModelSpecificWithWidget(String routeInit) : super(routeInit) {
    allocateBottomAppBar = allocatorBottomAppBar;
  }

  Widget? allocateCallDeviceWidget(JvxComponentIdentification selCpId) {
    return null;
  }

  @override
  void createTabsAndViews(Map<String, String> entriesCfgWidgets) {
    // ==================================================================
    // ==================================================================

    String imageNameBaresip =
        'packages/ayfphone/images/ayfphone_icons/phone-tab-icon.png';

    String textBaresip = 'BARESIP';
    AudYoFloHelper.widgetConfigCpIdShowTD(entriesCfgWidgets, cpTpBaresip);

    // ==================================================================
    // ==================================================================

    if (activatePrimaryAudio) {
      // ======================================================================
      // Add the primary audio device widget
      // ======================================================================
      String imageNameAudioIoPri =
          'packages/ayf_pack/images/ayf_icons/ayf-audio-settings.png';
      String textAudioIoPri = 'SYNC I/O';
      AudYoFloHelper.widgetConfigCpIdShowTD(entriesCfgWidgets, cpTpPriAudio);
      if (cpTpPriAudio.showWidget) {
        theAllocatedTabs.add(RotatedBox(
            quarterTurns: -1,
            child: Tooltip(
                message:
                    '${cpTpPriAudio.cpId.txt}--${cpTpPriAudio.cpIdDev.txt}',
                child: Column(children: [
                  Image.asset(imageNameAudioIoPri,
                      filterQuality: FilterQuality.medium,
                      //'images/fernlive_icons/fernlive-mixer.png',
                      width: navButtonSizeMain,
                      height: navButtonSizeMain),
                  Text(textAudioIoPri)
                ]))));
        theAllocatedTabViews.add(AudYoFloAudioSettingsWidget(
            cpTpPriAudio.cpId,
            'Audio Technologies',
            cpTpPriAudio.cpIdDev,
            'Audio Devices',
            'soundIo',
            (JvxComponentIdentification selCpId) =>
                AudYoFloAudioDeviceInputOutputChannelWidget(selCpId),
            allocateAudioDeviceSliver));
      }
    }

    // ======================================================================
    // Add the secondary audio device widget
    // ======================================================================
    String imageNameAudioIoSec =
        'packages/ayf_pack/images/ayf_icons/ayf-audio-settings.png';
    String textAudioIoSec = 'Audio I/O';
    AudYoFloHelper.widgetConfigCpIdShowTD(entriesCfgWidgets, cpTpSecAudio);

    if (cpTpSecAudio.showWidget) {
      theAllocatedTabs.add(RotatedBox(
          quarterTurns: -1,
          child: Tooltip(
              message: '${cpTpSecAudio.cpId.txt}--${cpTpSecAudio.cpIdDev.txt}',
              child: Column(children: [
                Image.asset(imageNameAudioIoSec,
                    filterQuality: FilterQuality.medium,
                    //'images/fernlive_icons/fernlive-mixer.png',
                    width: navButtonSizeMain,
                    height: navButtonSizeMain),
                Text(textAudioIoSec)
              ]))));
      theAllocatedTabViews.add(AudYoFloAudioSettingsWidget(
          cpTpSecAudio.cpId,
          'Audio Technologies',
          cpTpSecAudio.cpIdDev,
          'Audio Devices',
          'soundIo',
          (JvxComponentIdentification selCpId) =>
              AudYoFloAudioDeviceInputOutputChannelWidget(selCpId),
          allocateAudioDeviceSliver));
    }

    // ======================================================================
    // Add the secondary audio device widget
    // ======================================================================
    String imageNameVideoIo =
        'packages/ayf_pack/images/ayf_icons/ayf-video-settings.png';
    String textVideoIo = 'Video I/O';
    AudYoFloHelper.widgetConfigCpIdShowTD(entriesCfgWidgets, cpTpVideo);

    if (cpTpVideo.showWidget) {
      theAllocatedTabs.add(RotatedBox(
          quarterTurns: -1,
          child: Tooltip(
              message: '${cpTpVideo.cpId.txt}--${cpTpVideo.cpIdDev.txt}',
              child: Column(children: [
                Image.asset(imageNameVideoIo,
                    filterQuality: FilterQuality.medium,
                    //'images/fernlive_icons/fernlive-mixer.png',
                    width: navButtonSizeMain,
                    height: navButtonSizeMain),
                Text(textVideoIo)
              ]))));
      theAllocatedTabViews.add(AudYoFloAudioSettingsWidget(
          cpTpVideo.cpId,
          'Video Technologies',
          cpTpVideo.cpIdDev,
          'Video Devices',
          'videoIo',
          (JvxComponentIdentification selCpId) => null,
          allocateAudioDeviceSliver));
    }
    // ======================================================================
    // Add the file input widget
    // ======================================================================
    String imageNameFileIO =
        'packages/ayf_pack/images/ayf_icons/ayf-file-input.png';
    String textFileIo = 'File Input';
    AudYoFloHelper.widgetConfigCpIdShow(entriesCfgWidgets, cpTpFIo);
    if (cpTpFIo.showWidget) {
      theAllocatedTabs.add(RotatedBox(
          quarterTurns: -1,
          child: Tooltip(
              message: cpTpFIo.cpId.txt,
              child: Column(children: [
                Image.asset(imageNameFileIO,
                    filterQuality: FilterQuality.medium,
                    //'images/fernlive_icons/fernlive-mixer.png',
                    width: navButtonSizeMain,
                    height: navButtonSizeMain),
                Text(textFileIo)
              ]))));
      theAllocatedTabViews.add(AudYoFloFileInputControlWidget(
        cpTpFIo.cpId,
        "Drag Input File Here",
      ));
    }

    // ======================================================================
    // Add the file input widget
    // ======================================================================
    String imageNameVideoRender =
        'packages/ayf_pack/images/ayf_icons/ayf-video-render.png';
    String textVideoRender = 'Video Render';
    AudYoFloHelper.widgetConfigCpIdShow(entriesCfgWidgets, cpTpVRen);
    if (cpTpVRen.showWidget) {
      theAllocatedTabs.add(RotatedBox(
          quarterTurns: -1,
          child: Tooltip(
              message: cpTpVRen.cpId.txt,
              child: Column(children: [
                Image.asset(imageNameVideoRender,
                    filterQuality: FilterQuality.medium,
                    //'images/fernlive_icons/fernlive-mixer.png',
                    width: navButtonSizeMain,
                    height: navButtonSizeMain),
                Text(textVideoRender)
              ]))));
      theAllocatedTabViews.add(AudYoFloMultiVideoWidget(
        cpTpVRen.cpId,
        aspectRatioW: 320,
        aspectRatioH: 240,
      ));
    }
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
