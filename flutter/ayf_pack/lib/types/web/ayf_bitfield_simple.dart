import 'dart:math';
import 'dart:typed_data';
import 'package:fixnum/fixnum.dart' as fn;
import '../../ayf_pack_local.dart';

class AudYoFloBitFieldSimple extends AudYoFloBitFieldBackend {
  int num32BitBitfield = 0;
  AudYoFloBitFieldSimple();

  @override
  void initialize(int num32BitBitfieldArg, {int numEntriesArg = 1}) {
    num32BitBitfield = num32BitBitfieldArg;
    num32BitAlltogether = numEntriesArg * num32BitBitfield;
    numEntries = numEntriesArg;
    bitfield = Uint32List(num32BitAlltogether);
  }

  @override
  bool bitTest(int posi, {int offset = 0}) {
    int idxEntry = posi >> 5;
    int inWordIdx = posi - (idxEntry << 5);
    int inFldIdx = num32BitBitfield - 1 - idxEntry;
    inFldIdx += offset * num32BitBitfield;
    if ((bitfield![inFldIdx] & (1 << inWordIdx)) != 0) {
      return true;
    }
    return false;
  }

  @override
  int firstSet(int posiMax, {int offset = 0}) {
    int firstSetPosi = -1;
    posiMax = min(posiMax, num32BitBitfield);
    for (int posi = 0; posi < posiMax; posi++) {
      int idxEntry = posi >> 5;
      int inWordIdx = posi - (idxEntry << 5);
      int inFldIdx = num32BitBitfield - 1 - idxEntry;
      inFldIdx += offset * num32BitBitfield;
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
    int inFldIdx = num32BitBitfield - 1 - idxEntry;
    inFldIdx += offset * num32BitBitfield;
    bitfield![inFldIdx] |= (1 << inWordIdx);
  }

  @override
  void bitZSet(int posi, {int offset = 0}) {
    int idxEntry = posi >> 5;
    int inWordIdx = posi - (idxEntry << 5);
    int inFldIdx = num32BitBitfield - 1 - idxEntry;
    inFldIdx += offset * num32BitBitfield;
    bitfield![inFldIdx] = fn.Int32((1 << inWordIdx)) as int;
  }

  @override
  void bitClear(int posi, {int offset = 0}) {
    int idxEntry = posi >> 5;
    int inWordIdx = posi - (idxEntry << 5);
    int inFldIdx = num32BitBitfield - 1 - idxEntry;
    inFldIdx += offset * num32BitBitfield;
    bitfield![inFldIdx] &= ~(1 << inWordIdx);
  }

  @override
  void bitFClear({int offset = 0}) {
    for (int idx = 0; idx < num32BitBitfield; idx++) {
      int inFldIdx = num32BitBitfield - 1 - idx;
      inFldIdx += offset * num32BitBitfield;
      bitfield![inFldIdx] = fn.Int32(0) as int;
    }
  }

  @override
  void copy(AudYoFloBitField copyFrom, {int offset = 0}) {
    assert(offset < numEntries);
    if (copyFrom is AudYoFloBitFieldSimple) {
      assert(offset < copyFrom.numEntries);
      for (int idx = 0; idx < num32BitBitfield; idx++) {
        int inFldIdx = num32BitBitfield - 1 - idx;
        inFldIdx += offset * num32BitBitfield;
        bitfield![inFldIdx] = copyFrom.bitfield![inFldIdx];
      }
    }
  }

  @override
  void dispose() {
    num32BitBitfield = 0;
    numEntries = 0;
    num32BitAlltogether = 0;
    bitfield?.clear();
  }
}

AudYoFloBitField allocateAudYoFloBitfield() {
  return AudYoFloBitFieldSimple();
}
