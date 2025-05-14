import 'dart:ffi';
import 'package:ffi/ffi.dart';
import '../native-ffi/generated/ayf_ffi_gen_bind.dart';
import 'package:ayf_pack/ayf_pack.dart';

class jvxSequencerEventTypeNative {
  static jvxSequencerEventType decodeSequencerEventType(
      Pointer<one_sequencer_event> evPtr) {
    jvxSequencerEventType retVal = jvxSequencerEventType();
    one_sequencer_event ev = evPtr.ref;

    retVal.eventMask = ev.event_mask;
    retVal.fId = ev.fId;
    retVal.indicateFirstError = (ev.indicateFirstError != 0);
    retVal.qtp = jvxSequencerQueueTypeEEnum.fromInt(ev.qtp);
    retVal.seq_state_id = ev.seq_state_id;
    retVal.sequenceId = ev.sequenceId;
    retVal.setp = jvxSequencerElementTypeEEnum.fromInt(ev.setp);
    retVal.stepId = ev.stepId;
    retVal.statSeq = jvxSequencerStatusEEnum.fromInt(ev.statSeq);

    Pointer<Char> dPtr = ev.description;
    Pointer<Utf8> ptrU8 = dPtr.cast<Utf8>();
    retVal.description = ptrU8.toDartString();

    return retVal;
  }
}
