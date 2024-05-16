import 'dart:typed_data';
import 'package:fixnum/fixnum.dart';
import '../ayf_pack_local.dart';

class AudYoFloPropertyContentFromString {
  static int convertElementFldListInt64(Int64List fld, int idx, String token) {
    int? valI = int.tryParse(token);
    if (valI != null) {
      fld[idx] = valI.toSigned(64);
      return jvxErrorType.JVX_NO_ERROR;
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int convertElementFldListUint64(
      Uint64List fld, int idx, String token) {
    int? valI = int.tryParse(token);
    if (valI == null) {
    
      if(token == "unselected")
      {
        valI = -1;
      }
    }
    if (valI != null) {
      fld[idx] = valI.toUnsigned(64);
      return jvxErrorType.JVX_NO_ERROR;
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int convertElementFldListInt32(Int32List fld, int idx, String token,
      jvxPropertyDecoderHintTypeEnum decHtTp) {
    int? valI = int.tryParse(token);
    if (valI != null) {
      fld[idx] = valI.toSigned(32);
      return jvxErrorType.JVX_NO_ERROR;
    } else {
      if (decHtTp ==
          jvxPropertyDecoderHintTypeEnum
              .JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE) {
        if (token == 'dontcare') {
          fld[idx] = -1;
          return jvxErrorType.JVX_NO_ERROR;
        }
      }
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int convertElementFldListUint32(
      Uint32List fld, int idx, String token) {
    int? valI = int.tryParse(token);
    if (valI != null) {
      fld[idx] = valI.toUnsigned(32);
      return jvxErrorType.JVX_NO_ERROR;
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int convertElementFldListInt16(Int16List fld, int idx, String token,
      jvxPropertyDecoderHintTypeEnum decHtTp, AudYoFloBackendTranslator trans) {
    int? valI = int.tryParse(token);
    valI ??= decodeTextDecoderTypes(token, decHtTp, trans);
    
    if (valI != null) {
      fld[idx] = valI.toSigned(16);
      return jvxErrorType.JVX_NO_ERROR;
    } 
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int convertElementFldListUint16(
      Uint16List fld, int idx, String token, jvxPropertyDecoderHintTypeEnum decTp, AudYoFloBackendTranslator trans) {
    int? valI = int.tryParse(token);
    valI ??= decodeTextDecoderTypes(token, decTp, trans);

    if (valI != null) {
        fld[idx] = valI.toUnsigned(16);
      return jvxErrorType.JVX_NO_ERROR;
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int convertElementFldListInt8(Int8List fld, int idx, String token) {
    int? valI = int.tryParse(token);
    if (valI != null) {
      fld[idx] = valI.toSigned(8);
      return jvxErrorType.JVX_NO_ERROR;
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int convertElementFldListUint8(Uint8List fld, int idx, String token) {
    int? valI = int.tryParse(token);
    if (valI != null) {
      fld[idx] = valI.toUnsigned(8);
      return jvxErrorType.JVX_NO_ERROR;
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int convertElementFldListDouble(
      Float64List fld, int idx, String token) {
    double? valD = double.tryParse(token);
    if (valD != null) {
      fld[idx] = valD;
      return jvxErrorType.JVX_NO_ERROR;
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int convertElementFldListFloat(
      Float32List fld, int idx, String token) {
    double? valD = double.tryParse(token);
    if (valD != null) {
      fld[idx] = valD;
      return jvxErrorType.JVX_NO_ERROR;
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }

  static int updateFieldBitField(Uint32List buf, int numEntries,
      List<String> bitfldTextLst, int compileNum32BitBitfield) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    assert(numEntries == bitfldTextLst.length);
    int offs = 0;
    for (var token in bitfldTextLst) {
      // int idxEntry = posi >> 5;
      // int inWordIdx = posi - (idxEntry << 5);
      // int inFldIdx = compileNum32BitBitfield - 1 - idxEntry;
      // inFldIdx += offset * compileNum32BitBitfield;

      var bitfldText = token;
      String prefix = bitfldText.substring(0, 2);

      if (prefix == '0x') {
        String content = bitfldText.substring(2);
        int ll = content.length;
        int numEntriesW32 = ll >> 3; // /8

        // Reset the bitfield partials
        for (int idx = 0; idx < compileNum32BitBitfield; idx++) {
          var idxInBuf = compileNum32BitBitfield - idx - 1 + offs;
          buf[idxInBuf] = 0;
        }

        for (int idx = 0; idx < numEntriesW32; idx++) {
          String partStr = content.substring(ll - 8 * (idx + 1), ll - 8 * idx);
          Int64 val = Int64.parseHex(
              partStr); // better tryParseHex but it does not exist
          //if (val != null) {
          var idxInBuf = compileNum32BitBitfield - idx - 1 + offs;
          buf[idxInBuf] = val.toUnsigned(32).toInt();
          //} else {
          //  errCode = jvxErrorType.JVX_ERROR_PARSE_ERROR;
          //}
        }
      } else {
        errCode = jvxErrorType.JVX_ERROR_PARSE_ERROR;
      }
      offs += compileNum32BitBitfield;
    }
    return errCode;
  }

  static bool boolFromString(String str) {
    if ((str == '*') || (str == 'yes')) {
      return true;
    }
    return false;
  }

  static int? decodeTextDecoderTypes(String token, jvxPropertyDecoderHintTypeEnum decTp, AudYoFloBackendTranslator trans)
{
  int? valI;
    switch (decTp) {
      case jvxPropertyDecoderHintTypeEnum.JVX_PROPERTY_DECODER_SIMPLE_ONOFF:
        if (token == "yes") {
          valI = 1;
        } else {
          valI = 0;
        }
        break;
      case jvxPropertyDecoderHintTypeEnum.JVX_PROPERTY_DECODER_FORMAT_IDX:
        valI = trans.translateEnumString(token, 'jvxDataFormat', trans.compileFlags() );
        break;
      default:
        dbgPrint(
            'Failed to decode text string <$token> for property of type <${decTp.toString()}');
        break;
    }
    return valI;
  }

}
