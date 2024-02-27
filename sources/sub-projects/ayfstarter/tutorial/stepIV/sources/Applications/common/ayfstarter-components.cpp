// =============================================================
// Involve components from static lib
// =============================================================

#include "jvx.h"
#include "ayfAuNStarter.h"

#define JVX_ALGO_DESCRIPTION "ayfAuNStarter"
#define JVX_ALGO_INIT_FUNC ayfAuNStarter_init
#define JVX_ALGO_TERMINATE_FUNC ayfAuNStarter_terminate

#include "jvxApplications/jvx_init_link_objects_tpl.h"

// =============================================================
// Specify some runtime host configurations
// =============================================================
const char* componentsOnLoad_algorithms[] =
{
	"ayfAuNStarter",
	NULL
};

const char* componentsOnLoad_audiotechnologies[] =
{
	"jvxAuTGenericWrapper",
	nullptr
};
