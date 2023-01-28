#ifndef _TJVXCONNECTOR_H__
#define _TJVXCONNECTOR_H__

#include "CjvxCallProt.h"

	typedef struct
	{
		jvxSize buffersize;
		jvxSize dimX;
		jvxSize dimY;
		jvxSize number_input_channels;
		jvxSize number_output_channels;
		jvxSize samplerate;
		jvxDataFormat format;
		jvxDataFormatGroup subformat;
	} dedicatedSetup;

	typedef struct
	{
		struct
		{
			jvxSize min;
			jvxSize max;
		} buffersize;

		struct
		{
			jvxSize min;
			jvxSize max;
		} dimX;
		
		struct
		{
			jvxSize min;
			jvxSize max;
		} dimY;
		
		struct
		{
			jvxSize min;
			jvxSize max;
		} number_input_channels;

		struct
		{
			jvxSize min;
			jvxSize max;
		} number_output_channels;

		struct
		{
			jvxSize min;
			jvxSize max;
		} samplerate;

		struct
		{
			jvxDataFormat min;
			jvxDataFormat max;
		} format;
		
		struct
		{
			jvxDataFormatGroup min;
			jvxDataFormatGroup max;
		} subformat;
	} jvxNodePreferredParams;

	typedef struct
	{
		jvxSize buffersize;
		jvxSize dimX;
		jvxSize dimY;
		jvxSize number_input_channels;
		jvxSize number_output_channels;
		jvxSize samplerate;
		jvxDataFormat format;
		jvxDataFormatGroup subformat;
	} jvxNodeProcessingParams;
	
class jvxCommonSetNodeParams
{
public:

	std::vector<dedicatedSetup> allowedSetup;
	jvxNodePreferredParams preferred;
	jvxNodeProcessingParams processing;

	// If old processing interface is used, this controls the param checks
	jvxBool acceptOnlyExactMatchLinkDataInput;
	jvxBool acceptOnlyExactMatchLinkDataOutput;

	// Here, the object can specify some additional fail reasons
	jvxBool settingReadyToStart;
	std::string settingReadyFailReason;
};
	
class jvxCommonSetNodeAutoStart
{
public:
	jvxBool prepareOnChainPrepare;
	jvxBool impPrepareOnChainPrepare;

	jvxBool startOnChainStart;
	jvxBool impStartOnChainStart;

	jvxCommonSetNodeAutoStart()
	{
		prepareOnChainPrepare = true;
		impPrepareOnChainPrepare = false;
		startOnChainStart = true;
		impStartOnChainStart = false;
	};
} ;

#if 0
#define JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb) jvxApiString fdb##__; jvxApiString* fdb = &fdb##__;
#define JVX_CONNECTION_FEEDBACK_TYPE(fdb) jvxApiString* fdb
#define JVX_CONNECTION_FEEDBACK_TYPE_A(fdb) , JVX_CONNECTION_FEEDBACK_TYPE(fdb)
#define JVX_CONNECTION_FEEDBACK_CALL(fdb) fdb
#define JVX_CONNECTION_FEEDBACK_CALL_A(fdb) , fdb
#define JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt) if (fdb) fdb->assign(txt);
#define JVX_CONNECTION_FEEDBACK_GET_ERROR_STRING(fdb) fdb->std_str()
#define JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, txt) if(fdb) { jvxApiString __descr; \
										_common_set_ldslave.object->descriptor(&__descr); \
										std::string __errMess = "Object <"; \
										__errMess += __descr.std_str(); \
										__errMess += ">"; \
										__errMess += ", output Connector <"; \
										__errMess += _common_set_ldslave.descriptor; \
										__errMess += ">: "; \
										__errMess += txt; \
										__errMess += "."; \
										fdb->assign(__errMess); }
#else

#define JVX_CONNECTION_ORIGIN JVX_SOURCE_CODE_ORIGIN

// #define JVX_CONNECTION_ORIGIN __FUNCTION__ ":" Quotes(__LINE__)
// #define JVX_CONNECTION_ORIGIN ""

#define JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb) CjvxCallProt fdb##__; IjvxCallProt* fdb = static_cast<IjvxCallProt*>(&fdb##__);
#define JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_PTR(fdb) IjvxCallProt* fdb
#define JVX_CONNECTION_FEEDBACK_TYPE_INIT_PTR(fdb) fdb = NULL
#define JVX_CONNECTION_FEEDBACK_TYPE_ASSIGN(fdb, fdbfrom) if(fdb) {fdb->duplicateFrom(fdbfrom);}
//#define JVX_CONNECTION_FEEDBACK_TYPE_ALLOCATE(fdb) JVX_DSP_SAFE_ALLOCATE_OBJECT(fdb, CjvxCallProt)
//#define JVX_CONNECTION_FEEDBACK_TYPE_DELETE(fdb) JVX_DSP_SAFE_DELETE_OBJECT(fdb)
#define JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS(fdb) CjvxCallProt fdb##__; IjvxCallProt* fdb;
#define JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS_INIT(fdb) fdb = static_cast<IjvxCallProt*>(&fdb##__);
#define JVX_CONNECTION_FEEDBACK_TYPE(fdb) IjvxCallProt* fdb
#define JVX_CONNECTION_FEEDBACK_TYPE_F(fdb) IjvxCallProt* fdb,
#define JVX_CONNECTION_FEEDBACK_TYPE_A(fdb) , JVX_CONNECTION_FEEDBACK_TYPE(fdb)
#define JVX_CONNECTION_FEEDBACK_CALL(fdb) ((fdb != NULL) ? (fdb->newEntry(JVX_CONNECTION_ORIGIN)): NULL)
#define JVX_CONNECTION_FEEDBACK_CALL_A(fdb) , JVX_CONNECTION_FEEDBACK_CALL(fdb)
#define JVX_CONNECTION_FEEDBACK_REF_CALL(fdb) fdb
#define JVX_CONNECTION_FEEDBACK_REF_CALL_A(fdb) , JVX_CONNECTION_FEEDBACK_REF_CALL(fdb)
#define JVX_CONNECTION_FEEDBACK_CALL_NULL NULL
#define JVX_CONNECTION_FEEDBACK_CALL_A_NULL , NULL
#define JVX_CONNECTION_FEEDBACK_ON_ENTER(fdb, tp, loc) if(fdb) {fdb->setIdentification(tp, loc, JVX_CONNECTION_ORIGIN);}
#define JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, object) jvx_getCallProtObject(fdb, object, NULL, JVX_CONNECTION_ORIGIN)
#define JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_CONN(fdb, object, conn) jvx_getCallProtObject(fdb, object, conn, JVX_CONNECTION_ORIGIN)
#define JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, object, conn, comm) jvx_getCallProtObject(fdb, object, conn, JVX_CONNECTION_ORIGIN, NULL, comm)
#define JVX_CONNECTION_FEEDBACK_ON_ENTER_NO_OBJ_COMM_CONN(fdb, txt_orig, ctxt, comm) jvx_getCallProtNoLink(fdb, txt_orig, JVX_CONNECTION_ORIGIN, ctxt, comm)
#define JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_CONTEXT(fdb, object, ctxt) jvx_getCallProtObject(fdb, object, NULL, JVX_CONNECTION_ORIGIN, ctxt)
#define JVX_CONNECTION_FEEDBACK_ON_ENTER_DATAPROC_CONTEXT(fdb, proc) jvx_getCallProtDataProc(fdb, proc, JVX_CONNECTION_ORIGIN)
#define JVX_CONNECTION_FEEDBACK_ON_ENTER_CONN(fdb, refto) jvx_getCallProtConnector(fdb, refto, JVX_CONNECTION_ORIGIN)
#define JVX_CONNECTION_FEEDBACK_ON_ENTER_FAC_MAS(fdb, refto) jvx_getCallProtFactoryMaster(fdb, refto, JVX_CONNECTION_ORIGIN)
#define JVX_CONNECTION_FEEDBACK_ON_LEAVE_ERROR(fdb, res, err_mess) if(fdb) {fdb->setErrorCode(res, err_mess, JVX_CONNECTION_ORIGIN);}
#define JVX_CONNECTION_FEEDBACK_GET_ERROR_STRING(fdb) jvx_getCallProtStringShort(fdb)
#define JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt, errTp) JVX_CONNECTION_FEEDBACK_ON_LEAVE_ERROR(fdb, errTp, txt)
#define JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, txt, errTp) JVX_CONNECTION_FEEDBACK_ON_LEAVE_ERROR(fdb, errTp, txt)
#define JVX_CONNECTION_FEEDBACK_SET_COMMENT_STRING(fdb, txt) JVX_CONNECTION_FEEDBACK_ON_LEAVE_ERROR(fdb, JVX_NO_ERROR, txt)
#define JVX_CONNECTION_FEEDBACK_CLEAR(fdb) fdb->clear()

#define JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, theDataP) \
	if (fdb && theDataP) \
	{ \
		fdb->addEntry("params-test-in", jvx_linkDataParams2String(theDataP).c_str(), \
			JVX_NO_ERROR, IjvxCallProt::JVX_CALL_PROT_INFO); \
	}

#define JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_O(fdb, theData) \
	if (fdb && theData) \
	{ \
		fdb->addEntry("params-test-out", jvx_linkDataParams2String(theData).c_str(), \
			JVX_NO_ERROR, IjvxCallProt::JVX_CALL_PROT_INFO); \
	}

#define JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_O_TAG(fdb, theData, tag) \
	if (fdb && theData) \
	{ \
		fdb->addEntry(tag, jvx_linkDataParams2String(theData).c_str(), \
			JVX_NO_ERROR, IjvxCallProt::JVX_CALL_PROT_INFO); \
	}

#endif

#endif
