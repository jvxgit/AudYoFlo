#ifndef __CJVXVSTPLUGINPARAMETER__H__
#define __CJVXVSTPLUGINPARAMETER__H__

#include "jvx.h"
#include "TjvxPluginCommon.h"
#include "CjvxPluginCommon.h"

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/base/ustring.h"

namespace Steinberg {
namespace Vst {

class jvxVstGainParameter : public Parameter
{
	ParamValue offsetShift = 50;
	ParamValue scaleFac = 100.0;
	std::string units;
	jvxSize numDigits = 1;
public:
	jvxVstGainParameter(int32 flags, int32 id, const char* nm = "Gain",
		ParamValue offsetShiftArg = 50, ParamValue scaleFacArg = 100.0,
		const char* unit = "dB", jvxSize numDigitsArg = 1);

	void toString(ParamValue normValue, String128 string) const SMTG_OVERRIDE;
	bool fromString(const TChar* string, ParamValue& normValue) const SMTG_OVERRIDE;
	void toUnnormalized(jvxData& unNormValue) const;

	static std::string toStringStatic(ParamValue normValue, 
		jvxData scaleFac, jvxData offsetShift, jvxSize numDigits, const std::string& units);

	void get_show_params(jvxData& offs, jvxData& scale, jvxSize& nDigs, std::string& units);

};

class jvxVstStringListParameter : public StringListParameter
{
public:
	jvxVstStringListParameter(const TChar* title, ParamID tag, const char* empty_show_arg = nullptr,
		const TChar* units = nullptr,
		int32 flags = ParameterInfo::kCanAutomate | ParameterInfo::kIsList,
		UnitID unitID = kRootUnitId, const TChar* shortTitle = nullptr);
	void clear();
	jvxSize numberEntries();
	void toString(ParamValue _valueNormalized, String128 string)  const SMTG_OVERRIDE;
protected:
	std::string empty_show;
};

} // namespace Vst
} // namespace Steinberg

#endif
