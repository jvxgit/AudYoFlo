import 'dart:typed_data';
import 'package:ayf_pack/ayf_pack_web.dart';
import 'package:ayf_pack/properties/web/ayf_properties.dart';
import 'package:fixnum/fixnum.dart' as fn;
import '../../ayf_pack_local.dart';

class AudYoFloPropertyContentFromJsonSimple {
  static int updatePropertyContentFromJsonMap(
      AudYoFloPropertyContentBackend prop, Map newValues, AudYoFloBackendTranslator trans) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    switch (prop.jvxFormat) {
      case jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE:
        if (prop is AudYoFloPropertyMultiContentWeb<Int64List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Int64List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;
      case jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE:
      case jvxDataFormatEnum.JVX_DATAFORMAT_SIZE:
        if (prop is AudYoFloPropertyMultiContentWeb<Uint64List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Uint64List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;

      //32 BIT FIELDS
      case jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE:
        if (prop is AudYoFloPropertyMultiContentWeb<Int32List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Int32List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;
      case jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE:
        if (prop is AudYoFloPropertyMultiContentWeb<Uint32List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Uint32List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;

      // 16BIT FIELDS
      case jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE:
        if (prop is AudYoFloPropertyMultiContentWeb<Int16List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Int16List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;
      case jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE:
        if (prop is AudYoFloPropertyMultiContentWeb<Uint16List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Uint16List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;

      // 8BIT FIELDS
      case jvxDataFormatEnum.JVX_DATAFORMAT_8BIT:
        if (prop is AudYoFloPropertyMultiContentWeb<Int8List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Int8List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;
      case jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT:
        if (prop is AudYoFloPropertyMultiContentWeb<Uint8List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Uint8List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else {
          errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
        }
        break;

      // DOUBLE/FLOAT FIELDS
      case jvxDataFormatEnum.JVX_DATAFORMAT_DATA:
        if (prop is AudYoFloPropertyMultiContentWeb<Float64List>) {
          errCode =
              AudYoFloPropertyContentFromJson.convertTextToField<Float64List>(
                  prop.fld, prop.fldSz, newValues, prop.decoderHintType, trans);
        } else if (prop is AudYoFloPropertyMultiContentWeb<Float32List>) {
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
    return errCode;
  }
}

// This is a global function to drive different host types to different calls: web access here, but
// there is another call in the native code!!
int updatePropertyContentFromJsonMap_fields(
    AudYoFloPropertyContentBackend prop, Map newValues, AudYoFloBackendTranslator trans) {
  return AudYoFloPropertyContentFromJsonSimple.updatePropertyContentFromJsonMap(
      prop, newValues, trans);
}
