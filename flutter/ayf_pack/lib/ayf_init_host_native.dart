import 'dart:io';
import 'package:ayf_webhost/ayf_webhost.dart' as webhost;
import 'package:ayf_ffihost/ayf_ffihost.dart' as ffihost;
// Platform.isWindows

String ayf_init_host_options(Map<String, dynamic> configArgs,
    {bool forceFfiHost = false,
    bool forceWebHost = false,
    dynamic corePack,
    dynamic pixBuf}) {
  String initRoute = '/';

  // Host implementations supported for native ffi: Windows, linux and Android
  bool ffiHost = Platform.isWindows || Platform.isLinux || Platform.isAndroid;
  if (forceWebHost) {
    ffiHost = false;
  }
  if (forceFfiHost) {
    ffiHost = true;
  }
  if (ffiHost) {
    configArgs['host'] = ffihost.ayfhost;
  } else {
    configArgs['host'] = webhost.ayfhost;
    initRoute = '/connect';
  }
  if (corePack != null) {
    configArgs['corePack'] = corePack;
  }
  if (pixBuf != null) {
    configArgs['pixBuf'] = pixBuf;
  }
  return initRoute;
}
