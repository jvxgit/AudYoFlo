#include "jvx-helpers-product.h"
void
jvx_initTagName(jvxAssignedFileTags& tags)
{
	int i;
	tags.tagsSet = 0;
	for(i = 0; i < JVX_NUMBER_AUDIO_FILE_TAGS; i++)
	{
		tags.tags[i].clear();
	}
}

jvxErrorType
jvx_tagExprToTagStruct(std::string tagDescr, jvxAssignedFileTags& lst, std::string& path)
{
	int i,j;
	std::string lookforTags = "<TAGS>";
	std::string lookforPath = "<PATH>";
	std::string tok1;
	std::string tok_remain;
	std::string collect;
	std::string trash;
	jvxInt32 idxFound = false;
	jvx_initTagName(lst);

	tok1 = tagDescr.substr(0,lookforTags.size());
	if(tagDescr.size() < lookforTags.size())
	{
		return(JVX_ERROR_PARSE_ERROR);
	}
	tok_remain = tagDescr.substr(lookforTags.size(), std::string::npos);
	if(tok1 == lookforTags)
	{
		while(tok_remain.size() >= 4)
		{
			// Tags always with 4 chars
			tok1 = tok_remain.substr(0,4);

			tok_remain = tok_remain.substr(4, std::string::npos);

			idxFound = -1;
			for(i = 0; i < JVX_NUMBER_AUDIO_FILE_TAGS; i++)
			{
				lookforTags = jvxAudioFileTagType_txt[i];
				if(lookforTags == tok1)
				{
					idxFound = i;
					break;
				}
			}
			if(idxFound >= 0)
			{
				collect.clear();
				trash = tok_remain.substr(0,1);
				if(tok_remain.size() < 1)
				{
					return(JVX_ERROR_PARSE_ERROR);
				}
				tok_remain = tok_remain.substr(1, std::string::npos);
				if(trash == ":")
				{
					while(1)
					{
						trash = tok_remain.substr(0,1);
						if(tok_remain.size() < 1)
						{
							return(JVX_ERROR_PARSE_ERROR);
						}
						tok_remain = tok_remain.substr(1, std::string::npos);
						if(trash != ":")
						{
							collect+= trash;
						}
						else
						{
							break;
						}
					}

					int count = 0;
					count = atoi(collect.c_str());
					collect.clear();
					for(j = 0; j < count; j++)
					{
						trash = tok_remain.substr(0,1);
						if(tok_remain.size() < 1)
						{
							return(JVX_ERROR_PARSE_ERROR);
						}
						tok_remain = tok_remain.substr(1, std::string::npos);

						collect += trash;
					}

					lst.tags[idxFound] = collect;
					lst.tagsSet |= (jvxBitField)1 << idxFound;
				}
				else
				{
					return(JVX_ERROR_PARSE_ERROR);
				}
			}
			else
			{
				return(JVX_ERROR_PARSE_ERROR);
			}
		}
	}
	else if(tok1 == lookforPath)
	{
		path = tok_remain;
	}
	else
	{
		return(JVX_ERROR_PARSE_ERROR);
	}
	return(JVX_NO_ERROR);
}

std::string
jvx_tagStructToUserView(jvxAssignedFileTags& tags)
{
	int j;
	std::string retString = "";
	for(j = 0; j < JVX_NUMBER_AUDIO_FILE_TAGS; j++)
	{
		if(jvx_bitTest(tags.tagsSet,j))
		{
			if(!retString.empty())
			{
				retString += ", " + tags.tags[j];
			}
			else
			{
				retString += tags.tags[j];
			}
		}
	}
	return(retString);
}

jvxErrorType
jvx_tagStructToTagExpr(std::string& tagExpr, jvxAssignedFileTags lst, std::string path)
{
	int i;
	std::string txt;
	if(JVX_EVALUATE_BITFIELD(lst.tagsSet))
	{
		tagExpr = "<TAGS>";
		for(i = 0; i < JVX_NUMBER_AUDIO_FILE_TAGS; i++)
		{
			if(jvx_bitTest(lst.tagsSet,i))
			{
				tagExpr += jvxAudioFileTagType_txt[i];
				txt = lst.tags[i];
				tagExpr += ":" + jvx_size2String(txt.size()) + ":";
				tagExpr += txt;
			}
		}
	}
	else
	{
		tagExpr = "<PATH>";
		tagExpr += path;
	}
	return(JVX_NO_ERROR);
}

void
jvx_produceFilenameWavOut(std::string& fNameOut, std::string path, const std::string& prefix, const std::string& tag, jvxBool extend_date_time)
{
	fNameOut = prefix;
	if (extend_date_time)
	{
		fNameOut += "__";
		fNameOut += JVX_DATE();
		fNameOut += "__";
		fNameOut += JVX_TIME();
		fNameOut = jvx_replaceCharacter(fNameOut, '/', '_');
		fNameOut = jvx_replaceCharacter(fNameOut, ':', '_');
	}
	fNameOut = path + "/" + fNameOut;
	fNameOut += tag;
	fNameOut += ".wav";
}

