#ifndef __CJVXPROPERTIESSIMPLE_H__
#define __CJVXPROPERTIESSIMPLE_H__

#include "jvx.h"
#include <map>
#include <iostream>
//#define CJVXPROPERTIESSIMPLIFY_VERBOSE_OUTPUT

class CjvxPropertiesSimple
{
public:

	CjvxPropertiesSimple();

	virtual jvxErrorType get_property_id_handle(std::string name, jvxSize *idx, jvxCallManagerProperties& callGate);

	virtual jvxErrorType get_property(std::string name, jvxInt16 *val, jvxSize num , jvxSize offset ,
		jvxCallManagerProperties& callGate);

	virtual jvxErrorType get_property(std::string name, jvxCBool* val, jvxSize num, jvxSize offset,
		jvxCallManagerProperties& callGate);

	virtual jvxErrorType get_property(std::string name, jvxInt32 *val, jvxSize num, jvxSize offset,
		jvxCallManagerProperties& callGate);

	virtual jvxErrorType get_property(std::string name, jvxSize *val,
		jvxCallManagerProperties& callGate);

	virtual jvxErrorType get_property_number_selected(std::string name, jvxSize *val,
		jvxCallManagerProperties& callGate);

	virtual jvxErrorType get_property(std::string name, std::string &val,
		jvxCallManagerProperties& callGate);

	virtual jvxErrorType get_property(std::string name, jvxData *val, jvxSize num, jvxSize offset,
		jvxCallManagerProperties& callGate);

	virtual jvxErrorType set_property(std::string name, jvxInt16 *val, jvxSize num, jvxSize offset,
		jvxCallManagerProperties& callGate);
	virtual jvxErrorType set_property(std::string name, jvxCBool* val, jvxSize num, jvxSize offset,
		jvxCallManagerProperties& callGate);
	virtual jvxErrorType set_property(std::string name, jvxInt32 *val, jvxSize num, jvxSize offset,
		jvxCallManagerProperties& callGate);
	virtual jvxErrorType set_property(std::string name, jvxSize *val,
		jvxCallManagerProperties& callGate);
	
	virtual jvxErrorType set_property(std::string name, jvxApiString *val,
		jvxCallManagerProperties& callGate);

	virtual jvxErrorType set_property(std::string name, jvxData *val, jvxSize num, jvxSize offset,
		jvxCallManagerProperties& callGate);

	jvxErrorType get_property_typesafe(std::string name,
		jvxHandle *val,
		jvxDataFormat format,
		jvxSize num ,
		jvxSize offset ,
		jvxBool contentOnly,
		jvxCallManagerProperties& callGate);

	jvxErrorType set_property_typesafe(std::string name,
		jvxHandle *val,
		jvxDataFormat format,
		jvxSize num ,
		jvxSize offset,
		jvxBool contentOnly,
		jvxCallManagerProperties& callGate);

	virtual jvxErrorType init_properties(IjvxAccessProperties* thePropRef);

	virtual jvxErrorType terminate_properties();

	jvxBool is_ready_properties();

private:

	void reportErrorGet(std::string name, jvxErrorType err, std::string info = "");

	void reportErrorSet(std::string name, jvxErrorType err, std::string info = "");

	jvxErrorType refresh_properties();

	IjvxAccessProperties* thePropRef; 

	std::map<std::string,jvxInt32> propidmap;
	jvxSize savedPropRevision;

};

#endif
