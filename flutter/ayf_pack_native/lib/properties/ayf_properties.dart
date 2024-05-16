import 'package:ffi/ffi.dart';
import 'dart:ffi';
import 'dart:typed_data';
import 'package:fixnum/fixnum.dart' as fn;
import 'dart:math';

import '../ayf_pack_native.dart';
import 'package:ayf_pack/ayf_pack.dart';

AudYoFloPropertyContentBackend allocatePropertyContent(
    JvxComponentIdentification cpId,
    jvxDataFormatEnum form,
    int num,
    AudYoFloCompileFlags flags, 
    int numDigits) {
  switch (form) {
    case jvxDataFormatEnum.JVX_DATAFORMAT_DATA:
      if (flags.compileDataTypeSpec ==
          jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE) {
        return AudYoFloPropertyMultiContentNative<Double, Float64List>(
            cpId, num, form, numDigits);
      }
      return AudYoFloPropertyMultiContentNative<Float, Float32List>(
          cpId, num, form, numDigits);

    case jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE:
      return AudYoFloPropertyMultiContentNative<Int16, Int16List>(
          cpId, num, form, numDigits);
    case jvxDataFormatEnum.JVX_DATAFORMAT_8BIT:
      return AudYoFloPropertyMultiContentNative<Int8, Int8List>(
          cpId, num, form, numDigits);
    case jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE:
      return AudYoFloPropertyMultiContentNative<Int32, Int32List>(
          cpId, num, form, numDigits);
    case jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE:
      return AudYoFloPropertyMultiContentNative<Int64, Int64List>(
          cpId, num, form, numDigits);
    case jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE:
      return AudYoFloPropertyMultiContentNative<Uint16, Uint16List>(
          cpId, num, form, numDigits);
    case jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT:
      return AudYoFloPropertyMultiContentNative<Uint8, Uint8List>(
          cpId, num, form, numDigits);
    case jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE:
      return AudYoFloPropertyMultiContentNative<Uint32, Uint32List>(
          cpId, num, form, numDigits);
    case jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE:
      return AudYoFloPropertyMultiContentNative<Uint64, Uint64List>(
          cpId, num, form, numDigits);
    case jvxDataFormatEnum.JVX_DATAFORMAT_SIZE:
      return AudYoFloPropertyMultiContentNative<Uint64, Uint64List>(
          cpId, num, form, numDigits);
    case jvxDataFormatEnum.JVX_DATAFORMAT_STRING_LIST:
      return AudYoFloPropertyMultiStringBackend(cpId);
    case jvxDataFormatEnum.JVX_DATAFORMAT_STRING:
      return AudYoFloPropertySingleStringBackend(cpId);
    case jvxDataFormatEnum.JVX_DATAFORMAT_SELECTION_LIST:
      return AudYoFloPropertySelectionListNative(
          cpId, flags.compileNum32BitBitfield, num);
    case jvxDataFormatEnum.JVX_DATAFORMAT_VALUE_IN_RANGE:
      if (flags.compileDataTypeSpec ==
          jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE) {
        return AudYoFloPropertyValueInRangeNative<Double, Float64List>(
            cpId, num, numDigits);
      }
      return AudYoFloPropertyValueInRangeNative<Float, Float32List>(cpId, num, numDigits);

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
class AudYoFloPropertyMultiContentNative<T1 extends NativeType, T2>
    extends AudYoFloPropertyContentBackend
    with AudYoFloPropertyMultiContentBackend<T2> {
  jvxDataFormatEnum fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_NONE;
  Pointer<T1> fldPtr = nullptr;
  int szNativeElement = 0;

  AudYoFloPropertyContentBackend get parpropc => this;
  AudYoFloPropertyMultiContentNative(
      JvxComponentIdentification cpId, int sz, jvxDataFormatEnum formatArg, int numDigShow )
      : super(cpId, format: formatArg, num: sz) {
    
    numDigits = numDigShow;
    // Set the default format
    if (formatArg != jvxDataFormatEnum.JVX_DATAFORMAT_NONE) {
      fldFrmt = formatArg;
    }

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
    if (fldPtr != nullptr) {
      calloc.free(fldPtr);
    }
    fldSz = 0;
    super.num = 0;
    fldPtr = nullptr;
    super.dispose();
  }

  void recreateCore(int newSize) {
    fldSz = newSize;
    super.num = newSize;
    if (T1 == Double) {
      Pointer<Double> fldPtrLoc = calloc<Double>(fldSz).cast<Double>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_DOUBLE;
      szNativeElement = sizeOf<Double>();
    } else if (T1 == Float) {
      Pointer<Float> fldPtrLoc = calloc<Float>(fldSz).cast<Float>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_FLOAT;
      szNativeElement = sizeOf<Float>();
    } else if (T1 == Int64) {
      Pointer<Int64> fldPtrLoc = calloc<Int64>(fldSz).cast<Int64>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE;
      szNativeElement = sizeOf<Int64>();
    } else if (T1 == Int32) {
      Pointer<Int32> fldPtrLoc = calloc<Int32>(fldSz).cast<Int32>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE;
      szNativeElement = sizeOf<Int32>();
    } else if (T1 == Int16) {
      Pointer<Int16> fldPtrLoc = calloc<Int16>(fldSz).cast<Int16>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE;
      szNativeElement = sizeOf<Int16>();
    } else if (T1 == Int8) {
      Pointer<Int8> fldPtrLoc = calloc<Int8>(fldSz).cast<Int8>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_8BIT;
      szNativeElement = sizeOf<Int8>();
    } else if (T1 == Uint64) {
      Pointer<Uint64> fldPtrLoc = calloc<Uint64>(fldSz).cast<Uint64>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE;
      szNativeElement = sizeOf<Uint64>();
    } else if (T1 == Uint32) {
      Pointer<Uint32> fldPtrLoc = calloc<Uint32>(fldSz).cast<Uint32>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE;
      szNativeElement = sizeOf<Uint32>();
    } else if (T1 == Uint16) {
      Pointer<Uint16> fldPtrLoc = calloc<Uint16>(fldSz).cast<Uint16>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE;
      szNativeElement = sizeOf<Uint16>();
    } else if (T1 == Uint8) {
      Pointer<Uint8> fldPtrLoc = calloc<Uint8>(fldSz).cast<Uint8>();
      fldPtr = fldPtrLoc as Pointer<T1>;
      fld = fldPtrLoc.asTypedList(fldSz) as T2;
      fldFrmt = jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT;
      szNativeElement = sizeOf<Uint8>();
    } else {
      assert(false);
    }
  }
}

// ================================================================
// ================================================================

// Class to hold a selection list which is string list + selection entries
class AudYoFloPropertySelectionListNative
    extends AudYoFloPropertyMultiStringBackend
    with AudYoFloPropertySelectionListBackend {
  @override
  String toHelperString() {
    if (selection is AudYoFloBitFieldNative) {
      AudYoFloBitFieldNative sel = selection as AudYoFloBitFieldNative;
      return sel.numEntries.toString() +
          ' entries among options ' +
          super.toHelperString();
    } else {
      return '<invalid-type';
    }
  }

  @override
  AudYoFloPropertyMultiStringBackend get parpropms => this;

  // Show selection of ALL options. Typically, there is only one but there
  // may be more than just one. The selected options are spaced by ",",
  // the options are spaced by ";"
  @override
  String toString() {
    int idx = 0;
    String out = '';
    if (selection is AudYoFloBitFieldNative) {
      AudYoFloBitFieldNative sel = selection as AudYoFloBitFieldNative;

      int ptr = sel.raw.address;
      for (; idx < super.num; idx++) {
        Pointer<Uint32> ptrFld = Pointer<Uint32>.fromAddress(ptr);
        Uint32List bitfield = ptrFld.asTypedList(sel.compileNum32BitBitfield);
        AudYoFloBitFieldNative selectionIdx = AudYoFloBitFieldNative();
        selectionIdx.bitfield = bitfield;
        selectionIdx.compileNum32BitBitfield = sel.compileNum32BitBitfield;

        int posi = 0;
        String outOne = '';
        for (var elmS in entries) {
          // Always use offset "0" since we shift the pointer manually (line + 15)
          if (selectionIdx.bitTest(posi, offset: 0)) {
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
        ptr = ptr + sel.compileNum32BitBitfield * sizeOf<Uint32>();
      }
      out = '[' + out + ']';
    } else {
      out = '[<invalid-type>]';
    }
    return out;
  }

  // ===============================================================
  AudYoFloPropertySelectionListNative(
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
    if (selection is AudYoFloBitFieldNative) {
      AudYoFloBitFieldNative sel = selection as AudYoFloBitFieldNative;
      if (super.num > 0) {
        if (forceNew || (num != sel.numEntries)) {
          AudYoFloBitField newSelection = AudYoFloBitFieldNative();
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

class AudYoFloPropertyValueInRangeNative<T1 extends NativeType, T2>
    extends AudYoFloPropertyMultiContentNative<T1, T2>
    with AudYoFloPropertyValueInRangeBackend {
  AudYoFloPropertyValueInRangeNative(JvxComponentIdentification cpId, int sz, int numDigShow)
      : super(cpId, sz, jvxDataFormatEnum.JVX_DATAFORMAT_VALUE_IN_RANGE, numDigShow);

  @override
  AudYoFloPropertyMultiContentBackend get parpropmc => this;
}
/*
 * Simplified properties for local variables
 */
