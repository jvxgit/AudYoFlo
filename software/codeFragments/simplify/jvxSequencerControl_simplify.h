	// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 
	// Interface CjvxConditions
	// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 
	
virtual jvxErrorType JVX_CALLINGCONVENTION number_conditions(jvxSize* num)override
{return(_number_conditions(num));};

virtual jvxErrorType JVX_CALLINGCONVENTION description_condition(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxUInt64* statesAllowed)override
{return(_description_condition(idx, uniqueId, description, statesAllowed));};

virtual jvxErrorType JVX_CALLINGCONVENTION status_condition(jvxSize uniqueId, jvxBool* fulfilled, jvxSize* oper_state_on_return)override
{return(_status_condition(uniqueId, fulfilled, oper_state_on_return, static_cast<IjvxObject*>(this)));};

virtual jvxErrorType JVX_CALLINGCONVENTION number_commands(jvxSize* num)override
{return(_number_commands(num));};
	
virtual jvxErrorType JVX_CALLINGCONVENTION description_command(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxCBitField* statesAllowed)override
 {return(_description_command(idx, uniqueId, description, statesAllowed));};

virtual jvxErrorType JVX_CALLINGCONVENTION trigger_command(jvxSize uniqueId, jvxSize* oper_state_on_return)override
{return(_trigger_command(uniqueId, oper_state_on_return, static_cast<IjvxObject*>(this)));};
	
virtual jvxErrorType JVX_CALLINGCONVENTION number_relative_jumps(jvxSize* num)override
{return(_number_relative_jumps(num));};
	
virtual jvxErrorType JVX_CALLINGCONVENTION description_relative_jump(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxUInt64* stateValid)override
{return(_description_relative_jump(idx, uniqueId, description, stateValid));};

virtual jvxErrorType JVX_CALLINGCONVENTION trigger_relative_jump(jvxSize uniqueId, jvxSize* pc_increment, jvxSize* oper_state_on_return)override
{return(_trigger_relative_jump(uniqueId, pc_increment, oper_state_on_return, static_cast<IjvxObject*>(this)));};
