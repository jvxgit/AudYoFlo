import 'dart:math';
import 'package:fixnum/fixnum.dart' as fn;
import 'package:collection/collection.dart';
import 'package:flutter/foundation.dart';
import '../ayf_pack_local.dart';
import 'dart:typed_data';

int hexStringToFnInt32(resultParseInt32 result, String inStr) {
  fn.Int32 ret = fn.Int32(0);
  List<String> lstStr = inStr.split('0x');
  if (lstStr.length == 2) {
    if (lstStr[0].isEmpty) {
      String str1 = lstStr[1];

      int sz = str1.length;
      while (sz > 0) {
        String token = str1;
        if (sz > 8) {
          token = str1.substring(str1.length - 8, str1.length);
          str1 = str1.substring(0, str1.length - 8);
        } else {
          token = str1;
          str1 = '';
        }
        ret = ret << 32;
        int val = int.parse(token, radix: 16);
        ret |= fn.Int64(val);
        sz = str1.length;
      }
      result.val = ret;
      return jvxErrorType.JVX_NO_ERROR;
    }
  }
  result.error = true;
  return jvxErrorType.JVX_ERROR_PARSE_ERROR;
}

class resultParseInt64 {
  fn.Int64 val = fn.Int64(0);
  bool error = false;
}

class resultParseInt32 {
  fn.Int32 val = fn.Int32(0);
  bool error = false;
}

String? getStringEntryValueMap(Map valueMap, String nm) {
  String? retVal;
  MapEntry? entry =
      valueMap.entries.firstWhereOrNull((element) => element.key == nm);
  if (entry != null) {
    if (entry.value is String) {
      retVal = entry.value;
    }
  }
  return retVal;
}

int getIntEntryValueMap(Map valueMap, String nm) {
  int retVal = -1;
  MapEntry? entry =
      valueMap.entries.firstWhereOrNull((element) => element.key == nm);
  if (entry != null) {
    if (entry.value is int) {
      retVal = entry.value as int;
    } else if (entry.value is double) {
      double val = entry.value as double;
      retVal = val.toInt();
    }
  }
  return retVal;
}

List? getListValueMap(Map valueMap, String nm) {
  List? retVal;
  MapEntry? entry =
      valueMap.entries.firstWhereOrNull((element) => element.key == nm);
  if (entry != null) {
    if (entry.value is List) {
      retVal = entry.value;
    }
  }
  return retVal;
}

Map? getMapValueMap(Map valueMap, String nm) {
  Map? retVal;
  MapEntry? entry =
      valueMap.entries.firstWhereOrNull((element) => element.key == nm);
  if (entry != null) {
    if (entry.value is Map) {
      retVal = entry.value;
    }
  }
  return retVal;
}

List? getMapValueList(Map valueMap, String nm) {
  List? retVal;
  MapEntry? entry =
      valueMap.entries.firstWhereOrNull((element) => element.key == nm);
  if (entry != null) {
    if (entry.value is List) {
      retVal = entry.value;
    }
  }
  return retVal;
}

List<String> str2PropList(String ident) {
  List<String> onRet = [];
  List<String> parts = ident.split('[');
  if (parts.length == 2) {
    parts = parts[1].split(']');
    if (parts.length == 2) {
      onRet = parts[0].split(',');
    }
  } else {
    onRet.add(parts[0]);
  }

  return onRet;
}

String intValueList2String(List<int> lst) {
  String txt = '';
  for (int val in lst) {
    if (!txt.isEmpty) {
      txt += ', ';
    }
    txt += val.toString();
  }
  txt = '[' + txt + ']';
  return txt;
}

void applyChangeSelectionList(
    AudYoFloBitField selection, int posi, jvxPropertyDecoderHintTypeEnum decTp,
    {int offset = 0}) {
  switch (decTp) {
    case jvxPropertyDecoderHintTypeEnum.JVX_PROPERTY_DECODER_MULTI_SELECTION:
    case jvxPropertyDecoderHintTypeEnum
        .JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:
      if (selection.bitTest(posi, offset: offset)) {
        selection.bitClear(posi, offset: offset);
      } else {
        selection.bitSet(posi, offset: offset);
      }
      break;
    default:
      selection.bitZSet(posi, offset: offset);
  }
}

void incrementSelectionList(AudYoFloBitField selection, int numOptions,
    jvxPropertyDecoderHintTypeEnum decTp,
    {int offset = 0}) {
  int curPosition = selection.firstSet(numOptions, offset: offset);
  if (curPosition < 0) {
    curPosition = 0;
  } else {
    curPosition++;
    if (curPosition == numOptions) {
      curPosition = 0;
    }
  }
  selection.bitZSet(curPosition, offset: offset);
}

// =======================================================================

enum ayfChannelTypeEnum {
  JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE,
  JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE,
  JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE,
  JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL,
  JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY
}

class AudYoFloChannelTypeClass {
  ayfChannelTypeEnum classtype =
      ayfChannelTypeEnum.JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE;
  String name = 'Unknwon channel name';
  int idHwList = -1;

  void reset(int id) {
    classtype = ayfChannelTypeEnum.JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE;
    name = 'Unknown channel name';
    idHwList = id;
  }
}

void decodeInputOutputChannel(
    String description, AudYoFloChannelTypeClass clsOnReturn, int idLst) {
  clsOnReturn.reset(idLst);
  var tokens = description.split('::');
  clsOnReturn.name = description;

  if (tokens.length == 3) {
    // Channel name in correct format!
    switch (tokens[0]) {
      case 'hw':
        clsOnReturn.classtype =
            ayfChannelTypeEnum.JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE;
        break;
      case 'f':
        clsOnReturn.classtype =
            ayfChannelTypeEnum.JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE;
        break;
      case 'e':
        clsOnReturn.classtype =
            ayfChannelTypeEnum.JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL;
        break;
      case 'd':
        clsOnReturn.classtype =
            ayfChannelTypeEnum.JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY;
        break;
    }

    clsOnReturn.idHwList = int.parse(tokens[1]);
    clsOnReturn.name = tokens[2];
  }
}

// ========================================================================

class AudYoFloDeviceChannel {
  bool complies = false;
  String deviceName = 'Unknown device';
  AudYoFloChannelTypeClass channelName = AudYoFloChannelTypeClass();

  void reset(int idLst, String token) {
    deviceName = '';
    channelName.reset(idLst);
    complies = false;
  }
}

void decodeDeviceChannel(
    int idLst, String token, AudYoFloDeviceChannel retVal) {
  retVal.reset(idLst, token);
  var lst = token.split('--');
  if (lst.length == 2) {
    retVal.deviceName = lst[0];
    retVal.complies = true;

    decodeInputOutputChannel(lst[1], retVal.channelName, idLst);
    int idx = retVal.deviceName.indexOf(RegExp(r'<*>'));
    if (idx != -1) {
      retVal.deviceName =
          retVal.deviceName.substring(1, retVal.deviceName.length - 1);
    }
  } else {
    retVal.channelName.name = token;
    retVal.complies = false;
  }
}

enum ayfShortenTextOption {
  AYF_SHORTEN_TEXT_FRONT,
  AYF_SHORTEN_TEXT_BACK,
  AYF_SHORTEN_TEXT_MIDDLE
}

String limitString(String txt, int numCharsMax,
    {ayfShortenTextOption shortenOption =
        ayfShortenTextOption.AYF_SHORTEN_TEXT_MIDDLE}) {
  String retVal = txt;
  int numCharsUse = numCharsMax - 4;
  numCharsUse = max(2, numCharsUse);
  int numCharsStart = numCharsUse ~/ 2;
  switch (shortenOption) {
    case ayfShortenTextOption.AYF_SHORTEN_TEXT_FRONT:
      numCharsStart = 1;
      break;
    case ayfShortenTextOption.AYF_SHORTEN_TEXT_BACK:
      numCharsStart = numCharsUse;
      break;
    default:
      break;
  }
  int numCharsStop = numCharsUse - numCharsStart;

  if (txt.length > numCharsMax) {
    retVal =
        "${txt.substring(0, numCharsStart - 1)}....${txt.substring(txt.length - numCharsStop - 1)}";
  }

  return retVal;
}

AudYoFloPropertyContentBackend? pList2Elm(
    List<AudYoFloPropertyContentBackend?> lst, String descr) {
  return lst.firstWhereOrNull((element) {
    if (element != null) {
      if (element.descriptor == descr) {
        return true;
      }
    }
    return false;
  });
}

bool setSinglePropertyValueMulti<T>(
    AudYoFloPropertyContainer mc, int posi, T val) {
  bool retVal = false;
  if (mc is AudYoFloPropertyMultiContentBackend<Float64List>) {
    AudYoFloPropertyMultiContentBackend<Float64List> pp =
        mc as AudYoFloPropertyMultiContentBackend<Float64List>;
    if (pp.fld != null) {
      if (posi < pp.fldSz) {
        if (val is double) {
          pp.fld![posi] = val;
          retVal = true;
        }
      }
    }
  } else if (mc is AudYoFloPropertyMultiContentBackend<Float32List>) {
    AudYoFloPropertyMultiContentBackend<Float32List> pp =
        mc as AudYoFloPropertyMultiContentBackend<Float32List>;
    if (pp.fld != null) {
      if (posi < pp.fldSz) {
        if (val is double) {
          pp.fld![posi] = val;
          retVal = true;
        }
      }
    }
  }
  return retVal;
}

bool setAllPropertyValueMulti<T>(AudYoFloPropertyContainer mc, T val) {
  bool retVal = false;
  if (mc is AudYoFloPropertyMultiContentBackend<Float64List>) {
    AudYoFloPropertyMultiContentBackend<Float64List> pp =
        mc as AudYoFloPropertyMultiContentBackend<Float64List>;
    if (pp.fld != null) {
      for (int posi = 0; posi < pp.fldSz; posi++) {
        if (val is double) {
          pp.fld![posi] = val;
          retVal = true;
        }
      }
    }
  } else if (mc is AudYoFloPropertyMultiContentBackend<Float32List>) {
    AudYoFloPropertyMultiContentBackend<Float32List> pp =
        mc as AudYoFloPropertyMultiContentBackend<Float32List>;
    if (pp.fld != null) {
      for (int posi = 0; posi < pp.fldSz; posi++) {
        if (val is double) {
          pp.fld![posi] = val;
          retVal = true;
        }
      }
    }
  }
  return retVal;
}

// Return a value from a property list (call by reference,
// check the AudYoFloValRef type)
bool getSinglePropertyValueMulti<T>(
    AudYoFloPropertyContainer mc, int posi, AudYoFloCallByReference<T> val) {
  bool retVal = false;
  if (mc is AudYoFloPropertyMultiContentBackend<Float64List>) {
    AudYoFloPropertyMultiContentBackend<Float64List> pp =
        mc as AudYoFloPropertyMultiContentBackend<Float64List>;
    if (pp.fld != null) {
      if (posi < pp.fldSz) {
        if (val is AudYoFloCallByReference<double>) {
          AudYoFloCallByReference<double> tVal =
              val as AudYoFloCallByReference<double>;
          tVal.value = pp.fld![posi];
          retVal = true;
        }
      }
    }
  } else if (mc is AudYoFloPropertyMultiContentBackend<Float32List>) {
    AudYoFloPropertyMultiContentBackend<Float32List> pp =
        mc as AudYoFloPropertyMultiContentBackend<Float32List>;
    if (pp.fld != null) {
      if (posi < pp.fldSz) {
        if (val is AudYoFloCallByReference<double>) {
          AudYoFloCallByReference<double> tVal =
              val as AudYoFloCallByReference<double>;
          tVal.value = pp.fld![posi];
          retVal = true;
        }
      }
    }
  }
  return retVal;
}

bool propertyValueMultiFromString(
    AudYoFloPropertyMultiContentBackend prop, String txt,
    {int offset = 0, int num = -1}) {
  bool suc = false;
  int idx = 0;
  List<String> lstEntries = str2PropList(txt);
  int cntMax = lstEntries.length;
  cntMax = min(cntMax, prop.fldSz - offset);

  if (num > 0) {
    cntMax = min(cntMax, num);
  }

  if (prop is AudYoFloPropertyMultiContentBackend<Float64List>) {
    AudYoFloPropertyMultiContentBackend<Float64List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        double val = double.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }
  } else if (prop is AudYoFloPropertyMultiContentBackend<Float32List>) {
    AudYoFloPropertyMultiContentBackend<Float32List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        double val = double.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }

    // ================================================================
  } else if (prop is AudYoFloPropertyMultiContentBackend<Int64List>) {
    AudYoFloPropertyMultiContentBackend<Int64List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        int val = int.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }
  } else if (prop is AudYoFloPropertyMultiContentBackend<Int32List>) {
    AudYoFloPropertyMultiContentBackend<Int32List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        int val = int.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }
  } else if (prop is AudYoFloPropertyMultiContentBackend<Int16List>) {
    AudYoFloPropertyMultiContentBackend<Int16List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        int val = int.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }
  } else if (prop is AudYoFloPropertyMultiContentBackend<Int8List>) {
    AudYoFloPropertyMultiContentBackend<Int8List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        int val = int.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }

    // =======================================================================
  } else if (prop is AudYoFloPropertyMultiContentBackend<Uint64List>) {
    AudYoFloPropertyMultiContentBackend<Uint64List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        int val = int.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }
  } else if (prop is AudYoFloPropertyMultiContentBackend<Uint32List>) {
    AudYoFloPropertyMultiContentBackend<Uint32List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        int val = int.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }
  } else if (prop is AudYoFloPropertyMultiContentBackend<Uint16List>) {
    AudYoFloPropertyMultiContentBackend<Uint16List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        int val = int.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }
  } else if (prop is AudYoFloPropertyMultiContentBackend<Uint8List>) {
    AudYoFloPropertyMultiContentBackend<Uint8List> propM = prop;
    if (propM.fld != null) {
      for (idx = 0; idx < cntMax; idx++) {
        int val = int.parse(lstEntries[idx]);
        propM.fld![idx] = val;
        suc = true;
      }
    }
  }
  return suc;
}

String? extractStringFromJson(Map jsonMap, String nm) {
  String? retStrPtr = getStringEntryValueMap(jsonMap, nm);
  return retStrPtr;
}

Future<List<AudYoFloOneComponentSelectionOption>?>
    checkAndUpdateComponentOptions(
        AudYoFloBackendCache be, JvxComponentIdentification tp) async {
  List<AudYoFloOneComponentSelectionOption>? lst;

  // Two attempts
  for (int idx = 0; idx < 2; idx++) {
    // This call checks that the technology has been registered.
    // If you are not allowed to call this function the call throws
    // an exeption with the error reason
    try {
      lst = be.referenceComponentListInCache(tp);
    } on AudYoFloException catch (e) {
      // This exception may occur if the technology has been unselected in the
      // backend but the info about this has not yet been forwarded to the frontend.
      // In this case the list of devices is returned as empty.
      String txt = jvxErrorTypeEInt.toStringSingle(e.errCode);
      String cp = tp.txt;
      print(
          'Error reported when trying to get reference to the devices list for technology $cp: <$txt>');

      // Return an empty list
      return lst;
    }

    // the next call may be used without check safely
    if (lst == null) {
      int errCode = await be.triggerUpdateComponentList(tp);
      if (errCode != jvxErrorType.JVX_NO_ERROR) {
        assert(false);
      }
    }
  }

  return lst;
}

void dbgPrint(String output, {String intro = '', int stackTraceLevel = 1}) {
  String strace = '';
  String introtoken = '';

  var dbgPosi = StackTrace.current.toString();
  List<String> entries = dbgPosi.split('\n');
  if (stackTraceLevel < entries.length) {
    strace = entries[stackTraceLevel];
  }

  if (strace.isNotEmpty && intro.isNotEmpty) {
    introtoken = '$strace -- $intro: ';
  } else {
    if (intro.isNotEmpty) {
      introtoken = '$intro: ';
    } else {
      introtoken = '$strace -- ';
    }
  }
  debugPrint('$introtoken $output');
}

class AudYoFloConfigComponentForWidgetStartup {
  JvxComponentIdentification cpId;
  bool showWidget = true;
  bool initialized = false;
  String token;
  AudYoFloConfigComponentForWidgetStartup(this.cpId, this.token);
}

class AudYoFloConfigComponentForWidgetStartupTD
    extends AudYoFloConfigComponentForWidgetStartup {
  JvxComponentIdentification cpIdDev;
  String tokenDev;
  AudYoFloConfigComponentForWidgetStartupTD(JvxComponentIdentification cpId,
      String token, this.cpIdDev, this.tokenDev)
      : super(cpId, token);
}

JvxComponentIdentification widgetConfigCpId(
    Map<String, String> entriesCfgWidgets,
    JvxComponentIdentification defCp,
    String token) {
  var elmBinW = entriesCfgWidgets.entries
      .firstWhereOrNull((element) => element.key == token);
  if (elmBinW != null) {
    defCp = AudYoFloStringTranslator.componentIdentificationFromString(
        elmBinW.value);
  }
  return defCp;
}

void widgetConfigCpIdShow(Map<String, String> entriesCfgWidgets,
    AudYoFloConfigComponentForWidgetStartup cfgInOut) {
  if (!cfgInOut.initialized) {
    cfgInOut.showWidget = true;
    cfgInOut.initialized = true;

    var elmBinW = entriesCfgWidgets.entries
        .firstWhereOrNull((element) => element.key == cfgInOut.token);
    if (elmBinW != null) {
      if (elmBinW.value == 'off') {
        cfgInOut.showWidget = false;
      } else {
        cfgInOut.cpId =
            AudYoFloStringTranslator.componentIdentificationFromString(
                elmBinW.value);
      }
    }
  }
}

void widgetConfigCpIdShowTD(Map<String, String> entriesCfgWidgets,
    AudYoFloConfigComponentForWidgetStartupTD cfgInOut) {
  if (!cfgInOut.initialized) {
    cfgInOut.showWidget = true;
    cfgInOut.initialized = true;

    var elmBinWT = entriesCfgWidgets.entries
        .firstWhereOrNull((element) => element.key == cfgInOut.token);
    if (elmBinWT != null) {
      if (elmBinWT.value == 'off') {
        cfgInOut.showWidget = false;
      } else {
        var elmBinWD = entriesCfgWidgets.entries
            .firstWhereOrNull((element) => element.key == cfgInOut.tokenDev);
        if (elmBinWD != null) {
          cfgInOut.cpIdDev =
              AudYoFloStringTranslator.componentIdentificationFromString(
                  elmBinWD.value);
        }
      }
    }
  }
}

class AudYoFloMixerLevelHelper {
  double dbShiftGain = 50;
  double indexUnmapDb(double idxValdB) {
    double valdB = idxValdB + dbShiftGain;
    valdB -= 100;
    return valdB;
  }

  double gain2IndexDb(double gainVal) {
    double valDB = 20 * log(gainVal + 1e-5) / log(10); // 0..1 in area -100..0
    double idxVal = valDB + 100;
    idxVal -= dbShiftGain;
    idxVal = max(idxVal, 0);
    idxVal = min(idxVal, 100);
    return idxVal;
  }

  double index2GainDb(double idxValdB) {
    double valdB = indexUnmapDb(idxValdB);
    num gainVal = pow(10.0, valdB / 20.0);
    return gainVal.toDouble();
  }
}
