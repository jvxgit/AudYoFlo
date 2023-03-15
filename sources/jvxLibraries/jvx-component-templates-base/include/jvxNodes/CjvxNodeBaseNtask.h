#ifndef __CJVXNODEBASENTASK_H__
#define __CJVXNODEBASENTASK_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
//#include "templates/common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"

#include "pcg_CjvxNode_pcg.h"

#ifndef JVX_NODE_TYPE_SPECIFIER_TYPE
	#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_AUDIO_NODE
#endif

#ifndef JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "signal_processing_node/audio_node"
#endif

#define JVX_TEST_SETTING_PUT_ICON_ON_ENTER(set, prop_sec) \
	set.buffersize = prop_sec.buffersize.value; \
	set.samplerate = prop_sec.samplerate.value; \
	set.numberinputchannels = prop_sec.numberinputchannels.value; \
	set.numberoutputchannels = prop_sec.numberoutputchannels.value; \
	set.format = (jvxDataFormat)prop_sec.format.value; \
	set.segmentsize_x = prop_sec.segmentsize_x.value; \
	set.segmentsize_y = prop_sec.segmentsize_y.value; \
	set.subformat = (jvxDataFormatGroup)prop_sec.subformat.value;

#define JVX_TEST_SETTING_CPUT_ICON_ON_ENTER(set, prop_sec) \
	set.buffersize = prop_sec.buffersize.value; \
	set.samplerate = prop_sec.samplerate.value; \
	set.numberoutputchannels = prop_sec.numberinputchannels.value; \
	set.numberinputchannels = prop_sec.numberoutputchannels.value; \
	set.format = (jvxDataFormat)prop_sec.format.value; \
	set.segmentsize_x = prop_sec.segmentsize_x.value; \
	set.segmentsize_y = prop_sec.segmentsize_y.value; \
	set.subformat = (jvxDataFormatGroup)prop_sec.subformat.value;

#define JVX_TEST_SETTING_PROP_PUT(theData_out, prop_sec) \
	theData_out->con_params.buffersize = prop_sec.buffersize.value; \
	theData_out->con_params.rate = prop_sec.samplerate.value; \
	theData_out->con_params.format = (jvxDataFormat)prop_sec.format.value; \
	theData_out->con_params.number_channels = prop_sec.numberinputchannels.value; \
	theData_out->con_params.segmentation.x = prop_sec.segmentsize_x.value; \
	theData_out->con_params.segmentation.y = prop_sec.segmentsize_y.value; \
	theData_out->con_params.format_group = (jvxDataFormatGroup)prop_sec.subformat.value;

#define JVX_TEST_SETTING_PROP_CPUT(theData_out, prop_sec) \
	theData_out->con_params.buffersize = prop_sec.buffersize.value; \
	theData_out->con_params.rate = prop_sec.samplerate.value; \
	theData_out->con_params.format = (jvxDataFormat)prop_sec.format.value; \
	theData_out->con_params.number_channels = prop_sec.numberoutputchannels.value; \
	theData_out->con_params.segmentation_x = prop_sec.segmentsize_x.value; \
	theData_out->con_params.segmentation_y = prop_sec.segmentsize_y.value; \
	theData_out->con_params.format_group = (jvxDataFormatGroup)prop_sec.subformat.value;

#define JVX_TEST_SETTING_GET_ICON_ON_LEAVE(prop_sec, set) \
	prop_sec.buffersize.value = JVX_SIZE_INT32(set.buffersize); \
	prop_sec.samplerate.value = JVX_SIZE_INT32(set.samplerate); \
	prop_sec.numberinputchannels.value = JVX_SIZE_INT32(set.numberinputchannels); \
	prop_sec.numberoutputchannels.value = JVX_SIZE_INT32(set.numberoutputchannels); \
	prop_sec.format.value = JVX_SIZE_INT16(set.format); \
	prop_sec.segmentsize_x.value = JVX_SIZE_INT32(set.segmentsize_x); \
	prop_sec.segmentsize_y.value = JVX_SIZE_INT32(set.segmentsize_y); \
	assert((set.subformat == JVX_DATAFORMAT_GROUP_AUDIO_PCM) || (set.subformat == JVX_DATAFORMAT_GROUP_AUDIO_PCM_HOA)); 

#define JVX_TEST_SETTING_PROP_GET(prop_sec, prop_from) \
	prop_sec.buffersize.value = JVX_SIZE_INT32(prop_from->con_params.buffersize); \
	prop_sec.samplerate.value = JVX_SIZE_INT32(prop_from->con_params.rate); \
	prop_sec.format.value = JVX_SIZE_INT16(prop_from->con_params.format); \
	prop_sec.numberoutputchannels.value = JVX_SIZE_INT32(prop_from->con_params.number_channels); \
	prop_sec.segmentsize_x.value = JVX_SIZE_INT32(prop_from->con_params.segmentation_x); \
	prop_sec.segmentsize_y.value = JVX_SIZE_INT32(prop_from->con_params.segmentation_y); \
	prop_sec.subformat.value = JVX_SIZE_INT16(prop_from->con_params.format_group);

#define JVX_TEST_SETTING_PROP_COPY(prop_to, prop_from) \
	prop_to.buffersize.value = prop_from.buffersize.value; \
	prop_to.samplerate.value = prop_from.samplerate.value; \
	prop_to.numberinputchannels.value = prop_from.numberinputchannels.value; \
	prop_to.numberoutputchannels.value = prop_from.numberoutputchannels.value; \
	prop_to.format.value = prop_from.format.value; \
	prop_to.segmentsize_x.value = prop_from.segmentsize_x.value; \
	prop_to.segmentsize_y.value = prop_from.segmentsize_y.value; \
	prop_to.subformat.value = prop_from.subformat.value; 

#define JVX_TEST_SETTING_PROP_CCOPY(prop_to, prop_from) \
	prop_to.buffersize.value = prop_from.buffersize.value; \
	prop_to.samplerate.value = prop_from.samplerate.value; \
	prop_to.numberoutputchannels.value = prop_from.numberinputchannels.value; \
	prop_to.numberinputchannels.value = prop_from.numberoutputchannels.value; \
	prop_to.format.value = prop_from.format.value; \
	prop_to.segmentsize_x.value = prop_from.segmentsize_x.value; \
	prop_to.segmentsize_y.value = prop_from.segmentsize_y.value; \
	prop_to.subformat.value = prop_from.subformat.value; 

class CjvxNodeBaseNtask : public IjvxNode, public CjvxObject,
	public IjvxProperties, public CjvxProperties,
	public IjvxSequencerControl, public CjvxSequencerControl,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxConfiguration, 
	public CjvxNode_genpcg
{
protected:

public:
	JVX_CALLINGCONVENTION CjvxNodeBaseNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
	{
		_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
		_common_set.thisisme = static_cast<IjvxObject*>(this);
		_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);
	};


	virtual JVX_CALLINGCONVENTION ~CjvxNodeBaseNtask() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override;


#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxSequencerControl_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

#define JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
#define JVX_STATE_MACHINE_DEFINE_STARTSTOP
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP
#undef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS

#include "codeFragments/simplify/jvxObjects_simplify.h"

	// Interfaces and default implementations for connections
#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"

	// ===================================================================================================
	// Interface IjvxConfiguration
	// ===================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================================
	// Interface 
	// ===================================================================================================
#if 0
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
#endif

	jvxErrorType reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId);
};


#endif