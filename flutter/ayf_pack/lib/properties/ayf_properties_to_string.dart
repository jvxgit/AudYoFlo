import 'dart:math';
import 'dart:typed_data';
import 'package:fixnum/fixnum.dart';
import '../ayf_pack_local.dart';

class AudYoFloPropertyContentToString {
  // Currently not supported by backend!
  static String convertPropMultiStringToString(
      AudYoFloPropertyMultiStringBackend prop) {
    String retVal = '[';
    for (var elm in prop.entries) {
      if (elm != prop.entries.first) {
        retVal += ',';
      }
      retVal += elm;
    }
    retVal += ']';
    return retVal;
  }

  static String convertPropSelectionListToString(
      AudYoFloPropertySelectionListBackend prop, bool contentOnly) {
    String retVal = '';
    bool multiCont = false;
    if (prop.selection is AudYoFloBitFieldBackend) {
      AudYoFloBitFieldBackend sel = prop.selection as AudYoFloBitFieldBackend;
      multiCont = (sel.numEntries > 1);
      if (multiCont) {
        retVal += '[';
      }
      String oneEntry = '';
      for (var idx = 0; idx < sel.numEntries; idx++) {
        if(idx > 0) {
        retVal += ",";
      }
        for (var idxi = 0; idxi < sel.compileNum32BitBitfield; idxi++) {
          
          // For multi selections, consider the offset!!
          int idxO = idx * sel.compileNum32BitBitfield + idxi;
          if ((sel.bitfield![idxO] != 0) ||
              (idxi == sel.compileNum32BitBitfield - 1)) {
            // Make sure you see 8 digits per token!!
            String tok = sel.bitfield![idxO].toRadixString(16);
            for (int ii = tok.length; ii < 8; ii++) {
              oneEntry += '0';
            }
            oneEntry += tok;
          }
        }
      
      oneEntry = '0x' + oneEntry;
      retVal += oneEntry;
      oneEntry = '';
      }
      if (multiCont) {
        retVal += ']';
      }
    }

    if (!contentOnly) {
      AudYoFloBitFieldBackend sela = prop.selectable as AudYoFloBitFieldBackend;
      String oneEntry = '';
      for (var idxi = 0; idxi < sela.compileNum32BitBitfield; idxi++) {
        if ((sela.bitfield![idxi] != 0) ||
            (idxi == sela.compileNum32BitBitfield - 1)) {
          // Make sure you see 8 digits per token!!
          String tok = sela.bitfield![idxi].toRadixString(16);
          for (int ii = tok.length; ii < 8; ii++) {
            oneEntry += '0';
          }
          oneEntry += tok;
        }
      }
      oneEntry = '0x' + oneEntry;
      retVal += '::' + oneEntry;

      AudYoFloBitFieldBackend excl = prop.selectable as AudYoFloBitFieldBackend;
      oneEntry = '';
      for (var idxi = 0; idxi < excl.compileNum32BitBitfield; idxi++) {
        if ((excl.bitfield![idxi] != 0) ||
            (idxi == excl.compileNum32BitBitfield - 1)) {
          // Make sure you see 8 digits per token!!
          String tok = excl.bitfield![idxi].toRadixString(16);
          for (int ii = tok.length; ii < 8; ii++) {
            oneEntry += '0';
          }
          oneEntry += tok;
        }
      }
      oneEntry = '0x' + oneEntry;
      retVal += '::' + oneEntry;

      oneEntry = '';
      for (var elm in prop.parpropms.entries) {
        if (elm != prop.parpropms.entries.first) {
          oneEntry += ':';
        }
        retVal += '::' + oneEntry;
      }
    }
    return retVal;
  }

  static String? convertPropValueInRangeToString(
      AudYoFloPropertyValueInRangeBackend prop, bool contentOnly,
      {int precision = 4}) {
    String? retVal =
        convertPropMultiContentLst(prop.parpropmc.fld, precision: precision);
    if (retVal != null) {
      if (!contentOnly) {
        retVal += '::' + prop.minVal.toString();
        retVal += '::' + prop.maxVal.toString();
      }
    }
    return retVal;
  }

  static String? convertPropMultiContentLst(dynamic lst, {int offset = 0, int num = -1, int precision = 4}) {
    String retVal = '';
    bool multiCont = false;
    if (lst is List) {
      multiCont = (lst.length > 1);
    }
    if (multiCont) {
      retVal = '[';
    }

    int numCopy = lst.length - offset; 
    if(num >= 0)
    {
      numCopy = min(numCopy, num);
    }

    if (lst is Float64List) {      
        for (var i=0;i<numCopy; i++) {
          if(i != 0) retVal += ',';
        retVal += lst[i+offset].toStringAsPrecision(precision);
      }
    } else if (lst is Float32List) {
              for (var i=0;i<numCopy; i++) {
          if(i != 0) retVal += ',';
          retVal += lst[i+offset].toStringAsPrecision(precision);
      }
    } else {
       for (var i=0;i<numCopy; i++) {
        if(i != 0) retVal += ',';
        retVal += lst[i+offset].toString();
      }
    }

    if (multiCont) {
      retVal += ']';
    }
    return retVal;
  }

  static String? convertPropertyContentToString(
      AudYoFloPropertyContainer prop, bool contentOnly,
      {int offset = 0, int num = -1, int precision = 4 }) {
    String? retVal;

    switch (prop.jvxFormat) {
      // STRINGLIST
      case jvxDataFormatEnum.JVX_DATAFORMAT_STRING_LIST:
        if (prop is AudYoFloPropertyMultiStringBackend) {
          retVal = convertPropMultiStringToString(prop);
        }
        break;

      case jvxDataFormatEnum.JVX_DATAFORMAT_STRING:
        if (prop is AudYoFloPropertySingleStringBackend) {
          retVal = prop.value;
        }
        break;

      case jvxDataFormatEnum.JVX_DATAFORMAT_SELECTION_LIST:
        if (prop is AudYoFloPropertySelectionListBackend) {
          AudYoFloPropertySelectionListBackend propSpec =
              prop as AudYoFloPropertySelectionListBackend;
          retVal = convertPropSelectionListToString(propSpec, contentOnly);
        }
        break;

      case jvxDataFormatEnum.JVX_DATAFORMAT_VALUE_IN_RANGE:
        if (prop is AudYoFloPropertyValueInRangeBackend) {
          AudYoFloPropertyValueInRangeBackend propSpec =
              prop as AudYoFloPropertyValueInRangeBackend;
          retVal = convertPropValueInRangeToString(propSpec, contentOnly,
              precision: precision);
        }
        break;
      //64 BIT FIELDS
      case jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE:
      case jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE:
      case jvxDataFormatEnum.JVX_DATAFORMAT_SIZE:
      case jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE:
      case jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE:
      case jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE:
      case jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE:
      case jvxDataFormatEnum.JVX_DATAFORMAT_8BIT:
      case jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT:
      case jvxDataFormatEnum.JVX_DATAFORMAT_DATA:
        if (prop is AudYoFloPropertyMultiContentBackend) {
          AudYoFloPropertyMultiContentBackend propSpec =
              prop as AudYoFloPropertyMultiContentBackend;
          retVal =
              convertPropMultiContentLst(propSpec.fld, offset: offset, num:num, precision: precision);
        }
        break;
    }

    return retVal;
  }
}
