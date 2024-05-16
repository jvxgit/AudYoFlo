import 'dart:typed_data';

abstract class AudYoFloCBitFieldBase {
  void bitZSet(int posi);
  void bitSet(int posi);
  bool bitTest(int posi);
  void bitClear(int posi);
  void bitFClear();
}

abstract class AudYoFloBitField {
  void initialize(int num32BitBitfieldArg, {int numEntriesArg = 1});
  bool bitTest(int posi, {int offset = 0});
  int firstSet(int posiMax, {int offset = 0});
  void bitSet(int posi, {int offset = 0});
  void bitZSet(int posi, {int offset = 0});
  void bitClear(int posi, {int offset = 0});
  void bitFClear({int offset = 0});
  void copy(AudYoFloBitField copyFrom, {int offset = 0});
  void dispose();
}

abstract class AudYoFloBitFieldBackend extends AudYoFloBitField {
  int compileNum32BitBitfield = 0;
  int num32BitAlltogether = 0;
  Uint32List? bitfield;
  int numEntries = 0;
}
