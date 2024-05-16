import 'package:flutter/material.dart';

import 'package:collection/collection.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

// ======================================================================

// Define a callback to select one of the items. If the function returns a
// null the hint text is shown instead
typedef AudYoFloComponentSelectionOptionCallback
    = AudYoFloOneComponentSelectionOption? Function(
        List<AudYoFloOneComponentSelectionOption>);

// ======================================================================

// Widget to display a combo box with all components

class AudYoFloComponentSelectionOption extends StatelessWidget {
  // List of selections
  final List<AudYoFloOneComponentSelectionOption> pContent;

  // No selection hint
  final String noSelectionHint;

  // Callback to select one item right before build
  final AudYoFloComponentSelectionOptionCallback? cbSetSelectItem;

  // Constructor: important parameters passed as arguments
  AudYoFloComponentSelectionOption(
      {required this.pContent,
      required this.noSelectionHint,
      this.cbSetSelectItem});

  // Main build function
  @override
  Widget build(BuildContext context) {
    // The currently selected entry to be specified right at build function
    AudYoFloOneComponentSelectionOption? selInstance;
    if (cbSetSelectItem != null) {
      selInstance = cbSetSelectItem!(pContent);
    }

    // Return the dropdown menu
    return DropdownButton<AudYoFloOneComponentSelectionOption>(
        isExpanded: true,
        value: selInstance,
        icon: const Icon(Icons.arrow_downward),
        iconSize: 24,
        elevation: 16,
        hint: Text(noSelectionHint),
        style: const TextStyle(color: Colors.deepPurple),
        underline: Container(
          height: 2,
          color: Colors.deepPurpleAccent,
        ),
        onChanged: (AudYoFloOneComponentSelectionOption? newValue) {
          //setState(() {} <- I think we do not need this
        },
        items: pContent
            .map<DropdownMenuItem<AudYoFloOneComponentSelectionOption>>(
                (AudYoFloOneComponentSelectionOption value) {
          Widget? elementWidget;
          if (value.sids.length > 0) {
            String slots = intValueList2String(value.sids);
            elementWidget = Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  Expanded(child: Text(value.description)),
                  Padding(
                      padding: const EdgeInsets.all(8.0),
                      child: Container(
                          color: Colors.red,
                          child: Padding(
                              padding: const EdgeInsets.all(8.0),
                              child: Text(slots,
                                  style: TextStyle(color: Colors.white)))))
                ]);
          }
          if (elementWidget != null) {
            return DropdownMenuItem<AudYoFloOneComponentSelectionOption>(
              value: value,
              child: elementWidget,
            );
          } else {
            return DropdownMenuItem<AudYoFloOneComponentSelectionOption>(
              value: value,
              child: Text(value.description),
            );
          }
        }).toList());
  }
}

// ===========================================================================
class AudYoFloComponentSelectionComboBoxWidget extends StatefulWidget {
  final JvxComponentIdentification cpTp;
  AudYoFloComponentSelectionComboBoxWidget(this.cpTp);

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloComponentSelectionComboBox();
  }
}

class _AudYoFloComponentSelectionComboBox
    extends State<AudYoFloComponentSelectionComboBoxWidget> {
  List<AudYoFloOneComponentSelectionOption>? pContent;

  // AudYoFloStringWithId dropdownValuePrimDev = AudYoFloStringWithId('None', -1);
  @override
  Widget build(BuildContext context) {
    return Selector<AudYoFloBackendCache, int>(
        selector: (context, beModel) {
          int retVal = -1;
          // We are waiting for an update of the device list of the technology list
          MapEntry<JvxComponentIdentification, AudYoFloOneSelectionOptionEntry>?
              elm = beModel.deviceOptionMap.selectionOptionCacheMap.entries
                  .firstWhereOrNull((element) =>
                      (element.key.cpTp == widget.cpTp.cpTp) &&
                      (element.key.slotid == widget.cpTp.slotid));
          if (elm != null) {
            retVal = elm.value.options.ssUpdateId;
          }
          return retVal;
        },
        //shouldRebuild: (previous, next) => next ? false : true,
        shouldRebuild: (previous, next) => (next != previous) ? true : false,
        builder: (context, notifier, child) {
          AudYoFloBackendCache theBeCache =
              Provider.of<AudYoFloBackendCache>(context, listen: false);

          Future<int> triggerSelectionOptionList(
              JvxComponentIdentification tp) async {
            int errCode = jvxErrorType.JVX_NO_ERROR;
            pContent = await checkAndUpdateComponentOptions(theBeCache, tp);
            return errCode;
          }

          return FutureBuilder<int>(
              future: triggerSelectionOptionList(widget.cpTp),
              builder: (BuildContext context, AsyncSnapshot<int> snapshot) {
                if (snapshot.connectionState == ConnectionState.done) {
                  int errCode = jvxErrorType.JVX_ERROR_INTERNAL;
                  int? errCodePtr = snapshot.data;
                  if (errCodePtr != null) {
                    errCode = errCodePtr;
                  }

                  if (errCode == jvxErrorType.JVX_NO_ERROR) {
                    /*
                    if (pContent != null) {
                      for (var elm in pContent!) {
                        print('Found Device List: ${elm.descriptor}');
                      }
                    }
                    */
                  }
                }

                List<AudYoFloOneComponentSelectionOption> nnList = [];
                if (pContent != null) {
                  nnList = pContent!;
                }
                return AudYoFloComponentSelectionOption(
                    pContent: nnList, noSelectionHint: 'No item selected');
              });
        });
  }
}
