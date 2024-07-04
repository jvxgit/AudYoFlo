import 'package:flutter/material.dart';
import '../../ayf_pack_local.dart';

class AudYoFloFileInputActiveDevicesWidget extends StatefulWidget {
  const AudYoFloFileInputActiveDevicesWidget(this.identT, {Key? key})
      : super(key: key);

  final JvxComponentIdentification identT;

  @override
  _AudYoFloFileInputActiveDevicesWidgetState createState() =>
      _AudYoFloFileInputActiveDevicesWidgetState();
}

class _AudYoFloFileInputActiveDevicesWidgetState
    extends State<AudYoFloFileInputActiveDevicesWidget>
    with AudYoFloOneTechnologyDevicesOnChange {
  List<AudYoFloOneDeviceSelectionOption>? devLst;
  ScrollController _controllerL = ScrollController();

  @override
  void updateDevices(List<AudYoFloOneDeviceSelectionOption>? devs) {
    devLst = devs;
  }

  @override
  Widget build(BuildContext context) {
    cpId = widget.identT;
    return localBuild(context);
  }

  @override
  Widget innerWidget(BuildContext context) {
    List<AudYoFloOneDeviceSelectionOptionWithId> activeDevices = [];
    if (devLst != null) {
      for (var elm in devLst!) {
        if (elm.devIdent.slotsubid >= 0) {
          AudYoFloOneDeviceSelectionOptionWithId newContent =
              AudYoFloOneDeviceSelectionOptionWithId(elm, activeDevices.length);
          activeDevices.add(newContent);
        }
      }
    }

    return LayoutBuilder(builder: (_, constraints) {
      return CustomScrollView(slivers: <Widget>[
        AudYoFloDeviceSelectionSliverAppBar(('Audio File Input Devices')),
        SliverToBoxAdapter(
          child: SizedBox(
              height: 150,
              child: Scrollbar(
                  // 1) https://www.youtube.com/watch?v=DbkIQSvwnZc
                  // 2) https://stackoverflow.com/questions/69853729/flutter-the-scrollbars-scrollcontroller-has-no-scrollposition-attached
                  scrollbarOrientation: ScrollbarOrientation.bottom,
                  // thumbVisibility: true,
                  controller: _controllerL,
                  child: CustomScrollView(
                      scrollDirection: Axis.horizontal,
                      controller: _controllerL,
                      slivers: [
                        SliverList(
                          delegate: SliverChildBuilderDelegate(
                              (BuildContext context, int index) {
                            Widget? sliv = Container(
                                color: Colors.blueGrey,
                                width: 300, //constraints.maxWidth / 4,
                                child: AudYoFloOneFileDeviceSliver(
                                    activeDevices[index],
                                    devLst![index].devIdent,
                                    null));
                            return sliv;
                          }, childCount: activeDevices.length),
                        )
                      ]))),

          //),
          //),
        )
      ]);
    });
  }
}
