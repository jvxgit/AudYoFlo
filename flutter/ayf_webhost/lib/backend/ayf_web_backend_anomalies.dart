import 'package:collection/collection.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:http/http.dart' as http;
import '../ayf_web_backend_constants.dart';
import '../ayf_web_backend_typedefs.dart';
import 'ayf_web_backend_message_processor.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

import '../ayf_web_backend_helpers.dart';

import 'dart:async';
import 'dart:io';

mixin AudYoFloWebBackendAnomalies {
  void reportStatusDisconnect(int idReason, int subId, String description) {
    print(
        'Status disconnect reported: Id=$idReason - SubId=$subId - reason hint: $description');
  }

  void reportStatusErrorProtocol(int idReason, int subId, String description) {
    print(
        'Communication reported a protocol status error: Id=$idReason - SubId=$subId - reason hint: $description');
  }
}
