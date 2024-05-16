import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:ayf_pack/ayf_pack_method_channel.dart';

void main() {
  MethodChannelAyfPack platform = MethodChannelAyfPack();
  const MethodChannel channel = MethodChannel('ayf_pack');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
