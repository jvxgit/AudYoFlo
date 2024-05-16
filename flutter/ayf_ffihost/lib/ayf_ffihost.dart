library ayf_ffihost;

import 'backend/ayf_backend_bridge.dart';
import 'package:ayf_pack/ayf_pack.dart';
export 'backend/ayf_backend_adapter.dart';
export 'backend/ayf_backend_bridge.dart';

/// A Calculator.
class Calculator {
  /// Returns [value] plus 1.
  int addOne(int value) => value + 1;
}

AyfHost get ayfhost => AyfNativeAudioHost.instance;
