#ifndef __CJVXAUDIOPWIRETECHNOLOGY_H__
#define __CJVXAUDIOPWIRETECHNOLOGY_H__

#include "jvxAudioTechnologies/CjvxMixedDevicesAudioTechnology.h"
#include "CjvxAudioPWireDevice.h" 
#include "pcg_exports_technology.h"

#include <pipewire/pipewire.h>

enum class operationModeTechnology
{
	AYF_PIPEWIRE_OPERATION_DSP_FILTER,
	AYF_PIPEWIRE_OPERATION_INPUT_OUTPUT
};

enum class operationModeDevice
{
	AYF_PIPEWIRE_OPERATION_DEVICE_INPUT,
	AYF_PIPEWIRE_OPERATION_DEVICE_OUTPUT,
	AYF_PIPEWIRE_OPERATION_DEVICE_DUPLEX
};

class onePort
{
public:
	~onePort(){};
	jvxSize id = JVX_SIZE_UNSELECTED;
	int serial = 0;
	jvxSize ref_node = JVX_SIZE_UNSELECTED;
	jvxSize port_id = JVX_SIZE_UNSELECTED;
	std::string name;
	std::string nick;
	std::string direction;
	std::string physical;
	jvxDataFormat form = JVX_DATAFORMAT_NONE;
	onePort* copy()
	{
		onePort* newPort = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(newPort, onePort);
		newPort->id = id;
		newPort->serial = serial;
		newPort->ref_node = ref_node;
		newPort->port_id = port_id;
		newPort->name = name;
		newPort->nick = nick;
		newPort->direction = direction;
		newPort->physical = physical;
		newPort->form = form;
		return newPort;
	};
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
	int serial = 0;
	jvxSize ref_device = JVX_SIZE_UNSELECTED;
	std::list<onePort*> in_ports;
	std::list<onePort*> out_ports;
	std::string description;
	std::string name;
	std::string nick;
	jvxBool isSink = false;
	oneNode* copy()
	{
		oneNode* newNode = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(newNode, oneNode);
		newNode->id = id;
		newNode->serial = serial;
		newNode->ref_device = ref_device;
		newNode->description = description;
		newNode->name = name;
		newNode->nick = nick;
		newNode->isSink = isSink;
		for(auto elm: in_ports)
		{
			newNode->in_ports.push_back(elm->copy());
		}
		for(auto elm: out_ports)
		{
			newNode->out_ports.push_back(elm->copy());
		}
		return newNode;
	}
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
	int serial = 0;
	std::string api_name;
	std::string description;
	std::string name;
	std::string nick;
	std::list<oneNode*> sources;
	std::list<oneNode*> sinks;
	operationModeDevice opMode = operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_DUPLEX;

	oneDevice* copy()
	{
		oneDevice* newDev = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(newDev, oneDevice);
		newDev->id = id;
		newDev->serial = serial;
		newDev->api_name = api_name;
		newDev->description = description;
		newDev->name = name;
		newDev->nick = nick;
		newDev->opMode = opMode;
		for(auto elm: sources)
		{
			newDev->sources.push_back(elm->copy());
		}
		for(auto elm: sinks)
		{
			newDev->sinks.push_back(elm->copy());
		}
		return newDev;
	}
};

class CjvxAudioPWireTechnology: public CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>,
	public genPWire_technology
{
	friend class CjvxAudioPWireDevice;

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

	operationModeTechnology techMode = operationModeTechnology::AYF_PIPEWIRE_OPERATION_INPUT_OUTPUT;

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
