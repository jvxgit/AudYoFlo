import 'package:flutter/material.dart';
import '../../ayf_pack_local.dart';
import 'package:provider/provider.dart';
import 'package:collection/collection.dart';

class AudYoFloOneFileDeviceSliver extends StatelessWidget
    with AudYoFloOneComponentPropertyOnChange, AudYoFloPropertyOnChangeGroups {
  final AudYoFloOneDeviceSelectionOptionWithId option;
  final JvxComponentIdentification cpIdConf;
  final AudYoFloSelectionComponentUi? reportTarget;
  AudYoFloBackendCache? theBeCache;

  AudYoFloPropertyOnChangeOneGroup oneGroupThisWidget =
      AudYoFloPropertyOnChangeOneGroup([
    '/restart',
    '/fwd10',
    '/bwd10',
    '/close',
    '/togpause',
    '/length_sec',
    '/progress_percent',
    '/file_status',
    '/system/src_name'
  ]);

  AudYoFloOneFileDeviceSliver(this.option, this.cpIdConf, this.reportTarget) {
    super.cpId = cpIdConf;
    theGroups['thisWidget'] = oneGroupThisWidget;

    createList();
  }

  @override
  List<String> requestedPropertiesList() {
    return propsAllGroups;
  }

  @override
  void reconstructProperties(List<AudYoFloPropertyContainer> props) {
    reconstructProperyReferences(props);
  }

  @override
  Widget build(BuildContext context) {
    theBeCache = Provider.of<AudYoFloBackendCache>(context, listen: false);
    // Produce Device Name

    /*
    AudYoFloUiModel theUiCache =
        Provider.of<AudYoFloUiModel>(context, listen: false);
        */
    return localBuild(context);
  }

  @override
  Widget innerWidget(BuildContext context) {
    bool updateRealtime = false;
    // String showDeviceName = option.option.description;

    // Show device capabailities
    Widget icon = Icon(Icons.audio_file_outlined, size: 20);
    Widget? statusWidget;

    if (option.option.state
        .bitTest(jvxStateShiftEnum.JVX_STATE_SELECTED.index)) {
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.red, child: Icon(Icons.warning, size: 20)));
    } else if (option.option.state
        .bitTest(jvxStateShiftEnum.JVX_STATE_ACTIVE.index)) {
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.green, child: Icon(Icons.stop, size: 20)));
    } else if (option.option.state
        .bitTest(jvxStateShiftEnum.JVX_STATE_PREPARED.index)) {
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.yellow, child: Icon(Icons.pause, size: 20)));
    } else if (option.option.state
        .bitTest(jvxStateShiftEnum.JVX_STATE_PROCESSING.index)) {
      updateRealtime = true;
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.blue, child: Icon(Icons.play_arrow, size: 20)));
    } else {
      statusWidget = Padding(
          padding: const EdgeInsets.all(8.0),
          child: Container(
              color: Colors.grey,
              child: Icon(Icons.check_box_outline_blank, size: 20)));
    }

    // Extract filename
    String name = '';
    AudYoFloPropertyOnChangeOneGroup? groupThisWidget;
    MapEntry<String, AudYoFloPropertyOnChangeOneGroup>? groupEntryThis =
        theGroups.entries
            .firstWhereOrNull((element) => element.key == 'thisWidget');
    if (groupEntryThis != null) {
      groupThisWidget = groupEntryThis.value;
    }
    if (groupThisWidget != null) {
      var propStr = groupThisWidget.extractPropString('/system/src_name');
      if (propStr != null) {
        name = propStr.value;
      }
    }

    // Extract icon sizes
    double iconSize = 30;
    double boxHeight = 40;
    double boxWidth = 50;

    AudYoFloPropertyMultiContentBackend? propClose;
    if (groupThisWidget != null) {
      propClose = groupThisWidget.extractPropMultiContent('/close');
    }

    AudYoFloPropertyMultiContentBackend? propRestart;
    if (groupThisWidget != null) {
      propRestart = groupThisWidget.extractPropMultiContent('/restart');
    }

    AudYoFloPropertyMultiContentBackend? propFwd;
    if (groupThisWidget != null) {
      propFwd = groupThisWidget.extractPropMultiContent('/fwd10');
    }

    AudYoFloPropertyMultiContentBackend? propBwd;
    if (groupThisWidget != null) {
      propBwd = groupThisWidget.extractPropMultiContent('/bwd10');
    }

    AudYoFloPropertyMultiContentBackend? propTogPause;
    if (groupThisWidget != null) {
      propTogPause = groupThisWidget.extractPropMultiContent('/togpause');
    }

    double progressValue = 0;
    AudYoFloPropertyMultiContentBackend? propProgress;
    if (groupThisWidget != null) {
      propProgress =
          groupThisWidget.extractPropMultiContent('/progress_percent');
      if (propProgress != null) {
        progressValue = propProgress.fld[0];
      }
    }

    AudYoFloPropertySelectionListBackend? propFileStatus;
    if (groupThisWidget != null) {
      propFileStatus = groupThisWidget.extractPropSelectionList('/file_status');
    }

    IconData idat = Icons.stop;
    if (propFileStatus != null) {
      if (propFileStatus.selection.bitTest(1)) {
        // If playing..
        idat = Icons.pause;
      } else if (propFileStatus.selection.bitTest(2)) {
        idat = Icons.play_arrow;
      } else if (propFileStatus.selection.bitTest(3)) {
        idat = Icons.warning;
      }
    }

    return GestureDetector(
      onTap: () {
        if (reportTarget != null) {
          reportTarget!.reportSelectionComponentUi(option.option.devIdent);
        }
      },
      child: Padding(
        padding: const EdgeInsets.all(8.0),
        child: Container(
          color: Colors.grey,
          child: Card(
              child: Column(
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: [
                Row(children: [
                  Expanded(
                      child: Padding(
                          padding: const EdgeInsets.all(8.0),
                          child: Text(name /*showDeviceName*/))),
                  SizedBox(
                      height: boxHeight,
                      width: boxWidth,
                      child: FittedBox(
                          child: FloatingActionButton(
                              onPressed: () {
                                if (propClose != null) {
                                  propClose.fld[0] = 1;
                                  if (theBeCache != null) {
                                    List<String> propContents = ['/close'];
                                    theBeCache!.triggerSetProperties(
                                        cpIdConf, propContents);
                                  }
                                }
                              },
                              backgroundColor: Colors.black,
                              child: Icon(Icons.delete, size: iconSize)))),
                ]),
                Row(
                    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    //mainAxisAlignment: MainAxisAlignment.start, children: [

                    children: [
                      SizedBox(
                          height: boxHeight,
                          width: boxWidth,
                          child: FittedBox(
                              child: FloatingActionButton(
                                  onPressed: () {
                                    if (propRestart != null) {
                                      propRestart.fld[0] = 1;
                                      if (theBeCache != null) {
                                        theBeCache!.triggerSetProperties(
                                            cpIdConf,
                                            [propRestart.parpropc.descriptor]);
                                      }
                                    }
                                  },
                                  backgroundColor: Colors.black,
                                  child: Icon(Icons.keyboard_return,
                                      size: iconSize)))),
                      SizedBox(
                          height: boxHeight,
                          width: boxWidth,
                          child: FittedBox(
                              child: FloatingActionButton(
                                  onPressed: () {
                                    if (propBwd != null) {
                                      propBwd.fld[0] = 1;
                                      if (theBeCache != null) {
                                        theBeCache!.triggerSetProperties(
                                            cpIdConf,
                                            [propBwd.parpropc.descriptor]);
                                      }
                                    }
                                  },
                                  backgroundColor: Colors.black,
                                  child: Icon(Icons.fast_rewind,
                                      size: iconSize)))),
                      SizedBox(
                          height: boxHeight,
                          width: boxWidth,
                          child: FittedBox(
                              child: FloatingActionButton(
                                  onPressed: () {
                                    if (propFwd != null) {
                                      propFwd.fld[0] = 1;
                                      if (theBeCache != null) {
                                        theBeCache!.triggerSetProperties(
                                            cpIdConf,
                                            [propFwd.parpropc.descriptor]);
                                      }
                                    }
                                  },
                                  backgroundColor: Colors.black,
                                  child: Icon(Icons.fast_forward,
                                      size: iconSize)))),
                      SizedBox(
                          height: boxHeight,
                          width: boxWidth,
                          child: FittedBox(
                              child: FloatingActionButton(
                                  onPressed: () {
                                    if (propTogPause != null) {
                                      propTogPause.fld[0] = 1;
                                      if (theBeCache != null) {
                                        theBeCache!.triggerSetProperties(
                                            cpIdConf,
                                            [propTogPause.parpropc.descriptor]);
                                      }
                                    }
                                  },
                                  backgroundColor: Colors.black,
                                  child: Icon(idat, size: iconSize)))),
                      statusWidget
                    ]),
                Row(mainAxisAlignment: MainAxisAlignment.center, children: [
                  Expanded(
                      child: Padding(
                    padding: const EdgeInsets.all(8.0),
                    child: AudYoFloProgressIndicator(cpIdConf,
                        '/progress_percent', updateRealtime, progressValue),
                  ))
                ]),
              ])),
        ),
      ),
    );
  }
}
