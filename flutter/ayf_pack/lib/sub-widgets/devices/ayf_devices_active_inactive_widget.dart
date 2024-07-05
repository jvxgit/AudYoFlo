import 'package:provider/provider.dart';
import 'package:flutter/material.dart';
import '../../ayf_pack_local.dart';

import 'package:collection/collection.dart';

class AudYoFloDeviceSelectionSliverAppBar extends StatelessWidget {
  final String text;
  AudYoFloDeviceSelectionSliverAppBar(this.text);

  @override
  Widget build(BuildContext context) {
    return SliverAppBar(
      title: Center(
          child: Text(
        text,
        style: TextStyle(color: Colors.white, fontSize: 16),
      )),
      backgroundColor: Colors.blueGrey,
      pinned: false,
      stretch: false,
      onStretchTrigger: () async {
        print('Hello');
      },
      floating: true,
      snap: true,
      //expandedHeight: 200
    );
  }
}

class deviceSelectionDragData {
  deviceSelectionDragData(this.isActive, this.id);
  bool isActive = false;
  int id = -1;
}

class AudYoFloDeviceActiveInactiveWidget extends StatefulWidget {
  final List<AudYoFloOneDeviceSelectionOption>? pContent;
  final JvxComponentIdentification cpId;
  final AudYoFloSelectionComponentUi? reportWidget;
  final String tagDevices;
  final Widget? Function(
      AudYoFloOneDeviceSelectionOptionWithId option,
      bool fromActiveList,
      AudYoFloSelectionComponentUi?) callbackActiveInactiveDeviceSliver;
  AudYoFloDeviceActiveInactiveWidget(this.pContent, this.cpId, this.tagDevices,
      this.callbackActiveInactiveDeviceSliver,
      {this.reportWidget});

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloDeviceActiveInactiveWidgetStates();
  }
}

class _AudYoFloDeviceActiveInactiveWidgetStates
    extends State<AudYoFloDeviceActiveInactiveWidget> {
  ScrollController _controllerL = ScrollController();
  ScrollController _controllerR = ScrollController();
  List<AudYoFloOneDeviceSelectionOptionWithId> inactiveDevices = [];
  List<AudYoFloOneDeviceSelectionOptionWithId> activeDevices = [];

  void createActiveInactiveLists() {
    inactiveDevices.clear();
    activeDevices.clear();
    if (widget.pContent != null) {
      for (var elm in widget.pContent!) {
        if (elm.devIdent.slotsubid >= 0) {
          AudYoFloOneDeviceSelectionOptionWithId newContent =
              AudYoFloOneDeviceSelectionOptionWithId(elm, activeDevices.length);
          activeDevices.add(newContent);
        } else {
          AudYoFloOneDeviceSelectionOptionWithId newContent =
              AudYoFloOneDeviceSelectionOptionWithId(
                  elm, inactiveDevices.length);
          inactiveDevices.add(newContent);
        }
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    createActiveInactiveLists();
    //return Container(color: Colors.blue, child: Text('Success'));
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);

    return Row(children: [
      Flexible(
        flex: 1,
        child: DragTarget<deviceSelectionDragData>(
            builder: (context, candidateData, rejectedData) => CustomScrollView(
                  controller: _controllerL,
                  slivers: [
                    AudYoFloDeviceSelectionSliverAppBar(
                        ('Inactive ${widget.tagDevices}')),
                    SliverList(
                      delegate: SliverChildBuilderDelegate(
                          (BuildContext context, int index) {
                        Widget? sliv =
                            widget.callbackActiveInactiveDeviceSliver(
                                inactiveDevices[index], false, null);
                        return sliv;
                      }, childCount: inactiveDevices.length),
                    )
                  ],
                ),
            onWillAccept: (data) {
              if (data != null) {
                if (data.isActive) {
                  return true;
                }
                return false;
              }
              return false;
            },
            onAccept: (data) {
              // data.id is the index in the <inactiveDevices>
              // We then get the index in device option list (active AND inactive)
              // in <selId.option.optionIdx>
              // The "target" slot is DONTCARE which means "next available"
              AudYoFloOneDeviceSelectionOptionWithId selId =
                  activeDevices[data.id];
              JvxComponentIdentification cpDeactivate = selId.option.devIdent;
              theBeCache.triggerDeactivateComponent(cpDeactivate);
            }),
      ),
      Flexible(
        flex: 1,
        child: DragTarget<deviceSelectionDragData>(
            builder: (context, candidateData, rejectedData) => CustomScrollView(
                  controller: _controllerR,
                  slivers: [
                    AudYoFloDeviceSelectionSliverAppBar(
                        'Active ${widget.tagDevices}'),
                    SliverList(
                      delegate: SliverChildBuilderDelegate(
                          (BuildContext context, int index) {
                        // Prepare the drag data

                        Widget? sliv =
                            widget.callbackActiveInactiveDeviceSliver(
                                activeDevices[index],
                                true,
                                widget.reportWidget);
                        return sliv;
                      }, childCount: activeDevices.length),
                    )
                  ],
                ),
            onWillAccept: (data) {
              if (data != null) {
                if (!data.isActive) {
                  return true;
                }
                return false;
              }
              return false;
            },
            onAccept: (data) {
              // data.id is the index in the <inactiveDevices>
              // We then get the index in device option list (active AND inactive)
              // in <selId.option.optionIdx>
              // The "target" slot is DONTCARE which means "next available"
              AudYoFloOneDeviceSelectionOptionWithId selId =
                  inactiveDevices[data.id];
              JvxComponentIdentification cpActivate = selId.option.devIdent;
              cpActivate.slotsubid = jvxSize.JVX_SIZE_DONTCARE;
              theBeCache.triggerActivateComponent(
                  cpActivate, selId.option.optionIdx,
                  selectBeforeActivate: true); // Immediately activate!
            }),
      ),
    ]);
  }
}
