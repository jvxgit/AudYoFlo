#ifndef _TJVXMODULEONSTART_H__
#define _TJVXMODULEONSTART_H__

struct jvxModuleOnStart
{
	const char* modName = nullptr;
	std::function<void(IjvxObject*)> onSelect;
	std::function<void(IjvxObject*)> beforeUnselect;
	jvxModuleOnStart(const char* nm, std::function<void(IjvxObject*)> cbS = nullptr, std::function<void(IjvxObject*)> cbUS = nullptr) : modName(nm), onSelect(cbS), beforeUnselect(cbUS){};
};

#endif
