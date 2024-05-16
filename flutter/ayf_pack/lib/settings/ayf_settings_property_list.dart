import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import '../ayf_pack_local.dart';
import './ayf_settings_property_list_content.dart';

// This class defines the tab widget in the settings dialog. It uses the
// AudYoFloMultiComponentsPropertyOnChange base class to obtain the
// property references in the debug cache. It is configured in the derived class to
// combine multiple components and properties in one list. This list is then
// exposed in a sliver list.
// Note that widget as well as the state class are always re-allocated when the
// window is re-opended - since we have a window on top of the UI
class AudYoFloSettingsPropertyList extends StatefulWidget {
  final AudYoFloSettingsConfigurationSection secProps;
  bool dbgMessages;
  AudYoFloSettingsPropertyList(this.secProps, {this.dbgMessages = false}) {}
  @override
  State<StatefulWidget> createState() {
    return _AudYoFloSettingsPropertyListState();
  }
}

// State part of the widget. The list of components and properties is typically
// set before first calling the "localBuild" function.
class _AudYoFloSettingsPropertyListState
    extends State<AudYoFloSettingsPropertyList>
    with AudYoFloMultiComponentsPropertyOnChange {
  _AudYoFloSettingsPropertyListState();

  @override
  Widget build(BuildContext context) {
    // If the states are newly allocated the configuration must be done again
    if (configured == false) {
      resetMultiPropertySettings();
      for (var elm in widget.secProps.theSettings.entries) {
        // Any call to setComponentConsider will set the base class to state
        // configured = true
        setComponentConsider(elm.key);
      }
    }

    // Call the selector & future build process in the parent class
    return localBuild(context);
  }

  @override
  Widget innerWidget(BuildContext context) {
    // Here we end up in the core build function
    return AudYoFloSettingsPropertyListContent(
        latestResult, widget.dbgMessages);
  }

  // Callback to provide the property list for each component
  @override
  AudYoFloOneComponentSettings? requestedPropertiesList(
      JvxComponentIdentification cpId) {
    List<String> retLst = [];
    var elm = widget.secProps.theSettings.entries
        .firstWhereOrNull((element) => element.key == cpId);
    if (elm != null) {
      return elm.value;
    }
    return null;
  }
}
