import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

class AudYoFloPlaypannel extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    AudYoFloUiModel theUiModel =
        Provider.of<AudYoFloUiModel>(context, listen: false);
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);
    return SizedBox(
        height: theUiModel.heightBottomNavBar,
        child: Container(
          color: Colors.grey,
          child: Container(
            height: theUiModel.heightBottomNavBar * 0.8,
            child: Row(
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                DecoratedBox(
                    decoration: BoxDecoration(
                        color: Colors.blueGrey,
                        border: Border.all(),
                        borderRadius: BorderRadius.circular(5),
                        boxShadow: [
                          BoxShadow(
                              color: Colors.white,
                              offset: Offset(-2, -2),
                              blurRadius: 3),
                          BoxShadow(
                              color: Colors.black,
                              offset: Offset(10, 10),
                              blurRadius: 8)
                        ]),
                    child: Padding(
                        padding: EdgeInsets.all(5),
                        child: Row(children: [
                          SizedBox(
                              width: theUiModel.heightBottomNavBar,
                              height: theUiModel.heightBottomNavBar / 2,
                              child: FittedBox(
                                  child: FloatingActionButton(
                                      heroTag: UniqueKey(),
                                      child: Icon(Icons.fiber_manual_record,
                                          size: theUiModel.heightBottomNavBar /
                                              2),
                                      onPressed: () {},
                                      backgroundColor: Colors.black))),
                          SizedBox(
                              width: theUiModel.heightBottomNavBar * 2,
                              height: theUiModel.heightBottomNavBar * 0.7,
                              child: FittedBox(
                                  child: Selector<AudYoFloBackendCache, int>(
                                      selector: (context, notifyHandle) {
                                        return notifyHandle
                                            .sequencerSection.ssUpdateId;
                                      },
                                      //shouldRebuild: (previous, next) => next ? false : true,
                                      shouldRebuild: (previous, next) =>
                                          (next != previous) ? true : false,
                                      builder: (context, notifier, child) {
                                        jvxSequencerStatusEnum stat = theBeCache
                                            .sequencerSection.seqStatus;

                                        if (stat ==
                                            jvxSequencerStatusEnum
                                                .JVX_SEQUENCER_STATUS_NONE) {
                                          if (theBeCache.sequencerSection
                                              .processesReady) {
                                            return FloatingActionButton(
                                                heroTag: UniqueKey(),
                                                child: Icon(Icons.play_arrow,
                                                    size: theUiModel
                                                            .heightBottomNavBar /
                                                        2),
                                                onPressed: () {
                                                  theBeCache
                                                      .triggerStartSequencer();
                                                },
                                                backgroundColor: Colors.black);
                                          } else {
                                            return Tooltip(
                                                message: theBeCache
                                                    .sequencerSection
                                                    .reasonIfNot,
                                                child: FloatingActionButton(
                                                    heroTag: UniqueKey(),
                                                    child: Icon(Icons.cancel,
                                                        size: theUiModel
                                                                .heightBottomNavBar /
                                                            2),
                                                    onPressed: () {},
                                                    backgroundColor:
                                                        Colors.black));
                                          }
                                        } else {
                                          return FloatingActionButton(
                                              heroTag: UniqueKey(),
                                              child: Icon(Icons.stop,
                                                  size: theUiModel
                                                          .heightBottomNavBar /
                                                      2),
                                              onPressed: () {
                                                theBeCache
                                                    .triggerStopSequencer();
                                              },
                                              backgroundColor: Colors.black);
                                        }
                                      }))),
                          SizedBox(
                              width: theUiModel.heightBottomNavBar,
                              height: theUiModel.heightBottomNavBar / 2,
                              child: FittedBox(
                                  child: FloatingActionButton(
                                      heroTag: UniqueKey(),
                                      child: Icon(Icons.fiber_manual_record,
                                          size: theUiModel.heightBottomNavBar /
                                              2),
                                      onPressed: () {},
                                      backgroundColor: Colors.black))),
                        ])))
              ],
            ),
          ),
        ));
  }
}
