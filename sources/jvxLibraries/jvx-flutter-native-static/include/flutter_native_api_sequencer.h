#ifndef __FLUTTER_NATIVE_API_SEQUENCER_H__
#define __FLUTTER_NATIVE_API_SEQUENCER_H__

// Refer to file flutter_native_alloc.cpp
int ffi_sequencer_status(void* opaque_hdl);
int ffi_sequencer_start(void* opaque_hdl, int granularity_state_report);
int ffi_sequencer_stop(void* opaque_hdl);
int ffi_sequencer_ack_stop(void* opaque_hdl);
int ffi_sequencer_trigger(void* opaque_hdl);

#endif