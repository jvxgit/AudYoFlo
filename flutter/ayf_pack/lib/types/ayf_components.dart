import '../ayf_pack_local.dart';

class JvxComponentIdentification {
  JvxComponentTypeEnum cpTp; // = jvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN;
  int slotid; // = -1;
  int slotsubid; // = -1;
  int uid; // = -1;

  static JvxComponentIdentification from(JvxComponentIdentification cpFrom) {
    JvxComponentIdentification returnme = JvxComponentIdentification();
    returnme.cpTp = cpFrom.cpTp;
    returnme.slotid = cpFrom.slotid;
    returnme.slotsubid = cpFrom.slotsubid;
    returnme.uid = cpFrom.slotsubid;
    return returnme;
  }

  JvxComponentIdentification(
      {this.cpTp = JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN,
      this.slotid = -1,
      this.slotsubid = -1,
      this.uid = -1});

  void reset() {
    cpTp = JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN;
    slotid = -1;
    slotsubid = -1;
    uid = -1;
  }

  String get txt {
    String ret = cpTp.txt;
    ret += '<$slotid,$slotsubid>';
    return ret;
  }

  String get txtshort {
    String ret = "";
    if (slotid > 0) {
      ret = "<$slotid";
      if (slotsubid > 0) {
        ret = "$ret,$slotsubid>";
      } else {
        ret = "$ret>";
      }
    } else {
      if (slotsubid > 0) {
        ret = "<0,$slotsubid>";
      }
    }
    return ret;
  }

  bool operator ==(Object arg) {
    if (arg is JvxComponentIdentification) {
      JvxComponentIdentification argc = arg;
      if ((argc.cpTp == cpTp) &&
          (argc.slotid == slotid) &&
          (argc.slotsubid == slotsubid)) {
        return true;
      }
    }
    return false;
  }
}

JvxComponentTypeEnum string2CpTp(String inStr) {
  JvxComponentTypeEnum tpEn = JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN;

  switch (inStr) {
    case "signal processing technology":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY;
      break;
    case "signal processing device":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE;
      break;

    case "signal processing node":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_SIGNAL_PROCESSING_NODE;
      break;

    case "audio technology":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_TECHNOLOGY;
      break;

    case "audio device":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DEVICE;
      break;

    case "audio node":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_NODE;
      break;

    case "video technology":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_TECHNOLOGY;
      break;

    case "video device":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_DEVICE;
      break;

    case "video node":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_VIDEO_NODE;
      break;

    case "application controller technology":
      tpEn =
          JvxComponentTypeEnum.JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY;
      break;

    case "application controller device":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE;
      break;

    case "application controller node":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_APPLICATION_CONTROLLER_NODE;
      break;

    case "custom technology":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_TECHNOLOGY;
      break;

    case "custom device":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_DEVICE;
      break;

    case "custom node":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_CUSTOM_NODE;
      break;

    case "extern technology":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_TECHNOLOGY;
      break;

    case "extern device":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_DEVICE;
      break;

    case "extern node":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_NODE;
      break;

    case "dynamic node":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_DYNAMIC_NODE;
      break;

    case "package":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_PACKAGE;
      break;

    case "min host":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_MIN_HOST;
      break;

    case "factory host":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_FACTORY_HOST;
      break;

    case "host":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_HOST;
      break;

    case "packet filter rule":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_PACKETFILTER_RULE;
      break;

    case "log remote handler":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_LOGREMOTEHANDLER;
      break;

    case "audio codec":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_CODEC;
      break;

    case "audio encoder":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_ENCODER;
      break;

    case "audio decoder":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_DECODER;
      break;

    case "audio file reader":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_RT_AUDIO_FILE_READER;
      break;

    case "audio file writer":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_RT_AUDIO_FILE_WRITER;
      break;

    case "resampler":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_RESAMPLER;
      break;

    case "data converter":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_DATACONVERTER;
      break;

    case "processing group":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_PROCESSING_GROUP;
      break;

    case "processing process":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_PROCESSING_PROCESS;
      break;

    case "off host":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_OFF_HOST;
      break;

    case "interceptor":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_INTERCEPTOR;
      break;

    case "automation":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_SYSTEM_AUTOMATION;
      break;

    case "encryption/decryption":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_CRYPT;
      break;

    case "web server":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_WEBSERVER;
      break;

    case "remote call":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_REMOTE_CALL;
      break;

    case "event loop":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_EVENTLOOP;
      break;

    case "data logger":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_DATALOGGER;
      break;

    case "data log reader":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_DATALOGREADER;
      break;

    case "connection":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_CONNECTION;
      break;

    case "config processor":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_CONFIG_PROCESSOR;
      break;

    case "external call":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_EXTERNAL_CALL;
      break;

    case "thread controller":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_THREADCONTROLLER;
      break;

    case "threads":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_THREADS;
      break;

    case "shared mem":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_SHARED_MEMORY;
      break;

    case "text2speech":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_TEXT2SPEECH;
      break;

    case "textlog":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_SYSTEM_TEXT_LOG;
      break;

    case "localtextlog":
      tpEn = JvxComponentTypeEnum.JVX_COMPONENT_LOCAL_TEXT_LOG;
      break;

    default:
      break;
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
  }
  return tpEn;
}

JvxComponentIdentification string2CpId(String cpIdStr, int uid) {
  JvxComponentIdentification cpId = JvxComponentIdentification();
  var lst = cpIdStr.split('<');
  cpId.cpTp = string2CpTp(lst[0]);
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
}
