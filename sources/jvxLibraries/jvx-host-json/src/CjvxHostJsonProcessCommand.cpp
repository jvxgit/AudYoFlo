#include "CjvxHostJsonProcessCommand.h"

// =======================================================================================
// =======================================================================================
// =======================================================================================
// =======================================================================================

typedef struct
{
	std::string selector;
	jvxDrivehostCommandFamily fam;
} entryTable_commandfam;

entryTable_commandfam entries_commandfam[] =
{
	{ "show", JVX_DRIVEHOST_FAMILY_SHOW },
	{ "return", JVX_DRIVEHOST_FAMILY_RETURN },
	{ "act", JVX_DRIVEHOST_FAMILY_ACT },
	{ "file", JVX_DRIVEHOST_FAMILY_FILE },
	{ "", JVX_DRIVEHOST_FAMILY_NONE }
};

// =======================================================================================
// =======================================================================================
// =======================================================================================
// =======================================================================================

static jvxErrorType translateCommand2Family(std::string command, jvxDrivehostCommandFamily& fam)
{
	int cnt = 0;
	while (1)
	{
		std::string entry = entries_commandfam[cnt].selector;
		if (entry.empty())
		{
			break;
		}
		if (entry == command)
		{
			fam = entries_commandfam[cnt].fam;
			return JVX_NO_ERROR;
		}
		else
		{
			cnt++;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

static jvxErrorType translateFirstArg2Adress(std::string entry, jvxDrivehostAddress& addr, jvxSize& subAddress, jvxSize& subsubAddress, jvxSize& subsubsubAddress)
{
	jvxSize i;
	addr = JVX_DRIVEHOST_ADDRESS_NONE;
	subAddress = JVX_SIZE_UNSELECTED;
	subsubAddress = JVX_SIZE_UNSELECTED;
	subsubsubAddress = JVX_SIZE_UNSELECTED;
	jvxComponentType tpl = JVX_COMPONENT_UNKNOWN;
	jvxErrorType resL = JVX_NO_ERROR;
	if (entry == "")
	{
		addr = JVX_DRIVEHOST_ADDRESS_NONE;
		subAddress = 0;
	}
	else if (entry == "sequencer")
	{
		addr = JVX_DRIVEHOST_ADRESS_SEQUENCER;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "config")
	{
		addr = JVX_DRIVEHOST_ADRESS_CONFIG;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "components")
	{
		addr = JVX_DRIVEHOST_ADRESS_COMPONENTS;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "hosttypehandler")
	{
		addr = JVX_DRIVEHOST_ADRESS_HOSTTYPEHANDLER;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "configlines")
	{
		addr = JVX_DRIVEHOST_ADRESS_CONFIGLINE;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "system")
	{
		addr = JVX_DRIVEHOST_ADRESS_SYSTEM;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "alive")
	{
		addr = JVX_DRIVEHOST_ADRESS_ALIVE;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "connections")
	{
		addr = JVX_DRIVEHOST_ADRESS_CONNECTIONS;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "connection_rules")
	{
		addr = JVX_DRIVEHOST_ADRESS_CONNECTION_RULES;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "connection_dropzone")
	{
		addr = JVX_DRIVEHOST_ADRESS_CONNECTION_DROPZONE;
		subAddress = 0;
		subsubAddress = 0;
	}
	else if (entry == "version")
	{
		addr = JVX_DRIVEHOST_ADRESS_VERSION;
		subAddress = 0;
		subsubAddress = 0;
	}
	else
	{
		jvxComponentIdentification cpId = JVX_COMPONENT_UNKNOWN;
		jvxErrorType res = jvxComponentIdentification_decode(cpId, entry);
		if (res == JVX_NO_ERROR)
		{
			addr = JVX_DRIVEHOST_ADRESS_SINGLE_COMPONENT;
			subAddress = cpId.tp;
			subsubAddress = cpId.slotid;
			subsubsubAddress = cpId.slotsubid;
		}
		/*
		tpl = JVX_COMPONENT_UNKNOWN;
		std::string entry_main = entry;
		jvxSize slotid = JVX_SIZE_UNSELECTED;
		jvxSize slotsubid = JVX_SIZE_UNSELECTED;

		jvxComponentIdentification_decode
		size_t pos1 = entry.find('<');
		if (pos1 != std::string::npos)
		{
			jvxBool err = false;
			std::string slots_txt = entry.substr(pos1 + 1, std::string::npos);
			size_t pos2 = slots_txt.find('>');
			if(pos2 != std::string::npos)
			{
				slots_txt = slots_txt.substr(0, pos2);
				pos2 = slots_txt.find(':');
				if (pos2 != std::string::npos)
				{
					slotid = jvx_string2Size(slots_txt.substr(0, pos2), err);
					if (!err)
					{
						slotsubid = jvx_string2Size(slots_txt.substr(pos2 + 1, std::string::npos), err);
						if (err)
						{
							slotid = JVX_SIZE_UNSELECTED;
						}
						else
						{
							entry_main = entry.substr(0, pos1);
						}
					}
				}
			}
		}
		resL = jvxComponentType_decode(&tpl, entry_main);
		if (resL == JVX_NO_ERROR)
		{
			addr = JVX_DRIVEHOST_ADRESS_SINGLE_COMPONENT;
			subAddress = tpl;
			subsubAddress = 0;
		}
		*/
		/*
		for (i = 0; i < JVX_COMPONENT_LIMIT; i++)
		{
			if (
				(entry == jvxComponentType_str[i].friendly) ||
				(entry == jvxComponentType_str[i].full))
			{
				addr = JVX_DRIVEHOST_ADRESS_SINGLE_COMPONENT;
				subAddress = (jvxComponentType)i;
				subsubAddress = 0;
				break;
			}
		}*/
	}

	if (JVX_CHECK_SIZE_UNSELECTED(subAddress))
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_NO_ERROR;
}

void
CjvxHostJsonProcessCommand::translate_full_command(
	const std::string& command, 
	oneDrivehostCommand& dh_command,
	std::vector<std::string>& args,
	std::string& addArg,
	jvxBool& translate_command_failed,
	jvxBool& parse_error,
	jvxBool& command_empty,
	jvxBool& regular_command,
	jvxBool& translate_firstarg_failed)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string f_expr;

	if (command.empty())
	{
		command_empty = true;
	}
	else
	{
		jvxErrorType resParse = jvx_parseCommandIntoToken(command, f_expr, args, &addArg);
		if (resParse == JVX_NO_ERROR)
		{
			res = translateCommand2Family(f_expr, dh_command.family);
			if (res == JVX_NO_ERROR)
			{
				dh_command.address = JVX_DRIVEHOST_ADDRESS_NONE;
				if (args.size() > 0)
				{
					res = translateFirstArg2Adress(args[0], dh_command.address, dh_command.subaddress, dh_command.subsubaddress, dh_command.subsubsubaddress);

					if (res == JVX_NO_ERROR)
					{
						regular_command = true;
						/*
						if (retString)
						{
						*retString = strResponse;
						}*/
					}
					else
					{
						translate_firstarg_failed = true;
					}
				}
			}
			else
			{
				translate_command_failed = true;
			}
		}
		else
		{
			parse_error = true;
		}
	}
}