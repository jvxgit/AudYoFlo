import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'pixel_buffer_texture_platform_interface.dart';

/// An implementation of [PixelBufferTexturePlatform] that uses method channels.
class MethodChannelPixelBufferTexture extends PixelBufferTexturePlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('pixel_buffer_texture');

  @override
  Future<int> createTextureRenderer(int width, int height, String rendererId,
      String rendererCtxt, String rendererArg) async {
    int? textureId;
    try {
      textureId =
          await methodChannel.invokeMethod<int>('createTextureRenderer', {
        'width': width,
        'height': height,
        'rendererId': rendererId,
        'rendererCtxt': rendererCtxt,
        'rendererArg': rendererArg
      });
    } on PlatformException catch (e) {
      debugPrint(e.toString());
      return -1;
    }
    return textureId ?? -1;
  }

  @override
  Future<void> destroyTextureRenderer(int textureId) async {
    try {
      await methodChannel.invokeMethod<int>('destroyTextureRenderer', {
        'textureId': textureId,
      });
    } on PlatformException catch (e) {
      debugPrint(e.toString());
    }
  }

  @override
  Future<Map<dynamic, dynamic>?> getEntryPoints() async {
    final entries = await methodChannel
        .invokeMethod<Map<dynamic, dynamic>>('getEntryPoints');
    return entries;
  }
}
