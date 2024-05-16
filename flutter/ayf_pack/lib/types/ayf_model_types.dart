import 'package:collection/collection.dart';
import '../ayf_pack_local.dart';

// ==========================================================================

abstract class AudYoFloOneComponentSettings {
  JvxComponentIdentification cpId;
  String compTag;
  List<String> propTags;
  List<String> propTooltips;
  AudYoFloOneComponentSettings(
      this.cpId, this.compTag, this.propTags, this.propTooltips);
  String get specialization;
}

class AudYoFloOneComponentSettingsLocal extends AudYoFloOneComponentSettings {
  List<AudYoFloPropertyContentLocal> directProps = [];
  AudYoFloOneComponentSettingsLocal(String compTag, this.directProps,
      {List<String> propTags = const [], List<String> propTooltips = const []})
      : super(JvxComponentIdentification(), compTag, propTags, propTooltips);

  @override
  String get specialization {
    return 'AudYoFloOneComponentSettingsLocal';
  }
}

class AudYoFloOneComponentSettingsBackend extends AudYoFloOneComponentSettings {
  List<String> propsDescriptors;
  AudYoFloOneComponentSettingsBackend(
      JvxComponentIdentification cpId, String compTag, this.propsDescriptors,
      {List<String> propTags = const [], List<String> propTooltips = const []})
      : super(cpId, compTag, propTags, propTooltips);
  @override
  String get specialization {
    return 'AudYoFloOneComponentSettingsBackend';
  }
}

// ========================================================================

enum AudYoFloSettingsConfigurationPropIfCondition {
  AYF_PROPERTY_IF_EXISTS,
  AYF_PROPERTY_IF_NOT_EXISTS,
}

enum AudYoFloSettingsConfigurationPropThenAction {
  AYF_PROPERTY_THEN_ENABLE,
  AYF_PROPERTY_THEN_DISABLE,
}

class AudYoFloSettingsConfigurationDepend {
  String propAddress = '';
  AudYoFloSettingsConfigurationPropThenAction thenAction =
      AudYoFloSettingsConfigurationPropThenAction.AYF_PROPERTY_THEN_DISABLE;
}

class AudYoFloSettingsConfigurationDependency {
  String propertyCheckIf = '';

  AudYoFloSettingsConfigurationPropIfCondition ifCondition =
      AudYoFloSettingsConfigurationPropIfCondition.AYF_PROPERTY_IF_EXISTS;
  List<AudYoFloSettingsConfigurationDepend> propertyCheckThen = [];
}

// =========================================================================

class AudYoFloSettingsConfigurationSection {
  String nameEntry = '';
  String nameImage = '';
  // List<AudYoFloSettingsConfigurationDependency> lstDependencies = [];
  Map<JvxComponentIdentification, AudYoFloOneComponentSettings> theSettings =
      {};

  int addEntry(AudYoFloOneComponentSettings addMe) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    MapEntry<JvxComponentIdentification, AudYoFloOneComponentSettings>? elm =
        theSettings.entries.firstWhereOrNull((element) => false);
    if (elm == null) {
      theSettings[addMe.cpId] = addMe;
    }
    return errCode;
  }

  /*
  int addEntryList(JvxComponentIdentification cpId, String prop) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    MapEntry<JvxComponentIdentification, AudYoFloOneComponentSettings>? elm =
        theSettings.entries.firstWhereOrNull((element) => false);
    if (elm != null) {
      elm.value.propsSetting.add(prop);
    }
    return errCode;
  }
  */

  int remEntry(JvxComponentIdentification cpId) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    MapEntry<JvxComponentIdentification, AudYoFloOneComponentSettings>? elm =
        theSettings.entries.firstWhereOrNull((element) => false);
    if (elm != null) {
      theSettings.remove(cpId);
    }
    return errCode;
  }

  /*
  void addDependency(AudYoFloSettingsConfigurationDependency newDep) {
    lstDependencies.add(newDep);
  }
  */
}

class AudYoFloSettingsConfiguration {
  Map<String, AudYoFloSettingsConfigurationSection> allSections = {};
}
