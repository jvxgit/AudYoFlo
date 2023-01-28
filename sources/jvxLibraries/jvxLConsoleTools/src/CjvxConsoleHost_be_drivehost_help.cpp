#include "CjvxConsoleHost_be_drivehost.h"

void 
CjvxConsoleHost_be_drivehost::process_help_command(IjvxEventLoop_frontend* origin)
{
	jvxSize i;   
	std::cout << std::endl;
	std::cout << "Usage: <command>[addArg](<args>) -- [addArgs] and (<args>) are optional." << std::endl;
	std::cout << "\thelp: print this message" << std::endl;
	std::cout << "\tshow(<address>, <varargs>) : Show info about dedicated components or subsystems." << std::endl;
	std::cout << "\t\t <address>: system" << std::endl;
	std::cout << "\t\t\t\t " << "Show full or compact (<compact>) version of system status." << std::endl; 
	std::cout << "\t\t <address>: components" << std::endl;
	std::cout << "\t\t <address>: sequencer" << std::endl;
	std::cout << "\t\t\t\t " << "optionally specify runqueue or leavequeue" << std::endl;
	std::cout << "\t\t <address>: hosttypehandler" << std::endl;
	std::cout << "\t\t <address>: configline" << std::endl;
	std::cout << "\t\t <address>: <component> <varargs>" << std::endl;
	std::cout << "\t\t\t " << "<empty>: Show all available components of this type and highlight the one selected in slot/subslot." << std::endl;
	std::cout << "\t\t\t " << "properties: List component instance properties, arguments: <purpose>, <filter to select properties>," << std::endl;
	std::cout << "\t\t\t\t " << "purpose: [\"list\" or a combination of \"min\", \"core\", \"control\", \"full\", \"fullp\" with \"x\" (e.g., \"min|x\") to show different descriptor functions, x for extended info" << std::endl;
	std::cout << "\t\t\t\t " << "filter: either wildcard (e.g., /*/*, opt) or multiple properties in brackets []." << std::endl;
	std::cout << "\t\t\t " << "property: Displays the current property value for dedicated property," << std::endl;
	std::cout << "\t\t\t\t " << "multiple properties can be grouped in brackets [], each entry can be in parethesis () with additional parameters," << std::endl;
	std::cout << "\t\t\t\t " << "single entry for property with <property descriptor> OR <property_id> + <offset> (opt) + <num_elms> (opt) + <content_only> (opt)." << std::endl;
	std::cout << "\t\t\t\t " << "optional additional arguments <addArg>=\"cC\" yields to return content_only and/or compact version of the property." << std::endl;
	std::cout << "\t\t\t " << "slots: List populated slots for this property type." << std::endl;
	std::cout << "\t\t\t " << "devices: List all devices for a given technology." << std::endl;
	std::cout << "\t\t\t " << "options: List all selection options for a given component type." << std::endl;
	std::cout << "\t\t\t " << "commands: List component instance commands." << std::endl;
	std::cout << "\t\t\t " << "conditions: List component instance conditions." << std::endl;
	std::cout << "\t\t <address>: alive" << std::endl;
	std::cout << "\t\t <address>: connections" << std::endl;
	std::cout << "\t\t\t " << "Show all currently established data connections." << std::endl;
	std::cout << "\t\t\t " << "path (opt): List the connection path through the dedicated conection tree." << std::endl;
	std::cout << "\t\t <address>: connection_rules" << std::endl;
	std::cout << "\t\t\t " << "Show all currently defined connection rules." << std::endl;
	std::cout << "\t\t <address>: version" << std::endl;
	std::cout << "\t\t <address>: connection_dropzone" << std::endl;
	std::cout << "\t\t\t " << "Show status of connection dropzone." << std::endl;	
	std::cout << "\t\t\t " << "master_fac (opt): show all master factories; <filter_expr> Expression to show only matching entries." << std::endl;
	std::cout << "\t\t\t " << "connector_fac (opt): show all connection factories; <filter_expr> Expression to show only matching entries." << std::endl;
	std::cout << "\t\t <address>: config" << std::endl;
	std::cout << "\t\t\t " << "Show current configuration." << std::endl;

	// Show interface
	std::cout << "\t\t\t num_digits (opt)" << std::endl;
	std::cout << "\t\t\t get_property_compact_mode (opt)" << std::endl;
	std::cout << "\t\t\t prop_base64 (opt)" << std::endl;

	// This interface
	std::cout << "\t\t\t silent_mode (opt)" << std::endl;
	std::cout << "\t\t\t timeout_seq (opt)" << std::endl;

	std::cout << "\tact(<address>, <varargs>) : Run a specific action on one of the components." << std::endl;
	std::cout << "\t\t <address>: sequencer" << std::endl;
	std::cout << "\t\t\t add sequence" << std::endl; 
	std::cout << "\t\t\t\t <description>" << std::endl;
	std::cout << "\t\t\t remove sequence" << std::endl;
	std::cout << "\t\t\t\t <sequence id>" << std::endl;
	std::cout << "\t\t\t activate sequence" << std::endl;
	std::cout << "\t\t\t\t <sequence id>" << std::endl;
	std::cout << "\t\t\t add step" << std::endl;
	std::cout << "\t\t\t\t <sequence id>; <queuetype>; <step id> (opt); <steptype> (opt); <component> (opt)" << std::endl;
	std::cout << "\t\t\t\t <description> (opt); <function id> (opt); <timeout msec> (opt);  <label> (opt)" << std::endl;
	std::cout << "\t\t\t\t <label true> (opt); <label false> (opt)" << std::endl;
	std::cout << "\t\t\t remove step" << std::endl;
	std::cout << "\t\t\t\t <sequence id>;  <queuetype>; <step id>" << std::endl;
	std::cout << "\t\t\t modify step" << std::endl;
	std::cout << "\t\t\t\t <sequence id>; <queuetype>; <step id> (opt); <steptype> ; <component>" << std::endl;
	std::cout << "\t\t\t\t <description> (opt); <function id> (opt); <timeout msec> (opt); <label> (opt)" << std::endl;
	std::cout << "\t\t\t\t <label true> (opt); <label false> (opt)" << std::endl;
	std::cout << "\t\t\t switch steps" << std::endl;
	std::cout << "\t\t\t\t <sequence id>; <queuetype>; <step id1>; <step id2>" << std::endl;
	std::cout << "\t\t\t start: Start the sequencer." << std::endl;
	std::cout << "\t\t\t stop: Stop the sequencer (full stop)." << std::endl;
	std::cout << "\t\t\t stops: Stop the sequencer (current sequence)." << std::endl;
	std::cout << "\t\t <address>:<component>" << std::endl;
	std::cout << "\t\t\t select: Select a component instance." << std::endl;
	std::cout << "\t\t\t\t Instance (descriptor or id)." << std::endl;
	std::cout << "\t\t\t activate: Activate a component instance." << std::endl;
	std::cout << "\t\t\t\t Instance (descriptor or id, optional). If instance is specified, select first. Otherwise activate currently selected instance." << std::endl;
	std::cout << "\t\t\t deactivate: Deactivate a component instance." << std::endl;
	std::cout << "\t\t\t unselect: Unselect a component instance." << std::endl;
	std::cout << "\t\t\t get_property: Displays the current property value for dedicated property." << std::endl;
	std::cout << "\t\t\t\t " << "multiple properties can be grouped in brackets [], each entry can be in parethesis () with additional parameters," << std::endl;
	std::cout << "\t\t\t\t " << "single entry for property with <property descriptor> OR <property_id>; <offset> (opt); <num_elms> (opt), <content_only> (opt), <context> (opt)." << std::endl;
	std::cout << "\t\t\t\t " << "optional additional arguments <addArg>=\"cC\" yields to return content_only and/or compact version of the property." << std::endl; 
	std::cout << "\t\t\t set_property: Modifies the current property value for dedicated property." << std::endl;
	std::cout << "\t\t\t\t " << "multiple properties can be grouped in brackets [], each entry can be in parethesis () with additional parameters," << std::endl;
	std::cout << "\t\t\t\t " << "single entry for property with <property descriptor>; <value to pass to the property>; <offset> (opt), <num_elms> (opt)." << std::endl;
	std::cout << "\t\t\t\t " << "optional additional argument <addArg>=\"r\" yields to generate a report event in the backend." << std::endl;
	std::cout << "\t\t <address>: connections" << std::endl;
	std::cout << "\t\t\t create" << std::endl;
	std::cout << "\t\t\t\t Create a data connection process, <name_process>" << std::endl;
	std::cout << "\t\t\t disconnect" << std::endl;
	std::cout << "\t\t\t\t Disconnect a given data connection process, <name or id>" << std::endl;
	std::cout << "\t\t\t remove" << std::endl;
	std::cout << "\t\t\t\t Disconnect and remove a given data connection process, <name or id>" << std::endl;

	std::cout << "\t\t <address>: connection_dropzone" << std::endl;
	std::cout << "\t\t\t add_master" << std::endl;
	std::cout << "\t\t\t\t Add a master to the dropzone, <master factory name>; <master factory connector>; <component identification filter> (opt)" << std::endl;
	std::cout << "\t\t\t add_ocon" << std::endl;
	std::cout << "\t\t\t\t Add a connector to the dropzone, <connector factory name>; <output connector>; <component identification filter> (opt)" << std::endl;
	std::cout << "\t\t\t add_icon" << std::endl;
	std::cout << "\t\t\t\t Add a connector to the dropzone, <connector factory name>; <input connector>; <component identification filter> (opt)" << std::endl;
	std::cout << "\t\t\t add_bridge" << std::endl;
	std::cout << "\t\t\t\t Add a connector to the dropzone, <connector factory name>; <input connector>; <component identification filter> (opt)" << std::endl;	
	std::cout << "\t\t\t rem_bridge" << std::endl;
	std::cout << "\t\t\t\t Add a connector to the dropzone, <connector factory name>; <input connector>; <component identification filter> (opt)" << std::endl;	
	std::cout << "\t\t\t process_connect" << std::endl;
	std::cout << "\t\t\t\t Use the current dropzone to connect a process, <connection name>, <connector factory name>; <input connector>; <component identification filter> (opt)" << std::endl;	
	std::cout << "\t\t\t process_create_connect" << std::endl;
	std::cout << "\t\t\t\t Use the current dropzone to create and connect a process, <connection name>" << std::endl;
	std::cout << "\t\t\t reset" << std::endl;
	std::cout << "\t\t\t\t Reset the dropzone." << std::endl;

	std::cout << "\t\t <address>: config" << std::endl;

	// show configs
	std::cout << "\t\t\t num_digits" << std::endl;
	std::cout << "\t\t\t\t <integer> (default: 4)" << std::endl;
	std::cout << "\t\t\t get_property_compact_mode" << std::endl;
	std::cout << "\t\t\t\t Activate compact property output to reduce space, <yes> or <no> (default: no)" << std::endl;
	std::cout << "\t\t\t prop_base64" << std::endl;
	std::cout << "\t\t\t\t Activate to output buffers in base64 encoding, <yes> or <no> (default: yes)" << std::endl;

	// configs for this class
	std::cout << "\t\t\t silent_mode" << std::endl;
	std::cout << "\t\t\t\t Activate to print out results NOT originating from primary frontend command to primary fronted, <yes> or <no> (default: yes)" << std::endl;
	std::cout << "\t\t\t timeout_seq" << std::endl;
	std::cout << "\t\t\t\t integer" << std::endl;

	std::cout << "\tfile(<address>, <varargs>) : Perform a specific type of file action." << std::endl;
	std::cout << "\t\t <address>: config" << std::endl;
	std::cout << "\t\t\t read" << std::endl;
	std::cout << "\t\t\t\t Filename; (optional)" << std::endl;
	std::cout << "\t\t\t write" << std::endl;
	std::cout << "\t\t\t\t Filename; (optional)" << std::endl;
	std::cout << "\t Definitions: " << std::endl;
	std::cout << "\t\t <component>: " << std::flush;
	for (i = 1; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		std::cout << jvxComponentType_txt(i) << std::flush;
		if (i != (JVX_COMPONENT_ALL_LIMIT - 1))
		{
			std::cout << "; " << std::flush;
		}
	}
	std::cout << std::endl;

	std::cout << "\t\t <queuetype>: " << std::flush;
	for (i = 1; i < JVX_SEQUENCER_QUEUE_TYPE_LIMIT; i++)
	{
		std::cout << jvxSequencerQueueType_str[i].friendly << std::flush;
		if (i != (JVX_SEQUENCER_QUEUE_TYPE_LIMIT - 1))
		{
			std::cout << "; " << std::flush;
		}
	}
	std::cout << std::endl;

	std::cout << "\t\t <steptype>: " << std::flush;
	for (i = 1; i < JVX_SEQUENCER_TYPE_COMMAND_LIMIT; i++)
	{
		std::cout << jvxSequencerElementType_str[i].friendly << std::flush;
		if (i != (JVX_SEQUENCER_TYPE_COMMAND_LIMIT - 1))
		{
			std::cout << "; " << std::flush;
		}
	}
	std::cout << std::endl;
	std::cout << "\tquit: terminate program" << std::endl;

	jvxBool foundOne = false;

	std::string command = "help";

	std::list<jvxOneBackendAndState>::iterator elm = this->linkedSecBackends.begin();
	for (; elm != linkedSecBackends.end(); elm++)
	{

		TjvxEventLoopElement evelm;

		evelm.origin_fe = origin;
		evelm.priv_fe = NULL;
		evelm.target_be = elm->be;
		evelm.priv_be = NULL;

		evelm.param = (jvxHandle*)&command;
		evelm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

		evelm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
		evelm.eventClass = JVX_EVENTLOOP_REQUEST_IMMEDIATE;
		evelm.eventPriority = JVX_EVENTLOOP_PRIORITY_UNKNOWN;
		evelm.delta_t = JVX_SIZE_UNSELECTED;
		evelm.autoDeleteOnProcess = c_false;
		evLop->event_schedule(&evelm, NULL, NULL);

	}

	TjvxEventLoopElement evelm;
	evelm.origin_fe = linkedPriFrontend.fe;
	evelm.priv_fe = NULL;
	evelm.target_be = NULL;
	evelm.priv_be = NULL;

	evelm.param = (jvxHandle*)&command;
	evelm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

	evelm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
	evelm.eventClass = JVX_EVENTLOOP_REQUEST_IMMEDIATE;
	evelm.eventPriority = JVX_EVENTLOOP_PRIORITY_UNKNOWN;
	evelm.delta_t = JVX_SIZE_UNSELECTED;
	evelm.autoDeleteOnProcess = c_false;
	evLop->event_schedule(&evelm, NULL, NULL);
}