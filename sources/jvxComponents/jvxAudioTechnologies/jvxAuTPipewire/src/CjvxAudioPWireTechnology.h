#ifndef __CJVXAUDIOPWIRETECHNOLOGY_H__
#define __CJVXAUDIOPWIRETECHNOLOGY_H__

#include "jvxAudioTechnologies/CjvxMixedDevicesAudioTechnology.h"
#include "CjvxAudioPWireDevice.h" 
#include "pcg_exports_technology.h"

#include <pipewire/pipewire.h>

class onePort
{
public:
	~onePort(){};
	jvxSize id = JVX_SIZE_UNSELECTED;
	jvxSize ref_node = JVX_SIZE_UNSELECTED;
	jvxSize port_id = JVX_SIZE_UNSELECTED;
	std::string name;
	std::string nick;
	std::string direction;
	std::string physical;
	jvxDataFormat form = JVX_DATAFORMAT_NONE;
};

class oneNode
{
public:
	~oneNode()
	{
		for(auto elm: in_ports)
		{
			JVX_SAFE_DELETE_OBJECT(elm);
		}
		in_ports.clear();
		for(auto elm: out_ports)
		{
			JVX_SAFE_DELETE_OBJECT(elm);
		}
		out_ports.clear();
	};

	jvxSize id = JVX_SIZE_UNSELECTED;
	jvxSize ref_device = JVX_SIZE_UNSELECTED;
	std::list<onePort*> in_ports;
	std::list<onePort*> out_ports;
	std::string description;
	std::string name;
	std::string nick;
	jvxBool isSink = false;
};

class oneDevice
{
public:
	~oneDevice()
	{
		for(auto elm: sources)
		{
			JVX_SAFE_DELETE_OBJECT(elm);
		}
		sources.clear();
		for(auto elm: sinks)
		{
			JVX_SAFE_DELETE_OBJECT(elm);
		}
		sinks.clear();
	};

	jvxSize id = JVX_SIZE_UNSELECTED;
	std::string api_name;
	std::string description;
	std::string name;
	std::string nick;
	std::list<oneNode*> sources;
	std::list<oneNode*> sinks;
};

class CjvxAudioPWireTechnology: public CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>,
	public genPWire_technology
{

protected:
	int argc = 0;
    char** argv = nullptr;

    pw_thread_loop* loop = nullptr;
    struct pw_context *context = nullptr;
    struct pw_core *core = nullptr;
    struct pw_registry *registry = nullptr;
    struct spa_hook registry_listener;

	struct spa_hook async_listener;

	struct
	{
		int idMess_core = 0;
		int idMess_received = 0;
		JVX_NOTIFY_HANDLE_DECLARE(notWait);
	} async_run;

	std::list<oneDevice*> devices_unsorted;
	std::list<oneNode*> nodes_unsorted_sinks;
	std::list<oneNode*> nodes_unsorted_sources;
	std::list<onePort*> ports_unsorted;

	std::list<oneDevice*> devices_linked;
	
	std::list<CjvxAudioPWireDevice*> devices_active;

public:
	CjvxAudioPWireTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAudioPWireTechnology();

	jvxErrorType activate()override;
	jvxErrorType deactivate()override;

	void activate_technology_api() override;
	void deactivate_technology_api() override;

	void event_global_callback(uint32_t id,
                uint32_t permissions, const char *type, uint32_t version,
                const struct spa_dict *props);

	void async_run_start();
	void async_run_stop();
	jvxErrorType async_run_trigger();
	void async_run_cb(jvxUInt32 id, int seq);

	void sort_unsorted_devices();
	jvxBool  check_device_ready(oneDevice* theDevice);

	jvxErrorType put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;
	jvxErrorType get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

};

#endif
