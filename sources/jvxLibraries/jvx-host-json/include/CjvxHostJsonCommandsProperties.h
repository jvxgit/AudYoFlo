#ifndef _CJVXHOSTJSONCOMMANDSSHOWMIN_H__
#define _CJVXHOSTJSONCOMMANDSSHOWMIN_H__

#include "CjvxHostJsonProcessCommand.h"
#include "CjvxHostJsonCommandsConfigShow.h"

class CjvxHostJsonCommandsProperties
{
public:

	CjvxHostJsonCommandsConfigShow& config_show_ref;

public:
	CjvxHostJsonCommandsProperties(CjvxHostJsonCommandsConfigShow& cfg):
		config_show_ref(cfg)
	{
	};

	/*
	 * Accept an inner command of type 
	  <component>, properties, list, [/c*, /a*]
	 */
	jvxErrorType show_property_list(jvxComponentIdentification tp,
		const std::string& filter_purpose, const std::string& filter_prop_str,
		CjvxJsonElementList& jelmlst_ret, std::string& errTxt);

	// =============================================================================

	/*
	* 	 get_property: Displays the current property value for dedicated property.
								 multiple properties can be grouped in brackets [], each entry can be in parethesis () with additional parameters,
								 single entry for property with <property descriptor> OR <property_id>; <offset> (opt); <num_elms> (opt), <content_only> (opt), <context> (opt).
								 optional additional arguments <addArg>="cC" yields to return content_only and/or compact version of the property.
		Example for a returned property value, selection list:
		act(audio node, get_property, /system/sel_engage)
			{
                "return_code": "JVX_NO_ERROR",
                "descriptor": "/system/sel_engage",
                "format": "selection list",
                "offset": 0,
                "number_elements": 1,
                "decoder_type": "single selection",
                "access_type": "full read + write",
                "valid": "*",
                "property": "0x00000001::0x00000003::0x00000003::2",
                "property_fld":
                [
                        "Engage",
                        "Do not engage"
                ]
			}
		Example for a returned property value, single value:
		COMMAND: act(audio node, get_property, /volume.)
		{
				"return_code": "JVX_NO_ERROR",
				"descriptor": "/volume",
				"format": "jvxData(single)",
				"offset": 0,
				"number_elements": 1,
				"decoder_type": "none",
				"access_type": "full read + write",
				"valid": "*",
				"property": "1"
		}
		Example for selection list with "compact mode on"
		COMMAND: act[C](audio node, get_property, /system/sel_engage.)
		{
				"return_code": "JVX_NO_ERROR",
				"d": "0x353424e3",
				"p": "0x00000001::0x00000003::0x00000003::2",
				"pf":
				[
						"Engage",
						"Do not engage"
				]
		}
	 */
	jvxErrorType act_get_property_component_core(jvxComponentIdentification tp, 
		const std::string& props, const std::vector<std::string>& args,
		jvxSize off, CjvxJsonElementList& jelmret, jvxBool content_only, 
		jvxBool compact, std::string& errTxt);

	jvxErrorType show_property_component(
		jvxComponentIdentification tp,
		const std::string identificationTarget,
		const std::vector<std::string>& args,
		jvxSize off,
		CjvxJsonElementList& jelmret,
		jvxBool content_only,
		jvxBool compact, std::string& errTxt);

	jvxErrorType show_property_component_list(
		jvxComponentIdentification tp,
		const std::string& args,
		CjvxJsonElementList& jelmret,
		jvxBool content_only,
		jvxBool compact, std::string& errTxt);

	// ==============================================================================================
	/*
	 * Function to set a property. Here is what the function expects:
	 * Modifies the current property value for dedicated properties.
	 * multiple properties can be grouped in brackets [] with ";" to separate the distinct "set" procedures
	 * Each entry can be in parethesis () with additional parameters.
	 * A single entry for a property-set is composed of the <property descriptor> followed by the <value to pass> and an optional <offset>.
	 * The command can be run with the "additional arg" "r" to emit a report_set event to all listeners.
	 * The value may contain different entries for each datatypes - it is obvious what to pass for simple values but
	 * it is more difficult for value-in-range or selection lists:
	 * 1) All simple datatypes: pass new value as string, it will be converted. Note that size specific presets can be accepted
	 *    such as "dontcare".
	 * 2) Value-in-range: <value>[::<min>::<max>]
	 * 3) selection list: <value-selection>[::<selectable>[::<exclusive>[::<sellst-length>[::<sellst-options>]]]]] 
	 * 
	 * Example: Set selection-list property:
	 * COMMAND: act(audio node, set_property, /system/sel_engage, 0x1::0x1::0x1::3::a:b:c)
	 * a) Set selection:		0x1::xxx::xxx::x::xxxxx
	 * b) Set selectable:		xxx::0x1::xxx::x::xxxxx
	 * c) Set exclusive:		xxx::xxx::0x1::x::xxxxx
	 * d) Set number entries:	xxx::xxx::xxx::3::xxxxx
	 * e) Set entries in list:	xxx::xxx::xxx::x::a:b:c  <- supersedes number of entries d)
	 * 
	 * 
	 */
	jvxErrorType act_set_property_component_core(jvxComponentIdentification tp, const std::string& identificationTarget,
		const std::string& loadTarget, const std::string& offsetStart, CjvxJsonElementList& jelmret, jvxBool reportSet, std::string& errTxt);

	void attach_single_property_json_list(jvxComponentIdentification tp,
		const std::string& propDescr, std::vector<std::string>& argsIn, jvxSize off,
		CjvxJsonElementList& jelmlst_props, jvxBool content_only, jvxBool compact);

	jvxErrorType act_set_property_component_single(jvxComponentIdentification tp,
		const std::string& identificationTarget,
		const std::string& loadTarget, const std::string& offsetStart, CjvxJsonElementList& jelmret, jvxBool reportSet, std::string& errTxt);

	jvxErrorType act_set_property_component_list(
		jvxComponentIdentification tp,
		const std::string& props, CjvxJsonElementList& jelmret, jvxBool reportSet, std::string& errTxt);

	// ================================================================================

	virtual jvxErrorType requestReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp) = 0;
	virtual jvxErrorType returnReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp) = 0;

};

#endif