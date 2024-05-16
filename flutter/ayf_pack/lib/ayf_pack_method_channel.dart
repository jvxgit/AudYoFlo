import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'ayf_pack_platform_interface.dart';

/// An implementation of [AyfPackPlatform] that uses method channels.
class MethodChannelAyfPack extends AyfPackPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('ayf_pack');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
