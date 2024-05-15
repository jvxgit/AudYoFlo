import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'ayfcorepack_platform_interface.dart';

/// An implementation of [AyfcorepackPlatform] that uses method channels.
class MethodChannelAyfcorepack extends AyfcorepackPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('ayfcorepack');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
  
  @override
  Future<Map<dynamic, dynamic>?> getEntryPoints() async {
    final entries = await methodChannel
        .invokeMethod<Map<dynamic, dynamic>>('getEntryPoints');
    return entries;
 }

}
