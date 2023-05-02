#ifndef _IJVXTRIGGERCONNECTOR_H__
#define _IJVXTRIGGERCONNECTOR_H__

JVX_INTERFACE IjvxTriggerOutputConnector;
JVX_INTERFACE IjvxTriggerInputConnector;

JVX_INTERFACE IjvxTriggerConnector
{
	virtual ~IjvxTriggerConnector() {};
	
	virtual jvxErrorType trigger(jvxTriggerConnectorPurpose purp, jvxHandle* data = nullptr) = 0;
};

JVX_INTERFACE IjvxTriggerInputConnector: public IjvxTriggerConnector
{
public:
	virtual ~IjvxTriggerInputConnector(){};
	
	virtual jvxErrorType link_connect_otcon(IjvxTriggerOutputConnector* otcon) = 0;
	virtual jvxErrorType unlink_connect_otcon(IjvxTriggerOutputConnector* otcon) = 0;
};

JVX_INTERFACE IjvxTriggerOutputConnector : public IjvxTriggerConnector
{
public:
	virtual ~IjvxTriggerOutputConnector(){};
	
	virtual jvxErrorType link_connect_itcon(IjvxTriggerInputConnector* otcon) = 0;
	virtual jvxErrorType unlink_connect_itcon(IjvxTriggerInputConnector* otcon) = 0;
};

#endif