/* 
https://medium.com/nonstopio/flutter-best-practices-c3db1c3cd694
*/

import 'package:ayfcorepack/ayfcorepack.dart';
import 'package:flutter/material.dart';
import 'package:flutter/cupertino.dart';
import 'package:provider/provider.dart';
import '/models/ayf_ui_specific_widgets.dart';
import 'models/ayf_be_specific.dart';
import 'package:ayf_pack/ayf_pack.dart';

import 'package:ayf_ffihost/ayf_ffihost.dart'
    if (dart.library.html) 'package:ayf_webhost/ayf_webhost.dart';

// import 'package:ayf_webhost/ayf_webhost.dart';

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

  // The core pack allows to configure the ffi backend. It pre-loads dlls and the actual host
  configArgs['corePack'] = ayfcore;
  configArgs['host'] = ayfhost;
  configArgs['cmdArgs'] = arguments;
  platformSpec.configureSubSystem(configArgs);

  // ***************************************************
  runApp(MultiProvider(
      providers: [
        ChangeNotifierProvider<AudYoFloPeriodicNotifier>(
            create: (_) => AudYoFloPeriodicNotifier()),
        ChangeNotifierProvider<AudYoFloBackendCache>(
            create: (_) => AudYoFloBackendCacheSpecific()),
        ChangeNotifierProvider<AudYoFloUiModel>(
            create: (_) => AudYoFloUiModelSpecificWithWidget()),
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
