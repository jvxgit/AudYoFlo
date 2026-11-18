import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import '../ayf_pack_local.dart';
import 'dart:io' show Platform;

typedef AyfBottomBarAllocator = BottomAppBar? Function();

class AudYoFloUiHttpConnect {
  String ipAddr = '127.0.0.1';
  int ipPort = 8000;
  bool autoConnect = false;
}

mixin AudYoFloUiElementsTabBar {
  List<Widget> theAllocatedTabs = [];
  List<Widget> theAllocatedTabViews = [];

  List<Widget> get allocatedTabs => theAllocatedTabs;
  List<Widget> get allocatedTabViews => theAllocatedTabViews;

  void createTabsAndViews(Map<String, String> entriesCfgWidgets);

  void clearTabAndViews() {
    theAllocatedTabs.clear();
    theAllocatedTabViews.clear();
  }
}

class AudYoFloOneUiSection {
  bool show;
  String imageNameIcon;
  String textShow;
  AudYoFloOneUiSection(this.show, this.imageNameIcon, this.textShow);
}

abstract class AudYoFloUiModel
    with ChangeNotifier
    implements AudYoFloUiElementsTabBar {
  Map<String, dynamic> widgetSpecificConfigurations = {};

  AudYoFloOneUiSection debug = AudYoFloOneUiSection(
      true, 'packages/ayf_pack/images/ayf_icons/ayf-dbg.png', 'Debug');

  AudYoFloOneUiSection components = AudYoFloOneUiSection(
      true, 'packages/ayf_pack/images/ayf_icons/ayf-comp.png', 'Components');

  // =======================================================================

  AudYoFloPropertyValue fastRefreshPerMsecs = AudYoFloPropertyValue(
      50, "/settings/ui/refreshPerMsecFast", "Fast UI Refresh period [msecs].");

  AudYoFloPropertyValue slowRefreshPerMsecs = AudYoFloPropertyValue(1000,
      "/settings/ui/refreshPerMsecSlow", "Slow UI Refresh period [msecs].");

  AudYoFloSettingsConfiguration settingsConfigure =
      AudYoFloSettingsConfiguration();

  bool addUpper = false;
  double heightUpper = 80;

  bool addBottom = false;
  double heightBottom = 80;

  bool addUpperNavBar = true;
  double heightUpperNavBar = 40;

  bool addBottomNavBar = true;
  double heightBottomNavBar = 80;

  double innerSizeInit = 20;

  // ========================================================================

  // For the main tab layout
  double navButtonSizeMain = 50;
  double navButtonAreaWidthMain = 120;
  double navButtonAreaHeightMain = 100;
  double navButtonAreaHeightSpaceMain = 20;
  double navButtonAreaWidthSpaceMain = 20;

  // For the settings dialog
  double navButtonSizeSettings = 50;
  double navButtonAreaWidthSettings = 120;
  double navButtonAreaHeightSettings = 100;
  double navButtonAreaHeightSpacSettings = 20;
  double navButtonAreaWidthSpaceSettings = 20;

  double maxWidthSettings = 640;
  double maxHeightSettings = 480;

  double mixerWidth = 75;
  double mixerHeight = 400;

  // Show combo box to hold technology and devices of primary audio device
  bool showAudioDevTech = true;

  String textApp = 'myAppName';
  String imageNameApp = '';
  double imageWScaleApp = 5;
  double imageOAScaleApp = 0.7;

  // Splash screen, shown while the backend is initialized (initRoute "/").
  // flutter_native_splash does not support the desktop platforms, hence, the
  // splash screen is realized in Flutter (see AudYoFloMainWidget).
  bool showSplash = true;
  String splashImage = 'packages/ayf_pack/images/ayf_splash/splash.png';
  Color splashBackground = Colors.white;
  String splashText = '';

  // Show a (indeterminate) progress indicator below the splash text. Set this to
  // false if the backend initialization blocks the main thread (e.g., in
  // ffi_host_initialize): the animation then freezes in its first frame and
  // only shows a small, garbled arc.
  bool splashShowProgress = true;

  // Minimum time the splash screen is shown - avoids a flash on fast starts
  Duration splashMinDuration = const Duration(milliseconds: 800);

  bool currentColorSaveDialogTriggered = false;

  AyfBottomBarAllocator? allocateBottomAppBar;

  // Reference to platform specific object
  AudYoFloPlatformSpecific? platformSpec;

  // Provide startup route: "/" means that the host is immediately started,
  // "/connect" is a widget to accept an ip address with a port to connect
  String initRoute = '/';
  // String initRoute = '/connect';

  AudYoFloUiHttpConnect httpConnect = AudYoFloUiHttpConnect();

  // We initialize the platform specific part of the application.
  AudYoFloUiModel(this.initRoute) {
    if (Platform.isAndroid) {
      innerSizeInit = 90;
    }

    AudYoFloHelper.dbgPrint('Running constructor <AudYoFloUiModel>');
  }

  // Pre-configure some default values
  void configureSystemParameters(AudYoFloCompileFlags flags) {
    //
    // Initialize the selection bitfield
    //
  }

  int reportSetProperty(AudYoFloPropertyContentLocal prop) {
    bool notifyThis = false;
    int errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;

    if (prop.descriptor == fastRefreshPerMsecs.descriptor) {
      //binauralRenderer.spreadAngle = binauralRenderer.propSpread.value;
      notifyThis = true;
    }

    if (notifyThis) {
      notifyListeners();
    }
    return errCode;
  }

  // Interface function: obtain the list of properties right before showing them in the settings dialog
  void filterListProperties(
      String purpose, AudYoFloMultiComponentsPropertyOnChangeEntry entries) {}

  void notify() {
    notifyListeners();
  }

  @override
  void createTabsAndViews(Map<String, String> entriesCfgWidgets) {
    // Tabs are added to a list along multiple layers of the class hierarchie
    // Since we dont know if we are the first here, we can not clear the list
    // when entering the call. It is the task of the caller to run clear before

    // =======================================================================
    if (debug.show) {
      theAllocatedTabs.add(RotatedBox(
          quarterTurns: -1,
          child: Column(children: [
            ImageIcon(AssetImage(debug.imageNameIcon), size: navButtonSizeMain),
            Text(debug.textShow)
          ])));

      theAllocatedTabViews.add(const AudYoFloDebugWidget());
    }
    // =======================================================================
    if (components.show) {
      theAllocatedTabs.add(RotatedBox(
          quarterTurns: -1,
          child: Column(children: [
            ImageIcon(AssetImage(components.imageNameIcon),
                size: navButtonSizeMain),
            Text(components.textShow)
          ])));

      theAllocatedTabViews.add(const AudYoFloComponentWidget());
    }
  }

  // ========================================================================
  List<AudYoFloPropertyContainer> removeIfPresent(
      List<AudYoFloPropertyContainer> lst,
      String propIsPresent,
      List<String> removeThese) {
    AudYoFloPropertyContainer? ct =
        lst.firstWhereOrNull((element) => element.descriptor == propIsPresent);
    if (ct != null) {
      if (ct is AudYoFloPropertyContentBackend) {
        if (ct.isValid) {
          for (var remToken in removeThese)
            lst.removeWhere((element) => element.descriptor == remToken);
        }
      }
    }
    return lst;
  }
}
