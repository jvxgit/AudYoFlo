#ifndef _TJVXMODULEONSTART_H__
#define _TJVXMODULEONSTART_H__

#ifdef JVX_CPLUSPLUS_NO_STD_FUNCTION
typedef void (*jvxModuleOnStart_onSelect)(IjvxObject*, jvxHandle* priv);
typedef void (*jvxModuleOnStart_onUnselect)(IjvxObject*, jvxHandle* priv);
#endif

struct jvxModuleOnStart
{
	const char* modName = nullptr;
#ifndef JVX_CPLUSPLUS_NO_STD_FUNCTION
	std::function<void(IjvxObject*)> onSelect;
	std::function<void(IjvxObject*)> beforeUnselect;
	jvxModuleOnStart(const char* nm, std::function<void(IjvxObject*)> cbS = nullptr, std::function<void(IjvxObject*)> cbUS = nullptr) : modName(nm), onSelect(cbS), beforeUnselect(cbUS){};
#else
	jvxModuleOnStart_onSelect onSelect;
	jvxModuleOnStart_onUnselect beforeUnselect;
	jvxHandle* priv = nullptr;
	jvxModuleOnStart(const char* nm, jvxModuleOnStart_onSelect cbS = nullptr, jvxModuleOnStart_onUnselect cbUS = nullptr, jvxHandle* cbPrv = nullptr) : modName(nm),
			onSelect(cbS), beforeUnselect(cbUS), priv(cbPrv){};
#endif
};

#endif
