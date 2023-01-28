#ifndef __CJVXTEMPLATETECHNOLOGYVD_H__
#define __CJVXTEMPLATETECHNOLOGYVD_H__

#include "jvxTechnologies/CjvxTemplateTechnology.h"

/**
 * This class can be used for technologies that allow devices to be added
 * or removed at runtime. It is used to report an added and a removed device
 * to host and the automation instance of the application.
 */
template <class T> class CjvxTemplateTechnologyVD : 
	public CjvxTemplateTechnology<T>
{
protected:

public:

	JVX_CALLINGCONVENTION CjvxTemplateTechnologyVD(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
		CjvxTemplateTechnology<T>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
	};

	virtual JVX_CALLINGCONVENTION ~CjvxTemplateTechnologyVD()
	{
	};

	/**
	 * Call this function to report that a new device was added to the device list.
	 * To report a new device is only necessary if the device was added at runtime.
	 */
	void report_device_was_born(const std::string& ident)
	{
		// Report this to host for update of listed devices
		CjvxReportCommandRequest theRequest(
			jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST,
			this->_common_set.theComponentType);
		// This required here in Linux due to compiler processing standards
		this->_request_command(theRequest);

		// This will be forwarded to the applications "Automation" instance:
		// If the component is new it may be automatically activated
		// Automatic activation may trigger the autoconnect and this will also 
		// be reported to the automation instance.
		CjvxReportCommandRequest_id theRequestId(
			jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE,
			this->_common_set.theComponentType, ident.c_str(),
			jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);

		// This required here in Linux due to compiler processing standards
		this->_request_command(theRequestId);
	};

	/**
	 * Call this function to report that a new device was removed from the device list.
	 * To report a removed device is only necessary if the device was removed at runtime.
	 */
	void report_device_died(const std::string& ident)
	{
		CjvxReportCommandRequest_id theRequestId(
			jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE,
			this->_common_set.theComponentType, ident.c_str(),
			jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
		// This required here in Linux due to compiler processing standards
		this->_request_command(theRequestId);
	};
};

#endif
