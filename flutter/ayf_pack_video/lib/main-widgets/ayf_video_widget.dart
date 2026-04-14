import 'package:collection/collection.dart';
import 'package:ayf_pack/ayf_pack_common.dart';
import 'package:flutter/material.dart';
import 'package:pixel_buffer_texture/pixel_buffer_renderer.dart';

import 'dart:math';
import 'dart:core';

class AudYoFloMultiVideoWidget extends StatefulWidget {
  final JvxComponentIdentification cpId;
  final int aspectRatioW;
  final int aspectRatioH;
  final double margins;

  AudYoFloMultiVideoWidget(this.cpId,
      {this.aspectRatioW = 640, this.aspectRatioH = 480, this.margins = 8.0});

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloMultiVideoWidgetState();
  }
}

class _AudYoFloMultiVideoWidgetState extends State<AudYoFloMultiVideoWidget>
    with
        AudYoFloMultiComponentsPropertyOnChange,
        AudYoFloPropertyOnChangeGroups {
  AudYoFloPropertyOnChangeOneGroup oneGroupVideoWidget =
      AudYoFloPropertyOnChangeOneGroup([
    '/current_inputs',
  ]);

  ScrollController _myController = ScrollController();

  int get cache_status {
    assert(false);
    return 0;
  }

  AudYoFloOneComponentSettingsBackend mySettings =
      AudYoFloOneComponentSettingsBackend(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN,
              slotid: -1,
              slotsubid: -1),
          'tagPropertyVideoWidget',
          []);

  _AudYoFloMultiVideoWidgetState() {
    theGroups['videoWidget'] = oneGroupVideoWidget;

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

  @override
  Widget innerWidget(BuildContext context) {
    bool validData = false;
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
          validData = true;
        }
      }
    }

    return LayoutBuilder(builder: (context, constraints) {
      Widget? subWidget;
      if (validData) {
        String rendererId = widget.cpId.txt;
        // const String rendererArg = '{""}';
        String rendererArg = '';
        AudYoFloPropertySelectionListBackend? renderInputs =
            oneGroupVideoWidget!.extractPropSelectionList('/current_inputs');

        int numCols = 0;
        int numRows = 0;

        List<Widget> widgetsOneRow = [];
        List<Widget> rows = [];

        if (renderInputs != null) {
          int nWidgets = renderInputs.parpropms.entries.length;
          // nWidgets = 3; for testing only
          if (nWidgets > 0) {
            var tmp = sqrt(nWidgets.toDouble());
            numCols = tmp.ceil();
            tmp = nWidgets.toDouble() / numCols;
            numRows = tmp.ceil();

            if (numCols > 0) {
              var tmpW = constraints.maxWidth / numCols;
              var tmpH = constraints.maxHeight / numRows;

              var tmpWA = tmpH * widget.aspectRatioW / widget.aspectRatioH;
              var tmpHA = tmpW * widget.aspectRatioH / widget.aspectRatioW;
              if (tmpWA > tmpW) tmpH = tmpHA;
              if (tmpHA > tmpH) tmpW = tmpWA;

              int idxRowOld = 0;

              for (int idx = 0; idx < nWidgets; idx++) {
                int idxRow = idx ~/ numCols;
                if (idxRow != idxRowOld) {
                  Widget oneRow = Row(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: List.from(widgetsOneRow));
                  widgetsOneRow.clear();
                  rows.add(oneRow);
                  // New row started
                  idxRowOld = idxRow;
                }
                // int inRowCnt = idx % numCols;

                // Argument 1: Identify the mixer input
                var rendererCtxt = renderInputs.parpropms.entries[idx];

                // Argument 2: Identify the mixer target (install) property
                var propToken = "/rendering_target";

                // Argument 3..: Identify the databuffer size as used in the backend.
                var propsSize =
                    "sx=/inout/system/segmentsizex:sy=/inout/system/segmentsizey:f=/inout/system/dataformat:bs=/inout/system/framesize:fg=/inout/system/datasubformat:nb=/number_buffers";
                rendererArg = "$propToken:$propsSize";

                // ============================================================

                Widget theNewWidget = Padding(
                  padding: EdgeInsets.all(widget.margins),
                  child: PixelBufferRenderer(
                      width: (tmpW).floor() - 2 * widget.margins.floor(),
                      height: (tmpH).floor() - 2 * widget.margins.floor(),
                      /*
                      width: 640,
                      height: 480,
                      */
                      rendererId: rendererId,
                      rendererCtxt: rendererCtxt,
                      rendererArg: rendererArg),
                );
                /* For testing only:
                Widget theNewWidget =
                    Container(width: tmpW, height: tmpH, color: Colors.cyan);
                */
                widgetsOneRow.add(theNewWidget);
              }

              if (widgetsOneRow.isNotEmpty) {
                Widget oneRow = Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: List.from(widgetsOneRow));
                rows.add(oneRow);
                widgetsOneRow.clear();
              }

              // ============================================================
              // Now, design the subwidget
              subWidget = SizedBox(
                width: constraints.maxWidth,
                height: constraints.maxHeight,
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: rows,
                ),
              );
            }
          }
        }
      }
      return Container(color: Colors.black, child: subWidget);
    });
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
