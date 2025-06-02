import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:path/path.dart' as path;
import 'package:flutter_dotenv/flutter_dotenv.dart';
import 'dart:io' show Platform;
import 'package:flutter_window_close/flutter_window_close.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:collection/collection.dart';

class AudYoFloPlatformSpecificHtmlNat extends AudYoFloPlatformSpecific {
  AyfHost? theBeAdapter;
  AudYoFloBackendCache? theBeCache;
  @override
  AyfHost? referenceHost() {
    return theBeAdapter;
  }

  @override
  void prepareClose(BuildContext context) {
    /*
     * This installs a handler for the "close" event. 
     */

    theBeCache = Provider.of<AudYoFloBackendCache>(context, listen: false);

    // Window Close only on Desktop!
    if ((Platform.isWindows) || Platform.isLinux || Platform.isMacOS) {
      FlutterWindowClose.setWindowShouldCloseHandler(() async {
        bool wantClose = false;
        await showDialog(
            context: context,
            builder: (context) {
              return AlertDialog(
                  title: const Text('Do you really want to quit?'),
                  actions: [
                    ElevatedButton(
                        onPressed: () {
                          Navigator.of(context).pop(false);
                          wantClose = true;
                        },
                        child: const Text('Yes')),
                    ElevatedButton(
                        onPressed: () => Navigator.of(context).pop(false),
                        child: const Text('No')),
                  ]);
            });
        if (wantClose) {
          await theBeCache!.triggerClose();
        }
        return wantClose;
        //return wantToQuit;
      });
    }
  }

  @override
  Future<void> triggerClose() async {
    // await theBeCache!.triggerClose();
    await FlutterWindowClose.closeWindow();
  }

  // Allocate and return file drop widget
  @override
  Widget? allocateFileDropWidget(JvxComponentIdentification identT,
          String textShowDrag, double sizeIcon) =>
      AudYoFloFileInputCoreWidgetNative(identT, textShowDrag, sizeIcon);

  @override
  void configureSubSystem(Map<String, dynamic> cfg) {
    var elm = cfg.entries.firstWhereOrNull((element) => element.key == 'host');
    if (elm != null) {
      if (elm.value is AyfHost) {
        theBeAdapter = elm.value;
      }
    }

    assert(theBeAdapter != null);
  }

  @override
  Future<bool> initializeSubSystem(AudYoFloBackendCache? notifierBeCache,
      AudYoFloUiModel? uiModel, AudYoFloDebugModel? notifierDbgModel) async {
    return true;
  }

  @override
  String get lastError {
    if (theBeAdapter != null) {
      return theBeAdapter!.lastError;
    }
    return '';
  }
}

class AudYoFloPlatformSpecificNative extends AudYoFloPlatformSpecificHtmlNat {
  AyfCorePackIf? theCorePack;
  AyfCorePackIf? thePixBuf;
  bool nativeHostConnect = false;
  String appAlternativeName = '';

  @override
  void configureSubSystem(Map<String, dynamic> cfg) {
    super.configureSubSystem(cfg);

    String descr = theBeAdapter!.descriptionHost();
    MapEntry? elm;

    if (descr == "AyfNativeAudioHost") {
      nativeHostConnect = true;

      elm =
          cfg.entries.firstWhereOrNull((element) => element.key == 'corePack');
      if (elm != null) {
        if (elm.value is AyfCorePackIf) {
          theCorePack = elm.value;
        }
      }

      elm = cfg.entries.firstWhereOrNull((element) => element.key == 'pixBuf');
      if (elm != null) {
        if (elm.value is AyfCorePackIf) {
          thePixBuf = elm.value;
        }
      }

      // We need the core pack here!!
      assert(theCorePack != null);
    }

    elm = cfg.entries.firstWhereOrNull((element) => element.key == 'cmdArgs');
    if (elm != null) {
      if (elm.value is List<String>) {
        cmdArgs = elm.value;
      }
    }

    // cmdArgs miht also be empty

    elm = cfg.entries
        .firstWhereOrNull((element) => element.key == 'appAlternativeName');
    if (elm != null) {
      if (elm.value is String) {
        appAlternativeName = elm.value;
      }
    }
  }

  @override
  Future<bool> initializeSubSystem(AudYoFloBackendCache? notifierBeCache,
      AudYoFloUiModel? uiModel, AudYoFloDebugModel? notifierDbgModel) async {
    bool retVal = true;
    if (nativeHostConnect) {
      if ((theCorePack != null) &&
          (notifierBeCache != null) &&
          (notifierDbgModel != null) &&
          (theBeAdapter != null)) {
        // Get the cross link references to the corepack dll
        final entryPointsCorePack = await theCorePack!.getEntryPoints();
        Map<dynamic, dynamic>? entryPointsPixBuf;
        if (thePixBuf != null) {
          entryPointsPixBuf = await thePixBuf!.getEntryPoints();
        }

        // var elmOperation = entryPointsCorePack.entries.firstOrNull();

        // This will delay the initialization to happen AFTER every state has been created
        //Future.delayed(Duration(milliseconds: 1), () {
        String runExec = Platform.resolvedExecutable;
        if (appAlternativeName.isNotEmpty) {
          runExec = appAlternativeName;
        }
        String sepString = Platform.pathSeparator;
        String execName = Platform.executable;
        List<String> tokens = runExec.split(sepString);
        String appNameExe = tokens.last;
        tokens = appNameExe.split('.');
        String appNameToken = tokens[0].toLowerCase();
        String hostConfigPath = '';
        String hostConfigSymbol = '';
        int hostConfigAddr = 0;

        String pixBufConfigPath = '';
        String pixBufConfigSymbolList = '';
        int pixBufConfigAddrSet = 0;
        int pixBufConfigAddrReset = 0;

        bool loadEnvSuccess = true;

        String envFileName = "." + appNameToken + ".env";

        int executableInBinFolder = 0;
        String executableInBinFolderStr = "no";

        // Open the dynamic library
        var libraryPath = path.join("library-not-found");

        // We should specify a folder and a filename to allow subfiles to be taken from subfolders
        //var defaultConfigFName = 'data/flutter_assets/' + appNameToken + ".jvx";
        var defaultConfigFName = appNameToken + ".jvx";

        // Prepare the dynamic libraries
        // Read in the
        try {
          await dotenv.load(fileName: envFileName);
        } on Exception {
          loadEnvSuccess = false;
        }

        if (loadEnvSuccess) {
          libraryPath = dotenv.get('AYF_BACKEND_FFI_SHARED_LIBRARY',
              fallback: libraryPath);
          defaultConfigFName = dotenv.get(
              'AYF_BACKEND_FFI_DEFAULT_CONFIG_FILENAME',
              fallback: defaultConfigFName);
        }

        if (entryPointsCorePack is Map<dynamic, dynamic>) {
          Map<dynamic, dynamic> entryPointsMap = entryPointsCorePack;

          // Extract hostConfig module path
          hostConfigPath = entryPointsMap.entries.firstWhereOrNull((element) {
            if (element.key is String) {
              return (element.key == 'loadedModule');
            }
            return false;
          })?.value;

          // Extract hostConfig module path
          hostConfigSymbol = entryPointsMap.entries.firstWhereOrNull((element) {
            if (element.key is String) {
              return (element.key == 'moduleEntrySymbol');
            }
            return false;
          })?.value;

          hostConfigAddr = entryPointsMap.entries.firstWhereOrNull((element) {
            if (element.key is String) {
              return (element.key == 'moduleEntryAddress');
            }
            return false;
          })?.value;
        }

        // ===============================================================

        if (entryPointsPixBuf != null) {
          Map<dynamic, dynamic> entryPointsMap = entryPointsPixBuf;

          // Extract hostConfig module path
          pixBufConfigPath = entryPointsMap.entries.firstWhereOrNull((element) {
            if (element.key is String) {
              return (element.key == 'loadedModule');
            }
            return false;
          })?.value;

          // Extract hostConfig module path
          pixBufConfigSymbolList =
              entryPointsMap.entries.firstWhereOrNull((element) {
            if (element.key is String) {
              return (element.key == 'moduleEntrySymbol');
            }
            return false;
          })?.value;

          pixBufConfigAddrSet =
              entryPointsMap.entries.firstWhereOrNull((element) {
            if (element.key is String) {
              return (element.key == 'moduleEntryAddressSet');
            }
            return false;
          })?.value;

          pixBufConfigAddrReset =
              entryPointsMap.entries.firstWhereOrNull((element) {
            if (element.key is String) {
              return (element.key == 'moduleEntryAddressReset');
            }
            return false;
          })?.value;
        }

        // ===================================================================
        List<String> cmdArgsAdd = List.from(cmdArgs!);

        // Check for host config dll
        if (hostConfigPath.isNotEmpty) {
          cmdArgsAdd.add('--natconfm');
          cmdArgsAdd.add(hostConfigPath);
        }

        if (hostConfigSymbol.isNotEmpty) {
          cmdArgsAdd.add('--natconfs');
          cmdArgsAdd.add(hostConfigSymbol);
        }

        if (pixBufConfigPath.isNotEmpty) {
          cmdArgsAdd.add('--pixbufm');
          cmdArgsAdd.add(pixBufConfigPath);
        }

        if (hostConfigSymbol.isNotEmpty) {
          cmdArgsAdd.add('--pixbufs');
          cmdArgsAdd.add(pixBufConfigSymbolList);
        }

        if (loadEnvSuccess) {
          executableInBinFolderStr = dotenv.get(
              'AYF_BACKEND_EXECUTABLE_IN_BINFOLDER',
              fallback: executableInBinFolderStr);
          if (executableInBinFolderStr == "yes") {
            executableInBinFolder = 1;
          }
        }

        // If we have not specified a config file, we add a default option here:
        // We should always have a config filename set!!
        bool cfgFNameSpecified = false;
        for (var elm in cmdArgsAdd) {
          if (elm == "--config") {
            cfgFNameSpecified = true;
            break;
          }
        }
        if (!cfgFNameSpecified) {
          cmdArgsAdd.add("--config");
          cmdArgsAdd.add(defaultConfigFName);
        }

        if (theBeAdapter != null) {
          Map<String, dynamic> args = {};
          args['cmdArgs'] = cmdArgsAdd;
          args['libPath'] = libraryPath;
          args['exeInBin'] = executableInBinFolder;

          retVal = await theBeAdapter!
              .initialize(notifierBeCache, notifierDbgModel, args);

          uiModel!.configureSystemParameters(theBeAdapter!.compFlags);
        }
      }
    } else {
      if (notifierBeCache != null) {
        Map<String, dynamic> args = {};
        args['cmdArgs'] = cmdArgs;
        retVal = await theBeAdapter!
            .initialize(notifierBeCache, notifierDbgModel, args);
        if (retVal == false) {
          // theBeAdapter!.
        }
      }
      // HTML connect
    }

    return retVal;
  }
}

AudYoFloPlatformSpecific allocatePlatformSpecific() {
  return AudYoFloPlatformSpecificNative();
}
