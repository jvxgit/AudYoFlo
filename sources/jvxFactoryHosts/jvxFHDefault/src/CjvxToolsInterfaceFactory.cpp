#include "CjvxToolsInterfaceFactory.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxToolsInterfaceFactory::CjvxToolsInterfaceFactory(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxInterfaceFactory<IjvxFactoryHost>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	myUniqueId = 1;
#ifdef JVX_MINIMUM_HOST_FUNCTIONALITY
	config.minHostFunctionality = true;
#endif

#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
	config_dll.use_only_static_objects = true;
#else
	config_dll.use_only_static_objects = false;
#endif

}

CjvxToolsInterfaceFactory::~CjvxToolsInterfaceFactory()
{
}

// ==========================================================================================
// ==========================================================================================

/*
 * Initialize the current component
 */
jvxErrorType
CjvxToolsInterfaceFactory::initialize(IjvxHiddenInterface* theOtherhost)
{
	jvxErrorType res = _initialize(theOtherhost);
	if(res == JVX_NO_ERROR)
	{
		_common_set_host.threadId_registered = JVX_GET_CURRENT_THREAD_ID();

		reset();
	}
	return(res);
}

jvxErrorType
CjvxToolsInterfaceFactory::select(IjvxObject* theOwner)
{
	jvxErrorType res = _select(theOwner);
	if(res == JVX_NO_ERROR)
	{
		this->init__properties_selected();
		this->allocate__properties_selected();
		this->register__properties_selected(static_cast<CjvxProperties*>(this));
	}
	return(res);
}

jvxErrorType
CjvxToolsInterfaceFactory::system_ready()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxToolsInterfaceFactory::system_about_to_shutdown()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxToolsInterfaceFactory::owner(IjvxObject** dependsOn) 
{
	return _owner(dependsOn);
};

/*
 * Activate host component
 */
jvxErrorType
CjvxToolsInterfaceFactory::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;

	_common_set_min.theHostRef = static_cast<IjvxHiddenInterface*>(this);
	res = _activate_no_text_log();
	if(res == JVX_NO_ERROR)
	{
		_lock_properties_local();
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.component_path);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.configure_parts);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.do_unload_dlls);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.host_output_cout);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.host_verbose_dll);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_activate);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_dbglevel);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_expressions);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_filename);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile);
		_unlock_properties_local();

		if (!config_dll.use_only_static_objects)
		{
			this->loadAllComponents(JVX_EVALUATE_BITFIELD(CjvxHost_genpcg::properties_selected.do_unload_dlls.value.selection() & 0x1),
				CjvxHost_genpcg::properties_selected.component_path.value, true, true);
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

/*
 * Deactivate host component
 */
jvxErrorType
CjvxToolsInterfaceFactory::deactivate()
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{		
		jvxState stat = JVX_STATE_NONE;
		for(i = 0; i < _common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->state(&stat);
			assert(stat == JVX_STATE_NONE);
		}

		if (jvxrtst_bkp.theTextLogger_hdl)
		{
			jvxrtst_bkp.theTextLogger_hdl->stop();
			jvxrtst_bkp.theTextLogger_hdl->terminate();

			_common_set.theToolsHost->return_reference_tool(jvxComponentIdentification(JVX_COMPONENT_SYSTEM_TEXT_LOG, 
				JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED), jvxrtst_bkp.theTextLogger_obj);
			jvxrtst_bkp.theTextLogger_obj = NULL;
			jvxrtst_bkp.theTextLogger_hdl = NULL;
		}

		_lock_properties_local();
		_undo_update_property_access_type( CjvxHost_genpcg::properties_selected.component_path);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.configure_parts);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.do_unload_dlls);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.host_output_cout);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.host_verbose_dll);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_activate);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_dbglevel);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_expressions);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_filename);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile);
		_unlock_properties_local();

		res = _deactivate_no_text_log();
		assert(res == JVX_NO_ERROR);

		// unload all Dlls
		if (!config_dll.use_only_static_objects)
		{
			this->unloadAllComponents();
		}
		_common_set_min.theHostRef = NULL;
	}

	return(res);
}

jvxErrorType
CjvxToolsInterfaceFactory::unselect()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_SELECTED)
	{
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
CjvxToolsInterfaceFactory::terminate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _terminate();
	return(res);
}

// ==========================================================================================
// ==========================================================================================

jvxErrorType
CjvxToolsInterfaceFactory::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxHostInteraction::request_hidden_interface(tp, hdl);
	if(res != JVX_NO_ERROR)
	{
		res = JVX_NO_ERROR;
		switch(tp)
		{
		case JVX_INTERFACE_PROPERTIES:
			if(hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this));
			}
			break;
		case JVX_INTERFACE_TOOLS_HOST:
			if(hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxToolsHost*>(this));
			}
			break;
		case JVX_INTERFACE_UNIQUE_ID:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxUniqueId*>(this));
			}
			break;
		case JVX_INTERFACE_CONFIGURATION:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this));
			}
			break;
		case JVX_INTERFACE_CONFIGURATION_EXTENDER:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationExtender*>(this));
			}
			break;
		default:

			res = _request_hidden_interface(tp, hdl);
		}
	}
	return(res);
}

jvxErrorType
CjvxToolsInterfaceFactory::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxHostInteraction::return_hidden_interface(tp, hdl);
	if(res != JVX_NO_ERROR)
	{
		res = JVX_NO_ERROR;
		switch(tp)
		{
		case JVX_INTERFACE_PROPERTIES:
			if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_TOOLS_HOST:
			if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxToolsHost*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_UNIQUE_ID:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxUniqueId*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_CONFIGURATION:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_CONFIGURATION_EXTENDER:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationExtender*>(this)))
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

// ==========================================================================================
// ==========================================================================================

jvxErrorType
CjvxToolsInterfaceFactory::number_tools(const jvxComponentIdentification& tp, jvxSize* num)
{
	jvxErrorType res = JVX_NO_ERROR;
	int numRet = 0;
	jvxSize i;

	if (num)
	{
		*num = 0;
	}

	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp)
		{
			numRet++;
		}
	}
	if (num)
	{
		*num = numRet;
	}
	return(res);
}

jvxErrorType
CjvxToolsInterfaceFactory::identification_tool(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* description, jvxApiString* descriptor, jvxBool* mulInst)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	int numRet = 0;
	jvxApiString fldStr;
	jvxSize i;
	std::string txt = "Unknown";
	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp)
		{
			if (numRet == idx)
			{
				txt = "Unknown";
				if (description)
				{
					this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->description(description);
				}

				txt = "Unknown";
				if (descriptor)
				{
					this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->descriptor(descriptor);
				}

				if (mulInst)
				{
					*mulInst = this->_common_set_host.otherComponents.availableOtherComponents[i].common.allowsMultiObjects;
				}
				break;
			}
			numRet++;
		}
	}
	return(res);
}

jvxErrorType
CjvxToolsInterfaceFactory::reference_tool(const jvxComponentIdentification& tp, IjvxObject** theObject, 
	jvxSize filter_id, const char* filter_descriptor, jvxBitField filter_stateMask,
	IjvxReferenceSelector* decider)
{
	jvxSize i = 0;
	jvxSize cnt = 0;
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	jvxApiString fldStr ;
	jvxState stat = JVX_STATE_NONE;
	std::string txt;

	// If there is a filter, deactivate id return
	if (filter_descriptor)
	{
		filter_id = JVX_SIZE_UNSELECTED;
	}

	// =====================================================================================
	// Find in all other components
	// =====================================================================================

	cnt = 0;
	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp)
		{
			if (filter_descriptor)
			{
				txt = "Unknown";
				this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->descriptor(&fldStr);
				txt = fldStr.std_str();

				if (txt == filter_descriptor)
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
	return(res);
}

jvxErrorType
CjvxToolsInterfaceFactory::return_reference_tool(const jvxComponentIdentification& tp, IjvxObject* theObject)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize i;
	int numRet = 0;

	if (theObject == NULL)
	{
		return(JVX_NO_ERROR);
	}

	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp)
		{
			if (theObject == this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single)
			{
				this->_common_set_host.otherComponents.availableOtherComponents[i].refCount--;
				res = JVX_NO_ERROR;
				break;
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxToolsInterfaceFactory::instance_tool(jvxComponentType tp, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	int numRet = 0;
	jvxApiString fldStr;
	jvxSize i;
	std::string txt;
	bool isStandardC = false;

	for (i = 0; i < this->_common_set_host.otherComponents.availableOtherComponents.size(); i++)
	{
		if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.tp == tp)
		{
			if (filter_descriptor)
			{
				txt = "Unknown";
				if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.allowsMultiObjects)
				{
					if (this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit && 
						this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcTerm)
					{
						IjvxObject* locObject = NULL;
						res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit(&locObject, NULL,
							this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single);
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
#ifndef JVX_HOST_USE_ONLY_STATIC_OBJECTS
								std::cout << "Warning: Init function failed for object " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.dllPath << "." << std::endl;
#endif
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
#ifndef JVX_HOST_USE_ONLY_STATIC_OBJECTS
							std::cout << "Warning: Init function failed for object " << this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.dllPath << "." << std::endl;
#endif
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
								res = this->_common_set_host.otherComponents.availableOtherComponents[i].common.linkage.funcInit(theObject, NULL,
									this->_common_set_host.otherComponents.availableOtherComponents[i].theObj_single);
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
CjvxToolsInterfaceFactory::return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	int numRet = 0;
	jvxSize i;

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

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
