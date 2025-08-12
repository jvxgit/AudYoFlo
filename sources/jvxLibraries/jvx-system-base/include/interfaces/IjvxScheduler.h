#ifndef __IJVXSCHEDULERLANE_H__
#define __IJVXSCHEDULERLANE_H__

typedef enum
{
	JVX_SCHEDULER_ITERATOR_TYPE_NONE = 0,
	JVX_SCHEDULER_ITERATOR_TYPE_SLAVE = 1,
	JVX_SCHEDULER_ITERATOR_TYPE_MASTER = 2
} jvxSchedulerConnectionIterator_refobjecttype;

typedef enum
{
	JVX_SCHEDULER_ITERATOR_CONNECTION_NONE = 0,
	JVX_SCHEDULER_ITERATOR_CONNECTION_INPUT = 1,
	JVX_SCHEDULER_ITERATOR_CONNECTION_OUTPUT = 2
} jvxSchedulerConnectionIterator_connectortype;

typedef enum
{
	JVX_SCHEDULER_GROUP_NONE = 0,
	JVX_SCHEDULER_GROUP_NORMAL = 1,
	JVX_SCHEDULER_GROUP_PIPELINED = 2
} jvxSchedulerGroupType;

	
// ===============================================================================================



#if 0
// ================================================================================================
// ================================================================================================

JVX_INTERFACE IjvxSchedulerConnectionIterator
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxSchedulerConnectionIterator(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION number_active_connections(jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION active_connection(jvxSize idx, IjvxSchedulerConnectionIterator* connected) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_equivalent_connection(jvxHandle** elm_on_return, jvxSchedulerConnectionIterator_refobjecttype* , jvxSchedulerConnectionIterator_connectortype* ) = 0;
};

JVX_INTERFACE IjvxSchedulerLinkNet: public IjvxStateMachine
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxSchedulerLinkNet(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION associate_slave(IjvxLinkDataSlave* theSlave, jvxApiString* connector_out, jvxApiString* connector_in) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION link(IjvxLinkDataSlave* slave_out, jvxApiString* connector_slave_out, IjvxLinkDataSlave* slave_in, jvxApiString* connector_slave_in) = 0;
};

JVX_INTERFACE IjvxSchedulerLinkLane: public IjvxSchedulerLinkNet
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxSchedulerLinkLane(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION associate_master(IjvxConnectionMaster* theMaster, jvxApiString* connector_out, jvxApiString* connector_in) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION deassociate_master(IjvxConnectionMaster* theMaster) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION link_from_master(IjvxLinkDataSlave* slave_in, jvxApiString* connector_slave_in) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION link_to_master(IjvxLinkDataSlave* slave_out, jvxApiString* connector_slave_out) = 0;
};

JVX_INTERFACE IjvxSchedulerGroup : public IjvxInterfaceFactory, public IjvxSchedulerLinkNet
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxSchedulerGroup() {};
};

JVX_INTERFACE IjvxScheduler: public IjvxInterfaceFactory, public IjvxStateMachine
{	
public:

	virtual JVX_CALLINGCONVENTION ~IjvxScheduler(){};
	
	// Collect masters and slaves
	virtual jvxErrorType JVX_CALLINGCONVENTION number_registered_slaves(jvxSize* num_slaves) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION identification_registered_slave(jvxComponentIdentification* idTp, jvxSize idx) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_registered_slave(IjvxLinkDataSlave** slave, jvxSize idx) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_registered_slave(IjvxLinkDataSlave* slave) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION number_registered_masters(jvxSize* num_masters) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION identification_registered_master(jvxComponentIdentification* idTp, jvxSize idx) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_registered_master(IjvxConnectionMaster* master, jvxSize idx) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_registered_master(IjvxConnectionMaster* master, jvxSize idx) = 0;

	// Administer scheduler lanes
	virtual jvxErrorType JVX_CALLINGCONVENTION number_scheduler_lanes(jvxSize* num_lanes) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION name_scheduler_lane(jvxApiString* fldStr, jvxSize idx) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_scheduler_lane(jvxSize idx) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_scheduler_lane(IjvxSchedulerLinkLane** fldStr, jvxSize idx) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_scheduler_lane(IjvxSchedulerLinkLane* fldStr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION add_scheduler_lane(const char* name) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_lanes() = 0;
	
	// Administer scheduler groups
	virtual jvxErrorType JVX_CALLINGCONVENTION number_scheduler_groups(jvxSize* num_lanes) = 0;	
	virtual jvxErrorType JVX_CALLINGCONVENTION name_scheduler_group(jvxApiString* fldStr, jvxSize idx) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_scheduler_group(jvxSize idx) = 0;	
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_scheduler_group(IjvxSchedulerGroup** fldStr, jvxSize idx) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_scheduler_group(IjvxSchedulerGroup* fldStr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION add_scheduler_group(const char* name) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_groups() = 0;
	
};
	#endif
#endif
