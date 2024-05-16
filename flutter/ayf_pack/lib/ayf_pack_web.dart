library ayf_pack_web;

export 'properties/web/ayf_properties_from_json_simple.dart';
export 'types/web/ayf_bitfield_simple.dart';
export 'properties/web/ayf_properties.dart';
export 'platform/web/ayf_platform_specific.dart';

export 'helpers/web/ayf_helper_lib.dart';

// In order to *not* need this ignore, consider extracting the "web" version
// of your plugin as a separate package, instead of inlining it in the same
// package as the core of your plugin.
// ignore: avoid_web_libraries_in_flutter
import 'dart:html' as html show window;
import 'package:flutter_web_plugins/flutter_web_plugins.dart';
import 'ayf_pack_platform_interface.dart';

/// A web implementation of the AyfPackPlatform of the AyfPack plugin.
class AyfPackWeb extends AyfPackPlatform {
  /// Constructs a AyfPackWeb
  AyfPackWeb();

  static void registerWith(Registrar registrar) {
    AyfPackPlatform.instance = AyfPackWeb();
  }

  /// Returns a [String] containing the version of the platform.
  @override
  Future<String?> getPlatformVersion() async {
    final version = html.window.navigator.userAgent;
    return version;
  }
}
