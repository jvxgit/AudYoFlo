#ifndef __HJVXCPPMISC_H__
#define __HJVXCPPMISC_H__

class jvx_constCharStringLst
{
public:
	const char** cchars = nullptr;
	jvxSize num = 0;

	std::list<std::string> names;

	void add(const std::string nm)
	{
		names.push_back(nm);
	};

	void produce()
	{
		jvxSize cnt = 0;
		num = names.size();
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(cchars, const char*, names.size());
		for (const std::string& nm : names)
		{
			cchars[cnt] = nm.c_str();
			cnt++;
		}
	};

	void clear()
	{
		JVX_DSP_SAFE_DELETE_FIELD(cchars);
		cchars = nullptr;
		num = 0;
	};
};

// =========================================================



#endif
