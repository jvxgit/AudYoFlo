#include "CjvxNetworkSlaveDevice.h"

// ============================================================================
#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif


	void
CjvxNetworkSlaveDevice::update_exposed_properties_no_lock()
{
	jvxSize i;
	CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
	for (i = 0; i < genNetworkSlave_device::integrateiplink.local_audio_config.numberinputchannels.value; i++)
	{
		CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(
			"Input Channel #" + jvx_size2String(i));

		jvx_bitSet(CjvxAudioDevice::properties_active.inputchannelselection.value.selection(), i);
		CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = (jvxBitField)(-1);
	}

	CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
	for (i = 0; i < genNetworkSlave_device::integrateiplink.local_audio_config.numberoutputchannels.value; i++)
	{
		CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(
			"Output Channel #" + jvx_size2String(i));

		jvx_bitSet(CjvxAudioDevice::properties_active.outputchannelselection.value.selection(), i);
		CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = (jvxBitField)(-1);
	}

	genNetworkSlave_device::properties_active.sizesselection.value.entries.clear();
	genNetworkSlave_device::properties_active.sizesselection.value.entries.push_back(jvx_size2String(
		genNetworkSlave_device::integrateiplink.local_audio_config.buffersize.value));
	genNetworkSlave_device::properties_active.sizesselection.value.selection() = 0x1;
	genNetworkSlave_device::properties_active.sizesselection.isValid = true;
	CjvxAudioDevice::properties_active.buffersize.value = JVX_SIZE_INT32(genNetworkSlave_device::integrateiplink.local_audio_config.buffersize.value);

	genNetworkSlave_device::properties_active.ratesselection.value.entries.clear();
	genNetworkSlave_device::properties_active.ratesselection.value.entries.push_back(jvx_size2String(
		genNetworkSlave_device::integrateiplink.local_audio_config.samplerate.value));
	genNetworkSlave_device::properties_active.ratesselection.value.selection() = 1;
	genNetworkSlave_device::properties_active.ratesselection.isValid = true;
	CjvxAudioDevice::properties_active.samplerate.value = JVX_SIZE_INT32(genNetworkSlave_device::integrateiplink.local_audio_config.samplerate.value);

	CjvxAudioDevice::properties_active.formatselection.value.entries.push_back(jvxDataFormat_txt(
		genNetworkSlave_device::integrateiplink.local_audio_config.audioformat.value));
	CjvxAudioDevice::properties_active.formatselection.value.selection() = 1;
	CjvxAudioDevice::properties_active.formatselection.isValid = true;

}

void 
CjvxNetworkSlaveDevice::propsToInternalValue_no_lock()
{
	_lock_properties_local();
	inConnection.toConnectedPartner.bsize = CjvxAudioDevice_genpcg::properties_active.buffersize.value;
	inConnection.toConnectedPartner.srate = CjvxAudioDevice_genpcg::properties_active.samplerate.value;
	inConnection.toConnectedPartner.format = (jvxDataFormat)CjvxAudioDevice_genpcg::properties_active.format.value;
	inConnection.toConnectedPartner.inChannels = CjvxAudioDevice_genpcg::properties_active.numberinputchannels.value;
	inConnection.toConnectedPartner.outChannels = CjvxAudioDevice_genpcg::properties_active.numberoutputchannels.value;
	inConnection.toConnectedPartner.frameFormat = JVX_PROTOCOL_ADV_CHANNEL_FRAME_TYPE_NON_INTERLEAVED;
	inConnection.toConnectedPartner.subformat_in.use = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	inConnection.toConnectedPartner.subformat_out.use = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	_unlock_properties_local();
}

void
CjvxNetworkSlaveDevice::internalValueToProps_no_lock()
{
	_lock_properties_local();
	CjvxAudioDevice_genpcg::properties_active.buffersize.value = inConnection.toConnectedPartner.bsize;
	CjvxAudioDevice_genpcg::properties_active.samplerate.value = inConnection.toConnectedPartner.srate;
	CjvxAudioDevice_genpcg::properties_active.format.value = inConnection.toConnectedPartner.format;
	CjvxAudioDevice_genpcg::properties_active.numberinputchannels.value = JVX_SIZE_INT32(inConnection.toConnectedPartner.inChannels);
	CjvxAudioDevice_genpcg::properties_active.numberoutputchannels.value = JVX_SIZE_INT32(inConnection.toConnectedPartner.outChannels);
	_unlock_properties_local();
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
