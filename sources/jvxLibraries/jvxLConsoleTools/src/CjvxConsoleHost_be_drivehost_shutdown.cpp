#include "CjvxConsoleHost_be_drivehost.h"
#include "jvxHJvx.h"

#if 0
void 
CjvxConsoleHost_be_drivehost::terminateHost()
{
	jvxSize i;
	jvxState theState = JVX_STATE_NONE;
	jvxErrorType res = JVX_NO_ERROR;
	if (this->involvedComponents.theHost.hHost)
	{
		// ====================================================================
		// Stop main central widget
		//subWidgets.main.theWidget->inform_about_to_shutdown();

		(static_cast<IjvxObject*>(this->involvedComponents.theHost.hHost))->state(&theState);
		//this->involvedComponents.theHost.hHost->state(&theState);

		assert(theState > JVX_STATE_INIT);

		// Here, we deactivate all dynamic objects found in slots. We need to do this to
		// allow text log in deactivate callbacks
		for (i = JVX_COMPONENT_UNKNOWN + 1; i < JVX_COMPONENT_LIMIT; i++)
		{
			jvxSize j, k;
			jvxSize szSlots = 0;
			jvxSize szSubSlots = 0;
			jvxComponentType tpParent = JVX_COMPONENT_UNKNOWN;
			jvxState stat = JVX_STATE_NONE;
			jvxComponentIdentification tpLoc((jvxComponentType)i, 0, 0);

			this->involvedComponents.theHost.hHost->number_slots_component(tpLoc, &szSlots, NULL, NULL);
			for (j = 0; j < szSlots; j++)
			{
				jvxSize szSubSlots = 0;
				tpLoc.slotid = j;
				this->involvedComponents.theHost.hHost->number_slots_component(tpLoc, NULL, &szSubSlots, NULL);
				for (k = 0; k < szSubSlots; k++)
				{
					tpLoc.slotsubid = k;
					this->involvedComponents.theHost.hHost->state_component(tpLoc, &stat);
					if (stat == JVX_STATE_ACTIVE)
					{
						this->involvedComponents.theHost.hHost->deactivate_component(tpLoc);
						//subWidgets.main.theWidget->inform_inactive(tpLoc);
					}
				}
			}
		}

		// The deactivate function in the host checks that no component is left unterminated
		this->involvedComponents.theHost.hHost->deactivate();

		shutdown_specific();

		this->involvedComponents.theHost.hHost->unselect();
		this->involvedComponents.theHost.hHost->terminate();		

		// We changed the order: components may unregister tools on deactivate!
		if (involvedComponents.theTools.hTools)
		{
			involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(involvedComponents.theTools.hTools));
			involvedComponents.theTools.hTools = NULL;
		}

		jvxHJvx_terminate(this->involvedComponents.theHost.hobject);
		this->involvedComponents.theHost.hobject = NULL;
		this->involvedComponents.theHost.hHost = NULL;
	}
}

#endif