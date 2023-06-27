	if (id == cnt)
	{
		adescr->assign("Rs232");
#ifdef JVX_OS_WINDOWS
		* funcInit = jvxTRs232Win32_init;
		*funcTerm = jvxTRs232Win32_terminate;
#else
		* funcInit = jvxTRs232Glnx_init;
		*funcTerm = jvxTRs232Glnx_terminate;
#endif
		return(JVX_NO_ERROR);
	}
	cnt++;
