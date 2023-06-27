	if (id == cnt)
	{
		adescr->assign("New Socket");
		*funcInit = jvxTNewSocket_init;
		*funcTerm = jvxTNewSocket_terminate;
		return(JVX_NO_ERROR);
	}
	cnt++;