import 'dart:math';

import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:clipboard/clipboard.dart';
import 'package:fixnum/fixnum.dart' as fn;

import '../ayf_pack_local.dart';

class AudYoFloPropertyGridWidget extends StatefulWidget {
  final bool showDescriptor;
  final String regExprShow;
  AudYoFloPropertyGridWidget(
      {this.showDescriptor = false, this.regExprShow = ''});

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloPropertyGridWidgetStates();
  }
}

class _AudYoFloPropertyGridWidgetStates
    extends State<AudYoFloPropertyGridWidget> {
  ScrollController _myController = ScrollController();
  // int lastIdBackendCache = 0;
  List<AudYoFloPropertyContainer> allProperties = [];

  @override
  Widget build(BuildContext context) {
    //
    // outside selector is to be triggered on a different component election in dbg model
    //

    return Selector<AudYoFloDebugModel, int>(
        selector: (context, dbgModel) {
          return dbgModel.ssUpdateCpId;
        },
        shouldRebuild: (previous, next) => (next != previous) ? true : false,
        builder: (context, notifier, child) {
          // ======================================================================
          // This function is called whenever the selected component has changed
          // The selected component is stored in the debug model. Therefore we use
          // a provider to listen to the debug model
          // ======================================================================
          AudYoFloDebugModel theDbgModel =
              Provider.of<AudYoFloDebugModel>(context, listen: false);

          // Obtain currently active component
          JvxComponentIdentification cpId = theDbgModel.idSelectCp;

          // Inside selector is to be triggered by a state change in the
          // backend cache, e.g., when the component state changed
          return Selector<AudYoFloBackendCache, int>(
              selector: (context, beCache) {
                // if property is not there, return a -1
                int retVal = -1;

                // We are waiting for an update of the properties <ssUpdateId>
                retVal = beCache.updateIdComponentInCache(cpId);

                // Return the current id
                return retVal;
              },

              // Return true if the id is a different one than it was before
              shouldRebuild: (previous, next) =>
                  (next != previous) ? true : false,

              // The builder
              builder: (context, notifier, child) {
                // ======================================================================
                // The builder needs to wait for an update of the prorties. Therefore
                // we use a Future builder
                // ======================================================================
                Future<int> triggerPropertyList(
                    JvxComponentIdentification tp) async {
                  int errCode = jvxErrorType.JVX_NO_ERROR;
                  List<AudYoFloPropertyContainer>? props;
                  // print(tp.txt);

                  // First part: get all existing properties
                  List<String>? lst;
                  lst = theDbgModel.be
                      .referencePropertyListInCache(theDbgModel.idSelectCp);
                  if (lst == null) {
                    errCode = await theDbgModel.be
                        .triggerUpdatePropertyListComponent(
                            theDbgModel.idSelectCp);
                    if (errCode == jvxErrorType.JVX_NO_ERROR) {
                      lst = theDbgModel.be
                          .referencePropertyListInCache(theDbgModel.idSelectCp);
                    }
                  }

                  if (errCode == jvxErrorType.JVX_NO_ERROR) {
                    if (lst != null) {
                      if (lst.isEmpty) {
                        allProperties = [];
                      } else {
                        props = theDbgModel.be
                            .referenceValidPropertiesComponents(
                                theDbgModel.idSelectCp, lst);

                        if (props != null) {
                          if (widget.regExprShow.isNotEmpty) {
                            RegExp regExp = RegExp(
                              widget.regExprShow,
                              caseSensitive: false,
                              multiLine: false,
                            );
                            List<AudYoFloPropertyContainer> filteredLst = [];
                            for (var elm in props) {
                              if (widget.showDescriptor) {
                                if (regExp.hasMatch(elm.descriptor)) {
                                  filteredLst.add(elm);
                                }
                              } else {
                                if (regExp.hasMatch(elm.description)) {
                                  filteredLst.add(elm);
                                }
                              }
                            }

                            props = filteredLst;
                          }
                        }

                        if (props == null) {
                          errCode = await theDbgModel.be
                              .triggerUpdatePropertiesComponent(
                                  theDbgModel.idSelectCp, lst);
                          if (errCode == jvxErrorType.JVX_NO_ERROR) {
                            props = theDbgModel.be
                                .referenceValidPropertiesComponents(
                                    theDbgModel.idSelectCp, lst);
                          }
                        }
                      }
                    }
                  }
                  if (props == null) {
                    allProperties = [];
                  } else {
                    allProperties = props;
                  }

                  return errCode;
                }

                // Then, run the future builder
                return FutureBuilder<int>(
                    future: triggerPropertyList(theDbgModel.idSelectCp),
                    builder:
                        (BuildContext context, AsyncSnapshot<int> snapshot) {
                      if (snapshot.connectionState == ConnectionState.done) {
                        // The future has been completed. Check the errorcode
                        int errCode = jvxErrorType.JVX_ERROR_INTERNAL;
                        int? errCodePtr = snapshot.data;
                        if (errCodePtr != null) {
                          errCode = errCodePtr;
                        }

                        if (errCode == jvxErrorType.JVX_NO_ERROR) {
                          // Looks good, future operated as desired
                          return GridView.builder(
                              //scrollDirection: Axis.vertical,
                              gridDelegate:
                                  const SliverGridDelegateWithMaxCrossAxisExtent(
                                      maxCrossAxisExtent: 350,
                                      mainAxisExtent: 100,
                                      crossAxisSpacing: 20,
                                      mainAxisSpacing: 20),
                              itemBuilder: (context, index) {
                                return AudYoFloPropertySingleWidget(
                                  pContentShow: allProperties,
                                  idProp: index,
                                  showDescriptor: widget.showDescriptor,
                                );
                              },
                              itemCount: allProperties.length,
                              controller: _myController,
                              shrinkWrap: true);
                          //return Container(
                          //    color: Colors.green, child: Center(child: Text('Success')));
                        }

                        String errTxt =
                            jvxErrorTypeEInt.toStringSingle(errCode);
                        return Container(

                            // Display an error message
                            color: Colors.red,
                            child: Center(child: Text('Error = $errTxt')));
                      }

                      // Still waiting here
                      return Container(
                          color: Colors.grey,
                          child: Center(child: Text('Waiting')));
                    });
              });
        });
  }
}

// ============================================================

// Widget to show a single property. It is stateful since it stores the
// "showRealtime" status.
class AudYoFloPropertySingleWidget extends StatefulWidget {
  final List<AudYoFloPropertyContainer?> pContentShow;
  final int idProp;
  final bool showDescriptor;
  AudYoFloPropertySingleWidget(
      {required this.pContentShow,
      required this.idProp,
      required this.showDescriptor});

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloPropertySingleWidgetState();
  }
}

class _AudYoFloPropertySingleWidgetState
    extends State<AudYoFloPropertySingleWidget> {
  @override
  Widget build(BuildContext context) {
    AudYoFloPropertyContainer? propDescr;
    if (widget.idProp < widget.pContentShow.length) {
      AudYoFloPropertyContainer? ref =
          widget.pContentShow.elementAt(widget.idProp);
      if (ref != null) {
        propDescr = ref;
      }
    }
    return AudYoFloPropertyShow(
        propDescr: propDescr, showDescriptor: widget.showDescriptor);
  }
}

// =============================================================
class ayfSelListNOpts {
  int selIdx = 0;
  bool modeShowOffset = false;
}

enum ayfSpecificOperation {
  AYF_SPECIFIC_PROPERTY_SELECTIONLIST_NONE,
  AYF_SPECIFIC_PROPERTY_SELECTIONLIST_NOPTS
}

class AudYoFloPropertyShow extends StatefulWidget {
  final AudYoFloPropertyContainer? propDescr;
  final bool showDescriptor;
  AudYoFloPropertyShow({required this.propDescr, this.showDescriptor = false});

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloPropertyShowState();
  }
}

class _AudYoFloPropertyShowState extends State<AudYoFloPropertyShow> {
  bool updateRt = false;
  bool requirePropUpdate = false;
  @override
  Widget build(BuildContext context) {
    return Selector<AudYoFloPeriodicNotifier, int>(
        selector: (context, periodic) {
      return periodic.cntSlow;
    },
        // Return true if the id is a different one than it was before
        shouldRebuild: (previous, next) {
      if (((previous != next) && updateRt)) {
        if (!requirePropUpdate) {
          print('Update property with <${widget.propDescr!.descriptor}');
          requirePropUpdate = true;
          return true;
        }
      }
      return false;
    }, builder: (context, notifier, child) {
      String description = 'not-found';
      String ttip = 'not-found';
      String currentValueText = 'not-found';
      Widget? secondaryWidget;

      if (widget.propDescr != null) {
        description = widget.propDescr!.description;
        currentValueText = widget.propDescr!.toString();
        if (widget.propDescr is AudYoFloPropertyContentBackend) {
          AudYoFloPropertyContentBackend propNat =
              widget.propDescr! as AudYoFloPropertyContentBackend;
          String helpText = propNat.toHelperString();
          ttip = '->';
          if (helpText.isNotEmpty) {
            ttip += '$helpText :';
          }
          ttip += '${propNat.origin}';

          if (requirePropUpdate) {
            AudYoFloBackendCache theBeCache =
                Provider.of<AudYoFloBackendCache>(context, listen: false);

            JvxComponentIdentification cpId = widget.propDescr!.assCpIdent;
            List<String> props = [widget.propDescr!.descriptor];
            updateSingleProperty(theBeCache, cpId, props);
          }
          secondaryWidget = AudYoFloPropertyDetailView(propNat);
        } else {
          assert(false);
        }
      }

      Widget contentWidget = Text(currentValueText);
      ayfSpecificOperation specOper =
          ayfSpecificOperation.AYF_SPECIFIC_PROPERTY_SELECTIONLIST_NONE;

      if (widget.propDescr != null) {
        switch (widget.propDescr!.jvxFormat) {
          case jvxDataFormatEnum.JVX_DATAFORMAT_SELECTION_LIST:
            if (widget.propDescr! is AudYoFloPropertySelectionListBackend) {
              AudYoFloPropertySelectionListBackend selLst =
                  widget.propDescr! as AudYoFloPropertySelectionListBackend;

              ayfSelListNOpts nOpts = ayfSelListNOpts();

              // If we see a selection with multiple options, we
              // need to allow to select the options in UI as well.
              // For this purpose, we can add additional data in the property given the
              // map for specific options. Here, we can load and store the data as required.
              // The map will be deleted if the property descriptor is reported to be updated
              if (selLst.parpropms.num > 1) {
                specOper = ayfSpecificOperation
                    .AYF_SPECIFIC_PROPERTY_SELECTIONLIST_NOPTS;

                // Check if an offset was stored before
                var attMap = widget.propDescr?.attachedSpecific;
                if (attMap != null) {
                  var attMapEntry = attMap.entries.firstWhereOrNull(
                      (element) => element.key == 'gridWidgetSelLstNOpts');
                  if (attMapEntry != null) {
                    if (attMapEntry.value is ayfSelListNOpts) {
                      nOpts = attMapEntry.value as ayfSelListNOpts;
                    }
                  }
                }
                if (nOpts.selIdx >= selLst.parpropms.num) {
                  nOpts.selIdx = 0;
                }
              }
              // ============================================================

              Widget? newWidget;
              if (nOpts.modeShowOffset && selLst.parpropms.num > 1) {
                contentWidget = AudYoFloActiveTextField(
                  onEditingComplete: (String val) {
                    // ============================================================
                    // This callback is reached if we have edited a new offset value in case of a
                    // selection list with multiple options
                    int? selIdx = int.tryParse(val);
                    if (selIdx != null) {
                      setState(() {
                        var attMap = widget.propDescr?.attachedSpecific;
                        if (attMap != null) {
                          ayfSelListNOpts nOpts = ayfSelListNOpts();
                          var attMapEntry = attMap.entries.firstWhereOrNull(
                              (element) =>
                                  element.key == 'gridWidgetSelLstNOpts');
                          if (attMapEntry != null) {
                            if (attMapEntry.value is ayfSelListNOpts) {
                              nOpts = attMapEntry.value as ayfSelListNOpts;
                            }
                          }
                          nOpts.modeShowOffset = false;
                          nOpts.selIdx = selIdx;
                          attMap['gridWidgetSelLstNOpts'] = nOpts;
                        }
                      });
                    }
                    // ============================================================
                  },
                  showTextOnBuild: nOpts.selIdx.toString(),
                  haveCommandHistory: false,
                );
              } else {
                newWidget = AudYoFloPropListComboBoxWidget(
                  selLstBe: selLst,
                  height: 40,
                  offset: nOpts.selIdx,
                  numCharsMax: 20,
                );

                contentWidget = newWidget;
              }
              // else {
              // contentWidget = AudYoFloSimpleTextField(
              //    propBe: selLst.parpropms, height: 40);
              //}
            }
            break;

          case jvxDataFormatEnum.JVX_DATAFORMAT_STRING_LIST:
            if (widget.propDescr! is AudYoFloPropertyMultiStringBackend) {
              AudYoFloPropertyMultiStringBackend strLst =
                  widget.propDescr! as AudYoFloPropertyMultiStringBackend;
              contentWidget =
                  AudYoFloPropListComboBoxWidget(strLstBe: strLst, height: 35);
            }
            break;
          case jvxDataFormatEnum.JVX_DATAFORMAT_DATA:
          /*
        case jvxDataFormatEnum.JVX_DATAFORMAT_DOUBLE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_FLOAT:
        */
          case jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE:
          case jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE:
          case jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE:
          case jvxDataFormatEnum.JVX_DATAFORMAT_8BIT:
          case jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE:
          case jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE:
          case jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE:
          case jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT:
          case jvxDataFormatEnum.JVX_DATAFORMAT_SIZE:
          case jvxDataFormatEnum.JVX_DATAFORMAT_STRING:
          case jvxDataFormatEnum.JVX_DATAFORMAT_VALUE_IN_RANGE:
            contentWidget =
                AudYoFloSimpleTextField(propBe: widget.propDescr!, height: 35);
            break;
          default:
            break;
        }
      }

      String tooltipShow = '';
      String whatToShow = '';
      if (widget.propDescr != null) {
        tooltipShow = widget.propDescr!.descriptor;
        whatToShow = description;

        if (widget.showDescriptor) {
          whatToShow = widget.propDescr!.descriptor;
          tooltipShow = description;
        }
      }

      return GestureDetector(
        onSecondaryTap: () {
          String txt = '';
          if (widget.propDescr != null) {
            txt = widget.propDescr!.descriptor;
          }
          FlutterClipboard.copy(txt)
              .then((value) => print('Copied $txt to clipboard'));
        },
        onSecondaryLongPress: () {
          // ============================================================
          // We add a callbacl in case a long press has occurred. Then, we can configure specific
          // aspects
          if (specOper ==
              ayfSpecificOperation.AYF_SPECIFIC_PROPERTY_SELECTIONLIST_NOPTS) {
            // Toggle mode
            setState(() {
              var attMap = widget.propDescr?.attachedSpecific;
              if (attMap != null) {
                ayfSelListNOpts nOpts = ayfSelListNOpts();
                var attMapEntry = attMap.entries.firstWhereOrNull(
                    (element) => element.key == 'gridWidgetSelLstNOpts');
                if (attMapEntry != null) {
                  if (attMapEntry.value is ayfSelListNOpts) {
                    nOpts = attMapEntry.value as ayfSelListNOpts;
                  }
                }
                nOpts.modeShowOffset = !nOpts.modeShowOffset;
                attMap['gridWidgetSelLstNOpts'] = nOpts;
              }
            });
          }
        },
        onDoubleTap: () {
          String txt = '';
          if (widget.propDescr != null) {
            txt = widget.propDescr!.toString();
          }
          FlutterClipboard.copy(txt)
              .then((value) => print('Copied $txt to clipboard'));
        },
        child: Container(
            color: Colors.lightBlue,
            child: DecoratedBox(
                decoration: BoxDecoration(
                    color: Colors.grey[350],
                    border: Border.all(
                      color: Colors.grey,
                      width: 4,
                    ),
                    borderRadius: BorderRadius.circular(15)),
                child: Column(children: [
                  Container(
                      height: 100,
                      //padding: EdgeInsets.all(3),
                      child: Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Column(
                            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                            children: [
                              Flexible(
                                flex: 1,
                                child: Row(
                                    mainAxisAlignment:
                                        MainAxisAlignment.spaceBetween,
                                    children: [
                                      Flexible(
                                          flex: 1,
                                          child: Tooltip(
                                            child: Text(whatToShow),
                                            message: tooltipShow,
                                          )), // description
                                      Flexible(flex: 1, child: secondaryWidget!)
                                    ]),
                              ),
                              Flexible(
                                  flex: 1,
                                  child: Row(
                                    mainAxisAlignment: MainAxisAlignment.start,
                                    children: [
                                      Flexible(
                                          flex: 5,
                                          child: Tooltip(
                                              message: ttip,
                                              child: contentWidget)),
                                      Flexible(
                                          flex: 1,
                                          child: Checkbox(
                                            value: updateRt,
                                            onChanged: (value) {
                                              setState(() {
                                                updateRt = value!;
                                              });
                                            },
                                          ))
                                    ],
                                  ))
                            ]),
                      )),
                ]))),
      );
    });
  }

  void updateSingleProperty(AudYoFloBackendCache theBeCache,
      JvxComponentIdentification cpId, List<String> props) async {
    theBeCache.invalidatePropertiesComponent(cpId, props, true);
    await theBeCache.triggerUpdatePropertiesComponent(cpId, props);
    setState(() {
      requirePropUpdate = false;
    });
  }
}

class AudYoFloPropertyDetailView extends StatelessWidget {
  final AudYoFloPropertyContentBackend prop;

  AudYoFloPropertyDetailView(this.prop);

  @override
  Widget build(BuildContext context) {
    // TODO: implement build
    String txtAllowedState = "";
    if (prop.allowedStateMask.and(
            prop.allowedStateMask.fld, fn.Int32(jvxState.JVX_STATE_INIT)) !=
        0) {
      txtAllowedState += "i";
    } else {
      txtAllowedState += "-";
    }
    if (prop.allowedStateMask.and(
            prop.allowedStateMask.fld, fn.Int32(jvxState.JVX_STATE_SELECTED)) !=
        0) {
      txtAllowedState += "s";
    } else {
      txtAllowedState += "-";
    }
    if (prop.allowedStateMask.and(
            prop.allowedStateMask.fld, fn.Int32(jvxState.JVX_STATE_ACTIVE)) !=
        0) {
      txtAllowedState += "a";
    } else {
      txtAllowedState += "-";
    }
    if (prop.allowedStateMask.and(
            prop.allowedStateMask.fld, fn.Int32(jvxState.JVX_STATE_PREPARED)) !=
        0) {
      txtAllowedState += "p";
    } else {
      txtAllowedState += "-";
    }

    if (prop.allowedStateMask.and(prop.allowedStateMask.fld,
            fn.Int32(jvxState.JVX_STATE_PROCESSING)) !=
        0) {
      txtAllowedState += "r";
    } else {
      txtAllowedState += "-";
    }

    String txtAccess = "";
    switch (prop.accessType) {
      case jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE:
        txtAccess = "fu";
        break;
      case jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT:
        txtAccess = "rwc";
        break;
      case jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_READ_ONLY:
        txtAccess = "ro";
        break;
      case jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_READ_ONLY_ON_START:
        txtAccess = "ros";
        break;
      case jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_WRITE_ONLY:
        txtAccess = "wo";
        break;
    }
    return Row(mainAxisAlignment: MainAxisAlignment.end, children: [
      Flexible(
          flex: 5,
          child: Tooltip(
              message: "Allowed State Mask",
              child: Container(
                  color: Colors.green, child: Text(txtAllowedState)))),
      Spacer(flex: 1),
      Flexible(
          flex: 5,
          child: Tooltip(
              message: "Property Access Type",
              child: Container(color: Colors.blue, child: Text(txtAccess))))
    ]);
  }
}
