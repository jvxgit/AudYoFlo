import '../ayf_pack_local.dart';

// Mirrors the native struct jvxConnectionParams (TjvxTypes_cpp.h) as exposed
// via the FFI struct "connection_params" (flutter_native_types.h).
/*
class AudYoFloOneConnectorParams {
  int buffersize = 0;
  int rate = 0;
  int numberChannels = 0;
  jvxDataFormatEnum format = jvxDataFormatEnum.JVX_DATAFORMAT_NONE;
  jvxDataFormatGroupEnum formatGroup =
      jvxDataFormatGroupEnum.JVX_DATAFORMAT_GROUP_NONE;
  jvxDataflowEnum dataFlow = jvxDataflowEnum.JVX_DATAFLOW_PUSH_ACTIVE;
  int segmentationX = 0;
  int segmentationY = 0;
  AudYoFloCBitField64 additionalFlags = AudYoFloCBitField64();
  String formatSpec = '';

  // Compact one-line representation, meant to fit into a small UI tile.
  String get shortText =>
      'Buffersize: $buffersize, Rate: $rate, Channels: $numberChannels, '
      'Format: ${format.txt}';

  // Full representation of all connection parameters, meant for a tooltip.
  String get fullText {
    String txt = 'Buffersize: $buffersize, Rate: $rate, '
        'Channels: $numberChannels, Format: ${format.txt}, '
        'Group: ${formatGroup.txt}, Dataflow: ${dataFlow.txt}, '
        'Segmentation: $segmentationX x $segmentationY';
    if (formatSpec.isNotEmpty) {
      txt += ', Spec: $formatSpec';
    }
    return txt;
  }
}
*/
// One input or output connector: its descriptor plus the currently valid
// connection (processing) parameters.
/*
class AudYoFloOneConnectorEntry {
  String descriptor = 'not-set';
  int connectorIdx = -1;
  bool isInput = true;
  AudYoFloOneConnectorParams params = AudYoFloOneConnectorParams();
}
*/

// Cache of all input/output connectors and their connection parameters for
// one selected component. This is refreshed on demand, similar to the
// property cache. It is invalidated as a whole whenever the backend reports
// that the data chain containing this component has been (re-)tested
// (JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN).
class AudYoFloConnectorsComponentsCache {
  List<JvxConnector> inputConnectors = [];
  List<JvxConnector> outputConnectors = [];
  bool valid = false;
  int ssUpdateId = 0;

  void invalidate() {
    valid = false;
    ssUpdateId++;
  }
}
