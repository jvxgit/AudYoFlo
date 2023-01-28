#ifndef TJVXFRONTENDBACKEND_H__
#define TJVXFRONTENDBACKEND_H__

class jvxOneFrontendAndState
{
public:

	IjvxEventLoop_frontend* fe;
	jvxState st;

	jvxOneFrontendAndState()
	{
		fe = NULL;
		st = JVX_STATE_NONE;
	};

	bool operator == (IjvxEventLoop_frontend* compareThat)
	{
		if (fe == compareThat)
		{
			return true;
		}
		return false;
	};
};



// ==============================================================

class jvxOneBackendAndState
{
public:

	IjvxEventLoop_backend* be;
	jvxState st;

	jvxOneBackendAndState()
	{
		be = NULL;
		st = JVX_STATE_NONE;
	};

	bool operator == ( IjvxEventLoop_backend* compareThat)
	{
		if (be == compareThat)
		{
			return true;
		}
		return false;
	};
};

class jvxOnePriBackendAndState
{
public:

	IjvxEventLoop_backend_ctrl* be;
	jvxState st;

	jvxOnePriBackendAndState()
	{
		be = NULL;
		st = JVX_STATE_NONE;
	};

	bool operator == (IjvxEventLoop_backend_ctrl* compareThat)
	{
		if (be == compareThat)
		{
			return true;
		}
		return false;
	};
};

#endif