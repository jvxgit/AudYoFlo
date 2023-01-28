#include "CjvxBareNodeNio.h"

CjvxBareNodeNio::CjvxBareNodeNio(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxNodeBaseNio(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

CjvxBareNodeNio::~CjvxBareNodeNio()
{
}

jvxErrorType 
CjvxBareNodeNio::activate()
{
	jvxSize i;
	jvxSize jji, jjo;
	jvxErrorType res = CjvxNodeBaseNio::activate();
	if (res == JVX_NO_ERROR)
	{

		for (i = 0; i < _common_set_node_nio.fixedTasks.size(); i++)
		{
			std::vector< CjvxInputConnectorNio*> icons;
			std::vector< CjvxOutputConnectorNio*> ocons;

			for (jji = 0; jji < _common_set_node_nio.fixedTasks[i].inputConnectors.size(); jji++)
			{
				CjvxInputConnectorNio* theNewIcon = new CjvxInputConnectorNio(
					_common_set_node_nio.fixedTasks[i].inputConnectors[jji].idCon,
					_common_set_node_nio.fixedTasks[i].inputConnectors[jji].nmConnector.c_str(),
					static_cast<IjvxConnectorFactory*>(this),
					static_cast<CjvxCommonNio_report*>(this),
					_common_set_node_nio.fixedTasks[i].inputConnectors[jji].assocMaster);
				icons.push_back(theNewIcon);
			}

			for (jjo = 0; jjo < _common_set_node_nio.fixedTasks[i].outputConnectors.size(); jjo++)
			{
				CjvxOutputConnectorNio* theNewOcon = new CjvxOutputConnectorNio(
					_common_set_node_nio.fixedTasks[i].outputConnectors[jjo].idCon,
					_common_set_node_nio.fixedTasks[i].outputConnectors[jjo].nmConnector.c_str(),
					static_cast<IjvxConnectorFactory*>(this),
					static_cast<CjvxCommonNio_report*>(this),
					_common_set_node_nio.fixedTasks[i].outputConnectors[jjo].assocMaster);
				ocons.push_back(theNewOcon);
			}

			for (jji = 0; jji < icons.size(); jji++)
			{
				for (jjo = 0; jjo < _common_set_node_nio.fixedTasks[i].outputConnectors.size(); jjo++)
				{
					icons[jji]->add_reference(ocons[jjo]);
				}
			}
			for (jjo = 0; jjo < icons.size(); jjo++)
			{
				for (jji = 0; jji < _common_set_node_nio.fixedTasks[i].outputConnectors.size(); jji++)
				{
					ocons[jjo]->add_reference(icons[jjo]);
				}
			}

			for (jji = 0; jji < icons.size(); jji++)
			{
				oneInputConnectorElement newElmIn;
				newElmIn.theConnector = icons[jji];
				newElmIn.refCnt = 0;
				newElmIn.uId = icons[jji]->_common_set_comnio.ctxtId;
				_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn;
			}
			for (jjo = 0; jjo < ocons.size(); jjo++)
			{
				oneOutputConnectorElement newElmOut;
				newElmOut.theConnector = ocons[jjo];
				newElmOut.refCnt = 0;
				newElmOut.uId = icons[jjo]->_common_set_comnio.ctxtId;
				_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut;
			}
		}
		res = CjvxConnectorFactory::_activate_factory(static_cast<IjvxObject*>(this));

		if (res != JVX_NO_ERROR)
		{
			CjvxNodeBaseNio::deactivate();
		}
	}
	return res;
}

jvxErrorType 
CjvxBareNodeNio::deactivate()
{
	jvxErrorType res = CjvxNodeBaseNio::deactivate();
	if (res == JVX_NO_ERROR)
	{
		res = CjvxConnectorFactory::_deactivate_factory();
		_common_set_conn_factory.input_connectors.clear();
		_common_set_conn_factory.output_connectors.clear();
	}
	return res;

}