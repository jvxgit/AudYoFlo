#include "jvxNodes/CjvxNVTasks.h"

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxNVTasks, select_task)
{
	jvxBool trigger_update_ui = false;
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select))
	{
		show_properties_io_task(&trigger_update_ui);	
	}
	if (trigger_update_ui)
	{
		jvxCBitField prio;
		jvx_bitZSet(prio, JVX_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		_report_command_request(prio);
	}
	return JVX_NO_ERROR;
}

void
CjvxNVTasks::show_properties_io_task(jvxBool* trigger_update_ui)
{
	jvxSize cnt = 0;
	jvxSize selId = jvx_bitfieldSelection2Id(
		CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select.value.selection(),
		CjvxNVTasks_pcg::selected_vtask_properties.variable_tasks.select.value.entries.size()
	);
	if (trigger_update_ui)
	{
		*trigger_update_ui = false;
	}
	if (JVX_CHECK_SIZE_SELECTED(selId))
	{
		auto elmT = _common_set_node_vtask.variableTasks.begin();
		for (; elmT != _common_set_node_vtask.variableTasks.end(); elmT++)
		{
			jvxBool foundit = false;
			auto elmR = elmT->second.activeRuntimeTasks.begin();
			for (; elmR != elmT->second.activeRuntimeTasks.end(); elmR++)
			{
				if (cnt == selId)
				{
					show_properties_io_task_vtask(elmR->second);
					if(trigger_update_ui)
					{
						*trigger_update_ui = true;
					}
					foundit = true;
					break;
				}
				cnt++;
			}
			if (foundit)
			{
				break;
			}
		}
	}
}
