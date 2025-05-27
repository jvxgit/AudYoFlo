import 'package:fixnum/fixnum.dart' as fn;
import 'package:ayf_pack/ayf_pack.dart';

class AudYoFloDevicelistFromJson {
  static int updateDeviceList(
      Map<dynamic, dynamic> jsonMap,
      JvxComponentIdentification cpId,
      AudYoFloBackendCacheBectrlIf theBeCache,
      AudYoFloBackendTranslator trans) {
    int retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    String? errCodeExprPtr =
        AudYoFloHelper.getStringEntryValueMap(jsonMap, 'return_code');
    if (errCodeExprPtr != null) {
      // Convert error code
      String errCodeExpr = errCodeExprPtr;
      retVal = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
    }
    if (retVal == jvxErrorType.JVX_NO_ERROR) {
      var subSec = AudYoFloHelper.getMapValueList(jsonMap, 'devices');
      if (subSec != null) {
        List<AudYoFloOneDeviceSelectionOption> devices = [];
        int idx = 0;
        for (var elmD in subSec) {
          AudYoFloOneDeviceSelectionOption newDevice =
              AudYoFloOneDeviceSelectionOption();

          newDevice.optionIdx = idx;
          idx++;

          String? tmp =
              AudYoFloHelper.extractStringFromJson(elmD, 'description');
          if (tmp != null) {
            newDevice.description = tmp;
            tmp = null;
          }

          tmp = AudYoFloHelper.extractStringFromJson(elmD, 'descriptor');
          if (tmp != null) {
            newDevice.descriptor = tmp;
            tmp = null;
          }

          newDevice.devIdent.cpTp =
              JvxComponentTypeEEnum.fromInt(cpId.cpTp.index + 1);
          newDevice.devIdent.slotid = cpId.slotid;
          newDevice.devIdent.slotsubid = -1;

          tmp = AudYoFloHelper.extractStringFromJson(elmD, 'state');
          if (tmp != null) {
            newDevice.state.fld = fn.Int32(
                AudYoFloStringTranslator.translateEnumString(
                    tmp, 'jvxState', trans.compileFlags()));
            tmp = null;
          }

          var subSubSec = AudYoFloHelper.getMapValueMap(elmD, 'device_caps');
          if (subSubSec != null) {
            newDevice.selectable = false;
            tmp = AudYoFloHelper.extractStringFromJson(subSubSec, 'sel');
            if (tmp != null) {
              if (tmp == '*') {
                newDevice.selectable = true;
              }
              tmp = null;
            }

            newDevice.proxy = false;
            tmp = AudYoFloHelper.extractStringFromJson(subSubSec, 'proxy');
            if (tmp != null) {
              if (tmp == '*') {
                newDevice.proxy = true;
              }
              tmp = null;
            }

            newDevice.flow =
                jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_UNKNOWN;
            tmp = AudYoFloHelper.extractStringFromJson(subSubSec, 'flow_type');
            if (tmp != null) {
              newDevice.flow = jvxDeviceDataFlowTypeEEnum.fromInt(
                  AudYoFloStringTranslator.translateEnumString(
                      tmp, 'jvxDeviceDataFlowType', trans.compileFlags()));
              tmp = null;
            }

            newDevice.caps.fld = fn.Int32(0);
            tmp = AudYoFloHelper.extractStringFromJson(subSubSec, 'caps');
            if (tmp != null) {
              newDevice.caps.fld = fn.Int32(
                  AudYoFloStringTranslator.translateEnumString(
                      tmp, 'jvxDeviceCapabilityType', trans.compileFlags()));
              tmp = null;
            }

            tmp = AudYoFloHelper.extractStringFromJson(subSubSec, 'flags');
            if (tmp != null) {
              resultParseInt32 res = resultParseInt32();
              newDevice.flags.fld =
                  fn.Int32(AudYoFloHelper.hexStringToFnInt32(res, tmp));
              tmp = null;
            }

            var elmSSId = AudYoFloHelper.getMapValueList(elmD, 'slots');
            if (elmSSId != null) {
              if (elmSSId.isNotEmpty) {
                if (elmSSId[0] is int) {
                  // I can not remember any case in which a device is in differnt slots..
                  // Need to keep this in mind!!
                  newDevice.devIdent.slotsubid = elmSSId[0];
                }
              }
            }
          }
          devices.add(newDevice);
        }
        // Update the list and notify all!!
        theBeCache.updateDeviceListCacheCompleteNotify(cpId, devices);
      }
    }
    return retVal;
  }
}
