#include "jvx.h"

#define JVX_ALGO_DESCRIPTION "Ayf Matlab"
#define JVX_ALGO_INIT_FUNC ayfAuNMatlab_init
#define JVX_ALGO_TERMINATE_FUNC ayfAuNMatlab_terminate
#include "ayfAuNMatlab.h"

#include "jvxApplications/jvx_init_link_objects_tpl.h"


// =============================================================
// Specify some runtime host configurations
// =============================================================
#include "interfaces/all-hosts/configHostFeatures_common.h"

// The positions of the components are stored within these tables
const jvxModuleOnStart componentsOnLoad_algorithms[] =
{
	{"ayfAuNMatlab"},
	NULL
};

const jvxModuleOnStart componentsOnLoad_audiotechnologies[] =
{
	{"jvxAuTGenericWrapper"},
	nullptr
};

jvxErrorType configureHost_features_local(configureHost_features* theFeaturesF)
{
	theFeaturesF->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_NODE] = true;
	theFeaturesF->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_NODE] = componentsOnLoad_algorithms;

	theFeaturesF->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_TECHNOLOGY] = true;
	theFeaturesF->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_TECHNOLOGY] = componentsOnLoad_audiotechnologies;

	return(JVX_NO_ERROR);
}

jvxErrorType configureHost_invalidate_features_local(configureHost_features* theFeaturesF)
{
	return(JVX_NO_ERROR);
}

