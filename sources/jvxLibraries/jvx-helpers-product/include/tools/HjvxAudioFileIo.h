#ifndef __HJVXAUDIOFILEIO_H__
#define __HJVXAUDIOFILEIO_H__

#define JVX_GENERIC_HOST_FNAME_PREFIX "jvx_record_"

typedef struct
{
	/*
	JVX_AUDIO_FILE_TAG_BTFLD_ARTIST = 0x1,
	JVX_AUDIO_FILE_TAG_BTFLD_TITLE = 0x2,
	JVX_AUDIO_FILE_TAG_BTFLD_CREATION_DATA = 0x4,
	JVX_AUDIO_FILE_TAG_BTFLD_GENRE = 0x8
	*/
	jvxBitField tagsSet;
	std::string tags[JVX_NUMBER_AUDIO_FILE_TAGS];

} jvxAssignedFileTags;

void
jvx_initTagName(jvxAssignedFileTags& tags);

jvxErrorType
jvx_tagExprToTagStruct(std::string tagDescr, jvxAssignedFileTags& lst, std::string& path);

std::string
jvx_tagStructToUserView(jvxAssignedFileTags& tags);

jvxErrorType
jvx_tagStructToTagExpr(std::string& tagExpr, jvxAssignedFileTags lst, std::string path);

void
jvx_produceFilenameWavOut(std::string& fNameOut, std::string path, const std::string& prefix = JVX_GENERIC_HOST_FNAME_PREFIX, const std::string& tag = "",
	jvxBool extend_data_time = true);

#endif
