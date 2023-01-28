#include "CjvxIosAudioTechnology.h"
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFString.h>
#include <AudioUnit/AudioUnit.h>
// =========================================================================================

CjvxIosAudioTechnology::CjvxIosAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

jvxErrorType
CjvxIosAudioTechnology::activate()
{
    jvxErrorType res = CjvxAudioTechnology::activate();

    if(res == JVX_NO_ERROR)
    {
        res = initializeIosAudio();
    }
    return(res);
}


jvxErrorType
CjvxIosAudioTechnology::deactivate()
{
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
	  auto elm = _common_tech_set.lstDevices.begin();
	  for(; elm != _common_tech_set.lstDevices.end(); elm++)
	    {
	      delete(elm->hdlDev);
	    }
	  _common_tech_set.lstDevices.clear();

	}
	return(res);
}


jvxErrorType
CjvxIosAudioTechnology::initializeIosAudio()
{
    jvxErrorType res = JVX_NO_ERROR;
	CjvxIosAudioDevice* ptrDev = NULL;
    jvxSize num = 0;
    std::string nm = "UNKNOWN";
    jvxSize i;

    theAudioLink.initializeTechnology(_common_set_min.theHostRef);
    theAudioLink.get_number_input_ports(&num);

    for(i = 0; i < num; i++)
    {
        theAudioLink.get_description_input_port(i, nm);

        ptrDev = new CjvxIosAudioDevice(
			nm.c_str(), false,_common_set.theDescriptor.c_str(),
			_common_set.theFeatureClass,
			_common_set.theModuleName.c_str(),
			JVX_COMPONENT_ACCESS_SUB_COMPONENT,
			JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
	
        ptrDev->init(i, this);
        CjvxTechnology::oneDeviceWrapper elm;
        elm.hdlDev = ptrDev;
        _common_tech_set.lstDevices.push_back(elm);
    }
	if(res != JVX_NO_ERROR)
	{
		this->deactivate();
	}
	return(res);
}

jvxErrorType
CjvxIosAudioTechnology::terminateIosAudio()
{
	return(JVX_NO_ERROR);
}
