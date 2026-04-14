import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:collection/collection.dart';
import 'package:ayf_pack/ayf_pack_local.dart';
import 'ayf_phone_widget_tech_control.dart';
import 'ayf_phone_widget_devices.dart';

class AudYoFloStarterNTopLevelWidget extends StatefulWidget {
  final JvxComponentIdentification cpId;

  /*
  final double width;
  final double height;
  final String fNameMicSrcIcon;
  
  */

  final String tagDevices;
  final Widget? Function(JvxComponentIdentification selCpId)
      callbackDeviceWidget;

  const AudYoFloStarterNTopLevelWidget(this.cpId, this.tagDevices,
      this.callbackDeviceWidget, {super.key});

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloPhoneTopLevelWidgetState();
  }
}

// Full property access: Selector and future builder in one.
// Howeverm we use the <AudYoFloAnyPropertyOnComponentUpdate>
// class to simplfy the usage
class _AudYoFloPhoneTopLevelWidgetState
    extends State<AudYoFloStarterNTopLevelWidget>
    with
        AudYoFloMultiComponentsPropertyOnChange,
        AudYoFloPropertyOnChangeGroups {
  // Widget to hold phone status
  AudYoFloPropertyOnChangeOneGroup oneGroupStatus =
      AudYoFloPropertyOnChangeOneGroup([
    '/user_accounts',
    '/user_account_status',
    '/active_calls',
    '/phonebook_entries',
    '/phonebook_status',
    '/start_call',
    '/start_video'
  ]);

  AudYoFloPropertyOnChangeOneGroup oneGroupUAEdit =
      AudYoFloPropertyOnChangeOneGroup([
    '/user_accounts',
    '/user_account_status',
    '/active_calls',
    '/phonebook_entries',
    '/phonebook_status',
    '/start_call'
  ]);

  AudYoFloOneComponentSettingsBackend mySettings =
      AudYoFloOneComponentSettingsBackend(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN,
              slotid: -1,
              slotsubid: -1),
          'tagPropertyBaresip',
          []);

  _AudYoFloPhoneTopLevelWidgetState() {
    theGroups['status'] = oneGroupStatus;
    theGroups['UAEdit'] = oneGroupUAEdit;

    createList();

    // Store the local property component
    mySettings.propsDescriptors = propsAllGroups;
  }

  @override
  AudYoFloOneComponentSettings? requestedPropertiesList(
      JvxComponentIdentification cpId) {
    if (cpId == mySettings.cpId) {
      return mySettings;
    }
    return null;
  }

  void reconstructProperties(List<AudYoFloPropertyContainer> props) {
    reconstructProperyReferences(props);
  }

  Widget innerWidget(BuildContext context) {
    // We always end up here if the component reports any modification
    // Reconstruct the properties from the latestResults struct
    var fElm = latestResult.firstWhereOrNull((str) {
      if (str is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
        return (str.cpId == mySettings.cpId);
      }
      return false;
    });
    if (fElm != null) {
      if (fElm is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
        if (fElm.bprops != null) {
          reconstructProperties(fElm.bprops!);
        }
      }
    }

    return Column(
      children: [
        // This is how we can arrange a scroll list with a header:
        // All elements with fixed size and the endless one with Expanded!

        // Ctrl elements to start calls
        AudYoFloPhoneCtrlWidget(this),

        // Spacer
        Container(
          color: Colors.white,
          height: 10,
        ),

        // List of connections
        Expanded(
          child: Container(
          color: Colors.blue,          
        ),
        ),
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    if (configured == false) {
      // Read out the component of focus from the widget
      mySettings.cpId = widget.cpId;
      resetMultiPropertySettings();
      setComponentConsider(mySettings.cpId);
    }
    return localBuild(context);
  }
}
// The three properties

class AudYoFloPhoneWidget extends StatelessWidget {
  final JvxComponentIdentification cpId;

  final String tagDevices;
  final Widget? Function(JvxComponentIdentification selCpId)
      callbackDeviceWidget;

  const AudYoFloPhoneWidget(this.cpId, this.tagDevices,
      this.callbackDeviceWidget, {super.key});
  @override
  Widget build(BuildContext context) {
    AudYoFloUiModel uiModel =
        Provider.of<AudYoFloUiModel>(context, listen: false);

    // Update this widget if there is a notification in the uiModel
    return Consumer<AudYoFloUiModel>(builder: (context, notifier, child) {
      return AudYoFloStarterNTopLevelWidget(
          cpId, tagDevices, callbackDeviceWidget);
    });
  }
}
