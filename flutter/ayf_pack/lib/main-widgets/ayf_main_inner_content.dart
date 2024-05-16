import 'package:flutter/material.dart';
//import 'package:flutter/src/scheduler/ticker.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

class AudYoFloMainInnerContent extends StatefulWidget {
  @override
  _AudYoFloMainInnerContentStates createState() =>
      _AudYoFloMainInnerContentStates();
}

class _AudYoFloMainInnerContentStates extends State<AudYoFloMainInnerContent>
    with TickerProviderStateMixin {
  late TabController tabController;

  @override
  void initState() {
    super.initState();
    tabController = TabController(
      initialIndex: 0,
      length: 6,
      vsync: this,
    );
  }

  @override
  Widget build(BuildContext context) {
    final Size size = MediaQuery.of(context).size;

    AudYoFloUiModel theModel =
        Provider.of<AudYoFloUiModel>(context, listen: false);
    // print('$size');

    // Let us create the widgets to show up in the main window

    //
    // The TabController must be re-created since the length parameter is final.
    // We therefore copy the index to the initial Index
    // https://stackoverflow.com/questions/46180919/pattern-for-updating-tabcontroller-length-from-child-widget
    //

    // FIlter out the widget specific arguments
    var args = theModel.platformSpec!.cmdArgs;
    List<String> argsFilteredFwd = [];
    List<String> argsFilteredWidgets = [];
    bool foundEntry = false;
    for (var arg in args) {
      if (foundEntry) {
        argsFilteredWidgets.add(arg);
      } else {
        if (arg == "--widget") {
          foundEntry = true;
        } else {
          argsFilteredFwd.add(arg);
        }
      }
    }

    Map<String, String> entriesCfgWidgets = {};
    for (var elm in argsFilteredWidgets) {
      var lstElms = elm.split('=');
      if (lstElms.length == 2) {
        entriesCfgWidgets[lstElms[0]] = lstElms[1];
      }
    }

    theModel.platformSpec!.cmdArgs = argsFilteredFwd;

    // Clear the list before rebuilding it
    theModel.clearTabAndViews();

    // Rebuild it
    theModel.createTabsAndViews(entriesCfgWidgets);
    // var subWidgets = CreateUiElementsTabBar();
    // subWidgets.createTabsAndViews(theModel);

    int index = tabController.index;
    if (index >= theModel.allocatedTabViews.length) {
      index = 0;
    }
    tabController = TabController(
      initialIndex: tabController.index,
      length: theModel.allocatedTabViews.length,
      vsync: this,
    );
    return NotificationListener(
        onNotification: (SizeChangedLayoutNotification notification) {
          return true;
        },
        child: Center(
            child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: <Widget>[
              Container(
                  child: SingleChildScrollView(
                child: Column(children: [
                  SizedBox(
                      height: tabController.length *
                              theModel.navButtonAreaHeightMain +
                          theModel.navButtonAreaHeightSpaceMain,
                      width: theModel.navButtonAreaWidthMain,
                      child: RotatedBox(
                          quarterTurns: 1,
                          child: TabBar(
                            labelColor:
                                Colors.black, //Color.fromRGBO(4, 2, 46, 1),
                            labelStyle: const TextStyle(
                                fontStyle: FontStyle.normal,
                                fontWeight: FontWeight.bold),
                            unselectedLabelStyle: const TextStyle(
                                fontStyle: FontStyle.italic,
                                fontWeight: FontWeight.normal),
                            // labelStyle: theme.textTheme.headline1,
                            indicatorColor: Color.fromRGBO(48, 28, 46, 10),
                            unselectedLabelColor: Colors.grey,
                            controller: tabController,
                            tabs: theModel.allocatedTabs,
                          )))
                ]),
              )),
              Container(
                width: size.width - theModel.navButtonAreaWidthMain,
                child: TabBarView(

                    // https://stackoverflow.com/questions/51518393/disable-swiping-tabs-in-tabbar-flutter
                    physics: const NeverScrollableScrollPhysics(),
                    controller: tabController,
                    children: theModel.allocatedTabViews),
              )
            ])));
  }
}

/*
class FernliveTickerProvider implements TickerProvider {
  @override
  Ticker createTicker(TickerCallback onTick) {
    // TODO: implement createTicker
    throw UnimplementedError();
  }

  void onTickerReport(Duration elapsed) {}
}
*/



