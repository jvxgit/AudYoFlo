#ifndef _TJVXMODULEONSTART_H__
#define _TJVXMODULEONSTART_H__

struct jvxModuleOnStart
{
	const char* modName = nullptr;
	std::function<void(IjvxObject*)> theCallback;
	// cbOnSelect theCallback = nullptr;
	jvxModuleOnStart(const char* nm, std::function<void(IjvxObject*)> cb = nullptr) : modName(nm), theCallback(cb) {};
};

#endif
