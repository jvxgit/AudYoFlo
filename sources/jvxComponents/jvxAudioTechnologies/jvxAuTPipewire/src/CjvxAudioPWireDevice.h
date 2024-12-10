#ifndef __CJVXAUDIOPWIREDEVICE_H__
#define __CJVXAUDIOPWIREDEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"
#include "jvxAudioTechnologies/CjvxMixedDevicesAudioTechnology.h"
#include "jvxAudioTechnologies/CjvxMixedDevicesAudioDevice.h"
#include "pcg_exports_device.h"

#include <pipewire/pipewire.h>
#include <spa/param/latency-utils.h>
#include <spa/param/audio/format-utils.h>

// class CjvxAudioPWireTechnology<CjvxAudioPWireDevice>;

class oneDevice;
class CjvxAudioPWireTechnology;


class CjvxAudioPWireDevice: public CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>, public genPWire_device
{
protected:
	oneDevice* theDevicehandle = nullptr;
	CjvxAudioPWireTechnology* parent = nullptr;
	jvxBool loopLocal = true;
    pw_thread_loop* loop_dev = nullptr;

	struct
	{
		struct
		{
			struct pw_filter *filter = nullptr;
			void **in_ports = nullptr;
			void **out_ports = nullptr;
			float **in_data = nullptr;
			float **out_data = nullptr;
		} inProcessing;

		struct spa_process_latency_info lat_info =
			{
				.ns = 10 * SPA_NSEC_PER_MSEC};		
	} duplex;

	struct 
	{
		struct pw_stream_events stream_events = {
			PW_VERSION_STREAM_EVENTS,
			.process = nullptr,
		};
		 pw_stream *stream = nullptr;
		 jvxData accumulator = 0;

		 int16_t *dst_i16 = nullptr;
        jvxData *dst_data = nullptr;
	} output;

	struct 
	{
		struct pw_stream_events stream_events = {
			PW_VERSION_STREAM_EVENTS,
			.process = nullptr,
		};
		 pw_stream *stream = nullptr;		 
	} input;

	struct
	{
		pw_thread_loop* loop = nullptr;
		jvxSize numChansInMax = 0;
		jvxSize numChansOutMax = 0;
		jvxBitField maskInput = 0;
		jvxBitField maskOutput = 0;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
		jvxSize rate = 48000;
		jvxSize bsize = 1024;
		struct spa_audio_info_raw aud_info = {
			.format = SPA_AUDIO_FORMAT_S16,
			.rate = 48000,
			.channels = 2};


	} common;

public:
	JVX_CALLINGCONVENTION CjvxAudioPWireDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAudioPWireDevice();

	jvxErrorType  activate_device_api();
	jvxErrorType  deactivate_device_api();

	void set_references_api(oneDevice* theDevicehandle, CjvxAudioPWireTechnology* parentArg);
	oneDevice* references_api();

	jvxErrorType try_match_settings_backward_ocon(jvxLinkDataDescriptor* ld_con JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	// Optional: 
	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	jvxErrorType process_buffers_icon(jvxSize mt_mask = JVX_SIZE_UNSELECTED, jvxSize idx_stage = JVX_SIZE_UNSELECTED) override;

	jvxErrorType start_device_duplex();
	void process_buffer_duplex(struct spa_io_position *position);

	jvxErrorType start_device_input();
	jvxErrorType stop_device_input();
	void process_buffer_input();

	// Simple output device
	jvxErrorType start_device_output_pw();
	jvxErrorType stop_device_output_pw();
	void process_buffer_output_pw();
	jvxErrorType process_buffer_icon_output(jvxSize mt_mask, jvxSize idx_stage);

};

#endif
