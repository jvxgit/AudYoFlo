import 'package:provider/provider.dart';
import 'package:flutter/material.dart';
import '../ayf_pack_local.dart';

class AudYoFloComponentWidget extends StatefulWidget {
  const AudYoFloComponentWidget({Key? key}) : super(key: key);

  @override
  _AudYoFloComponentWidgetState createState() =>
      _AudYoFloComponentWidgetState();
}

class _AudYoFloComponentWidgetState extends State<AudYoFloComponentWidget> {
  String dropdownValueProcess = 'None';
  bool showDescriptor = false;
  String regExpr = '';

  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);
    AudYoFloDebugModel theDbgModel =
        Provider.of<AudYoFloDebugModel>(context, listen: false);
    JvxComponentIdentification cpTpSelected = JvxComponentIdentification();

    // =====================================================
    // Find the list of processes to be exposed in combo box
    // =====================================================
    List<String> entriesProcs =
        theBeCache.availProcessDescriptions(lst: ['None']);
    //dropdownValueProcess = 'None';
    dropdownValueProcess = entriesProcs.elementAt(theDbgModel.idSelectProc + 1);

    return Selector<AudYoFloBackendCache, int>(
        selector: (context, beModel) => theBeCache.componentSection.ssUpdateId,
        //shouldRebuild: (previous, next) => next ? false : true,
        shouldRebuild: (previous, next) => (next != previous) ? true : false,
        builder: (context, notifier, child) {
          //FernLiveBackendBridge theBridge = new FernLiveBackendBridge();
          return Container(
              color: Colors.white,
              child: Column(children: [
                Flexible(
                    flex: 0,
                    child: Row(
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        children: [
                          Flexible(flex: 1, child: Text('Processes')),
                          Flexible(
                            flex: 3,
                            child: DropdownButton<String>(
                              value: dropdownValueProcess,
                              icon: const Icon(Icons.arrow_downward),
                              isExpanded: true,
                              iconSize: 24,
                              elevation: 16,
                              style: const TextStyle(color: Colors.deepPurple),
                              underline: Container(
                                height: 2,
                                color: Colors.deepPurpleAccent,
                              ),
                              onChanged: (String? newValue) {
                                /*
                                theBeModel.updatePropertyViewer(
                                  theDbgModel.idSelectCp

                                )*/
                                setState(() {
                                  theDbgModel.idSelectCp =
                                      JvxComponentIdentification();
                                  // theComponent = theDbgModel.idSelectCp

                                  theDbgModel.selectNewComponent(
                                      theDbgModel.idSelectCp);
                                  theDbgModel.idSelectProc =
                                      entriesProcs.indexWhere(
                                          (element) => element == newValue);
                                  if (theDbgModel.idSelectProc >= 0) {
                                    theDbgModel.idSelectProc--;
                                  }
                                });
                              },
                              items: entriesProcs.map<DropdownMenuItem<String>>(
                                  (String value) {
                                return DropdownMenuItem<String>(
                                  value: value,
                                  child: Text(value),
                                );
                              }).toList(),
                            ),
                          ),
                          Flexible(flex: 1, child: Text('Components')),
                          Flexible(
                              flex: 4, child: AudYoFloDropdownComponents()),
                          Flexible(
                              flex: 2,
                              child: AudYoFloActiveTextField(
                                  showTextOnBuild: regExpr,
                                  haveCommandHistory: true,
                                  onEditingComplete: (String val) {
                                    setState(() {
                                      regExpr = val;
                                    });
                                  })),
                          Checkbox(
                            value: showDescriptor,
                            onChanged: (value) {
                              setState(() {
                                showDescriptor = !showDescriptor;
                              });
                            },
                          ),
                        ])),
                Flexible(
                  flex: 1,
                  child: AudYoFloPropertyGridWidget(
                      showDescriptor: showDescriptor, regExprShow: regExpr),
                )
              ]));
        });
    //Container(color: Colors.yellow)
    //])))
  }
}

// ===================================================================
/*
 * This would almost be realizable as a stateful widget BUT it is not since
 * we need to trigger an update on itself when clicking another component.
 */
class AudYoFloDropdownComponents extends StatefulWidget {
  AudYoFloDropdownComponents();

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloDropdownWidgetStates();
  }
}

class _AudYoFloDropdownWidgetStates extends State<AudYoFloDropdownComponents> {
  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);
    AudYoFloDebugModel theDbgModel =
        Provider.of<AudYoFloDebugModel>(context, listen: false);

    List<JvxComponentIdentification> lstComponents = [];
    lstComponents.clear();

    // Add the "none" component
    lstComponents.add(JvxComponentIdentification());
    lstComponents = theBeCache.findComponentsMatchProcess(
        theDbgModel.idSelectProc,
        lstIn: lstComponents);

    // If the component selected before is no longer available, here we have a mismatch
    // Therefore, we check and correct the slection before we show it
    theDbgModel.verifyComponentSelection(lstComponents);

    return LayoutBuilder(builder: (context, constraints) {
      return SizedBox(
          width: constraints.maxWidth,
          child: Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              Flexible(
                flex: 3,
                child: DropdownButton<JvxComponentIdentification>(
                  value: theDbgModel.idSelectCp,
                  icon: const Icon(Icons.arrow_downward),
                  isExpanded: true,
                  iconSize: 24,
                  elevation: 16,
                  style: const TextStyle(color: Colors.deepPurple),
                  underline: Container(
                    height: 2,
                    color: Colors.deepPurpleAccent,
                  ),
                  onChanged: (JvxComponentIdentification? newValue) {
                    if (newValue != null) {
                      // The setState is to update this combobox, the operation on the dbgModel
                      // will trigger the provider on the dbgModel to update the grid widget with all
                      // the properties
                      setState(() {
                        theDbgModel.selectNewComponent(newValue);
                      });
                    }
                  },
                  items: lstComponents
                      .map<DropdownMenuItem<JvxComponentIdentification>>(
                          (JvxComponentIdentification value) {
                    String showText = theBeCache.descriptionComponent(value);
                    return DropdownMenuItem<JvxComponentIdentification>(
                      value: value,
                      child: Text(showText),
                    );
                  }).toList(),
                ),
              ),
              Flexible(
                flex: 1,
                child: ElevatedButton(
                    style: ButtonStyle(backgroundColor:
                        MaterialStateProperty.resolveWith<Color>(
                      (Set<MaterialState> states) {
                        if (states.contains(MaterialState.pressed))
                          return Colors.grey;
                        return Colors.black; // Use the component's default.
                      },
                    )),
                    child: const Text('Refresh'),
                    // color: Colors.black,q
                    //child: Icon(Icons.clear),
                    onPressed: () {
                      setState(() {
                        theBeCache.invalidatePropertiesComponent(
                            theDbgModel.idSelectCp, [], false,
                            invalidateAll: true);
                        theDbgModel.selectNewComponent(theDbgModel.idSelectCp);
                      });
                    }),
              )
            ],
          ));
    });
  }
}
