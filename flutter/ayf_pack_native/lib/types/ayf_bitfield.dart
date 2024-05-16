import 'dart:math';
import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:fixnum/fixnum.dart' as fn;
import 'package:ayf_pack/ayf_pack.dart';

// ===========================================================================
class AudYoFloBitFieldNative extends AudYoFloBitFieldBackend {
  Pointer<Uint32> raw = nullptr;
  AudYoFloBitFieldNative();

  @override
  void initialize(int num32BitBitfieldArg, {int numEntriesArg = 1}) {
    compileNum32BitBitfield = num32BitBitfieldArg;
    numEntries = numEntriesArg;
    num32BitAlltogether = numEntriesArg * compileNum32BitBitfield;
    raw = calloc<Uint32>(num32BitAlltogether);
    bitfield = raw.asTypedList(num32BitAlltogether);
  }

  @override
  bool bitTest(int posi, {int offset = 0}) {
    int idxEntry = posi >> 5;
    int inWordIdx = posi - (idxEntry << 5);
    int inFldIdx = compileNum32BitBitfield - 1 - idxEntry;
    inFldIdx += offset * compileNum32BitBitfield;
    if ((bitfield![inFldIdx] & (1 << inWordIdx)) != 0) {
      return true;
    }
    return false;
  }

  @override
  int firstSet(int posiMax, {int offset = 0}) {
    int firstSetPosi = -1;
    posiMax = min(posiMax, compileNum32BitBitfield);
    for (int posi = 0; posi < posiMax; posi++) {
      int idxEntry = posi >> 5;
      int inWordIdx = posi - (idxEntry << 5);
      int inFldIdx = compileNum32BitBitfield - 1 - idxEntry;
      inFldIdx += offset * compileNum32BitBitfield;
      if ((bitfield![inFldIdx] & (1 << inWordIdx)) != 0) {
        firstSetPosi = posi;
        break;
      }
    }
    return firstSetPosi;
  }

  @override
  void bitSet(int posi, {int offset = 0}) {
    int idxEntry = posi >> 5;
    int inWordIdx = posi - (idxEntry << 5);
    int inFldIdx = compileNum32BitBitfield - 1 - idxEntry;
    inFldIdx += offset * compileNum32BitBitfield;
    bitfield![inFldIdx] |= (1 << inWordIdx);
  }

  @override
  void bitZSet(int posi, {int offset = 0}) {
    int idxEntry = posi >> 5;
    int inWordIdx = posi - (idxEntry << 5);
    int inFldIdx = compileNum32BitBitfield - 1 - idxEntry;
    inFldIdx += offset * compileNum32BitBitfield;
    if (inFldIdx > 0) {
      bitfield![inFldIdx] = (1 << inWordIdx);
    }
  }

  @override
  void bitClear(int posi, {int offset = 0}) {
    int idxEntry = posi >> 5;
    int inWordIdx = posi - (idxEntry << 5);
    int inFldIdx = compileNum32BitBitfield - 1 - idxEntry;
    inFldIdx += offset * compileNum32BitBitfield;
    if (inFldIdx > 0) {
      bitfield![inFldIdx] &= ~(1 << inWordIdx);
    }
  }

  @override
  void bitFClear({int offset = 0}) {
    for (int idx = 0; idx < compileNum32BitBitfield; idx++) {
      int inFldIdx = compileNum32BitBitfield - 1 - idx;
      inFldIdx += offset * compileNum32BitBitfield;
      if (inFldIdx > 0) {
        bitfield![inFldIdx] = 0;
      }
    }
  }

  @override
  void copy(AudYoFloBitField copyFrom, {int offset = 0}) {
    assert(offset < numEntries);
    if (copyFrom is AudYoFloBitFieldNative) {
      assert(offset < copyFrom.numEntries);
      for (int idx = 0; idx < compileNum32BitBitfield; idx++) {
        int inFldIdx = compileNum32BitBitfield - 1 - idx;
        inFldIdx += offset * compileNum32BitBitfield;
        bitfield![inFldIdx] = copyFrom.bitfield![inFldIdx];
      }
    }
  }

  @override
  void dispose() {
    if (raw != nullptr) {
      calloc.free(raw);
    }
    raw = nullptr;
    compileNum32BitBitfield = 0;
    numEntries = 0;
    num32BitAlltogether = 0;
    bitfield = null;
  }
}

AudYoFloBitField allocateAudYoFloBitfield() {
  return AudYoFloBitFieldNative();
}
