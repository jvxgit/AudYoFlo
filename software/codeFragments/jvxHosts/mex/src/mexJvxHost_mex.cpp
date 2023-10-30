#include "mexJvxHost.h"
#include "mexCmdMapperJvxHost.h"
#include "HjvxMex2CConverter.h"
#include "CjvxJson.h"

void
mexJvxHost::mexCellContentToStringList(const mxArray* arr, std::vector<std::string>& strList)
{
	const mwSize* sz = mxGetDimensions(arr);
	for(int ii = 0; ii < sz[1]; ii++)
	{
		// Get reference to current cell field
		mxArray* arrWork = mxGetCell(arr, ii);
		if(mxIsChar(arrWork))
		{
			strList.push_back(jvx_mex_2_cstring(arrWork));
		}
		else if(mxIsCell(arrWork))
		{
			mexCellContentToStringList(arrWork,strList);
		}
	}
}

void
mexJvxHost::mexReturnComponentIdentification(mxArray*& plhs, const jvxComponentIdentification& cpTp)
{
	mxArray* arr = NULL;
	jvxSize numFlds = 3;
	const char** fld = NULL;

	plhs = NULL;

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	JVX_DSP_SAFE_ALLOCATE_FIELD(fld, const char*, numFlds);

	fld[0] = FLD_NAME_COMPONENT_IDENTIFICATION_TYPE;
	fld[1] = FLD_NAME_COMPONENT_IDENTIFICATION_SLOT_ID;
	fld[2] = FLD_NAME_COMPONENT_IDENTIFICATION_SLOT_SUB_ID;

	plhs = mxCreateStructArray(ndim, dims, numFlds, fld);

	JVX_DSP_SAFE_DELETE_FIELD(fld);
	fld = NULL;

	arr = NULL;
	mexReturnString(arr, jvxComponentType_txt(cpTp.tp));
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	mexReturnInt32(arr, (jvxInt32)cpTp.slotid);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	arr = NULL;
	mexReturnInt32(arr, (jvxInt32)cpTp.slotsubid);
	mxSetFieldByNumber(plhs, 0, 2, arr);

}

void
mexJvxHost::mexReturnStructSetup(mxArray*& plhs)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;

	const char** fld = NULL;

	JVX_SAFE_NEW_FLD(fld, const char*, 6);
	std::string fldName0 = FLD_NAME_COPYRIGHT_STRING;
	std::string fldName1 = FLD_NAME_VERSION_STRING;
	std::string fldName2 = FLD_NAME_MODULENAME_STRING;
	std::string fldName3 = FLD_NAME_STATE_BITFIELD;
	std::string fldName4 = FLD_NAME_COMPONENTS;
	std::string fldName5 = FLD_NAME_FLOAT_CONFIGURATION_ID;

	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();
	fld[3] = fldName3.c_str();
	fld[4] = fldName4.c_str();
	fld[5] = fldName5.c_str();

	plhs = mxCreateStructArray(ndim, dims, 6, fld);
	JVX_SAFE_DELETE_FLD(fld, const char*);

	//===================================================

	this->mexReturnString(arr, EXPRESSION_COPYRIGHT);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	arr = NULL;

	this->mexReturnString(arr, EXPRESSION_VERSION);
	mxSetFieldByNumber(plhs, 0, 1, arr);
	arr = NULL;

	this->mexReturnString(arr, EXPRESSION_MODULENAME);
	mxSetFieldByNumber(plhs, 0, 2, arr);
	arr = NULL;

	this->involvedComponents.theHost.hobject->state(&theState);
	this->mexReturnBitField(arr, (jvxCBitField)theState);
	mxSetFieldByNumber(plhs, 0, 3, arr);
	arr = NULL;

	this->mexReturnComponents(arr);
	mxSetFieldByNumber(plhs, 0, 4, arr);
	arr = NULL;

	jvxInt16 flt_id = JVX_FLOAT_DATAFORMAT_ID;
	this->mexReturnInt16(arr, flt_id);
	mxSetFieldByNumber(plhs, 0, 5, arr);
}

void
mexJvxHost::mexReturnComponents(mxArray*& plhs)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize i;

	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;

	const char** fld = NULL;
	std::string* fldStr = NULL;

	JVX_SAFE_NEW_FLD(fld, const char*, JVX_COMPONENT_ALL_LIMIT);
	JVX_SAFE_NEW_FLD(fldStr, std::string, JVX_COMPONENT_ALL_LIMIT);

	for(i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		std::string nmComponent = jvxComponentType_txtf(i);
		std::transform(nmComponent.begin(), nmComponent.end(), nmComponent.begin(), ::toupper);
		nmComponent = jvx_replaceStrInStr(nmComponent, " ", "_");
		fldStr[i] = nmComponent;
		fld[i] = fldStr[i].c_str();
	}

	plhs = mxCreateStructArray(ndim, dims, JVX_COMPONENT_ALL_LIMIT, fld);
	JVX_SAFE_DELETE_FLD(fld, const char*);
	JVX_SAFE_DELETE_FLD(fldStr, std::string);

	for(i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		arr = NULL;
		jvxComponentType tp = (jvxComponentType)i;

		this->mexReturnStructOneComponent(arr, tpAll[tp]);
		mxSetFieldByNumber(plhs, 0, (int)i, arr);
	}

}

void
mexJvxHost::mexReturnStructOneComponent(mxArray*& plhs, const jvxComponentIdentification& tp)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;

	const char** fld = NULL;
	std::string* fldStr = NULL;

	involvedHost.hHost->number_components_system(tp, &num);

	if(num > 0)
	{
		JVX_SAFE_NEW_FLD(fld, const char*, num);
		JVX_SAFE_NEW_FLD(fldStr, std::string, num);

		for(i = 0; i < num; i++)
		{
			fldStr[i]= std::string(PREFIX_ONE_COMPONENT) + "_" + jvx_size2String(i);
			fld[i] = fldStr[i].c_str();
		}

		plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

		JVX_SAFE_DELETE_FLD(fld, const char*);
		JVX_SAFE_DELETE_FLD(fldStr, std::string);

		for(i = 0; i < num; i++)
		{
			arr = NULL;
			mexReturnStructContentOneComponent(arr, tp, i);

			mxSetFieldByNumber(plhs, 0, (int)i, arr);
		}
	}
}

void
mexJvxHost::mexReturnStructContentOneComponent(mxArray*& plhs, const jvxComponentIdentification& tp,jvxSize idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;
	jvxSize selId = JVX_SIZE_UNSELECTED;
	jvxBool isReady = false;
	jvxBitField btFld = 0;
	jvxComponentAccessType acTp = JVX_COMPONENT_ACCESS_UNKNOWN;

	std::string fldName0 = FLD_NAME_COMPONENT_DESCRIPTION_STRING;
	std::string fldName1 = FLD_NAME_COMPONENT_DESCRIPTOR_STRING;
	std::string fldName2 = FLD_NAME_COMPONENT_MODULE_NAME_STRING;
	std::string fldName3 = FLD_NAME_COMPONENT_FEATURE_CLASS_BITFIELD;
	std::string fldName4 = FLD_NAME_COMPONENT_STATE_BITFIELD;
	std::string fldName5 = FLD_NAME_COMPONENT_SUBFIELD;

	const char* fld[6];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();
	fld[3] = fldName3.c_str();
	fld[4] = fldName4.c_str();
	fld[5] = fldName5.c_str();

	plhs = mxCreateStructArray(ndim, dims, 6, fld);

	arr = NULL;
	jvxApiString fldStr;

	res = involvedHost.hHost->description_component_system(tp, idx, &fldStr);
	if(res == JVX_NO_ERROR)
	{
		std::string token = fldStr.std_str();
		mexReturnString(arr, token);
	}
	mxSetFieldByNumber(plhs, 0, 0, arr);

	// ==========

	arr = NULL;
	res = involvedHost.hHost->descriptor_component_system(tp, idx, &fldStr);
	if(res == JVX_NO_ERROR)
	{
		std::string token = fldStr.std_str();
		mexReturnString(arr, token);
	}
	mxSetFieldByNumber(plhs, 0, 1, arr);

		// ==========

	arr = NULL;
	res = involvedHost.hHost->module_reference_component_system(tp, idx, &fldStr, &acTp);
	if(res == JVX_NO_ERROR)
	{
		std::string token = fldStr.std_str();
		token += "<";
		token += jvxComponentAccessType_txt(acTp);
		token += ">";
		mexReturnString(arr, token);
	}
	mxSetFieldByNumber(plhs, 0, 2, arr);

	arr = NULL;
	res = involvedHost.hHost->feature_class_component_system(tp, idx, &btFld);
	if(res == JVX_NO_ERROR)
	{
		mexReturnBitField(arr, btFld);
	}
	mxSetFieldByNumber(plhs, 0, 3, arr);

	res = involvedHost.hHost->selection_component(tp, &selId);
	if(idx == selId)
	{
		arr= NULL;
		res = involvedHost.hHost->state_selected_component(tp, &theState);
		assert(res == JVX_NO_ERROR);
		mexReturnBitField(arr, theState);
	}
	else
	{
		theState = JVX_STATE_NONE;
		mexReturnBitField(arr, theState);
	}
	mxSetFieldByNumber(plhs, 0, 4, arr);

	arr= NULL;
	mxSetFieldByNumber(plhs, 0, 5, arr);
}

void
mexJvxHost::mexReturnStructComponentReady(mxArray*& plhs, const jvxComponentIdentification& tp)
{
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;
	jvxBool isReady = false;
	jvxApiString fldStr;

	std::string fldName0 = FLD_NAME_COMPONENT_ISREADY_BOOL;
	std::string fldName1 = FLD_NAME_COMPONENT_REASON_IF_NOT_READY_STRING;

	const char* fld[2];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();

	plhs = mxCreateStructArray(ndim, dims, 2, fld);
	arr = NULL;
	isReady = false;
	std::string reason = "";

	res = involvedHost.hHost->is_ready_selected_component(tp, &isReady, &fldStr);
	reason = fldStr.std_str();

	mexReturnBool(arr, isReady);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	mexReturnString(arr, reason);
	mxSetFieldByNumber(plhs, 0, 1, arr);
}

void
mexJvxHost::mexReturnStructComponentStatus(mxArray*& plhs, const jvxComponentIdentification& tp)
{
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;
	jvxBool isReady = false;

	std::string fldName0 = FLD_NAME_COMPONENT_STATE_BITFIELD;
	std::string fldName1 = FLD_NAME_COMPONENT_FEATURE_CLASS_BITFIELD;

	const char* fld[2];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();

	plhs = mxCreateStructArray(ndim, dims, 2, fld);
	arr = NULL;
	jvxBitField btFld = 0;

	res = involvedHost.hHost->state_selected_component(tp, &theState);
	res = involvedHost.hHost->feature_class_selected_component(tp, &btFld);

	mexReturnBitField(arr, theState);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	mexReturnBitField(arr, btFld);
	mxSetFieldByNumber(plhs, 0, 1, arr);
}

void
mexJvxHost::mexReturnStructCommandsComponents(mxArray*& plhs, const jvxComponentIdentification& tp)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;

	const char** fld = NULL;
	std::string* fldStr = NULL;

	IjvxObject* theObj = NULL;
	jvxHandle* theHdl = NULL;
	IjvxSequencerControl* theControl = NULL;

	res = involvedHost.hHost->request_object_selected_component(tp, &theObj);
	if(res == JVX_NO_ERROR && theObj)
	{
		theObj->request_specialization(&theHdl, NULL, NULL);
		if(theHdl)
		{
			switch(tp.tp)
			{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
				((IjvxTechnology*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
				((IjvxDevice*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
				((IjvxNode*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
			}

			if(theControl)
			{
				theControl->number_commands(&num);

				if(num > 0)
				{
					JVX_SAFE_NEW_FLD(fld, const char*, num);
					JVX_SAFE_NEW_FLD(fldStr, std::string, num);

					for(i = 0; i < num; i++)
					{
						fldStr[i]= std::string(PREFIX_ONE_COMMAND) + "_" + jvx_size2String(i);
						fld[i] = fldStr[i].c_str();
					}

					plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

					JVX_SAFE_DELETE_FLD(fld, const char*);
					JVX_SAFE_DELETE_FLD(fldStr, std::string);

					for(i = 0; i < num; i++)
					{
						arr = NULL;
						mexReturnStructOneCommand(arr, theControl, theObj, i);

						mxSetFieldByNumber(plhs, 0, (int)i, arr);
					}
				}
				else
				{
					mexFillEmpty(&plhs, 1, 0);
				}

				switch(tp.tp)
				{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
					((IjvxTechnology*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
					((IjvxDevice*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
					((IjvxNode*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
				}
			}
			else
			{
				mexFillEmpty(&plhs, 1, 0);
			}
		}
		else
		{
			assert(0);
		}
		involvedHost.hHost->return_object_selected_component(tp, theObj);
	}	
	else
	{
		mexFillEmpty(&plhs, 1, 0);
	}
}

void
mexJvxHost::mexReturnStructOneCommand(mxArray*& plhs, IjvxSequencerControl* theControl, IjvxObject* theObj, jvxSize idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;

	jvxSize uniqueId = 0;
	jvxApiString description;
	jvxCBitField stateValid = 0;

	std::string fldName0 = FLD_NAME_COMMAND_ID_INT32;
	std::string fldName1 = FLD_NAME_COMMAND_DESCRIPTION_STRING;
	std::string fldName2 = FLD_NAME_COMMAND_STATE_ALLOW_BITFIELD;

	const char* fld[3];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();

	plhs = mxCreateStructArray(ndim, dims, 3, fld);
	arr = NULL;
	
	res = theControl->description_command(idx, &uniqueId, &description, &stateValid);
	assert(res == JVX_NO_ERROR);

	mexReturnInt32(arr, (jvxInt32)uniqueId);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	mexReturnString(arr, description.std_str());

	mxSetFieldByNumber(plhs, 0, 1, arr);
	mexReturnCBitField(arr, stateValid);
	mxSetFieldByNumber(plhs, 0, 2, arr);
}


void
mexJvxHost::mexReturnStructConditionsComponents(mxArray*& plhs, const jvxComponentIdentification& tp)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;

	const char** fld = NULL;
	std::string* fldStr = NULL;

	IjvxObject* theObj = NULL;
	jvxHandle* theHdl = NULL;
	IjvxSequencerControl* theControl = NULL;

	res = involvedHost.hHost->request_object_selected_component(tp, &theObj);
	if(res == JVX_NO_ERROR && theObj)
	{
		theObj->request_specialization(&theHdl, NULL, NULL);
		if(theHdl)
		{
			switch(tp.tp)
			{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
				((IjvxTechnology*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
				((IjvxDevice*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
				((IjvxNode*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
			}

			if(theControl)
			{
				theControl->number_conditions(&num);

				if(num > 0)
				{
					JVX_SAFE_NEW_FLD(fld, const char*, num);
					JVX_SAFE_NEW_FLD(fldStr, std::string, num);

					for(i = 0; i < num; i++)
					{
						fldStr[i]= std::string(PREFIX_ONE_COMMAND) + "_" + jvx_size2String(i);
						fld[i] = fldStr[i].c_str();
					}

					plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

					JVX_SAFE_DELETE_FLD(fld, const char*);
					JVX_SAFE_DELETE_FLD(fldStr, std::string);

					for(i = 0; i < num; i++)
					{
						arr = NULL;
						mexReturnStructOneCondition(arr, theControl, theObj, i);

						mxSetFieldByNumber(plhs, 0, (int)i, arr);
					}
				}
				else
				{
					mexFillEmpty(&plhs, 1, 0);
				}

				switch(tp.tp)
				{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
					((IjvxTechnology*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
					((IjvxDevice*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
					((IjvxNode*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
				}
			}
			else
			{
				mexFillEmpty(&plhs, 1, 0);
			}
		}
		else
		{
			assert(0);
		}
		involvedHost.hHost->return_object_selected_component(tp, theObj);
	}	
	else
	{
		mexFillEmpty(&plhs, 1, 0);
	}
}

void
mexJvxHost::mexReturnStructOneCondition(mxArray*& plhs, IjvxSequencerControl* theControl, IjvxObject* theObj, jvxSize idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;

	jvxSize uniqueId = 0;
	jvxApiString description;
	jvxUInt64 stateValid = 0;

	std::string fldName0 = FLD_NAME_CONDITION_ID_INT32;
	std::string fldName1 = FLD_NAME_CONDITION_DESCRIPTION_STRING;
	std::string fldName2 = FLD_NAME_CONDITION_STATE_ALLOW_BITFIELD;

	const char* fld[3];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();

	plhs = mxCreateStructArray(ndim, dims, 3, fld);
	arr = NULL;
	
	res = theControl->description_condition(idx, &uniqueId, &description, &stateValid);
	assert(res == JVX_NO_ERROR);

	mexReturnInt32(arr, (jvxInt32)uniqueId);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	mexReturnString(arr, description.std_str());
	mxSetFieldByNumber(plhs, 0, 1, arr);
	mexReturnBitField(arr, stateValid);
	mxSetFieldByNumber(plhs, 0, 2, arr);
}

void
mexJvxHost::mexReturnStructRelativeJumpsComponents(mxArray*& plhs, const jvxComponentIdentification& tp)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;

	const char** fld = NULL;
	std::string* fldStr = NULL;

	IjvxObject* theObj = NULL;
	jvxHandle* theHdl = NULL;
	IjvxSequencerControl* theControl = NULL;

	res = involvedHost.hHost->request_object_selected_component(tp, &theObj);
	if(res == JVX_NO_ERROR && theObj)
	{
		theObj->request_specialization(&theHdl, NULL, NULL);
		if(theHdl)
		{
			switch(tp.tp)
			{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
				((IjvxTechnology*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
				((IjvxDevice*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
				((IjvxNode*)theHdl)->request_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle**>(&theControl));
				break;
			}

			if(theControl)
			{
				theControl->number_relative_jumps(&num);

				if(num > 0)
				{
					JVX_SAFE_NEW_FLD(fld, const char*, num);
					JVX_SAFE_NEW_FLD(fldStr, std::string, num);

					for(i = 0; i < num; i++)
					{
						fldStr[i]= std::string(PREFIX_ONE_COMMAND) + "_" + jvx_size2String(i);
						fld[i] = fldStr[i].c_str();
					}

					plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

					JVX_SAFE_DELETE_FLD(fld, const char*);
					JVX_SAFE_DELETE_FLD(fldStr, std::string);

					for(i = 0; i < num; i++)
					{
						arr = NULL;
						mexReturnStructOneRelJump(arr, theControl, theObj, i);

						mxSetFieldByNumber(plhs, 0, (int)i, arr);
					}
				}
				else
				{
					mexFillEmpty(&plhs, 1, 0);
				}

				switch(tp.tp)
				{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
					((IjvxTechnology*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
					((IjvxDevice*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
					((IjvxNode*)theHdl)->return_hidden_interface(JVX_INTERFACE_SEQUENCERCONTROL, reinterpret_cast<jvxHandle*>(theControl));
					break;
				}
			}
			else
			{
				mexFillEmpty(&plhs, 1, 0);
			}
		}
		else
		{
			assert(0);
		}
		involvedHost.hHost->return_object_selected_component(tp, theObj);
	}	
	else
	{
		mexFillEmpty(&plhs, 1, 0);
	}
}

void
mexJvxHost::mexReturnStructOneRelJump(mxArray*& plhs, IjvxSequencerControl* theControl, IjvxObject* theObj, jvxSize idx)
{
	jvxErrorType res = JVX_NO_ERROR;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	jvxSize num = 0;
	plhs = NULL;
	mxArray* arr = NULL;

	jvxSize uniqueId = 0;
	jvxApiString description;
	jvxUInt64 stateValid = 0;

	std::string fldName0 = FLD_NAME_RELJUMP_ID_INT32;
	std::string fldName1 = FLD_NAME_RELJUMP_DESCRIPTION_STRING;
	std::string fldName2 = FLD_NAME_RELJUMP_STATE_ALLOW_BITFIELD;

	const char* fld[3];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();

	plhs = mxCreateStructArray(ndim, dims, 3, fld);
	arr = NULL;
	
	res = theControl->description_relative_jump(idx, &uniqueId, &description, &stateValid);
	assert(res == JVX_NO_ERROR);

	mexReturnInt32(arr, (jvxInt32)uniqueId);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	mexReturnString(arr, description.std_str());
	mxSetFieldByNumber(plhs, 0, 1, arr);
	mexReturnBitField(arr, stateValid);
	mxSetFieldByNumber(plhs, 0, 2, arr);
}

// =======================================================================
// CONNECTION PROCESSES
// =======================================================================

void
mexJvxHost::mexReturnStructConnectionProcesses(mxArray*& plhs, const std::string& nmProcess, jvxSize idProcess)
{
	jvxSize i;
	jvxSize num = 0;
	std::vector<jvxSize> validIds;

	IjvxDataConnections* theDataConnections = NULL;
	plhs = NULL;

	const char** fld = NULL;
	std::string* fldStr = NULL;

	involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));

	assert(theDataConnections);

	theDataConnections->number_connections_process(&num);
	for (i = 0; i < num; i++)
	{
		IjvxDataConnectionProcess* datProc = NULL;
		theDataConnections->reference_connection_process(i, &datProc);
		assert(datProc);

		if (!nmProcess.empty())
		{
			jvxApiString strRet;
			datProc->descriptor_connection(&strRet);

			if (jvx_compareStringsWildcard(nmProcess, strRet.std_str()))
			{
				validIds.push_back(i);
			}
		}
		else if (JVX_CHECK_SIZE_SELECTED(idProcess))
		{
			if (i == idProcess)
			{
				validIds.push_back(i);
			}
		}
		else
		{
			validIds.push_back(i);
		}
		theDataConnections->return_reference_connection_process(datProc);
	}


	if (validIds.size())
	{
		num = validIds.size();
		SZ_MAT_TYPE ndim = 2;
		SZ_MAT_TYPE dims[2] = { 1, 1 };

		plhs = NULL;
		mxArray* arr = NULL;

		JVX_SAFE_NEW_FLD(fld, const char*, num);
		JVX_SAFE_NEW_FLD(fldStr, std::string, num);

		for (i = 0; i < num; i++)
		{
			fldStr[i] = std::string(PREFIX_ONE_CONNECTION_PROCESS) + "_" + jvx_size2String(i);
			fld[i] = fldStr[i].c_str();
		}

		plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

		JVX_SAFE_DELETE_FLD(fld, const char*);
		JVX_SAFE_DELETE_FLD(fldStr, std::string);

		for (i = 0; i < num; i++)
		{
			IjvxDataConnectionProcess* datProc = NULL;
			theDataConnections->reference_connection_process(validIds[i], &datProc);
			assert(datProc);

			arr = NULL;
			mexReturnStructOneConnectionProcess(arr, i, datProc);
			mxSetFieldByNumber(plhs, 0, (int)i, arr);

			theDataConnections->return_reference_connection_process(datProc);
		}
	}
	else
	{
		mexFillEmpty(&plhs, 1, 0);
	}

	involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theDataConnections));
}

void
mexJvxHost::mexReturnStructOneConnectionProcess(mxArray*& plhs, jvxSize mxId, IjvxDataConnectionProcess* theDataConnectionProc)
{
	jvxApiString fldStr;
	jvxApiString selFac;
	jvxApiString selMasCon;
	std::string name;
	jvxSize ruleId= JVX_SIZE_UNSELECTED;
	jvxComponentIdentification cpId;
	IjvxConnectionMaster* master = NULL;
	mxArray* arr = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numFlds = 9;
	const char** fld = NULL;
	jvxErrorType resL = JVX_NO_ERROR;
	CjvxJsonElementList jsonElmLst;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdbList);
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdbTst);
	std::string txt;
	jvxBool testok = false;
	jvxBool isAutoSetup = false;
	jvxSize uId = JVX_SIZE_UNSELECTED;

	res = theDataConnectionProc->descriptor_connection(&fldStr);	
	if (res == JVX_NO_ERROR)
	{
		name = fldStr.std_str();
	}

	theDataConnectionProc->misc_connection_parameters(&ruleId, nullptr);

	theDataConnectionProc->unique_id_connections(&uId);

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, const char*, numFlds);

	fld[0] = FLD_NAME_CONNECTION_PROCESS_DESCRIPTION;
	fld[1] = FLD_NAME_CONNECTION_PROCESS_UID;
	fld[2] = FLD_NAME_CONNECTION_PROCESS_PROC_IS_AUTO_SETUP;
	fld[3] = FLD_NAME_CONNECTION_PROCESS_MASTER;
	fld[4] = FLD_NAME_CONNECTION_PROCESS_BRIDGES;
	fld[5] = FLD_NAME_CONNECTION_PROCESS_PATH_DESCRIPTION;
	fld[6] = FLD_NAME_CONNECTION_PROCESS_TEST_SUCCESS;
	fld[7] = FLD_NAME_CONNECTION_PROCESS_CHAIN_OK;
	fld[8] = FLD_NAME_CONNECTION_PROCESS_REASON_IF_NOT_READY;

	plhs = mxCreateStructArray(ndim, dims, numFlds, fld);
	JVX_DSP_SAFE_DELETE_FIELD(fld);
	fld = NULL;

	arr = NULL;
	mexReturnString(arr, name);
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	mexReturnInt64(arr, uId);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	isAutoSetup = false;
	if (JVX_CHECK_SIZE_SELECTED(isAutoSetup))
		isAutoSetup = true;

	arr = NULL;
	mexReturnBool(arr, isAutoSetup);
	mxSetFieldByNumber(plhs, 0, 2, arr);

	theDataConnectionProc->associated_master(&master);
	arr = NULL;
	mexReturnConnectionAssociatedMaster(arr, master);
	mxSetFieldByNumber(plhs, 0, 3, arr);

	arr = NULL;
	mexReturnConnectionProcessBridges(arr, theDataConnectionProc);
	mxSetFieldByNumber(plhs, 0, 4, arr);

	arr = NULL;
	resL = theDataConnectionProc->transfer_forward_chain(JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON, &jsonElmLst JVX_CONNECTION_FEEDBACK_CALL_A(fdbList));
	assert(resL == JVX_NO_ERROR);
	jsonElmLst.printToStringView(txt);
	mexReturnString(arr, txt);
	mxSetFieldByNumber(plhs, 0, 5, arr);

	arr = NULL;
	//theDataConnectionProc->check_process_ready(&)
	theDataConnectionProc->status_chain(&testok JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
	mexReturnBool(arr, testok);
	mxSetFieldByNumber(plhs, 0, 6, arr);

	arr = NULL;
	testok = false;
	resL = theDataConnectionProc->check_process_ready(&fldStr);
	if (resL == JVX_NO_ERROR)
	{
		testok = true;
	}
	mexReturnBool(arr, testok);
	mxSetFieldByNumber(plhs, 0, 7, arr);

	arr = NULL;
	mexReturnString(arr, fldStr.std_str());
	mxSetFieldByNumber(plhs, 0, 8, arr);
}

void 
mexJvxHost::mexReturnConnectionAssociatedMaster(mxArray*& plhs, IjvxConnectionMaster* master)
{
	mxArray* arr = NULL;
	jvxSize numFlds = 4;
	const char** fld = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification tpId;
	jvxApiString strMF;
	jvxApiString strM;
	jvxApiString strMD;
	IjvxConnectionMasterFactory* myPar = NULL;

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, const char*, numFlds);

	fld[0] = FLD_NAME_CONNECTION_PROCESS_MASTER_NAME;
	fld[1] = FLD_NAME_CONNECTION_PROCESS_MASTER_DESCRIPTION;
	fld[2] = FLD_NAME_CONNECTION_PROCESS_MASTER_FACTORY_COMP_ID;
	fld[3] = FLD_NAME_CONNECTION_PROCESS_MASTER_FACTORY_NAME;

	plhs = mxCreateStructArray(ndim, dims, numFlds, fld);
	JVX_DSP_SAFE_DELETE_FIELD(fld);
	fld = NULL;

	arr = NULL;
	master->name_master(&strM);
	mexReturnString(arr, strM.std_str());
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	master->descriptor_master(&strMD);;
	mexReturnString(arr, strMD.std_str());
	mxSetFieldByNumber(plhs, 0, 1, arr);


	master->parent_master_factory(&myPar);
	assert(myPar);
	jvx_request_interfaceToObject(myPar, NULL, &tpId, &strMF);
	
	arr = NULL;
	mexReturnComponentIdentification(arr, tpId);
	mxSetFieldByNumber(plhs, 0, 2, arr);

	arr = NULL;
	mexReturnString(arr, strMF.std_str());
	mxSetFieldByNumber(plhs, 0, 3, arr);
}

void 
mexJvxHost::mexReturnConnectionProcessBridges(mxArray*& plhs, IjvxDataConnectionProcess* theDataConnectionProc)
{
	jvxSize num = 0;
	jvxSize i;
	const char** fld = NULL;
	std::string* fldStr = NULL;

	theDataConnectionProc->number_bridges(&num);
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	plhs = NULL;
	mxArray* arr = NULL;

	JVX_SAFE_NEW_FLD(fld, const char*, num);
	JVX_SAFE_NEW_FLD(fldStr, std::string, num);

	for (i = 0; i < num; i++)
	{
		fldStr[i] = std::string(PREFIX_ONE_CONNECTION_PROCESS_BRIDGE) + "_" + jvx_size2String(i);
		fld[i] = fldStr[i].c_str();
	}

	plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

	JVX_SAFE_DELETE_FLD(fld, const char*);
	JVX_SAFE_DELETE_FLD(fldStr, std::string);

	for (i = 0; i < num; i++)
	{
		IjvxConnectorBridge* theBridge = NULL;

		arr = NULL;
		theDataConnectionProc->reference_bridge(i, &theBridge);
		assert(theBridge);

		mexReturnStructOneConnectionProcessBridge(arr, theBridge);
		mxSetFieldByNumber(plhs, 0, (int)i, arr);


		theDataConnectionProc->return_reference_bridge(theBridge);
	}
}

void
mexJvxHost::mexReturnStructOneConnectionProcessBridge(mxArray*& plhs, IjvxConnectorBridge* theBridge)
{
	jvxSize numFlds = 3;
	const char** fld = NULL;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	plhs = NULL;
	mxArray* arr = NULL;

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, const char*, numFlds);

	fld[0] = FLD_NAME_CONNECTION_PROCESS_BRIDGE_DESCRIPTOR;
	fld[1] = FLD_NAME_CONNECTION_PROCESS_BRIDGE_OUTPUT_CONNECTOR;
	fld[2] = FLD_NAME_CONNECTION_PROCESS_BRIDGE_INPUT_CONNECTOR;

	plhs = mxCreateStructArray(ndim, dims, numFlds, fld);
	JVX_DSP_SAFE_DELETE_FIELD(fld);
	fld = NULL;


	jvxApiString strB;
	IjvxOutputConnector* theFromRef = NULL;
	IjvxInputConnector* theToRef = NULL;

	arr = NULL;
	theBridge->descriptor_bridge(&strB);
	mexReturnString(arr, strB.std_str());
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	theBridge->reference_connect_from(&theFromRef);
	mexReturnOutputConnector(arr, theFromRef);
	mxSetFieldByNumber(plhs, 0, 1, arr);
	theBridge->return_reference_connect_from(theFromRef);

	arr = NULL;
	theBridge->reference_connect_to(&theToRef);
	mexReturnInputConnector(arr, theToRef);
	mxSetFieldByNumber(plhs, 0, 2, arr);
	theBridge->return_reference_connect_to(theToRef);

}

void
mexJvxHost::mexReturnOutputConnector(mxArray*& plhs, IjvxOutputConnector* theOCon)
{
	mxArray* arr = NULL;
	jvxSize numFlds = 3;
	const char** fld = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification tpId;
	jvxApiString strC;
	jvxApiString strF;
	IjvxConnectorFactory* myPar = NULL;

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, const char*, numFlds);

	fld[0] = FLD_NAME_CONNECTION_PROCESS_OUTPUT_CONNECTOR_DESCRIPTION;
	fld[1] = FLD_NAME_CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_COMP_ID;
	fld[2] = FLD_NAME_CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_NAME;

	plhs = mxCreateStructArray(ndim, dims, numFlds, fld);
	JVX_DSP_SAFE_DELETE_FIELD(fld);
	fld = NULL;

	arr = NULL;
	theOCon->descriptor_connector(&strC);
	mexReturnString(arr, strC.std_str());
	mxSetFieldByNumber(plhs, 0, 0, arr);

	theOCon->parent_factory(&myPar);
	assert(myPar);
	jvx_request_interfaceToObject(myPar, NULL, &tpId, &strF);

	arr = NULL;
	mexReturnComponentIdentification(arr, tpId);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	arr = NULL;
	mexReturnString(arr, strF.std_str());
	mxSetFieldByNumber(plhs, 0, 2, arr);
}

void
mexJvxHost::mexReturnInputConnector(mxArray*& plhs, IjvxInputConnector* theICon)
{
	mxArray* arr = NULL;
	jvxSize numFlds = 3;
	const char** fld = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification tpId;
	jvxApiString strC;
	jvxApiString strF;
	IjvxConnectorFactory* myPar = NULL;

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, const char*, numFlds);

	fld[0] = FLD_NAME_CONNECTION_PROCESS_INPUT_CONNECTOR_DESCRIPTION;
	fld[1] = FLD_NAME_CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_COMP_ID;
	fld[2] = FLD_NAME_CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_NAME;

	plhs = mxCreateStructArray(ndim, dims, numFlds, fld);
	JVX_DSP_SAFE_DELETE_FIELD(fld);
	fld = NULL;

	arr = NULL;
	theICon->descriptor_connector(&strC);
	mexReturnString(arr, strC.std_str());
	mxSetFieldByNumber(plhs, 0, 0, arr);

	theICon->parent_factory(&myPar);
	assert(myPar);
	jvx_request_interfaceToObject(myPar, NULL, &tpId, &strF);

	arr = NULL;
	mexReturnComponentIdentification(arr, tpId);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	arr = NULL;
	mexReturnString(arr, strF.std_str());
	mxSetFieldByNumber(plhs, 0, 2, arr);
}

// =======================================================================
// CONNECTION RULES
// =======================================================================

void 
mexJvxHost::mexReturnStructConnectionRules(mxArray*& plhs, const std::string& nmRule, jvxSize idRule)
{
	jvxSize i;
	jvxSize num = 0;
	std::vector<jvxSize> validIds;

	IjvxDataConnections* theDataConnections = NULL;
	plhs = NULL;

	const char** fld = NULL;
	std::string* fldStr = NULL;

	involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theDataConnections));

	assert(theDataConnections);

	theDataConnections->number_connection_rules(&num);
	for (i = 0; i < num; i++)
	{
		IjvxDataConnectionRule* datRule = NULL;
		theDataConnections->reference_connection_rule(i, &datRule);
		assert(datRule);

		if (!nmRule.empty())
		{
			jvxApiString strRet;
			datRule->description_rule(&strRet, NULL);

			if (jvx_compareStringsWildcard(nmRule, strRet.std_str()))
			{
				validIds.push_back(i);
			}
		}
		else if (JVX_CHECK_SIZE_SELECTED(idRule))
		{
			if (i == idRule)
			{
				validIds.push_back(i);
			}
		}
		else
		{
			validIds.push_back(i);
		}
		theDataConnections->return_reference_connection_rule(datRule);
	}


	if (validIds.size())
	{
		num = validIds.size();
		SZ_MAT_TYPE ndim = 2;
		SZ_MAT_TYPE dims[2] = { 1, 1 };

		plhs = NULL;
		mxArray* arr = NULL;

		JVX_SAFE_NEW_FLD(fld, const char*, num);
		JVX_SAFE_NEW_FLD(fldStr, std::string, num);

		for (i = 0; i < num; i++)
		{
			fldStr[i] = std::string(PREFIX_ONE_CONNECTION_RULE) + "_" + jvx_size2String(i);
			fld[i] = fldStr[i].c_str();
		}

		plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

		JVX_SAFE_DELETE_FLD(fld, const char*);
		JVX_SAFE_DELETE_FLD(fldStr, std::string);

		for (i = 0; i < num; i++)
		{
			IjvxDataConnectionRule* datRule = NULL;
			theDataConnections->reference_connection_rule(validIds[i], &datRule);
			assert(datRule);

			arr = NULL;
			mexReturnStructOneConnectionRule(arr, i, datRule);
			mxSetFieldByNumber(plhs, 0, (int)i, arr);

			theDataConnections->return_reference_connection_rule(datRule);
		}
	}
	else
	{
		mexFillEmpty(&plhs, 1, 0);
	}

	involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theDataConnections));
}

void 
mexJvxHost::mexReturnStructOneConnectionRule(mxArray*& plhs, jvxSize mxId, IjvxDataConnectionRule* theDataConnectionRule)
{
	jvxApiString fldStr;
	jvxApiString selFac;
	jvxApiString selMasCon;
	std::string name;
	jvxBool isDefaultRule = false;
	jvxComponentIdentification cpId;
	mxArray* arr = NULL;
	jvxErrorType res = theDataConnectionRule->description_rule(&fldStr, &isDefaultRule);
	jvxSize numFlds = 4;
	const char** fld = NULL;

	if (res == JVX_NO_ERROR)
	{
		name = fldStr.std_str();
	}

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, const char*, numFlds);

	fld[0] = FLD_NAME_CONNECTION_RULE_NAME;
	fld[1] = FLD_NAME_CONNECTION_RULE_IS_DEFAULT;
	fld[2] = FLD_NAME_CONNECTION_RULE_MASTER;
	fld[3] = FLD_NAME_CONNECTION_RULE_BRIDGES;

	plhs = mxCreateStructArray(ndim, dims, numFlds, fld);
	JVX_DSP_SAFE_DELETE_FIELD(fld);
	fld = NULL;

	arr = NULL;
	mexReturnString(arr, name);
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	mexReturnBool(arr, isDefaultRule);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	theDataConnectionRule->get_master(&cpId, &selFac, &selMasCon);
	arr = NULL;
	mexReturnConnectionRuleMaster(arr, cpId, selFac.std_str(), selMasCon.std_str());
	mxSetFieldByNumber(plhs, 0, 2, arr);

	arr = NULL;
	mexReturnConnectionRuleBridges(arr, theDataConnectionRule);
	mxSetFieldByNumber(plhs, 0, 3, arr);
}

void 
mexJvxHost::mexReturnConnectionRuleMaster(mxArray*& plhs, const jvxComponentIdentification& cpId,
	const std::string& selFac, const std::string& selMasCon)
{
	std::string name;
	mxArray* arr = NULL;
	jvxSize numFlds = 3;
	const char** fld = NULL;

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	JVX_DSP_SAFE_ALLOCATE_FIELD(fld, const char*, numFlds);

	fld[0] = FLD_NAME_CONNECTION_RULE_MASTER_COMP_ID;
	fld[1] = FLD_NAME_CONNECTION_RULE_MASTER_FAC_SEL;
	fld[2] = FLD_NAME_CONNECTION_RULE_MASTER_MAS_SEL;

	plhs = mxCreateStructArray(ndim, dims, numFlds, fld);

	JVX_DSP_SAFE_DELETE_FIELD(fld);
	fld = NULL;

	arr = NULL;
	mexReturnComponentIdentification(arr, cpId);
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	mexReturnString(arr, selFac);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	arr = NULL;
	mexReturnString(arr, selMasCon);
	mxSetFieldByNumber(plhs, 0, 2, arr);
}

void
mexJvxHost::mexReturnConnectionRuleBridges(mxArray*& plhs, IjvxDataConnectionRule* theConRul)
{
	jvxSize num = 0;
	const char** fld = NULL;
	std::string* fldStr = NULL;
	jvxSize i;

	theConRul->number_bridges(&num);
	if (num > 0)
	{
		SZ_MAT_TYPE ndim = 2;
		SZ_MAT_TYPE dims[2] = { 1, 1 };

		plhs = NULL;
		mxArray* arr = NULL;

		JVX_SAFE_NEW_FLD(fld, const char*, num);
		JVX_SAFE_NEW_FLD(fldStr, std::string, num);

		for (i = 0; i < num; i++)
		{
			fldStr[i] = std::string(PREFIX_ONE_RULE_BRIDGE) + "_" + jvx_size2String(i);
			fld[i] = fldStr[i].c_str();
		}

		plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

		JVX_SAFE_DELETE_FLD(fld, const char*);
		JVX_SAFE_DELETE_FLD(fldStr, std::string);
		for (i = 0; i < num; i++)
		{
			arr = NULL;
			mexReturnStructOneConnectionRuleBridge(arr, i, theConRul);
			mxSetFieldByNumber(plhs, 0, (int)i, arr);
		}
	}
	else
	{
		mexFillEmpty(&plhs, 1, 0);
	}
}

void 
mexJvxHost::mexReturnStructOneConnectionRuleBridge(mxArray*& plhs, jvxSize id, IjvxDataConnectionRule* theConRul)
{
	mxArray* arr = NULL;
	jvxSize numFlds = 7;
	const char** fld = NULL;

	plhs = NULL;

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	JVX_DSP_SAFE_ALLOCATE_FIELD(fld, const char*, numFlds);

	fld[0] = FLD_NAME_CONNECTION_RULE_BRIDGE_NAME;
	fld[1] = FLD_NAME_CONNECTION_RULE_BRIDGE_FROM_COMP_ID;
	fld[2] = FLD_NAME_CONNECTION_RULE_BRIDGE_FROM_FAC_SEL;
	fld[3] = FLD_NAME_CONNECTION_RULE_BRIDGE_FROM_SEL;
	fld[4] = FLD_NAME_CONNECTION_RULE_BRIDGE_TO_COMP_ID;
	fld[5] = FLD_NAME_CONNECTION_RULE_BRIDGE_TO_FAC_SEL;
	fld[6] = FLD_NAME_CONNECTION_RULE_BRIDGE_TO_SEL;

	plhs = mxCreateStructArray(ndim, dims, numFlds, fld);

	JVX_DSP_SAFE_DELETE_FIELD(fld);
	fld = NULL;
	
	jvxApiString nm; 

	jvxComponentIdentification cp_id_from;
	jvxApiString nm_fac_from;
	jvxApiString nm_from;
	jvxComponentIdentification cp_id_to;
	jvxApiString nm_fac_to;
	jvxApiString nm_to;

	theConRul->get_bridge(id, &nm, &cp_id_from, &nm_fac_from, &nm_from, &cp_id_to, &nm_fac_to, &nm_to);

	arr = NULL;
	mexReturnString(arr, nm.std_str());
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	mexReturnComponentIdentification(arr, cp_id_from);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	arr = NULL;
	mexReturnString(arr, nm_fac_from.std_str());
	mxSetFieldByNumber(plhs, 0, 2, arr);

	arr = NULL;
	mexReturnString(arr, nm_from.std_str());
	mxSetFieldByNumber(plhs, 0, 3, arr);

	arr = NULL;
	mexReturnComponentIdentification(arr, cp_id_to);
	mxSetFieldByNumber(plhs, 0, 4, arr);

	arr = NULL;
	mexReturnString(arr, nm_fac_to.std_str());
	mxSetFieldByNumber(plhs, 0, 5, arr);

	arr = NULL;
	mexReturnString(arr, nm_to.std_str());
	mxSetFieldByNumber(plhs, 0, 6, arr);


}

// ========================================================================================

void
mexJvxHost::mexReturnStructSequencer(mxArray*& plhs)
{
	jvxSize i;
	jvxSize num = 0;
	IjvxSequencer* theSequencer = NULL;
	plhs = NULL;

	const char** fld = NULL;
	std::string* fldStr = NULL;

	involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));

	if(theSequencer)
	{
		theSequencer->number_sequences(&num);
		if(num > 0)
		{
			SZ_MAT_TYPE ndim = 2;
			SZ_MAT_TYPE dims[2] = {1, 1};

			plhs = NULL;
			mxArray* arr = NULL;
			jvxState theState = JVX_STATE_NONE;

			JVX_SAFE_NEW_FLD(fld, const char*, num);
			JVX_SAFE_NEW_FLD(fldStr, std::string, num);

			for(i = 0; i < num; i++)
			{
				fldStr[i]= std::string(PREFIX_ONE_SEQUENCE) + "_" + jvx_size2String(i);
				fld[i] = fldStr[i].c_str();
			}

			plhs = mxCreateStructArray(ndim, dims, (int)num, fld);


			JVX_SAFE_DELETE_FLD(fld, const char*);
			JVX_SAFE_DELETE_FLD(fldStr, std::string);
			for(i = 0; i < num; i++)
			{
				arr = NULL;
				mexReturnStructOneSequence(arr, i, theSequencer, involvedComponents.theHost.hobject);
				mxSetFieldByNumber(plhs, 0, (int)i, arr);

			}
		}
		else
		{
			mexFillEmpty(&plhs, 1, 0);
		}

		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));
	}
}

void
mexJvxHost::mexReturnStructOneSequence(mxArray*& plhs, jvxSize idxSequence, IjvxSequencer* theSequencer, IjvxObject* theObj)
{
	jvxApiString fldStr;
	jvxApiString fldStrLab;
	jvxSize numRun = 0;
	jvxSize numLeave = 0;
	jvxBool markedForProcessing = false;
	std::string name;
	std::string label;
	mxArray* arr = NULL;
	jvxBool isDefault = false;
	jvxErrorType res = theSequencer->description_sequence(idxSequence, &fldStr, &fldStrLab, &numRun, &numLeave, &markedForProcessing, &isDefault);
	if(res == JVX_NO_ERROR)
	{
		name = fldStr.std_str();
		label = fldStrLab.std_str();
	}

	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	std::string fldName0 = FLD_NAME_SEQUENCE_DESCRIPTION_STRING;
	std::string fldName1 = FLD_NAME_SEQUENCE_LABEL_STRING;
	std::string fldName2 = FLD_NAME_MARKED_FOR_PROCESS_BOOL;
	std::string fldName3 = FLD_NAME_MARKED_FOR_PROCESS_BOOL;
	std::string fldName4 = FLD_NAME_RUN_STEPS;
	std::string fldName5 = FLD_NAME_LEAVE_STEPS;

	const char* fld[6];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();
	fld[3] = fldName3.c_str();
	fld[4] = fldName4.c_str();
	fld[5] = fldName5.c_str();

	plhs = mxCreateStructArray(ndim, dims, 6, fld);

	arr = NULL;
	mexReturnString(arr, name);
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	mexReturnString(arr, label);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	arr = NULL;
	mexReturnBool(arr, markedForProcessing);
	mxSetFieldByNumber(plhs, 0, 2, arr);

	arr = NULL;
	mexReturnBool(arr, isDefault);
	mxSetFieldByNumber(plhs, 0, 3, arr);

	arr = NULL;
	mexReturnStructOneSequenceStepQueue(arr, idxSequence, JVX_SEQUENCER_QUEUE_TYPE_RUN, numRun, theSequencer, theObj);
	mxSetFieldByNumber(plhs, 0, 4, arr);

	arr = NULL;
	mexReturnStructOneSequenceStepQueue(arr, idxSequence, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, numLeave, theSequencer, theObj);
	mxSetFieldByNumber(plhs, 0, 5, arr);

}


void
mexJvxHost::mexReturnStructOneSequenceStepQueue(mxArray*& plhs, jvxSize idxSequence, jvxSequencerQueueType qtp, jvxSize num,  IjvxSequencer* theSequencer, IjvxObject* theObj)
{
	jvxSize i;
	const char** fld = NULL;
	std::string* fldStr = NULL;
	mxArray* arr = NULL;
	if(num > 0)
	{
		SZ_MAT_TYPE ndim = 2;
		SZ_MAT_TYPE dims[2] = {1, 1};
		JVX_SAFE_NEW_FLD(fld, const char*, num);
		JVX_SAFE_NEW_FLD(fldStr, std::string, num);

		for(i = 0; i < num; i ++)
		{
			fldStr[i] = PREFIX_ONE_SEQUENCE_STEP + jvx_size2String(i);
			fld[i] = fldStr[i].c_str();
		}

		plhs = mxCreateStructArray(ndim, dims, (int)num, fld);

		JVX_SAFE_DELETE_FLD(fld, const char*);
		JVX_SAFE_DELETE_FLD(fldStr, std::string);

		for(i = 0; i < num; i ++)
		{
			arr = NULL;
			mexReturnStructOneSequenceStep(arr, idxSequence, qtp, i, theSequencer, theObj);
			mxSetFieldByNumber(plhs, 0, (int)i, arr);
		}
	}
}

void
mexJvxHost::mexReturnStructOneSequenceStep(mxArray*& plhs, jvxSize idxSequence, jvxSequencerQueueType qtp, jvxSize stepId, IjvxSequencer* theSequencer, IjvxObject* theObj)
{
	jvxApiString fldStr;
	jvxApiString fldLabel;
	jvxApiString fldLabelTrue;
	jvxApiString fldLabelFalse;
	jvxBool b_active = false;
	jvxSequencerElementType elmTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification cpTp;
	jvxSize funcId = JVX_SIZE_UNSELECTED;
	jvxInt64 timeout_ms = 0;
	jvxCBitField assoc_mode = 0;
	jvxErrorType res = theSequencer->description_step_sequence(idxSequence, stepId, qtp, &fldStr, &elmTp, 
		&cpTp, &funcId, &timeout_ms, &fldLabel, &fldLabelTrue, &fldLabelFalse, &b_active, &assoc_mode);
	mxArray* arr = NULL;
	std::string descr;
	std::string label;
	std::string label_true;
	std::string label_false;
	if(res == JVX_NO_ERROR)
	{
		descr = fldStr.std_str();
		label= fldLabel.std_str();
		label_true = fldLabelTrue.std_str();
		label_false = fldLabelFalse.std_str();

		SZ_MAT_TYPE ndim = 2;
		SZ_MAT_TYPE dims[2] = {1, 1};

		std::string fldName0 = FLD_NAME_SEQUENCE_STEP_DESCRIPTION_STRING;
		std::string fldName1 = FLD_NAME_SEQUENCE_STEP_ELEMENT_TYPE_INT32;
		std::string fldName2 = FLD_NAME_SEQUENCE_STEP_COMPONENT_TYPE_INT32;
		std::string fldName3 = FLD_NAME_SEQUENCE_STEP_FUNCTION_ID_INT32;
		std::string fldName4 = FLD_NAME_SEQUENCE_STEP_TIMEOUT_MS_INT64;
		std::string fldName5 = FLD_NAME_SEQUENCE_STEP_LABEL_STRING;
		std::string fldName6 = FLD_NAME_SEQUENCE_STEP_LABEL_TRUE_STRING;
		std::string fldName7 = FLD_NAME_SEQUENCE_STEP_LABEL_FALSE_STRING;
		std::string fldName8 = FLD_NAME_SEQUENCE_STEP_BREAK_ACTIVE;
		std::string fldName9 = FLD_NAME_SEQUENCE_STEP_ASSOCIATED_MODE;

		const char* fld[10];
		fld[0] = fldName0.c_str();
		fld[1] = fldName1.c_str();
		fld[2] = fldName2.c_str();
		fld[3] = fldName3.c_str();
		fld[4] = fldName4.c_str();
		fld[5] = fldName5.c_str();
		fld[6] = fldName6.c_str();
		fld[7] = fldName7.c_str();
		fld[8] = fldName8.c_str();
		fld[9] = fldName9.c_str();

		plhs = mxCreateStructArray(ndim, dims, 10, fld);

		arr = NULL;
		mexReturnString(arr, descr);
		mxSetFieldByNumber(plhs, 0, 0, arr);

		arr = NULL;
		mexReturnInt32(arr, elmTp);
		mxSetFieldByNumber(plhs, 0, 1, arr);

		arr = NULL;
		mexReturnInt32(arr, cpTp.tp);
		mxSetFieldByNumber(plhs, 0, 2, arr);

		arr = NULL;
		mexReturnInt32(arr, JVX_SIZE_INT32(funcId));
		mxSetFieldByNumber(plhs, 0, 3, arr);

		arr = NULL;
		mexReturnInt64(arr, timeout_ms);
		mxSetFieldByNumber(plhs, 0, 4, arr);

		arr = NULL;
		mexReturnString(arr, label);
		mxSetFieldByNumber(plhs, 0, 5, arr);

		arr = NULL;
		mexReturnString(arr, label_true);
		mxSetFieldByNumber(plhs, 0, 6, arr);

		arr = NULL;
		mexReturnString(arr, label_false);
		mxSetFieldByNumber(plhs, 0, 7, arr);

		arr = NULL;
		mexReturnBool(arr, b_active);
		mxSetFieldByNumber(plhs, 0, 8, arr);

		arr = NULL;
		mexReturnCBitField(arr, assoc_mode);
		mxSetFieldByNumber(plhs, 0, 9, arr);
	}
}

void
mexJvxHost::mexReturnSequencerStatus(mxArray*& plhs, jvxSequencerStatus status, jvxSize idxSeq, jvxSequencerQueueType qTp, jvxSize idxStep, jvxBool loopEn)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	mxArray* arr = NULL;

	std::string fldName0 = FLD_NAME_PROCESS_STATUS_STATUS_INT32;
	std::string fldName1 = FLD_NAME_PROCESS_STATUS_IDX_SEQ_INT32;
	std::string fldName2 = FLD_NAME_PROCESS_STATUS_QUEUE_TYPE_INT32;
	std::string fldName3 = FLD_NAME_PROCESS_STATUS_IDX_STEP_INT32;
	std::string fldName4 = FLD_NAME_PROCESS_STATUS_LOOP_ENABLED_BOOL;

	const char* fld[5];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();
	fld[3] = fldName3.c_str();
	fld[4] = fldName4.c_str();

	plhs = mxCreateStructArray(ndim, dims, 5, fld);

	arr = NULL;
	mexReturnInt32(arr, (jvxInt32)status);
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	mexReturnInt32(arr, JVX_SIZE_INT32(idxSeq));
	mxSetFieldByNumber(plhs, 0, 1, arr);

	arr = NULL;
	mexReturnInt32(arr, (jvxInt32)qTp);
	mxSetFieldByNumber(plhs, 0, 2, arr);

	arr = NULL;
	mexReturnInt32(arr, JVX_SIZE_INT32(idxStep));
	mxSetFieldByNumber(plhs, 0, 3, arr);

	arr = NULL;
	mexReturnBool(arr, loopEn);
	mxSetFieldByNumber(plhs, 0, 4, arr);
}

void
mexJvxHost::mexReturnStructReadyForStartResult(mxArray*& plhs, jvxBool isReady, const std::string& reasonIfNot)
{
	mxArray* arr = NULL;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	plhs = NULL;

	const char** fld;
	std::string fldName1 = FLD_NAME_COMPONENT_ISREADY_BOOL;
	std::string fldName2 = FLD_NAME_COMPONENT_REASON_IF_NOT_READY_STRING;

	fld = new const char* [2];
	fld[0] = fldName1.c_str();
	fld[1] = fldName2.c_str();

	plhs = mxCreateStructArray(ndim, dims, 2, fld);
	delete[](fld);

	mexReturnBool(arr, isReady);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	arr = NULL;

	mexReturnString(arr, reasonIfNot);
	mxSetFieldByNumber(plhs, 0, 1, arr);
	arr = NULL;
}