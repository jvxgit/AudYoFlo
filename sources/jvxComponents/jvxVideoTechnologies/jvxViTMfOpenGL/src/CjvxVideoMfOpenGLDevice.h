#ifndef __CJVXVIDEOMFOPENGLDEVICE_H__
#define __CJVXVIDEOMFOPENGLDEVICE_H__

#include "jvxVideoTechnologies/CjvxVideoDevice.h"

#ifdef JVX_USE_GLEW_GLUT
#include "jvxVideoRenderCore.h"
#endif

#include "jvx_threads.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#define VERBOSE_1
#define VERBOSE_2
#define ALPHA 0.95

#include "pcg_exports_device.h"
class CjvxVideoMfOpenGLTechnology;

class CjvxVideoMfOpenGLDevice: public CjvxVideoDevice,  
	public IMFSourceReaderCallback, 
#ifdef JVX_USE_GLEW_GLUT
	public jvxVideoRenderCore_nobc,
#endif
	public genMf_device
{
	typedef struct
	{
		int width;
		int height;
		jvxSize id;
		jvxDataFormat form;
		jvxDataFormatGroup subform_hw;
		jvxDataFormatGroup subform_sw;
		jvxData fps_min;
		jvxData fps_max;
		jvxData fps;
		std::string descriptor;
	} jvxVideoSetting;

private:

	typedef enum
	{
		JVX_GL_DO_NOT_RENDER = 0,
		JVX_GL_RENDER_EXTERNAL = 1
#ifdef JVX_USE_GLEW_GLUT
		, JVX_GL_RENDER_NATIVE = 2
#endif

	}jvxOpenGlRenderTarget;

	CjvxVideoMfOpenGLTechnology* myParent;

	IMFActivate* thisisme;
	IMFMediaSource* srcPtr;
	IMFSourceReader* readPtr;

	long mRefCount;
	std::vector<jvxVideoSetting> lstModes;

	struct
	{
		jvxInt32 stride; // Stride may be negative!!
		jvxSize szElement;
		jvxSize szLine;
		jvxSize szRaw;
		jvxData curRate;
		jvxInt64 lastTime;
		jvxData avrgT;
		jvxSize inputOverrun;
		jvxSize numFrames;
		//jvxSize numBuffers;
		//jvxSize idxBuffer;
		jvxState streamState;

		struct
		{
			jvxBool inConvertBufferInUse = false;
			jvxDataFormatGroup form_hw = jvxDataFormatGroup::JVX_DATAFORMAT_GROUP_NONE;
			jvxData szElement = 0;
			jvxSize lField = 0;
			jvxByte* bufRead = nullptr;
			jvxSize segWidth = 0;
			jvxSize segHeight = 0;
			jvxSize plane1_Sz = 0;
			jvxSize plane2_offset = 0;
			jvxSize plane2_Sz = 0;
		} convertOnRead;
	} runtime;

	JVX_MUTEX_HANDLE safeAccess;
	jvxTimeStampData tStamp;

	struct
	{
		JVX_THREAD_HANDLE hdlThread;
		JVX_THREAD_ID idThread;

		jvxSize width;
		jvxSize height;
		jvxHandle*** bufs;
		jvxSize szFld;
		jvxSize numChannels;
		jvxSize numBufs;
		jvxDataFormat form;
		jvxDataFormatGroup subform;

		jvxBool threadIsInitialized;
		jvxSize idxBuf_Read;
		jvxSize idxBuf_Fheight;
		JVX_MUTEX_HANDLE safeAccessBuffer;

		jvxBool requestStop;
		jvxOpenGlRenderTarget renderingTarget;
	} nativeGl;


public:
	JVX_CALLINGCONVENTION CjvxVideoMfOpenGLDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxVideoMfOpenGLDevice();

	jvxErrorType select(IjvxObject* owner) override;
	jvxErrorType unselect() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	// Interface IjvxConfiguration
	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename = "", jvxInt32 lineno = -1)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================
	// Master interface
	// ===================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ===================================================================================
	// New linkdata connection interface
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset, 
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage, 
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION install_property_reference(jvxCallManagerProperties& callGate,
			const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION uninstall_property_reference(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(on_mode_selected);

#ifdef JVX_USE_GLEW_GLUT
	void run_native_gl();
	void do_rendering_gl();
	void do_idle_gl();
#endif

	IMFAttributes * bootup_camera();
	void shutdown_camera();

#if 0

	virtual jvxErrorType JVX_CALLINGCONVENTION select();

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect();

#endif

	//void updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);
	void updateDependentVariables_nolock();

	jvxErrorType init(CjvxVideoMfOpenGLTechnology* par, IMFActivate* inPtr, jvxBool isDefaultDevice);
	void scanProperties(IMFSourceReader* reader);

	jvxErrorType prepare_data(jvxBool runPrepare JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// ====================================================================
	STDMETHODIMP QueryInterface(REFIID aRiid, void** aPpv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP OnReadSample(
		HRESULT aStatus,
		DWORD aStreamIndex,
		DWORD aStreamFlags,
		LONGLONG aTimestamp,
		IMFSample *aSample
	);
	STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *);
	STDMETHODIMP OnFlush(DWORD);

};

#endif
