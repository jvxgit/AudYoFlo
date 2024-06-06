#ifndef __CJVXNEGOTIATE_H__
#define __CJVXNEGOTIATE_H__

class CjvxObjectLog;

typedef enum
{
	JVX_NEGOTIATE_CONSTRAINT_NO_CHANGE,
	JVX_NEGOTIATE_CONSTRAINT_CHANGE
} jvxConstraintSetResult;

class CjvxNegotiate_common
{
	class jvxNegotiateParamsRange
	{
	public:
		struct
		{
			jvxSize min;
			jvxSize max;
		} buffersize;

		struct
		{
			jvxSize min;
			jvxSize max;
		} dimX;

		struct
		{
			jvxSize min;
			jvxSize max;
		} dimY;

		struct
		{
			jvxSize min;
			jvxSize max;
		} number_channels;

		struct
		{
			jvxSize min;
			jvxSize max;
		} samplerate;

		struct
		{
			jvxDataFormat min;
			jvxDataFormat max;
		} format;

		struct
		{
			jvxDataFormatGroup min;
			jvxDataFormatGroup max;
		} subformat;

		struct
		{
			jvxDataflow min;
			jvxDataflow max;
		} data_flow;

		/**
		 * The range values are initialized to accept every setting - all entries are dont-care values.
		 * The component should set the preferred values by using one of the functions _set_parameters_fixed
		 * or _update_parameters_fixed. The function _clear_parameters_fixed allows to convert fixed specified 
		 * values into a dont-care value.
		 * The parameters number_channels and segmentation_y as well as the buffersize and segmentation_x can be 
		 * coupled by using the member variables coupleBsizeDimX and oupleChannelsDimY which are activated by default.
		 */
		jvxNegotiateParamsRange()
		{
			number_channels.min = JVX_SIZE_UNSELECTED;
			number_channels.max = JVX_SIZE_UNSELECTED;
			
			buffersize.min = JVX_SIZE_UNSELECTED;
			buffersize.max = JVX_SIZE_UNSELECTED;

			samplerate.min = JVX_SIZE_UNSELECTED;
			samplerate.max = JVX_SIZE_UNSELECTED;

			format.min = JVX_DATAFORMAT_NONE;
			format.max = JVX_DATAFORMAT_NONE;
			
			subformat.min = JVX_DATAFORMAT_GROUP_NONE;
			subformat.max = JVX_DATAFORMAT_GROUP_NONE;

			dimX.min = JVX_SIZE_UNSELECTED;
			dimX.max = JVX_SIZE_UNSELECTED;

			dimY.min = JVX_SIZE_UNSELECTED;
			dimY.max = JVX_SIZE_UNSELECTED;

			data_flow.min = jvxDataflow::JVX_DATAFLOW_DONT_CARE;
			data_flow.max = jvxDataflow::JVX_DATAFLOW_DONT_CARE;
		};
	} ;
public:

	enum class negBehaviorType
	{
		JVX_BEHAVIOR_AUDIO,
		JVX_BEHAVIOR_VIDEO
	};
	std::vector<dedicatedSetup> allowedSetup;
	jvxNegotiateParamsRange preferred;
	jvxNegotiateParamsRange stack_preferred;
	jvxBool stack_pushed;
	jvxLinkDataDescriptor_con_params _latest_results;
	negBehaviorType negBehavior = negBehaviorType::JVX_BEHAVIOR_AUDIO;
	jvxBool allowCompromiseOutput = true;
	CjvxObjectLog* logObj = nullptr;

public:

	CjvxNegotiate_common();

	/**
	 * This function allows to specify a FIXED setting. Note that the default arguments are set to valid values - no dont-cares.
	 *///=======================================================================================================
	void _set_parameters_fixed(
		jvxSize num_channels = 1,
		jvxSize bsize = 512,
		jvxSize srate = 48000,
		jvxDataFormat format = JVX_DATAFORMAT_DATA,
		jvxDataFormatGroup sub_format = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED,
		jvxDataflow datflow = jvxDataflow::JVX_DATAFLOW_DONT_CARE,
		jvxSize segment_x = JVX_SIZE_UNSELECTED,
		jvxSize segment_y = JVX_SIZE_UNSELECTED);

	/** This function updates the preferred settings. When invoking with no argument or with the default arg,
	 * no constraint will be set.
	 *///======================================================================================================
	jvxConstraintSetResult _update_parameters_fixed(
		jvxSize num_channels = JVX_SIZE_UNSELECTED,
		jvxSize bsize = JVX_SIZE_UNSELECTED,
		jvxSize srate = JVX_SIZE_UNSELECTED,
		jvxDataFormat format = JVX_DATAFORMAT_NONE,
		jvxDataFormatGroup sub_format = JVX_DATAFORMAT_GROUP_NONE,
		jvxDataflow dflow = jvxDataflow::JVX_DATAFLOW_DONT_CARE,
		jvxLinkDataDescriptor* datOut = NULL,
		jvxSize segment_x = JVX_SIZE_UNSELECTED,
		jvxSize segment_y = JVX_SIZE_UNSELECTED);

	jvxConstraintSetResult _clear_parameters_fixed(
		jvxBool numChansIn = false,
		jvxBool bsize = false,
		jvxBool srate = false,
		jvxBool format = false,
		jvxBool sub_format = false,
		jvxBool datflow = false,
		jvxBool segsizex = true,
		jvxBool segsizey = true);

	jvxErrorType derive(jvxLinkDataDescriptor_con_params* params);

	jvxErrorType _push_constraints();
	jvxErrorType _pop_constraints();
	jvxBool _stack_pushed();
	
	void compare_core(jvxLinkDataDescriptor* ld_cp, jvxLinkDataDescriptor* ld_fix, jvxBool& thereismismatch);

	/**
	 * This function takes in the propsed values from the successor and compares it
	 * with the currently allowed settings. If the requested setup does not match, the values in
	 * ld will be modified such that it matches the allowed parameters. Then, the content in dataOut will
	 * be adapted accordingly and finally, the value of JVX_ERROR_COMPROMISE is returned. This lets
	 * the caller know that parameter modification was performed. The successor may accept the modified
	 * parameter values OR it may return the error code to the previous output connector if a match can not be found!
	 * If modFlags are a nullptr reference, the modified values will be copied to the dataOut struct.
	 * Otherwisem the caller can evaluate the modFlags to act as desired.
	 */
	jvxErrorType _negotiate_transfer_backward_ocon(
		jvxLinkDataDescriptor* ld,
		jvxLinkDataDescriptor* dataOut,
		IjvxObject* this_pointer,
		jvxCBitField* modFlags
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	void _constrain_ldesc(jvxLinkDataDescriptor* theData) const;
};
	
class CjvxNegotiate_input : public CjvxNegotiate_common
{
public:
	CjvxNegotiate_input();

	//! This function runs the actual negotiation and returns the alternative proposal.
	jvxErrorType _negotiate_connect_icon(
		jvxLinkDataDescriptor* theData_in,
		IjvxObject* this_pointer,
		const char*descror
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
};

class CjvxNegotiate_output : public CjvxNegotiate_common
{
public:
	CjvxNegotiate_output();	
};

#endif
