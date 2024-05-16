import 'package:flutter/material.dart';
import '../ayf_pack_local.dart';

class AudYoFloRouteGenerator {
  static const String hostPage = '/';
  static const String settingsPage = '/settings';
  static const String otherPage = '/other';
  static const String connectPage = '/connect';

  AudYoFloRouteGenerator._() {}
  static Route<dynamic> generateRoute(RouteSettings settings) {
    switch (settings.name) {
      case hostPage:
        return MaterialPageRoute(
          builder: (_) => AudYoFloMainAppWidget(),
        );
      case settingsPage:
        return MaterialPageRoute(
          builder: (_) => AudYoFloMainAppWidget(),
        );
      case connectPage:
        return MaterialPageRoute(
          builder: (_) => const AudYoFloConnectWidget(),
        );
      default:
        throw FormatException("Route not found");
    }
  }
}
