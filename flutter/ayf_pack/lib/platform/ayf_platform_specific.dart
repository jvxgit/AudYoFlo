import 'package:flutter/material.dart';
import '../../ayf_pack_local.dart';

abstract class AudYoFloPlatformSpecific {
  List<String> cmdArgs = [];

  AyfHost? referenceHost();

  void prepareClose(BuildContext prepareClose);

  Widget? allocateFileDropWidget(
      JvxComponentIdentification identT, String textShowDrag, double sizeIcon);

  void configureSubSystem(Map<String, dynamic> args);

  Future<bool> initializeSubSystem(AudYoFloBackendCache? notifierBeCache,
      AudYoFloUiModel? uiModel, AudYoFloDebugModel? notifierDbgModel);

  String get lastError;
}
