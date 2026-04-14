import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'pixel_buffer_texture_method_channel.dart';

abstract class PixelBufferTexturePlatform extends PlatformInterface {
  /// Constructs a PixelBufferTexturePlatform.
  PixelBufferTexturePlatform() : super(token: _token);

  static final Object _token = Object();

  static PixelBufferTexturePlatform _instance =
      MethodChannelPixelBufferTexture();

  /// The default instance of [PixelBufferTexturePlatform] to use.
  ///
  /// Defaults to [MethodChannelPixelBufferTexture].
  static PixelBufferTexturePlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [PixelBufferTexturePlatform] when
  /// they register themselves.
  static set instance(PixelBufferTexturePlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<int> createTextureRenderer(int width, int height, String rendererId,
      String rendererCtxt, String rendererArg) {
    throw UnimplementedError(
        'createTextureRenderer() has not been implemented.');
  }

  Future<void> destroyTextureRenderer(int textureId) {
    throw UnimplementedError(
        'destroyTextureRenderer() has not been implemented.');
  }

  Future<Map<dynamic, dynamic>?> getEntryPoints() {
    throw UnimplementedError('getEntryPoints() has not been implemented.');
  }
}
