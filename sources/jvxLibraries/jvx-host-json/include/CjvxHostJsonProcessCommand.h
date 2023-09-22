#ifndef _CJVXHOSTJSONPROCESSCOMMAND_H__
#define _CJVXHOSTJSONPROCESSCOMMAND_H__

#include "jvx.h"
#include "CjvxJson.h"

typedef enum
{
	JVX_DRIVEHOST_FAMILY_NONE,
	JVX_DRIVEHOST_FAMILY_SHOW,
	JVX_DRIVEHOST_FAMILY_RETURN,
	JVX_DRIVEHOST_FAMILY_ACT,
	JVX_DRIVEHOST_FAMILY_FILE
} jvxDrivehostCommandFamily;

typedef enum
{
	JVX_DRIVEHOST_ADDRESS_NONE,
	JVX_DRIVEHOST_ADRESS_COMPONENTS,
	JVX_DRIVEHOST_ADRESS_SINGLE_COMPONENT,
	JVX_DRIVEHOST_ADRESS_CONFIG,
	JVX_DRIVEHOST_ADRESS_SEQUENCER,
	JVX_DRIVEHOST_ADRESS_SYSTEM,
	JVX_DRIVEHOST_ADRESS_HOSTTYPEHANDLER,
	JVX_DRIVEHOST_ADRESS_CONFIGLINE,
	JVX_DRIVEHOST_ADRESS_CONNECTIONS,
	JVX_DRIVEHOST_ADRESS_CONNECTION_RULES,
	JVX_DRIVEHOST_ADRESS_CONNECTION_DROPZONE,
	JVX_DRIVEHOST_ADRESS_VERSION,
	JVX_DRIVEHOST_ADRESS_ALIVE
} jvxDrivehostAddress;

typedef enum
{
	JVX_DRIVEHOST_CONNECTION_SHOW_NORMAL,
	JVX_DRIVEHOST_CONNECTION_SHOW_COMPACT,
	JVX_DRIVEHOST_CONNECTION_SHOW_PATH,
	JVX_DRIVEHOST_CONNECTION_SHOW_UPATH,
	JVX_DRIVEHOST_CONNECTION_SHOW_READY,
	JVX_DRIVEHOST_CONNECTION_SHOW_LAST
} jvxDrivehostConnectionShow;

typedef jvxSize jvxDrivehostSubAddress;

struct oneDrivehostCommand
{
	jvxDrivehostCommandFamily family = JVX_DRIVEHOST_FAMILY_NONE;
	jvxDrivehostAddress address = JVX_DRIVEHOST_ADDRESS_NONE;
	jvxDrivehostSubAddress subaddress = 0;
	jvxDrivehostSubAddress subsubaddress = 0;
	jvxDrivehostSubAddress subsubsubaddress = 0;
};

class CjvxHostJsonProcessCommand
{
public:
	void translate_full_command(const std::string& command,
		oneDrivehostCommand& dh_command,
		std::vector<std::string>& args,
		std::string& addArg,
		jvxBool& translate_command_failed,
		jvxBool& parse_error,
		jvxBool& command_empty,
		jvxBool& regular_command,
		jvxBool& translate_firstarg_failed);

	virtual jvxErrorType process_command_abstraction(
		const oneDrivehostCommand& dh_command, const std::vector<std::string>& args,
		const std::string& addArg, jvxSize off, CjvxJsonElementList& jsec, jvxBool* systemUpdate) = 0;

	//virtual jvxComponentIdentification get_default_component_id(jvxSize tpSz) { return JVX_COMPONENT_UNKNOWN; };
};

#endif
