import 'package:fixnum/fixnum.dart' as fn;
import '../../ayf_pack_local.dart';

abstract class AudYoFloCBitFieldT<T extends fn.IntX>
    extends AudYoFloCBitFieldBase {
  T fld;
  int szChar;
  AudYoFloCBitFieldT(this.fld, this.szChar);

  // ==================================================
  // Abstract helpers to be implemented in subclass
  T shiftl(T old, int num);

  T ori(T old, int val);
  T or(T old, T val);

  T and(T old, T val);
  T reset();
  T one();
  T neg(T val);
  // ==================================================

  // Some re-defined base class functions
  void bitFClear() {
    fld = reset();
  }

  @override
  void bitClear(int posi) {
    T andwith = one();
    andwith = shiftl(andwith, posi);
    andwith = neg(andwith);
    fld = and(fld, andwith);
  }

  @override
  void bitZSet(int posi) {
    fld = one();
    fld = shiftl(fld, posi);
  }

  @override
  void bitSet(int posi) {
    T orwith = one();
    orwith = shiftl(orwith, posi);
    fld = or(fld, orwith);
  }

  @override
  bool bitTest(int posi) {
    T comp = shiftl(one(), posi);
    T val = and(fld, comp);
    return val != 0;
  }

  // Sophisticated functions
  int fromHexString(String inStr) {
    fld = reset();
    List<String> lstStr = inStr.split('0x');
    if (lstStr.length == 2) {
      if (lstStr[0].isEmpty) {
        String str1 = lstStr[1];

        int sz = str1.length;
        if (sz > szChar) {
          return jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
        }

        while (sz > 0) {
          String token = str1;
          if (sz > 8) {
            token = str1.substring(str1.length - 8, str1.length);
            str1 = str1.substring(0, str1.length - 8);
          } else {
            token = str1;
            str1 = '';
          }
          fld = shiftl(fld, 32);
          int val = int.parse(token, radix: 16);
          fld = ori(fld, val);
          sz = str1.length;
        }
        return jvxErrorType.JVX_NO_ERROR;
      }
    }
    return jvxErrorType.JVX_ERROR_PARSE_ERROR;
  }
}

class AudYoFloCBitField64 extends AudYoFloCBitFieldT<fn.Int64> {
  // Some
  @override
  fn.Int64 reset() => fn.Int64(0);

  @override
  fn.Int64 one() => fn.Int64(1);

  @override
  fn.Int64 shiftl(fn.Int64 val, int posi) {
    return val << posi;
  }

  @override
  fn.Int64 ori(fn.Int64 val, int vali) {
    val |= fn.Int64(vali);
    return val;
  }

  @override
  fn.Int64 and(fn.Int64 val, fn.Int64 vali) {
    val &= vali;
    return val;
  }

  @override
  fn.Int64 or(fn.Int64 val, fn.Int64 vali) {
    val |= vali;
    return val;
  }

  @override
  fn.Int64 neg(fn.Int64 val) {
    val = ~val;
    return val;
  }

  AudYoFloCBitField64() : super(fn.Int64(0), 16);
}

class AudYoFloCBitField32 extends AudYoFloCBitFieldT<fn.Int32> {
  @override
  fn.Int32 reset() => fn.Int32(0);

  @override
  fn.Int32 one() => fn.Int32(1);

  @override
  fn.Int32 shiftl(fn.Int32 val, int posi) {
    return val << posi;
  }

  @override
  fn.Int32 ori(fn.Int32 val, int vali) {
    val |= fn.Int64(vali);
    return val;
  }

  @override
  fn.Int32 and(fn.Int32 val, fn.Int32 vali) {
    val &= vali;
    return val;
  }

  @override
  fn.Int32 or(fn.Int32 val, fn.Int32 vali) {
    val |= vali;
    return val;
  }

  @override
  fn.Int32 neg(fn.Int32 val) {
    val = ~val;
    return val;
  }

  AudYoFloCBitField32() : super(fn.Int32(0), 8);
}

typedef AudYoFloCBitField = AudYoFloCBitField64;
