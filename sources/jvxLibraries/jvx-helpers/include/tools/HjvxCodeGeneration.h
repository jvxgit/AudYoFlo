#ifndef _HJVX_CODE_GENERATION_MACROS_H__
#define _HJVX_CODE_GENERATION_MACROS_H__

#define TOKEN_FLOAT(dt) "(" dt ")%.30e"
#define TOKEN_INT "%d"
#define MATRIX_ONE_FIELD

#define FILL_TABLE_VALUES(fHdl, numVals, field, idx, token) \
	for(idx = 0; idx < numVals; idx++) \
	{ \
		if(idx != 0) \
		{ \
			fprintf(fHdl, ", "); \
		} \
		fprintf(fHdl, token, field[idx]); \
	} 
#define FILL_TABLE_VALUES_2D(fHdl, numValsY, numValsX, field, idx1, idx2, token) \
	for(idx1 = 0; idx1 < numValsY; idx1++) \
	{ \
		for(idx2 = 0; idx2 < numValsX; idx2++) \
		{ \
			if(idx2 != 0)\
			{ \
				fprintf(fHdl, ", "); \
			} \
			fprintf(fHdl, token, field[idx1][idx2]); \
			if((idx2 == (numValsX-1))&&(idx1 != (numValsY-1))) \
			{ \
				fprintf(fHdl, ",\n\t"); \
			} \
		} \
	} 

#define FILL_TABLE_CONST(fHdl, numVals, nmConst, idx) \
	for(idx = 0; idx < numVals; idx++) \
	{ \
		if(idx != 0) \
		{ \
			fprintf(fHdl, ", "); \
		} \
		fprintf(fHdl, nmConst); \
	} 
#define FILL_TABLE_CONST_2D(fHdl, numValsY, numValsX, nmConst, idx1, idx2) \
	for(idx1 = 0; idx1 < numValsY; idx1++) \
	{ \
		for(idx2 = 0; idx2 < numValsX; idx2++) \
		{ \
			if(!((idx1 == 0) && (idx2 == 0)))\
			{ \
				fprintf(fHdl, ", "); \
			} \
			fprintf(fHdl, nmConst); \
		} \
	} 

#define WRITE_STATIC_MACRO_DEFINE_1xN(fPtr, tpVar, nmVar, nmNumVar, numVals, fld, idx, token) \
  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + (std::string)"[" + (std::string)nmNumVar + (std::string)"] = \n{\n\t").c_str()); \
	FILL_TABLE_VALUES(fPtr, numVals, fld, idx, token); \
	fprintf(fPtr, "\n};\n"); 

#define WRITE_STATIC_MACRO_DEFINE_1xN_SET_ZERO(fPtr, tpVar, nmVar, nmNumVar, token) \
  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + (std::string)"[" + (std::string)nmNumVar + (std::string)"];\n").c_str()); 

#define WRITE_STATIC_MACRO_DEFINE_1xN_CONST(fPtr, tpVar, nmVar, nmNumVar, numVals, nmConst, idx, token) \
  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + (std::string)"[" + (std::string)nmNumVar + (std::string)"] = \n{\n\t").c_str()); \
	FILL_TABLE_CONST(fPtr, numVals, nmConst, idx); \
	fprintf(fPtr, "\n};\n"); 

#define WRITE_STATIC_MACRO_DEFINE_MxN_OF(fPtr, tpVar, nmVar, nmNumVarY, nmNumVarX, numValsY, numValsX, fld, idx1, idx2, token) \
  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + (std::string)"[" + \
		(std::string)nmNumVarY + (std::string)"*" + (std::string)nmNumVarX + (std::string)"] = \n{\n\t").c_str()); \
	FILL_TABLE_VALUES_2D(fPtr, numValsY, numValsX, fld, idx1, idx2, token); \
	fprintf(fPtr, "\n};\n"); 

#define WRITE_STATIC_MACRO_DEFINE_MxN_SET_ZERO_OF(fPtr, tpVar, nmVar, nmNumVarY, nmNumVarX) \
  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + (std::string)"[" + \
	(std::string)nmNumVarY + (std::string)"*" + (std::string)nmNumVarX + (std::string)"];\n").c_str()); 

#define WRITE_STATIC_MACRO_DEFINE_MxN_SET_CONST_OF(fPtr, tpVar, nmVar, nmNumVarY, nmNumVarX, numValsY, numValsX, nmConst, idx1, idx2, token) \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
	  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + "_" + jvx_int2String(idx1) + (std::string)"[" + \
			(std::string)nmNumVarY + (std::string)"][" + (std::string)nmNumVarX + (std::string)"] = \n{\n\t").c_str()); \
		FILL_TABLE_CONST_2D(fPtr, numValsY, numValsX, nmConst, idx1, idx2); \
		fprintf(fPtr, "\n};\n"); \
	} 

#define WRITE_STATIC_MACRO_DEFINE_MxN(fPtr, tpVar, nmVar, nmNumVarY, nmNumVarX, numValsY, numValsX, fld, idx1, idx2, token) \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
	  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + "_" + jvx_int2String(idx1) + (std::string)"[" + \
			(std::string)nmNumVarX + (std::string)"] = \n{\n\t").c_str()); \
		FILL_TABLE_VALUES(fPtr, numValsX, fld[idx1], idx2, token); \
		fprintf(fPtr, "\n};\n"); \
	} \
	fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)"* " + (std::string)nmVar + (std::string)"[" + \
			(std::string)nmNumVarY + (std::string)"] = \n{\n\t").c_str()); \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
		if(idx1 != 0) \
		{ \
			fprintf(fPtr, ", "); \
		} \
		fprintf(fPtr, "%s", ((std::string)nmVar + "_" + jvx_int2String(idx1)).c_str()); \
	} \
	fprintf(fPtr, "\n};\n"); 

#define WRITE_STATIC_MACRO_DEFINE_MxN_SET_ZERO(fPtr, tpVar, nmVar, nmNumVarY, nmNumVarX, numValsY, idx1, token) \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
	  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + "_" + jvx_int2String(idx1) + (std::string)"[" + \
			(std::string)nmNumVarX + (std::string)"];\n").c_str()); \
	} \
	fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)"* " + (std::string)nmVar + (std::string)"[" + \
			(std::string)nmNumVarY + (std::string)"] = \n{\n\t").c_str()); \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
		if(idx1 != 0) \
		{ \
		  fprintf(fPtr, "%s", ", ");	\
		} \
		fprintf(fPtr, "%s", ((std::string)nmVar + "_" + jvx_int2String(idx1)).c_str()); \
	} \
	fprintf(fPtr, "\n};\n"); 

#define WRITE_STATIC_MACRO_DEFINE_MxN_SET_ZERO_PREPRINT(fPtr, tpVar, nmVar, nmNumVarY, nmNumVarX, numValsY, idx1, token, preprint1, preprint2) \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
	  fprintf(fPtr, "%s", preprint1); \
	  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + "_" + jvx_int2String(idx1) + (std::string)"[" + \
			(std::string)nmNumVarX + (std::string)"];\n").c_str()); \
	} \
	fprintf(fPtr, "%s", preprint2); \
	fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)"* " + (std::string)nmVar + (std::string)"[" + \
			(std::string)nmNumVarY + (std::string)"] = \n{\n\t").c_str()); \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
		if(idx1 != 0) \
		{ \
		  fprintf(fPtr, "%s", ", ");	\
		} \
		fprintf(fPtr, "%s", ((std::string)nmVar + "_" + jvx_int2String(idx1)).c_str()); \
	} \
	fprintf(fPtr, "\n};\n"); 

#define WRITE_STATIC_MACRO_DEFINE_MxN_SET_CONST(fPtr, tpVar, nmVar, nmNumVarY, nmNumVarX, numValsY, numValsX, nmConst, idx1, idx2, token) \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
	  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + "_" + jvx_int2String(idx1) + (std::string)"[" + \
			(std::string)nmNumVarX + (std::string)"] = \n{\n\t").c_str()); \
		FILL_TABLE_CONST(fPtr, numValsX, nmCOnst, idx2); \
		fprintf(fPtr, "\n};\n");			 \
	} \
	fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)"* " + (std::string)nmVar + (std::string)"[" + \
			(std::string)nmNumVarY + (std::string)"] = \n{\n\t").c_str()); \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
		if(idx1 != 0) \
		{ \
			fprintf(fPtr, ", "); \
		} \
		fprintf(fPtr, ((std::string)nmVar + "_" + jvx_int2String(idx1)).c_str()); \
	} \
	fprintf(fPtr, "\n};\n"); 

#define WRITE_STATIC_MACRO_DEFINE_MxNVAR_SET_ZERO(fPtr, tpVar, nmVar, nmNumVarY, numValsY, numValsX_var, idx1, token) \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
	  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + "_" + jvx_int2String(idx1) + (std::string)"[" + \
			jvx_int2String(numValsX_var[idx1]) + (std::string)"];\n").c_str()); \
	} \
	fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)"* " + (std::string)nmVar + (std::string)"[" + \
			(std::string)nmNumVarY + (std::string)"] = \n{\n\t").c_str()); \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
		if(idx1 != 0) \
		{ \
			fprintf(fPtr, ", "); \
		} \
		fprintf(fPtr, "%s", ((std::string)nmVar + "_" + jvx_int2String(idx1)).c_str()); \
	} \
	fprintf(fPtr, "\n};\n"); 

#define WRITE_STATIC_MACRO_DEFINE_MxNVAR_SET_ZERO_PREPRINT(fPtr, tpVar, nmVar, nmNumVarY, numValsY, numValsX_var, idx1, token, preprint1, preprint2) \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
	  fprintf(fPtr, "%s", preprint1); \
	  fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + "_" + jvx_int2String(idx1) + (std::string)"[" + \
			jvx_int2String(numValsX_var[idx1]) + (std::string)"];\n").c_str()); \
	} \
	fprintf(fPtr, "%s", preprint2); \
	fprintf(fPtr, "%s", ((std::string)tpVar + (std::string)"* " + (std::string)nmVar + (std::string)"[" + \
			(std::string)nmNumVarY + (std::string)"] = \n{\n\t").c_str()); \
	for(idx1= 0; idx1 < numValsY; idx1++) \
	{\
		if(idx1 != 0) \
		{ \
			fprintf(fPtr, ", "); \
		} \
		fprintf(fPtr, "%s", ((std::string)nmVar + "_" + jvx_int2String(idx1)).c_str()); \
	} \
	fprintf(fPtr, "\n};\n"); 

#define WRITE_STATIC_MACRO_DEFINE_1_SET_ZERO(fPtr, tpVar, nmVar) \
  fprintf(fPtr_src, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + " = 0;\n").c_str()); 

#define WRITE_STATIC_MACRO_DEFINE_1(fPtr, tpVar, nmVar, nmValue) \
  fprintf(fPtr_src, "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + " = " + nmValue + ";\n").c_str()); 

#define WRITE_STATIC_MACRO_DEFINE_1_VALUE(fPtr, tpVar, nmVar, value, token) \
  fprintf(fPtr_src, "%s" token "%s", ((std::string)tpVar + (std::string)" " + (std::string)nmVar + " = ").c_str(), value, ";\n"); 
  
#ifdef __cplusplus

#define Q(x) #x
#define QUOTE(x) Q(x)

#define JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE_SIMPLE(out, txt, nd) \
	JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE(out, QUOTE(txt), &txt, JVX_DATAFORMAT_FROM_POINTER(&txt), nd)

#define JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE_SIMPLE_IDX(out, txt, nd,idx) \
	JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE_IDX(out, QUOTE(txt), txt, JVX_DATAFORMAT_FROM_POINTER(txt), nd, idx)

#define JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE_SIMPLE_CONVERT(out, txt, cvt) \
	JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE_CONVERT(out, QUOTE(txt), (jvxSize)txt, cvt)

jvxErrorType JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE_CONVERT(std::string& out, const std::string& varName, jvxSize idxEnum, const std::string& cvt);
jvxErrorType JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE(std::string& out, const std::string& varName, jvxHandle* fld, jvxDataFormat form, jvxSize numDigits);
jvxErrorType JVX_OUTPUT_LINE_ASSIGN_SINGLE_VALUE_IDX(std::string& out, const std::string& varName, jvxHandle* fld, jvxDataFormat form, jvxSize numDigits, jvxSize idx);
jvxErrorType JVX_OUTPUT_LINE_ASSIGN_FIELD(std::string& out, const std::string& varName, jvxHandle* fld, jvxDataFormat form, jvxSize dimY, jvxSize numTabs, jvxSize numDigits);
jvxErrorType JVX_OUTPUT_LINE_ASSIGN_ENUM(std::string& out, const std::string& varName, jvxSize idxOption, jvxTextHelpers* helpers, jvxSize numHelpersMax);

#endif
#endif