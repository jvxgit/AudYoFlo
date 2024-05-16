import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'ayf_pack_method_channel.dart';

abstract class AyfPackPlatform extends PlatformInterface {
  /// Constructs a AyfPackPlatform.
  AyfPackPlatform() : super(token: _token);

  static final Object _token = Object();

  static AyfPackPlatform _instance = MethodChannelAyfPack();

  /// The default instance of [AyfPackPlatform] to use.
  ///
  /// Defaults to [MethodChannelAyfPack].
  static AyfPackPlatform get instance => _instance;
  
  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [AyfPackPlatform] when
  /// they register themselves.
  static set instance(AyfPackPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
