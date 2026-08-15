import 'package:ayf_pack/ayf_pack.dart';

class AudYoFloConnectorlistFromJson {
  static int updateConnectorList(
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
      var subSec = AudYoFloHelper.getMapValueList(jsonMap, 'connectors');
      if (subSec != null) {
        List<JvxConnector> inputConnectors = [];
        List<JvxConnector> outputConnectors = [];
        for (var elmC in subSec) {
          JvxConnector newCon = JvxConnector();

          bool isInput = true;
          String? tmp =
              AudYoFloHelper.extractStringFromJson(elmC, 'direction');
          if (tmp != null) {
            isInput = (tmp == 'input');
            tmp = null;
          }
          newCon.isInput = isInput;

          tmp = AudYoFloHelper.extractStringFromJson(elmC, 'descriptor');
          if (tmp != null) {
            newCon.descriptor = tmp;
            tmp = null;
          }

          var params = AudYoFloHelper.getMapValueMap(elmC, 'params');
          if (params != null) {
            newCon.buffersize =
                AudYoFloHelper.getIntEntryValueMap(params, 'buffersize');
            newCon.rate = AudYoFloHelper.getIntEntryValueMap(params, 'rate');
            newCon.numberChannels =
                AudYoFloHelper.getIntEntryValueMap(params, 'number_channels');

            tmp = AudYoFloHelper.extractStringFromJson(params, 'format');
            if (tmp != null) {
              newCon.format = jvxDataFormatEEnum.fromInt(
                  AudYoFloStringTranslator.translateEnumString(
                      tmp, 'jvxDataFormat', trans.compileFlags()));
              tmp = null;
            }

            tmp =
                AudYoFloHelper.extractStringFromJson(params, 'format_group');
            if (tmp != null) {
              newCon.formatGroup = jvxDataFormatGroupEEnum.fromInt(
                  AudYoFloStringTranslator.translateEnumString(
                      tmp, 'jvxDataFormatGroup', trans.compileFlags()));
              tmp = null;
            }

            tmp = AudYoFloHelper.extractStringFromJson(params, 'dataflow');
            if (tmp != null) {
              newCon.dataFlow = jvxDataflowEEnum.fromInt(
                  AudYoFloStringTranslator.translateEnumString(
                      tmp, 'jvxDataflow', trans.compileFlags()));
              tmp = null;
            }
          }

          if (isInput) {
            inputConnectors.add(newCon);
          } else {
            outputConnectors.add(newCon);
          }
        }
        // Update the cache and notify all listeners!!
        theBeCache.updateConnectorsCacheCompleteNotify(
            cpId, inputConnectors, outputConnectors);
      }
    }
    return retVal;
  }
}
