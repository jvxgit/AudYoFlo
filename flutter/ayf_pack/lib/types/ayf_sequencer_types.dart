import 'package:flutter/foundation.dart';

// =======================================================================
enum jvxSequencerStatusEnum {
  JVX_SEQUENCER_STATUS_NONE,
  JVX_SEQUENCER_STATUS_STARTUP,
  JVX_SEQUENCER_STATUS_IN_OPERATION,
  JVX_SEQUENCER_STATUS_WAITING,
  JVX_SEQUENCER_STATUS_WANTS_TO_STOP_PROCESS,
  JVX_SEQUENCER_STATUS_WANTS_TO_STOP_SEQUENCE,
  JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS,
  JVX_SEQUENCER_STATUS_ERROR,
  JVX_SEQUENCER_STATUS_SHUTDOWN_COMPLETE
}

extension jvxSequencerStatusEEnum on jvxSequencerStatusEnum {
  String get txt => describeEnum(this);
  static jvxSequencerStatusEnum fromInt(int val) {
    jvxSequencerStatusEnum ss =
        jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;
    if (val < jvxSequencerStatusEnum.values.length) {
      ss = jvxSequencerStatusEnum.values[val];
    }
    return ss;
  }
}

// =====================================================================

enum jvxSequencerQueueTypeEnum {
  JVX_SEQUENCER_QUEUE_TYPE_NONE,
  JVX_SEQUENCER_QUEUE_TYPE_RUN,
  JVX_SEQUENCER_QUEUE_TYPE_LEAVE,
  JVX_SEQUENCER_QUEUE_TYPE_LIMIT
}

extension jvxSequencerQueueTypeEEnum on jvxSequencerQueueTypeEnum {
  String get txt => describeEnum(this);
  static jvxSequencerQueueTypeEnum fromInt(int val) {
    jvxSequencerQueueTypeEnum ss =
        jvxSequencerQueueTypeEnum.JVX_SEQUENCER_QUEUE_TYPE_NONE;
    if (val < jvxSequencerQueueTypeEnum.values.length) {
      ss = jvxSequencerQueueTypeEnum.values[val];
    }
    return ss;
  }
}

enum jvxSequencerElementTypeEnum {
  JVX_SEQUENCER_TYPE_COMMAND_NONE,
  JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_ACTIVATE,
  JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE,
  JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START,
  JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP,
  JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS,
  JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_DEACTIVATE,
  JVX_SEQUENCER_TYPE_CONDITION_WAIT,
  JVX_SEQUENCER_TYPE_CONDITION_JUMP,
  JVX_SEQUENCER_TYPE_JUMP,
  JVX_SEQUENCER_TYPE_COMMAND_SPECIFIC,
  JVX_SEQUENCER_TYPE_WAIT_FOREVER,
  JVX_SEQUENCER_TYPE_WAIT_CONDITION_RELATIVE_JUMP,
  JVX_SEQUENCER_TYPE_BREAK,
  JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER,
  JVX_SEQUENCER_TYPE_REQUEST_START_IN_PROCESSING_LOOP,
  JVX_SEQUENCER_TYPE_REQUEST_STOP_IN_PROCESSING_LOOP,
  JVX_SEQUENCER_TYPE_CALLBACK,
  JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE,
  JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START,
  JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP,
  JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS,
  JVX_SEQUENCER_TYPE_COMMAND_OUTPUT_TEXT,
  JVX_SEQUENCER_TYPE_COMMAND_CALL_SEQUENCE,
  JVX_SEQUENCER_TYPE_COMMAND_SWITCH_STATE,
  JVX_SEQUENCER_TYPE_COMMAND_RESET_STATE,
  JVX_SEQUENCER_TYPE_COMMAND_PROCESS_TEST,
  JVX_SEQUENCER_TYPE_COMMAND_INTERCEPT,
  JVX_SEQUENCER_TYPE_COMMAND_NOP,
  JVX_SEQUENCER_TYPE_COMMAND_SET_PROPERTY,
  JVX_SEQUENCER_TYPE_COMMAND_INVOKE_ERROR,
  JVX_SEQUENCER_TYPE_COMMAND_INVOKE_FATAL_ERROR,
  JVX_SEQUENCER_TYPE_UNCONDITION_WAIT,
  JVX_SEQUENCER_TYPE_WAIT_AND_RUN_TASKS
}

extension jvxSequencerElementTypeEEnum on jvxSequencerElementTypeEnum {
  String get txt => describeEnum(this);
  static jvxSequencerElementTypeEnum fromInt(int val) {
    jvxSequencerElementTypeEnum ss =
        jvxSequencerElementTypeEnum.JVX_SEQUENCER_TYPE_COMMAND_NONE;
    if (val < jvxSequencerElementTypeEnum.values.length) {
      ss = jvxSequencerElementTypeEnum.values[val];
    }
    return ss;
  }
}

// ============================================================

abstract class jvxSequencerEventBitfield {
  static const int JVX_SEQUENCER_EVENT_NONE = 0;
  static const int JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE = 0x1;
  static const int JVX_SEQUENCER_EVENT_SEQUENCE_STARTUP_COMPLETE = 0x2;
  static const int JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP = 0x4;
  static const int JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR = 0x8;
  static const int JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE = 0x10;
  static const int JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR =
      0x20;
  static const int JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT =
      0x40;
  static const int JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS = 0x80;
  static const int JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR =
      0x100;
  static const int JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT =
      0x200;
  static const int JVX_SEQUENCER_EVENT_PROCESS_TERMINATED = 0x400;
  static const int JVX_SEQUENCER_EVENT_INFO_TEXT = 0x800;
  static const int JVX_SEQUENCER_EVENT_DEBUG_MESSAGE = 0x1000;
  static const int JVX_SEQUENCER_EVENT_REPORT_OPERATION_STATE = 0x2000;
  static const int JVX_SEQUENCER_EVENT_REPORT_ENTERED_STEP_BREAK = 0x4000;
  static const int JVX_SEQUENCER_EVENT_REPORT_SKIPPED_STEP = 0x8000;
}

class jvxSequencerEventType {
  int eventMask = jvxSequencerEventBitfield.JVX_SEQUENCER_EVENT_NONE;
  String description = '';
  int sequenceId = -1;
  int stepId = -1;
  int fId = -1;
  int seq_state_id = -1;
  jvxSequencerStatusEnum statSeq =
      jvxSequencerStatusEnum.JVX_SEQUENCER_STATUS_NONE;

  jvxSequencerQueueTypeEnum qtp =
      jvxSequencerQueueTypeEnum.JVX_SEQUENCER_QUEUE_TYPE_NONE;
  jvxSequencerElementTypeEnum setp =
      jvxSequencerElementTypeEnum.JVX_SEQUENCER_TYPE_COMMAND_NONE;

  bool indicateFirstError = false;
}
