#ifndef __jvxQtSaCustomBase_H__
#define __jvxQtSaCustomBase_H__

class jvxQtSaCustomBase
{

private: 
	std::string propsPrefix;
	std::string addConfigTokens;

public:
	jvxQtSaCustomBase()
	{
		propsPrefix = "";
	};
	
	virtual jvxErrorType setPropsPrefix(const char* pref)
	{
		propsPrefix = pref;
		return(JVX_NO_ERROR);
	};

	virtual jvxErrorType setAdditonalConfigTokens(const char* configAdds)
	{
		addConfigTokens = configAdds;
		return(JVX_NO_ERROR);
	};

	virtual jvxErrorType getPropsPrefix(jvxApiString* fldStr)
	{
		if (fldStr)
		{
			fldStr->assign(propsPrefix);
		}
		return(JVX_NO_ERROR);
	};

	virtual jvxErrorType getAdditonalConfigTokens(jvxApiString* fldStr)
	{
		fldStr->assign( addConfigTokens);
		return(JVX_NO_ERROR);
	};

	virtual jvxErrorType setWidgetTitle(const char* tit)
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

	virtual jvxErrorType clearUiElements()
	{
		return(JVX_ERROR_UNSUPPORTED);
	};

	/*
	jvxErrorType deallocate(jvxString* fldStr)
	{
		HjvxObject_deallocate(fldStr);
		return(JVX_NO_ERROR);
	};
	*/
};

#endif