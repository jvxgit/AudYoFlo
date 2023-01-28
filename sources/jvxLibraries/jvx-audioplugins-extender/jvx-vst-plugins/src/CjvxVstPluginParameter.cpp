#include "CjvxVstPluginParameter.h"
#include "pluginterfaces/base/ustring.h"

#include "jvx.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// GainParameter Implementation
//------------------------------------------------------------------------
jvxVstGainParameter::jvxVstGainParameter(int32 flags, int32 id,
	const char* nm, ParamValue offsetShiftArg,
	ParamValue scaleFacArg, const char* unit, jvxSize numDigitsArg )
{
	Steinberg::UString (info.title, 
		USTRINGSIZE (info.title)).assign (USTRING (nm));
	Steinberg::UString (info.units, 
		USTRINGSIZE (info.units)).assign (USTRING (unit));

	info.flags = flags;
	info.id = id;
	info.stepCount = 0;
	info.defaultNormalizedValue = 0.5f;
	info.unitId = kRootUnitId;

	setNormalized (1.f);

	offsetShift = offsetShiftArg;
	scaleFac = scaleFacArg;
	units = unit;
	numDigits = numDigitsArg;
}

//------------------------------------------------------------------------
void jvxVstGainParameter::toString (ParamValue normValue, String128 string) const
{
	std::string txt = toStringStatic(normValue, 
		scaleFac, offsetShift, numDigits, units);
	Steinberg::UString(string, 128).fromAscii(txt.c_str());

	/*
	char text[32];
	normValue = normValue * scaleFac;
	if (offsetShift)
	{
		normValue -= offsetShift;
	}
	if (normValue > 0)
	{
		sprintf(text, "+%.1f %s", ((float)normValue), units.c_str());
	}
	else
	{
		sprintf(text, "%.1f %s", ((float)normValue), units.c_str());
	}
	Steinberg::UString (string, 128).fromAscii (text);
	*/
}

void 
jvxVstGainParameter::get_show_params(jvxData& offs, jvxData& scale, jvxSize& nDigs, std::string& unitReturn)
{
	offs = offsetShift;
	scale = scaleFac;
	unitReturn = units;
	nDigs = numDigits;
}

std::string
jvxVstGainParameter::toStringStatic(ParamValue normValue, 
	jvxData scaleFac, jvxData offsetShift, jvxSize numDigits, const std::string& units)
{
	std::string txtOut;
	normValue = normValue * scaleFac;
	if (offsetShift)
	{
		normValue -= offsetShift;
	}
	if (normValue > 0)
	{
		txtOut = "+";
	}
	txtOut += jvx_data2String(normValue, numDigits); 
	if (!units.empty())
	{
		txtOut += " ";
		txtOut += units;
	}
	return txtOut;
}

//------------------------------------------------------------------------
bool jvxVstGainParameter::fromString (const TChar* string, ParamValue& normValue) const
{
	String wrapper ((TChar*)string); // don't know buffer size here!
	double tmp = 0.0;
	if (wrapper.scanFloat (tmp))
	{		
		// Value in dB is mapped to 0..1
		tmp = tmp + offsetShift;
		tmp *= 1./scaleFac;
		normValue = JVX_MIN(JVX_MAX(tmp, 0), 1);
		return true;
	}
	return false;
}

void jvxVstGainParameter::toUnnormalized(jvxData& unNormValue) const
{
	jvxData norm = this->getNormalized();
	unNormValue = norm * scaleFac;
	if (offsetShift)
	{
		unNormValue -= offsetShift;
	}
}

//------------------------------------------------------------------------

jvxVstStringListParameter::jvxVstStringListParameter(const TChar* title, ParamID tag, 
	const char* empty_show_arg, const TChar* units,
	int32 flags, UnitID unitID, const TChar* shortTitle) :
	StringListParameter(title, tag, units, flags, unitID, shortTitle)
{
	if (empty_show_arg)
	{
		empty_show = empty_show_arg;
	}
	else
	{
		empty_show = "none";
	}
}

void
jvxVstStringListParameter::clear()
{
	this->strings.clear();
	info.stepCount = -1;
}

jvxSize
jvxVstStringListParameter::numberEntries()
{
	return this->strings.size();
}

void 
jvxVstStringListParameter::toString(ParamValue _valueNormalized, String128 string)  const
{
	int32 index = (int32)toPlain(_valueNormalized);
	if (index < strings.size())
	{
		StringListParameter::toString(_valueNormalized, string);
	}
	else
	{
		Steinberg::UString(string, 128).fromAscii(empty_show.c_str());
		//string[0] = 0;
	}
}

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
