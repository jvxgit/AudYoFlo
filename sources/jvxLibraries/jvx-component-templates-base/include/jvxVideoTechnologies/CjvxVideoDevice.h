#ifndef __CJVXVIDEODEVICE_H__
#define __CJVXVIDEODEVICE_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxDevice.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"

#include "pcg_CjvxVideoDevice_pcg.h"

/*
Format YUY2 422: https://en.wikipedia.org/wiki/YUV
 */
#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxVideoDevice: public IjvxDevice, public CjvxDevice, public CjvxObject, 
	public IjvxProperties, public CjvxProperties,
	public IjvxSequencerControl, public CjvxSequencerControl,
	public IjvxConfiguration,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector,
	public IjvxConnectionMaster, public CjvxConnectionMaster,
	public IjvxConnectionMasterFactory, public CjvxConnectionMasterFactory,
	public CjvxVideoDevice_genpcg
{
protected:

	struct
	{
		/*
		jvxInt32 samplerate;
		jvxInt32 buffersize;
		jvx_oneSelectionOption_cpp inputchannels;
		jvx_oneSelectionOption_cpp outputchannels;
		*/
		IjvxInputConnector* link_video_node;
		IjvxDataProcessor* link_video_proc;

		std::vector<jvxDataFormat> formats;
		std::vector<jvxDataFormatGroup> subformats;
	} _common_set_video_device;

    struct
    {
        jvxInt32 fps;
        jvxInt32 resolution_x;
        jvxInt32 resolution_y;
        jvxDataFormat format;
        jvxDataFormatGroup subformat;
    } _inproc;

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	JVX_CALLINGCONVENTION CjvxVideoDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxVideoDevice();

	// ======================================================================================
	// ======================================================================================
#define  JVX_STATE_MACHINE_DEFINE_STARTSTOP
#define  JVX_STATE_MACHINE_DEFINE_READY
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_READY
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	// ======================================================================================
	// ======================================================================================

	// ======================================================================================
	// Interface properties
	// ======================================================================================
#define JVX_PROPERTY_SIMPLIFY_OVERWRITE_SET
#include "codeFragments/simplify/jvxProperties_simplify.h"
#undef JVX_PROPERTY_SIMPLIFY_OVERWRITE_SET
	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	// Interface IjvxConfiguration

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename = "",
		jvxInt32 lineno = -1)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	// =====================================================================
	// All object default implementations
	// =====================================================================
#include "codeFragments/simplify/jvxObjects_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Allow control of object from sequencer
	// =====================================================================
#include "codeFragments/simplify/jvxSequencerControl_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Link to default input/output connector factory and input/output connector implementations
	// =====================================================================
#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// =====================================================================
#define JVX_INPUT_OUTPUT_CONNECTOR_MASTER
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_INPUT_CONNECTOR_ENDPOINT
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Link to default master factory and master implementations
	// =====================================================================
#include "codeFragments/simplify/jvxConnectorMasterFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// =====================================================================
#include "codeFragments/simplify/jvxConnectionMaster_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// For all non-object interfaces, return object reference
	// =====================================================================
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
	// =====================================================================
	// =====================================================================
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"
	// =====================================================================
	// Activate the relevant hidden interfaces
	// =====================================================================
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#define JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
	// =======================================================================
	// =======================================================================

#include "codeFragments/simplify/jvxDevice_simplify.h"

protected:

	jvxErrorType reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId);
	void updateDependentVariables_nolock(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurpose);
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
