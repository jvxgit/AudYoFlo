import '../ayf_pack_local.dart';

enum jvxPropertyCacheStatusFlagShifts {
  JVX_PROPERTY_DESCRIPTOR_INVALID,
  JVX_PROPERTY_DESCRIPTOR_VALID,
  JVX_PROPERTY_DESCRIPTOR_ERROR,

  JVX_PROPERTY_CONTENT_INVALID,
  JVX_PROPERTY_CONTENT_VALID,
  JVX_PROPERTY_CONTENT_ERROR,

  JVX_PROPERTY_CONTENT_SET
}

enum jvxPropertyDescriptorTypeEnum {
  JVX_PROPERTY_MIN_DESCRIPTOR,
  JVX_PROPERTY_BE_DESCRIPTOR
}

class AudYoFloPropertyContainer {
  int ssUpdateId = 0;

  // Cache status to hold indicators for every state
  AudYoFloCBitField32 cache_status = AudYoFloCBitField32();

  jvxPropertyDescriptorTypeEnum descrType =
      jvxPropertyDescriptorTypeEnum.JVX_PROPERTY_MIN_DESCRIPTOR;

  // Associated component, required for set_property
  JvxComponentIdentification assCpIdent = JvxComponentIdentification();

  // Format
  jvxDataFormatEnum jvxFormat = jvxDataFormatEnum.JVX_DATAFORMAT_NONE;

// Number element
  int num = 0;

// Description of the property
  String description = '';

  // Descriptor to address
  String descriptor;

  Map<String, dynamic> attachedSpecific = {};

  AudYoFloPropertyContainer(
      this.jvxFormat, this.num, this.descriptor, this.description);

  // Some interface functions
  void dispose() {}

  bool invalidateOnStateSwitch(jvxStateSwitchEnum ss) {
    return false;
  }

  bool checkValidDescriptor() {
    return cache_status.bitTest(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_VALID.index);
  }

  bool checkValidContent() {
    return cache_status.bitTest(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_VALID.index);
  }

  bool checkInvalidContent() {
    return cache_status.bitTest(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_INVALID.index);
  }
}

class AudYoFloPropertyContentLocal extends AudYoFloPropertyContainer {
  AudYoFloPropertyContentLocal(
      jvxDataFormatEnum format, int num, String descriptor, String description)
      : super(format, num, descriptor, description);

  @override
  String toString() {
    return 'not-implemented';
  }
}

class AudYoFloPropertyStringLocal extends AudYoFloPropertyContentLocal {
  String value;

  AudYoFloPropertyStringLocal(this.value, String descriptor, String description)
      : super(jvxDataFormatEnum.JVX_DATAFORMAT_STRING, 1, descriptor,
            description);

  @override
  String toString() {
    return value;
  }
}

class AudYoFloPropertyValue extends AudYoFloPropertyContentLocal {
  double value;

  AudYoFloPropertyValue(this.value, String descriptor, String description)
      : super(jvxDataFormatEnum.JVX_DATAFORMAT_STRING, 1, descriptor,
            description);

  @override
  String toString() {
    return value.toString();
  }
}

class AudYoFloPropertySelectionListLocal extends AudYoFloPropertyContentLocal {
  List<String> entries;
  AudYoFloBitField selection = allocateAudYoFloBitfield();
  jvxPropertyDecoderHintTypeEnum decTp;
  AudYoFloPropertySelectionListLocal(
      this.entries, String descriptor, String description, this.decTp)
      : super(jvxDataFormatEnum.JVX_DATAFORMAT_SELECTION_LIST, 1, descriptor,
            description);

  @override
  String toString() {
    String out = '';
    for (var elmS in entries) {
      if (out.isEmpty) {
        out = elmS;
      } else {
        out = '$out, $elmS';
      }
    }
    out = '[$out]';
    return out;
  }
}

// ============================================================================

