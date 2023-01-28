#ifndef __HJVXDATALOGGER_OTF_H__
#define __HJVXDATALOGGER_OTF_H__

#include "jvx.h"

JVX_INTERFACE IjvxDataLogger_Otf_report
{
 public:
  virtual jvxErrorType JVX_CALLINGCONVENTION report_prepare() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_start(std::string fName) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_stop(std::string fName) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_postprocess() = 0;
};

// ================================================================

class HjvxDataLogger_Otf
{
protected:
	
	IjvxObject* theDataLogger_obj;
	IjvxDataLogger* theDataLogger_hdl;
	jvxState status;
	JVX_MUTEX_HANDLE safeAccess_Otf;
	std::string nmFile;
	IjvxDataLogger_Otf_report* bwd;
public:
	HjvxDataLogger_Otf();
	~HjvxDataLogger_Otf();
	jvxErrorType initialize(IjvxDataLogger_Otf_report* bwdp);
	jvxErrorType terminate();
	jvxErrorType activate(const std::string& fName, const std::string& postfix);
	jvxErrorType deactivate();
	jvxErrorType start();
	jvxErrorType stop();
};

#endif
