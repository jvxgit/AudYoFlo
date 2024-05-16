import 'package:flutter/services.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

import './ayf_app_bar.dart';
import './ayf_bottom_app_bar.dart';
import './ayf_upper_content.dart';
import './ayf_bottom_content.dart';
import './ayf_main_inner_content.dart';

// https://medium.com/@dvargahali/dart-2-conditional-imports-update-16147a776aa8
// We would need conditional imports which are not supported. Hence, do it
// manually.

class AudYoFloMainLayout extends StatelessWidget {
  final Size size;

  //late FocusNode focusNode;
  AudYoFloMainLayout({required this.size /*, required this.focusNode*/});

  @override
  Widget build(BuildContext context) {
    AudYoFloUiModel theUiModel =
        Provider.of<AudYoFloUiModel>(context, listen: false);
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);

    List<Widget> cwidgets = [];
    Widget? upper;
    Widget inner;
    Widget? bottom;

    double computeSizeInner = 20;

    PreferredSizeWidget? myAppBar;

    Widget? myBottomBar;
    if (theUiModel.addUpperNavBar) {
      myAppBar = AudYoFloAppBar(
          context,
          theUiModel.textApp,
          theUiModel.imageNameApp,
          theUiModel.imageWScaleApp,
          theUiModel.imageOAScaleApp,
          theUiModel.heightUpperNavBar,
          theBeCache);

      computeSizeInner += theUiModel.heightUpperNavBar;
    }

    if (theUiModel.addBottomNavBar) {
      if (theUiModel.allocateBottomAppBar != null) {
        myBottomBar = theUiModel.allocateBottomAppBar!();
      } else {
        myBottomBar = AudYoFloBottomAppBar();
      }
      computeSizeInner += theUiModel.heightBottomNavBar;
    }
    // =====================================================

    if (theUiModel.addUpper) {
      upper = Container(
          width: size.width,
          height: theUiModel.heightUpper,
          child: LayoutBuilder(builder: (context, constraints) {
            return AudYoFloUpperContent();
          }));
      computeSizeInner += theUiModel.heightUpper;
    }

    if (theUiModel.addBottom) {
      bottom = Container(
          width: size.width,
          height: theUiModel.heightBottom,
          child: LayoutBuilder(builder: (context, constraints) {
            return AudYoFloBottomContent();
          }));
      computeSizeInner += theUiModel.heightBottom;
    }

    inner = Container(
        width: size.width,
        height: size.height - computeSizeInner,
        child: LayoutBuilder(builder: (context, constraints) {
          return AudYoFloMainInnerContent();
        }));

    if (upper != null) cwidgets.add(upper);
    cwidgets.add(inner);
    if (bottom != null) cwidgets.add(bottom);

    // Here, we have platform specific code!!
    if (theUiModel.platformSpec != null) {
      theUiModel.platformSpec!.prepareClose(context);
    }

    return Scaffold(
        appBar: myAppBar,
        bottomNavigationBar: myBottomBar,
        body: Shortcuts(
          shortcuts: const <ShortcutActivator, Intent>{
            SingleActivator(LogicalKeyboardKey.space, control: true):
                SpaceIntent(),
          },
          child: Actions(
            actions: {
              SpaceIntent: CallbackAction<SpaceIntent>(onInvoke: (intent) {
                theBeCache.triggerToggleSequencer();
              })
            },
            child: FocusScope(
              autofocus: true,
              child: Container(
                  child: Column(
                children: cwidgets,
              )),
            ),
          ),
        ));
  }
}

class SpaceIntent extends Intent {
  const SpaceIntent();
}
