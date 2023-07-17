#ifndef __CJVXCUTGPSDEVICE_H__
#define __CJVXCUTGPSDEVICE_H__

#include "jvx.h"

#define NUM_MESSAGES_IN_QUEUE_MAX 10
#define TIMEOUT_IF_MQUEUE_CAN_NOT_BE_HANDLED_MSEC 500

#define JVX_FLOW_CONDITION_FLOW_BOUNDS_ID 0

#include "jvxGenericConnectionTechnologies/CjvxGenericConnectionTextDevice.h"
#include "jvxGenericRs232Technologies/CjvxGenericRs232Device.h"

#include "CjvxFlexibleConnectionTextDevice.h"
#include "pcg_exports_device.h"

/**
 * Class to drive a flow control rs-232 device. Each device sorresponds to one rs-232 connection.
 * Flow control devices do not send any messages but accept incoming messages.
 */
class CjvxCuTGpsDevice : public CjvxFlexibleConnectionTextDevice<CjvxGenericConnectionTextDevice<CjvxGenericRs232Device>>,
	public genGpsRs232_device
{

public:

	//! Constructor
	JVX_CALLINGCONVENTION CjvxCuTGpsDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	//! Destructor
	virtual JVX_CALLINGCONVENTION ~CjvxCuTGpsDevice();

	//! Call specific initialization of rs232 Flow Control Device
	jvxErrorType activate_specific_connection() override;

	//! Call specific deactivation function
	jvxErrorType deactivate_specific_connection() override;

	//! Function to initiate initialization messages on first connect
	void activate_init_messages() override;

	//! Observer timeout callback
	void report_observer_timeout() override;

	//! Single sequencer condition: check that flow is in the right state
	JVX_SEQCONTROL_CONDITION_C_CALLBACK_DECLARE(check_condition_device);

	//! Put configuration to apply specific parameters
	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename, jvxInt32 lineno) override;

	//! Get configuration to store specific property values
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) override;

	jvxErrorType set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	void convert_nmea_gpvtg(const std::string& token);

	void convert_nmea_gpgga(const std::string& token);
		
	void convert_nmea_gpgsa(const std::string& token);
	
	void convert_nmea_gpgll(const std::string& token);
	
	void convert_nmea_gprmc(const std::string& token);

	void convert_nmea_gpgsv(const std::string& token);

};

#endif