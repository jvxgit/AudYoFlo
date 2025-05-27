import 'dart:typed_data';
import 'dart:ffi';
import 'package:ayf_pack/ayf_pack.dart';

class AudYoFloPropertyContentFromJsonNative {
  static int updatePropertyContentFromJsonMap_field(
      AudYoFloPropertyContentBackend prop,
      Map newValues,
      AudYoFloBackendTranslator trans) {
    int errCode = jvxErrorType.JVX_NO_ERROR;

    switch (prop.jvxFormat) {
      case jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE:
        if (prop is AudYoFloPropertyMultiContentNative<Int64, Int64List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Int64List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;
      case jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE:
      case jvxDataFormatEnum.JVX_DATAFORMAT_SIZE:
        if (prop is AudYoFloPropertyMultiContentNative<Uint64, Uint64List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Uint64List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;

      //32 BIT FIELDS
      case jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE:
        if (prop is AudYoFloPropertyMultiContentNative<Int32, Int32List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Int32List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;
      case jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE:
        if (prop is AudYoFloPropertyMultiContentNative<Uint32, Uint32List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Uint32List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;

      // 16BIT FIELDS
      case jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE:
        if (prop is AudYoFloPropertyMultiContentNative<Int16, Int16List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Int16List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;
      case jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE:
        if (prop is AudYoFloPropertyMultiContentNative<Uint16, Uint16List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Uint16List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;

      // 8BIT FIELDS
      case jvxDataFormatEnum.JVX_DATAFORMAT_8BIT:
        if (prop is AudYoFloPropertyMultiContentNative<Int8, Int8List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Int8List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;
      case jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT:
        if (prop is AudYoFloPropertyMultiContentNative<Uint8, Uint8List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Uint8List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;

      // DOUBLE/FLOAT FIELDS
      case jvxDataFormatEnum.JVX_DATAFORMAT_DATA:
        if (prop is AudYoFloPropertyMultiContentNative<Double, Float64List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Float64List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else if (prop
            is AudYoFloPropertyMultiContentNative<Float, Float32List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Float32List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;
      default:
        assert(false);
        break;
    }

    if (errCode != jvxErrorType.JVX_NO_ERROR) {
      var str1 = prop.descriptor;
      var str2 = AudYoFloHelper.extractStringFromJson(newValues, 'property');
      str2 ??= 'unknown';
      var str3 = jvxErrorTypeEInt.toStringSingle(errCode);
      AudYoFloHelper.dbgPrint(
          'Error converting property <$str1> from <$str2>, reason: $str3');
    }
    return errCode;
  }
}

// This is a global function to drive different host types to different calls: native access here, but
// there is another call in the web code!!
int updatePropertyContentFromJsonMap_fields(AudYoFloPropertyContentBackend prop,
    Map newValues, AudYoFloBackendTranslator trans) {
  return AudYoFloPropertyContentFromJsonNative
      .updatePropertyContentFromJsonMap_field(prop, newValues, trans);
}
