#include "CjvxExampleDevice.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE
{
#endif

#define strsize 32
static char str[strsize] = { 0 };
#define JVX_DARWIN_VERBOSE
#define JVX_TIMEOUT_EREIGNIS_GET_MS 200

// =====================================================================

CjvxExampleDevice::CjvxExampleDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
    CjvxCustomDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
	{
		jvx_lock_text_log(jvxrtst_bkp);
		jvxrtst << "::" << __FUNCTION__ << ": " << "constructor of " << _common_set.theDescriptor << " device component" << std::endl;
		jvx_unlock_text_log(jvxrtst_bkp);
	}

	JVX_THREADS_FORWARD_C_CALLBACK_RESET(thread_ascan.fptr);
	JVX_THREADS_FORWARD_C_CALLBACK_ASSIGN(thread_ascan.fptr, CjvxExampleDevice, started, stopped, expired, wokeup);
}

CjvxExampleDevice::~CjvxExampleDevice()
{
	/*
	 * NO: the log file has long been deactivated in deactivate!!
	 if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
	{
		jvx_lock_text_log(jvxrtst_bkp);
		jvxrtst << "::" << __FUNCTION__ << ": " << "destructor of " << _common_set.theDescriptor << " device component" << std::endl;
		jvx_unlock_text_log(jvxrtst_bkp);
	}
	*/
}

jvxErrorType
CjvxExampleDevice::activate()
{
	jvxErrorType res = CjvxCustomDevice::activate();
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << ": " << "activate " << _common_set.theDescriptor << " device component" << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << ": " << "initialization of core library." << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		// Ascan read interface
		resL = jvx_example_field1_initCfg(&runtime.inspField1);
		assert(resL == JVX_DSP_NO_ERROR);

		resL = jvx_example_field1_init(&runtime.inspField1);
		assert(resL == JVX_DSP_NO_ERROR);

		resL = jvx_example_field2_initCfg(&runtime.inspField2);
		assert(resL == JVX_DSP_NO_ERROR);

		resL = jvx_example_field2_init(&runtime.inspField2);
		assert(resL == JVX_DSP_NO_ERROR);

		// =======================================================================

		genExample::init_all();
		genExample::allocate_all();

		genExample::associate__field1(
			static_cast<CjvxProperties*>(this),
			(jvxInt32*)runtime.inspField1.prmSync.fld, runtime.inspField1.prmSync.fld_num);

		genExample::associate__field2(
			static_cast<CjvxProperties*>(this),
			(jvxInt32*)runtime.inspField2.prmSync.fld, runtime.inspField2.prmSync.fld_num);

		genExample::register_callbacks(static_cast<CjvxProperties*>(this),
			//cb_example_ascan_read_trigger_pre, 
			//cb_example_ascan_read_trigger_post,
			cb_example_field1_read_memory_get_pre,
			cb_example_field2_read_memory_get_pre,
			reinterpret_cast<jvxHandle*>(this), NULL);

		genExample::register_all(static_cast<CjvxProperties*>(this));

		//startThread();

	}
	return res;

}

jvxErrorType
CjvxExampleDevice::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	jvxDspBaseErrorType resL;
	if (res == JVX_NO_ERROR)
	{
		//stopThread();

		genExample::unregister_all(static_cast<CjvxProperties*>(this));
		genExample::deassociate__field1(static_cast<CjvxProperties*>(this));
		genExample::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);
		genExample::deallocate_all();

		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << ": " << "termination of core library." << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}
		
		resL = jvx_example_field2_terminate(&runtime.inspField2);
		assert(resL == JVX_DSP_NO_ERROR);

		resL = jvx_example_field1_terminate(&runtime.inspField1);
		assert(resL == JVX_DSP_NO_ERROR);

		res = CjvxCustomDevice::deactivate();
		assert(res == JVX_NO_ERROR);
	}
	return res;
}


jvxErrorType
CjvxExampleDevice::prepare()
{
    jvxSize num = 0;
    jvxErrorType res = CjvxCustomDevice::prepare();
    if (res == JVX_NO_ERROR)
    {
    }
    return res;
}

jvxErrorType
CjvxExampleDevice::postprocess()
{
    jvxErrorType res = CjvxCustomDevice::postprocess();
    if (res == JVX_NO_ERROR)
    {
	
    }
    return res;
}

jvxErrorType
CjvxExampleDevice::start()
{
    jvxErrorType res = CjvxCustomDevice::start();
    if (res == JVX_NO_ERROR)
    {

    }
    return res;
}

jvxErrorType
CjvxExampleDevice::stop()
{
    jvxErrorType res = CjvxCustomDevice::stop();
    if (res == JVX_NO_ERROR)
    {

    }
    return res;
}

jvxErrorType
CjvxExampleDevice::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename, jvxInt32 lineno)
{
    std::vector<std::string> warns;
    jvxErrorType res = CjvxCustomDevice::put_configuration(callConf, processor,
							   sectionToContainAllSubsectionsForMe,
							   filename, lineno);
    if (res == JVX_NO_ERROR)
    {
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
	    genExample::put_configuration_all(callConf, processor,
					     sectionToContainAllSubsectionsForMe,
					     &warns);
	}
    }
    return res;
}

jvxErrorType
CjvxExampleDevice::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
    jvxErrorType res = CjvxCustomDevice::get_configuration(callConf, processor,
							   sectionWhereToAddAllSubsections);
    if (res == JVX_NO_ERROR)
    {
	genExample::get_configuration_all(callConf, processor,
					 sectionWhereToAddAllSubsections);
    }
    return res;
}

jvxErrorType
CjvxExampleDevice::set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = CjvxCustomDevice::set_property(
		callGate, rawPtr, ident, trans);

    if (res == JVX_NO_ERROR)
    {

    }
    return res;
}


#ifdef JVX_PROJECT_NAMESPACE
}
#endif

