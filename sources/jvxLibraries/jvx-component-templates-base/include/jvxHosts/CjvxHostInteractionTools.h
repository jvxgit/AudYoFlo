#ifndef __CJVXHOSTINTERACTIONTOOLS_H__

#include "jvx.h"
#include "jvxHosts/CjvxHostInteraction.h"

template <class T>
class CjvxHostInteractionTools : public T
{
public:
	CjvxHostInteractionTools(): T()
	{
	};
	
	~CjvxHostInteractionTools()
	{
	};
	
	virtual jvxErrorType _reference_tool(const jvxComponentIdentification& tp, IjvxObject** theObject,
			jvxSize filter_id, const char* filter_descriptor, jvxBitField filter_stateMask,
			IjvxReferenceSelector* decider) 
	{
		jvxSize i = 0;
		jvxSize cnt = 0;
		jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		jvxApiString fldStr;
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
		return(res);
	}

	virtual jvxErrorType _return_reference_tool(const jvxComponentIdentification& tp, IjvxObject* theObject)
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
		return(res);
	}

	virtual jvxErrorType _instance_tool(jvxComponentType tp, IjvxObject** theObject, jvxSize filter_id,
		const char* filter_descriptor)
	{
		jvxSize i;
		std::string txt;
		jvxApiString fldStr;
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		int numRet = 0;
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
		return res;
	}

	virtual jvxErrorType _return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor)
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
		return res;
	}

	virtual jvxErrorType _number_tools(const jvxComponentIdentification& tp, jvxSize* num)
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
	virtual jvxErrorType _identification_tool(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* description, jvxApiString* descriptor, jvxBool* mulInst) 

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

};

#endif
