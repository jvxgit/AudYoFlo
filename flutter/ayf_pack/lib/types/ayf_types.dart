import '../ayf_pack_local.dart';

enum AudYoFloCommandQueueElement { AYF_COMMAND_QUEUE_NONE }

class AudYoFloOneComponentOptions {
  String name = '';
  int stat = jvxState.JVX_STATE_NONE;
  AudYoFloOneComponentOptions({required this.name, required this.stat});
}

class AudYoFloOneComponentSelectionOptions {
  List<AudYoFloOneComponentOptions> compAvail = [];

  // The list of available components may depend on a "parent".
  // In this case, the list of available components must be updated if the state changes.
  JvxComponentIdentification cpIdDependsOn;
  int ssUpdateId = 0;

  AudYoFloOneComponentSelectionOptions({required this.cpIdDependsOn});
}

class AudYoFloOneComponentWithDetail {
  JvxComponentIdentification cpId = JvxComponentIdentification();
  String description = '';
  String moduleName = '';
}

class AudYoFloStringWithId {
  String name = 'Unknown';
  int id = -1;
  AudYoFloStringWithId(this.name, this.id);
}

class AudYoFloException implements Exception {
  int errCode = jvxErrorType.JVX_NO_ERROR;

  AudYoFloException(this.errCode, String reason) {
    Exception(reason);
  }
}

// ===========================================================
// This is a little bit complicated: a call by reference does not work for
// simple datatypes and a return value in a template function can not be assigned
// (or I did not find a way how to assign it). Therefore we need to pass
// the value in a struct. This will be a call by reference here and there
class AudYoFloCallByReference<T> {
  T variable;
  AudYoFloCallByReference(this.variable);
  set value(T valArg) {
    variable = valArg;
  }
}

// This class defines a callback that reports the selection (=clicked) of a
// specific component. Currently, it is used to select a device within the list of active
// devices
mixin AudYoFloSelectionComponentUi {
  // End up in this callback in case a selection was chosen (=clicked)
  void reportSelectionComponentUi(JvxComponentIdentification cpId);
}

class AudYoFloCompileFlags {
  int compileNum32BitBitfield = 0;
  jvxDataTypeSpecEnum compileDataTypeSpec =
      jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_DOUBLE;
}
