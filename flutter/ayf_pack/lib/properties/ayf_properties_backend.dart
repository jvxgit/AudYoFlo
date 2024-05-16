import '../../ayf_pack_local.dart';

enum jvxPropertyProgressStates {
  JVX_PROPERTY_PROGRESS_NONE,
  JVX_PROPERTY_PROGRESS_WAIT
}

// ##################################################################
// ##################################################################
class AudYoFloPropertyDescriptorBackend extends AudYoFloPropertyContainer {
  // Last result from operation
  int last_error = jvxErrorType.JVX_NO_ERROR;

  // Update of this cache location

  // Show which parts are valid
  AudYoFloCBitField32 valid_parts = AudYoFloCBitField32();

  // =====================================================================

  // Gobal idx in the context of this component
  int globalIdx = -1;

  // Property category
  jvxPropertyCategoryTypeEnum category =
      jvxPropertyCategoryTypeEnum.JVX_PROPERTY_CATEGORY_UNKNOWN;

  // =====================================================================
  // State in which the componnent is valid
  AudYoFloCBitField32 allowedStateMask = AudYoFloCBitField32();

  // Not used currently
  AudYoFloCBitField32 allowedThreadingMask = AudYoFloCBitField32();

  // Default setting to enable that properties are invalidated on state switch
  jvxPropertyInvalidateTypeEnum reqInvalidateOnStateSwitch =
      jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_INACTIVE;
  jvxPropertyInvalidateTypeEnum reqInvalidateOnTest =
      jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_INACTIVE;

  bool installable = false;

  // Access type of this property
  jvxPropertyAccessTypeEnum accessType =
      jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_NONE;

  // Decoder hint type
  jvxPropertyDecoderHintTypeEnum decoderHintType =
      jvxPropertyDecoderHintTypeEnum.JVX_PROPERTY_DECODER_NONE;

  jvxPropertyContextEnum ctxt =
      jvxPropertyContextEnum.JVX_PROPERTY_CONTEXT_UNKNOWN;

  // =====================================================================

  // Validity as returned from the API
  bool isValid = false;

  // ============================================
  // Name as returnd from API
  String name = '';

  // ==============================================================

  // Origin (if available)
  String origin = '';

  AudYoFloCBitField64 accessFlags = AudYoFloCBitField();

  AudYoFloCBitField32 configFlags = AudYoFloCBitField32();

  // ==============================================================
  AudYoFloPropertyDescriptorBackend(JvxComponentIdentification assCpIdentArg,
      {String descriptor = '',
      String description = '',
      jvxDataFormatEnum jvxFormat = jvxDataFormatEnum.JVX_DATAFORMAT_NONE,
      int num = 1})
      : super(jvxFormat, num, descriptor, description) {
    super.assCpIdent = assCpIdentArg;
    descrType = jvxPropertyDescriptorTypeEnum.JVX_PROPERTY_BE_DESCRIPTOR;
    cache_status.bitSet(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_INVALID.index);
    cache_status.bitSet(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_INVALID.index);
  }

  // ==============================================================

  void validateDescriptor() {
    cache_status.bitClear(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_INVALID.index);
    cache_status.bitClear(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_ERROR.index);
    cache_status.bitSet(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_VALID.index);
    ssUpdateId++;
  }

  void validateContent() {
    cache_status.bitClear(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_INVALID.index);
    cache_status.bitClear(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_ERROR.index);
    cache_status.bitSet(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_VALID.index);
    ssUpdateId++;
  }

  // ==============================================================

  void invalidateDescriptor(bool error) {
    cache_status.bitClear(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_VALID.index);
    if (error) {
      cache_status.bitSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_ERROR.index);
    } else {
      cache_status.bitSet(jvxPropertyCacheStatusFlagShifts
          .JVX_PROPERTY_DESCRIPTOR_INVALID.index);
    }
    ssUpdateId++;
  }

  void invalidateContent(bool error) {
    // Remove all flags
    cache_status.bitClear(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_VALID.index);
    cache_status.bitClear(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_ERROR.index);
    cache_status.bitClear(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_INVALID.index);

    // Set the appropriate flag
    if (error) {
      cache_status.bitSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_ERROR.index);
    } else {
      cache_status.bitSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_INVALID.index);
    }
    ssUpdateId++;
  }

  // ==============================================================
  // ==============================================================

  @override
  void dispose() {
    cache_status.bitFClear();
  }
}

// =====================================================================
// =====================================================================

class AudYoFloPropertyContentBackend extends AudYoFloPropertyDescriptorBackend {
  AudYoFloPropertyContentBackend(JvxComponentIdentification assCpIdent,
      {String descriptor = '',
      jvxDataFormatEnum format = jvxDataFormatEnum.JVX_DATAFORMAT_NONE,
      jvxDataTypeSpecEnum dataType =
          jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE,
      int num = 1})
      : super(assCpIdent, descriptor: descriptor, jvxFormat: format, num: num);

  String toString() {
    return 'Not Supported';
  }

  String toHelperString() {
    return '';
  }

  // Show if an error occurred on the retrieval of the content
  String errDescription = '';

  jvxDataTypeSpecEnum dataType = jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE;

  jvxPropertyProgressStates setInProgress =
      jvxPropertyProgressStates.JVX_PROPERTY_PROGRESS_NONE;

  jvxPropertyProgressStates getInProgress =
      jvxPropertyProgressStates.JVX_PROPERTY_PROGRESS_NONE;

  // Show that content is no longer valid. If it is invalid, this may be due to
  // a state change. In that case, we need to remove it
  @override
  bool invalidateOnStateSwitch(jvxStateSwitchEnum ss) {
    bool removeFromCache = false;

    switch (reqInvalidateOnStateSwitch) {
      case jvxPropertyInvalidateTypeEnum
          .JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT:
        invalidateDescriptor(false);
        invalidateContent(false); /* invalidate also content */
        break;

      case jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_CONTENT:
        invalidateContent(false);
        break;
      // Set the status back to initial
      //cache_status.bitFClear();
      default:
        int stateFlagNew = 0;
        switch (ss) {
          case jvxStateSwitchEnum.JVX_STATE_SWITCH_SELECT:
            stateFlagNew = jvxState.JVX_STATE_SELECTED;
            break;
          case jvxStateSwitchEnum.JVX_STATE_SWITCH_ACTIVATE:
            stateFlagNew = jvxState.JVX_STATE_ACTIVE;
            break;
          case jvxStateSwitchEnum.JVX_STATE_SWITCH_PREPARE:
            stateFlagNew = jvxState.JVX_STATE_PREPARED;
            break;
          case jvxStateSwitchEnum.JVX_STATE_SWITCH_START:
            stateFlagNew = jvxState.JVX_STATE_PROCESSING;
            break;
          case jvxStateSwitchEnum.JVX_STATE_SWITCH_STOP:
            stateFlagNew = jvxState.JVX_STATE_PREPARED;
            break;
          case jvxStateSwitchEnum.JVX_STATE_SWITCH_POSTPROCESS:
            stateFlagNew = jvxState.JVX_STATE_ACTIVE;
            break;
          case jvxStateSwitchEnum.JVX_STATE_SWITCH_DEACTIVATE:
            stateFlagNew = jvxState.JVX_STATE_SELECTED;
            break;
          default:
        }
        if ((allowedStateMask.fld & stateFlagNew) == 0) {
          removeFromCache = true;
        }
    }
    return removeFromCache;
  }

  /*
  int descriptorFromJsonMap(AudYoFloBackendAdapter ref, Map jsonMap) {
    int res = jvxErrorType.JVX_NO_ERROR;

    return res;
  }
  */
}

// ==================================================================
// ==================================================================

abstract class AudYoFloPropertyMultiContentBackend<T2> {
  T2? fld;
  int fldSz = 0;
  int numDigits = 4;
  AudYoFloPropertyContentBackend get parpropc;
}

// ==================================================================
// ==================================================================

// Single string class
class AudYoFloPropertySingleStringBackend
    extends AudYoFloPropertyContentBackend {
  // Single value string
  String value = '';

  @override
  String toString() {
    return value;
  }

  AudYoFloPropertySingleStringBackend(JvxComponentIdentification assCpIdent)
      : super(assCpIdent, format: jvxDataFormatEnum.JVX_DATAFORMAT_STRING);
}

// ================================================================
// ================================================================

// Hold string list property
class AudYoFloPropertyMultiStringBackend
    extends AudYoFloPropertyContentBackend {
  // Actual array of entries
  List<String> entries = [];

  @override
  String toHelperString() {
    String out = '';
    for (var elmS in entries) {
      if (out.isEmpty) {
        out = elmS;
      } else {
        out = out + ', ' + elmS;
      }
    }
    out = '[' + out + ']';
    return out;
  }

  @override
  String toString() {
    String out = '';
    for (var elmS in entries) {
      if (out.isEmpty) {
        out = elmS;
      } else {
        out = out + ', ' + elmS;
      }
    }
    out = '[' + out + ']';
    return out;
  }

  // Destructor
  @override
  void dispose() {
    entries.clear();
    super.dispose();
  }

  AudYoFloPropertyMultiStringBackend(JvxComponentIdentification cpId,
      {jvxDataFormatEnum format = jvxDataFormatEnum.JVX_DATAFORMAT_STRING_LIST})
      : super(cpId, format: format);
}

// ================================================================
// ================================================================
abstract class AudYoFloPropertyValueInRangeBackend {
  double minVal = 0;
  double maxVal = 1;
  AudYoFloPropertyMultiContentBackend get parpropmc;
}

// ================================================================
// ================================================================

abstract class AudYoFloPropertySelectionListBackend {
  int compileNum32BitBitfield = 0;
  AudYoFloBitField selection = allocateAudYoFloBitfield();
  AudYoFloBitField selectable = allocateAudYoFloBitfield();
  AudYoFloBitField exclusive = allocateAudYoFloBitfield();
  AudYoFloPropertyMultiStringBackend get parpropms;
}
