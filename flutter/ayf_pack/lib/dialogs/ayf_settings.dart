import 'dart:math';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

// Audio settings dialog: only purpose is to start the dialog and to set the proper size
class AudYoFloSettingsDialog extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    final Size size = MediaQuery.of(context).size;
    return SimpleDialog(
        insetPadding: EdgeInsets.zero,
        titlePadding: EdgeInsets.zero,
        contentPadding: EdgeInsets.zero,
        children: [AudYoFloSettingsInnerTabs(size.width, size.height)]);
  }
}

// ===========================================================================

class AudYoFloSettingsInnerTabs extends StatefulWidget {
  final double width;
  final double height;
  AudYoFloSettingsInnerTabs(this.width, this.height);

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloSettingsInnerTabsState();
  }
}

class _AudYoFloSettingsInnerTabsState extends State<AudYoFloSettingsInnerTabs>
    with TickerProviderStateMixin {
  late TabController tabController;
  int idxTab = 0;
  @override
  void initState() {
    // TODO: implement initState
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);
    AudYoFloUiModel theUiModel =
        Provider.of<AudYoFloUiModel>(context, listen: false);

    double innerWidth = widget.width * 0.9;
    double innerHeight = widget.height * 0.9;
    innerWidth = min(innerWidth, theUiModel.maxWidthSettings);
    innerHeight = min(innerHeight, theUiModel.maxHeightSettings);

    return Consumer<AudYoFloUiModel>(
      builder: (context, notifier, child) {
        SettingsUiElementsTabBar elementTabs = SettingsUiElementsTabBar();
        elementTabs.createTabsAndViews(theUiModel);

        tabController = TabController(
          initialIndex: idxTab,
          length: elementTabs.theAllocatedTabViews.length,
          vsync: this,
        );
        return Container(
            width: innerWidth,
            height: innerHeight,
            child: NotificationListener(
                onNotification: (SizeChangedLayoutNotification notification) {
                  return true;
                },
                child: Center(
                    child: Column(
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        crossAxisAlignment: CrossAxisAlignment.stretch,
                        children: <Widget>[
                      Container(
                          child: SingleChildScrollView(
                        child: Row(
                            mainAxisAlignment: MainAxisAlignment.start,
                            children: [
                              SizedBox(
                                  width: tabController.length *
                                          theUiModel
                                              .navButtonAreaWidthSettings +
                                      theUiModel
                                          .navButtonAreaWidthSpaceSettings,
                                  height:
                                      theUiModel.navButtonAreaHeightSettings,
                                  child: TabBar(
                                    onTap: (value) {
                                      idxTab = value;
                                    },
                                    labelColor: Colors
                                        .black, //Color.fromRGBO(4, 2, 46, 1),
                                    labelStyle: TextStyle(
                                        fontStyle: FontStyle.normal,
                                        fontWeight: FontWeight.bold),
                                    unselectedLabelStyle: TextStyle(
                                        fontStyle: FontStyle.italic,
                                        fontWeight: FontWeight.normal),
                                    // labelStyle: theme.textTheme.headline1,
                                    indicatorColor:
                                        Color.fromRGBO(48, 28, 46, 10),
                                    unselectedLabelColor: Colors.grey,
                                    controller: tabController,
                                    tabs: elementTabs.theAllocatedTabs,
                                  ))
                            ]),
                      )),
                      Container(
                        height: innerHeight -
                            theUiModel.navButtonAreaHeightSettings,
                        child: TabBarView(
                            controller: tabController,
                            children: elementTabs.theAllocatedTabViews),
                      )
                    ]))));
      },
    );
  }
}

// ==========================================================================

class SettingsUiElementsTabBar {
  List<Widget> theAllocatedTabs = [];
  List<Widget> theAllocatedTabViews = [];
  void createTabsAndViews(AudYoFloUiModel uimodel) {
    theAllocatedTabs.clear();

    for (var elm in uimodel.settingsConfigure.allSections.entries) {
      if (elm.value.nameImage.isNotEmpty) {
        theAllocatedTabs.add(
            Column(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
          Image.asset(elm.value.nameImage,
              filterQuality: FilterQuality.medium,
              width: uimodel.navButtonSizeSettings,
              height: uimodel.navButtonSizeSettings),
          Text(elm.value.nameEntry)
        ]));
      } else {
        theAllocatedTabs.add(Column(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [Text(elm.value.nameEntry)]));
      }

      theAllocatedTabViews.add(AudYoFloSettingsPropertyList(elm.value));
    }
  }
}
