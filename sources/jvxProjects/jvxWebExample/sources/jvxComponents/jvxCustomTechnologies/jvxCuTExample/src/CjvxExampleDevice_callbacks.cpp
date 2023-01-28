#include "CjvxExampleDevice.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE
{
#endif
	
/*
JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxExampleDevice, cb_example_pramirq0_read_trigger_pre)
{
    jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
    return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxExampleDevice, cb_example_pramirq0_read_trigger_post)
{
    jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;

	return JVX_NO_ERROR;
}
*/
JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxExampleDevice, cb_example_field1_read_memory_get_pre)
{
	if (
		(genExample::field1.memory.globalIdx == ident.id) &&
		(genExample::field1.memory.category == ident.cat))
	{
		jvxDspBaseErrorType resL = jvx_example_field1_read_memory(&runtime.inspField1);
		assert(resL == JVX_DSP_NO_ERROR);
	}

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxExampleDevice, cb_example_field2_read_memory_get_pre)
{
	if (
		(genExample::field2.memory.globalIdx == ident.id) &&
		(genExample::field2.memory.category == ident.cat))
	{
		jvxDspBaseErrorType resL = jvx_example_field2_read_memory(&runtime.inspField2);
		assert(resL == JVX_DSP_NO_ERROR);
	}

	return JVX_NO_ERROR;
}

//============================================================================

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC(CjvxExampleDevice, started)
{
	return JVX_NO_ERROR;
}

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC(CjvxExampleDevice, stopped)
{
	return JVX_NO_ERROR;
}

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_IP(CjvxExampleDevice, expired)
{
	jvxErrorType res = JVX_NO_ERROR;
	return JVX_NO_ERROR;
}

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_IP(CjvxExampleDevice, wokeup)
{
	return JVX_NO_ERROR;
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
