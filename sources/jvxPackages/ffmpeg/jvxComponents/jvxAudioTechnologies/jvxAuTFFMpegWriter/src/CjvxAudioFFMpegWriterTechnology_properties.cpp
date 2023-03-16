#include "CjvxAudioFFMpegWriterTechnology.h"

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegWriterTechnology, control_fileio)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string devSpec;
	jvxSize idSelectNew; 
	if(JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegWriter_technology::control.add_device))
	{
		if (genFFMpegWriter_technology::control.add_device.value)
		{
			// To do: add file device
			res = activateOneFile(
				genFFMpegWriter_technology::file_params.file_folder.value,
				genFFMpegWriter_technology::file_params.filename_prefix.value,
				devSpec,
				idSelectNew);

			if (res == JVX_NO_ERROR)
			{
				// Update everything
				update_local_properties(true, idSelectNew);
				report_device_was_born(devSpec);
			}
			else
			{
				// Error description already set in activateOneFile
			}
		}
		genFFMpegWriter_technology::control.add_device.value = false;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegWriter_technology::control.remove_device))
	{
		if (genFFMpegWriter_technology::control.remove_device.value)
		{
			jvxSize idSel = jvx_bitfieldSelection2Id(
				genFFMpegWriter_technology::file_params.device_lists.value.selection(),
				genFFMpegWriter_technology::file_params.device_lists.value.entries.size());
			if (idSel < genFFMpegWriter_technology::file_params.device_lists.value.entries.size())
			{
				jvxApiString astr;
				assert(idSel < lstDevType.size());
				auto elm = lstDevType.begin();
				std::advance(elm, idSel);
				elm->second->description(&astr);
				std::string ident = astr.std_str();
				report_device_died(ident);
			}
			genFFMpegWriter_technology::control.remove_device.value = false;
		}
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegWriterTechnology, modify_wav_file_params)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (
		(JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genFFMpegWriter_technology::wav_properties.sample_resolution)) ||
		(JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genFFMpegWriter_technology::wav_properties.endian)) ||
		(JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genFFMpegWriter_technology::wav_properties.sample_type)) ||
		(JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genFFMpegWriter_technology::wav_properties.sub_type)))
	{
		update_wav_params_from_props();
	}

	return res;
}

void
CjvxAudioFFMpegWriterTechnology::update_wav_params_from_props()
{
	wav_start_params.resolution = genFFMpegWriter_technology::translate__wav_properties__sample_resolution_from();
	wav_start_params.endian = genFFMpegWriter_technology::translate__wav_properties__endian_from();
	wav_start_params.sample_type = genFFMpegWriter_technology::translate__wav_properties__sample_type_from();
	wav_start_params.sub_type = genFFMpegWriter_technology::translate__wav_properties__sub_type_from();
}