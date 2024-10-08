#ifndef __CJVXVIDEOMFWINDOWSDEVICE_H__
#define __CJVXVIDEOMFWINDOWSDEVICE_H__

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
class CjvxVideoMfWindowsTechnology;

class CjvxVideoMfWindowsDevice: public CjvxVideoDevice,  
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
		jvxDataFormatGroup subform;
		jvxData fps_min;
		jvxData fps_max;
		jvxData fps;
		std::string descriptor;
	} jvxVideoSetting;

private:

	CjvxVideoMfWindowsTechnology* myParent;

	IMFActivate* thisisme;
	IMFMediaSource* srcPtr;
	IMFSourceReader* readPtr;

	long mRefCount;
	std::vector<jvxVideoSetting> lstModes;

	struct
	{
		jvxInt32 stride; // Stride may be negative!!
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
			jvxSize szElement = 0;
			jvxSize szElementsLine = 0;
			jvxSize szElementsField = 0;
		} params_sw;

		struct
		{
			jvxSize segWidth = 0;
			jvxSize segHeight = 0;
			jvxSize plane1_Sz = 0;
			jvxDataFormatGroup form_hw = jvxDataFormatGroup::JVX_DATAFORMAT_GROUP_NONE;
		} convertOnRead;
	} runtime;

	JVX_MUTEX_HANDLE safeAccess;
	jvxTimeStampData tStamp;

public:
	JVX_CALLINGCONVENTION CjvxVideoMfWindowsDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxVideoMfWindowsDevice();

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

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(on_mode_selected);

	IMFAttributes * bootup_camera();
	void shutdown_camera();

	//void updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);
	void updateDependentVariables_nolock();

	jvxErrorType init(CjvxVideoMfWindowsTechnology* par, IMFActivate* inPtr, jvxBool isDefaultDevice);
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
