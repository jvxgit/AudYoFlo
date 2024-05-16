import 'package:flutter/material.dart';
import 'package:ayf_pack/ayf_pack.dart';
// import '../ayfpacks/ayf_pack.dart';

import 'package:ayf_pack/main-widgets/ayf_audio_settings_widget.dart';

class AudYoFloUiModelSpecific extends AudYoFloUiModel
    with AudYoFloUiElementsTabBar {
  // ==========================================================================
  // We define some rendering parameters for the audio io widget.
  // We use package specific icons here.
  // ==========================================================================
  AudYoFloAudioDeviceRenderSection soundIo = AudYoFloAudioDeviceRenderSection(
      imageNameSoundIo: 'packages/ayf_pack/images/ayf_icons/ayf-soundio.png',
      imageNameSoundI: 'packages/ayf_pack/images/ayf_icons/ayf-soundi.png',
      imageNameSoundO: 'packages/ayf_pack/images/ayf_icons/ayf-soundo.png');

  // Add a local property of type "string"
  AudYoFloPropertyStringLocal propTestme = AudYoFloPropertyStringLocal(
      'Hallo Test', "/settings/testme", "Description");

  AudYoFloUiModelSpecific() {
    textApp = 'ayf_ffihost_example';
    imageNameApp = 'images/ayfbinrender_icons/ayfbinrender-logo.png';

    // Define some specific configuration entities
    widgetSpecificConfigurations['soundIo'] = soundIo;

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
