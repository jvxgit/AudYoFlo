import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'ayfcorepack_method_channel.dart';

abstract class AyfcorepackPlatform extends PlatformInterface {
  /// Constructs a AyfcorepackPlatform.
  AyfcorepackPlatform() : super(token: _token);

  static final Object _token = Object();

  static AyfcorepackPlatform _instance = MethodChannelAyfcorepack();

  /// The default instance of [AyfcorepackPlatform] to use.
  ///
  /// Defaults to [MethodChannelAyfcorepack].
  static AyfcorepackPlatform get instance => _instance;
  
  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [AyfcorepackPlatform] when
  /// they register themselves.
  static set instance(AyfcorepackPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
  
  Future<Map<dynamic, dynamic>?> getEntryPoints() {
    throw UnimplementedError('getEntryPoints() has not been implemented.');
  }

}
