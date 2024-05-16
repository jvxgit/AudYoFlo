/*
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

bool setSinglePropertyValueMulti<T>(
    AudYoFloPropertyContainer mc, int posi, T val) {
  bool retVal = false;
  if (mc is AudYoFloPropertyMultiContentNative<Double, Float64List>) {
    AudYoFloPropertyMultiContentNative<Double, Float64List> pp = mc;
    if (pp.fld != null) {
      if (posi < pp.fldSz) {
        if (val is double) {
          pp.fld![posi] = val;
          retVal = true;
        }
      }
    }
  } else if (mc is AudYoFloPropertyMultiContentNative<Float, Float32List>) {
    AudYoFloPropertyMultiContentNative<Float, Float32List> pp = mc;
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

// Return a value from a property list (call by reference,
// check the AudYoFloValRef type)
bool getSinglePropertyValueMulti<T>(
    AudYoFloPropertyContainer mc, int posi, AudYoFloCallByReference<T> val) {
  bool retVal = false;
  if (mc is AudYoFloPropertyMultiContentNative<Double, Float64List>) {
    AudYoFloPropertyMultiContentNative<Double, Float64List> pp = mc;
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
  } else if (mc is AudYoFloPropertyMultiContentNative<Float, Float32List>) {
    AudYoFloPropertyMultiContentNative<Float, Float32List> pp = mc;
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
*/