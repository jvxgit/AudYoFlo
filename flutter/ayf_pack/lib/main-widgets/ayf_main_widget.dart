import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:path/path.dart' as path;
import 'package:flutter_dotenv/flutter_dotenv.dart';
import 'dart:io' show Platform;
import '../ayf_pack_local.dart';
// import 'package:ayfcorepack/ayfcorepack.dart';
import 'package:collection/collection.dart';

import '../routes/ayf_routes.dart';

//============================================================
// Define a callback that is used to hookup functions on a periodic basis
//============================================================

//============================================================
// Create the main FernLive Widget
// This widget is stateful, hence, check out the stateful part.
//============================================================

class AudYoFloMainWidget extends StatefulWidget {
  final AudYoFloPlatformSpecific platformSpecific;

  // https://stackoverflow.com/questions/54279223/flutter-default-assignment-of-list-parameter-in-a-constructor
  // The splash screen is configured in AudYoFloUiModel (showSplash, splash*)
  const AudYoFloMainWidget({Key? key, required this.platformSpecific})
      : super(key: key);

  // Return the stateful part
  @override
  _AudYoFloMainWidgetStates createState() => _AudYoFloMainWidgetStates();
}

//Stateful part of this widget
class _AudYoFloMainWidgetStates extends State<AudYoFloMainWidget>
    with WidgetsBindingObserver
    implements AudYoFloPeriodicCallbacks {
// ======================================================================

  // WindowsFernNative pltf_inst = WindowsFernNative();

  // String _windowSize = 'Unknown';
  // This is the main timer with a fast and a slow callback
  // In each callback, a notifier is used to inform everyone interested about
  // the callback
  AudYoFloHostTimer? hTimer;

  // This is the configuration module for the algorithm

  // This object must be constructed here since it is used in initState
  // AudioFlowBackendBridge theBEBridge = AudioFlowBackendBridge();
  AyfHost? theBeAdapter; //AyfNativeAudioHost.instance;

  // This is to grant access to the periodic notifiers - but don't listen, we are the only one to write it
  AudYoFloPeriodicNotifier? notifierPeriodic;
  AudYoFloBackendCache? notifierBeCache;
  AudYoFloDebugModel? notifierDbgModel;
  AudYoFloUiModel? uiModel;

  // Backend initialization running: show the splash screen
  bool _initPending = false;

  Future<void> _initializeWithSplash() async {
    final minDuration = Future.delayed(uiModel!.splashMinDuration);

    // The backend initialization (ffi_host_initialize) runs synchronously in the
    // main thread and may block it for a long time (e.g., RS232 port scan). Hence,
    // make sure that the splash screen - including the asynchronously decoded
    // image - is really presented BEFORE the blocking call is entered.
    try {
      await precacheImage(AssetImage(uiModel!.splashImage), context);
    } catch (e) {
      print('Failed to load splash image <${uiModel!.splashImage}>: $e');
    }
    if (!mounted) return;
    WidgetsBinding.instance.scheduleFrame();
    await WidgetsBinding.instance.endOfFrame;
    // Give the raster thread and the platform runner (window show) time to
    // present the frame before the UI thread blocks
    await Future.delayed(const Duration(milliseconds: 100));
    if (!mounted) return;

    try {
      await uiModel!.platformSpec!
          .initializeSubSystem(notifierBeCache, uiModel, notifierDbgModel);
    } catch (e) {
      print('Backend initialization failed: $e');
    }
    await minDuration;
    if (mounted) {
      setState(() => _initPending = false);
    }
  }

  Widget _splashScreen() {
    final model = uiModel!;
    return Container(
        color: model.splashBackground,
        alignment: Alignment.center,
        child: Column(mainAxisSize: MainAxisSize.min, children: [
          ConstrainedBox(
              constraints: const BoxConstraints(maxWidth: 320, maxHeight: 220),
              child: Image.asset(model.splashImage, fit: BoxFit.contain)),
          const SizedBox(height: 32),
          if (model.splashText.isNotEmpty) ...[
            Text(model.splashText,
                style: const TextStyle(
                    fontSize: 18,
                    color: Colors.black54,
                    decoration: TextDecoration.none)),
            const SizedBox(height: 16),
          ],
          if (model.splashShowProgress)
            const SizedBox(
                width: 28,
                height: 28,
                child: CircularProgressIndicator(strokeWidth: 3)),
        ]));
  }

  // ========================================================
  // Build function to create the widget. Note: The initState
  // function is always called before the Buid function.
  // ========================================================
  @override
  Widget build(BuildContext context) {
    // Access to the notifier object
    notifierPeriodic =
        Provider.of<AudYoFloPeriodicNotifier>(context, listen: false);
    notifierBeCache = Provider.of<AudYoFloBackendCache>(context, listen: false);
    notifierDbgModel = Provider.of<AudYoFloDebugModel>(context, listen: false);
    uiModel = Provider.of<AudYoFloUiModel>(context, listen: false);

    /*
          if (updateDueToRestart) {
            if (uiModel!.initRoute != "/") {
              Navigator.pushNamed(context, uiModel!.initRoute);
            }
          }
          */

    // If we pass by at this positon for the first time adopt the audioflow
    // handles and store in states
    if (theBeAdapter == null) {
      // Attach the platform specific part in uiModel
      assert(uiModel != null);
      uiModel!.platformSpec = widget.platformSpecific;
      theBeAdapter = widget.platformSpecific.referenceHost();

      /* 
     * Here, we run the main component initialization functions in an async function.
     * 
     * We need to delay the initialization to allow all stateful widgets to 
     * connect to the bridge within the initState function - and the order of initState
     * is unpredictable 
     * https://oleksandrkirichenko.com/blog/delayed-code-execution-in-flutter/ 
     * 
     * 
     */

      if (uiModel!.initRoute == "/") {
        // initializeSubSystem starts the backend engine. We might couple it immediately OR we
        // may expect some input before connecting - depending on the routes. If we start with the
        // main route, we immediately couple the backend.
        // The splash screen is shown until the backend is up. The initialization is started after
        // the first frame, such that the splash screen is visible also if the initialization
        // blocks the UI thread.
        if (uiModel!.showSplash) {
          _initPending = true;
          WidgetsBinding.instance
              .addPostFrameCallback((_) => _initializeWithSplash());
        } else {
          uiModel!.platformSpec!
              .initializeSubSystem(notifierBeCache, uiModel, notifierDbgModel);
        }
      }

      // =================================================================
      // Start the class to realize the periodic callbacks. It receives the
      // two callbacks as functional arguments
      hTimer = AudYoFloHostTimer(callbacks: this);

      // Actually start the timers
      hTimer?.initTimer();
    }

    /*
    switch(uiModel.startRoute)
    {
      case ''
    }
    */
    if (_initPending) {
      return MaterialApp(
          key: const ValueKey('ayfSplash'),
          debugShowCheckedModeBanner: false,
          home: _splashScreen());
    }

    // Own key: the main app must be created from scratch after the splash
    // screen - initialRoute is only evaluated when the navigator is created
    return MaterialApp(
      key: const ValueKey('ayfMain'),
      initialRoute: uiModel!.initRoute,
      onGenerateRoute: AudYoFloRouteGenerator.generateRoute,
    );
  }

  // ========================================================
  // Initialization function to initialize the states
  // ========================================================
  @override
  void initState() {
    super.initState();

    // Add this object as an observer for events - do not know exactly which
    // events right now. I think it is for didChangeAppLifecycleState
    WidgetsBinding.instance.addObserver(this);

    /*
     * I have no idea why the shutdown tool can not live here. If I 
     * try to open a dialog from this class it just does not show.
     * I think it is because this class is not connected to the widget tree
     * as the MaterialApp is only the child of this instance.
     * Therefore I have moved this part into the ayf_main_layout widget!
     * 
    FlutterWindowClose.setWindowShouldCloseHandler(() async {
      theBEBridge.triggeredClose();
      return true;
    });
    */
  }

  // ========================================================
  // The opposite of the init_state function called at end of lifetime
  // ========================================================
  @override
  void dispose() {
    // stop the timer
    hTimer?.termTimer();

    // Terminate the native bridge
    theBeAdapter?.terminate();

    super.dispose();
  }

  // ========================================================
  // This function is important to detect lifecycle changes
  // At the moment, it does not even detect the "stop" button :-(
  // ========================================================
  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    switch (state) {
      case AppLifecycleState.resumed:
        print("Hallo1");
        break;
      case AppLifecycleState.paused:
        print("Hallo2");
        break;
      default:
        break;
    }
  }

  // ========================================================
  // Callbacks for the fast and the slow callback
  // ========================================================
  @override
  int reportFast() {
    // notifierPeriodic?.fast = true;
    notifierPeriodic?.cntFast++;
    notifierPeriodic?.notify();
    return uiModel!.fastRefreshPerMsecs.value.toInt();
  }

  @override
  int reportSlow() {
    theBeAdapter?.periodicTriggerSlow();
    // notifierPeriodic?.fast = false;
    notifierPeriodic?.cntSlow++;
    notifierPeriodic?.notify();
    return uiModel!.slowRefreshPerMsecs.value.toInt();
    //print("Hello slow");
  }

  // Run the system init routines in an async call!
}
