#ifndef _JVX_SYSTEM_MISC_UX_H__
#define _JVX_SYSTEM_MISC_UX_H__

JVX_STATIC_INLINE bool JVX_GETENVIRONMENTVARIABLE(char* var,char* bufRet,int ll)
{
	const char* varP = getenv(var);
	std::string str = "";
	memset(bufRet, 0, sizeof(char)*ll);
	if(varP)
	{
		str = varP;
//		for(int i = 0; i < ll; i++)
//		{
//			bufRet[i] = varP[i];
			//if(varP[i] == 0)
			//	break;
//		}
		memcpy(bufRet, str.c_str(), sizeof(char)*str.size());
		return(true);
	}
	return(false);
}

#define JVX_SETENVIRONMENTVARIABLE(var, value, set) setenv(var,  value, set)

#endif
