import 'package:fixnum/fixnum.dart' as fn;
import 'package:ayf_pack/ayf_pack.dart';

class AudYoFloSequencerStatusFromJson {
  static int updateSequencerStatus(
      Map<dynamic, dynamic> jsonMap, AudYoFloBackendCacheBectrlIf theBeCache) {
    int retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    String? errCodeExprPtr = getStringEntryValueMap(jsonMap, 'return_code');
    if (errCodeExprPtr != null) {
      // Convert error code
      String errCodeExpr = errCodeExprPtr;
      retVal = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
    }

    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      var readyForStartPtr = extractStringFromJson(jsonMap, "ready_for_start");
      var reasonIfNotStr = extractStringFromJson(jsonMap, "reason_if_not");
      if ((readyForStartPtr != null) && (reasonIfNotStr != null)) {
        String readyForStartExpr = readyForStartPtr;
        int readyForStart =
            jvxErrorTypeEInt.fromStringSingle(readyForStartExpr);
        theBeCache.updateProcessStatusCompleteNotify(
            (readyForStart == jvxErrorType.JVX_NO_ERROR), reasonIfNotStr);
      }
    }

    return retVal;
  }
}
