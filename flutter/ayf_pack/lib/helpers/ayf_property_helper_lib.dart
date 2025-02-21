import '../ayf_pack_local.dart';
import 'dart:typed_data';

bool extractBoolProp(AudYoFloPropertyMultiContentBackend? prop) {
  bool flagOnReturn = false;
  AudYoFloPropertyMultiContentBackend<Uint16List>? propUi16;
  if (prop != null) {
    if (prop is AudYoFloPropertyMultiContentBackend<Uint16List>) {
      propUi16 = prop;
    }
    if (propUi16 != null) {
      if (propUi16.fldSz > 0) flagOnReturn = (propUi16.fld![0] != 0);
    }
  }
  return flagOnReturn;
}

bool toggleTransferBoolProp(AudYoFloBackendCache? theBeCache,
    AudYoFloPropertyMultiContentBackend? prop) {
  bool retVal = false;
  if (theBeCache != null) {
    AudYoFloPropertyMultiContentBackend<Uint16List>? propUi16;
    List<String> propLst = [];
    if (prop != null) {
      if (prop is AudYoFloPropertyMultiContentBackend<Uint16List>) {
        propUi16 = prop;
      }
      if (propUi16 != null) {
        if (propUi16.fld != null) {
          if (propUi16.fld![0] != 0) {
            propUi16.fld![0] = 0;
          } else {
            propUi16.fld![0] = 1;
          }
          propLst.add(propUi16.parpropc.descriptor);
        }
        theBeCache!.triggerSetProperties(prop.parpropc.assCpIdent, propLst,
            report: AyfPropertyReportLevel
                .AYF_BACKEND_REPORT_COMPONENT_PROPERTY_COLLECT);
        retVal = true;
      }
    }
  }
  return retVal;
}
