import 'dart:io';

import 'package:ayf_webhost/ayf_webhost.dart' as webhost;

// Platform.isWindows

String ayf_init_host_options(Map<String, dynamic> configArgs,
    {bool forceFfiHost = false, bool forceWebHost = false, dynamic corePack}) {
  String initRoute = '/connect';
  configArgs['host'] = webhost.ayfhost;
  return initRoute;
}
