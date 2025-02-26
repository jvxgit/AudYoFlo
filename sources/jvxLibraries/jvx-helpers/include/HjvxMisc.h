#ifndef _JVXMISCHELPERS_H__
#define _JVXMISCHELPERS_H__

#define NUM_DIGITS_RELATION 5

#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <cstdlib>
#include <functional>
#include "jvx_dsp_base.h"

class CjvxObject;

// ============================================================================

// Macro for SIGN
#ifndef JVX_SIGN
#define JVX_SIGN(a) (a > 0 ? 1:-1)
#endif

#ifndef JVX_ABS
#define JVX_ABS(a) (a > 0 ? a:-a)
#endif

// =========================================================================
// =========================================================================

// ============================================================================

#ifndef JVX_SAFE_NEW_OBJ
#define JVX_SAFE_NEW_OBJ(a, b) a = new b;
#endif

#ifndef JVX_SAFE_NEW_FLD
#define JVX_SAFE_NEW_FLD(ptr,type,numElms) ptr = new type[numElms]
#endif

#ifndef JVX_SAFE_NEW_FLD_CVRT
#define JVX_SAFE_NEW_FLD_CVRT(a,b,c, d) a = (d)new b[c]
#endif

#ifndef JVX_SAFE_MALLOC_OBJ
#define JVX_SAFE_MALLOC_OBJ(a, b) a = (b*)malloc(sizeof(b));
#endif

#ifndef JVX_SAFE_MALLOC_FLD
#define JVX_SAFE_MALLOC_FLD(a,b,c) a = (b*) malloc(sizeof(b) * c);
#endif

#ifndef JVX_SAFE_DELETE_OBJ
#define JVX_SAFE_DELETE_OBJ(a) if(a) { delete(a); a= NULL; }
#endif

#ifndef JVX_SAFE_DELETE_FLD
#define JVX_SAFE_DELETE_FLD(ptr, type) if(ptr) { delete[]((type*)ptr); ptr= NULL; }
#endif

#ifndef JVX_SAFE_FREE_OBJ
#define JVX_SAFE_FREE_OBJ(a) if(a) { free(a); a= NULL; }
#endif

#ifndef JVX_SAFE_FREE_FLD
#define JVX_SAFE_FREE_FLD(a) if(a) { free(a); a= NULL; }
#endif

#ifndef JVX_SAFE_DELETE_OBJ_CVRT
#define JVX_SAFE_DELETE_OBJ_CVRT(a,b) if(a) { delete((b)a); a = NULL; }
#endif

#ifndef JVX_SAFE_DELETE_FLD_CVRT
#define JVX_SAFE_DELETE_FLD_CVRT(a,b) if(a) { delete[]((b)a); a = NULL; }
#endif

#ifndef JVX_SAFE_FREE_FLD_PRECONVERT
#define JVX_SAFE_FREE_FLD_CVRT(a,b) if(a) { free((b)a); a = NULL; }
#endif


#ifndef JVX_DATA_2_DATA
#define JVX_DATA_2_DATA(in) in
#endif
#ifndef JVX_DATA_2_FLOAT
#define JVX_DATA_2_FLOAT(in) ((jvxFloat)in)
#endif
#ifndef JVX_DATA_2_INT32
#define JVX_DATA_2_INT32(in) JVX_DATA2INT32(in * JVX_MAX_INT_32_DATA)
#endif
#ifndef JVX_DATA_2_INT24
#define JVX_DATA_2_INT24(in) JVX_DATA2INT32(in * JVX_MAX_INT_24_DATA)
#endif
#ifndef JVX_DATA_2_INT16
#define JVX_DATA_2_INT16(in) JVX_DATA2INT16(in * JVX_MAX_INT_16_DATA)
#endif
#ifndef JVX_DATA_2_INT8
#define JVX_DATA_2_INT8(in) JVX_DATA2INT8(in * JVX_MAX_INT_8_DATA)
#endif
#ifndef JVX_DATA_2_INT64
#define JVX_DATA_2_INT64(in) JVX_DATA2INT64(in * JVX_MAX_INT_64_DATA)
#endif

#ifndef JVX_FLOAT_2_DATA
#define JVX_FLOAT_2_DATA(in) ((jvxData)in)
#endif
#ifndef JVX_INT32_2_DATA
#define JVX_INT32_2_DATA(in) ((jvxData)in * JVX_MAX_INT_32_DIV)
#endif
#ifndef JVX_INT24_2_DATA
#define JVX_INT24_2_DATA(in) ((jvxData)in * JVX_MAX_INT_24_DIV)
#endif
#ifndef JVX_INT16_2_DATA
#define JVX_INT16_2_DATA(in) ((jvxData)in * JVX_MAX_INT_16_DIV)
#endif
#ifndef JVX_INT64_2_DATA
#define JVX_INT64_2_DATA(in) ((jvxData)in * JVX_MAX_INT_64_DIV)
#endif
#ifndef JVX_INT8_2_DATA
#define JVX_INT8_2_DATA(in) ((jvxData)in * JVX_MAX_INT_8_DIV)
#endif

jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxInt32* ptr);
jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxInt16* ptr);
jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxInt8* ptr);
jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxInt64* ptr);
jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxData* ptr);
jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxSize* ptr);
jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxCBool* ptr);

#ifndef JVX_LOCAL_ASSERT_PROPERTIES_LEAVE
#define JVX_LOCAL_ASSERT_PROPERTIES_LEAVE true
#endif

#define JVX_LOCAL_ASSERT_PROPERTIES(condition, propname, res) \
	if(!condition) \
	{ \
		std::cout << "Fatal error: " << __FUNCTION__ << ": " << __LINE__ << ": failed to set property <" << propname << ", reason: " << jvxErrorType_descr(res) << std::endl; \
		if(JVX_LOCAL_ASSERT_PROPERTIES_LEAVE) \
		{ \
			assert(condition); \
		} \
	}

#define JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE(propRef, outVar, format, propname, callGate) \
	{ \
		jPAD ident(propname); \
		jPD trans(true); \
		jvxErrorType res = propRef->get_property(callGate, jPRG(&outVar, 1, format), ident, trans); \
		JVX_LOCAL_ASSERT_PROPERTIES((res == JVX_NO_ERROR), propname, res); \
	}

#define JVX_LOCAL_ASSERT_GET_PROPERTIES_SINGLE_FULL(propRef, outVar, format, propname, callGate) \
	{ \
		jPAD ident(propname); \
		jPD trans; \
		jvxErrorType res = propRef->get_property(callGate, jPRG(&outVar, 1, format), ident, trans); \
		JVX_LOCAL_ASSERT_PROPERTIES((res == JVX_NO_ERROR), propname, res); \
	}

#define JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE(propRef, outVar, format, propname, callGate) \
	{ \
		jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(propname); \
		jvx::propertyDetail::CjvxTranferDetail trans(true, 0); \
		jvxErrorType res = propRef->set_property(callGate , \
			jPRG(&outVar, 1, format), ident, trans); \
		JVX_LOCAL_ASSERT_PROPERTIES((res == JVX_NO_ERROR), propname, res); \
	}

#define JVX_LOCAL_ASSERT_SET_PROPERTIES_SINGLE_FULL(propRef, outVar, format, propname, callGate) \
	{ \
		jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(propname); \
		jvx::propertyDetail::CjvxTranferDetail trans(false, 0); \
		jvxErrorType res = propRef->set_property(callGate, \
			 &outVar, 1, format, ident, trans); \
		JVX_LOCAL_ASSERT_PROPERTIES((res == JVX_NO_ERROR), propname, res); \
	}

// ============================================================================

#ifdef JVX_PROPERTY_ACCESS_CHECK_VERBOSE 

#define JVX_CHECK_PROPERTY_ACCESS_OK(arg1, arg2, arg3, arg4) jvx_check_property_access_ok(arg1, arg2, arg3, arg4, ((std::string)__FUNCTION__ +":" +__FILE__ +":" +Quotes(__LINE__)))
#define JVX_CHECK_PROPERTY_ACCESS_OK_AF(arg1, arg2, arg3, arg4) jvx_check_property_access_ok(arg1, arg2, arg3, arg4, ((std::string)__FUNCTION__ +":" +__FILE__ +":" +Quotes(__LINE__)), true)
#define JVX_PROPERTY_DESCRIBE_IDX_CAT(argId, argCat) ("Prop<" + jvx_size2String( argId) + "," + jvxPropertyCategoryType_txt(argCat) + ">")
#define JVX_PROPERTY_DESCRIBE_ID(argId) ("Prop[" + jvx_size2String( argId) + "]")

bool jvx_check_property_access_ok(jvxErrorType res, jvxAccessProtocol accProt, const std::string& context, IjvxAccessProperties* ac_prop, const std::string& loc, jvxBool acceptFail = false);
bool jvx_check_property_access_ok(jvxErrorType res, jvxAccessProtocol accProt, const std::string& context, IjvxProperties* obj, const std::string& loc, jvxBool acceptFail = false);
bool jvx_check_property_access_ok(jvxErrorType res, jvxAccessProtocol accProt, const std::string& context, const std::string& compId, const std::string& loc, jvxBool acceptFail = false);

#else

#define JVX_CHECK_PROPERTY_ACCESS_OK(arg1, arg2, arg3, arg4) jvx_check_property_access_ok(arg1, arg2)

#endif

#define JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns) \
	for (jvxSize i = 0; i < warns.size(); i++) \
	{ \
		std::cout << __FUNCTION__ << ": Warning: " << warns[i] << std::endl; \
	} \
	warns.clear();

#define JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_REPORT(warns) \
	for (jvxSize i = 0; i < warns.size(); i++) \
	{ \
		this->_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING); \
	} \
	warns.clear();

// ==================================================================
#define JVX_CLASS_SIMPLE_SET_GET_DECLARE(tp, str, member) \
	tp member ## _get() \
	{ \
		return str.member;	\
	}; \
	void member ## _set(const tp& in)

#define JVX_CLASS_SIMPLE_SET_GET_DEFINE(tp, str, member) \
	tp member ## _get() \
	{ \
		return str.member;	\
	}; \
	void member ## _set(const tp& in) \
	{ \
		str.member = in; \
	};

#define JVX_CLASS_SIMPLE_SET_DEFINE_START(cls, tp, str, member) \
	void cls::member ## _set(const tp& in) \
	{ \
		str.member = in;

#define JVX_CLASS_SIMPLE_SET_DEFINE_STOP }

#define JVX_CLASS_SIMPLE_SET_GET_DEFINE_SINGLE(tp, str, member, element) \
	jvxErrorType member  ## _ ## element ## _get_idx(jvxSize idx, tp& ret) \
	{ \
		if(idx < str.member.size()) \
		{ \
			ret = str.member[idx].element; \
			return JVX_NO_ERROR; \
		} \
		return JVX_ERROR_ID_OUT_OF_BOUNDS;	\
	}; \
	jvxErrorType member ## _ ## element ## _set_idx(jvxSize idx, const tp& in) \
	{ \
		if(idx < str.member.size()) \
		{ \
			str.member[idx].element = in; \
			return JVX_NO_ERROR; \
		} \
		return JVX_ERROR_ID_OUT_OF_BOUNDS;	\
	}; \


#define JVX_CLASS_SIMPLE_SET_GET_DECLARE_SINGLE(tp, str, member, element) \
	jvxErrorType member  ## _ ## element ## _get_idx(jvxSize idx, tp& ret); \
	jvxErrorType member ## _ ## element ## _set_idx(jvxSize idx, const tp& in);

#define JVX_CLASS_SIMPLE_SET_GET_DEFINE_SINGLE_START(cls, tp, str, member, element) \
	jvxErrorType cls::member  ## _ ## element ## _get_idx(jvxSize idx, tp& ret) \
	{ \
		if(idx < str.member.size()) \
		{ \
			ret = str.member[idx].element; \
			return JVX_NO_ERROR; \
		} \
		return JVX_ERROR_ID_OUT_OF_BOUNDS;	\
	}; \
	jvxErrorType cls::member ## _ ## element ## _set_idx(jvxSize idx, const tp& in) \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		if(idx >= str.member.size()) \
		{ \
			return JVX_ERROR_ID_OUT_OF_BOUNDS;	\
		} \
		str.member[idx].element = in; 

#define JVX_CLASS_SIMPLE_SET_DEFINE_SINGLE_STOP \
		return res; \
	}


// ==================================================================
// ==================================================================

bool jvx_check_property_access_ok(jvxErrorType res, jvxAccessProtocol accProt);

// ==================================================================
// ==================================================================

#define JVX_OUTPUT_REPORT_DEBUG_LOCATION __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ":"

// These macros are used to output text information either via the report reference (if available)
// or via the cout output. 
// The order of usage always is 
// JVX_OUTPUT_REPORT_DEBUG_COUT_START
// JVX_OUTPUT_REPORT_DEBUG_COUT_REF << JVX_OUTPUT_REPORT_DEBUG_LOCATION << "..." << std::endl; 
// JVX_OUTPUT_REPORT_DEBUG_COUT_STOP
// ==================================================================================
#define JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep) \
	std::ostringstream scout; \
	std::ostream* coutl = &std::cout; \
	if (rep) \
	{ \
		coutl = &scout; \
	}

#define JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep) \
	if (rep) \
	{ \
		rep->report_simple_text_message(scout.str().c_str(), JVX_REPORT_PRIORITY_DEBUG); \
	} \
	else \
	{ \
		std::cout << std::endl; \
	}


#define JVX_OUTPUT_REPORT_DEBUG_COUT_REF *coutl

// bool JVX_CHECK_PROPERTY_ACCESS_OK(jvxErrorType res, jvxAccessProtocol* accProt);

// ============================================================================

void jvx_static_lock(jvxHandle* priv);

void jvx_static_unlock(jvxHandle* priv);

jvxErrorType jvx_static_try_lock(jvxHandle* priv);

// ============================================================================

#define JVX_DATA_2STRING_CONST_FORMAT_E -128
#define JVX_DATA_2STRING_CONST_FORMAT_G -256


jvxComponentType 
jvx_componentNameToComponentType(std::string cpString);

/**
 * Function to copy a string type variable in a fixed length character buffer.
 * The field will definitely be terminated by a NULL character.
 *///=======================================================================
bool
jvx_fillCharStr(char* ptrField, size_t lField, const std::string& str);

/**
 * Functions to convert values value into strings.
 *///=======================================================================
std::string jvx_int2String(int val);
std::string jvx_uint2String(unsigned int val);
std::string jvx_int322String(jvxInt32 val);
std::string jvx_uint322String(jvxUInt32 val);
std::string jvx_int162String(jvxInt16 val);
std::string jvx_uint162String(jvxUInt16 val);
std::string jvx_intx2String(int val);
std::string jvx_size2String(jvxSize val, jvxSize modeGen = 0);
std::string jvx_int642String(jvxInt64 val);
std::string jvx_uint642String(jvxUInt64 val);
std::string jvx_uint642Hexstring(jvxUInt64 val);
std::string jvx_uint322Hexstring(jvxUInt32 val);
std::string jvx_uint162Hexstring(jvxUInt16 val);
std::string jvx_data2String(jvxData convert);
std::string jvx_data2String(jvxData convert, int digits); // <- int to allow values lower than 0 for special purposes
std::string jvx_data2String_highPrecision(jvxData convert, jvxSize digits);

#define jvx_bitfield642String jvx_uint642Hexstring
#define jvx_bitfield162String jvx_uint162Hexstring

#ifdef JVX_CBITFIELD_IS_UINT64
#define jvx_cbitfield2Hexstring jvx_uint642Hexstring
#endif

std::string jvx_produceFilenameDateClock(const std::string& prefix);

#define JVX_PROP_BIT_2_SEL_ID(prop) \
	jvx_bitfieldSelection2Id(prop.value.selection(0), prop.value.entries.size())

// ==============================================================================
jvxSize
jvx_bitfieldSelection2Id(jvxBitField sel, jvxSize numSel);

jvxSize
jvx_bitfieldSelection2Id(jvxPropertyContainerSingle<jvxSelectionList_cpp> elm, jvxSize entrySel = 0);

#define JVX_BITFIELD_SELECTION_ID(prop) jvx_bitfieldSelection2Id(prop.value.selection(), prop.value.entries.size())


jvxSize
jvx_cbitfieldSelection2Id(jvxCBitField sel);

jvxCBitField
jvx_bitfield2CBitfield(jvxBitField sel, jvxSize offs = 0, jvxSize * firstPosiNonZero = nullptr);

jvxInt32
jvx_uint642Id(jvxUInt64 sel);

jvxInt32
jvx_bitfieldSelectionMax(jvxBitField sel);

std::string
jvx_selectionProp2String_start(jvxPropertyContainerSingle<jvxSelectionList_cpp>&selLstProp);

void
jvx_selectionProp2String_stop(jvxPropertyContainerSingle<jvxSelectionList_cpp>&selLstProp,
	const std::string & oldSelection, jvxSize & idSel, jvxSize idxEntry = 0);

//intmax_t
//jvx_string2IntMax(const std::string& in, jvxBool& err);

jvxSize 
jvx_string2Size(const std::string& in, jvxBool& err);

jvxData
jvx_string2Data(const std::string& in, jvxBool& err);

intmax_t
jvx_string2IntMax(const std::string& in, jvxBool& err);

jvxInt64
jvx_string2Int64(const std::string& in, jvxBool& err);

jvxUInt64
jvx_string2UInt64(const std::string& in, jvxBool& err);

jvxUInt16
jvx_string2UInt16(const std::string& in, jvxBool& err);

std::string
jvx_valueList2String(jvxHandle* ptrVal, jvxDataFormat format, jvxSize num, jvxSize numDigits, const std::string& sep = ",", const std::string& startToken = "[", const std::string& stopToken = "]");

jvxErrorType
jvx_string2ValueList(const std::string& input, jvxHandle* ptrVal, jvxDataFormat format, jvxSize num, jvxSize* numUsed = NULL);

std::string 
jvx_valueList2BinString(jvxHandle* ptrVal, jvxDataFormat format, jvxSize num, std::string& retValPP);

jvxErrorType
jvx_binString2ValueList(const std::string& in, const std::string& inPP, jvxHandle* ptrOut, jvxDataFormat format, jvxSize num, jvxSize* requiredBytes, jvxSize* numValuesCopied = NULL);

jvxErrorType jvx_binString2ValueListAlloc(const std::string txt, jvxHandle** fld_alloc, jvxSize* num_entries, jvxDataFormat* format);
jvxErrorType jvx_binString2ValueListDeallocate(jvxHandle* fldAllocated);

std::string jvx_prepareStringForLogfile(const std::string& oneMessage);

jvxErrorType jvx_string2Context(const std::string ctxtStr, jvxContext* ctxt);
jvxErrorType jvx_context2String(std::string& ctxtStr, jvxContext* ctxt);

#define jvx_string2bitfield64 jvx_string2UInt64 
#define jvx_string2bitfield16 jvx_string2UInt16 

/**
 *
 *///=======================================================================
std::string
jvx_replaceDirectorySeparators_toWindows(std::string path, char replaceChar_from, char replaceChar_to);

std::string
jvx_replaceDirectorySeparators_toWindows(std::string path, char replaceChar_from, const char* replaceChar_to);

std::string
jvx_replaceSpecialCharactersBeforeWrite(std::string text, std::string characterReplace);

std::string 
jvx_replaceSpecialCharactersBeforeWriteSystem(std::string inputStr);

/*
std::string
jvx_removePathFromFile(const std::string& pathToFile);
*/

std::string
jvx_absoluteToRelativePath(std::string path, bool isFile, const char* curPath = NULL);

std::string
jvx_extractFileFromFilePath(const std::string& fName);

std::string
jvx_extractDirectoryFromFilePath(const std::string& fName);

std::string
jvx_extractExtensionFromFilePath(const std::string& fName);

std::string
jvx_fileBaseName(const std::string& fName);

std::string
jvx_changeDirectoryUp(const std::string& path);

std::string 
jvx_changeDirectoryPath(const std::string& path, const std::string& path_ext);

std::string
jvx_replaceCharacter(std::string path, char replace, char by);

std::string
jvx_replaceStrInStr(std::string strText, const std::string& strLookFor, const std::string& strReplace, jvxSize* numReplaced = NULL);

/**
 * Given the buffersize, the processing format and the number of channels, this function computes the size
 * in bytes
 *///==========================================================
size_t
jvx_numberBytesField(int buffersize, jvxDataFormat format, int chans);

#ifdef JVX_OS_WINDOWS

void
jvx_addPathEnvironment_onlywindows(std::vector<std::string>& additionalPaths);
#endif

#ifdef JVX_OS_WINDOWS

/**
 * Function to convert a standard string into a LPWSTR. This is required for the IPC since MS
 * tends to use their own specific non-standard conform strings.
 *///==========================================================================
wchar_t*
jvx_stdString2LPWSTR_alloc(std::string txt);

/**
 * Function to convert a standard string into a LPWSTR. This is required for the IPC since MS
 * tends to use their own specific non-standard conform strings.
 *///==========================================================================
void
jvx_stdString2LPWSTR_delete(wchar_t* ptr);

#endif


/**
* Convert all letter to uppercases letters
*///=======================================================================
std::string jvx_toUpper(const std::string& txt);

/**
 * Check a 64 bit bitfield for allowed states
 *///=====================================================================
std::string jvx_validStates2String(jvxBitField states);

/* Parse a command of format "<f_expr>[<addArg>](<args>)*/
jvxErrorType
jvx_parseCommandIntoToken(std::string command, std::string& f_expr, std::vector<std::string>& args, std::string* addArg = nullptr);

/*
jvxErrorType
jvx_parseStrArrayIntoTokens(std::string expr, std::list<std::string>& args, char sepTok = ',');
*/

//jvxErrorType 
//jvx_parseStringListIntoTokens(std::string expr, std::vector<std::string>& args, char sep_token = ',');

void
jvx_tokenRemoveCharLeftRight(
	std::string& oneToken,
	char removeit = ' ',
	jvxBool removeLeft = true,
	jvxBool removeRight = true);

std::string jvx_tokenRemoveChar(const std::string& oneToken, char* removeit);

std::string 
jvx_constructPropertyLinkDescriptor(std::string tag, std::string propname, std::vector<std::string> paramlst);

std::string 
jvx_createPropertyLinkDescriptor(std::string tag, std::string propname, std::vector<std::string> paramlst);

jvxErrorType	
jvx_parsePropertyKeyList(std::vector<std::string>& paramlst, std::string propname, std::string& propval, jvxSize* matchedId = NULL);

jvxErrorType	
jvx_parsePropertyStringToKeylist(std::string propTargetNameStr, std::vector<std::string>& paramlst);

jvxErrorType	
jvx_parsePropertyLinkDescriptor(std::string propTargetNameStr, std::string& tag, std::string& propname, std::vector<std::string>& paramlst);

std::string
jvx_parseStringFromBrackets(const std::string& in, jvxBool& errBracketsNotFound, char lB = '(', char rB = ')');

jvxErrorType 
jvx_stringToMacAddress(std::string str, jvxUInt8* mac);

std::string 
jvx_macAddressToString(jvxUInt8* mac);

std::string 
jvx_shortenStringName(jvxSize numLetters, std::string input, jvxSize relation = 1, jvxSize fragmnts = 4);

std::string 
jvx_makeFilePath(const std::string& folder, const std::string& fname);

std::string 
jvx_makePathExpr(const std::string& prefix, const std::string& suffix, jvxBool outputIsAPrefix = false, jvxBool forceLeadingSlash = true);

std::string
jvx_pathExprFromFilePath(std::string path, char sep = '/');

std::string
jvx_fileNameFromFilePath(std::string path);

void
jvx_composePathExpr(std::vector<std::string>& lst, std::string& path, jvxSize idx);

std::string
jvx_popBackPathExpr(const std::string& pathExpr);

void
jvx_decomposePathExpr(std::string path, std::vector<std::string>& lst);

bool
jvx_findPathExprEntry_idx(std::string path, std::string& theEntry, jvxSize idx);

jvxErrorType 
jvx_parseCommandLineOneToken(std::string in, std::vector<std::string>& out, char sep = ' ', jvxSize numMax = JVX_SIZE_UNSELECTED);

std::string
jvx_charReplace(std::string input, char* replaceChar_from,  const char** replaceChar_to, jvxSize numReplace);

jvxErrorType 
jvx_parseValueString_size(std::string input, jvxSize* numberValues);

jvxErrorType 
jvx_parseValueString_content(std::string input, jvxHandle* fld, jvxSize* numberValues, jvxDataFormat form);
	
jvxBool
jvx_compareComponentIdentWildcard(jvxComponentIdentification cp_this_wc, jvxComponentIdentification to_this);

jvxBool 
jvx_compareStringsWildcard(std::string compareme_wc, std::string tome);

namespace jvx {
	namespace helper {

		std::string propSelListFirstSelection(const jvxPropertyContainerSingle<jvxSelectionList_cpp>& selLst);
		std::string popNextElement(std::string& text, const std::list<std::string>& tokens);

		jvxErrorType parseStringListIntoTokens(std::string expr, std::list<std::string>& args, char sep_token = ',');
		jvxErrorType parseStringListIntoTokens(std::string expr, std::vector<std::string>& args, char sep_token = ',');

		std::string filterEscapes(const std::string& in, jvxBool allowSomeEscapes = false);
		std::string asciToUtf8(const std::string& in);
		std::string utf82Ascii(const std::string& in);
		jvxInt32 hash(std::string str);

		void debug_out_command_request(const CjvxReportCommandRequest& request, std::ostream& str, const std::string& tag);

		jvxErrorType scanForFiles(const std::string& pathname, const std::string& ext, std::function< void(const std::string& fNamePath) > lambda, const std::string& wcName  ="*");
		
		jvxHandle* genericDataBufferAllocate1D(jvxDataFormat form, jvxSize num);
		void genericDataBufferDeallocate(jvxHandle*& buf, jvxDataFormat form);

		jvxErrorType genericDataBufferConvert(jvxHandle** bufsIn, jvxDataFormat formIn, jvxHandle** bufsOut, jvxDataFormat formOut, jvxSize nChans, jvxSize numElms);
		jvxBool checkAllowTypeChange(jvxComponentType tpOld, jvxComponentType tpNew);

		jvxBool translate_transfer_chain_get_properties(jvx::propertyCallCompactRefList* propCallCompact, IjvxProperties* prop_ptr);

		template <class T>
		std::list<T> parseNumericExpression(const std::string& txt, jvxBool& err)
		{
			jvxSize i;
			int state = 0;
			std::list<T> returnVal;
			T myRow;
			std::string oneToken;
			jvxData val;
			err = false;
			jvxBool leadingBraces = false;

			for (i = 0; i < txt.size(); i++)
			{
				char oneChar = txt[i];

				switch (state)
				{
				case 0:
					if (
						(oneChar != ' ') && (oneChar != '\t'))
					{
						state = 1;
						oneToken = "";
						if (oneChar == '[')
						{
							leadingBraces = true;
						}
						else
						{
							i--;
							leadingBraces = false;
						}
					}
					break;

				case 1:
					if (
						(oneChar != ' ') && (oneChar != '\t'))
					{
						state = 2;
						oneToken = "";
					}
					else
					{
						break;
					}
				case 2:
					if ((oneChar == ',') || (oneChar == ' ') || (oneChar == '\t'))
					{
						jvxBool errL = false;
						//val = atof(oneToken.c_str());
						if (
							(oneToken.size() >= 2) &&
							((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
						{
							val = (jvxData)jvx_string2Int64(oneToken, errL);
						}
						else
						{
							val = jvx_string2Data(oneToken, errL);
						}
						if (errL)
							err = true;
						myRow.push_back(val);
						oneToken = "";
						state = 1;
					}
					else if (oneChar == ';')
					{
						if (!oneToken.empty())
						{
							jvxBool errL = false;
							if (
								(oneToken.size() >= 2) &&
								((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
							{
								val = (jvxData)jvx_string2Int64(oneToken, errL);
							}
							else
							{
								val = jvx_string2Data(oneToken, errL);
							}
							if (errL)
								err = true;
							myRow.push_back(val);
						}
						oneToken = "";
						returnVal.push_back(myRow);
						myRow.clear();
						state = 1;
					}
					else if (oneChar == ']')
					{
						if (leadingBraces)
						{
							if (!oneToken.empty())
							{
								jvxBool errL = false;
								if (
									(oneToken.size() >= 2) &&
									((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
								{
									val = (jvxData)jvx_string2Int64(oneToken, errL);
								}
								else
								{
									val = jvx_string2Data(oneToken, errL);
								}
								if (errL)
									err = true;
								myRow.push_back(val);
							}
							if (!myRow.empty())
							{
								returnVal.push_back(myRow);
								myRow.clear();
							}
							state = 0;

							// The ] terminates the parser..
							break;
						}
						else
						{
							err = true;
						}
					}
					else
					{
						oneToken += oneChar;
					}
					break;
				}
			}// for (i = 0; i < txt.size(); i++)
			if (!leadingBraces)
			{
				if (!oneToken.empty())
				{
					jvxBool errL = false;
					if (
						(oneToken.size() >= 2) &&
						((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
					{
						val = (jvxData)jvx_string2Int64(oneToken, errL);
					}
					else
					{
						val = jvx_string2Data(oneToken, errL);
					}
					if (errL)
						err = true;
					myRow.push_back(val);
				}
				if (!myRow.empty())
				{
					returnVal.push_back(myRow);
				}
			}
			return returnVal;
		};
	}

	namespace align {

		void resetComponentIdOnUnset(jvxComponentIdentification& tp);
	}

	// The following code from here:
	// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
	class coutColor 
	{
	public:
		enum class cc
		{
			FG_RED = 31,
			FG_GREEN = 32,
			FG_BLUE = 34,
			FG_DEFAULT = 39,
			BG_RED = 41,
			BG_GREEN = 42,
			BG_BLUE = 44,
			BG_DEFAULT = 49
		};

		cc code = cc::BG_DEFAULT;
	public:
		coutColor(cc pCode = cc::BG_DEFAULT) : code(pCode)
		{
		};

		friend std::ostream&
			operator<<(std::ostream& os, const coutColor& mod) {
			return os << "\033[1;" << (int)mod.code << "m";
		}
	};

}

void 
jvx_AllocateOneRcParameter(jvxRCOneParameter* ptr, jvxDataFormat form, jvxSize numChans, jvxSize bSize, jvxBool allocateBuf = true);

void 
jvx_DeallocateOneRcParameter(jvxRCOneParameter* ptr);

bool 
jvxFileExists(const char *filePath, const char* fileName);

jvxErrorType
jvx_exchange_property_callback_local_pack(jvxPropertyCallback cb, jvxHandle* cb_priv, jvxFloatingPointer_propadmin* constr, const std::string& propDescription, jvxBool do_set);

jvxErrorType
jvx_exchange_property_callback_local_unpack(jvxFloatingPointer* in, jvxFloatingPointer_propadmin*& constr, std::string& propDescription, jvxBool& do_set);

// =========================================================================
// =========================================================================

inline jvxSize jvx_stateToIndex(jvxState theState)
{
	jvxUInt32 statInt = jvx_bitFieldValue32(theState);
	switch(statInt)
	{
	case JVX_STATE_NONE:
		return(0);
	case JVX_STATE_INIT:
		return(1);
	case JVX_STATE_SELECTED:
		return(2);
	case JVX_STATE_ACTIVE:
		return(3);
	case JVX_STATE_PREPARED:
		return(4);
	case JVX_STATE_PROCESSING:
		return(5);
	case JVX_STATE_COMPLETE:
		return(6);
	case (jvxUInt32)JVX_STATE_DONTCARE:
		return(JVX_SIZE_DONTCARE);
	}
   	return(JVX_SIZE_UNSELECTED);
}

inline jvxState jvx_idxToState(jvxSize id)
{
	if(id == JVX_SIZE_UNSELECTED)
	{
		return(JVX_STATE_NONE);
	}
	if(id == JVX_SIZE_DONTCARE)
	{
		return(JVX_STATE_DONTCARE);
	}

	switch(id)
	{
	case 0:
		return(JVX_STATE_NONE);
	case 1:
		return(JVX_STATE_INIT);
	case 2:
		return(JVX_STATE_SELECTED);
	case 3:
		return(JVX_STATE_ACTIVE);
	case 4:
		return(JVX_STATE_PREPARED);
	case 5:
		return(JVX_STATE_PROCESSING);
	case 6:
		return(JVX_STATE_COMPLETE);
	default:
		break;
	}
	assert(0);
	return(JVX_STATE_NONE);
}

inline jvxBitField
JVX_DATA2BITFIELD(jvxData a)
{
	return (jvxBitField)((jvxInt32)((a)+JVX_SIGN(a)*0.5));
}
inline void jvx_switchEndianess_int32(jvxInt32* value)
{
	jvxSize i;
	jvxInt32 retVal = 0;
	jvxInt32 tmp;
	for(i = 0; i < 4; i++)
	{
		retVal = retVal << 8;
		tmp = *value >> i*8;
		retVal |= (tmp & 0xFF);
	}
	*value = retVal;
}

inline void jvx_switchEndianess_int16(jvxInt16* value)
{
	jvxSize i;
	jvxInt16 retVal = 0;
	for(i = 0; i < 2; i++)
	{
		retVal = retVal << 8;
		retVal |= (*value >> i*8) & 0xFF;
	}
	*value = retVal;
}

inline std::string jvx_loopify(std::string token, jvxSize cnt)
{
	jvxSize i;
	std::string retV;
	for(i = 0; i < cnt; i++)
	{
		retV += token;		
	}
	return(retV);
}

inline jvxBool jvx_isStringsWildcard(std::string checkme)
{
	jvxSize i;
	for (i = 0; i < checkme.size(); i++)
	{
		if ((checkme[i] == '?') ||
			(checkme[i] == '*') ||
			(checkme[i] == '+'))
		{
			return(true);
		}
	}
	return(false);
}

//jvxComponentIdentification jvxCreateComponentIdentification(jvxComponentType tp, jvxSize slotid, jvxSize slotsubid);
//jvxComponentIdentification jvxCreateComponentIdentificationUnlocated(jvxComponentType tp);
std::string jvxComponentIdentification_txt(jvxComponentIdentification cpTp, bool expl = false);
jvxErrorType jvxComponentIdentification_decode(jvxComponentIdentification& cpTp, const std::string& in);

jvxErrorType jvxSequencerElementType_decode(jvxSequencerElementType& tp, const std::string& txt);
jvxErrorType jvxSequencerQueueType_decode(jvxSequencerQueueType& tp, const std::string& txt);

void jvx_request_interfaceToObject(IjvxInterfaceReference* toadd, 
	IjvxObject** theObj, 
	jvxComponentIdentification* tp, 
	jvxApiString* descror,
	jvxApiString* descrion = NULL);
void jvx_return_interfaceToObject(IjvxInterfaceReference* toadd, IjvxObject* theObj);

void jvx_commonConnectorToObjectDescription(IjvxCommonConnector* in_con, std::string& txt, const std::string& str);
void jvx_connectionmasterToObjectDescription(IjvxConnectionMaster* mas, std::string& txt, const std::string& str);

// Groupd some audio config paramaters
typedef struct
{
	jvxInt32 samplerate;
	jvxInt32 buffersize;
	jvxDataFormat format;
	jvxInt32 numInputs;
	jvxInt32 numOutputs;
	
	jvxSize minChanCntInput;
    jvxSize minChanCntOutput;
    jvxSize maxChanCntInput;
    jvxSize maxChanCntOutput;
    jvxBitField inChannelsSelectionMask;
    jvxBitField outChannelsSelectionMask;
} jvxAudioParams;

typedef struct
{
	jvxInt32 samplerate;
	jvxInt32 buffersize;
	jvxDataFormat format;
	jvxInt32 numInputs;
	jvxInt32 numOutputs;

	jvxSize minChanCntInput;
	jvxSize minChanCntOutput;
	jvxSize maxChanCntInput;
	jvxSize maxChanCntOutput;
	jvxBitField inChannelsSelectionMask;
	jvxBitField outChannelsSelectionMask;

	jvxSize segx;
	jvxSize segy;
	jvxDataFormatGroup subform;
	jvxCBitField exthints;
} jvxNodeParams;

#ifdef __cplusplus

void jvx_runAllActiveMainComponents(IjvxHost* hostRef, std::function<void(IjvxObject* theOwner, IjvxHiddenInterface* iface)> func);

jvxErrorType jvx_activateObjectInModule(
	IjvxHost* hHostRef, 
	jvxComponentIdentification& tp, 
	const std::string& refModuleName, 
	IjvxObject* theOwner = nullptr,
	jvxBool extend_if_necessary = false,
	const std::string& attach_name = "",
	jvxBool attachUId = false,
	jvxComponentType cpRemap = JVX_COMPONENT_UNKNOWN);
jvxErrorType jvx_deactivateObjectInModule(IjvxHost* hHostRef, jvxComponentIdentification& tp);

JVX_STATIC_INLINE jvxErrorType jvx_errc(jvxDspBaseErrorType errin)
{
	jvxErrorType errout = JVX_NO_ERROR;
	switch(errin)
	{
	case JVX_DSP_NO_ERROR:
		errout = JVX_NO_ERROR;
		break;
    case JVX_DSP_ERROR_UNKNOWN:
		errout = JVX_ERROR_UNKNOWN;
		break;
	case JVX_DSP_ERROR_INVALID_ARGUMENT:
		errout = JVX_ERROR_INVALID_ARGUMENT;
		break;
    case JVX_DSP_ERROR_WRONG_STATE:
		errout = JVX_ERROR_WRONG_STATE;
		break;
    case JVX_DSP_ERROR_UNSUPPORTED:
		errout = JVX_ERROR_UNSUPPORTED;
		break;
    case JVX_DSP_ERROR_BUFFER_OVERFLOW:
		errout = JVX_ERROR_BUFFER_OVERFLOW;
		break;
    case JVX_DSP_ERROR_INTERNAL:
		errout = JVX_ERROR_INTERNAL;
		break;
    case JVX_DSP_ERROR_CALL_SUB_COMPONENT_FAILED:
		errout = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		break;
    case JVX_DSP_ERROR_INVALID_SETTING:
		errout = JVX_ERROR_INVALID_SETTING;
		break;
    case JVX_DSP_ERROR_ABORT:
		errout = JVX_ERROR_ABORT;
		break;
	default:
		// If we end up here, we try to convert a c error type to a cpp error type while there is no
		// equivalent error type in cpp.
		assert(0);
	}
	return errout;
}

#define JVX_ASSERT_PROPERTY_ACCESS_RETURN(resL, txt) \
	if (resL != JVX_NO_ERROR) \
	{ \
		std::cout << "Failed to access property " << "<" << txt << ">" << ", error code " << jvxErrorType_descr(resL) << std::endl; \
	}

template<typename T> inline T JVX_DATA2T(jvxData a)
{
	return (T)((a)+JVX_SIGN(a)*0.5);
}

template<typename T> bool jvx_insertListBeforePosition(std::vector<T>& lst, T& newItem, jvxSize position)
{
	jvxSize i;
	
	// position = -1 is already considered
	if(position < lst.size())
	{
		std::vector<T> newList;
		for(i = 0; i < position; i++)
		{
			newList.push_back(lst[i]);
		}
		newList.push_back(newItem);
		for(i = position; i < lst.size(); i++)
		{
			newList.push_back(lst[i]);
		}
		lst = newList;
		return(true);
	}
	return(false);
}

template<typename T> bool jvx_insertListAfterPosition(std::vector<T>& lst, T& newItem, jvxSize position)
{
	jvxSize i;
	if(position < lst.size())
	{
		std::vector<T> newList;
		for(i = 0; i <= position; i++)
		{
			newList.push_back(lst[i]);
		}
		newList.push_back(newItem);
		for(i = position+1; i < lst.size(); i++)
		{
			newList.push_back(lst[i]);
		}
		lst = newList;
		return(true);
	}
	return(false);
}

template<typename T> jvxErrorType jvx_removeItemFromPosition(std::vector<T>& lst, jvxSize position)
{
	jvxSize i;
	if(position < (int)lst.size())
	{
		std::vector<T> newList;
		for(i = 0; i < position; i++)
		{
			newList.push_back(lst[i]);
		}
		for(i = position+1; i < (int)lst.size(); i++)
		{
			newList.push_back(lst[i]);
		}
		lst = newList;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
}

template<typename T, typename T_sel>
typename std::list<T>::iterator jvx_findItemSelectorInList(std::list<T>& lst, const T_sel& selection)
{
	typename std::list<T>::iterator elm = lst.begin();
	for(; elm != lst.end(); elm++)
	{
		if(elm->selector == selection)
		{
			break;
		}
	}
	return(elm);
}

template<typename T, typename T_sel>
typename std::vector<T>::iterator jvx_findItemSelectorInList(std::vector<T>& lst, const T_sel& selection)
{
	typename std::vector<T>::iterator elm = lst.begin();
	for (; elm != lst.end(); elm++)
	{
		if (elm->selector == selection)
		{
			break;
		}
	}
	return(elm);
}
template<typename T, typename T_sel>
typename std::vector<T>::iterator jvx_findItemSelectorInList_one(std::vector<T>& lst, const T_sel& selection, jvxSize idxSelector)
{
    typename std::vector<T>::iterator elm = lst.begin();
	for(; elm != lst.end(); elm++)
	{
		if(elm->selector[idxSelector] == selection)
		{
			break;
		}
	}
	return(elm);
}

template<typename T, typename T_sel0, typename T_sel1>
typename std::vector<T>::iterator jvx_findItemSelectorInList_two(std::vector<T>& lst, const T_sel0& selection0, const T_sel1& selection1)
{
	// jvxSize i;
	typename std::vector<T>::iterator elm = lst.begin();
	for (; elm != lst.end(); elm++)
	{
		// jvxBool foundit = false;
		if (
			(elm->selector0 == selection0) &&
			(elm->selector1 == selection1))
		{			
			break;
		}
	}
	return(elm);
}

template<typename T> typename std::vector<T>::iterator jvx_findItemIdInList_idx(std::vector<T>& lst, jvxSize& idx)
{
	typename std::vector<T>::iterator elm = lst.begin();
	jvxSize cnt = 0;
	for(; elm != lst.end(); elm++, cnt++)
	{
		if(cnt == idx)
		{
			break;
		}
	}
	return(elm);
}

void jvx_parseNumericExpression(std::string txt, jvxBool& err, std::vector<std::vector<jvxData> >&);
void jvx_parseNumericExpression(std::string txt, jvxBool& err, std::vector<std::vector<jvxInt64> >&);
void jvx_parseNumericExpression(std::string txt, jvxBool& err, std::vector<std::vector<jvxUInt64> >&);
std::vector<jvxData> jvx_parseCsvExpression(std::string txt, jvxBool& err, char sep_char = ';');
std::vector<std::string> jvx_parseCsvStringExpression(std::string txt, jvxBool& err);

std::string jvx_data2NumericExpressionString(std::vector<std::vector<jvxData> > & lst, int numDigs);
std::string jvx_data2NumericExpressionString(jvxData** lst, jvxSize N, jvxSize M, int numDigs);
std::string jvx_data2CsvExpressionString(std::vector<jvxData>& lst, int numDigs);
std::string jvx_string2CsvExpressionString(std::vector<std::string>& lst);
std::string jvx_stringNumericExpression(std::vector<jvxData> & lst, int numDigs);

jvxCBitField jvx_string2CBitField(std::string token);
std::string jvx_CBitField2String(jvxCBitField myField);

jvxBool jvx_compare_match_component_identification(const jvxComponentIdentification& comp_this, const jvxComponentIdentification& to_match);

jvxErrorType jvx_parseHttpQueryIntoPieces(std::vector<std::string>& qList, std::string in);
jvxErrorType jvx_findValueHttpQuery(std::vector<std::string>& qList, std::string& out, const std::string& lookfor);
	
std::string jvx_getCallProtStringLocalError(IjvxCallProt* fdb);
std::string jvx_getCallProtStringNextErrorTree(IjvxCallProt* fdb, jvxBool& err);

void jvx_getCallProtObject(IjvxCallProt* fdb, IjvxObject* object, const char* conn, const char* origin, const char* context = NULL, const char* comment = NULL);
void jvx_getCallProtConnector(IjvxCallProt* fdb, IjvxCommonConnector* refto, const char* origin);
void jvx_getCallProtNoLink(IjvxCallProt* fdb, const std::string& txt_orig, const char* origin, const char* ctxt, const char* comment);
void jvx_getCallProtFactoryMaster(IjvxCallProt* fdb, IjvxConnectionMasterFactory* refto, const char* origin);

std::string jvx_linkDataParams2String(jvxLinkDataDescriptor* lData);

jvxErrorType jvx_eventloop_copy_message(jvxHandle** ptr, jvxSize paramType, jvxSize szFld);
jvxErrorType jvx_eventloop_delete_message(jvxHandle** ptr, jvxSize paramType, jvxSize szFld);

void jvx_request_test_chain(jvxLinkDataDescriptor* data, CjvxObject* theObj, jvxCBitField reportFlags = ((jvxCBitField)1 << JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT));

void jvx_check_value(jvxInt64 val, jvxDataFormat form);
void jvx_check_value(jvxUInt64 val, jvxDataFormat form);

jvxErrorType jvx_add_default_sequence_sequencer(IjvxSequencer* theSequencer,
	bool onlyIfNoSequence,
	jvxOneSequencerStepDefinition* defaultSteps_run,
	jvxSize numSteps_run,
	jvxOneSequencerStepDefinition* defaultSteps_leave,
	jvxSize numSteps_leave,
	const std::string& nm_def_seq,
	const std::string& lb_def_seq = "");

void jvx_select_next_avail_slot(IjvxHost* host, jvxComponentType tp, jvxComponentIdentification* tps);

jvxErrorType jvx_activateComponentAsOwnerDevice(IjvxHost* theHostHdl,
	jvxComponentType tpTech,const std::string& cpTag, IjvxObject*& theObjHdl,
	jvxComponentIdentification& theObjId, IjvxObject* thisisme);

jvxErrorType jvx_deactivateComponentAsOwnerDevice(IjvxHost* theHostHdl,
	jvxComponentIdentification& theObjId);

std::string jvx_textMessagePrioToString(std::string message, jvxReportPriority prio);

std::string
jvx_create_text_seq_report(const std::string& intro, jvxSize sequenceId,
	jvxSize stepId, jvxSequencerQueueType tp,
	jvxSequencerElementType stp, jvxSize fId,
	jvxSize operation_state, const std::string& description);

std::string jvx_time_string();
std::string jvx_date_string();

// =================================================================================

#define JVX_SUBCALL_FUNCTION_DECLARE_OVERRIDE(fname, argsd) \
	virtual jvxErrorType JVX_CALLINGCONVENTION fname argsd override

#define JVX_SUBCALL_PULLIN_FUNCTION_DECLARE(fname, argsd, argsc) \
	virtual jvxErrorType JVX_CALLINGCONVENTION fname argsd override \
	{ \
		return _ ## fname argsc ; \
	}

#define JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_FUNCNAME(fname1, fname2, argsd, argsc) \
	virtual jvxErrorType JVX_CALLINGCONVENTION fname1 argsd override \
	{ \
		return _ ## fname2 argsc ; \
	}

#define JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_CLASS(fname, cname, argsd, argsc) \
	virtual jvxErrorType JVX_CALLINGCONVENTION fname argsd override \
	{ \
		return cname._ ## fname argsc ; \
	}

#define JVX_SUBCALL_PULLIN_FUNCTION_DECLARE_FUNCNAME_CLASS(fname1, fname2, cname, argsd, argsc) \
	virtual jvxErrorType JVX_CALLINGCONVENTION fname1 argsd override \
	{ \
		return cname._ ## fname2 argsc ; \
	}

// =================================================================================

void jvx_getFilesFolderPath(std::vector<std::string>& lst, const std::string& pathName, const std::string& extension);

JVX_STATIC_INLINE std::string JVX_DISPLAY_CONNECTOR(IjvxCommonConnector* ref)
{
	jvxApiString connname;
	std::string txt;
	ref->descriptor_connector(&connname);
	jvx_commonConnectorToObjectDescription(ref, txt, connname.std_str());
	return txt;
}

JVX_STATIC_INLINE std::string JVX_DISPLAY_MASTER(IjvxConnectionMaster* ref)
{
	jvxApiString mastername;
	std::string txt;
	ref->descriptor_master(&mastername);
	jvx_connectionmasterToObjectDescription(ref, txt, mastername.std_str());
	return txt;
}

JVX_STATIC_INLINE void jvx_printJvxString_ind(std::ostream& os, jvxApiString* str)
{
	jvxSize i;
	int j;
	int cnt = 0;
	std::string txt = str->std_str();
	for (i = 0; i < txt.size(); i++)
	{
		if (txt.substr(i,1) == JVX_TEXT_NEW_SEGMENT_CHAR_START)
		{
			os << std::endl;
			for (j = 0; j < cnt; j++)
			{
				os << " ";
			}
			os << JVX_TEXT_NEW_SEGMENT_CHAR_START << std::endl;
			cnt++;
			for (j = 0; j < cnt; j++)
			{
				os << " ";
			}
		}
		else if (txt.substr(i, 1) == JVX_TEXT_NEW_SEGMENT_CHAR_STOP)
		{
			os << std::endl;
			cnt--;
			for (j = 0; j < cnt; j++)
			{
				os << " ";
			}
			os << JVX_TEXT_NEW_SEGMENT_CHAR_STOP << std::endl;
			for (j = 0; j < cnt; j++)
			{
				os << " ";
			}
			
		}
		else
		{
			os << txt[i];
		}
	}
}

CjvxReportCommandRequest* jvx_command_request_copy_alloc(const CjvxReportCommandRequest& in);
void jvx_command_request_copy_dealloc(CjvxReportCommandRequest* in);

JVX_INTERFACE jvx_lock
{
public:
	virtual ~jvx_lock() {};

	virtual void lock() = 0;
	virtual void unlock() = 0;
	virtual bool try_lock() = 0;
};

class jvxostream : public std::streambuf, public jvx_lock
{
	
private:
	IjvxTextLog* theLog = nullptr;
	char* theBuffer = nullptr;
	jvxState theStat;
	std::string moduleName;
	jvxSize debugLevel = 0;
	jvxBool active = true;
	JVX_MUTEX_HANDLE theLock;
	JVX_THREAD_ID lockThreadId;
	jvxSize refCnt;
	JVX_THREAD_ID threadId_lasttime;

public:
	jvxostream();
	~jvxostream();
	void set_module_name(const std::string& modName);
	void set_debug_level(jvxSize dbgLev);

	jvxErrorType setReference(IjvxTextLog* theRef, char* theBufferP, jvxSize szBufP);
	jvxErrorType unsetReference();
	virtual int sync() override;
	virtual int overflow(int ch)override;

	virtual void lock() override;
	virtual void unlock() override;
	virtual bool try_lock() override;

	void setActive(jvxBool activeArg);
};

class jvxrtst_backup
{

public:
	IjvxToolsHost* theToolsHost = nullptr;
	IjvxObject* theTextLogger_obj = nullptr;
	IjvxTextLog* theTextLogger_hdl = nullptr;
	std::string theModuleName;
	jvxostream jvxos;
	char* jvxlst_buf = nullptr;
	jvxSize jvxlst_buf_sz = 0;

public:
	jvxrtst_backup()
	{
		theTextLogger_hdl = NULL;
		theTextLogger_obj = NULL;
		jvxlst_buf = NULL;
		jvxlst_buf_sz = 0;
		theModuleName = "NOT_SET"; 
		theToolsHost = NULL;
	};
	void set_module_name(const std::string& modName)
	{
		theModuleName = modName;
		jvxos.set_module_name(theModuleName);
	};
} ;

template <class T> 
class jvxLockWithVariable
{
public:
	T v;
	JVX_MUTEX_HANDLE lockHdl;

	jvxLockWithVariable()
	{
		JVX_INITIALIZE_MUTEX(lockHdl);
	};

	~jvxLockWithVariable()
	{
		JVX_TERMINATE_MUTEX(lockHdl);
	};

	void lock()
	{
		JVX_LOCK_MUTEX(lockHdl);
	}

	void unlock()
	{
		JVX_UNLOCK_MUTEX(lockHdl);
	}

	jvxBool try_lock()
	{
		JVX_TRY_LOCK_MUTEX_RESULT_TYPE res = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
		JVX_TRY_LOCK_MUTEX(res, lockHdl);
		if (res == JVX_TRY_LOCK_MUTEX_SUCCESS)
		{
			return true;
		}
		return false;
	}
};

template <class T>
class jvxRWLockWithVariable
{
public:
	enum class jvxRwLockFunction
	{
		JVX_RW_LOCK_SHARED,
		JVX_RW_LOCK_EXCLUSIVE
		//, JVX_RW_LOCK_NO_LOCK // <- this to ensure proper use!
	};

private:

	jvxSize lockStat[2] = { 0, 0 };

public:
	T v;
	JVX_RW_MUTEX_HANDLE lockHdl;

	jvxRWLockWithVariable()
	{
		JVX_INITIALIZE_RW_MUTEX(lockHdl);
	};

	~jvxRWLockWithVariable()
	{
		assert(lockStat[0] == 0);
		assert(lockStat[1] == 0);

		JVX_TERMINATE_RW_MUTEX(lockHdl);
	};

	void lock(jvxRwLockFunction func = jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE)
	{
		switch (func)
		{
		case jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE:
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(lockHdl);			
			break;
		case jvxRwLockFunction::JVX_RW_LOCK_SHARED:
			JVX_LOCK_RW_MUTEX_SHARED(lockHdl);
			break;
		default:
			assert(0);
		}

		// We hold the lock here. Here, the type should fit
		lockStat[(int)func]++;
	}
	void unlock(jvxRwLockFunction func = jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE)
	{
		assert(lockStat[(int)func]);
		lockStat[(int)func]--;

		switch (func)
		{
		case jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE:
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(lockHdl);
			break;
		case jvxRwLockFunction::JVX_RW_LOCK_SHARED:
			JVX_UNLOCK_RW_MUTEX_SHARED(lockHdl);
			break;
		default:
			assert(0);
		}
	}

	jvxBool try_lock(jvxRwLockFunction func = jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE)
	{
		JVX_TRY_LOCK_RW_MUTEX_RESULT_TYPE res = JVX_TRY_LOCK_RW_MUTEX_INIT_NO_SUCCESS;
		// assert(lockStat == jvxRwLockFunction::JVX_RW_LOCK_NO_LOCK);
		switch (func)
		{
		case jvxRwLockFunction::JVX_RW_LOCK_EXCLUSIVE:
			JVX_TRY_LOCK_RW_MUTEX_EXCLUSIVE(res, lockHdl);
			break;
		case jvxRwLockFunction::JVX_RW_LOCK_SHARED:
			JVX_TRY_LOCK_RW_MUTEX_SHARED(res, lockHdl);
			break;
		default:
			assert(0);
		}
		if (JVX_TRY_LOCK_RW_MUTEX_CHECK_SUCCESS(res))
		{
			// lockStat = func;
			lockStat[(int)func]++;
			return true;
		}
		return false;
	}
};

#define JVX_DEFINE_RT_ST_INSTANCES \
	jvxrtst_backup jvxrtst_bkp; \
	std::ostream jvxrtst;

#define JVX_INIT_RT_ST_INSTANCES \
	jvxrtst(&jvxrtst_bkp.jvxos)
#define JVX_INIT_RT_ST_INSTANCES_A , JVX_INIT_RT_ST_INSTANCES

void jvx_init_text_log(jvxrtst_backup& bkp);
void jvx_terminate_text_log(jvxrtst_backup& bkp);

void jvx_request_text_log(jvxrtst_backup& bkp);
void jvx_return_text_log(jvxrtst_backup& bkp); 

bool jvx_try_lock_text_log(jvxrtst_backup& bkp, jvxSize logLev);
void jvx_lock_text_log(jvxrtst_backup& bkp, jvxSize logLev);
void jvx_unlock_text_log(jvxrtst_backup& bkp);

int jvxLogLevel2Id(jvxLogLevel lev);

#define JVX_LOCAL_START_OUTPUT_STREAM(varos, varsb) \
		std::ostream varos(&varsb); 

#define JVX_START_LOCK_LOG_REF(ptr, LEVEL) \
	{ \
		jvxBool __dbgCout = false; \
		jvxSize __logLevel = jvxLogLevel2Id(LEVEL); \
		if (ptr && ptr->jvxrtst_bkp.theTextLogger_hdl && ptr->jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, __logLevel, &__dbgCout)) \
		{ \
			std::ostream* logptr = &ptr->jvxrtst; \
			if(__dbgCout) \
				logptr = &std::cout; \
			std::ostream& log = *logptr; \
			jvx_lock_text_log(ptr->jvxrtst_bkp, __logLevel);

#define JVX_STOP_LOCK_LOG_REF(ptr) \
			jvx_unlock_text_log(ptr->jvxrtst_bkp); \
		} \
	}

#define JVX_START_LOCK_LOG(LEVEL) JVX_START_LOCK_LOG_REF(this, LEVEL)
#define JVX_STOP_LOCK_LOG JVX_STOP_LOCK_LOG_REF(this)

#define TL3 JVX_START_LOCK_LOG_REF(this, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT)
#define TL JVX_STOP_LOCK_LOG_REF(this)

#endif // #idef _CPLUSPLUS

#endif
