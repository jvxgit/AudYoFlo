import 'package:flutter_test/flutter_test.dart';
import 'package:ayf_pack/ayf_pack_local.dart';
import 'package:ayf_pack/ayf_pack_platform_interface.dart';
import 'package:ayf_pack/ayf_pack_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockAyfPackPlatform
    with MockPlatformInterfaceMixin
    implements AyfPackPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final AyfPackPlatform initialPlatform = AyfPackPlatform.instance;

  test('$MethodChannelAyfPack is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelAyfPack>());
  });

  test('getPlatformVersion', () async {
    AyfPack ayfPackPlugin = AyfPack();
    MockAyfPackPlatform fakePlatform = MockAyfPackPlatform();
    AyfPackPlatform.instance = fakePlatform;

    expect(await ayfPackPlugin.getPlatformVersion(), '42');
  });
}
