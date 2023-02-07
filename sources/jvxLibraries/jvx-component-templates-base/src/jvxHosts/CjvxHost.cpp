#include "jvx.h"

#include "jvxHosts/CjvxHost.h"
#include <iostream>

#include "jvxHosts/CjvxHost-tpl.h"

// ===================================================================
// Some static helpers
// ===================================================================

/**
 * Check entries in list to detect existing entries
 */
static bool
isInList(std::vector<std::string> lst, std::string entry)
{
	jvxSize i;
	for (i = 0; i < lst.size(); i++)
	{
		if (lst[i] == entry)
		{
			return(true);
		}
	}
	return(false);
}

// ====================================================================
// Member functions: Allocate/deallocate object
// ====================================================================

/**
 * Constructor: Only those commands required on this class derivation level.
 */
CjvxHost::CjvxHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.tp = JVX_COMPONENT_HOST;
	_common_set.theComponentType.slotid = JVX_SIZE_UNSELECTED;
	_common_set.theComponentType.slotsubid = JVX_SIZE_UNSELECTED;

	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxHost*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	_common_set_host.reportOnConfig = NULL;
	_common_set_host.reportUnit = NULL;

	_common_set_sequencer.inOperation = NULL;

	myUniqueIdSelf = 1;
	hostUniqueId = myUniqueIdSelf + 1;

	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);

	this->_set_unique_descriptor("main host");
}

/**
 * Destructor: Only terminate call
 */
CjvxHost::~CjvxHost()
{
	this->terminate();
}

// ====================================================================
// Member functions: State transitions
// ====================================================================

/*
 * Initialize the current component
 */
jvxErrorType
CjvxHost::initialize(IjvxHiddenInterface* theOtherhost)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theOtherhost == NULL)
	{
		theOtherhost = static_cast<IjvxHiddenInterface*>(this);
	}
	else
	{
		// Does this happen?? A host in a host would not make sense..
		assert(0);
	}

	res = _initialize(theOtherhost);
	if (res == JVX_NO_ERROR)
	{
		_common_set_host.threadId_registered = JVX_GET_CURRENT_THREAD_ID();

		reset();
	}
	return(res);
}

jvxErrorType
CjvxHost::select(IjvxObject* theOwner)
{
	jvxErrorType res = _select(theOwner);
	if (res == JVX_NO_ERROR)
	{
		_common_set.theComponentType.slotid = 0;
		_common_set.theComponentType.slotsubid = 0;

		// The unique id is obtained in the select function. However, this can not happen within the host since a valid host reference must be present
		// request_unique_host_id(&_common_set.register_host_id);

		this->init__properties_selected();
		this->allocate__properties_selected();
		this->register__properties_selected(static_cast<CjvxProperties*>(this));

		this->init__ext_callbacks__properties_callbacks();
		this->allocate__ext_callbacks__properties_callbacks();
		this->register__ext_callbacks__properties_callbacks(static_cast<CjvxProperties*>(this));

		CjvxHost_genpcg::register_callbacks(static_cast<CjvxProperties*>(this),
			cb_command_post_set,
			cb_command_pre_get,
			reinterpret_cast<jvxHandle*>(this), NULL);

	}
	return(res);
}

void
CjvxHost::add_self_reference()
{
	// Add this host to list of accessible objects
	// All other components
	jvxOneComponentModule elm;

	elm.theObj_single = static_cast<IjvxObject*>(this);
	elm.common.isExternalComponent = true;
	elm.common.id = (jvxSize)_common_set_host.otherComponents.availableOtherComponents.size();
	elm.common.linkage.dllPath = "NONE";
	elm.common.linkage.sharedObjectEntry = NULL;
	elm.common.linkage.funcInit = NULL;
	elm.common.linkage.funcTerm = NULL;
	elm.common.tp = JVX_COMPONENT_HOST;
	elm.common.allowsMultiObjects = false;
	elm.common.externalLink.description = "main host object";
	elm.common.dllProps = 0;
	elm.common.hObject = elm.theObj_single;

	_common_set_host.otherComponents.availableOtherComponents.push_back(elm);
}

void
CjvxHost::remove_self_reference()
{
	auto elm = _common_set_host.otherComponents.availableOtherComponents.begin();
	for (; elm != _common_set_host.otherComponents.availableOtherComponents.end(); elm++)
	{
		if (elm->theObj_single == static_cast<IjvxObject*>(this))
		{
			_common_set_host.otherComponents.availableOtherComponents.erase(elm);
			break;
		}
	}
}

/*
 * Activate host component
 */
jvxErrorType
CjvxHost::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;

	res = _activate_no_text_log(); // We need to wait with text log start until components have been loaded
	if (res == JVX_NO_ERROR)
	{
		_set_system_refs(_common_set_min.theHostRef, _common_set_host.reportUnit);

		CjvxHost_genpcg::allocate__ext_interfaces__properties_active();
		CjvxHost_genpcg::register__ext_interfaces__properties_active(static_cast<CjvxProperties*>(this));

		// ===================================================================================================

		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.component_path.category,
			CjvxHost_genpcg::properties_selected.component_path.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.configure_parts.category,
			CjvxHost_genpcg::properties_selected.configure_parts.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.do_unload_dlls.category,
			CjvxHost_genpcg::properties_selected.do_unload_dlls.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.host_output_cout.category,
			CjvxHost_genpcg::properties_selected.host_output_cout.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.host_verbose_dll.category,
			CjvxHost_genpcg::properties_selected.host_verbose_dll.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_activate.category,
			CjvxHost_genpcg::properties_selected.textLog_activate.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_dbglevel.category,
			CjvxHost_genpcg::properties_selected.textLog_dbglevel.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_expressions.category,
			CjvxHost_genpcg::properties_selected.textLog_expressions.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_filename.category,
			CjvxHost_genpcg::properties_selected.textLog_filename.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.category,
			CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.category,
			CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.category,
			CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.globalIdx);

		if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_restart.value == NULL)
		{
			CjvxHost_genpcg::ext_interfaces.properties_active.trigger_restart.isValid = false;
		}
		if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_shutdown.value == NULL)
		{
			CjvxHost_genpcg::ext_interfaces.properties_active.trigger_shutdown.isValid = false;
		}

		load_dynamic_objects();

		// add_self_reference();
		for (i = 0; i < _common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single)
			{
				_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single->single_instance_post_bootup(static_cast<IjvxHiddenInterface*>(this));
			}
		}

		if (CjvxHost_genpcg::properties_selected.textLog_activate.value == c_true)
		{
			if (_common_set.theToolsHost)
			{
				jvxErrorType resL = _common_set.theToolsHost->reference_tool(JVX_COMPONENT_SYSTEM_TEXT_LOG,
					&jvxrtst_bkp.theTextLogger_obj, 0, NULL);
				if ((resL == JVX_NO_ERROR) && jvxrtst_bkp.theTextLogger_obj)
				{
					resL = jvxrtst_bkp.theTextLogger_obj->request_specialization(reinterpret_cast<jvxHandle**>(&jvxrtst_bkp.theTextLogger_hdl), NULL, NULL);
				}
			}

			if (jvxrtst_bkp.theTextLogger_hdl)
			{
				std::cout << "Starting text log file:" << std::endl;
				std::cout << " --> Filename = " << CjvxHost_genpcg::properties_selected.textLog_filename.value << std::endl;
				std::cout << " --> Internal file buffersize = " << CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.value << std::endl;
				std::cout << " --> Internal cbuf buffersize = " << CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.value << std::endl;
				std::cout << " --> File transfer size = " << CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.value << std::endl;
				std::cout << " --> Debug level = " << CjvxHost_genpcg::properties_selected.textLog_dbglevel.value << std::endl;

				jvxrtst_bkp.theTextLogger_hdl->initialize(
					static_cast<IjvxHiddenInterface*>(this),
					CjvxHost_genpcg::properties_selected.textLog_filename.value.c_str(),
					CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.value,
					CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.value,
					CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.value,
					CjvxHost_genpcg::properties_selected.textLog_dbglevel.value);

				std::cout << " --> Module selection expression = " << std::endl;
				for (i = 0; i < CjvxHost_genpcg::properties_selected.textLog_expressions.value.size(); i++)
				{
					std::cout << " -->-->" << CjvxHost_genpcg::properties_selected.textLog_expressions.value[i] << std::endl;
					jvxrtst_bkp.theTextLogger_hdl->addTextLogExpression(CjvxHost_genpcg::properties_selected.textLog_expressions.value[i].c_str());
				}
				jvxrtst_bkp.theTextLogger_hdl->start();
			}
		}
	}

	return(res);
}

jvxErrorType
CjvxHost::unselectAllComponents()
{
	jvxSize i, j, k;
	// A little bit of complicated since the list of slots is resized on every unselect!!
	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		while (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech.size())
		{
			for (j = 0; j < _common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech.size(); j++)
			{
				if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_tech)
				{
					while (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev.size())
					{
						for (k = 0; k < _common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev.size(); k++)
						{
							if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].dev)
							{
								jvxComponentIdentification tp;
								tp.tp = _common_set_types.registeredTechnologyTypes[i].selector[1];
								tp.slotid = j;
								tp.slotsubid = k;
								this->connection_factory_to_be_removed(
									tp,
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].cfac,
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].mfac);


								if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].cfac)
								{
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].dev->return_hidden_interface(
										JVX_INTERFACE_CONNECTOR_FACTORY,
										reinterpret_cast<jvxHandle*>(_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].cfac));
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].cfac = NULL;
								}

								if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].mfac)
								{
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].dev->return_hidden_interface(
										JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
										reinterpret_cast<jvxHandle*>(_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].mfac));
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].mfac = NULL;
								}

								this->deactivate_selected_component(
									jvxComponentIdentification(_common_set_types.registeredTechnologyTypes[i].selector[1], j, k));

								// We need to pass a reference here to set the slot id /subid to unselected. 
								// If we run this termination, the slots must be reset by the caller!!
								jvxComponentIdentification unselectThis = jvxComponentIdentification(_common_set_types.registeredTechnologyTypes[i].selector[1], j, k);
								this->unselect_selected_component(unselectThis);
								break;
							}
						}
					}

					/* !! Important hint: Technologies typically do not have connector factories, therefore we do not need to report to host hook !! */
					this->deactivate_selected_component(
						jvxComponentIdentification(_common_set_types.registeredTechnologyTypes[i].selector[0], j, 0));

					// We need to pass a reference here to set the slot id /subid to unselected.
					// If we run this termination, the slots must be reset by the caller!!
					jvxComponentIdentification unselectThis = jvxComponentIdentification(_common_set_types.registeredTechnologyTypes[i].selector[0], j, 0);
					this->unselect_selected_component(unselectThis);
					break;
				}
			}
		}
	}

	for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
	{
		while (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut.size())
		{
			for (j = 0; j < _common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut.size(); j++)
			{
				if (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].obj)
				{
					// This is the second termination location. Only those functions are handled here which were not
					// completed in the first steps
					if (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac ||
						_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac)
					{
						jvxComponentIdentification tp;
						tp.tp = _common_set_types.registeredNodeTypes[i].selector[0];
						tp.slotid = j;
						tp.slotsubid = 0;

						this->connection_factory_to_be_removed(
							tp,
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac,
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac);

						if (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac)
						{
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].obj->return_hidden_interface(
								JVX_INTERFACE_CONNECTOR_FACTORY,
								reinterpret_cast<jvxHandle*>(_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac));
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac = NULL;
						}
						if (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac)
						{
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].obj->return_hidden_interface(
								JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
								reinterpret_cast<jvxHandle*>(_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac));
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac = NULL;
						}
					}
					this->deactivate_selected_component(
						jvxComponentIdentification(_common_set_types.registeredNodeTypes[i].selector[0], j, 0));


					// We need to pass a reference here to set the slot id /subid to unselected.
					// If we run this termination, the slots must be reset by the caller!!
					jvxComponentIdentification unselectThis = jvxComponentIdentification(_common_set_types.registeredNodeTypes[i].selector[0], j, 0);
					this->unselect_selected_component(unselectThis);
					break;
				}
			}
		}
	}
	return JVX_NO_ERROR;
}
/*
 * Deactivate host component
 */
jvxErrorType
CjvxHost::deactivate()
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// remove_self_reference();

		unselectAllComponents();

		jvxState stat = JVX_STATE_NONE;
		for (i = 0; i < _common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single)
			{
				_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single->single_instance_pre_shutdown();
			}
			_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->state(&stat);
			assert(stat == JVX_STATE_NONE);
		}

		if (jvxrtst_bkp.theTextLogger_hdl)
		{
			jvxrtst_bkp.theTextLogger_hdl->stop();
			jvxrtst_bkp.theTextLogger_hdl->terminate();

			_common_set.theToolsHost->return_reference_tool(jvxComponentIdentification(JVX_COMPONENT_SYSTEM_TEXT_LOG, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED), jvxrtst_bkp.theTextLogger_obj);
			jvxrtst_bkp.theTextLogger_obj = NULL;
			jvxrtst_bkp.theTextLogger_hdl = NULL;
		}

		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.component_path.category,
			CjvxHost_genpcg::properties_selected.component_path.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.configure_parts.category,
			CjvxHost_genpcg::properties_selected.configure_parts.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.do_unload_dlls.category,
			CjvxHost_genpcg::properties_selected.do_unload_dlls.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.host_output_cout.category,
			CjvxHost_genpcg::properties_selected.host_output_cout.globalIdx);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.host_verbose_dll.category,
			CjvxHost_genpcg::properties_selected.host_verbose_dll.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_activate.category,
			CjvxHost_genpcg::properties_selected.textLog_activate.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_dbglevel.category,
			CjvxHost_genpcg::properties_selected.textLog_dbglevel.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_expressions.category,
			CjvxHost_genpcg::properties_selected.textLog_expressions.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_filename.category,
			CjvxHost_genpcg::properties_selected.textLog_filename.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.category,
			CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.category,
			CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.globalIdx);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.category,
			CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.globalIdx);

		CjvxHost_genpcg::unregister__ext_interfaces__properties_active(static_cast<CjvxProperties*>(this));
		CjvxHost_genpcg::deallocate__ext_interfaces__properties_active();

		res = _deactivate_no_text_log();
		assert(res == JVX_NO_ERROR);

		// unload all Dlls
		unload_dynamic_objects();

		_set_system_refs(nullptr, nullptr);
	}

	return(res);
}

jvxErrorType
CjvxHost::unselect()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		CjvxHost_genpcg::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);

		unregister__ext_callbacks__properties_callbacks(static_cast<CjvxProperties*>(this));
		deallocate__ext_callbacks__properties_callbacks();

		unregister__properties_selected(static_cast<CjvxProperties*>(this));
		deallocate__properties_selected();

		res = _unselect();
		assert(res == JVX_NO_ERROR);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

/*
 * Terminate host component
 */
jvxErrorType
CjvxHost::terminate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _terminate();
	return(res);
}

// ====================================================================
// Member functions: Configuration
// ====================================================================



// ============================================================================================
/*
jvxErrorType
CjvxHost::set_external_widget_host(jvxHandle* theWidgetHost, jvxWidgetHostType widgetType)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_ERROR_UNSUPPORTED;
	return res;
}
*/

// ====================================================================
// Member functions: IjvxHiddenInterface
// ====================================================================

jvxErrorType
CjvxHost::object_hidden_interface(IjvxObject** objRef)
{
	if (objRef)
	{
		*objRef = static_cast<IjvxObject*>(this);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHost::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxHostInteraction::request_hidden_interface(tp, hdl);
	if (res != JVX_NO_ERROR)
	{
		res = JVX_NO_ERROR;
		switch (tp)
		{
		case JVX_INTERFACE_PROPERTIES:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this));
			}
			break;

		case JVX_INTERFACE_TOOLS_HOST:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxToolsHost*>(this));
			}
			return(JVX_NO_ERROR);

		case JVX_INTERFACE_UNIQUE_ID:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxUniqueId*>(this));
			}
			return(JVX_NO_ERROR);

		case JVX_INTERFACE_PROPERTY_ATTACH:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxPropertyAttach*>(this));
			}
			return(JVX_NO_ERROR);

		case JVX_INTERFACE_SEQUENCER:
			if (hdl)
			{
				*hdl = static_cast<IjvxSequencer*>(this);
			}
			break;
		case JVX_INTERFACE_HOSTTYPEHANDLER:
			if (hdl)
			{
				*hdl = static_cast<IjvxHostTypeHandler*>(this);
			}
			break;
		case JVX_INTERFACE_HOST:
			if (hdl)
			{
				*hdl = static_cast<IjvxHost*>(this);
			}
			break;

		case JVX_INTERFACE_DATA_CONNECTIONS:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxDataConnections*>(this));
			}
			break;

		default:

			res = _request_hidden_interface(tp, hdl);
		}
	}
	return(res);
}

jvxErrorType
CjvxHost::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxHostInteraction::return_hidden_interface(tp, hdl);
	if (res != JVX_NO_ERROR)
	{
		res = JVX_NO_ERROR;
		switch (tp)
		{
		case JVX_INTERFACE_PROPERTIES:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_TOOLS_HOST:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxToolsHost*>(this)))
			{
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_INVALID_ARGUMENT);

		case JVX_INTERFACE_UNIQUE_ID:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxUniqueId*>(this)))
			{
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_INVALID_ARGUMENT);

		case JVX_INTERFACE_PROPERTY_ATTACH:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxPropertyAttach*>(this)))
			{
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_INVALID_ARGUMENT);


		case JVX_INTERFACE_SEQUENCER:
			if (hdl == static_cast<IjvxSequencer*>(this))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_HOSTTYPEHANDLER:
			if (hdl == static_cast<IjvxHostTypeHandler*>(this))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_HOST:
			if (hdl == static_cast<IjvxHost*>(this))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_DATA_CONNECTIONS:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxDataConnections*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;
		default:
			res = _return_hidden_interface(tp, hdl);
		}
	}
	return res;
}



jvxErrorType
CjvxHost::request_hidden_interface_selected_component(const jvxComponentIdentification& tp, jvxInterfaceType ifTp, jvxHandle** iface)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		IjvxHiddenInterface* theIface = NULL;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					theIface = static_cast<IjvxHiddenInterface*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							theIface = static_cast<IjvxHiddenInterface*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
				elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

				if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
				{
					if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
					{
						if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
						{
							theIface = static_cast<IjvxHiddenInterface*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
			}
		}

		if ((res == JVX_NO_ERROR) && theIface)
		{
			res = theIface->request_hidden_interface(ifTp, iface);
			if (res != JVX_NO_ERROR)
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
CjvxHost::return_hidden_interface_selected_component(const jvxComponentIdentification& tp, jvxInterfaceType ifTp, jvxHandle* iface)
{

	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		IjvxHiddenInterface* theIface = NULL;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					theIface = static_cast<IjvxHiddenInterface*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
				}
				else
				{
					res = JVX_ERROR_WRONG_STATE_SUBMODULE;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							theIface = static_cast<IjvxHiddenInterface*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
				elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

				if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
				{
					if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
					{
						if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
						{
							theIface = static_cast<IjvxHiddenInterface*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
			}
		}

		if ((res == JVX_NO_ERROR) && theIface)
		{
			res = theIface->return_hidden_interface(ifTp, iface);
			if (res != JVX_NO_ERROR)
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

// ======================================================================
// ======================================================================

jvxErrorType
CjvxHost::request_object_selected_component(const jvxComponentIdentification& tp, IjvxObject** theObj)
{

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
					{
						if (theObj)
						{
							*theObj = static_cast<IjvxObject*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
						}
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE_SUBMODULE;
					}
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							if (theObj)
							{
								*theObj = static_cast<IjvxObject*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
							}
							res = JVX_NO_ERROR;
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
			elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

			if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
			{
				if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
						{
							if (theObj)
							{
								*theObj = static_cast<IjvxObject*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj);
							}
							res = JVX_NO_ERROR;
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
			elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

			if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
			{
				if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						if (elmIt_si->instances.theHandle_shortcut[tp.slotid].obj)
						{
							if (theObj)
							{
								*theObj = static_cast<IjvxObject*>(elmIt_si->instances.theHandle_shortcut[tp.slotid].obj);
							}
							res = JVX_NO_ERROR;
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
				}
			}
		}

		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			// Special rule to return host component
			if (
				(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
			{
				if (theObj)
				{
					*theObj = static_cast<IjvxObject*>(this);
				}
				res = JVX_NO_ERROR;
			}
			else if (tp.tp == JVX_COMPONENT_PROCESSING_PROCESS)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				jvxSize num = 0;
				jvxSize i;
				IjvxDataConnectionProcess* conProc = NULL;
				jvxComponentIdentification tpComp;

				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				number_connections_process(&num);
				for (i = 0; i < num; i++)
				{
					reference_connection_process(i, &conProc);
					assert(conProc);
					conProc->request_specialization(NULL, &tpComp, NULL, NULL);
					return_reference_connection_process(conProc);
					if (tp.slotid == tpComp.slotid)
					{
						conProc->object_hidden_interface(theObj);
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
			else if (tp.tp == JVX_COMPONENT_PROCESSING_GROUP)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				jvxSize num = 0;
				jvxSize i;
				IjvxDataConnectionGroup* conGrp = NULL;
				jvxComponentIdentification tpComp;

				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				number_connections_group(&num);
				for (i = 0; i < num; i++)
				{
					reference_connection_group(i, &conGrp);
					assert(conGrp);
					conGrp->request_specialization(NULL, &tpComp, NULL, NULL);
					return_reference_connection_group(conGrp);
					if (tp.slotid == tpComp.slotid)
					{
						conGrp->object_hidden_interface(theObj);
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
			else
			{
				res = reference_tool(tp, theObj, JVX_SIZE_UNSELECTED, nullptr);
			}
		}
	}

	return(res);
}

jvxErrorType
CjvxHost::return_object_selected_component(const jvxComponentIdentification& tp, IjvxObject* theObj)
{
	IjvxObject* theObjComp = NULL;

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;

		std::vector<oneTechnologyType>::iterator elmIt_tech;
		elmIt_tech = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 0);
		if (elmIt_tech != _common_set_types.registeredTechnologyTypes.end())
		{
			res = JVX_NO_ERROR;
			if (tp.slotid < elmIt_tech->technologyInstances.selTech.size())
			{
				if (tp.slotsubid != 0)
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
				else
				{
					if (elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
					{
						theObjComp = static_cast<IjvxObject*>(elmIt_tech->technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech);
					}
					else
					{
						res = JVX_ERROR_WRONG_STATE_SUBMODULE;
					}
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneTechnologyType>::iterator elmIt_dev;
			elmIt_dev = jvx_findItemSelectorInList_one<oneTechnologyType, jvxComponentType>(_common_set_types.registeredTechnologyTypes, tp.tp, 1);
			if (elmIt_dev != _common_set_types.registeredTechnologyTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_dev->technologyInstances.selTech.size())
				{
					if (tp.slotsubid < elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev.size())
					{
						if (elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev)
						{
							theObjComp = static_cast<IjvxObject*>(elmIt_dev->technologyInstances.selTech[tp.slotid].theHandle_shortcut_dev[tp.slotsubid].dev);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxNode>>::iterator elmIt_ep;
			elmIt_ep = jvx_findItemSelectorInList_one<oneObjType<IjvxNode>, jvxComponentType>(_common_set_types.registeredNodeTypes, tp.tp, 0);

			if (elmIt_ep != _common_set_types.registeredNodeTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_ep->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						if (elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj)
						{
							theObjComp = static_cast<IjvxObject*>(elmIt_ep->instances.theHandle_shortcut[tp.slotid].obj);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}
		if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			std::vector<oneObjType<IjvxSimpleComponent>>::iterator elmIt_si;
			elmIt_si = jvx_findItemSelectorInList_one<oneObjType<IjvxSimpleComponent>, jvxComponentType>(_common_set_types.registeredSimpleTypes, tp.tp, 0);

			if (elmIt_si != _common_set_types.registeredSimpleTypes.end())
			{
				res = JVX_NO_ERROR;
				if (tp.slotid < elmIt_si->instances.theHandle_shortcut.size())
				{
					if (tp.slotsubid != 0)
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					else
					{
						if (elmIt_si->instances.theHandle_shortcut[tp.slotid].obj)
						{
							theObjComp = static_cast<IjvxObject*>(elmIt_si->instances.theHandle_shortcut[tp.slotid].obj);
						}
						else
						{
							res = JVX_ERROR_WRONG_STATE_SUBMODULE;
						}
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
		}

		if (res == JVX_NO_ERROR)
		{
			if (theObjComp)
			{
				if (theObjComp != theObj)
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			return res;
		}

		if(res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			if (
				(tp.tp == JVX_COMPONENT_HOST) && (tp.slotid == 0) && (tp.slotsubid == 0))
			{
				res = JVX_NO_ERROR;
				if (theObj != static_cast<IjvxObject*>(this))
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else if (tp.tp == JVX_COMPONENT_PROCESSING_PROCESS)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				jvxSize num = 0;
				jvxSize i;
				IjvxDataConnectionProcess* conProc = NULL;
				jvxComponentIdentification tpComp;

				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				number_connections_process(&num);
				for (i = 0; i < num; i++)
				{
					reference_connection_process(i, &conProc);
					assert(conProc);
					conProc->request_specialization(NULL, &tpComp, NULL, NULL);
					return_reference_connection_process(conProc);
					if (tp.slotid == tpComp.slotid)
					{
						// conProc->object_hidden_interface(theObj); returning not directly required
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
			else if (tp.tp == JVX_COMPONENT_PROCESSING_GROUP)
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				jvxSize num = 0;
				jvxSize i;
				IjvxDataConnectionGroup* conGrp = NULL;
				jvxComponentIdentification tpComp;

				res = JVX_ERROR_ELEMENT_NOT_FOUND;
				number_connections_group(&num);
				for (i = 0; i < num; i++)
				{
					reference_connection_group(i, &conGrp);
					assert(conGrp);
					conGrp->request_specialization(NULL, &tpComp, NULL, NULL);
					return_reference_connection_group(conGrp);
					if (tp.slotid == tpComp.slotid)
					{
						// conProc->object_hidden_interface(theObj); returning not directly required
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
			else
			{
				res = return_reference_tool(tp, theObj);
			}
		}
	}
	
	return(res);
}

void
CjvxHost::load_dynamic_objects()
{
}

void
CjvxHost::unload_dynamic_objects()
{
}

// ===============================================================================================

// ================================================================
// Interface IjvxToolsHost
// ================================================================

jvxErrorType
CjvxHost::number_tools(const jvxComponentIdentification& tp, jvxSize* num)
{
	jvxErrorType res = JVX_NO_ERROR;
	int numRet = 0;
	jvxSize cnt = 0;
	jvxSize i;

	res = this->number_components_system(tp, num);

	// If the first part returned an error, the type may reference another object type
	if (res != JVX_NO_ERROR)
	{
		for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp.tp)
			{
				cnt++;
			}
		}

		if (num)
		{
			*num = cnt;
		}
		res = JVX_NO_ERROR;
	}
	return(res);
}

jvxErrorType
CjvxHost::identification_tool(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* description, jvxApiString* descriptor, jvxBool* multipleInstances)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	int numRet = 0;
	jvxSize i;
	bool isStandardC = false;
	jvxSize idSelect = JVX_SIZE_UNSELECTED;

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (
			(_common_set_types.registeredTechnologyTypes[i].selector[0] == tp.tp) ||
			(_common_set_types.registeredTechnologyTypes[i].selector[1] == tp.tp))

		{
			if (idx < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size())
			{
				res = JVX_NO_ERROR;
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[idx].theHandle_single->description(description);
				_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[idx].theHandle_single->descriptor(descriptor);

				if (multipleInstances)
				{
					*multipleInstances = _common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[idx].common.allowsMultiObjects;
				}
			}
			return res;
		}
	}


	for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
	{
		if (_common_set_types.registeredNodeTypes[i].selector[0] == tp.tp)
		{
			if (idx < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size())
			{
				res = JVX_NO_ERROR;
				_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[idx].theHandle_single->description(description);
				_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[idx].theHandle_single->descriptor(descriptor);

				if (multipleInstances)
				{
					*multipleInstances = _common_set_types.registeredNodeTypes[i].instances.availableEndpoints[idx].common.allowsMultiObjects;
				}
			}
			return res;
		}
	}

	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp.tp)
		{
			if (numRet == idx)
			{
				res = JVX_NO_ERROR;
				this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->description(description);
				this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->descriptor(descriptor);
				break;
			}
			numRet++;
		}
	}
	return(res);
}

/*
 * Options to call this function for NODES and simple objects :
 * 1) Specify a valid slotid : return reference of the associated element
 * 2) Specify slotid JVX_SIZE_UNSELECTED :
 * Return the single offside object
 * 3) Specify slotid JVX_SIZE_DONTCARE :
 * Search the currently selected objects of the type to find among those objects that have the right state(filter_stateMask)
 * -filter_id = 0 : Description matches the string in filter_token
 * -filter_id = 1 : Descriptor matches the string in filter_token
 * -otherwise : Module name matches the string in filter_token
 * */
jvxErrorType
CjvxHost::reference_tool(const jvxComponentIdentification& tp,
	IjvxObject** theObject, jvxSize filter_id,
	const char* filter_descriptor,
	jvxBitField filter_stateMask,
	IjvxReferenceSelector* decider)
{
	jvxSize i = 0;
	jvxSize cnt = 0;
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	jvxApiString fldStr;
	jvxState stat = JVX_STATE_NONE;
	jvxSize idxSelect = JVX_SIZE_UNSELECTED;
	jvxBool foundit = false;
	jvxBool isDevice = false;
	std::string txt;

	// If there is a filter, deactivate id return
	if (filter_descriptor)
	{
		filter_id = JVX_SIZE_UNSELECTED;
	}

	// =====================================================================================
	// Find in technologies/devices
	// =====================================================================================

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp.tp)
		{
			idxSelect = (jvxInt32)i;
			break;
		}

		if (_common_set_types.registeredTechnologyTypes[i].selector[1] == tp.tp)
		{
			idxSelect = (jvxInt32)i;
			isDevice = true;
			break;
		}
	}

	if (JVX_CHECK_SIZE_SELECTED(idxSelect))
	{
		if (isDevice)
		{
			// There must be an active technology...
			if (tp.slotid < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech.size())
			{
				if (_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					jvxSize numDevices = 0;
					_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->number_devices(&numDevices);

					if (filter_stateMask == JVX_STATE_DONTCARE)
					{
						if (filter_descriptor)
						{
							for (i = 0; i < numDevices; i++)
							{
								txt = "Unknown";
								_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->descriptor_device(i, &fldStr);

								txt = fldStr.std_str();

								if (jvx_compareStringsWildcard(filter_descriptor, txt))
								{
									filter_id = i;
									break;
								}
							}
						}

						if (filter_id < numDevices)
						{
							if (theObject)
							{
								IjvxDevice* theDevice = NULL;
								_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->request_device(filter_id, &theDevice);
								if (theDevice)
								{
									*theObject = static_cast<IjvxObject*>(theDevice);
									res = JVX_NO_ERROR;
								}
								else
								{
									res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
								}
							}
							else
							{
								res = JVX_NO_ERROR;
							}
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;
						}
					}
					else
					{
						cnt = 0;
						res = JVX_ERROR_ELEMENT_NOT_FOUND;

						for (i = 0; i < numDevices; i++)
						{
							if (theObject)
							{
								IjvxDevice* theDevice = NULL;
								_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->request_device(i, &theDevice);
								if (theDevice)
								{
									jvxState stat = JVX_STATE_NONE;
									theDevice->state(&stat);
									if (JVX_EVALUATE_BITFIELD(stat & filter_stateMask))
									{
										if (filter_descriptor)
										{
											txt = "Unknown";
											theDevice->descriptor(&fldStr);
											txt = fldStr.std_str();

											if (jvx_compareStringsWildcard(filter_descriptor, txt))
											{
												*theObject = static_cast<IjvxObject*>(theDevice);
												res = JVX_NO_ERROR;
												break;
											}
										}
										else
										{
											if (cnt == filter_id)
											{
												*theObject = static_cast<IjvxObject*>(theDevice);
												res = JVX_NO_ERROR;
												break;
											}
											else
											{
												cnt++;
											}
										}
									}
								}
								else
								{
									res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
									break;
								}
							}
							else
							{
								res = JVX_NO_ERROR;
							}
						}
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		} // if (isDevice)
		else
		{
			if (tp.slotid == JVX_SIZE_UNSELECTED)
			{
				if (filter_stateMask == JVX_STATE_DONTCARE)
				{
					if (filter_descriptor)
					{
						for (i = 0; i < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies.size(); i++)
						{
							txt = "Unknown";
							_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single->descriptor(&fldStr);

							txt = fldStr.std_str();

							if (jvx_compareStringsWildcard(filter_descriptor, txt))
							{
								filter_id = i;
								break;
							}
						}
					}


					if (filter_id < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies.size())
					{
						if (theObject)
						{
							*theObject = _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[filter_id].theHandle_single;
						}
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
					cnt = 0;
					for (i = 0; i < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies.size(); i++)
					{
						assert(_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single);
						stat = JVX_STATE_NONE;
						_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single->state(&stat);
						if (JVX_EVALUATE_BITFIELD(stat & filter_stateMask))
						{
							if (filter_descriptor)
							{
								txt = "Unknown";
								_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single->descriptor(&fldStr);
								txt = fldStr.std_str();

								if (jvx_compareStringsWildcard(filter_descriptor, txt))
								{
									filter_id = i;
									break;
								}
							}
							else
							{
								if (filter_id == cnt)
								{
									if (theObject)
									{
										*theObject = _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single;
									}
									res = JVX_NO_ERROR;
									break;
								}
								else
								{
									cnt++;
								}
							}
						}
					}
				}
			}
			else
			{
				if (tp.slotid < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech.size())
				{
					if (theObject)
					{
						*theObject = _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech;
					}
					res = JVX_NO_ERROR;
				}
			}
		}
		return(res);
	}

	// =====================================================================================
	// Find in nodes
	// =====================================================================================

	// New template based implementation
	res = t_reference_tool<IjvxNode>(_common_set_types.registeredNodeTypes,
		tp, theObject, filter_id, filter_descriptor, filter_stateMask, decider);

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		res = t_reference_tool<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes,
			tp, theObject, filter_id, filter_descriptor, filter_stateMask, decider);
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		// =====================================================================================
		// Find in all other components
		// =====================================================================================

		cnt = 0;
		for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp.tp)
			{
				if (filter_descriptor)
				{
					txt = "Unknown";
					this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->descriptor(&fldStr);

					txt = fldStr.std_str();

					if (jvx_compareStringsWildcard(filter_descriptor, txt))
					{
						if (theObject)
						{
							*theObject = this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single;
							this->_common_set_host.otherComponents.availableOtherComponents[i].refCount++;
						}
						res = JVX_NO_ERROR;
						break;
					}
				}
				else
				{
					if (cnt == filter_id)
					{
						if (theObject)
						{
							*theObject = this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single;
							this->_common_set_host.otherComponents.availableOtherComponents[i].refCount++;
						}
						res = JVX_NO_ERROR;
						break;
					}
					cnt++;
				}
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxHost::return_reference_tool(const jvxComponentIdentification& tp, IjvxObject* theObject)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize i;
	int numRet = 0;
	jvxApiString fldStr;

	jvxInt32 idxSelect = -1;
	jvxBool isDevice = false;

	if (theObject == NULL)
	{
		return(JVX_NO_ERROR);
	}

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp.tp)
		{
			idxSelect = (jvxInt32)i;
			break;
		}
		if (_common_set_types.registeredTechnologyTypes[i].selector[1] == tp.tp)

		{
			idxSelect = (jvxInt32)i;
			isDevice = true;
			break;
		}
	}

	if (idxSelect >= 0)
	{
		if (isDevice)
		{
			if (tp.slotid < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech.size())
			{
				// There must be an active technology...
				if (_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
				{
					jvxSize numDevices = 0;
					_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->number_devices(&numDevices);

					res = JVX_ERROR_INVALID_ARGUMENT;
					for (i = 0; i < numDevices; i++)
					{
						IjvxDevice* theDevice = NULL;
						_common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech->request_device(i, &theDevice);
						if (theObject == static_cast<IjvxObject*>(theDevice))
						{
							res = JVX_NO_ERROR;
							break;
						}
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			if (tp.slotid == JVX_SIZE_UNSELECTED)
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
				for (i = 0; i < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies.size(); i++)
				{
					if (theObject == _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.availableTechnologies[i].theHandle_single)
					{
						res = JVX_NO_ERROR;
						break;
					}
				}
			}
			else
			{
				if (tp.slotid < _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech.size())
				{
					if (theObject == _common_set_types.registeredTechnologyTypes[idxSelect].technologyInstances.selTech[tp.slotid].theHandle_shortcut_tech)
					{
						res = JVX_NO_ERROR;
					}
				}
			}
		}
		return(res);
	}

	// For the nodes we only need the type since the object already indicates which instance to release
	res = t_return_reference_tool<IjvxNode>(_common_set_types.registeredNodeTypes,
		tp.tp, theObject);
	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		res = t_return_reference_tool<IjvxSimpleComponent>(_common_set_types.registeredSimpleTypes,
			tp.tp, theObject);
	}

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp.tp)
			{
				if (theObject == this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single)
				{
					this->_common_set_host.otherComponents.availableOtherComponents[i].refCount--;
					res = JVX_NO_ERROR;
					break;
				}
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxHost::instance_tool(jvxComponentType tp, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	int numRet = 0;
	jvxApiString fldStr;
	std::string cmpStr;
	jvxSize i, j;
	std::string txt;

	// If there is a filter, deactivate id return
	if (filter_descriptor)
	{
		filter_id = JVX_SIZE_UNSELECTED;
	}

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp)
		{
			// Check if we allow return of a technology instance
			if (JVX_CHECK_SIZE_UNSELECTED(filter_id))
			{
				if (filter_descriptor)
				{
					for (j = 0; j < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size(); i++)
					{
						this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].theHandle_single->descriptor(&fldStr);
						cmpStr = fldStr.std_str();
						if (cmpStr == (std::string)filter_descriptor)
						{
							filter_id = j;
							break;
						}
					}
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(filter_id))
			{
				if (filter_id < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size())
				{
					if (this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[filter_id].common.allowsMultiObjects)
					{
						res = this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[filter_id].common.linkage.funcInit(
							theObject, NULL, this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].theHandle_single);
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_SETTING;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			return res;
		}
		else if (_common_set_types.registeredTechnologyTypes[i].selector[1] == tp)
		{
			res = JVX_ERROR_UNSUPPORTED;
			return res;
		}
	}

	for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
	{
		if (_common_set_types.registeredNodeTypes[i].selector[0] == tp)
		{
			// Check if we allow return of a node instance
			if (JVX_CHECK_SIZE_UNSELECTED(filter_id))
			{
				if (filter_descriptor)
				{
					for (j = 0; j < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size(); j++)
					{
						this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[j].theHandle_single->descriptor(&fldStr);
						cmpStr = fldStr.std_str();
						if (cmpStr == (std::string)filter_descriptor)
						{
							filter_id = j;
						}
					}
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(filter_id))
			{
				if (filter_id < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size())
				{
					if (this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].common.allowsMultiObjects)
					{
						res = this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].common.linkage.funcInit(
							theObject, NULL, this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].theHandle_single);
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_SETTING;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			return res;
		}
	}

	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp)
		{
			if (filter_descriptor)
			{
				txt = "Unknown";
				if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.allowsMultiObjects)
				{
					if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit && this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm)
					{
						IjvxObject* locObject = NULL;
						res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit(&locObject,
							NULL, this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single);
						if (res == JVX_NO_ERROR)
						{
							locObject->descriptor(&fldStr);
							txt = fldStr.std_str();

							if (txt == filter_descriptor)
							{
								if (theObject)
								{
									*theObject = locObject;
									this->_common_set_host.otherComponents.availableOtherComponents[i].leased.push_back(*theObject);
								}
								res = JVX_NO_ERROR;
								break;
							}
							else
							{
								res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm(locObject);
								res = JVX_ERROR_ID_OUT_OF_BOUNDS;// reset to original error case
							}
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;// reset to original error case
							if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.isExternalComponent)
							{
								std::cout << "Warning: Init function failed for object " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.externalLink.description << "." << std::endl;

							}
							else
							{
								std::cout << "Warning: Init function failed for object from dynamic library " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.dllPath << "." << std::endl;
							}
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;// reset to original error case
						if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.isExternalComponent)
						{
							std::cout << "Warning: Init function failed for object " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.externalLink.description << "." << std::endl;

						}
						else
						{
							std::cout << "Warning: Init function failed for object  from dynamic library " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.dllPath << "." << std::endl;
						}
					}
				}
				else
				{
					// Object does not allow multiple allocation, this will not be correct
				}
			}
			else
			{
				if (numRet == filter_id)
				{
					if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.allowsMultiObjects)
					{
						if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit && this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm)
						{
							if (theObject)
							{
								res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit(theObject,
									NULL, this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single);
								if (res == JVX_NO_ERROR)
								{
									this->_common_set_host.otherComponents.availableOtherComponents[i].leased.push_back(*theObject);
								}
								else
								{
									res = JVX_ERROR_INTERNAL;
								}
							}
							else
							{
								res = JVX_ERROR_INVALID_ARGUMENT;
							}
						}
						else
						{
							res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
						}
					}
					else
					{
						res = JVX_ERROR_UNSUPPORTED;
					}
					break;
				}
				numRet++;
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxHost::return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	int numRet = 0;
	jvxApiString fldStr;
	std::string cmpStr;
	jvxSize i, j;
	bool isStandardC = false;

	// If there is a filter, deactivate id return
	if (filter_descriptor)
	{
		filter_id = JVX_SIZE_UNSELECTED;
	}

	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp)
		{
			if (_common_set_types.registeredTechnologyTypes[i].selector[0] == tp)
			{
				// Check if we allow return of a technology instance
				if (JVX_CHECK_SIZE_UNSELECTED(filter_id))
				{
					if (filter_descriptor)
					{
						for (j = 0; j < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size(); i++)
						{
							this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[j].theHandle_single->descriptor(&fldStr);
							cmpStr = fldStr.std_str();
							if (cmpStr == (std::string)filter_descriptor)
							{
								filter_id = j;
								break;
							}
						}
					}
				}
				if (JVX_CHECK_SIZE_SELECTED(filter_id))
				{
					if (filter_id < this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies.size())
					{
						if (this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[filter_id].common.allowsMultiObjects)
						{
							res = this->_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies[filter_id].common.linkage.funcTerm(theObject);
							res = JVX_NO_ERROR;
						}
						else
						{
							res = JVX_ERROR_INVALID_SETTING;
						}
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
				}
				else
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
				return res;
			}
			else if (_common_set_types.registeredTechnologyTypes[i].selector[1] == tp)
			{
				res = JVX_ERROR_UNSUPPORTED;
				return res;
			}
		}
	}


	for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
	{
		if (_common_set_types.registeredNodeTypes[i].selector[0] == tp)
		{
			// Check if we allow return of a node instance
			if (JVX_CHECK_SIZE_UNSELECTED(filter_id))
			{
				if (filter_descriptor)
				{
					for (j = 0; j < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size(); j++)
					{
						this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[j].theHandle_single->descriptor(&fldStr);
						cmpStr = fldStr.std_str();

						if (cmpStr == (std::string)filter_descriptor)
						{
							filter_id = j;
							break;
						}
					}
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(filter_id))
			{
				if (filter_id < this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints.size())
				{
					if (this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].common.allowsMultiObjects)
					{
						res = this->_common_set_types.registeredNodeTypes[i].instances.availableEndpoints[filter_id].common.linkage.funcTerm(theObject);
						res = JVX_NO_ERROR;
					}
					else
					{
						res = JVX_ERROR_INVALID_SETTING;
					}
				}
				else
				{
					res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			return res;
		}
	}

	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp)
		{
			if (numRet == filter_id)
			{
				if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.allowsMultiObjects)
				{
					std::vector<IjvxObject*>::iterator elm = this->_common_set_host.otherComponents.availableOtherComponents[i].leased.begin();
					for (; elm != this->_common_set_host.otherComponents.availableOtherComponents[i].leased.end(); elm++)
					{
						if (*elm == theObject)
						{
							break;
						}
					}
					if (elm != this->_common_set_host.otherComponents.availableOtherComponents[i].leased.end())
					{
						assert(this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm);

						res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm(theObject);
						if (res != JVX_NO_ERROR)
						{
							res = JVX_ERROR_INTERNAL;
						}
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
				else
				{
					res = JVX_ERROR_UNSUPPORTED;
				}
				break;
			}
			numRet++;
		}
	}
	return(res);
}



