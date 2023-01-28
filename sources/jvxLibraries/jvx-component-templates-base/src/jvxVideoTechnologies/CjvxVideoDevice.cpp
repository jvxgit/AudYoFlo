#include "jvxVideoTechnologies/CjvxVideoDevice.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxVideoDevice::CjvxVideoDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_VIDEO_DEVICE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	this->_common_set_video_device.link_video_node = NULL;
	this->_common_set_video_device.link_video_proc = NULL;
}

CjvxVideoDevice::~CjvxVideoDevice()
{
	this->terminate();
}

jvxErrorType
CjvxVideoDevice::activate()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	res = _activate();
	if(res == JVX_NO_ERROR)
	{
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), 
			"default", static_cast<IjvxConnectionMaster*>(this), _common_set.theModuleName.c_str());
		JVX_ACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER(static_cast<CjvxObject*>(this), "default");

		init_all();
		allocate_all();
		register_all(static_cast<CjvxProperties*>(this));
	}
	return(res);
}

jvxErrorType
CjvxVideoDevice::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _deactivate();
	if(res == JVX_NO_ERROR)
	{

		unregister_all(static_cast<CjvxProperties*>(this));
		//deassociate__properties_active(static_cast<CjvxProperties*>(this));

		deallocate_all();

		CjvxVideoDevice_genpcg::video.format.value = (jvxInt16)JVX_DATAFORMAT_NONE;

		JVX_DEACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER();
		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
}

jvxErrorType
CjvxVideoDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxBool report_update = false;
	jvxErrorType res = JVX_NO_ERROR;
	res = _set_property(
		callGate,
		rawPtr,
		ident, trans);
	if(res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		updateDependentVariables_nolock(propId, category, false, callGate.call_purpose);
	}
	return(res);
}

jvxErrorType
CjvxVideoDevice::put_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	if(this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// Read all parameters from this class
		CjvxVideoDevice_genpcg::put_configuration__video(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		if(!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for(i = 0; i < warnings.size(); i++)
			{
				txt = "Failed to read property " + warnings[i];
				_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}


			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		// Update all dependant props
		//this->updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

	}
	return(res);
}

jvxErrorType
CjvxVideoDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{

	// Write all parameters from this class
	CjvxVideoDevice_genpcg::get_configuration__video(callConf, processor, sectionWhereToAddAllSubsections);

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxVideoDevice::prepare()
{
 //   jvxSize i;
	jvxErrorType res = _prepare();
	if(res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_start();

	}
	return(res);
}

jvxErrorType
CjvxVideoDevice::postprocess()
{
	jvxErrorType res = _postprocess();
	if(res == JVX_NO_ERROR)
	{

		CjvxProperties::_update_properties_on_stop();

	}
	return(res);
}

void
CjvxVideoDevice::updateDependentVariables_nolock(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurpose)
{
	jvxBool reportSize = false;

	if (
		updateAll ||
		(
		 (propId == CjvxVideoDevice_genpcg::video.segmentsize_x.globalIdx) &&
		 (CjvxVideoDevice_genpcg::video.segmentsize_x.category == category)
		 ))
	{
		reportSize = true;
		this->_report_property_has_changed(
			CjvxVideoDevice_genpcg::video.segmentsize_x.category,
			CjvxVideoDevice_genpcg::video.segmentsize_x.globalIdx,
			false, 0, 1,
			JVX_COMPONENT_UNKNOWN, callPurpose);
	}

	if (
		updateAll ||
		(
		 (propId == CjvxVideoDevice_genpcg::video.segmentsize_y.globalIdx) &&
		 (CjvxVideoDevice_genpcg::video.segmentsize_y.category == category)))
	{
		reportSize = true;
		this->_report_property_has_changed(
			CjvxVideoDevice_genpcg::video.segmentsize_y.category,
			CjvxVideoDevice_genpcg::video.segmentsize_y.globalIdx,
			false, 0, 1,
			JVX_COMPONENT_UNKNOWN, callPurpose);
	}

	if (
		updateAll ||
		(
		 (propId == CjvxVideoDevice_genpcg::video.format.globalIdx) &&
		 (CjvxVideoDevice_genpcg::video.format.category == category)))
	{
		reportSize = true;
		this->_report_property_has_changed(
			CjvxVideoDevice_genpcg::video.format.category,
			CjvxVideoDevice_genpcg::video.format.globalIdx,
			false, 0, 1,
			JVX_COMPONENT_UNKNOWN, callPurpose);
	}

	if (
		updateAll ||
		(
		 (propId == CjvxVideoDevice_genpcg::video.subformat.globalIdx) &&
		 (CjvxVideoDevice_genpcg::video.subformat.category == category)))
	{
		reportSize = true;
		this->_report_property_has_changed(
			CjvxVideoDevice_genpcg::video.subformat.category,
			CjvxVideoDevice_genpcg::video.subformat.globalIdx,
			false, 0,1, 
			JVX_COMPONENT_UNKNOWN, callPurpose);
	}

	if (
		updateAll ||
		(
		 (propId == CjvxVideoDevice_genpcg::video.framerate.globalIdx) &&
		 (CjvxVideoDevice_genpcg::video.framerate.category == category)))
	{
		this->_report_property_has_changed(
			CjvxVideoDevice_genpcg::video.framerate.category,
			CjvxVideoDevice_genpcg::video.framerate.globalIdx,
			false, 0, 1,
			JVX_COMPONENT_UNKNOWN, callPurpose);
	}

	if (reportSize)
	{
		this->_report_property_has_changed(
			CjvxVideoDevice_genpcg::video.framesize.category,
			CjvxVideoDevice_genpcg::video.framesize.globalIdx,
			false, 0, 1,
			JVX_COMPONENT_UNKNOWN, callPurpose);
	}
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
