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

  // String regExpr = '';

  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);
    AudYoFloDebugModel theDbgModel =
        Provider.of<AudYoFloDebugModel>(context, listen: false);
    // JvxComponentIdentification cpTpSelected = JvxComponentIdentification();

    bool showDescriptor = theDbgModel.getShowDescriptor(theDbgModel.idSelectCp);
    bool showSelected = theDbgModel.getShowSelected(theDbgModel.idSelectCp);

    // =====================================================
    // Find the list of processes to be exposed in combo box
    // =====================================================
    List<String> entriesProcs =
        theBeCache.availProcessDescriptions(lst: ['None']);
    //dropdownValueProcess = 'None';
    if(theDbgModel.idSelectProc > entriesProcs.length)
    {
      theDbgModel.idSelectProc = -1;
    }
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
                              flex: 4,

                              // We add a callback here to detect the change of the current component to update the regexpr to search
                              child: AudYoFloDropdownComponents(OnChanged: () {
                                setState(() {});
                              })),
                          Flexible(
                              flex: 2,
                              child: AudYoFloActiveTextField(
                                  showTextOnBuild:
                                      theDbgModel.getRegExpressionShow(
                                          theDbgModel.idSelectCp),
                                  haveCommandHistory: true,
                                  onEditingComplete: (String val) {
                                    setState(() {
                                      theDbgModel.setRegExpressionShow(
                                          theDbgModel.idSelectCp, val);
                                    });
                                  })),
                          Tooltip(
                            message:
                                'Show and filter descriptor rather than descriptions',
                            child: Checkbox(
                              value: showDescriptor,
                              onChanged: (value) {
                                setState(() {
                                  theDbgModel.setShowDescriptor(
                                      theDbgModel.idSelectCp, !showDescriptor);
                                });
                              },
                            ),
                          ),
                          Tooltip(
                            message: 'Show only the selected properties',
                            child: Checkbox(
                              value: showSelected,
                              onChanged: (value) {
                                setState(() {
                                  theDbgModel.setShowSelected(
                                      theDbgModel.idSelectCp, !showSelected);
                                });
                              },
                            ),
                          ),
                        ])),
                Flexible(
                  flex: 1,
                  child: AudYoFloPropertyGridWidget(
                    showDescriptor: showDescriptor,
                    showSelected:
                        showSelected, /*, regExprShow: regExpr <- uses the regExpr from dbgModel */
                  ),
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
  void Function()? OnChanged;
  AudYoFloDropdownComponents({this.OnChanged});

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
    String ttipText = theDbgModel.idSelectCp.txt;

    return LayoutBuilder(builder: (context, constraints) {
      return SizedBox(
          width: constraints.maxWidth,
          child: Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              Flexible(
                flex: 3,
                child: Tooltip(
                  message: ttipText,
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

                        // However, we allow an optional callback in addition to simplify usage
                        if (widget.OnChanged != null) {
                          widget.OnChanged!();
                        }

                        setState(() {
                          theDbgModel.selectNewComponent(newValue);
                        });
                      }
                    },
                    items: lstComponents
                        .map<DropdownMenuItem<JvxComponentIdentification>>(
                            (JvxComponentIdentification value) {
                      String showText = theBeCache.descriptionComponent(value);
                      showText = "${showText}${value.txtshort}";
                      return DropdownMenuItem<JvxComponentIdentification>(
                        value: value,
                        child: Text(showText),
                      );
                    }).toList(),
                  ),
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
