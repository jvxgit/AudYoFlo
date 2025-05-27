import 'dart:convert';

import 'package:ayf_pack/ayf_pack.dart';
import 'backend/ayf_web_backend_anomalies.dart';

import 'package:http/http.dart' as http;
import 'dart:io';
import 'dart:async';

class AudYoFloUrlRequest {
  Map<dynamic, dynamic> jsonMap = {};
  String lastError = '';
}

// ===========================================================

mixin AudYoFloWebBackendHelpers {
  AudYoFloWebBackendAnomalies? reportHandler;
  void initializeHelper(AudYoFloWebBackendAnomalies reportRef) {
    reportHandler = reportRef;
  }

  Future<int> webRequestHttpGet(
      Uri url, AudYoFloUrlRequest resultOnReturn) async {
    int retCode = jvxErrorType.JVX_NO_ERROR;
    resultOnReturn.lastError = '';
    try {
      var response = await http.get(url);
      if (response.statusCode == 200) {
        String responseBody = response.body;
        if (responseBody.isNotEmpty) {
          resultOnReturn.jsonMap = jsonDecode(responseBody);
        } else {
          resultOnReturn.jsonMap = {};
        }
      }
    } on TimeoutException catch (e) {
      resultOnReturn.lastError = 'Timeout Error: $e';
      retCode = jvxErrorType.JVX_ERROR_TIMEOUT;
    } on SocketException catch (e) {
      resultOnReturn.lastError = 'Socket Error: $e';
      retCode = jvxErrorType.JVX_ERROR_PROTOCOL_ERROR;
    } on Error catch (e) {
      resultOnReturn.lastError = 'General Error: $e';
      retCode = jvxErrorType.JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
    } on http.ClientException catch (e) {
      resultOnReturn.lastError = 'Client Exception Error: $e';
      retCode = jvxErrorType.JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
    }

    if (retCode == jvxErrorType.JVX_NO_ERROR) {
      String? errCodeExprPtr = AudYoFloHelper.getStringEntryValueMap(
          resultOnReturn.jsonMap, 'return_code');
      if (errCodeExprPtr != null) {
        // Convert error code
        String errCodeExpr = errCodeExprPtr;
        retCode = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
      }

      if (retCode != jvxErrorType.JVX_NO_ERROR) {
        resultOnReturn.lastError = 'Json response returned ' +
            jvxErrorTypeEInt.toStringSingle(retCode);
      }
    }
    return retCode;
  }

  Future<int> webRequestHttpPost(
      Uri url, String param, AudYoFloUrlRequest resultOnReturn) async {
    int retCode = jvxErrorType.JVX_NO_ERROR;
    resultOnReturn.lastError = '';
    try {
      var response = await http.post(url, body: param);
      if (response.statusCode == 200) {
        String responseBody = response.body;
        resultOnReturn.jsonMap = jsonDecode(responseBody);
      }
    } on TimeoutException catch (e) {
      resultOnReturn.lastError = 'Timeout Error: $e';
      retCode = jvxErrorType.JVX_ERROR_TIMEOUT;
    } on SocketException catch (e) {
      resultOnReturn.lastError = 'Socket Error: $e';
      retCode = jvxErrorType.JVX_ERROR_PROTOCOL_ERROR;
    } on Error catch (e) {
      resultOnReturn.lastError = 'General Error: $e';
      retCode = jvxErrorType.JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
    } on http.ClientException catch (e) {
      resultOnReturn.lastError = 'Client Exception Error: $e';
      retCode = jvxErrorType.JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
    }

    if (retCode == jvxErrorType.JVX_NO_ERROR) {
      String? errCodeExprPtr = AudYoFloHelper.getStringEntryValueMap(
          resultOnReturn.jsonMap, 'return_code');
      if (errCodeExprPtr != null) {
        // Convert error code
        String errCodeExpr = errCodeExprPtr;
        retCode = jvxErrorTypeEInt.fromStringSingle(errCodeExpr);
      }

      if (retCode != jvxErrorType.JVX_NO_ERROR) {
        resultOnReturn.lastError = 'Json response returned ' +
            jvxErrorTypeEInt.toStringSingle(retCode);
      }
    }
    return retCode;
  }

  JvxComponentTypeEnum translateStringComponentType(String inStr) {
    JvxComponentTypeEnum retVal = string2CpTp(inStr);
    if (retVal == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      if (reportHandler != null) {
        reportHandler!.reportStatusErrorProtocol(1, 0, inStr);
      }
    }
    return retVal;
  }

  /*
    switch (inStr) {
      case "signal processing technology":
        return JvxComponentTypeEnum.JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY;
      case "signal processing device":
        return JvxComponentTypeEnum.JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE;
      case "signal processing node":
        return JvxComponentTypeEnum.JVX_COMPONENT_SIGNAL_PROCESSING_NODE;
      case "audio technology":
        return JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY;
      case "audio device":
        return JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE;
      case "audio node":
        return JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_NODE;
      case "video technology":
        return JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_TECHNOLOGY;
      case "video device":
        return JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_DEVICE;
      case "video node":
        return JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_NODE;
      case "application controller technology":
        return JvxComponentTypeEnum
            .JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY;
      case "application controller device":
        return JvxComponentTypeEnum.JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE;
      case "application controller node":
        return JvxComponentTypeEnum.JVX_COMPONENT_APPLICATION_CONTROLLER_NODE;
      case "custom technology":
        return JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_TECHNOLOGY;
      case "custom device":
        return JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_DEVICE;
      case "custom node":
        return JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_NODE;
      case "extern technology":
        return JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_TECHNOLOGY;
      case "extern device":
        return JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_DEVICE;
      case "extern node":
        return JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_NODE;
      case "dynamic node":
        return JvxComponentTypeEnum.JVX_COMPONENT_DYNAMIC_NODE;
      case "package":
        return JvxComponentTypeEnum.JVX_COMPONENT_PACKAGE;
      case "min host":
        return JvxComponentTypeEnum.JVX_COMPONENT_MIN_HOST;
      case "factory host":
        return JvxComponentTypeEnum.JVX_COMPONENT_FACTORY_HOST;
      case "host":
        return JvxComponentTypeEnum.JVX_COMPONENT_HOST;

      case "packet filter rule":
        return JvxComponentTypeEnum.JVX_COMPONENT_PACKETFILTER_RULE;
      case "log remote handler":
        return JvxComponentTypeEnum.JVX_COMPONENT_LOGREMOTEHANDLER;
      case "audio codec":
        return JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_CODEC;
      case "audio encoder":
        return JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_ENCODER;
      case "audio decoder":
        return JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DECODER;
      case "audio file reader":
        return JvxComponentTypeEnum.JVX_COMPONENT_RT_AUDIO_FILE_READER;
      case "audio file writer":
        return JvxComponentTypeEnum.JVX_COMPONENT_RT_AUDIO_FILE_WRITER;
      case "resampler":
        return JvxComponentTypeEnum.JVX_COMPONENT_RESAMPLER;
      case "data converter":
        return JvxComponentTypeEnum.JVX_COMPONENT_DATACONVERTER;
      case "processing group":
        return JvxComponentTypeEnum.JVX_COMPONENT_PROCESSING_GROUP;
      case "processing process":
        return JvxComponentTypeEnum.JVX_COMPONENT_PROCESSING_PROCESS;
      case "off host":
        return JvxComponentTypeEnum.JVX_COMPONENT_OFF_HOST;
      case "interceptor":
        return JvxComponentTypeEnum.JVX_COMPONENT_INTERCEPTOR;
      case "automation":
        return JvxComponentTypeEnum.JVX_COMPONENT_SYSTEM_AUTOMATION;

      case "encryption/decryption":
        return JvxComponentTypeEnum.JVX_COMPONENT_CRYPT;
      case "web server":
        return JvxComponentTypeEnum.JVX_COMPONENT_WEBSERVER;
      case "remote call":
        return JvxComponentTypeEnum.JVX_COMPONENT_REMOTE_CALL;
      case "event loop":
        return JvxComponentTypeEnum.JVX_COMPONENT_EVENTLOOP;
      case "data logger":
        return JvxComponentTypeEnum.JVX_COMPONENT_DATALOGGER;
      case "data log reader":
        return JvxComponentTypeEnum.JVX_COMPONENT_DATALOGREADER;
      case "connection":
        return JvxComponentTypeEnum.JVX_COMPONENT_CONNECTION;
      case "config processor":
        return JvxComponentTypeEnum.JVX_COMPONENT_CONFIG_PROCESSOR;
      case "external call":
        return JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_CALL;
      case "thread controller":
        return JvxComponentTypeEnum.JVX_COMPONENT_THREADCONTROLLER;
      case "threads":
        return JvxComponentTypeEnum.JVX_COMPONENT_THREADS;
      case "shared mem":
        return JvxComponentTypeEnum.JVX_COMPONENT_SHARED_MEMORY;
      case "text2speech":
        return JvxComponentTypeEnum.JVX_COMPONENT_TEXT2SPEECH;
      case "textlog":
        return JvxComponentTypeEnum.JVX_COMPONENT_SYSTEM_TEXT_LOG;
      case "localtextlog":
        return JvxComponentTypeEnum.JVX_COMPONENT_LOCAL_TEXT_LOG;
      default:
        if (reportHandler != null) {
          reportHandler!.reportStatusErrorProtocol(1, 0, inStr);
        }
        break;
*/

  /*
    case :
      return JvxComponentTypeEnum.;
	{"event loop", "JVX_COMPONENT_EVENTLOOP" },
    case :
      return JvxComponentTypeEnum.;
	{"data logger", "JVX_COMPONENT_DATALOGGER" },
    case :
      return JvxComponentTypeEnum.;
	{"data log reader", "JVX_COMPONENT_DATALOGREADER" },
    case :
      return JvxComponentTypeEnum.;
	{"connection", "JVX_COMPONENT_CONNECTION" },
    case :
      return JvxComponentTypeEnum.;
	{"config processor", "JVX_COMPONENT_CONFIG_PROCESSOR" },
    case :
      return JvxComponentTypeEnum.;
	{"external call", "JVX_COMPONENT_EXTERNAL_CALL" },
    case :
      return JvxComponentTypeEnum.;
	{"thread controller", "JVX_COMPONENT_THREADCONTROLLER" },
    case :
      return JvxComponentTypeEnum.;
	{"threads", "JVX_COMPONENT_THREADS"},
    case :
      return JvxComponentTypeEnum.;
	{"shared mem", "JVX_COMPONENT_SHARED_MEMORY"},
	{ "text2speech", "JVX_COMPONENT_TEXT2SPEECH" },
	{ "textlog", "JVX_COMPONENT_SYSTEM_TEXT_LOG" },
      default: 
      assert(false);
  }
}

	{
	{ "localtextlog", "JVX_COMPONENT_LOCAL_TEXT_LOG" },
	{"encryption/decryption", "JVX_COMPONENT_CRYPT" },
	{"web server", "JVX_COMPONENT_WEBSERVER" },
	{"remote call", "JVX_COMPONENT_REMOTE_CALL" },
	{"packet filter rule", "JVX_COMPONENT_PACKETFILTER_RULE" },
	{"log remote handler", "JVX_COMPONENT_LOGREMOTEHANDLER" }
  */

  /*
    }
    return JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN;
  }
  */

  jvxComponentTypeClassEnum translateStringComponentTypeClass(String inStr) {
    switch (inStr) {
      case "technologies":
        return jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_TECHNOLOGY;
      case "nodes":
        return jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NODE;
      case "hosts":
        return jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_HOST;
      case "codecs":
        return jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_CODEC;
      case "tools":
        return jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_TOOL;
      case "simple":
        return jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_SIMPLE;
      case "device":
        return jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_DEVICE;
      case "process":
        return jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_PROCESS;
    }
    return jvxComponentTypeClassEnum.JVX_COMPONENT_TYPE_NONE;
  }

  JvxComponentIdentification translateStringComponentIdentification(
      String cpIdStr, int uid) {
    JvxComponentIdentification retVal = JvxComponentIdentification();
    retVal = string2CpId(cpIdStr, uid);
    if (retVal == JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      if (reportHandler != null) {
        reportHandler!.reportStatusErrorProtocol(1, 0, cpIdStr);
      }
    }
    return retVal;

/*
    JvxComponentIdentification cpId = JvxComponentIdentification();
    var lst = cpIdStr.split('<');
    cpId.cpTp = translateStringComponentType(lst[0]);
    if (cpId.cpTp != JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpId.slotid = 0;
      cpId.slotsubid = 0;
    }
    if (lst.length > 1) {
      lst = lst[1].split('>');
      lst = lst[0].split(',');
      cpId.slotid = int.parse(lst[0]);
      if (lst.length > 1) {
        cpId.slotsubid = int.parse(lst[1]);
      }
    }

    cpId.uid = uid;    
    return cpId;
    */
  }

  int translateStringState(state) {
    return jvxStateEInt.fromStringSingle(state);
  }

  String translateCompType(JvxComponentTypeEnum cpTp) {
    String retVal = '';
    switch (cpTp) {
      /*
    case JvxComponentTypeEnum.XXX:
    retVal = 'XXX';
    break;
    */
      case JvxComponentTypeEnum.JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY:
        retVal = 'signal_processing_technology';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE:
        retVal = 'signal_processing_device';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_SIGNAL_PROCESSING_NODE:
        retVal = 'signal_processing_node';
        break;

      case JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY:
        retVal = 'audio_technology';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE:
        retVal = 'audio_device';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_NODE:
        retVal = 'audio_node';
        break;

      case JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_TECHNOLOGY:
        retVal = 'video technology';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_DEVICE:
        retVal = 'video_device';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_NODE:
        retVal = 'video_node';
        break;

      case JvxComponentTypeEnum.JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY:
        retVal = 'application_controller_technology';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE:
        retVal = 'application_controller_device';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_APPLICATION_CONTROLLER_NODE:
        retVal = 'application_controller_node';
        break;

      case JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_TECHNOLOGY:
        retVal = 'custom_technology';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_DEVICE:
        retVal = 'custom_device';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_NODE:
        retVal = 'custom_node';
        break;

      case JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_TECHNOLOGY:
        retVal = 'extern_technology';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_DEVICE:
        retVal = 'extern_device';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_NODE:
        retVal = 'extern_node';
        break;

      case JvxComponentTypeEnum.JVX_COMPONENT_PACKAGE:
        retVal = 'component_package';
        break;

      case JvxComponentTypeEnum.JVX_COMPONENT_MIN_HOST:
        retVal = 'min_host';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_FACTORY_HOST:
        retVal = 'factory_host';
        break;
      case JvxComponentTypeEnum.JVX_COMPONENT_HOST:
        retVal = 'host';
        break;

      /*
  case JvxComponentTypeEnum.JVX_COMPONENT_EVENTLOOP:
    retVal = 'event_loop';
    break;
  
  case JvxComponentTypeEnum.JVX_COMPONENT_DATALOGGER:
    retVal = 'datalogger';
    break;
  case JvxComponentTypeEnum.JVX_COMPONENT_DATALOGREADER:
    retVal = 'datalogreader';
    break;

  case JvxComponentTypeEnum.JVX_COMPONENT_CONNECTION:
    retVal = 'connection';
    break;
  JVX_COMPONENT_CONFIG_PROCESSOR,
  JVX_COMPONENT_EXTERNAL_CALL,
  JVX_COMPONENT_THREADCONTROLLER,
  JVX_COMPONENT_THREADS,
  JVX_COMPONENT_SHARED_MEMORY,
  JVX_COMPONENT_TEXT2SPEECH,
  JVX_COMPONENT_SYSTEM_TEXT_LOG,
  JVX_COMPONENT_LOCAL_TEXT_LOG,
  JVX_COMPONENT_CRYPT,
  JVX_COMPONENT_WEBSERVER,
  JVX_COMPONENT_REMOTE_CALL,
  JVX_COMPONENT_PACKETFILTER_RULE,
  JVX_COMPONENT_LOGREMOTEHANDLER,
  */
      default:
        assert(false);
    }
    return retVal;
  }

  // @override is not correct but this mixin implements interface
  int translateEnumString(
      String selection, String formatName, AudYoFloCompileFlags flags) {
    return AudYoFloStringTranslator.translateEnumString(
        selection, formatName, flags);
  }
}
