/* 
https://medium.com/nonstopio/flutter-best-practices-c3db1c3cd694
*/
// import 'package:dart/foundation.dart'
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/cupertino.dart';
import 'package:provider/provider.dart';
import '/models/ayf_ui_specific_widgets.dart';
import 'models/ayf_be_specific.dart';
import 'package:ayf_pack/ayf_pack.dart';

import 'package:ayfcorepack/ayfcorepack.dart' as corepack;

import 'package:ayf_pack/ayf_init_host_native.dart'
    if (dart.library.html) 'package:ayf_pack/ayf_init_host_web.dart';

// import 'package:flutter_native_splash/flutter_native_splash.dart';

// Commando arguments:
// https://github.com/flutter/flutter/issues/32986

// TO DO: ADD GLOBAL VARIABLES TO LINK FROM STATIC CALLS TO WHATEVER
// https://stackoverflow.com/questions/29182581/global-variables-in-dart

void main(List<String> arguments) {
  /*
  if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
    setWindowTitle('App title');
    setWindowMinSize(const Size(700, 500));
    setWindowMaxSize(Size.infinite);
  }
  */

  // What is this good for in native windows?? It seems that native splash image
  // does not work on platform windows!
  //
  // https://www.youtube.com/watch?v=dB0dOnc2k10
  // WidgetsBinding widgetsBinding = WidgetsFlutterBinding.ensureInitialized();
  // FlutterNativeSplash.preserve(widgetsBinding: widgetsBinding);
  //

  // Initialize the platform specific part here
  AudYoFloPlatformSpecific platformSpec = allocatePlatformSpecific();
  Map<String, dynamic> configArgs = {};

// ===========================================================================
  // Optins for the host
  // ===========================================================================
  dynamic corePack;
  bool forceWebHost = false;
  if (!forceWebHost) {
    corePack = corepack.ayfcore;
  }
  // ===========================================================================
  /* The core pack configures the backend. For every new use-case, we need another corepack */
  // Currently, ffihost limited to windows
  String initRoute = ayf_init_host_options(configArgs,
      forceWebHost: forceWebHost, corePack: corePack);

  configArgs['cmdArgs'] = arguments;

  if (Platform.isAndroid) {
    configArgs['appAlternativeName'] = 'starter_app';
  }

  platformSpec.configureSubSystem(configArgs);

  // ***************************************************
  runApp(MultiProvider(
      providers: [
        ChangeNotifierProvider<AudYoFloPeriodicNotifier>(
            create: (_) => AudYoFloPeriodicNotifier()),
        ChangeNotifierProvider<AudYoFloBackendCache>(
            create: (_) => AudYoFloBackendCacheSpecific()),
        ChangeNotifierProvider<AudYoFloUiModel>(
            create: (_) => AudYoFloUiModelSpecificWithWidget(initRoute)),
        ChangeNotifierProvider<AudYoFloDebugModel>(
            create: (_) => AudYoFloDebugModel()),
        /*ChangeNotifierProvider<FernliveCacheModel>(
        create: (_) => FernliveCacheModel()),*/
      ],

      // Here, we enter the main widget. We pass the backend adapter (here: ffi bridge)
      // and the core pack which holds the Ayf components precompiled plugin
      child: AudYoFloMainWidget(
        platformSpecific: platformSpec,
      )));
}
