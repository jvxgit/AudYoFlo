#ifndef __CJVXVSTPLUGINCONTROLLER__H__
#define __CJVXVSTPLUGINCONTROLLER__H__

#include "jvx.h"
#include "TjvxPluginCommon.h"
#include "CjvxPluginCommon.h"
#include "IjvxPluginCommon.h"

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/base/ustring.h"

namespace Steinberg {
namespace Vst {
		
//------------------------------------------------------------------------

class CjvxPluginVstController: public IjvxPluginParamMessage, public IjvxPluginMessageControl
{
protected:
	EditController* theController = nullptr;
public:
	CjvxPluginVstController();

	// ========================================================================
	// ========================================================================

	/**
	 * Process an incoming message that passes a string list. If there is no parameter to which the message can
	 * be directly mapped to it is forwarded to the controller via this member function. 
	 */ 
	virtual void process_unmatched_strlist(Steinberg::int32 id, jvxData normVal, jvxApiStringList* strl) = 0;

	/**
	 * Process an incoming message that passes a simple normalized parameter. If there is no parameter to which the message can
	 * be directly mapped to it is forwarded to the controller via this member function.
	 */
	virtual void process_unmatched_parameter(int32 id, jvxData normVal, jvxApiStringList* strl) = 0;

	//! Function to report a state update (started and stopped).
	virtual void process_state_update(jvxAudioPluginBinaryStateChange stat) = 0;

	/**
	 * Member function to report a completed process. Typically, a process is defined as a set of 
	 * messages that are handled automatically. With the last message the receiver responds by sending a 
	 * "process complete" message to the sender.
	 */
	virtual void process_process_complete(jvxInt16 proc_id) = 0;

	/**
	 * This function is used to report the transfer of a linear field with values
	 */
	virtual void process_linear_field(
		jvxData* fld_ptr, jvxSize fld_sz, 
		jvxInt32 id, jvxInt32 subid,
		jvxSize num, jvxSize offset) = 0;

	// ========================================================================
	jvxErrorType handle_message_from_processor(IMessage* message);
	
	void trigger_update_params(jvxInt32* allIds, jvxSize numAllIds, const std::string& tag, jvxInt16 process_id);
	void trigger_update_state(jvxInt16 process_id);

	// ===========================================================================
	virtual jvxErrorType trigger_transfer_param_norm_strlist(jvxInt32 id, jvxData normv, jvxErrorType resL, jvxApiStringList* strl, const char* tag, jvxInt16 process_id) override;
	virtual jvxErrorType trigger_transfer_param_norm(jvxInt32 id, jvxData normv, jvxErrorType resL, const char* tag, jvxInt16 process_id) override;

	// ===========================================================================
	virtual jvxErrorType plugin_send_message(jvxAudioPluginBinaryHeader* fld) override;

};

} // namespace Vst
} // namespace Steinberg

#endif