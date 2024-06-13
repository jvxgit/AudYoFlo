//import 'dart:html';
import 'dart:typed_data';
import 'package:ayf_pack/ayf_pack.dart';

import 'dart:convert';
import 'package:collection/collection.dart';
import '../../ayf_web_backend_binary.dart';
import './ayf_web_backend_anomalies.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

import '../ayf_web_backend_helpers.dart';
import '../ayf_web_backend_constants.dart';
import '../ayf_web_backend_typedefs.dart';
import './ayf_web_backend_adapter.dart';
import './ayf_web_backend_anomalies.dart';

import 'dart:async';

// ==========================================================================================

class AudYoFloPropertyStreamDefintionConfirmed
    extends AudYoFloPropertyStreamDefintion {
  bool registrationSuccess = false;
  int userId = 0;
  int streamId = 0;

  AudYoFloPropertyStreamDefintionConfirmed(
      AudYoFloPropertyStreamDefintion copyFrom) {
    target = copyFrom.target;
    stateMaskActive = copyFrom.stateMaskActive;
    offset = copyFrom.offset;
    format = copyFrom.format;
    dec_hint_tp = copyFrom.dec_hint_tp;
    num_elements = copyFrom.num_elements;
    param0 = copyFrom.param0;
    param1 = copyFrom.param1;
    cond_update = copyFrom.cond_update;
    param_cond_update = copyFrom.param_cond_update;
    requires_flow_control = copyFrom.requires_flow_control;
    priority = copyFrom.priority;
    port = copyFrom.port;
    num_emit_min = copyFrom.num_emit_min;
    num_emit_max = copyFrom.num_emit_max;
    propertyDescriptor = copyFrom.propertyDescriptor;
  }
}

// ==========================================================================================

class AudYoFloWebPropertyStream {
  // @override
  void Function(JvxComponentIdentification, JvxPropertyStreamMessageType msgT,
      String propertyDescriptor, Uint8List data, int offsel)? callback;
  // jvxComponentIdentification cpId;

  Completer<void>? completeWebSocketTransactionBinary;

  AudYoFloWebBackendAnomalies? reportRef;

  WebSocketChannel? channelBinary;

  JvxBinSocketConnection conStat =
      JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_NONE;

  String wsTarget = "";

  Map<int, AudYoFloPropertyStreamDefintionConfirmed> propDescriptionListOnInit =
      {};
  Map<int, AudYoFloPropertyStreamDefintionConfirmed>
      propDescriptionListConfirmed = {};

  // ==================================================================

  void initializePropStream(
      String wsTargetArg, AudYoFloWebBackendAnomalies? reportRefArg) {
    wsTarget = wsTargetArg;
    reportRef = reportRefArg;
  }

  // ===============================================================================
  Future<int> triggerStopPropertyStream() async {
    if (conStat != JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_NONE) {
      channelBinary?.sink.close();
    }
    return jvxErrorType.JVX_NO_ERROR;
  }

// ===============================================================================
  Future<int> triggerStartPropertyStream(
      List<AudYoFloPropertyStreamDefintion> propDescriptions,
      Function(JvxComponentIdentification, JvxPropertyStreamMessageType msgT,
              String propertyDescriptor, Uint8List data, int offsel)
          cbArg,
      {int desired_timeout_msec = 1000,
      int desired_ping_granularity = 10}) async {
    if (conStat == JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_NONE) {
      callback = cbArg;
      String address = '/jvx/host/socket/';
      int cnt = 1;
      for (var elm in propDescriptions) {
        AudYoFloPropertyStreamDefintionConfirmed oneElm =
            AudYoFloPropertyStreamDefintionConfirmed(elm);
        propDescriptionListOnInit[cnt] = oneElm;
        cnt++;
      }
      try {
        channelBinary = WebSocketChannel.connect(Uri.parse(wsTarget + address));
      } catch (exc) {
        print('$exc');
      }

      channelBinary!.stream.listen(onMessageBinary,
          onDone: onConnectionDoneBinary, onError: onConnectionErrorBinary);

      // ===========================================================================
      // ===========================================================================

      JvxPropertyConfigurePropertySend hdrTrans =
          JvxPropertyConfigurePropertySend();

      hdrTrans.loc_header.paketsize_ui32 =
          JvxPropertyConfigurePropertySend.sizeofType();
      hdrTrans.loc_header.loc_header.proto_family_ui16 =
          JvxProtocolType.JVX_PROTOCOL_TYPE_PROPERTY_STREAM.index;
      hdrTrans.loc_header.purpose_ui16 =
          JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST |
              JvxPsType.JVX_PS_CONFIGURE_PROPERTY_OBSERVATION.index;
      hdrTrans.tick_msec_ui16 = desired_timeout_msec;
      hdrTrans.ping_count_ui16 = desired_ping_granularity;

      List<int> handshake = hdrTrans.serialize();

      completeWebSocketTransactionBinary = Completer<void>();
      channelBinary!.sink.add(handshake);
      conStat = JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_INIT;
      await completeWebSocketTransactionBinary!.future;

      // ===========================================================================
      // ===========================================================================
      if (conStat == JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_STARTED) {
        int cnt = 1;
        jvxPropertyPropertyObserveHeader newPropAssign =
            jvxPropertyPropertyObserveHeader();
        newPropAssign.loc_header.loc_header.proto_family_ui16 =
            JvxProtocolType.JVX_PROTOCOL_TYPE_PROPERTY_STREAM.index;
        newPropAssign.loc_header.purpose_ui16 =
            JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST |
                JvxPsType.JVX_PS_ADD_PROPERTY_TO_OBSERVE.index;
        for (var elm in propDescriptionListOnInit.entries) {
          String property =
              elm.value.propertyDescriptor; //'/my_property_registered';
          newPropAssign.loc_header.paketsize_ui32 =
              jvxPropertyPropertyObserveHeader.sizeofType() + property.length;
          newPropAssign.user_specific_ui32 = elm.key;

          cnt++;
          List<int> assignFld = newPropAssign.serialize();
          var str = ascii.encode(property);
          for (var elm in str) {
            assignFld.add(elm);
          }
          completeWebSocketTransactionBinary = Completer<void>();
          channelBinary!.sink.add(assignFld);

          await completeWebSocketTransactionBinary!.future;
        }

        // We might check that all properties have been moved from the one to the other list!!
        propDescriptionListOnInit.clear();
      } else {
        dbgPrint("Binary Socket Connection failed!");
      }
    }
    return jvxErrorType.JVX_NO_ERROR;
  }

  void onMessageBinary(dynamic body) async {
    Uint8List lst = body;
    bool messageIsAResponse = false;

    // Run a state machine
    switch (conStat) {
      case JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_INIT:

        // I this state, we expect the first message from other side
        jvxPropertyConfigurePropertySend_response resp =
            jvxPropertyConfigurePropertySend_response();
        resp.fromIntList(lst);
        if ((resp.loc_header.loc_header.proto_family_ui16 ==
                JvxProtocolType.JVX_PROTOCOL_TYPE_PROPERTY_STREAM.index) &&
            (resp.loc_header.purpose_ui16 ==
                (JvxPsType.JVX_PS_CONFIGURE_PROPERTY_OBSERVATION.index |
                    JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)) &&
            (resp.errcode_ui16 == jvxErrorType.JVX_NO_ERROR)) {
          conStat = JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_STARTED;
          messageIsAResponse = true;
        }
        break;
      case JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_STARTED:

        // Here, we are in full operation. Either, we get a response regarding the newly installed property or
        // we receive a streamed property

        // First, decode the first part only
        jvxProtocolHeader firstPartResponse = jvxProtocolHeader();
        firstPartResponse.fromIntList(lst);
        if ((firstPartResponse.loc_header.proto_family_ui16 ==
            JvxProtocolType.JVX_PROTOCOL_TYPE_PROPERTY_STREAM.index)) {
          if (firstPartResponse.purpose_ui16 ==
              (JvxPsType.JVX_PS_ADD_PROPERTY_TO_OBSERVE.index |
                  JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)) {
            messageIsAResponse = true;
            // Now, decode the full message
            jvxPropertyPropertyObserveHeader_response resp =
                jvxPropertyPropertyObserveHeader_response();
            resp.fromIntList(lst);
            if (resp.errcode_ui16 == jvxErrorType.JVX_NO_ERROR) {
              // Re-locate the pending entry
              var elm = propDescriptionListOnInit.entries.firstWhereOrNull(
                  (element) => element.key == resp.user_specific_ui32);
              if (elm == null) {
                print(
                    'Received message to confirm property with user id ${resp.user_specific_ui32} - which is not known in the list of requested properties.');
              } else {
                // Store in list of confirmed properties
                AudYoFloPropertyStreamDefintionConfirmed copyElm = elm.value;
                copyElm.streamId = resp.stream_id_ui16;
                propDescriptionListConfirmed[resp.stream_id_ui16] = copyElm;

                // propDescriptionListOnInit.remove(resp.user_specific_ui32); <- we should ot remove that element here: this callback is the result of a call in a for-loop which breaks otherwise

                print(
                    'Property ${copyElm.propertyDescriptor} confirmed by backend.');
              }
            } else {
              String errDescr =
                  jvxErrorTypeEInt.toStringSingle(resp.errcode_ui16);
              print(
                  'Registration of property with user id ${resp.user_specific_ui32} returned error code $errDescr.');
            }
          } else if (firstPartResponse.purpose_ui16 ==
              (JvxPsType.JVX_PS_SEND_LINEARFIELD.index |
                  JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER)) {
            jvxPropertyPropertyObserveHeader_response respData =
                jvxPropertyPropertyObserveHeader_response();
            respData.fromIntList(lst);

            // Match stream id to registered elemnent
            var elm = propDescriptionListConfirmed.entries.firstWhereOrNull(
                (element) => element.key == respData.stream_id_ui16);
            if (elm != null) {
              if (callback != null) {
                callback!(
                    JvxComponentIdentification(
                        cpTp: JvxComponentTypeEEnum.fromInt(
                            respData.component_type_ui16),
                        slotid: respData.component_slot_ui16,
                        slotsubid: respData.component_subslot_ui16),
                    JvxPropertyStreamMessageType
                        .JVX_PROP_STREAM_SEND_LINEARFIELD,
                    elm.value.propertyDescriptor,
                    lst,
                    jvxPropertyPropertyObserveHeader_response.sizeofType());
              }
            } else {
              // Error case
            }
          } else if (firstPartResponse.purpose_ui16 ==
              (JvxPsType.JVX_PS_SEND_CIRCFIELD.index |
                  JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER)) {
            jvxPropertyPropertyObserveHeader_response respData =
                jvxPropertyPropertyObserveHeader_response();
            respData.fromIntList(lst);

            if (callback != null) {
              callback!(
                  JvxComponentIdentification(
                      cpTp: JvxComponentTypeEEnum.fromInt(
                          respData.component_type_ui16),
                      slotid: respData.component_slot_ui16,
                      slotsubid: respData.component_subslot_ui16),
                  JvxPropertyStreamMessageType.JVX_PROP_STREAM_SEND_CIRCFIELD,
                  '',
                  lst,
                  jvxPropertyPropertyObserveHeader_response.sizeofType());
            }
          }

          /*JvxPropertyStreamMessageType
                    JVX_PS_SEND_LINEARFIELD,
  JVX_PS_SEND_CIRCFIELD*/
        }
        break;
      default:
        break;
    }
    if (messageIsAResponse) {
      if (!completeWebSocketTransactionBinary!.isCompleted) {
        completeWebSocketTransactionBinary!.complete(null);
      }
    }
  }

  void onConnectionDoneBinary() {
    conStat = JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_NONE;
    print(
        'Ws connection reports a disconnect. Informing app to switch to disconnect status.');
    reportRef!.reportStatusDisconnect(0, 0, 'Websocket disconnected');
  }

  void onConnectionErrorBinary(error) {
    conStat = JvxBinSocketConnection.JVX_BIN_SOCKET_CONNECTION_NONE;
    print('Ws connection reports a connection error, reason:  <$error>');
    reportRef!.reportStatusDisconnect(1, 0, error);
  }
}
