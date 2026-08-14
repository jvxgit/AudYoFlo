#ifndef __JVXTYPEDEFSVTASL_H__
#define __JVXTYPEDEFSVTASL_H__

#include "common/CjvxNegotiate.h"
	class CjvxInputConnectorVtask;
	class CjvxOutputConnectorVtask;
	JVX_INTERFACE IjvxInputOutputConnectorVtask;

	/*
	class CjvxInputConnectorVtask;
	class CjvxOutputConnectorVtask;

	template <class t_con> class jvxOneConnectorTaskRuntime_con
	{
	public:

		t_con* con;
		jvxState stat;
		jvxOneConnectorTaskRuntime_con()
		{
			con = NULL;
			stat = JVX_STATE_NONE;
		};
	};
	*/

	template <class T> class oneConnectorVTask_con
	{
	public:
		T* con;

		oneConnectorVTask_con()
		{
			con = NULL;
		};
	};

	template <class t_con, class t_neg> class oneConnectorVTask_use: public oneConnectorVTask_con<t_con>
	{
	public:

		jvxState stat;
		jvxLinkDataDescriptor_con_params params;
		t_neg neg;
		std::string ctxt_tag;
		jvxComponentIdentification ctxt_cpTp;
		jvxBool trigger_only_mode = false;
		oneConnectorVTask_use()
		{			
			stat = JVX_STATE_NONE;
		};
	};

	class jvxOneConnectorTaskRuntime
	{
	public:

		jvxSize idCtxtGroup = JVX_SIZE_UNSELECTED;
		jvxSize idCtxt = JVX_SIZE_UNSELECTED;

		std::vector<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>> icons;
		std::vector < oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>> ocons;

		jvxBitField iconsSelected;
		jvxBitField oconsSelected;

		IjvxConnectionMaster* associatedMaster = nullptr;
		IjvxDataConnectionCommon* associatedCommon = nullptr;

		jvxOneConnectorTaskRuntime()
		{
			jvx_bitFClear(iconsSelected);
			jvx_bitFClear(oconsSelected);
		};
	};	
	
#endif
	