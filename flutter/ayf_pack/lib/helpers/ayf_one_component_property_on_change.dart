import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

abstract class AudYoFloOneComponentPropertyOnChange {
  // ================================================
  // Member variables to configure behavior
  // ================================================

  // Single property descriptor to "listen" for in selector to update
  // build. If this tag is not set, the component is observed with not specific
  // property.
  String? propDescriptorUpdate;

  bool dbgOutput = false;

  // Specify the component to which the properties belongs that are handled in
  // this class.
  JvxComponentIdentification cpId = JvxComponentIdentification(
      cpTp: JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN,
      slotid: 0,
      slotsubid: 0);

  // =================================================
  // Abstract callbacks
  Widget innerWidget(BuildContext context);
  List<String> requestedPropertiesList();
  void reconstructProperties(List<AudYoFloPropertyContainer> props);
  // =================================================

  // =================================================
  // Basic functionality
  // ================================================
  Widget localBuild(BuildContext context) {
    // Get link to backend cache
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);

    // Here we start the selector & future builder code
    return Selector<AudYoFloBackendCache, int>(selector: (context, beCache) {
      // if property is not there, return a -1
      int retVal = -1;

      // Either check for a specific property OR the component
      // Note that the call to triggerPropertySet call must be invoked
      // accordingly:
      // report: AyfBackendReportLevelEnum.AYF_BACKEND_REPORT_SINGLE_PROPERTY
      // to report update of a single property,
      // report:  AyfBackendReportLevelEnum.AYF_BACKEND_REPORT_PROPERTY_CACHE_COMPONENT
      // to report update of a component
      if (propDescriptorUpdate != null) {
        if (dbgOutput) {
          String txt = cpId.txt;
          print(
              'Running Provider Selector on component $txt - property $propDescriptorUpdate.');
        }

        retVal = beCache.updateIdPropertyComponentInCache(
            cpId, propDescriptorUpdate!);
      } else {
        // We are waiting for an update of the properties <ssUpdateId>
        if (dbgOutput) {
          String txt = cpId.txt;
          print('Running Provider Selector on component $txt.');
        }
        retVal = beCache.updateIdComponentInCache(cpId);
      }

      if (dbgOutput) {
        print('Update selector update id $retVal.');
      }
      // Return the current id
      return retVal;
    },

        // Return true if the id is a different one than it was before
        shouldRebuild: (previous, next) {
      if (next != previous) {
        if (dbgOutput) {
          print('Should rebuild is positive.');
        }
        return true;
      }
      if (dbgOutput) {
        print('Should rebuild is negative.');
      }
      return false;
    },

        // The builder
        builder: (context, notifier, child) {
      Future<int> triggerPropertyLists(JvxComponentIdentification cpTp) async {
        int errCode = jvxErrorType.JVX_NO_ERROR;
        List<AudYoFloPropertyContainer>? props;
        List<String> lst = requestedPropertiesList();

        if (dbgOutput) {
          print('Property update on properties:');
          for (var elms in lst) {
            print('- $elms');
          }
        }

        props = theBeCache.referenceValidPropertiesComponents(cpTp, lst);
        if (props == null) {
          if (dbgOutput) {
            print('Not all properties valid, running update routine.');
          }

          // No need to invoke a listener - we wait for the operation to complete
          errCode = await theBeCache.triggerUpdatePropertiesComponent(cpTp, lst,
              report: AyfPropertyReportLevel.AYF_FRONTEND_REPORT_NO_REPORT);
          if (errCode == jvxErrorType.JVX_NO_ERROR) {
            props = theBeCache.referenceValidPropertiesComponents(cpTp, lst);
          } else {
            cpId.reset();
          }
        }

        //inChannels = null;
        //outChannels = null;
        if (props != null) {
          reconstructProperties(props);
        }

        return errCode;
      }

      return FutureBuilder<int>(
          future: triggerPropertyLists(cpId),
          builder: (BuildContext context, AsyncSnapshot<int> snapshot) {
            if (dbgOutput) {
              print('Running function <innerWidget>.');
            }

            return innerWidget(context);
          });
    });
  }
}
