#include "CjvxAudioFFMpegWriterTechnology.h"
#include "CjvxAudioFFMpegCommon.h"

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

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegWriterTechnology, set_file_type)
{
	jvxErrorType res = JVX_NO_ERROR;	
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegWriter_technology::file_params.file_type))
	{
		update_local_properties(true, 0);
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegWriter_technology::file_params.file_sub_type))
	{

	}
	return res;
}

void
CjvxAudioFFMpegWriterTechnology::update_local_properties(jvxBool trig_host, jvxSize idSelectNew)
{
	std::string old_sel;
	jvxSize cnt;
	jvxSize i;
	jvxSize idSel = jvx_bitfieldSelection2Id(
		genFFMpegWriter_technology::file_params.device_lists.value.selection(),
		genFFMpegWriter_technology::file_params.device_lists.value.entries.size());
	if (idSel < genFFMpegWriter_technology::file_params.device_lists.value.entries.size())
	{
		old_sel = genFFMpegWriter_technology::file_params.device_lists.value.entries[idSel];
	}

	idSel = JVX_SIZE_UNSELECTED;
	cnt = 0;
	genFFMpegWriter_technology::file_params.device_lists.value.entries.clear();
	for (std::pair<IjvxDevice*, CjvxAudioFFMpegWriterDevice*> elm : lstDevType)
	{
		genFFMpegWriter_technology::file_params.device_lists.value.entries.push_back(
			elm.second->_common_set_min.theDescription);
		if (elm.second->_common_set_min.theDescription == old_sel)
		{
			idSel = cnt;
		}
		cnt++;
	}

	if (idSel >= genFFMpegWriter_technology::file_params.device_lists.value.entries.size())
	{
		if (genFFMpegWriter_technology::file_params.device_lists.value.entries.size())
		{
			idSel = 0;
		}
		else
		{
			idSel = JVX_SIZE_UNSELECTED;
		}
	}

	if (JVX_CHECK_SIZE_SELECTED(idSel))
	{
		jvx_bitZSet(
			genFFMpegWriter_technology::file_params.device_lists.value.selection(),
			idSel);
	}

	if (
		JVX_CHECK_SIZE_SELECTED(idSelectNew) &&
		(idSelectNew < genFFMpegWriter_technology::file_params.device_lists.value.entries.size()))
	{
		jvx_bitZSet(
			genFFMpegWriter_technology::file_params.device_lists.value.selection(),
			idSelectNew);
	}

	// Update the wav file subtypes
	std::string old_entry;
	jvxSize id = JVX_BITFIELD_SELECTION_ID(genFFMpegWriter_technology::file_params.file_sub_type);
	if (JVX_CHECK_SIZE_SELECTED(id))
	{
		old_entry = genFFMpegWriter_technology::file_params.file_sub_type.value.entries[id];
	}
	id = JVX_SIZE_UNSELECTED;
	genFFMpegWriter_technology::file_params.file_sub_type.value.entries.clear();
	jvxAudioFFMpegWriteFiletype fileTp = genFFMpegWriter_technology::translate__file_params__file_type_from();

	cnt = 0;
	switch (fileTp)
	{
	case jvxAudioFFMpegWriteFiletype::JVX_FFMPEG_FILEWRITER_WAV:

		while (1)
		{
			const char* opt = optionsWav[cnt];
			if (opt)
			{
				genFFMpegWriter_technology::file_params.file_sub_type.value.entries.push_back(opt);
				if (opt == old_entry)
				{
					id = cnt;
				}
			}
			else
			{
				break;
			}
			cnt++;
		}
		break;
	case jvxAudioFFMpegWriteFiletype::JVX_FFMPEG_FILEWRITER_MP3:
	case jvxAudioFFMpegWriteFiletype::JVX_FFMPEG_FILEWRITER_M4A:

		while (1)
		{
			const char* opt = optionsMp3M4a[cnt];
			if (opt)
			{
				genFFMpegWriter_technology::file_params.file_sub_type.value.entries.push_back(opt);
				if (opt == old_entry)
				{
					id = cnt;
				}
			}
			else
			{
				break;
			}
			cnt++;
		}
		break;
	}

	if (JVX_CHECK_SIZE_UNSELECTED(id))
	{
		if (genFFMpegWriter_technology::file_params.file_sub_type.value.entries.size())
		{
			switch (fileTp)
			{
			case jvxAudioFFMpegWriteFiletype::JVX_FFMPEG_FILEWRITER_WAV:
				id = JVX_OPTION_INIT_WAV;
				break;
			case jvxAudioFFMpegWriteFiletype::JVX_FFMPEG_FILEWRITER_MP3:
			case jvxAudioFFMpegWriteFiletype::JVX_FFMPEG_FILEWRITER_M4A:
				id = JVX_OPTION_INIT_MP3M4A;
				break;
			default:
				id = 0;
			}

		}
	}
	if (JVX_CHECK_SIZE_SELECTED(id))
	{
		jvx_bitZSet(genFFMpegWriter_technology::file_params.file_sub_type.value.selection(), id);
	}
	// jvxWavDifferentSubTypes

	if (trig_host)
	{
		this->_request_command(CjvxReportCommandRequest_id(
			jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY_CONTENT,
			_common_set.theComponentType,
			CjvxProperties::property_changed_descriptor_tag_add( genFFMpegWriter_technology::file_params.file_sub_type.descriptor.std_str()).c_str()));

		CjvxProperties::add_property_report_collect(genFFMpegWriter_technology::file_params.file_sub_type.descriptor.std_str(), true);

		this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		this->_report_property_has_changed(
			genFFMpegWriter_technology::file_params.device_lists.category,
			genFFMpegWriter_technology::file_params.device_lists.globalIdx,
			false, 0, 1, JVX_COMPONENT_UNKNOWN);
	}
}