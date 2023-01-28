#include "jvxTools/jvxPacketFilterRules/CjvxPacketFilterRule.h"

CjvxPacketFilterRule::CjvxPacketFilterRule(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_PACKETFILTER_RULE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxPacketFilterRule*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}
	
CjvxPacketFilterRule::~CjvxPacketFilterRule()
{
}

jvxErrorType 
CjvxPacketFilterRule::initialize(IjvxHiddenInterface* hRef)
{
	jvxErrorType res = CjvxObject::_initialize(hRef);
	return(res);
}

jvxErrorType 
CjvxPacketFilterRule::activate( )
{
	jvxErrorType res = CjvxObject::_activate();
	if(res == JVX_NO_ERROR)
	{
		CjvxPacketFilterRules_genpcg::init_all();
		CjvxPacketFilterRules_genpcg::allocate_all();
		CjvxPacketFilterRules_genpcg::register_all(static_cast<CjvxProperties*>(this));


	}
	return(res);
}

jvxErrorType 
CjvxPacketFilterRule::preprare_rule(jvxSize context_id,
	IjvxOnePacketQueue_queue*regularQueue, IjvxOnePacketQueue_send* regularSend,
	IjvxOnePacketQueue_queue* irregularQueue, IjvxOnePacketQueue_send* irregularSend)
{
	return(JVX_NO_ERROR);
}
		
jvxErrorType
CjvxPacketFilterRule::postprocess_rule(jvxSize context_id)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxPacketFilterRule::process_rule(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_tag_add, jvxSize id_tag_add, jvxSize context_id, jvxBool* packet_remove)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxPacketFilterRule::deactivate()
{
	jvxErrorType res = CjvxObject::_deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxPacketFilterRules_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxPacketFilterRules_genpcg::deallocate_all();
	}
	return(res);
}
		
jvxErrorType 
CjvxPacketFilterRule::terminate()
{
	return(JVX_NO_ERROR);
}
