import 'ayfcorepack_platform_interface.dart';
import 'package:ayf_pack/ayf_pack.dart';

class Ayfcorepack extends AyfCorePackIf {
  Future<String?> getPlatformVersion() {
    return AyfcorepackPlatform.instance.getPlatformVersion();
  }

  @override
  Future<Map<dynamic, dynamic>?> getEntryPoints() {
    return AyfcorepackPlatform.instance.getEntryPoints();
  }
}

Ayfcorepack get ayfcore => Ayfcorepack();
