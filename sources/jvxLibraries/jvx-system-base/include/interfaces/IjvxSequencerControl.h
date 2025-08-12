#ifndef __IJVXSEQUENCERCONTROL_H__
#define __IJVXSEQUENCERCONTROL_H__

JVX_INTERFACE IjvxSequencerControl: public IjvxInterfaceReference
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxSequencerControl(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_conditions(jvxSize* num) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION description_condition(jvxSize idx, jvxSize* unqiueId, jvxApiString* description, jvxUInt64* stateValid) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION status_condition(jvxSize uniqueId, jvxBool* fulfilled, jvxSize* oper_state_on_return) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION number_commands(jvxSize* num) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION description_command(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxUInt64* stateValid) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_command(jvxSize uniqueId, jvxSize* oper_state_on_return) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION number_relative_jumps(jvxSize* num) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION description_relative_jump(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxUInt64* stateValid) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_relative_jump(jvxSize uniqueId, jvxSize* pc_increment, jvxSize* oper_state_on_return) = 0;
};

#endif
