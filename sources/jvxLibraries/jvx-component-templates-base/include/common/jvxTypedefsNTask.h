#ifndef _JVXTYPEDEFSNTASK_H__
#define _JVXTYPEDEFSNTASK_H__

struct jvxOneConnectorDefine
	{
		jvxSize idCon;
		std::string nmConnector;
		IjvxConnectionMaster* assocMaster;

		jvxOneConnectorDefine()
		{
			idCon = JVX_SIZE_UNSELECTED;
			assocMaster = NULL;
		}
	};

	class CjvxInputConnectorNtask;
	class CjvxOutputConnectorNtask;

	class jvxOneConnectorTask
	{
	public:
		jvxOneConnectorDefine inputConnector;
		jvxOneConnectorDefine outputConnector;
		CjvxInputConnectorNtask* icon;
		CjvxOutputConnectorNtask* ocon;

		jvxOneConnectorTask()
		{
			icon = NULL;
			ocon = NULL;
		};
	} ;

	
#endif
	