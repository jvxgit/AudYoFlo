#ifndef __CJVXCONDITIONS_H__
#define __CJVXCONDITIONS_H__

#include "jvx.h"

typedef jvxErrorType (*callback_trigger_command)(jvxSize uniqueId, jvxSize* oper_state_on_return, jvxHandle* privData);
typedef jvxErrorType(*callback_trigger_condition)(jvxSize uniqueId, jvxBool* cond, jvxSize* oper_state_on_return, jvxHandle* privData);
typedef jvxErrorType (*callback_trigger_relative_jump)(jvxSize uniqueId, jvxSize* pc_increment, jvxSize* oper_state_on_return, jvxHandle* privData);

class CjvxSequencerControl
{
protected:
	struct oneCondition
	{
		callback_trigger_condition theStaticFunc;
		jvxHandle* priv;
		bool* fulfilled;
		jvxSize uniqueId;
		std::string description;
		jvxCBitField stateValid;
		oneCondition()
		{
			theStaticFunc = NULL;
			priv = NULL;
			fulfilled = NULL;
			uniqueId = JVX_SIZE_UNSELECTED;
			stateValid = JVX_STATE_NONE;
		};
	} ;

	struct oneCommand
	{
		callback_trigger_command theStaticFunc;
		jvxHandle* priv;
		jvxSize uniqueId;
		std::string description;
		jvxCBitField stateValid;
		oneCommand()
		{
			theStaticFunc = NULL;
			priv = NULL;
			uniqueId = JVX_SIZE_UNSELECTED;
			stateValid = JVX_STATE_NONE;
		}
	} ;

	struct oneRelativeJump
	{
		callback_trigger_relative_jump theStaticFunc;
		jvxHandle* priv;
		jvxSize uniqueId;
		std::string description;
		jvxCBitField stateValid;
		oneRelativeJump()
		{
			theStaticFunc = NULL;
			priv = NULL;
			uniqueId = JVX_SIZE_UNSELECTED;
			stateValid = JVX_STATE_NONE;
		}
	};

	struct
	{
		std::map<jvxSize, oneCondition> registeredConditions;
		std::map<jvxSize, oneCommand> registeredCommands;
		std::map<jvxSize, oneRelativeJump> registeredJumps;
	} _common_set_sequencerControl;

public:

	CjvxSequencerControl();

	~CjvxSequencerControl();

	jvxErrorType _number_conditions(jvxSize* num);

	jvxErrorType _description_condition(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxCBitField* stateValid);

	jvxErrorType _status_condition(jvxSize uniqueId, jvxBool* fulfilled, jvxSize* oper_state_on_return, IjvxObject* theObj);

	jvxErrorType _number_commands(jvxSize* num);

	jvxErrorType _description_command(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxCBitField* stateValid);

	jvxErrorType _trigger_command(jvxSize uniqueId, jvxSize* oper_state_on_return, IjvxObject* theObj);

	jvxErrorType _number_relative_jumps(jvxSize* num);

	jvxErrorType _description_relative_jump(jvxSize idx, jvxSize* uniqueId, jvxApiString* description, jvxCBitField* stateValid);

	jvxErrorType _trigger_relative_jump(jvxSize uniqueId, jvxSize* pc_increment, jvxSize* oper_state_on_return, IjvxObject* theObj);

	// Some local helpers

	jvxErrorType addCondition(oneCondition& elmCo);
	jvxErrorType remCondition(jvxSize uId);
	jvxErrorType remAllConditions();

	jvxErrorType addCommand(oneCommand& elmC);
	jvxErrorType remCommand(jvxSize uId);
	jvxErrorType remAllCommands();
	
	jvxErrorType addRelativeJump(oneRelativeJump& elmJ);
	jvxErrorType remRelativeJump(jvxSize uId);
	jvxErrorType remAllRelativeJumps();
};

#define JVX_SEQCONTROL_RELATIVEJUMP_FORWARD_C_CALLBACK_EXECUTE_FULL(baseclass, callbackname) jvxErrorType \
baseclass::callbackname(jvxSize uniqueId, jvxSize* pc_increment, jvxSize* oper_state, jvxHandle* privateData) \
{ \
	if (privateData) \
	{ \
		baseclass* this_pointer = reinterpret_cast<baseclass*>(privateData); \
		return(this_pointer->ic_ ## callbackname(uniqueId, pc_increment, oper_state)); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} \
jvxErrorType \
baseclass::ic_ ## callbackname(jvxSize uniqueId, jvxSize* pc_increment, jvxSize* oper_state)

#define JVX_SEQCONTROL_RELATIVEJUMP_FORWARD_C_CALLBACK_DECLARE(callbackname) \
static jvxErrorType \
callbackname(jvxSize uniqueId, jvxSize* pc_increment, jvxSize* oper_state, jvxHandle* privateData); \
jvxErrorType \
ic_ ## callbackname(jvxSize uniqueId, jvxSize* pc_increment, jvxSize* oper_state)


#define JVX_SEQCONTROL_COMMAND_C_CALLBACK_EXECUTE_FULL(baseclass, callbackname) jvxErrorType \
baseclass::callbackname(jvxSize uniqueId, jvxSize* oper_state, jvxHandle* privateData) \
{ \
	if (privateData) \
	{ \
		baseclass* this_pointer = reinterpret_cast<baseclass*>(privateData); \
		return(this_pointer->ic_ ## callbackname(uniqueId, oper_state)); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} \
jvxErrorType \
baseclass::ic_ ## callbackname(jvxSize uniqueId, jvxSize* oper_state)

#define JVX_SEQCONTROL_COMMAND_C_CALLBACK_DECLARE(callbackname) \
static jvxErrorType \
callbackname(jvxSize uniqueId, jvxSize* oper_state, jvxHandle* privData); \
jvxErrorType \
ic_ ## callbackname(jvxSize uniqueId, jvxSize* oper_state)


#define JVX_SEQCONTROL_CONDITION_C_CALLBACK_EXECUTE_FULL(baseclass, callbackname) jvxErrorType \
baseclass::callbackname(jvxSize uniqueId, jvxBool* fulfilled, jvxSize* oper_state, jvxHandle* privateData) \
{ \
	if (privateData) \
	{ \
		baseclass* this_pointer = reinterpret_cast<baseclass*>(privateData); \
		return(this_pointer->ic_ ## callbackname(uniqueId, fulfilled, oper_state)); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} \
jvxErrorType \
baseclass::ic_ ## callbackname(jvxSize uniqueId, jvxBool* fulfilled, jvxSize* oper_state)

#define JVX_SEQCONTROL_CONDITION_C_CALLBACK_DECLARE(callbackname) \
static jvxErrorType \
callbackname(jvxSize uniqueId, jvxBool* fulfilled, jvxSize* oper_state, jvxHandle* privData); \
jvxErrorType \
ic_ ## callbackname(jvxSize uniqueId, jvxBool* fulfilled, jvxSize* oper_state)

#endif
