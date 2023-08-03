#ifndef __CJVXINTERFACEHOSTTPL_H__
#define __CJVXINTERFACEHOSTTPL_H__

#include "jvx.h"
#include "jvxFactoryHosts/CjvxInterfaceFactory.h"

// Template "H" represents the host type, template "T" represents the hostinteraction specialization
// COnstraint: T must be derived from CjvxHostInteraction!!
template <class H, class T>
class CjvxInterfaceHostTpl : public CjvxInterfaceFactory<H>, 
	public IjvxConfiguration, public IjvxConfigurationExtender, public T
{
public:
	JVX_CALLINGCONVENTION CjvxInterfaceHostTpl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxInterfaceFactory<H>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{

	};
	virtual JVX_CALLINGCONVENTION ~CjvxInterfaceHostTpl()
	{

	};

	// ===================================================================================================
	// Interface <IjvxHiddenInterface>
	// ===================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = CjvxHostInteraction::request_hidden_interface(tp, hdl); // <- this is not T since we forward this to the registered additional interfaces!
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
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = CjvxHostInteraction::return_hidden_interface(tp, hdl); // <- this is not T since we forward this to the registered additional interfaces!
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
	};
};

template <class H, class T>
class CjvxInterfaceHostTplConnections : public CjvxInterfaceHostTpl<H, T>,
	public IjvxDataConnections, public CjvxDataConnections
{
public:
	JVX_CALLINGCONVENTION CjvxInterfaceHostTplConnections(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxInterfaceHostTpl<H, T>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{

	};
	virtual JVX_CALLINGCONVENTION ~CjvxInterfaceHostTplConnections()
	{

	};

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		
			switch (tp)
			{
			case JVX_INTERFACE_DATA_CONNECTIONS:
				if (hdl)
				{
					*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxDataConnections*>(this));
				}
				break;
			
			default:

				res = CjvxInterfaceHostTpl<H, T>::request_hidden_interface(tp, hdl);
			
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		
			switch (tp)
			{
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
				res = CjvxInterfaceHostTpl<H, T>::return_hidden_interface(tp, hdl);
			
		}
		return res;
	};
#include "codeFragments/simplify/jvxDataConnections_simplify.h"
};
#endif
