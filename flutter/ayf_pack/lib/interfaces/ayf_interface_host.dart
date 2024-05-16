import '../ayf_pack_local.dart';

abstract class AyfHost {
  // The initialize function may test some things and return in a delayed way. Therefore, we
  // need to use a future here. Also note that specific host parameters must be specified in
  // argument args
  Future<bool> initialize(AudYoFloBackendCache beCache,
      AudYoFloDebugModel? dbgModel, Map<String, dynamic> args);

  // Return a string to specify which host adapter is in use.
  String descriptionHost();

  // Terminate the host
  void terminate();

  // This must be called from the main thread from time to time in case the native host is in use
  void periodicTriggerSlow();

  // Return the comple flags to address properties in a correct way.
  AudYoFloCompileFlags get compFlags;

  // Return last error that occurred
  String get lastError;
}
