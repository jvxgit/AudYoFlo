#ifndef __CJVXPACKETFILTERRULE_H__
#define __CJVXPACKETFILTERRULE_H__

#include "jvx.h"

#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"
#include "pcg_CjvxPacketFilterRule_pcg.h"

class CjvxPacketFilterRule: public IjvxPacketFilterRule, public CjvxObject, 
	public IjvxProperties, public CjvxProperties, public CjvxPacketFilterRules_genpcg
{
protected:
	
public:
	CjvxPacketFilterRule(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	
	~CjvxPacketFilterRule();


	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hRef) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION preprare_rule(jvxSize context_id,
		IjvxOnePacketQueue_queue*regularQueue, IjvxOnePacketQueue_send* regularSend,
		IjvxOnePacketQueue_queue* irregularQueue, IjvxOnePacketQueue_send* irregularSend)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_rule(jvxByte* fld, jvxSize numBytesUsed, jvxHandle* ptr_tag_add, jvxSize id_tag_add, jvxSize context_id, jvxBool* packet_remove) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_rule(jvxSize context_id) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;
		
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() override;

#include "codeFragments/simplify/jvxObjects_simplify.h"

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES

#include "codeFragments/simplify/jvxProperties_simplify.h"

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
};

#endif