#include "jvx-helpers-product.h"

static const char* jvxGenericWrapperChannelType_str[JVX_GENERIC_WRAPPER_CHANNEL_TYPE_LIMIT] =
{
	"--",
	"hw", 
	"f",
	"e",
	"d"
};

bool
jvx_genw_detectLeadingToken(std::string nameIn, std::string& nmChan, std::string searchme)
{
	nmChan = nameIn;
	if (nameIn.size() >= searchme.size())
	{
		if (nameIn.substr(0, searchme.size()) == searchme)
		{
			nmChan = nameIn.substr(searchme.size(), std::string::npos);
			return(true);
		}
	}
	return(false);
}

jvxErrorType
jvx_genw_decodeChannelName(std::string nameIn,  jvxGenericWrapperChannelType& purpChan, std::string& nmChan, 	/*int& id_global,*/ int& id_rel_device)
{
	int i;
	std::string purpChanTxt;
	purpChan = JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE;
	nmChan = "";
	//id_global = -1;
	id_rel_device = -1;
	
	std::string fTok = "::";
	std::string misc;
	size_t pos = nameIn.find(fTok);
	if(pos == std::string::npos)
	{
		return(JVX_ERROR_PARSE_ERROR);
	}
	else
	{
		purpChanTxt = nameIn.substr(0, pos);
	}
	pos += fTok.size();
	if(nameIn.size() <= pos)
	{
		return(JVX_ERROR_PARSE_ERROR);
	}
	else
	{
		nameIn = nameIn.substr(pos, std::string::npos);
	}

	/*
	pos = nameIn.rfind(fTok);
	if(pos == std::string::npos)
	{
		return(JVX_ERROR_PARSE_ERROR);
	}
	else
	{
		pos += fTok.size();
	}
	if(nameIn.size() <= pos)
	{
		return(JVX_ERROR_PARSE_ERROR);
	}
	else
	{
		misc = nameIn.substr(pos, std::string::npos);
		id_global = atoi(misc.c_str());

		pos -= fTok.size();
		nameIn = nameIn.substr(0, pos);
	}
	*/

	pos = nameIn.find(fTok);
	if(pos == std::string::npos)
	{
		return(JVX_ERROR_PARSE_ERROR);
	}
	else
	{
		misc = nameIn.substr(0,pos);
		id_rel_device = atoi(misc.c_str());
	}
	pos += fTok.size();
	if(nameIn.size() <= pos)
	{
		return(JVX_ERROR_PARSE_ERROR);
	}
	else
	{
		nmChan = nameIn.substr(pos, std::string::npos);
	}

	for(i = 0; i < JVX_GENERIC_WRAPPER_CHANNEL_TYPE_LIMIT; i++)
	{
		if(purpChanTxt == jvxGenericWrapperChannelType_str[i])
		{
			purpChan = (jvxGenericWrapperChannelType)i;
			break;
		}
	}

	if(purpChan == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE)
	{
		return(JVX_ERROR_PARSE_ERROR);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
jvx_genw_encodeChannelName(std::string& nameOut, jvxGenericWrapperChannelType purpChan, std::string nmChan, std::string fnmChan,	int id_rel_device/*, int id_global*/)
{
	nameOut = jvxGenericWrapperChannelType_str[purpChan];
	if(fnmChan.empty())
	{
		nameOut += "::" + jvx_int2String(id_rel_device) + "::" + nmChan;// + "::" + jvx_int2String(id_global);
	}
	else
	{
		nameOut += "::" + jvx_int2String(id_rel_device) + "::" + fnmChan;// + "::" + jvx_int2String(id_global);
	}

	return(JVX_NO_ERROR);
}

