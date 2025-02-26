import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../../ayf_pack_local.dart';

class AudYoFloPlatformSpecificHtml extends AudYoFloPlatformSpecific {
  AyfHost? theBeAdapter;
  @override
  AyfHost? referenceHost() {
    return theBeAdapter;
  }

  @override
  void prepareClose(BuildContext context) {
    /*
     * This installs a handler for the "close" event. 
     */

    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);

    // Skip this part: we can not really catch the close button on "web"
  }

  @override
  Future<void> triggerClose(AudYoFloBackendCache theBeCache) async {}

  // Allocate and return file drop widget - there is none at this time..
  @override
  Widget? allocateFileDropWidget(JvxComponentIdentification identT,
          String textShowDrag, double sizeIcon) =>
      null;

  @override
  Future<bool> initializeSubSystem(AudYoFloBackendCache? notifierBeCache,
      AudYoFloUiModel? uiModel, AudYoFloDebugModel? notifierDbgModel) async {
    // Do nothing atz the moment

    bool retVal = true;
    if (notifierBeCache != null) {
      Map<String, dynamic> args = {};
      args['cmdArgs'] = cmdArgs;
      retVal = await theBeAdapter!
          .initialize(notifierBeCache, notifierDbgModel, args);
      if (retVal == false) {
        // theBeAdapter!.
      }
    }

    return true;
  }

  @override
  void configureSubSystem(Map<String, dynamic> args) {
    var elm = args.entries.firstWhereOrNull((element) => element.key == 'host');
    if (elm != null) {
      if (elm.value is AyfHost) {
        theBeAdapter = elm.value as AyfHost;
      }
    }
    assert(theBeAdapter != null);
  }

  @override
  String get lastError {
    if (theBeAdapter != null) {
      return theBeAdapter!.lastError;
    }
    return '';
  }
}

AudYoFloPlatformSpecific allocatePlatformSpecific() {
  return AudYoFloPlatformSpecificHtml();
}
