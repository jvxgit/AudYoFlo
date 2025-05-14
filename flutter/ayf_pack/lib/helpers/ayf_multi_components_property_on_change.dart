import 'package:flutter/material.dart';
import 'package:collection/collection.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

class AudYoFloMultiComponentsPropertyOnChangeEntry {
  AudYoFloOneComponentSettings settings;
  AudYoFloMultiComponentsPropertyOnChangeEntry(this.settings);
}

class AudYoFloMultiComponentsPropertyOnChangeEntryBackend
    extends AudYoFloMultiComponentsPropertyOnChangeEntry {
  AudYoFloMultiComponentsPropertyOnChangeEntryBackend(
      AudYoFloOneComponentSettings settings, this.cpId)
      : super(settings);
  JvxComponentIdentification cpId;
  List<AudYoFloPropertyContainer>? bprops;
}

class AudYoFloMultiComponentsPropertyOnChangeEntryLocal
    extends AudYoFloMultiComponentsPropertyOnChangeEntry {
  AudYoFloMultiComponentsPropertyOnChangeEntryLocal(
      AudYoFloOneComponentSettings settings)
      : super(settings);

  List<AudYoFloPropertyContentLocal>? lprops;
}

mixin AudYoFloMultiComponentsPropertyOnChange {
  // ================================================
  // Member variables to configure behavior
  // ================================================

  // Single property descriptor to "listen" for in selector to update
  // build. If this tag is not set, the component is observed with not specific
  // property.
  // String? propDescriptorUpdate;

  // Specify the component to which the properties belongs that are handled in
  // this class.
  List<JvxComponentIdentification> cpsId = [];
  bool configured = false;

  // =================================================
  // Abstract callbacks
  Widget innerWidget(BuildContext context);
  AudYoFloOneComponentSettings? requestedPropertiesList(
      JvxComponentIdentification cpId);
  // =================================================

  List<int> ids = [];
  int latestValue = -1;

  // =================================================

  List<AudYoFloMultiComponentsPropertyOnChangeEntry> latestResult = [];

  void setComponentConsider(JvxComponentIdentification cpId) {
    configured = true;
    JvxComponentIdentification? elmInList =
        cpsId.firstWhereOrNull((element) => element == cpId);
    if (elmInList == null) {
      cpsId.add(cpId);
      ids.add(-1);
    }
  }

  void resetMultiPropertySettings() {
    configured = false;
    ids = [];
    latestValue = -1;
  }

  // =================================================
  // Basic functionality
  // ================================================
  Widget localBuild(BuildContext context) {
    // Get link to backend cache
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);

    // Here we start the selector & future builder code
    return Selector<AudYoFloBackendCache, int>(
        selector: (context, beCache) {
          // if property is not there, return a -1

          int cnt = 0;

          // Check here if any of the components has seen an update
          // There is no option to listen to a single component or
          // a dedicated property - I do not think this is necessary
          // since the invalidated properties are in the cash and only
          // those require an update
          for (var elm in cpsId) {
            int retVal = beCache.updateIdComponentInCache(elm);
            if (retVal != ids[cnt]) {
              latestValue++;
            }
            ids[cnt] = retVal;
            cnt++;
          }

          // Return the current id
          return latestValue;
        },

        // Return true if the id is a different one than it was before
        shouldRebuild: (previous, next) => (next != previous) ? true : false,

        // The builder
        builder: (context, notifier, child) {
          Future<int> triggerPropertyLists() async {
            int errCode = jvxErrorType.JVX_NO_ERROR;

            latestResult.clear();
            List<Future<int>> waitFutures = [];
            List<AudYoFloMultiComponentsPropertyOnChangeEntryBackend>
                waitElements = [];

            for (var elm in cpsId) {
              AudYoFloOneComponentSettings? propList =
                  requestedPropertiesList(elm);

              // Skip this component if there is no property selected
              if (propList != null) {
                AudYoFloOneComponentSettings pList = propList;

                if (pList is AudYoFloOneComponentSettingsLocal) {
                  AudYoFloMultiComponentsPropertyOnChangeEntryLocal nextProp =
                      AudYoFloMultiComponentsPropertyOnChangeEntryLocal(pList);
                  nextProp.lprops = pList.directProps;

                  // Use this as it is
                  latestResult.add(nextProp);
                } else {
                  if (pList is AudYoFloOneComponentSettingsBackend) {
                    AudYoFloOneComponentSettingsBackend pListBackend = pList;
                    AudYoFloMultiComponentsPropertyOnChangeEntryBackend
                        nextProp =
                        AudYoFloMultiComponentsPropertyOnChangeEntryBackend(
                            pList, elm);
                    nextProp.bprops =
                        theBeCache.referenceValidPropertiesComponents(
                            nextProp.cpId, pListBackend.propsDescriptors);
                    if (nextProp.bprops == null) {
                      waitFutures.add(
                          theBeCache.triggerUpdatePropertiesComponent(
                              nextProp.cpId, pListBackend.propsDescriptors));
                      waitElements.add(nextProp);
                    }
                    latestResult.add(nextProp);
                  }
                }
              }
            }

            if (waitFutures.isNotEmpty) {
              int cnt = 0;
              List<int> retVals = await Future.wait(waitFutures);
              for (var elmR in retVals) {
                if (errCode == jvxErrorType.JVX_NO_ERROR) {
                  AudYoFloOneComponentSettingsBackend pListBackend =
                      waitElements[cnt].settings
                          as AudYoFloOneComponentSettingsBackend;
                  waitElements[cnt].bprops =
                      theBeCache.referenceValidPropertiesComponents(
                          waitElements[cnt].cpId,
                          pListBackend.propsDescriptors);
                }
              }
            }

            for (var elmL in latestResult) {
              if (elmL is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
                if (elmL.bprops != null) {
                  for (var elmP in elmL.bprops!) {
                    if (!elmP.checkValidContent()) {
                      var token1 = elmP.assCpIdent.toString();
                      var token2 = elmP.descriptor;
                      dbgPrint(
                          'Warning: For component <$token1>, property <$token2> is returned as invalid.');
                    }
                  }
                }
              }
            }
            return errCode;
          }

          return FutureBuilder<int>(
              future: triggerPropertyLists(),
              builder: (BuildContext context, AsyncSnapshot<int> snapshot) {
                return innerWidget(context);
              });
        });
  }
}
