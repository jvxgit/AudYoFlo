import 'ayf_pack_platform_interface.dart';
export 'ayf_pack_common.dart';
export 'package:ayf_pack_native/ayf_pack_native.dart'
    if (dart.library.html) 'ayf_pack_web.dart';

class AyfPack {
  Future<String?> getPlatformVersion() {
    return AyfPackPlatform.instance.getPlatformVersion();
  }
}
