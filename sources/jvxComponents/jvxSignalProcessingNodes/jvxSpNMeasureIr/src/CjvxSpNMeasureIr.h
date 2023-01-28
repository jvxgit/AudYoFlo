
#include "jvxNodes/CjvxBareNode1io_rearrange.h"
#include "CjvxSpNMeasureIr_oneMeasurement.h"
#include "pcg_exports_node.h"

#define JVX_COMPOSE_MEASUREMENT(cnt) ("Measurement_" + jvx_size2String(cnt))

class CjvxSpNMeasureIr : public CjvxBareNode1io_rearrange, public IjvxSpNMeasureIr_oneMeasurement_report, public genMeasureIr_node
{

private:

	std::list<CjvxSpNMeasureIr_oneMeasurement*> measurements;	
	jvxSize selMeasures = JVX_SIZE_UNSELECTED;
	std::list<oneMeasurementResult> collectedResults;

	struct inProcessingT
	{
		jvxInt32 idMeasure = JVX_SIZE_UNSELECTED_INT32;
		std::vector< CjvxSpNMeasureIr_oneMeasurement*> activeMeasures;
		CjvxSpNMeasureIr_oneMeasurement* currentMeasure = nullptr;
		jvxBool completed = false;
		jvxBool completeReported = false;
		jvxBool do_start = false;
		jvxInt32 cntDownStart = 0; // <- must be signed!
	};

	inProcessingT inProcessing;

	struct localTextLogT
	{
		jvxSize sz = 0;
		jvxBool use = false;

		IjvxObject* theobj = nullptr;
		IjvxLocalTextLog* thehdl = nullptr;

		std::string separatorToken;
	};
	
	localTextLogT localTextLog;

public:

	JVX_CALLINGCONVENTION CjvxSpNMeasureIr(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNMeasureIr();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	void JVX_CALLINGCONVENTION test_set_output_parameters() override;
	virtual jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	// ========================================================================
	// Properties
	// ========================================================================

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_config_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_generator_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_measurement_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_measurement_prehook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(add_entry);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(remove_entry);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(trigger_start);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_all_channels_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_monitor_prehook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_evaluation_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_result_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_measurement_data_prehook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_result_import_measurement);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_config_measurement_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_config_measurement_prehook);


	// ========================================================================
	// Configuration
	// ========================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	void put_configuration_all_measurements(jvxConfigData* secRead, IjvxConfigProcessor* processor);

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	void get_configuration_one_measurement(CjvxSpNMeasureIr_oneMeasurement* oneM, jvxSize cnt,
		jvxConfigData* secWrite, IjvxConfigProcessor* processor);

	// ========================================================================
	// Helpers
	// ========================================================================

	void reconstruct_properties(jvxSize selMeasuresNew, jvxBool triggerUpdate = true);
	void update_generator_properties();

	// ========================================================================

	void channels_from_properties();
	void channels_to_properties();

	// ========================================================================

	void generator_from_props(CjvxSpNMeasureIr_oneMeasurement* elm);
	void generator_to_props(CjvxSpNMeasureIr_oneMeasurement* elm);

	void evaluation_from_props(CjvxSpNMeasureIr_oneMeasurement* elm);
	void evaluation_to_props(CjvxSpNMeasureIr_oneMeasurement* elm);

	void set_measurement_idx();

	// ========================================================================

	void allInPutOnOff(CjvxSpNMeasureIr_oneMeasurement* elm, jvxBool allInOn, jvxBool allInOff, jvxBool allOutOn, jvxBool allOutOff);

	std::string correctNameAndSet(std::string nm, CjvxSpNMeasureIr_oneMeasurement* elm);

	void update_plot_measurements();
	void update_plot_channels(jvxSize idSelMeas);

	void read_result(const std::string fName, jvxBool& updateUi);
	void callback_report_txt(const std::string& txt);
	virtual void jvxSpNMeasureIr_reportText(const std::string& txt) override;

	void clear_all_results();
};