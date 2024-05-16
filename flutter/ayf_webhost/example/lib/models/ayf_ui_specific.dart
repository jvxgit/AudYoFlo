import 'package:flutter/material.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:ayf_pack_native/ayf_pack_native.dart'
    if (dart.library.html) 'package:ayf_pack/ayf_pack_web.dart';

// import '../ayfpacks/ayf_pack.dart';

import 'package:ayfbinrender/ayf_binrender_widget.dart';
import 'package:ayf_pack/main-widgets/ayf_audio_settings_widget.dart';

class AudYoFloUiModelSpecific extends AudYoFloUiModel
    with AudYoFloUiElementsTabBar {
  // ==========================================================================
  // We define the ui modeling parameters for the binaural rendering widget here.
  // Note that this instance also defines a local property shown in the settings
  // ==========================================================================
  AudYoFloBinauralRendererSection binauralRenderer =
      AudYoFloBinauralRendererSection(true,
          'packages/ayf_pack/images/ayf_icons/ayf-brender.png', 'Bin. Renderer',
          imageNameIcon: 'packages/ayf_pack/images/ayf_icons/ayf-micsrc.png',
          height: 70,
          width: 70);

  // ==========================================================================
  // We define some rendering parameters for the audio io widget.
  // We use package specific icons here.
  // ==========================================================================
  AudYoFloAudioDeviceRenderSection soundIo = AudYoFloAudioDeviceRenderSection(
      imageNameSoundIo: 'packages/ayf_pack/images/ayf_icons/ayf-soundio.png',
      imageNameSoundI: 'packages/ayf_pack/images/ayf_icons/ayf-soundi.png',
      imageNameSoundO: 'packages/ayf_pack/images/ayf_icons/ayf-soundo.png');

  AudYoFloAudioDeviceFileInputSection fileinput =
      AudYoFloAudioDeviceFileInputSection(sizeIcon: 55);

  AudYoFloHeadtrackerDeviceRenderSection headtracker =
      AudYoFloHeadtrackerDeviceRenderSection(
          imageNameHeadtracker:
              'packages/ayf_pack/images/ayf_icons/ayf-headtracker.png');

  // Add a local property of type "string"
  AudYoFloPropertyStringLocal propTestme = AudYoFloPropertyStringLocal(
      'Hallo Test', "/settings/testme", "Description");

  AudYoFloUiModelSpecific() {
    textApp = 'ayfbinrender';
    imageNameApp = 'images/ayfbinrender_icons/ayfbinrender-logo.png';

    // Define some specific configuration entities
    widgetSpecificConfigurations['binauralRenderer'] = binauralRenderer;
    widgetSpecificConfigurations['soundIo'] = soundIo;
    widgetSpecificConfigurations['headtracker'] = headtracker;
    widgetSpecificConfigurations['fileinput'] = fileinput;

    showAudioDevTech = false;

    // =====================================================================
    // Define the settings dialog entries
    // =====================================================================
  }

  @override
  void configureSystemParameters(AudYoFloCompileFlags flags) {
    super.configureSystemParameters(flags);
    AudYoFloSettingsConfigurationSection oneSection =
        AudYoFloSettingsConfigurationSection();

    oneSection = AudYoFloSettingsConfigurationSection();
    oneSection.nameEntry = 'Application';

    // =======================================================================
    // Initialize the local property at this position
    // =======================================================================
    binauralRenderer.propIconSize.selection
        .initialize(flags.compileNum32BitBitfield);
    binauralRenderer.propIconSize.selection.bitZSet(0);
    AudYoFloOneComponentSettingsLocal addThisLoUi =
        AudYoFloOneComponentSettingsLocal('Ui Properties', [
      binauralRenderer.propIconSize,
      binauralRenderer.propSpread,
      fastRefreshPerMsecs,
      slowRefreshPerMsecs
    ], propTags: [
      'Binaural Render Icon Size',
      'Spread Azimuth Angle',
      'Fast callback period',
      'Slow callback period',
    ], propTooltips: [
      'Specify the size of the binaural render icon.',
      'Setup the maximum spread angle in case of icon spread button',
      ' Specify the period for the fast timer',
      ' Specify the period for the slow timer',
    ]);
    oneSection.addEntry(addThisLoUi);

    AudYoFloOneComponentSettingsBackend addThisBeHost =
        AudYoFloOneComponentSettingsBackend(
            JvxComponentIdentification(
                cpTp: JvxComponentTypeEnum.JVX_COMPONENT_HOST,
                slotid: 0,
                slotsubid: 0),
            'Host Configuration',
            [
          '/jvxLibHost/autostart'

          ///ext_interfaces/application_property_autostart'
        ],
            propTags: [
          'Activate Autostart'
        ],
            propTooltips: [
          'Activate Autostart feature to take effect when starting app the next time. When activated, audio processing will start immediately.'
        ]);

    oneSection.addEntry(addThisBeHost);

    AudYoFloOneComponentSettingsBackend addThisBeAsio =
        AudYoFloOneComponentSettingsBackend(
            JvxComponentIdentification(
                cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE,
                slotid: 0,
                slotsubid: 0),
            'ASIO Audio Device',
            [
          '/JVX_GENW/system/sel_framesize',
          '/JVX_GENW/system/sel_rate',
          '/system/rate',
          '/system/sel_framesize'
        ],
            propTags: [
          'Select ASIO Buffersize',
          'Select ASIO Samplerate',
          'Samplerate',
          'Buffersize'
        ],
            propTooltips: [
          'Tooltip: Select ASIO Buffersize',
          'Tooltip: Select ASIO Samplerate',
          'Tooltip: Samplerate',
          'Tooltip: Buffersize'
        ]);

    oneSection.addEntry(addThisBeAsio);

    settingsConfigure.allSections[oneSection.nameEntry] = oneSection;

    // Here we may add an additional combobox in the component settings
    // showAudioDevTech = true;

    // Create another section
    // This is here only for demonstration and is not active at the moment!
    if (false) {
      oneSection = AudYoFloSettingsConfigurationSection();
      oneSection.nameEntry = 'Sip';

      AudYoFloOneComponentSettingsBackend addThisBeSip =
          AudYoFloOneComponentSettingsBackend(
              JvxComponentIdentification(
                  cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE,
                  slotid: 3,
                  slotsubid: 0),
              'Sip Device',
              [
            '/xyz',
            '/xyz',
            '/xyz'
          ],
              propTags: [
            '/xyz',
            '/xyz',
            '/xyz'
          ],
              propTooltips: [
            'Tooltip: /xyz',
            'Tooltip: /xyz',
            'Tooltip: /xyz'
          ]);
      oneSection.addEntry(addThisBeSip);

      settingsConfigure.allSections[oneSection.nameEntry] = oneSection;
    }
  }

  @override
  int reportSetProperty(AudYoFloPropertyContentLocal prop) {
    bool notifyThis = false;
    if (prop.descriptor == "/settings/testme") {
      return jvxErrorType.JVX_NO_ERROR;
    }

    if (prop.descriptor == binauralRenderer.propIconSize.descriptor) {
      if (binauralRenderer.propIconSize.selection.bitTest(0)) {
        binauralRenderer.height = 40;
        binauralRenderer.width = 40;
      } else if (binauralRenderer.propIconSize.selection.bitTest(1)) {
        binauralRenderer.height = 70;
        binauralRenderer.width = 70;
      } else if (binauralRenderer.propIconSize.selection.bitTest(2)) {
        binauralRenderer.height = 100;
        binauralRenderer.width = 100;
      }
      notifyThis = true;
    }

    if (prop.descriptor == binauralRenderer.propSpread.descriptor) {
      binauralRenderer.spreadAngle = binauralRenderer.propSpread.value;
      notifyThis = true;
    }

    if (notifyThis) {
      notifyListeners();
    }

    return super.reportSetProperty(prop);
  }

  @override
  void filterListProperties(
      String token, AudYoFloMultiComponentsPropertyOnChangeEntry entries) {
    if (entries.settings.cpId ==
        JvxComponentIdentification(
            cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE,
            slotid: 0,
            slotsubid: 0)) {
      if (entries is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
        AudYoFloMultiComponentsPropertyOnChangeEntryBackend entriesBe = entries;
        List<AudYoFloPropertyContainer>? lst = entriesBe.bprops;
        if (lst != null) {
          if (token == 'Settings') {
            lst = filterListProperties_settings(lst);
          }
          entriesBe.bprops = lst;
        }
      }
      // var modLst = entries.;
    }
    super.filterListProperties('Settings', entries);
  }

  List<AudYoFloPropertyContainer> filterListProperties_settings(
      List<AudYoFloPropertyContainer> lst) {
    lst = removeIfPresent(
        lst, '/JVX_GENW/system/sel_framesize', ['/system/framesize']);
    lst = removeIfPresent(lst, '/JVX_GENW/system/sel_rate', ['/system/rate']);

    return lst;
  }
}
