#ifndef __JVXTYPEDEFSMASTERTASK_H__
#define __JVXTYPEDEFSMASTERTASK_H__

struct jvxOneMasterTask
	{
		std::string nmMaster;
		std::string nmConnector;
		jvxSize idCntxt;
		IjvxConnectionMaster* theMaster;
	public:
		jvxOneMasterTask()
		{
			idCntxt = JVX_SIZE_UNSELECTED;
			theMaster = NULL;
		};
	} ;
	
	typedef struct
	{
		JVX_MUTEX_HANDLE mut_hookup;
		jvxBitField flags_hookup;

		std::vector<jvxOneMasterTask> fixedMaster;
		// std::vector<jvxOneVariableConnectorTaskDefinition> variableTasksDefines;
	} jvxOneMasterCollection;
	
#endif