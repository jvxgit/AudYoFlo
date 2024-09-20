#ifndef __CJVXVIDEOV4L2DEVICE_H__
#define __CJVXVIDEOV4L2DEVICE_H__

#include "jvxVideoTechnologies/CjvxVideoDevice.h"

#include "jvx_threads.h"

#include <memory>
#include <thread>
#include <atomic>
#include <vector>

#define VERBOSE_1
#define VERBOSE_2
#define ALPHA 0.95

#include "pcg_exports_device.h"
class CjvxVideoV4L2Technology;

class CjvxVideoV4L2Device: public CjvxVideoDevice,
	public genV4L2_device
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

	typedef struct
	{
		void *ptr;
		size_t length;
	} Buffer;

private:

	CjvxVideoV4L2Technology* myParent;

	std::thread read_frame_thread;
	std::vector<Buffer> buffers;

	std::string device;
	int dev_fd;

	//long mRefCount;
	std::vector<jvxVideoSetting> lstModes;

	struct
	{
		jvxSize szElement;
		jvxSize szLine;
		jvxSize szRaw;
	//	jvxData curRate;
		jvxInt64 lastTime;
		jvxData avrgT;
		jvxSize inputOverrun;
		jvxSize numFrames;
		jvxState streamState;
	} runtime;

	//JVX_MUTEX_HANDLE safeAccess;
	jvxTimeStampData tStamp;

	static int xioctl(int fd, int request, void *arg);

	void start_camera();
	void stop_camera();

	int start_capturing();
	void stop_capturing();

	int init_buffers();
	void free_buffers();

	void read_frame_thread_main();
	int read_frame();

public:
	JVX_CALLINGCONVENTION CjvxVideoV4L2Device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxVideoV4L2Device();

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

	//void updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);
	void updateDependentVariables_nolock();

	jvxErrorType init(CjvxVideoV4L2Technology* par, const char* device, jvxBool isDefaultDevice);
	void scanProperties();
	jvxErrorType set_device_mode(jvxSize idModeSelect);

	jvxErrorType prepare_data(jvxBool runPrepare JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
};

#endif
