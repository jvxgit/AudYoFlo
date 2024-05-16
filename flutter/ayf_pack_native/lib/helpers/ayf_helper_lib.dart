import 'dart:ffi';
import 'dart:math';

import 'package:ffi/ffi.dart';
import 'dart:typed_data';
import 'package:collection/collection.dart';

//import 'package:ayf_pack/ayf_pack.dart';

import 'package:ayf_pack/ayf_pack.dart';

void printFormattedTextMessage(int id2, Pointer<Void> load_fld) {
  Pointer<Utf8> cstr = load_fld.cast<Utf8>();
  String txt = cstr.toDartString();
  String typeMess = "";
  switch (id2) {
    case jvxReportPriority.JVX_REPORT_PRIORITY_DEBUG:
      typeMess = "DEBUG: ";
      break;
    case jvxReportPriority.JVX_REPORT_PRIORITY_ERROR:
      typeMess = "ERROR: ";
      break;
    case jvxReportPriority.JVX_REPORT_PRIORITY_INFO:
      typeMess = "INFO: ";
      break;
    case jvxReportPriority.JVX_REPORT_PRIORITY_SUCCESS:
      typeMess = "SUCCESS: ";
      break;
    case jvxReportPriority.JVX_REPORT_PRIORITY_VERBOSE:
      typeMess = "VERBOSE: ";
      break;
    case jvxReportPriority.JVX_REPORT_PRIORITY_WARNING:
      typeMess = "WARNING: ";
      break;
  }
  print("${typeMess}${txt}");
}

// This function checks if a cahced device list is in memory, otherwise it refreshes the
// list and returns it afterwards
Future<List<AudYoFloOneDeviceSelectionOption>?> checkAndUpdateDeviceOptions(
    AudYoFloBackendCache be, JvxComponentIdentification tp) async {
  List<AudYoFloOneDeviceSelectionOption>? lst;

  // Two attempts
  for (int idx = 0; idx < 2; idx++) {
    // This call checks that the technology has been registered.
    // If you are not allowed to call this function the call throws
    // an exeption with the error reason
    try {
      lst = be.referenceDeviceListInCache(tp);
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
      int errCode = await be.triggerUpdateDeviceList(tp);
      if (errCode != jvxErrorType.JVX_NO_ERROR) {
        String errTxt = jvxErrorTypeEInt.toStringSingle(errCode);
        print('Could not find any devices, error reason: <$errTxt>.');
      }
    }
  }

  return lst;
}
