#include "jvx.h"

#include "jvxHosts/CjvxHost_dll.h"

CjvxHost_dll::CjvxHost_dll(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{

}

CjvxHost_dll::~CjvxHost_dll()
{
}

void
CjvxHost_dll::load_dynamic_objects()
{
	this->loadAllComponents(JVX_EVALUATE_BITFIELD(CjvxHost_genpcg::properties_selected.do_unload_dlls.value.selection() & 0x1), 
			CjvxHost_genpcg::properties_selected.component_path.value, (CjvxHost_genpcg::properties_selected.host_output_cout.value == c_true),
			(CjvxHost_genpcg::properties_selected.host_verbose_dll.value == c_true));
}

void 
CjvxHost_dll::unload_dynamic_objects()
{
	this->unloadAllComponents();
}



