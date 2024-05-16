import 'package:flutter/foundation.dart';

// ===========================================================

enum jvxPropertyAccessTypeEnum {
  JVX_PROPERTY_ACCESS_NONE,
  JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE,
  JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT,
  JVX_PROPERTY_ACCESS_READ_ONLY,
  JVX_PROPERTY_ACCESS_READ_ONLY_ON_START,
  JVX_PROPERTY_ACCESS_WRITE_ONLY
}

extension jvxPropertyAccessTypeEEnum on jvxPropertyAccessTypeEnum {
  static String get formatName => "jvxPropertyAccessType";
  String get txt => describeEnum(this);
  static jvxPropertyAccessTypeEnum fromInt(int val) {
    jvxPropertyAccessTypeEnum ss =
        jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_NONE;
    if ((val >= 0) && (val < jvxPropertyAccessTypeEnum.values.length)) {
      ss = jvxPropertyAccessTypeEnum.values[val];
    }
    return ss;
  }
}

// ===========================================================

enum jvxPropertyCategoryTypeEnum {
  JVX_PROPERTY_CATEGORY_UNKNOWN,
  JVX_PROPERTY_CATEGORY_PREDEFINED,
  JVX_PROPERTY_CATEGORY_UNSPECIFIC
}

extension jvxPropertyCategoryTypeEEnum on jvxPropertyCategoryTypeEnum {
  static String get formatName => "jvxPropertyCategoryType";
  String get txt => describeEnum(this);
  static jvxPropertyCategoryTypeEnum fromInt(int val) {
    jvxPropertyCategoryTypeEnum ss =
        jvxPropertyCategoryTypeEnum.JVX_PROPERTY_CATEGORY_UNKNOWN;
    if ((val >= 0) && (val < jvxPropertyCategoryTypeEnum.values.length)) {
      ss = jvxPropertyCategoryTypeEnum.values[val];
    }
    return ss;
  }
}

// ===========================================================

enum jvxPropertyInvalidateTypeEnum {
  JVX_PROPERTY_INVALIDATE_INACTIVE,
  JVX_PROPERTY_INVALIDATE_CONTENT,
  JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT,
  JVX_PROPERTY_INVALIDATE_LIMIT
}

extension jvxPropertyInvalidateTypeEEnum on jvxPropertyInvalidateTypeEnum {
  static String get formatName => "jvxPropertyInvalidateType";
  String get txt => describeEnum(this);
  static jvxPropertyInvalidateTypeEnum fromInt(int val) {
    jvxPropertyInvalidateTypeEnum ss =
        jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_INACTIVE;
    if ((val >= 0) && (val < jvxPropertyInvalidateTypeEnum.values.length)) {
      ss = jvxPropertyInvalidateTypeEnum.values[val];
    }
    return ss;
  }
}

// ===========================================================

enum jvxPropertyDecoderHintTypeEnum {
  JVX_PROPERTY_DECODER_NONE,
  JVX_PROPERTY_DECODER_FILENAME_OPEN,
  JVX_PROPERTY_DECODER_DIRECTORY_SELECT,
  JVX_PROPERTY_DECODER_FILENAME_SAVE,
  JVX_PROPERTY_DECODER_IP_ADDRESS,
  JVX_PROPERTY_DECODER_PLOT_ARRAY,
  JVX_PROPERTY_DECODER_BITFIELD,
  JVX_PROPERTY_DECODER_SINGLE_SELECTION,
  JVX_PROPERTY_DECODER_MULTI_SELECTION,
  JVX_PROPERTY_DECODER_FORMAT_IDX,
  JVX_PROPERTY_DECODER_PROGRESSBAR,
  JVX_PROPERTY_DECODER_INPUT_FILE_LIST,
  JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST,
  JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER,
  JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER,
  JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER,
  JVX_PROPERTY_DECODER_COMMAND,
  JVX_PROPERTY_DECODER_SIMPLE_ONOFF,
  JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE,
  JVX_PROPERTY_DECODER_SUBFORMAT_IDX,
  JVX_PROPERTY_DECODER_DATAFLOW_IDX,
  JVX_PROPERTY_DECODER_ENUM_TYPE,
  JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE,
  JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG,
}

extension jvxPropertyDecoderHintTypeEEnum on jvxPropertyDecoderHintTypeEnum {
  static String get formatName => "jvxPropertyDecoderHintType";
  String get txt => describeEnum(this);
  static jvxPropertyDecoderHintTypeEnum fromInt(int val) {
    jvxPropertyDecoderHintTypeEnum ss =
        jvxPropertyDecoderHintTypeEnum.JVX_PROPERTY_DECODER_NONE;
    if ((val >= 0) && (val < jvxPropertyDecoderHintTypeEnum.values.length)) {
      ss = jvxPropertyDecoderHintTypeEnum.values[val];
    }
    return ss;
  }
}

// ===========================================================

enum jvxPropertyContextEnum {
  JVX_PROPERTY_CONTEXT_UNKNOWN,
  JVX_PROPERTY_CONTEXT_PARAMETER,
  JVX_PROPERTY_CONTEXT_INFO,
  JVX_PROPERTY_CONTEXT_RESULT,
  JVX_PROPERTY_CONTEXT_COMMAND,
  JVX_PROPERTY_CONTEXT_VIEWBUFFER,
  JVX_PROPERTY_CONTEXT_LIMIT
}

extension jvxPropertyContexteEEnum on jvxPropertyContextEnum {
  static String get formatName => "jvxPropertyContext";
  String get txt => describeEnum(this);
  static jvxPropertyContextEnum fromInt(int val) {
    jvxPropertyContextEnum ss =
        jvxPropertyContextEnum.JVX_PROPERTY_CONTEXT_UNKNOWN;
    if ((val >= 0) && (val < jvxPropertyContextEnum.values.length)) {
      ss = jvxPropertyContextEnum.values[val];
    }
    return ss;
  }
}
