import '../ayf_pack_local.dart';

class AudYoFloStringTranslator {
  static int translateEnumString(
      String selection, String formatName, AudYoFloCompileFlags flags) {
    int retVal = -1;
    if (formatName == "jvxDataFormat") {
      retVal = jvxDataFormatEnum.JVX_DATAFORMAT_NONE.index;
      String tp = 'double';
      if (flags != null) {
        if (flags.compileDataTypeSpec ==
            jvxDataTypeSpecEnum.JVX_DATA_TYPE_SPEC_FLOAT) {
          tp = 'single';
        }
      }
      tp = 'jvxData' + '(' + tp + ')';
      if (selection == tp) {
        retVal = jvxDataFormatEnum.JVX_DATAFORMAT_DATA.index;
      } else {
        switch (selection) {
          case 'none':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_NONE.index;
            break;
          case 'jvxData':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_DATA.index;
            break;

          case '16 Bit le':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE.index;
            break;

          case '32 Bit le':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE.index;
            break;

          case '64 Bit le':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE.index;
            break;

          case '8 Bit':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_8BIT.index;
            break;

          case 'size':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_SIZE.index;
            break;

          case 'string':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_STRING.index;
            break;

          case 'string list':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_STRING_LIST.index;
            break;

          case 'selection list':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_SELECTION_LIST.index;
            break;

          case 'value in range':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_VALUE_IN_RANGE.index;
            break;

          case 'u16 Bit le':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE.index;
            break;

          case 'u32 Bit le':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE.index;
            break;

          case 'u64 Bit le':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE.index;
            break;

          case 'u8 Bit':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT.index;
            break;

          case 'float':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_FLOAT.index;
            break;

          case 'double':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_DOUBLE.index;
            break;

          case 'byte':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_BYTE.index;
            break;

          case 'handle':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_HANDLE.index;
            break;

          case 'pointer':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_POINTER.index;
            break;

          case 'callback':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_CALLBACK.index;
            break;

          case 'interface':
            retVal = jvxDataFormatEnum.JVX_DATAFORMAT_INTERFACE.index;
            break;

          default:
            print(
                'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');

            break;
        }
      }
    } else if (formatName == "jvxPropertyCategoryType") {
      retVal = jvxPropertyCategoryType.JVX_PROPERTY_CATEGORY_UNKNOWN;
      switch (selection) {
        case 'unknown':
          retVal = jvxPropertyCategoryType.JVX_PROPERTY_CATEGORY_UNKNOWN;
          break;
        case 'predefined':
          retVal = jvxPropertyCategoryType.JVX_PROPERTY_CATEGORY_PREDEFINED;
          break;
        case 'unspecific':
          retVal = jvxPropertyCategoryType.JVX_PROPERTY_CATEGORY_UNSPECIFIC;
          break;
        default:
          print(
              'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');

          break;
      }
    } else if (formatName == 'jvxPropertyDecoderHintType') {
      {
        retVal = jvxPropertyDecoderHintTypeEnum.JVX_PROPERTY_DECODER_NONE.index;
        switch (selection) {
          case 'none':
            retVal =
                jvxPropertyDecoderHintTypeEnum.JVX_PROPERTY_DECODER_NONE.index;
            break;
          case 'file name (open)':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_FILENAME_OPEN.index;
            break;

          case 'directory (select)':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_DIRECTORY_SELECT.index;
            break;

          case 'file name (save)':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_FILENAME_SAVE.index;
            break;

          case 'ip address':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_IP_ADDRESS.index;
            break;

          case 'plot array':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_PLOT_ARRAY.index;
            break;

          case 'bit field':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_BITFIELD.index;
            break;

          case 'single selection':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_SINGLE_SELECTION.index;
            break;

          case 'multi selection':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_MULTI_SELECTION.index;
            break;

          case 'format (index)':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_FORMAT_IDX.index;
            break;

          case 'progress bar':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_PROGRESSBAR.index;
            break;

          case 'input file list':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_INPUT_FILE_LIST.index;
            break;

          case 'output file list':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST.index;
            break;

          case 'multi selection change order':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER.index;
            break;

          case 'multi channel circ buffer':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER.index;
            break;

          case 'multi channel switch buffer':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER.index;
            break;

          case 'command':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_COMMAND.index;
            break;

          case 'simple onoff':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_SIMPLE_ONOFF.index;
            break;

          case 'value or dont care':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE.index;
            break;

          case 'subformat (index)':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_SUBFORMAT_IDX.index;
            break;

          case 'dataflow (index)':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_DATAFLOW_IDX.index;
            break;

          case 'enum type':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_ENUM_TYPE.index;
            break;

          case 'property extender':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE.index;
            break;

          case 'local text log':
            retVal = jvxPropertyDecoderHintTypeEnum
                .JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG.index;
            break;

          default:
            print(
                'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');

            break;
        }
      }
    } else if (formatName == 'jvxPropertyAccessType') {
      retVal = jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_NONE.index;
      switch (selection) {
        case 'none':
          retVal = jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_NONE.index;
          break;
        case 'full read + write':
          retVal = jvxPropertyAccessTypeEnum
              .JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE.index;
          break;

        case 'read + write':
          retVal = jvxPropertyAccessTypeEnum
              .JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT.index;
          break;

        case 'read':
          retVal =
              jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_READ_ONLY.index;
          break;

        case 'reados':
          retVal = jvxPropertyAccessTypeEnum
              .JVX_PROPERTY_ACCESS_READ_ONLY_ON_START.index;
          break;

        case 'write':
          retVal =
              jvxPropertyAccessTypeEnum.JVX_PROPERTY_ACCESS_WRITE_ONLY.index;
          break;
        default:
          print(
              'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');

          break;
      }
    } else if (formatName == "jvxState") {
      retVal = 0;

      // We may correct here in the future: If multiples states are combined in the text expression.
      // It seems that this is currently not in use.
      if (selection == 'none') {
        retVal |= jvxState.JVX_STATE_NONE;
      }
      if (selection == 'selected') {
        retVal |= jvxState.JVX_STATE_SELECTED;
      }
      if (selection == 'active') {
        retVal |= jvxState.JVX_STATE_ACTIVE;
      }
      if (selection == 'prepared') {
        retVal |= jvxState.JVX_STATE_PREPARED;
      }
      if (selection == 'prepared') {
        retVal |= jvxState.JVX_STATE_PREPARED;
      }
      if (selection == 'processing') {
        retVal |= jvxState.JVX_STATE_PROCESSING;
      }
      if (selection == 'complete') {
        retVal |= jvxState.JVX_STATE_COMPLETE;
      }

      // A return value of "0" indicates a problem!
      assert(retVal != 0);
    } else if (formatName == "jvxDeviceDataFlowType") {
      retVal = jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_UNKNOWN.index;
      switch (selection) {
        case 'unknown':
          retVal = jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_UNKNOWN.index;
          break;
        case 'active':
          retVal = jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_ACTIVE.index;
          break;

        case 'passive':
          retVal = jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_PASSIVE.index;
          break;

        case 'offline':
          retVal = jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_OFFLINE.index;
          break;

        case 'control':
          retVal = jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_CONTROL.index;
          break;
        default:
          print(
              'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');
          break;
      }
    } else if (formatName == "jvxDeviceCapabilityType") {
      retVal = 0;
      var tokens = selection.split('|');
      for (var elmT in tokens) {
        if (elmT == 'Unknown') {
          retVal |= 1 <<
              jvxDeviceCapabilityTypeShift
                  .JVX_DEVICE_CAPABILITY_UNKNOWN_SHIFT.index;
          break;
        }
        if (elmT == 'Input') {
          retVal |= 1 <<
              jvxDeviceCapabilityTypeShift
                  .JVX_DEVICE_CAPABILITY_INPUT_SHIFT.index;
          break;
        }
        if (elmT == 'Output') {
          retVal |= 1 <<
              jvxDeviceCapabilityTypeShift
                  .JVX_DEVICE_CAPABILITY_OUTPUT_SHIFT.index;
          break;
        }
        if (elmT == 'Duplex') {
          retVal |= 1 <<
              jvxDeviceCapabilityTypeShift
                  .JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT.index;
          break;
        }
        if (elmT == 'Other') {
          retVal |= 1 <<
              jvxDeviceCapabilityTypeShift
                  .JVX_DEVICE_CAPABILITY_OTHER_SHIFT.index;
          break;
        }
      }
    } else if (formatName == "jvxReportCommandRequest") {
      // cmdreq-unspecific
      retVal = jvxReportCommandRequestEnum
          .JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC.index;
      switch (selection) {
        case 'unspecific':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC.index;
          break;

        case 'cmdreq-upd-cp-lst':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST.index;
          break;

        case 'cmdreq-upd-stat-cp-lst':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT_LIST.index;
          break;

        case 'cmdreq-upd-stat-cp':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT.index;
          break;

        case 'cmdreq-sys-stat':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED.index;
          break;

        case 'cmdreq-trig-seq-imm':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE.index;
          break;

        case 'cmdreq-upd-props':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES.index;
          break;

        case 'cmdreq-resched-main':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_RESCHEDULE_MAIN.index;
          break;

        case 'cmdreq-born-sub-dev':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE.index;
          break;

        case 'cmdreq-died-sub-dev':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE.index;
          break;

        case 'cmdreq-born-comp':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_COMPONENT.index;
          break;

        case 'cmdreq-died-comp':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_COMPONENT.index;
          break;

        case 'cmdreq-proc-connect':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED.index;
          break;

        case 'cmdreq-proc-discon':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED
              .index;
          break;

        case 'cmdreq-req-test-chain':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN.index;
          break;

        case 'cmdreq-rep-cp-sswitch':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH.index;
          break;

        case 'cmdreq-config-complete':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE.index;
          break;

        case 'cmdreq-rep-seq-evt':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_EVENT.index;
          break;

        case 'cmdreq-rep-seq_cb':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_CALLBACK.index;
          break;

        case 'cmdreq-proc-dis-cmplt':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_DISCONNECT_COMPLETE
              .index;
          break;

        case 'cmdreq-req-upd-prop':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY.index;
          break;

        case 'cmdreq-proc-test':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_REPORT_TEST_SUCCESS.index;
          break;
        case 'cmdreq-req-test-chain-run':
          retVal = jvxReportCommandRequestEnum
              .JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN_RUN.index;
          break;
        default:
          print(
              'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');
          break;
      }
    } else if (formatName == "jvxReportCommandDataType") {
      retVal = jvxReportCommandDataTypeEnum
          .JVX_REPORT_COMMAND_TYPE_BASE.index; // 'cmdreqtp-base'
      switch (selection) {
        case 'cmdreqtp-base':
          retVal =
              jvxReportCommandDataTypeEnum.JVX_REPORT_COMMAND_TYPE_BASE.index;
          break;

        case 'cmdreqtp-sched':
          retVal = jvxReportCommandDataTypeEnum
              .JVX_REPORT_COMMAND_TYPE_SCHEDULE.index;
          break;

        case 'cmdreqtp-ident':
          retVal =
              jvxReportCommandDataTypeEnum.JVX_REPORT_COMMAND_TYPE_IDENT.index;
          break;

        case 'cmdreqtp-uid':
          retVal =
              jvxReportCommandDataTypeEnum.JVX_REPORT_COMMAND_TYPE_UID.index;
          break;

        case 'cmdreqtp-ss':
          retVal =
              jvxReportCommandDataTypeEnum.JVX_REPORT_COMMAND_TYPE_SS.index;
          break;
        case 'cmdreqtp-seq':
          retVal =
              jvxReportCommandDataTypeEnum.JVX_REPORT_COMMAND_TYPE_SEQ.index;
          break;
        default:
          print(
              'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');
          break;
      }
    } else if (formatName == 'jvxStateSwitch') {
      retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_NONE.index;
      switch (selection) {
        case 'none':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_NONE.index;
          break;

        case 'initialize':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_INIT.index;
          break;

        case 'select':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_SELECT.index;
          break;

        case 'activate':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_ACTIVATE.index;
          break;

        case 'prepare':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_PREPARE.index;
          break;

        case 'start':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_START.index;
          break;

        case 'stop':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_STOP.index;
          break;

        case 'postprocess':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_POSTPROCESS.index;
          break;

        case 'deactivate':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_DEACTIVATE.index;
          break;

        case 'unselect':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_UNSELECT.index;
          break;

        case 'terminate':
          retVal = jvxStateSwitchEnum.JVX_STATE_SWITCH_TERMINATE.index;
          break;

        default:
          print(
              'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');
          break;
      }
    } else if (formatName == 'jvxSequencerQueueType') {
      retVal = jvxSequencerQueueTypeEnum.JVX_SEQUENCER_QUEUE_TYPE_NONE.index;
      switch (selection) {
        case 'none':
          retVal =
              jvxSequencerQueueTypeEnum.JVX_SEQUENCER_QUEUE_TYPE_NONE.index;
          break;
        case 'queue_type_run':
          retVal = jvxSequencerQueueTypeEnum.JVX_SEQUENCER_QUEUE_TYPE_RUN.index;
          break;
        case 'queue_type_leave':
          retVal =
              jvxSequencerQueueTypeEnum.JVX_SEQUENCER_QUEUE_TYPE_LEAVE.index;
          break;
        default:
          print(
              'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');
          break;
      }
    } else if (formatName == 'jvxSequencerElementType') {
      retVal =
          jvxSequencerElementTypeEnum.JVX_SEQUENCER_TYPE_COMMAND_NONE.index;
      switch (selection) {
        case 'none':
          retVal =
              jvxSequencerElementTypeEnum.JVX_SEQUENCER_TYPE_COMMAND_NONE.index;
          break;

        case 'activate':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_ACTIVATE.index;
          break;

        case 'prepare':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE.index;
          break;

        case 'start':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START.index;
          break;

        case 'stop':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP.index;
          break;

        case 'post process':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS.index;
          break;

        case 'deactivate':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_DEACTIVATE.index;
          break;

        case 'condition wait':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_CONDITION_WAIT.index;
          break;

        case 'condition jump':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_CONDITION_JUMP.index;
          break;

        case 'jump':
          retVal = jvxSequencerElementTypeEnum.JVX_SEQUENCER_TYPE_JUMP.index;
          break;

        case 'command':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_SPECIFIC.index;
          break;

        case 'wait until stop':
          retVal =
              jvxSequencerElementTypeEnum.JVX_SEQUENCER_TYPE_WAIT_FOREVER.index;
          break;

        case 'relative jump':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_WAIT_CONDITION_RELATIVE_JUMP.index;
          break;

        case 'break':
          retVal = jvxSequencerElementTypeEnum.JVX_SEQUENCER_TYPE_BREAK.index;
          break;

        case 'request viewer update':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER.index;
          break;

        case 'start processing loop':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_REQUEST_START_IN_PROCESSING_LOOP.index;
          break;

        case 'stop processing loop':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_REQUEST_STOP_IN_PROCESSING_LOOP.index;
          break;

        case 'callback':
          retVal =
              jvxSequencerElementTypeEnum.JVX_SEQUENCER_TYPE_CALLBACK.index;
          break;

        case 'proc prepare':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE.index;
          break;

        case 'proc start':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START.index;
          break;

        case 'proc stop':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP.index;
          break;

        case 'proc post process':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS.index;
          break;

        case 'output text':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_OUTPUT_TEXT.index;
          break;

        case 'call seq':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_CALL_SEQUENCE.index;
          break;

        case 'switch state':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_SWITCH_STATE.index;
          break;

        case 'reset state':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_RESET_STATE.index;
          break;

        case 'proc test':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_PROCESS_TEST.index;
          break;

        case '':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_INTERCEPT.index;
          break;

        case 'nop':
          retVal =
              jvxSequencerElementTypeEnum.JVX_SEQUENCER_TYPE_COMMAND_NOP.index;
          break;

        case 'set_property':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_SET_PROPERTY.index;
          break;

        case 'error':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_INVOKE_ERROR.index;
          break;

        case 'fata_error':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_COMMAND_INVOKE_FATAL_ERROR.index;
          break;

        case 'uncond wait':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_UNCONDITION_WAIT.index;
          break;

        case 'wait_tasks':
          retVal = jvxSequencerElementTypeEnum
              .JVX_SEQUENCER_TYPE_WAIT_AND_RUN_TASKS.index;
          break;
        default:
          print(
              'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');
          break;
      }
    } else if (formatName == 'jvxSequencerStatus') {
      retVal = jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE.index;
      switch (selection) {
        case 'none':
          retVal = jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE.index;
          break;

        case 'startup':
          retVal = jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_STARTUP.index;
          break;

        case 'in operation':
          retVal =
              jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_IN_OPERATION.index;
          break;

        case 'waiting':
          retVal = jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_WAITING.index;
          break;

        case 'wants stop process':
          retVal = jvxSequencerStatusEnum
              .JVX_SEQUENCER_STATUS_WANTS_TO_STOP_PROCESS.index;
          break;

        case 'wants stop sequence':
          retVal = jvxSequencerStatusEnum
              .JVX_SEQUENCER_STATUS_WANTS_TO_STOP_SEQUENCE.index;
          break;

        case 'shutdown':
          retVal = jvxSequencerStatusEnum
              .JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS.index;
          break;

        case 'error':
          retVal = jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_ERROR.index;
          break;

        case 'shutdown complete':
          retVal = jvxSequencerStatusEnum
              .JVX_SEQUENCER_STATUS_SHUTDOWN_COMPLETE.index;
          break;
        default:
          print(
              'Warning: For datatype <$formatName>, a selection value of <$selection> is not handled!');
          break;
      }
    } else {
      assert(false);
    }
    return retVal;
  }

  static JvxComponentIdentification componentIdentificationFromString(
      String cpIdStr) {
    JvxComponentIdentification cpId = JvxComponentIdentification();
    var lst = cpIdStr.split('<');
    cpId.cpTp = componentTypeFromString(lst[0]);
    if (cpId.cpTp != JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpId.slotid = 0;
      cpId.slotsubid = 0;
    }
    if (lst.length > 1) {
      lst = lst[1].split('>');
      lst = lst[0].split(',');

      // If comma does not work, try -
      if (lst.length == 1) {
        lst = lst[0].split('-');
      }
      cpId.slotsubid = 0;
      cpId.slotid = int.parse(lst[0]);
      if (lst.length > 1) {
        cpId.slotsubid = int.parse(lst[1]);
      }
    }

    cpId.uid = 0;
    return cpId;
  }

  static JvxComponentTypeEnum componentTypeFromString(String inStr) {
    bool foundit = false;
    JvxComponentTypeEnum cpTp = JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN;

    for (int i = JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN.index;
        i < JvxComponentTypeEnum.values.length;
        i++) {
      JvxComponentTypeEnum cpTpTest = JvxComponentTypeEEnum.fromInt(i);
      String str = cpTpTest.txt;
      if (inStr == str) {
        foundit = true;
        cpTp = cpTpTest;
        break;
      }
    }
    if (!foundit) {
      print('Unable to find component type < ${inStr} >.');
    }
    return cpTp;
  }
}
