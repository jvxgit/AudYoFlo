import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

mixin AudYoFloOneTechnologyDevicesOnChange {
  // ================================================
  // Member variables to configure behavior
  // ================================================

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
  void updateDevices(List<AudYoFloOneDeviceSelectionOption> devs);
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

      // We are waiting for an update of the properties <ssUpdateId>
      if (dbgOutput) {
        String txt = cpId.txt;
        print('Running Provider Selector on component $txt.');
      }

      retVal = beCache.updateIdDeviceListInCache(cpId);
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
      Future<int> triggerDevicesLists(JvxComponentIdentification cpTp) async {
        int errCode = jvxErrorType.JVX_NO_ERROR;
        List<AudYoFloOneDeviceSelectionOption>? devs;

        devs = theBeCache.referenceDeviceListInCache(cpTp);
        if (devs == null) {
          if (dbgOutput) {
            print('Devices not valid, running update routine.');
          }

          // No need to invoke a listener - we wait for the operation to complete
          errCode = await theBeCache.triggerUpdateDeviceList(cpTp);
          if (errCode == jvxErrorType.JVX_NO_ERROR) {
            devs = theBeCache.referenceDeviceListInCache(cpTp);
          } else {
            if (errCode == jvxErrorType.JVX_ERROR_NOT_READY) {
              AudYoFloHelper.dbgPrint(
                  'Backend not ready yet, coming back later!');
            } else {
              AudYoFloHelper.dbgPrint(
                  'Backend returns error, deactivating widget main function!');
              cpId.reset();
            }
          }
        }

        //inChannels = null;
        //outChannels = null;
        if (devs != null) {
          updateDevices(devs);
        }

        return errCode;
      }

      return FutureBuilder<int>(
          future: triggerDevicesLists(cpId),
          builder: (BuildContext context, AsyncSnapshot<int> snapshot) {
            if (dbgOutput) {
              print('Running function <innerWidget>.');
            }

            return innerWidget(context);
          });
    });
  }
}
