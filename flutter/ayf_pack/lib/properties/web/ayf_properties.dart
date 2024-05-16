import 'package:ayf_pack/ayf_pack_web.dart';
import 'dart:typed_data';
import 'package:fixnum/fixnum.dart' as fn;
import 'dart:math';

import '../../ayf_pack_local.dart';

AudYoFloPropertyContentBackend allocatePropertyContent(
    JvxComponentIdentification cpId,
    jvxDataFormatEnum form,
    int num,
    AudYoFloCompileFlags flags,
    int numDigits) {
  // numDigits not yet in use in web frontend.
  switch (form) {
    case jvxDataFormatEnum.JVX_DATAFORMAT_DATA:
      if (flags.compileDataTypeSpec ==
          jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE) {
        return AudYoFloPropertyMultiContentWeb<Float64List>(cpId, num, form);
      }
      return AudYoFloPropertyMultiContentWeb<Float32List>(cpId, num, form);

    // Uint64List and Int64List not supported in "web" - use int64 instead

    case jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE:
      return AudYoFloPropertyMultiContentWeb<Int16List>(cpId, num, form);
    case jvxDataFormatEnum.JVX_DATAFORMAT_8BIT:
      return AudYoFloPropertyMultiContentWeb<Int8List>(cpId, num, form);
    case jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE:
      return AudYoFloPropertyMultiContentWeb<Int32List>(cpId, num, form);
    case jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE:
      return AudYoFloPropertyMultiContentWeb<Int32List>(cpId, num, form);
    case jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE:
      return AudYoFloPropertyMultiContentWeb<Uint16List>(cpId, num, form);
    case jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT:
      return AudYoFloPropertyMultiContentWeb<Uint8List>(cpId, num, form);
    case jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE:
      return AudYoFloPropertyMultiContentWeb<Uint32List>(cpId, num, form);
    case jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE:
      return AudYoFloPropertyMultiContentWeb<Uint32List>(cpId, num, form);
    case jvxDataFormatEnum.JVX_DATAFORMAT_SIZE:
      return AudYoFloPropertyMultiContentWeb<Uint32List>(cpId, num, form);
    case jvxDataFormatEnum.JVX_DATAFORMAT_STRING_LIST:
      return AudYoFloPropertyMultiStringBackend(cpId);
    case jvxDataFormatEnum.JVX_DATAFORMAT_STRING:
      return AudYoFloPropertySingleStringBackend(cpId);
    case jvxDataFormatEnum.JVX_DATAFORMAT_SELECTION_LIST:
      return AudYoFloPropertySelectionListWeb(
          cpId, flags.compileNum32BitBitfield, num);
    case jvxDataFormatEnum.JVX_DATAFORMAT_VALUE_IN_RANGE:
      if (flags.compileDataTypeSpec ==
          jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE) {
        return AudYoFloPropertyValueInRangeWeb<Float64List>(cpId, num);
      }
      return AudYoFloPropertyValueInRangeWeb<Float32List>(cpId, num);

    //return AudYoFloPropertyVal(cpId, flags.compileNum32BitBitfield);

    default:
      break;
  }

  return AudYoFloPropertyContentBackend(cpId);
}

// ====================================================================
// ====================================================================

// Class to hold a vector/array property, template to use one code for all types
// but remain type stable
class AudYoFloPropertyMultiContentWeb<T2 extends List>
    extends AudYoFloPropertyContentBackend
    with AudYoFloPropertyMultiContentBackend<T2> {
  AudYoFloPropertyContentBackend get parpropc => this;
  AudYoFloPropertyMultiContentWeb(
      JvxComponentIdentification cpId, int sz, jvxDataFormatEnum formatArg)
      : super(cpId, format: formatArg, num: sz) {
    // In the rebuild function the fldFrmt is set according to the "real" format, not the
    // "jvxrt" format.
    rebuild(sz);
  }

  @override
  String toHelperString() {
    String txt = fldSz.toString() + ' entries of type ' + jvxFormat.toString();
    return txt;
  }

  @override
  String toString() {
    String retVal = '';
    if (fld is Float32List) {
      Float32List ptr = fld as Float32List;
      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toStringAsFixed(numDigits);
      }
    }
    if (fld is Float64List) {
      Float64List ptr = fld as Float64List;
      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toStringAsFixed(numDigits);
      }
    }
    // ================================================================
    if (fld is Int8List) {
      Int8List ptr = fld as Int8List;
      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toString();
      }
    }
    if (fld is Int16List) {
      Int16List ptr = fld as Int16List;
      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toString();
      }
    }
    if (fld is Int32List) {
      Int32List ptr = fld as Int32List;
      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toString();
      }
    }
    if (fld is Int64List) {
      Int64List ptr = fld as Int64List;
      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toString();
      }
    }
    // ----------------------------------------
    if (fld is Uint8List) {
      Uint8List ptr = fld as Uint8List;
      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toString();
      }
    }
    if (fld is Uint16List) {
      Uint16List ptr = fld as Uint16List;

      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toString();
      }
    }
    if (fld is Uint32List) {
      Uint32List ptr = fld as Uint32List;
      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toString();
      }
    } else if (fld is Uint64List) {
      Uint64List ptr = fld as Uint64List;
      for (int idx = 0; idx < fldSz; idx++) {
        if (retVal.isNotEmpty) {
          retVal += ', ';
        }
        retVal += ptr.elementAt(idx).toString();
      }
    }
    retVal = '[' + retVal + ']';
    return retVal;
  }

  void rebuild(int newSize) {
    if (fldSz != newSize) {
      if (fldSz != 0) {
        dispose();
      }
    }

    if (newSize != 0) {
      recreateCore(newSize);
    }
  }

  // Destructor. MUST BE CALLED MANUALLY ANYWAY, flutter does not
  // have destructors due to comaptibility with javascript,
  // https://github.com/dart-lang/sdk/issues/3691
  @override
  void dispose() {
    if (fld != null) {
      fld!.clear();
    }
    fldSz = 0;
    super.num = 0;
    super.dispose();
  }

  void recreateCore(int newSize) {
    fldSz = newSize;
    super.num = newSize;

    if (T2 == Float32List) {
      fld = Float32List(fldSz) as T2;
    } else if (T2 == Float64List) {
      fld = Float64List(fldSz) as T2;
    } else if (T2 == Int8List) {
      fld = Int8List(fldSz) as T2;
    } else if (T2 == Int16List) {
      fld = Int16List(fldSz) as T2;
    } else if (T2 == Int32List) {
      fld = Int32List(fldSz) as T2;
    } else if (T2 == Int64List) {
      fld = Int64List(fldSz) as T2;
    } else if (T2 == Uint8List) {
      fld = Uint8List(fldSz) as T2;
    } else if (T2 == Uint16List) {
      fld = Uint16List(fldSz) as T2;
    } else if (T2 == Uint32List) {
      fld = Uint32List(fldSz) as T2;
    } else if (T2 == Uint64List) {
      fld = Uint64List(fldSz) as T2;
    } else {
      assert(false);
    }
  }
}

// ================================================================
// ================================================================

// Class to hold a selection list which is string list + selection entries
class AudYoFloPropertySelectionListWeb
    extends AudYoFloPropertyMultiStringBackend
    with AudYoFloPropertySelectionListBackend {
  @override
  String toHelperString() {
    if (selection is AudYoFloBitFieldSimple) {
      AudYoFloBitFieldSimple sel = selection as AudYoFloBitFieldSimple;
      return sel.numEntries.toString() +
          ' entries among options ' +
          super.toHelperString();
    } else {
      return '<invalid-type';
    }
  }

  @override
  AudYoFloPropertyMultiStringBackend get parpropms => this;

  @override
  String toString() {
    int idx = 0;
    String out = '';

    // Loop over the number of options. Typically, there is one but there might be more!
    for (; idx < super.num; idx++) {
      // in the current option..
      int posi = 0;
      String outOne = '';
      for (var elmS in entries) {
        // Always use offset "0" since we shift the pointer manually (line + 15)
        if (selection.bitTest(posi, offset: idx)) {
          if (outOne.isEmpty) {
            outOne = elmS;
          } else {
            outOne = outOne + ', ' + elmS;
          }
        }
        posi++;
      }
      if (out.isEmpty) {
        out = outOne;
      } else {
        out = out + '; ' + outOne;
      }
    }
    out = '[' + out + ']';
    return out;
  }

  // ===============================================================
  AudYoFloPropertySelectionListWeb(
      JvxComponentIdentification cpId, int compileNum32BitBitfieldArg, int num)
      : super(cpId, format: jvxDataFormatEnum.JVX_DATAFORMAT_SELECTION_LIST) {
    compileNum32BitBitfield = compileNum32BitBitfieldArg;
    resize(num, false, false);
    // selection.initialize(compileNum32BitBitfield);
    selectable.initialize(compileNum32BitBitfield, numEntriesArg: 1);
    exclusive.initialize(compileNum32BitBitfield, numEntriesArg: 1);
  }

  void resize(int numArg, bool copyOld, bool forceNew) {
    // In this case the selection list deviates from the string list:
    // There might be a 0-length selection field
    super.num = numArg;
    if (selection is AudYoFloBitFieldBackend) {
      AudYoFloBitFieldSimple sel = selection as AudYoFloBitFieldSimple;
      if (super.num > 0) {
        if (forceNew || (num != sel.numEntries)) {
          AudYoFloBitField newSelection = AudYoFloBitFieldSimple();
          assert(numArg > 0);
          newSelection.initialize(compileNum32BitBitfield,
              numEntriesArg: numArg);
          int idx = 0;
          if (copyOld) {
            int numCpy = min(numArg, sel.numEntries);
            for (; idx < numCpy; idx++) {
              newSelection.copy(selection, offset: idx);
            }
          }
          selection.dispose();
          selection = newSelection;
        }
      }
    } else {
      assert(false);
    }
  }
}

// ========================================================================

class AudYoFloPropertyValueInRangeWeb<T2 extends List>
    extends AudYoFloPropertyMultiContentWeb<T2>
    with AudYoFloPropertyValueInRangeBackend {
  AudYoFloPropertyValueInRangeWeb(JvxComponentIdentification cpId, int sz)
      : super(cpId, sz, jvxDataFormatEnum.JVX_DATAFORMAT_VALUE_IN_RANGE);

  @override
  AudYoFloPropertyMultiContentBackend get parpropmc => this;
}
