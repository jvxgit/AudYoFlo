#include "CjvxSpNSimpleZeroCopy.h"

CjvxSpNSimpleZeroCopy::CjvxSpNSimpleZeroCopy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io_zerocopy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	/*
	 * Configurations:
	 * 
	 * Enable zero copy
	_common_set_ldslave.zeroCopyBuffering_cfg = true;

	 * Automatic prepare
	_common_set_node_base_1io.prepareOnChainPrepare = true;
	_common_set_node_base_1io.startOnChainStart = true;
	_common_set_node_base_1io.impPrepareOnChainPrepare = true;
	_common_set_node_base_1io.impStartOnChainStart = true;

	 * Default values for data parameters
	_common_set_1io_zerocopy.num_iochans = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.bsize_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.rate_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.form_io = JVX_DATAFORMAT_NONE;
	_common_set_1io_zerocopy.subform_io = JVX_DATAFORMAT_GROUP_NONE;
	
	 * Complaints about data prameters are directly forwarded
	forward_complain = true;
	 */
}

CjvxSpNSimpleZeroCopy::~CjvxSpNSimpleZeroCopy()
{	
} 

jvxErrorType
CjvxSpNSimpleZeroCopy::activate()
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::activate();
	if (res == JVX_NO_ERROR)
	{
		/*
		 * Activate all properties 
		 */
		genSimpleZeroCopy_node::init_all();
		genSimpleZeroCopy_node::allocate_all();
		genSimpleZeroCopy_node::register_all(static_cast<CjvxProperties*>(this));
	}
	return res;
}

jvxErrorType
CjvxSpNSimpleZeroCopy::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		/*
		 * Deactivate all properties
		 */
		res = CjvxBareNode1io_zerocopy::deactivate();
	}
	return res;
}

// =====================================================================================

#include "jvx-helpers-cpp.h"
jvxErrorType
CjvxSpNSimpleZeroCopy::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (!genSimpleZeroCopy_node::config.bypass.value)
	{
		jvxData** bufsInOut = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_ldslave.theData_in, idx_stage);

		// In-place processing
	}
	

	// Forward the processed data to next chain element
	res = _process_buffers_icon(mt_mask, idx_stage);
	return res;
}

// =====================================================================================

jvxErrorType
CjvxSpNSimpleZeroCopy::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::put_configuration(
		callMan,
		processor,
		sectionToContainAllSubsectionsForMe,
		filename,
		lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			std::vector<std::string> warns;
			genSimpleZeroCopy_node::put_configuration_all(
				callMan,
				processor,
				sectionToContainAllSubsectionsForMe,
				&warns);
			JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNSimpleZeroCopy::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::get_configuration(
		callMan,
		processor,
		sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genSimpleZeroCopy_node::get_configuration_all(
			callMan,
			processor,
			sectionWhereToAddAllSubsections);
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSimpleZeroCopy, set_bypass_mode)
{
	/*
	 * Arguments
	 * jvxCallManagerProperties* callGate
	 * jvxHandle* fld
	 * jvxSize* id
	 * jvxPropertyCategoryType* cat
	 * jvxSize* offsetStart
	 * jvxSize* numElements
	 * jvxDataFormat* form
	 */

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genSimpleZeroCopy_node::config.bypass))
	{
		// Incorporate the property value
		if (genSimpleZeroCopy_node::config.bypass.value)
		{

		}
		else
		{

		}
	}

	return JVX_NO_ERROR;
}