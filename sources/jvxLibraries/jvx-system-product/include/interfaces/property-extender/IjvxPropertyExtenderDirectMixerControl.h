#ifndef _IJVXPROPERTYEXTENDERDIRECTMIXERCONTROL_H__
#define _IJVXPROPERTYEXTENDERDIRECTMIXERCONTROL_H__

JVX_INTERFACE IjvxDirectMixerControl
{
public:
	virtual ~IjvxDirectMixerControl() {};

};

JVX_INTERFACE IjvxDirectMixerControlClient: public IjvxPropertyExtender
{
public:
	virtual ~IjvxDirectMixerControlClient() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION register_direct_control_interface(IjvxDirectMixerControl* reg_me) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_direct_control_interface(IjvxDirectMixerControl* unreg_me)  = 0;
};

#endif
