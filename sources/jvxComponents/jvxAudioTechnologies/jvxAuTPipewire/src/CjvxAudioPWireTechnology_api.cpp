#include "CjvxAudioPWireTechnology.h"

#define JVX_ASYNC_TIMEOUT 5000


// ================================================================================0
// Some hints about samplerates: https://gitlab.freedesktop.org/pipewire/pipewire/-/wikis/Guide-Rates
// -> Command: <pw-metadata -n settings 0 clock.rate <samplerate>>
// -> Command: <pw-metadata -n settings 0 clock.force-rate <samplerate>>
// The "force" option will trigger a modification of buffersize while in operation
// -> Make an entry in ~/.config/pipewire/pipewire.conf.d/10-rates.conf  -> 
// # Adds more common rates
// context.properties = {
//     default.clock.allowed-rates = [ 44100 48000 88200 96000 ]
// }

// How to delete links in helvum: https://gitlab.freedesktop.org/pipewire/helvum/-/issues/50
// -> same connection as on connect will disconnect
// Set the buffersize: https://linuxmusicians.com/viewtopic.php?t=25768
// -> pw-metadata -n settings 0 clock.force-quantum <buffersize>
// -> pw-metadata -n settings 0 clock.quantum <buffersize>
// Resetting the force-constraints
// -> pw-metadata -n settings 0 clock.force-rate 0
// -> pw-metadata -n settings 0 clock.force-quantum 0
// The "force" option will trigger a modification of buffersize while in operation
// Works even with odd numbers like 53 :-)
//
// Show all objects in pipewire universe: pw-cli list-objects >> log2.txt

static void on_async_done(void *data, uint32_t id, int seq)
{
    if(data)
    {
        CjvxAudioPWireTechnology* this_pointer = (CjvxAudioPWireTechnology* ) data;
        if(this_pointer)
        {
            this_pointer->async_run_cb(id, seq);
        }
    }
}

static void registry_event_global(void *data, uint32_t id,
                uint32_t permissions, const char *type, uint32_t version,
                const struct spa_dict *props)
{
    if(data)
    {
        CjvxAudioPWireTechnology* this_pointer = (CjvxAudioPWireTechnology* ) data;
        if(this_pointer)
        {
            this_pointer->event_global_callback(id, permissions, type, version, props);
        }
    }
        
}

static const struct pw_registry_events registry_events = 
{
    PW_VERSION_REGISTRY_EVENTS,
    .global = registry_event_global,
};

static const struct pw_core_events async_events =
{
    PW_VERSION_CORE_EVENTS,
    .done = on_async_done,
};

// ========================================================================================
// ========================================================================================

void 
CjvxAudioPWireTechnology::activate_technology_api()
{
	jvxSize i;
	
    // Start Pipewire
    pw_init(&argc, &argv);
    
    data.opt_name = "settings";

    std::cout << "Compiled with libpipewire " << pw_get_headers_version() << ", linked with libpipewire " << pw_get_library_version() << std::endl;

    loop_tech = pw_thread_loop_new("The Technology Loop", NULL /* properties */);
    context = pw_context_new(pw_thread_loop_get_loop(loop_tech),
                             NULL /* properties */,
                             0 /* user_data size */);

    core = pw_context_connect(context,
                              NULL /* properties */,
                              0 /* user_data size */);

    registry = pw_core_get_registry(core, PW_VERSION_REGISTRY,
                                    0 /* user_data size */);

    spa_zero(registry_listener);
    pw_registry_add_listener(registry, &registry_listener,
                             &registry_events, this);

    this->async_run_start();

    // Start the actual thread
    pw_thread_loop_start(loop_tech);

    // First round to connect objects
    this->async_run_trigger();

    // Second round to receive metadata
    this->async_run_trigger();

    std::cout << __FUNCTION__ << ": Initialization of Pipewire service complete."  << std::endl;
    std::cout << __FUNCTION__ << "System rate: " << system.samplerate << std::endl;
    std::cout << __FUNCTION__ << "System rate (force): " << system.samplerate_force << std::endl;
    std::cout << __FUNCTION__ << "System buffersize: " << system.buffersize << std::endl;
    std::cout << __FUNCTION__ << "System buffersize (force): " << system.buffersize_force << std::endl;

    // There should be initial values, let us take these as startup condition
    pw_thread_loop_lock(loop_tech);    
     this->sort_unsorted_devices();

    std::string nmPrefix = "Dummy";

    // https://gitlab.freedesktop.org/pipewire/pipewire/-/blob/8030a9f36098e442e1659014b8cdc0e41c77388d/src/tools/pw-metadata.c

    // ==============================================================================
    // If we are in IN-OUT MODE, we declare input and out streams
    // ==============================================================================
    if(techMode == operationModeTechnology::AYF_PIPEWIRE_OPERATION_INPUT_OUTPUT)
    {
        std::list<oneDevice*> devices_linked_add;
        for(auto& elm: devices_linked)
        {
            oneDevice* newDev = elm->copy();
            elm->opMode = operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_INPUT;
            
            newDev->opMode = operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_OUTPUT;
            devices_linked_add.push_back(newDev);
        }

        for(auto& elm: devices_linked_add)
        {
            devices_linked.push_back(elm);
        }
    }

    // ==============================================================================
    // Construct the devices
    // ==============================================================================

    while(devices_linked.size())
	{
        auto elm = devices_linked.begin();
        oneDevice *theDev = *elm;

        devices_linked.erase(elm);

        std::string devName = theDev->nick;
        if(devName.empty())
        {
            devName = theDev->description; 
        }

        switch(theDev->opMode)
        {
            case operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_INPUT:
                devName += "-input";
                break;
            case operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_OUTPUT:
                devName += "-output";
                break;
            default:
                break;
        }
        
		// Do whatever is required
		CjvxAudioPWireDevice* newDevice = local_allocate_device(devName.c_str(), false,
			_common_set.theDescriptor.c_str(),
			_common_set.theFeatureClass,
			_common_set.theModuleName.c_str(),
			JVX_COMPONENT_ACCESS_SUB_COMPONENT,
			(jvxComponentType)(_common_set.theComponentType.tp + 1),
			"", NULL, JVX_SIZE_UNSELECTED, false);

        newDevice->set_references_api(theDev, this);

		// Whatever to be done for initialization
		oneDeviceWrapper elmDW;
		elmDW.hdlDev = static_cast<IjvxDevice*>(newDevice);
		elmDW.actsAsProxy = false;
		_common_tech_set.lstDevices.push_back(elmDW);

        devices_active.push_back(newDevice);
	}

    system.sysState = JVX_STATE_INIT;
    pw_thread_loop_unlock(loop_tech);    

};

void 
CjvxAudioPWireTechnology::deactivate_technology_api()
{
    for(auto elm: devices_active)
    {
        // Request specific pointer handle and move to the list of linked devices
        oneDevice* dev = elm->references_api();
        devices_linked.push_back(dev);
        
        // We do not delete the devices here since they will be deleted in base class!!
        // JVX_SAFE_DELETE_OBJECT(dev);
        // local_deallocate_device(&elm);
    }

    for(auto elm: devices_linked)
    {
        JVX_SAFE_DELETE_OBJECT(elm);
    }

    for(auto elm: devices_unsorted)
    {
        JVX_SAFE_DELETE_OBJECT(elm);
    }
	for(auto elm: nodes_unsorted_sinks)
    {
        JVX_SAFE_DELETE_OBJECT(elm);
    }
	for(auto elm: nodes_unsorted_sources)
    {
        JVX_SAFE_DELETE_OBJECT(elm);
    }
	for(auto elm: ports_unsorted)
    {
        JVX_SAFE_DELETE_OBJECT(elm);
    }

    this->async_run_stop();

    pw_thread_loop_lock(loop_tech); 
    if (metadata)
	{
    	spa_hook_remove(&metadata_listener);
        pw_proxy_destroy((struct pw_proxy*)metadata);
    }
    metadata = nullptr;

	spa_hook_remove(&registry_listener);
    pw_proxy_destroy((struct pw_proxy *)registry);
    registry = nullptr;
	// spa_hook_remove(&data.core_listener);
    pw_core_disconnect(core);
    pw_context_destroy(context);
    pw_thread_loop_unlock(loop_tech);

    pw_thread_loop_stop(loop_tech);
    pw_thread_loop_destroy(loop_tech);

    pw_deinit();

    argc = 0;
    argv = nullptr;
    loop_tech = nullptr;
    context = nullptr;
    core = nullptr;
    registry = nullptr;
    struct spa_hook registry_listener;
}

// ========================================================================================
// ========================================================================================

// All code related to rate and buffersize was taken from here:
// https://gitlab.freedesktop.org/pipewire/pipewire/-/blob/8030a9f36098e442e1659014b8cdc0e41c77388d/src/tools/pw-metadata.c
// 
static int metadata_property(void *data, uint32_t id,
		const char *key, const char *type, const char *value)
{
	
    CjvxAudioPWireTechnology* this_pointer = (CjvxAudioPWireTechnology*)data;
    if(this_pointer)
    {
        this_pointer->metadata_property_report(id, key, type, value);
    }
	return 0;
}

void 
CjvxAudioPWireTechnology::metadata_property_report(uint32_t id,
		const char *key, const char *type, const char *value)
{
	if ((data.opt_id == SPA_ID_INVALID || data.opt_id == id) &&
	    (data.opt_key == NULL || spa_streq(data.opt_key, key))) 
    {
		if (key == NULL) 
        {
			printf("remove: id:%u all keys\n", id);
		} 
        else if (value == NULL) 
        {
			printf("remove: id:%u key:'%s'\n", id, key);
		} 
        else 
        {
			printf("update: id:%u key:'%s' value:'%s' type:'%s'\n", id, key, value, type);
            if(id == 0)
            {
                if(strcmp(key, "clock.force-quantum"))
                {
                    if(value)
                    {
                        system.buffersize_force = atoi(value);
                    }
                }
                if(strcmp(key, "clock.quantum"))
                {
                    if(value)
                    {
                        system.buffersize = atoi(value);
                    }
                }
                if(strcmp(key, "clock.rate"))
                {
                    if(value)
                    {
                        system.samplerate = atoi(value);
                    }
                }
                if(strcmp(key, "clock.force-rate"))
                {
                    if(value)
                    {
                        system.samplerate_force = atoi(value);
                    }
                }
            }
		}
	}
}


static const struct pw_metadata_events metadata_events = {
	PW_VERSION_METADATA_EVENTS,
	.property = metadata_property
};


void 
CjvxAudioPWireTechnology::event_global_callback(uint32_t id,
                uint32_t permissions, const char *type, uint32_t version,
                const struct spa_dict *props)
{
    printf("object: id:%u type:%s/%d\n", id, type, version);

#if 0
    if (strcmp(type, PW_TYPE_INTERFACE_Metadata) == 0) 
    {
    	const char *str;
    	if (props != NULL &&
	        (str = spa_dict_lookup(props, PW_KEY_METADATA_NAME)) != NULL &&
	        !spa_streq(str, data.opt_name))
		    return;

	    if (metadata != NULL) 
        {
    		pw_log_warn("Multiple metadata: ignoring metadata %d", id);
		    return;
	    }

	    printf("Found \"%s\" metadata %d\n", data.opt_name, id);
	    metadata = (pw_metadata*)pw_registry_bind(registry,
			    id, type, PW_VERSION_METADATA, 0);

	    if (data.opt_delete) 
        {
		    if (data.opt_id != SPA_ID_INVALID) 
            {
			    if (data.opt_key != NULL)
				    printf("delete property: id:%u key:%s\n", data.opt_id, data.opt_key);
			    else
    				printf("delete properties: id:%u\n", data.opt_id);
			    pw_metadata_set_property(metadata, data.opt_id, data.opt_key, NULL, NULL);
		    } 
            else 
            {
			    printf("delete all properties\n");
			    pw_metadata_clear(metadata);
		    }
	    } 
        else if (data.opt_id != SPA_ID_INVALID && data.opt_key != NULL && data.opt_value != NULL) 
        {
		    printf("set property: id:%u key:%s value:%s type:%s\n",
				data.opt_id, data.opt_key, data.opt_value, data.opt_type);
		    pw_metadata_set_property(metadata, data.opt_id, data.opt_key, data.opt_type, data.opt_value);
	    } 
        else 
        {
            pw_metadata_add_listener(metadata,
				    &metadata_listener,
				    &metadata_events, this);        
	    }
    }
#endif
#if 1
    if (strcmp(type, PW_TYPE_INTERFACE_Metadata) == 0) 
    {
        // Install a listener to the metadata type
        // Code mainly taken from here
        // https://gitlab.freedesktop.org/pipewire/pipewire/-/blob/8030a9f36098e442e1659014b8cdc0e41c77388d/src/tools/pw-metadata.c
        // -- and simplified!
        const char *metadata_name = spa_dict_lookup(props, "metadata.name");
        if (metadata_name && (strcmp(metadata_name, "settings") == 0))
        {
	        assert (metadata == NULL);
	        metadata = (pw_metadata*)pw_registry_bind(registry, id, type, PW_VERSION_METADATA, 0);
            pw_metadata_add_listener(metadata, &metadata_listener, &metadata_events, this);        
	    }
    }
#endif

    if (strcmp(type, PW_TYPE_INTERFACE_Device) == 0) 
    {
        const char *media_class = spa_dict_lookup(props, "media.class");
        if (media_class && (strstr(media_class, "Device") || strstr(media_class, "device"))) {
            const char* serial =  spa_dict_lookup(props, "object.serial");
            const char *nick = spa_dict_lookup(props, "device.nick");
            const char *descr = spa_dict_lookup(props, "device.description");
            const char *api_name = spa_dict_lookup(props, "device.api");
            const char *name = spa_dict_lookup(props, "device.name");
            oneDevice* newDev = nullptr;
            JVX_SAFE_ALLOCATE_OBJECT(newDev, oneDevice);
            newDev->id = id;
            if(serial) newDev->serial = atoi(serial);
            if(descr) newDev->description = descr;
            if(nick) newDev->nick = nick;
            if(api_name) newDev->api_name = api_name;
            if(name) newDev->name = name;

            devices_unsorted.push_back(newDev);
        }
    }
    if (strcmp(type, PW_TYPE_INTERFACE_Node) == 0) 
    {
        const char *media_class = spa_dict_lookup(props, "media.class");
        if (media_class && (strstr(media_class, "Sink") || strstr(media_class, "sink"))) 
        {
            const char* serial = spa_dict_lookup(props, "object.serial");
            const char *dev_id = spa_dict_lookup(props, "device.id");
            const char *nick = spa_dict_lookup(props, "node.nick");
            const char *descr = spa_dict_lookup(props, "node.description");
            const char *name = spa_dict_lookup(props, "node.name");

            oneNode* newNode = nullptr;
            JVX_SAFE_ALLOCATE_OBJECT(newNode, oneNode);
            newNode->id = id;
            if(serial) newNode->serial = atoi(serial);
            newNode->isSink = true;
            if(dev_id) newNode->ref_device = atoi(dev_id);
            if(descr) newNode->description = descr;
            if(nick) newNode->nick = nick;
            if(name) newNode->name = name;
            nodes_unsorted_sinks.push_back(newNode);
        }
        else if (media_class && (strstr(media_class, "Source") || strstr(media_class, "source"))) 
        {
            const char* serial = spa_dict_lookup(props, "object.serial");
            const char *dev_id = spa_dict_lookup(props, "device.id");
            const char *nick = spa_dict_lookup(props, "node.nick");
            const char *descr = spa_dict_lookup(props, "node.description");
            const char *name = spa_dict_lookup(props, "node.name");

            oneNode* newNode = nullptr;
            JVX_SAFE_ALLOCATE_OBJECT(newNode, oneNode);
            
            newNode->id = id;
            if(serial) newNode->serial = atoi(serial);
            newNode->isSink = false;
            if(dev_id) newNode->ref_device = atoi(dev_id);
            if(descr) newNode->description = descr;
            if(nick) newNode->nick = nick;
            if(name) newNode->name = name;
            nodes_unsorted_sources.push_back(newNode);
        }
    }

    if (strcmp(type, PW_TYPE_INTERFACE_Port) == 0) 
    {
        const char *audio_channel = spa_dict_lookup(props, "audio.channel");
        if (audio_channel) 
        {
            const char *serial = spa_dict_lookup(props, "object.serial");
            const char *node_id = spa_dict_lookup(props, "node.id");
            const char *port_id = spa_dict_lookup(props, "port.id");
            const char *nick = spa_dict_lookup(props, "port.nick");
            const char *name = spa_dict_lookup(props, "port.name");
            const char *direction = spa_dict_lookup(props, "port.direction");
            const char *physical = spa_dict_lookup(props, "port.physical");

            onePort* newPort = nullptr;
            JVX_SAFE_ALLOCATE_OBJECT(newPort, onePort);
            newPort->id = id;
            if(serial) newPort->serial = atoi(serial);
            if(node_id) newPort->ref_node = atoi(node_id);
            if(port_id) newPort->port_id = atoi(port_id);
            if(nick) newPort->nick = nick;
            if(name) newPort->name = name;
            if(direction) newPort->direction = direction;
            if(physical) newPort->physical = physical;
            ports_unsorted.push_back(newPort);
        }        
    }
}

// ========================================================================================
// ========================================================================================

// All related to async system requests - already synchronized 
void 
CjvxAudioPWireTechnology::async_run_start()
{
    pw_core_add_listener(core, &async_listener, &async_events, this);
}

void 
CjvxAudioPWireTechnology::async_run_stop()
{
    spa_hook_remove(&async_listener);
}

jvxErrorType 
CjvxAudioPWireTechnology::async_run_trigger()
{
    if(async_run.idMess_core != 0)
    {
        return JVX_ERROR_COMPONENT_BUSY;
    }
    JVX_WAIT_FOR_NOTIFICATION_I(async_run.notWait);
    async_run.idMess_received = 0;
    async_run.idMess_core = pw_core_sync(core, PW_ID_CORE, 0);
    JVX_WAIT_FOR_NOTIFICATION_II_MS(async_run.notWait, JVX_ASYNC_TIMEOUT);

    if(async_run.idMess_received == async_run.idMess_core )
    {
        std::cout << __FUNCTION__ << "Async message transfer successfully completed." << std::endl;  
        async_run.idMess_core = 0;      
    }
    else
    {
        std::cout << __FUNCTION__ << "Error in asynchronous message transfer." << std::endl;
    }
    return JVX_NO_ERROR;
}

// Collect requested data right here!!
// ========================================================================================
// ========================================================================================

void 
CjvxAudioPWireTechnology::async_run_cb(jvxUInt32 id, int seq)
{
    if(seq == async_run.idMess_core )
    {
        async_run.idMess_received = seq;
        JVX_SET_NOTIFICATION(async_run.notWait);
    }
}

void 
CjvxAudioPWireTechnology::sort_unsorted_devices()
{
    // Associate the ports to the nodes and the nodes to the devices
    std::list<onePort*> new_ports_unsorted;
       
    while(ports_unsorted.size())
    {
        auto elmP = ports_unsorted.begin();

        // Extract one port
        onePort* thePort = (*elmP);
        ports_unsorted.erase(elmP);

        jvxSize searchId = thePort->ref_node;
        jvxBool newParentFound = false;
        for(auto& elmN: nodes_unsorted_sinks)
        {
            if(elmN->id == searchId)
            {
                if(thePort->direction == "in")
                {
                    elmN->in_ports.push_back(thePort);
                }
                else
                {                
                    elmN->out_ports.push_back(thePort);                    
                }
                newParentFound = true;
            }
        }
        if(!newParentFound)
        {
            for (auto &elmN : nodes_unsorted_sources)
            {
                if (elmN->id == searchId)
                {
                    if (thePort->direction == "in")
                    {
                        elmN->in_ports.push_back(thePort);
                    }
                    else
                    {
                        elmN->out_ports.push_back(thePort);
                    }
                    newParentFound = true;
                }
            }
        }
        if(!newParentFound)
        {
            new_ports_unsorted.push_back(thePort);
        }
    }
    ports_unsorted = new_ports_unsorted;

    // ============================================================================
    std::list<oneNode*> new_nodes_unsorted_sinks;
    while(nodes_unsorted_sinks.size())
    {
        auto elmN = nodes_unsorted_sinks.begin();

        // Extract one port
        oneNode* theNode = (*elmN);
        nodes_unsorted_sinks.erase(elmN);

        jvxSize searchId = theNode->ref_device;
        jvxBool newParentFound = false;
        for(auto& elmD: devices_unsorted)
        {
            jvxSize searchId = theNode->ref_device;

            if(elmD->id == searchId)
            {
                if(theNode->isSink)
                {
                    elmD->sinks.push_back(theNode);
                }
                else
                {                
                    elmD->sources.push_back(theNode);                    
                }
                newParentFound = true;
            }
        }
        if(!newParentFound)
        {
            new_nodes_unsorted_sinks.push_back(theNode);
        }
    }
    nodes_unsorted_sinks = new_nodes_unsorted_sinks;

    // ============================================================================
    std::list<oneNode*> new_nodes_unsorted_sources;
    while(nodes_unsorted_sources.size())
    {
        auto elmN = nodes_unsorted_sources.begin();

        // Extract one port
        oneNode* theNode = (*elmN);
        nodes_unsorted_sources.erase(elmN);

        jvxSize searchId = theNode->ref_device;
        jvxBool newParentFound = false;
        for(auto& elmD: devices_unsorted)
        {
            jvxSize searchId = theNode->ref_device;

            if(elmD->id == searchId)
            {
                if(theNode->isSink)
                {
                    elmD->sinks.push_back(theNode);
                }
                else
                {                
                    elmD->sources.push_back(theNode);                    
                }
                newParentFound = true;
            }
        }
        if(!newParentFound)
        {
            new_nodes_unsorted_sources.push_back(theNode);
        }
    }
    nodes_unsorted_sources = new_nodes_unsorted_sources;

    // Finally, pre-sort the valid devices!
    std::list<oneDevice*> new_devices_unsorted;
    while(devices_unsorted.size())
    {
        auto elmD = devices_unsorted.begin();
        oneDevice* newDev = *elmD;

        devices_unsorted.erase(elmD);
        if(check_device_ready(newDev))
        {
            devices_linked.push_back(newDev);
        }
        else
        {
            new_devices_unsorted.push_back(newDev);
        }
    } 
}

jvxBool 
CjvxAudioPWireTechnology::check_device_ready(oneDevice* theDevice)
{
    jvxBool isReady = false;
    isReady = (theDevice->sinks.size() > 0) ||(theDevice->sources.size() > 0);
    return isReady;
}

