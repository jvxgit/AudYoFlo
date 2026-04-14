import 'package:flutter_test/flutter_test.dart';
import 'package:ayfcorepack/ayfcorepack.dart';
import 'package:ayfcorepack/ayfcorepack_platform_interface.dart';
import 'package:ayfcorepack/ayfcorepack_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockAyfcorepackPlatform
    with MockPlatformInterfaceMixin
    implements AyfcorepackPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');

  @override
  Future<Map<dynamic, dynamic>?> getEntryPoints() async {
    Map<String, String> retMap = {};
    return retMap;
  }
}

void main() {
  final AyfcorepackPlatform initialPlatform = AyfcorepackPlatform.instance;

  test('$MethodChannelAyfcorepack is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelAyfcorepack>());
  });

  test('getPlatformVersion', () async {
    Ayfcorepack ayfcorepackPlugin = Ayfcorepack();
    MockAyfcorepackPlatform fakePlatform = MockAyfcorepackPlatform();
    AyfcorepackPlatform.instance = fakePlatform;

    expect(await ayfcorepackPlugin.getPlatformVersion(), '42');
  });
}
