#ifndef __CJVXMASTERDEVICE_H__
#define __CJVXMASTERDEVICE_H__

#include "jvx.h"

#include "common/CjvxObject.h"
#include "common/CjvxDevice.h"
#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"

class CjvxSimpleMasterDevice :
	public IjvxDevice, public CjvxDevice, public CjvxObject,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector,
	public IjvxConnectionMaster, public CjvxConnectionMaster,
	public IjvxConnectionMasterFactory, public CjvxConnectionMasterFactory
{
private:

	/*
	class CjvxCommonSetSimpleMasterDevice
	{
	public:
		jvxDeviceCapabilities devCaps = jvxDeviceCapabilities::JVX_DEVICE_CAPABILITY_UNKNOWN;
		jvxBool avail = true;
		jvxSize idTechnology = -1;
		jvxDataFormatGroup dGroup = JVX_DATAFORMAT_GROUP_NONE;
		std::string nmStream = "default";

	} _common_set_simple_master_device;
	*/

public:
	JVX_CALLINGCONVENTION CjvxSimpleMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxSimpleMasterDevice();

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master_autostate(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// =====================================================================
// Link to default input/output connector factory and input/output connector implementations
// =====================================================================
#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// =====================================================================
#define JVX_INPUT_OUTPUT_CONNECTOR_MASTER
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#define JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#undef JVX_INPUT_CONNECTOR_ENDPOINT

	virtual void test_set_output_parameters();

	// =====================================================================
	// =====================================================================

	// =====================================================================
// Link to default master factory and master implementations
// =====================================================================
#include "codeFragments/simplify/jvxConnectorMasterFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Master interface: Default implementation requires to find current settings
	// =====================================================================
#define JVX_CONNECTION_MASTER_SKIP_TEST
#include "codeFragments/simplify/jvxConnectionMaster_simplify.h"
#undef JVX_CONNECTION_MASTER_SKIP_TEST

	// =====================================================================
	// =====================================================================

		// =====================================================================
	// Activate the relevant hidden interfaces
	// =====================================================================
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#define JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION filterOptionsDevice(jvxBool* avail = nullptr,
		jvxSize* numDataStreams = nullptr,
		jvxSize* assSlot = nullptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION filterOptionsDataStream(jvxSize idx = 0,
		jvxDeviceCapabilities* caps = nullptr,
		jvxDataFormatGroup* dataGroup = nullptr,
		jvxApiString* astr = nullptr) override;
		*/

	// =====================================================================
	// All object default implementations
	// =====================================================================
#define JVX_OBJECT_SKIP_SET_LOCATION_INFO
#include "codeFragments/simplify/jvxObjects_simplify.h"
#undef JVX_OBJECT_SKIP_SET_LOCATION_INFO

	jvxErrorType set_location_info(const jvxComponentIdentification& tpMe) override;

	// =====================================================================
	// =====================================================================

	// =====================================================================
	// =====================================================================
#include "codeFragments/simplify/jvxDevice_simplify.h"
};

#endif
