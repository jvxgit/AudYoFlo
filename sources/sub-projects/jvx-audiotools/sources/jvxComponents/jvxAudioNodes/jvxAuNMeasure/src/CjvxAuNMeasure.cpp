#include "jvx.h"
#include "CjvxAuNMeasure.h"

#define LOGFILE_NUMBER_FRAMES_MEMORY 100
#define LOGFILE_STOP_TIMEOUT_MS 4000

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxAuNMeasure::CjvxAuNMeasure(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioNode_logfile(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected( JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);
	myWaveGenerators = NULL;
	currentMode = JVX_AUNMEASURE_OPERATION_MODE_NONE;
	uniqueId = 0;
	myLanes.selectionId = JVX_SIZE_UNSELECTED;
	myMeasurements.selectionId = JVX_SIZE_UNSELECTED;
	viewStruct_channel.theCircBufferHeader = NULL;
	viewStruct_timing.theCircBufferHeader = NULL;

	CjvxAudioNode_logfile::logger_prop_prefix = "jvxAuNMeasure";

	CjvxAudioNode_logfile_genpcg::init__audiologtofile__properties_logtofile();
}

CjvxAuNMeasure::~CjvxAuNMeasure()
{
}

jvxErrorType
CjvxAuNMeasure::activate()
{
	jvxBool updateUi1 = false;
	jvxErrorType res = CjvxAudioNode_logfile::activate();
	if(res == JVX_NO_ERROR)
	{
		genMeasure_node::init__properties_mode();
		genMeasure_node::allocate__properties_mode();
		genMeasure_node::register__properties_mode(static_cast<CjvxProperties*>(this));

		genMeasure_node::init__measureir__properties_measureIr();
		genMeasure_node::allocate__measureir__properties_measureIr();
		genMeasure_node::register__measureir__properties_measureIr(static_cast<CjvxProperties*>(this));

		genMeasure_node::init__measureir__properties_inproc();
		genMeasure_node::allocate__measureir__properties_inproc();
		genMeasure_node::register__measureir__properties_inproc(static_cast<CjvxProperties*>(this));

		genMeasure_node::init__measureir__properties_active_higher_all_Lanes();
		genMeasure_node::allocate__measureir__properties_active_higher_all_Lanes();
		genMeasure_node::register__measureir__properties_active_higher_all_Lanes(static_cast<CjvxProperties*>(this));

		genMeasure_node::init__measureir__properties_active_higher_active_Lane();
		genMeasure_node::allocate__measureir__properties_active_higher_active_Lane();
		genMeasure_node::register__measureir__properties_active_higher_active_Lane(static_cast<CjvxProperties*>(this));

		genMeasure_node::init__properties_common();
		genMeasure_node::allocate__properties_common();
		genMeasure_node::register__properties_common(static_cast<CjvxProperties*>(this));

		genMeasure_node::init__measureir__all_Measurements();
		genMeasure_node::allocate__measureir__all_Measurements();
		genMeasure_node::register__measureir__all_Measurements(static_cast<CjvxProperties*>(this));

		genMeasure_node::init__measureir__selected_Measurement();
		genMeasure_node::allocate__measureir__selected_Measurement();
		genMeasure_node::register__measureir__selected_Measurement(static_cast<CjvxProperties*>(this));

		genMeasure_node::init__measuretiming__resultsTiming();
		genMeasure_node::allocate__measuretiming__resultsTiming();
		genMeasure_node::register__measuretiming__resultsTiming(static_cast<CjvxProperties*>(this));

		genMeasure_node::init__measuretiming__runtimeTiming();
		genMeasure_node::allocate__measuretiming__runtimeTiming();
		genMeasure_node::register__measuretiming__runtimeTiming(static_cast<CjvxProperties*>(this));

#ifdef JVX_USE_SPECTRUM_PLOT
		genMeasure_node::init__expose_spectrogram__visual_data_if();
		genMeasure_node::allocate__expose_spectrogram__visual_data_if();
		genMeasure_node::register__expose_spectrogram__visual_data_if(static_cast<CjvxProperties*>(this));

		genMeasure_node::expose_spectrogram.visual_data_if.format.value = JVX_DATAFORMAT_BYTE;
		genMeasure_node::expose_spectrogram.visual_data_if.number_buffers.value = 1;
		// only on init: genMf_device::expose_visual_if.visual_data_if.operation_mode = ...;
		// only on set_ext_ref: genMf_device::expose_visual_if.visual_data_if.rendering_external = ...;
		genMeasure_node::expose_spectrogram.visual_data_if.segmentsize_x.value = 513;
		genMeasure_node::expose_spectrogram.visual_data_if.segmentsize_y.value = 1024;
		genMeasure_node::expose_spectrogram.visual_data_if.subformat.value = JVX_DATAFORMAT_GROUP_VIDEO_RGB24;
#endif

		genMeasure_node::register_callbacks(static_cast<CjvxProperties*>(this), set_buffer_post, set_buffer_pre, reinterpret_cast<jvxHandle*>(this), nullptr);

		jvx_factory_allocate_jvxIntegrateWaveGenerators(&myWaveGenerators);
		res = myWaveGenerators->activate(static_cast<CjvxProperties*>(this), "WaveGenerators");
		//res = myWaveGenerators->activate(&myProperties, "WaveGenerators");
		myWaveGenerators->setParameters(
			CjvxAudioNode_genpcg::properties_parameters.samplerate.value,
			CjvxAudioNode_genpcg::properties_parameters.buffersize.value,
			CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value,
				(jvxDataFormat)CjvxAudioNode_genpcg::properties_parameters.format.value,
				&updateUi1
				);

		if(updateUi1)
		{
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		}
	}
	return(res);
};

jvxErrorType
CjvxAuNMeasure::deactivate()
{
	jvxErrorType res = CjvxAudioNode_logfile::deactivate();
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{

		res = myWaveGenerators->deactivate();
		jvx_factory_deallocate_jvxIntegrateWaveGenerators(myWaveGenerators);

		genMeasure_node::unregister_callbacks(static_cast<CjvxProperties*>(this), nullptr);

#ifdef JVX_USE_SPECTRUM_PLOT
		genMeasure_node::unregister__expose_spectrogram__visual_data_if(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__expose_spectrogram__visual_data_if();
#endif

		genMeasure_node::unregister__properties_mode(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__properties_mode();

		genMeasure_node::unregister__measureir__properties_measureIr(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__measureir__properties_measureIr();

		genMeasure_node::unregister__measureir__properties_inproc(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__measureir__properties_inproc();

		genMeasure_node::unregister__measureir__properties_active_higher_all_Lanes(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__measureir__properties_active_higher_all_Lanes();

		genMeasure_node::unregister__measureir__properties_active_higher_active_Lane(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__measureir__properties_active_higher_active_Lane();

		genMeasure_node::unregister__properties_common(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__properties_common();

		genMeasure_node::unregister__properties_common(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__properties_common();

		genMeasure_node::unregister__measureir__all_Measurements(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__measureir__all_Measurements();

		genMeasure_node::unregister__measureir__selected_Measurement(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__measureir__selected_Measurement();

		genMeasure_node::unregister__measuretiming__resultsTiming(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__measuretiming__resultsTiming();

		genMeasure_node::unregister__measuretiming__runtimeTiming(static_cast<CjvxProperties*>(this));
		genMeasure_node::deallocate__measuretiming__runtimeTiming();

	}
	return(res);
};


jvxErrorType
CjvxAuNMeasure::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxCallManagerProperties callGateLoc;
	jvxBool updateUi1 = false;
	jvxBool updateUi2 = false;
	jvxBool updateUi3 = false;
	jvxBool updateUi4 = false;
	jvxBool modifiedLane = false;
	std::string command;
	std::string f_expr;
	std::string descr;
	std::vector<std::string> args;
	jvxApiString fldStr;
	jvxSize i;
	std::string path;
	jvxErrorType res = CjvxAudioNode_logfile::set_property(callGate, rawPtr, ident, trans);

	if(res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		/* If property for processing have changed */
		// Old style property setting. Should no longer be used but it may
		if(
			((propId == CjvxAudioNode_genpcg::properties_parameters.samplerate.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.samplerate.category)) ||
			((propId == CjvxAudioNode_genpcg::properties_parameters.buffersize.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.buffersize.category)) ||
			((propId == CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.category)) ||
			((propId == CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.globalIdx) && (category == CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.category)) ||
			((propId == CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.globalIdx) && (category == CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.category)))
		{
			// This must be here to support some old style interface connections, e.g. aec
			update_interfaces();
			/*
			myWaveGenerators->setParameters(
				JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.samplerate.value,
				JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.buffersize.value,
				JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value,
				(jvxDataFormat)JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.format.value,
				&updateUi1
				);
		
			genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries.clear();
			genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries.clear();
			genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.entries.clear();
			for (i = 0; i < JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value; i++)
			{
				genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries.push_back(("Input Channel #" + jvx_size2String(i)).c_str());
				genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries.push_back(("Input Channel #" + jvx_size2String(i)).c_str());
				genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.entries.push_back(("Input Channel #" + jvx_size2String(i)).c_str());

			}
			for (i = 0; i < JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value; i++)
			{
				genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries.push_back(("Output Channel #" + jvx_size2String(i)).c_str());
				genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries.push_back(("Output Channel #" + jvx_size2String(i)).c_str());
				genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.entries.push_back(("Output Channel #" + jvx_size2String(i)).c_str());
			}
			checkupdateParams_channels();
			updateParamsLanes_toOutside();
			updateParamsObservationTiming_toOutside();*/
		}

		// Pass it on to submodule
		res = myWaveGenerators->set_property_local(rawPtr,
			propId, category,
			trans.offsetStart,
			trans.contentOnly, callGate.call_purpose,
			&updateUi2);

		if(
			(propId == genMeasure_node::properties_common.specifyCommand.globalIdx) &&
			(category == genMeasure_node::properties_common.specifyCommand.category))
		{
			// Interpret command!
			command = genMeasure_node::properties_common.specifyCommand.value;

			res = jvx_parseCommandIntoToken(command, f_expr, args);
			if(res == JVX_NO_ERROR)
			{
				if(f_expr == "clearAllMeasurements")
				{
					genMeasure_node::deassociate__measureir__selected_Measurement(static_cast<CjvxProperties*>(this));
					for(i = 0; i < myMeasurements.theMeasurements.size();i++)
					{
						JVX_SAFE_DELETE_FLD(myMeasurements.theMeasurements[i].theIr, jvxData);
						myMeasurements.theMeasurements[i].theIr = NULL;
					}
					genMeasure_node::measureir.all_Measurements.selectMeasurement.value.entries.clear();
					genMeasure_node::measureir.all_Measurements.selectMeasurement.value.selection() = 0;
					myMeasurements.selectionId = JVX_SIZE_UNSELECTED;
					myMeasurements.theMeasurements.clear();

					updateParamsMeasurements_toOutside();
				}
				else
				{
					if(f_expr == "addLane")
					{
						oneEntryLanes newlane;
						if(args.size() == 1)
						{
							descr = args[0];

						}
						if(descr.empty())
						{
							descr = "Measure Lane#" + jvx_size2String(uniqueId);
						}
						uniqueId ++;
						newlane.typeChannelTest = JVX_AUMEASURE_NONE;
						newlane.idChannelTest = -1;
						newlane.typeChannelMeasure = JVX_AUMEASURE_NONE;
						newlane.idChannelMeasure = -1;
						newlane.description = descr;
						myLanes.theLanes.push_back(newlane);
						myLanes.selectionId = myLanes.theLanes.size()-1;
					}
					if(f_expr == "removeLane")
					{
						if(args.size() == 1)
						{
							descr = args[0];
						}
						else
						{
							if(JVX_CHECK_SIZE_SELECTED(myLanes.selectionId))
							{
								descr = myLanes.theLanes[myLanes.selectionId].description;
							}
						}
						std::vector<oneEntryLanes>::iterator elm = myLanes.theLanes.begin();
						for(; elm != myLanes.theLanes.end(); elm++)
						{
							if(descr == elm->description)
							{
								break;
							}
						}

						if(elm != myLanes.theLanes.end())
						{
							//jvx_generatorwave_deactivate(&elm->theGenerator);
							myLanes.theLanes.erase(elm);
						}
						if(myLanes.selectionId >= myLanes.theLanes.size())
						{
							myLanes.selectionId = myLanes.theLanes.size()-1;
						}
					}
					if(f_expr == "removeAll")
					{
						std::vector<oneEntryLanes>::iterator elm = myLanes.theLanes.begin();
						for(; elm != myLanes.theLanes.end(); elm++)
						{
							//jvx_generatorwave_deactivate(&elm->theGenerator);

						}
						myLanes.theLanes.clear();
						if(myLanes.selectionId >= myLanes.theLanes.size())
						{
							myLanes.selectionId = myLanes.theLanes.size()-1;
						}
					}
					genMeasure_node::properties_common.specifyCommand.value = "";
					checkupdateParams_lanes();
					updateParamsLanes_toOutside();
				}
			}
		}

		if((propId == genMeasure_node::measureir.properties_active_higher_all_Lanes.selectLane.globalIdx ) && (category == genMeasure_node::measureir.properties_active_higher_all_Lanes.selectLane.category ))
		{
			// Select lane
			myLanes.selectionId = jvx_bitfieldSelection2Id(genMeasure_node::measureir.properties_active_higher_all_Lanes.selectLane.value.selection(),
			genMeasure_node::measureir.properties_active_higher_all_Lanes.selectLane.value.entries.size());
			updateParamsLanes_toOutside();
		}

		if(
			((propId == genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.globalIdx ) && (category == genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.category )) ||
			((propId == genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.globalIdx ) && (category == genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.category ))
			)
		{
			updateParamsLanes_toInside();
			updateParamsLanes_toOutside();
		}

		if(
			(propId == genMeasure_node::measureir.all_Measurements.selectMeasurement.globalIdx ) && (category == genMeasure_node::measureir.all_Measurements.selectMeasurement.category )
			)
		{
			myMeasurements.selectionId = jvx_bitfieldSelection2Id(
				genMeasure_node::measureir.all_Measurements.selectMeasurement.value.selection(),
				genMeasure_node::measureir.all_Measurements.selectMeasurement.value.entries.size());
			updateParamsMeasurements_toOutside();
		}

		if((propId == genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.globalIdx ) && (category == genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.category ))
		{
			// Select lane
			jvxSize sel = jvx_bitfieldSelection2Id(genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.selection(),
				genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.entries.size());
			if(JVX_CHECK_SIZE_UNSELECTED(sel))
			{
				observationChannel.typeChannel = JVX_AUMEASURE_NONE;
				observationChannel.idChannel = JVX_SIZE_UNSELECTED;
			}
			else if(sel < CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value)
			{
				observationChannel.typeChannel = JVX_AUMEASURE_INPUT;
				observationChannel.idChannel = sel;
			}
			else if(sel < (CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value + CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value))
			{
				observationChannel.typeChannel = JVX_AUMEASURE_OUTPUT;
				observationChannel.idChannel = sel - CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value;
			}
			else
			{
				assert(0);
			}
			updateParamsObservationTiming_toOutside();
		}

		if(
			(propId == genMeasure_node::properties_mode.currentMode.globalIdx) &&
			(category == genMeasure_node::properties_mode.currentMode.category))
		{
			if(JVX_EVALUATE_BITFIELD(genMeasure_node::properties_mode.currentMode.value.selection() & 0x2))
			{
				if(currentMode == JVX_AUNMEASURE_OPERATION_MODE_MEASURE_TIMING)
				{
					// Reset all generators
					command = "removeAll()";
					fldStr.assign_const(command.c_str(), command.size());
					jvx_set_property(static_cast<IjvxProperties*>(this), (jvxHandle*)(&fldStr), 0, 1, JVX_DATAFORMAT_STRING, true, 
						"/WaveGenerators/specifyCommand", callGateLoc);
					currentMode = JVX_AUNMEASURE_OPERATION_MODE_NONE;
				}

				if(currentMode == JVX_AUNMEASURE_OPERATION_MODE_NONE)
				{
					command = "addGenerator(TestSignal)";
					fldStr.assign_const(command.c_str(), command.size());
					jvx_set_property(static_cast<IjvxProperties*>(this), (jvxHandle*)(&fldStr), 0, 1, JVX_DATAFORMAT_STRING, true, 
						"/WaveGenerators/specifyCommand", callGateLoc);
					currentMode = JVX_AUNMEASURE_OPERATION_MODE_MEASURE_IR;
				}
			}
			if(JVX_EVALUATE_BITFIELD(genMeasure_node::properties_mode.currentMode.value.selection() & 0x4))
			{
				if(currentMode == JVX_AUNMEASURE_OPERATION_MODE_MEASURE_IR)
				{
					// Reset all generators
					command = "removeAll()";
					fldStr.assign_const(command.c_str(), command.size());
					jvx_set_property(static_cast<IjvxProperties*>(this), (jvxHandle*)(&fldStr), 0, 1, JVX_DATAFORMAT_STRING, true, 
						"/WaveGenerators/specifyCommand", callGateLoc);
					currentMode = JVX_AUNMEASURE_OPERATION_MODE_NONE;
				}

				if(currentMode == JVX_AUNMEASURE_OPERATION_MODE_NONE)
				{
					command = "addGenerator(OutputSignal)";
					fldStr.assign_const(command.c_str(), command.size());
					jvx_set_property(static_cast<IjvxProperties*>(this), (jvxHandle*)(&fldStr), 0, 1, JVX_DATAFORMAT_STRING, true, 
						"/WaveGenerators/specifyCommand", callGateLoc);
					currentMode = JVX_AUNMEASURE_OPERATION_MODE_MEASURE_TIMING;
				}
			}

			switch(currentMode)
			{
			case JVX_AUNMEASURE_OPERATION_MODE_NONE:
				genMeasure_node::properties_mode.currentMode.value.selection() = 0x1;
				break;
			case JVX_AUNMEASURE_OPERATION_MODE_MEASURE_IR:
				genMeasure_node::properties_mode.currentMode.value.selection() = 0x2;
				break;
			case JVX_AUNMEASURE_OPERATION_MODE_MEASURE_TIMING:
				genMeasure_node::properties_mode.currentMode.value.selection() = 0x4;
				break;
			}
		}

		if(updateUi1 || updateUi2 || updateUi3 || modifiedLane)
		{
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		}
	}
	return(res);
}

jvxErrorType
CjvxAuNMeasure::get_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	return(CjvxAudioNode_logfile::get_property(callGate,
		rawPtr, ident, trans));
}

static jvxUInt64 diffMax = 0;

jvxErrorType
CjvxAuNMeasure::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numFilled = 0;
	jvxBool allStopped = false;
	jvxSize i;
	jvxData deltaT = 0;
	jvxData* ptr = & deltaT;
	jvxTick tstart = JVX_GET_TICKCOUNT_US_GET_CURRENT(&statAnalysis.tStamp);
	jvxInt64 tstop = 0;
	std::string err;
	jvxErrorType resL = JVX_NO_ERROR;

	// This default function does not tolerate a lot of unexpected settings
	assert(theData->con_params.format == theData->con_compat.format);
	assert(theData->con_params.buffersize == theData->con_compat.buffersize);
	assert(theData->con_compat.format == JVX_DATAFORMAT_DATA);

	for(i = 0; i < theData->con_compat.number_channels; i++)
	{
		memset(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender][i],
			0, jvxDataFormat_size[theData->con_compat.format]);
	}

	switch(currentMode)
	{
	case JVX_AUNMEASURE_OPERATION_MODE_MEASURE_IR:
		if(jvx_bitTest(genMeasure_node::measureir.properties_inproc.currentStatus.value.selection(), 2))
		{
			res = myWaveGenerators->process(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender], numFilled, allStopped);

			for(i = 0; i < myLanes.theLanes.size(); i++)
			{
				jvxMeasureIr* thePointer = &myLanes.theLanes[i].runtime.coreMeasure;

				jvxSize idx_start = myLanes.theLanes[i].runtime.cntSamples;
				jvxSize idx_stop = thePointer->prm_sync.lRecording;

				jvxData* inTest = thePointer->prm_sync.fldBufferTestSignal;
				jvxData* inMeasure = thePointer->prm_sync.fldBufferMeasureSignal;

				jvxSize nmCopy = idx_stop - idx_start;
				nmCopy = JVX_MIN(nmCopy, _common_set_node_params_1io.processing.buffersize);

				jvxData* copyFrom = NULL;
				if(myLanes.theLanes[i].typeChannelTest == JVX_AUMEASURE_INPUT)
				{
					copyFrom = (jvxData*)theData->con_data.buffers[idx_sender_to_receiver][myLanes.theLanes[i].idChannelTest];
				}
				if(myLanes.theLanes[i].typeChannelTest == JVX_AUMEASURE_OUTPUT)
				{
					copyFrom = (jvxData*)theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender][myLanes.theLanes[i].idChannelTest];
				}
				if(copyFrom)
				{
					memcpy(inTest + idx_start, copyFrom, nmCopy * sizeof(jvxData));
				}

				copyFrom = NULL;
				if(myLanes.theLanes[i].typeChannelMeasure == JVX_AUMEASURE_INPUT)
				{
					copyFrom = (jvxData*)theData->con_data.buffers[idx_sender_to_receiver][myLanes.theLanes[i].idChannelMeasure];
				}
				if(myLanes.theLanes[i].typeChannelMeasure == JVX_AUMEASURE_OUTPUT)
				{
					copyFrom = (jvxData*)theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender][myLanes.theLanes[i].idChannelMeasure];
				}
				if(copyFrom)
				{
					memcpy(inMeasure + idx_start, copyFrom, nmCopy * sizeof(jvxData));
				}
				myLanes.theLanes[i].runtime.cntSamples += nmCopy;
			}
			if(allStopped)
			{
				if(!runtime.endProcessingTriggered)
				{
					for(i = 0; i < myLanes.theLanes.size(); i++)
					{
						jvxMeasureIr* thePointer = &myLanes.theLanes[i].runtime.coreMeasure;

						jvxSize idx_start = myLanes.theLanes[i].runtime.cntSamples;
						jvxSize idx_stop = thePointer->prm_sync.lRecording;
						assert(idx_start == idx_stop);
					}

					_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT);
					runtime.endProcessingTriggered = true;
					jvx_bitZSet(genMeasure_node::measureir.properties_inproc.currentStatus.value.selection(), 3);
				}
			}
		}
		else
		{
			runtime.sampleCntCountdown = (runtime.sampleCntCountdown + theData->con_compat.buffersize);
			genMeasure_node::measureir.properties_inproc.stillWaitCountdown.value = (jvxData)(runtime.sampleCntCountdownMax - runtime.sampleCntCountdown) / (jvxData)_common_set_node_params_1io.processing.samplerate;
			if(runtime.sampleCntCountdown >= runtime.sampleCntCountdownMax)
			{
				jvx_bitZSet(genMeasure_node::measureir.properties_inproc.currentStatus.value.selection(), 2);
			}
		}
		break;
	case JVX_AUNMEASURE_OPERATION_MODE_MEASURE_TIMING:

		if(statAnalysis.previousTstamp_us > 0)
		{
			deltaT = (jvxData) tstart - statAnalysis.previousTstamp_us;
			deltaT = deltaT * 0.001;
			statAnalysis.theObj.addEvent(deltaT, statAnalysis.statAna_lineidx);
		}
		statAnalysis.previousTstamp_us = tstart;
		res = myWaveGenerators->process(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender], numFilled, allStopped);

		// Provide data for output
		switch(observationChannel.typeChannel)
		{
		case JVX_AUMEASURE_INPUT:
			viewStruct_channel.plotter.process_circbuffer((jvxData**)theData->con_data.buffers[idx_sender_to_receiver] + observationChannel.idChannel);
			break;
		case JVX_AUMEASURE_OUTPUT:
			viewStruct_channel.plotter.process_circbuffer((jvxData**)theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender] + observationChannel.idChannel);
			break;
		}

		viewStruct_timing.plotter.process_circbuffer((jvxData**)&ptr);
		break;
	default:
		break;
	}

	// Write all channels
	CjvxAudioNode_logfile::log_write_all_channels(theData, idx_sender_to_receiver, idx_receiver_to_sender);
	/*
	JVX_AUDIONODE_CLASSNAME::theLogger.process_logging_try(uniqueMyId, 
		thePointer, 
		theNumberChannels, 
		theFormat, 
		theBuffersize, 
		NULL);
	*/
	/*
	tstop = JVX_GET_TICKCOUNT_US_GET(statAnalysis.tStamp);
	tstop = tstop - tstart;
	if(tstop > 1500)
	{
		std::cout << tstop << std::endl;
	}*/
	return(res);
};

jvxErrorType
CjvxAuNMeasure::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
	jvxInt32 valI32;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	res = CjvxAudioNode_logfile::prepare_sender_to_receiver(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if(res == JVX_NO_ERROR)
	{
		res = myWaveGenerators->startAll();
		runtime.endProcessingTriggered = false;
		if(currentMode == JVX_AUNMEASURE_OPERATION_MODE_MEASURE_IR)
		{
			runtime.sampleCntCountdownMax = (jvxSize)floor(_common_set_node_params_1io.processing.samplerate * genMeasure_node::measureir.properties_measureIr.countdownBeforeStart.value);
			runtime.sampleCntCountdown = 0;
			runtime.iterationCount = 0;
			jvx_bitZSet(genMeasure_node::measureir.properties_inproc.currentStatus.value.selection(), 1); 
			genMeasure_node::measureir.properties_inproc.stillWaitCountdown.value = (jvxData)(runtime.sampleCntCountdownMax - runtime.sampleCntCountdown) / (jvxData)_common_set_node_params_1io.processing.samplerate;

			for(i = 0; i < myLanes.theLanes.size(); i++)
			{
				jvx_measure_ir_initConfig(&myLanes.theLanes[i].runtime.coreMeasure);

				// Find out the number of iterations
				res = jvx_get_property(static_cast<IjvxProperties*>(this), &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, false, 
					"/WaveGenerators/loopCount_activeGenerator", callGate);
				JVX_LOCAL_ASSERT_PROPERTIES((res == JVX_NO_ERROR), "/WaveGenerators/loopCount_activeGenerator", res)
					myLanes.theLanes[i].runtime.coreMeasure.prm_init.numReps = valI32;

				// Find out the length of the testsignal
				res = jvx_get_property(static_cast<IjvxProperties*>(this), &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, false, 
					"/WaveGenerators/simulationLength_activeGenerator", callGate);
				JVX_LOCAL_ASSERT_PROPERTIES((res == JVX_NO_ERROR), "/WaveGenerators/simulationLength_activeGenerator", res)
					myLanes.theLanes[i].runtime.coreMeasure.prm_init.szTestsignal_onerep = valI32;

				myLanes.theLanes[i].runtime.coreMeasure.prm_init.computeIr = true;
				myLanes.theLanes[i].runtime.coreMeasure.prm_init.computeXc = false;
				myLanes.theLanes[i].runtime.coreMeasure.prm_init.skipNumber = 0;

				jvx_measure_ir_prepare(&myLanes.theLanes[i].runtime.coreMeasure);
				jvx_measure_ir_update(&myLanes.theLanes[i].runtime.coreMeasure, JVX_DSP_UPDATE_SYNC, false);

				myLanes.theLanes[i].runtime.cntSamples = 0;
			}
		}
		else
		{
			JVX_GET_TICKCOUNT_US_SETREF(&statAnalysis.tStamp);
			statAnalysis.previousTstamp_us = 0;
			statAnalysis.bsize = JVX_SIZE_INT32(_common_set_node_params_1io.processing.buffersize);
			statAnalysis.rate = _common_set_node_params_1io.processing.samplerate;
			statAnalysis.deltatoneframe = (jvxData)statAnalysis.bsize / (jvxData)statAnalysis.rate * 1000;
			statAnalysis.numberObservations = JVX_DATA2INT32(2.0 * TIMEOUT_MASTAT_MS /statAnalysis.deltatoneframe);
			statAnalysis.theObj.registerOneLine(statAnalysis.numberObservations , statAnalysis.deltatoneframe , &statAnalysis.statAna_lineidx);
			statAnalysis.theObj.start(true, TIMEOUT_MASTAT_MS, static_cast<CjvxMaStatAnalysis_report*>(this));
			statAnalysis.theObj.prepareUpdate(statAnalysis.statAna_lineidx);
			genMeasure_node::measuretiming.resultsTiming.theodeltat_Timing.value = statAnalysis.deltatoneframe;

			viewStruct_channel.circBufferAttached = false;
			if(viewStruct_channel.theCircBufferHeader)
			{
				viewStruct_channel.resamplingFactor = (jvxData) (TIMEPERIOD_SHOW_SIGNAL * _common_set_node_params_1io.processing.samplerate) / (jvxData)viewStruct_channel.theCircBufferHeader->length;
				viewStruct_channel.circBufferAttached = true;
				viewStruct_channel.plotter.init_circbuffer(1, CjvxAudioNode_genpcg::properties_parameters.buffersize.value, 3, JVX_CIRCBUFFER_VISUAL_RESAMPLER);
				viewStruct_channel.plotter.set_endpoint_circbuffer(viewStruct_channel.theCircBufferHeader);
				viewStruct_channel.plotter.set_properties_circbuffer(JVX_CIRCBUFFER_VISUAL_RESAMPLER, 0, &viewStruct_channel.resamplingFactor);
				viewStruct_channel.plotter.set_mode_circbuffer(JVX_CIRCBUFFER_VISUAL_RESAMPLER);
				viewStruct_channel.plotter.start_circbuffer(CjvxMultiChannelCircBufferPlotResampler::JVX_CIRCBUFFER_MULTITHREAD, true);
			}

			viewStruct_timing.circBufferAttached = false;
			if(viewStruct_timing.theCircBufferHeader)
			{
				viewStruct_timing.resamplingFactor = (jvxData) (TIMEPERIOD_SHOW_SIGNAL * _common_set_node_params_1io.processing.samplerate) / (jvxData)viewStruct_timing.theCircBufferHeader->length / (jvxData)_common_set_node_params_1io.processing.buffersize;
				viewStruct_timing.circBufferAttached = true;
				viewStruct_timing.plotter.init_circbuffer(1, 1, 3, JVX_CIRCBUFFER_VISUAL_RESAMPLER);
				viewStruct_timing.plotter.set_endpoint_circbuffer(viewStruct_timing.theCircBufferHeader);
				viewStruct_timing.plotter.set_properties_circbuffer(JVX_CIRCBUFFER_VISUAL_RESAMPLER, 0, &viewStruct_timing.resamplingFactor);
				viewStruct_timing.plotter.set_mode_circbuffer(JVX_CIRCBUFFER_VISUAL_RESAMPLER);
				viewStruct_timing.plotter.start_circbuffer(CjvxMultiChannelCircBufferPlotResampler::JVX_CIRCBUFFER_MULTITHREAD, true);
			}
		}

#ifdef JVX_USE_SPECTRUM_PLOT
		jvx_spectral_plot_initCfg(&theSpectrumMatHdl);
		theSpectrumMatHdl.initPrm.bufferSize = _common_set_node_params_1io.processing.buffersize;
		theSpectrumMatHdl.initPrm.channels = 1;
		theSpectrumMatHdl.initPrm.fftSize = JVX_FFT_TOOLS_FFT_SIZE_1024;
		theSpectrumMatHdl.initPrm.highdB = 0;
		theSpectrumMatHdl.initPrm.lowdB = 0;
		theSpectrumMatHdl.initPrm.hopSize = _common_set_node_params_1io.processing.buffersize;
		theSpectrumMatHdl.initPrm.lowdB = -120;
		jvx_spectral_plot_init(&theSpectrumMatHdl);

#endif
	}
	return(res);
}

jvxErrorType
CjvxAuNMeasure::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioNode_logfile::postprocess_sender_to_receiver(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if(res == JVX_NO_ERROR)
	{
		std::string description = "description";
		std::string datetime = JVX_DATE();
		datetime += "_";
		datetime += JVX_TIME();

		res = myWaveGenerators->stopAll();
		if(currentMode == JVX_AUNMEASURE_OPERATION_MODE_MEASURE_IR)
		{
			if (runtime.endProcessingTriggered)
			{
				for (i = 0; i < myLanes.theLanes.size(); i++)
				{
					jvxAuNOneMeasurement theNewMeasurement;
					theNewMeasurement.tag = genMeasure_node::measureir.all_Measurements.tagPrefix_Measurement.value + "::" + myLanes.theLanes[i].description + "<" + datetime + ">";
					// Compute impulse response and xc - if desired
					jvx_measure_ir_process(&myLanes.theLanes[i].runtime.coreMeasure);

					// Copy result..
					theNewMeasurement.lIr = myLanes.theLanes[i].runtime.coreMeasure.prm_sync.lIResponse;
					theNewMeasurement.theIr = NULL;
					JVX_SAFE_NEW_FLD(theNewMeasurement.theIr, jvxData, theNewMeasurement.lIr);
					memcpy(theNewMeasurement.theIr, myLanes.theLanes[i].runtime.coreMeasure.prm_sync.fldIResponse, sizeof(jvxData) * theNewMeasurement.lIr);

					// Postprocess
					jvx_measure_ir_postprocess(&myLanes.theLanes[i].runtime.coreMeasure);

					myMeasurements.theMeasurements.push_back(theNewMeasurement);
				}
			}
			else
			{
				_report_text_message("Measurement incomplete,  no results stored!", JVX_REPORT_PRIORITY_INFO);
			}
			this->_lock_properties_local();

			jvxSize idBefore = measureir.all_Measurements.selectMeasurement.value.entries.size();
			genMeasure_node::measureir.all_Measurements.selectMeasurement.value.entries.clear();
			for(i = 0; i < myMeasurements.theMeasurements.size(); i++)
			{
				genMeasure_node::measureir.all_Measurements.selectMeasurement.value.entries.push_back(myMeasurements.theMeasurements[i].tag);
			}
			this->myMeasurements.selectionId = idBefore;

			//
			updateParamsMeasurements_toOutside();

			jvx_bitZSet(genMeasure_node::measureir.properties_inproc.currentStatus.value.selection(), 0);
			this->_unlock_properties_local();
		}
		else
		{
			if(viewStruct_channel.circBufferAttached)
			{
				viewStruct_channel.resamplingFactor = 0;
				viewStruct_channel.circBufferAttached = false;
				viewStruct_channel.plotter.stop_circbuffer();
				viewStruct_channel.plotter.terminate_circbuffer();
			}

			if(viewStruct_timing.circBufferAttached)
			{
				viewStruct_timing.resamplingFactor = 0;
				viewStruct_timing.circBufferAttached = false;
				viewStruct_timing.plotter.stop_circbuffer();
				viewStruct_timing.plotter.terminate_circbuffer();
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxAuNMeasure::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionToContainAllSubsectionsForMe, 
								     const char* filename,
								     jvxInt32 lineno)
{
	jvxSize i,j;
	jvxConfigData* thesubsec = NULL,* newSecLane = NULL;

	std::vector<std::string> warns;
	std::vector<std::string> availableChannelsMeas;
	std::vector<std::string> availableChannelsTest;
	jvxErrorType res = JVX_NO_ERROR;

	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		res = myWaveGenerators->put_configuration(callConf, processor,
			sectionToContainAllSubsectionsForMe, filename,
			lineno);
		genMeasure_node::put_configuration__properties_mode(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
		for(i = 0; i < warns.size(); i++)
		{
			_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
		warns.clear();

		if(JVX_EVALUATE_BITFIELD(properties_mode.currentMode.value.selection() & 0x2))
		{
			currentMode = JVX_AUNMEASURE_OPERATION_MODE_MEASURE_IR;
		}
		else if(JVX_EVALUATE_BITFIELD(properties_mode.currentMode.value.selection() & 0x4))
		{
			currentMode = JVX_AUNMEASURE_OPERATION_MODE_MEASURE_TIMING;
		}
		else
		{
			currentMode = JVX_AUNMEASURE_OPERATION_MODE_NONE;
		}

		genMeasure_node::put_configuration__measureir__properties_measureIr(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
		for(i = 0; i < warns.size(); i++)
		{
			_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
		warns.clear();

		availableChannelsMeas = genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries;
		availableChannelsTest = genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries;

		processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &thesubsec, JVX_AUN_LANES_CONFIG);
		if(thesubsec)
		{
			jvxSize num = 0;
			std::string valSelMeas = "";
			std::string valSelTest = "";
			jvxSize sel = JVX_SIZE_UNSELECTED;
			processor->getNumberEntriesCurrentSection(thesubsec, &num);
			for(i = 0; i < num; i++)
			{
				newSecLane = NULL;
				processor->getReferenceEntryCurrentSection_id(thesubsec, &newSecLane, i);
				if(newSecLane)
				{
					oneEntryLanes newlane;
					uniqueId ++;
					newlane.typeChannelTest = JVX_AUMEASURE_NONE;
					newlane.idChannelTest = -1;
					newlane.typeChannelMeasure = JVX_AUMEASURE_NONE;
					newlane.idChannelMeasure = -1;
					newlane.description = "";
					myLanes.theLanes.push_back(newlane);
					myLanes.selectionId = myLanes.theLanes.size()-1;
					genMeasure_node::put_configuration__measureir__properties_active_higher_active_Lane(callConf, processor, newSecLane, &warns);
					for(j = 0; j < warns.size(); j++)
					{
						_report_text_message(warns[j].c_str(), JVX_REPORT_PRIORITY_WARNING);
					}
					warns.clear();
					myLanes.theLanes[myLanes.selectionId].description = genMeasure_node::measureir.properties_active_higher_active_Lane.description_activeLane.value;
					valSelMeas = "";
					sel = jvx_bitfieldSelection2Id( genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.selection(),
						genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries.size());
					if(JVX_CHECK_SIZE_SELECTED(sel))
					{
						valSelMeas = genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries[sel];
					}
					sel = JVX_SIZE_UNSELECTED;
					for(j =0; j < availableChannelsMeas.size(); j++)
					{
						if(valSelMeas == availableChannelsMeas[j])
						{
							sel = j;
							break;
						}
					}

					if(JVX_CHECK_SIZE_SELECTED(sel))
					{
						if(sel >= CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value)
						{
							myLanes.theLanes[myLanes.selectionId].typeChannelMeasure = JVX_AUMEASURE_OUTPUT;
							myLanes.theLanes[myLanes.selectionId].idChannelMeasure = JVX_SIZE_INT32(sel - CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value);
						}
						else
						{
							myLanes.theLanes[myLanes.selectionId].typeChannelMeasure = JVX_AUMEASURE_INPUT;
							myLanes.theLanes[myLanes.selectionId].idChannelMeasure = JVX_SIZE_INT32(sel);
						}
					}

					valSelTest = "";
					sel = jvx_bitfieldSelection2Id(genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.selection(),
						genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries.size());
					if(JVX_CHECK_SIZE_SELECTED(sel))
					{
						valSelTest = genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries[sel];
					}

					sel = JVX_SIZE_UNSELECTED;
					for(j =0; j < availableChannelsTest.size(); j++)
					{
						if(valSelTest == availableChannelsTest[j])
						{
							sel = j;
							break;
						}
					}

					if(JVX_CHECK_SIZE_SELECTED(sel))
					{
						if(sel >= CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value)
						{
							myLanes.theLanes[myLanes.selectionId].typeChannelTest = JVX_AUMEASURE_OUTPUT;
							myLanes.theLanes[myLanes.selectionId].idChannelTest = JVX_SIZE_INT32(sel - CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value);
						}
						else
						{
							myLanes.theLanes[myLanes.selectionId].typeChannelTest = JVX_AUMEASURE_INPUT;
							myLanes.theLanes[myLanes.selectionId].idChannelTest = JVX_SIZE_INT32(sel);
						}
					}
				}
			}
		}

		genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries = availableChannelsMeas;
		genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries = availableChannelsTest;

		genMeasure_node::put_configuration__measureir__properties_active_higher_all_Lanes(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
		for(i = 0; i < warns.size(); i++)
		{
			_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
		warns.clear();
		updateParamsLanes_toOutside();

	}
	return(res);}

jvxErrorType
CjvxAuNMeasure::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxConfigData* newSec = NULL,* newLocSec = NULL;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		res = myWaveGenerators->get_configuration(callConf, processor,
			sectionWhereToAddAllSubsections);
		genMeasure_node::get_configuration__properties_mode(callConf, processor, sectionWhereToAddAllSubsections);
		genMeasure_node::get_configuration__measureir__properties_measureIr(callConf, processor, sectionWhereToAddAllSubsections);

		processor->createEmptySection(&newSec, JVX_AUN_LANES_CONFIG);

		genMeasure_node::get_configuration__measureir__properties_active_higher_all_Lanes(callConf, processor, sectionWhereToAddAllSubsections);
		jvxSize oldSel = myLanes.selectionId;
		for(i = 0; i < myLanes.theLanes.size(); i++)
		{
			processor->createEmptySection(&newLocSec, (JVX_AUN_ACTIVE_LANE_CONFIG_PREFIX + jvx_size2String(i)).c_str());
			myLanes.selectionId = i;
			updateParamsLanes_toOutside();
			genMeasure_node::get_configuration__measureir__properties_active_higher_active_Lane(callConf, processor, newLocSec);
			processor->addSubsectionToSection(newSec, newLocSec);
		}

		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, newSec);
		myLanes.selectionId = oldSel;
		updateParamsLanes_toOutside();

	}
	return(res);
}

void
CjvxAuNMeasure::checkupdateParams_lanes()
{
	jvxSize i;
	if(genMeasure_node::measureir.properties_active_higher_all_Lanes.selectLane.value.entries.size() != myLanes.theLanes.size())
	{
		genMeasure_node::measureir.properties_active_higher_all_Lanes.selectLane.value.entries.clear();
		for(i = 0; i < myLanes.theLanes.size(); i++)
		{
			genMeasure_node::measureir.properties_active_higher_all_Lanes.selectLane.value.entries.push_back(myLanes.theLanes[i].description);
		}
		if(myLanes.selectionId >= myLanes.theLanes.size())
		{
			myLanes.selectionId = JVX_SIZE_UNSELECTED;
		}
		if(JVX_CHECK_SIZE_UNSELECTED(myLanes.selectionId))
		{
			myLanes.selectionId = myLanes.theLanes.size()-1;
		}
		if(JVX_CHECK_SIZE_SELECTED(myLanes.selectionId))
		{
			jvx_bitZSet(genMeasure_node::measureir.properties_active_higher_all_Lanes.selectLane.value.selection(), myLanes.selectionId);
		}
		else
		{
			genMeasure_node::measureir.properties_active_higher_all_Lanes.selectLane.value.selection() = 0;
		}
	}
}

void
CjvxAuNMeasure::checkupdateParams_channels()
{
	jvxSize i;
	for(i = 0; i < myLanes.theLanes.size(); i++)
	{
		if(myLanes.theLanes[i].typeChannelMeasure == JVX_AUMEASURE_INPUT)
		{
			if(myLanes.theLanes[i].idChannelMeasure > CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value)
			{
				myLanes.theLanes[i].idChannelMeasure = CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value-1;
			}
			if(JVX_CHECK_SIZE_UNSELECTED(myLanes.theLanes[i].idChannelMeasure))
			{
				myLanes.theLanes[i].typeChannelMeasure = JVX_AUMEASURE_NONE;
			}
		}
		if(myLanes.theLanes[i].typeChannelMeasure == JVX_AUMEASURE_OUTPUT)
		{
			if(myLanes.theLanes[i].idChannelMeasure > CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value)
			{
				myLanes.theLanes[i].idChannelMeasure = CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value-1;
			}
			if(JVX_CHECK_SIZE_UNSELECTED(myLanes.theLanes[i].idChannelMeasure))
			{
				myLanes.theLanes[i].typeChannelMeasure = JVX_AUMEASURE_NONE;
			}
		}
		/*
		if(myLanes.theLanes[i].typeChannelMeasure == JVX_AUMEASURE_NONE)
		{
			if(JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value > 0)
			{
				myLanes.theLanes[i].typeChannelMeasure = JVX_AUMEASURE_INPUT;
				myLanes.theLanes[i].idChannelMeasure = 0;
			}
			else if(JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value > 0)
			{
				myLanes.theLanes[i].typeChannelMeasure = JVX_AUMEASURE_OUTPUT;
				myLanes.theLanes[i].idChannelMeasure = 0;
			}
		}
		*/

		if(myLanes.theLanes[i].typeChannelTest == JVX_AUMEASURE_INPUT)
		{
			if(myLanes.theLanes[i].idChannelTest > CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value)
			{
				myLanes.theLanes[i].idChannelTest = CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value-1;
			}
			if(JVX_CHECK_SIZE_UNSELECTED(myLanes.theLanes[i].idChannelTest))
			{
				myLanes.theLanes[i].typeChannelMeasure = JVX_AUMEASURE_NONE;
			}
		}
		if(myLanes.theLanes[i].typeChannelTest == JVX_AUMEASURE_OUTPUT)
		{
			if(myLanes.theLanes[i].idChannelTest > CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value)
			{
				myLanes.theLanes[i].idChannelTest = CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value-1;
			}
			if(JVX_CHECK_SIZE_UNSELECTED(myLanes.theLanes[i].idChannelTest))
			{
				myLanes.theLanes[i].typeChannelTest = JVX_AUMEASURE_NONE;
			}
		}
		/*
		if(myLanes.theLanes[i].typeChannelTest == JVX_AUMEASURE_NONE)
		{
			if(JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value > 0)
			{
				myLanes.theLanes[i].typeChannelTest = JVX_AUMEASURE_OUTPUT;
				myLanes.theLanes[i].idChannelTest = 0;
			}
			else if(JVX_AUDIONODE_BASE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value > 0)
			{
				myLanes.theLanes[i].typeChannelTest = JVX_AUMEASURE_INPUT;
				myLanes.theLanes[i].idChannelTest = 0;
			}
		}
		*/
	}

	if(observationChannel.typeChannel == JVX_AUMEASURE_INPUT)
	{
		if(observationChannel.idChannel > CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value)
		{
			observationChannel.idChannel = CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value-1;
		}
		if(JVX_CHECK_SIZE_UNSELECTED(observationChannel.idChannel))
		{
			observationChannel.idChannel = JVX_AUMEASURE_NONE;
		}
	}
	if(observationChannel.idChannel == JVX_AUMEASURE_OUTPUT)
	{
		if(observationChannel.idChannel > CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value)
		{
			observationChannel.idChannel = CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value-1;
		}
		if(JVX_CHECK_SIZE_UNSELECTED(observationChannel.idChannel))
		{
			observationChannel.idChannel = JVX_AUMEASURE_NONE;
		}
	}

}

void
CjvxAuNMeasure::updateParamsLanes_toInside()
{
	oneEntryLanes* selPtr = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idxChan = JVX_SIZE_UNSELECTED;

	if(JVX_CHECK_SIZE_SELECTED(myLanes.selectionId))
	{
		selPtr = &myLanes.theLanes[myLanes.selectionId];
	}

	if(selPtr)
	{
		jvxSize measSel = jvx_bitfieldSelection2Id(genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.selection(),
			genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries.size());
		if(JVX_CHECK_SIZE_UNSELECTED(measSel))
		{
			selPtr->typeChannelMeasure = JVX_AUMEASURE_NONE;
			selPtr->idChannelMeasure = JVX_SIZE_UNSELECTED;
		}
		else if(measSel < CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value)
		{
			selPtr->typeChannelMeasure = JVX_AUMEASURE_INPUT;
			selPtr->idChannelMeasure = measSel;
		}
		else if(measSel < (CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value + CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value))
		{
			selPtr->typeChannelMeasure = JVX_AUMEASURE_OUTPUT;
			selPtr->idChannelMeasure = measSel - CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value;
		}
		else
		{
			assert(0);
		}

		jvxSize testSel = jvx_bitfieldSelection2Id(genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.selection(),
			genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries.size());
		if(JVX_CHECK_SIZE_UNSELECTED(testSel))
		{
			selPtr->typeChannelTest = JVX_AUMEASURE_NONE;
			selPtr->idChannelTest = JVX_SIZE_UNSELECTED;
		}
		else if(testSel < CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value)
		{
			selPtr->typeChannelTest = JVX_AUMEASURE_INPUT;
			selPtr->idChannelTest = testSel;
		}
		else if(testSel < (CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value + CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value))
		{
			selPtr->typeChannelTest = JVX_AUMEASURE_OUTPUT;
			selPtr->idChannelTest = testSel - CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value;
		}
		else
		{
			assert(0);
		}
	}
}

void
CjvxAuNMeasure::updateParamsLanes_toOutside()
{
	oneEntryLanes* selPtr = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idxChan = JVX_SIZE_UNSELECTED;

	if(JVX_CHECK_SIZE_SELECTED(myLanes.selectionId))
	{
		selPtr = &myLanes.theLanes[myLanes.selectionId];
	}

	if(selPtr)
	{
		switch(selPtr->typeChannelMeasure)
		{
		case JVX_AUMEASURE_NONE:
			genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.selection() = 0;
			break;
		case JVX_AUMEASURE_INPUT:
			jvx_bitZSet(genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.selection(), selPtr->idChannelMeasure);
			break;
		case JVX_AUMEASURE_OUTPUT:
			jvx_bitZSet(genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.selection(), 
				(selPtr->idChannelMeasure + CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value));
			break;
		}
		switch(selPtr->typeChannelTest)
		{
		case JVX_AUMEASURE_NONE:
			genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.selection() = 0;
			break;
		case JVX_AUMEASURE_INPUT:
			jvx_bitZSet(genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.selection(), selPtr->idChannelTest);
			break;
		case JVX_AUMEASURE_OUTPUT:
			jvx_bitZSet(genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.selection(), 
				(selPtr->idChannelTest + CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value));
			break;
		}

		genMeasure_node::measureir.properties_active_higher_active_Lane.description_activeLane.value = selPtr->description;
	}
}

void
CjvxAuNMeasure::updateParamsObservationTiming_toOutside()
{
	switch(observationChannel.typeChannel)
	{
	case JVX_AUMEASURE_NONE:
		genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.selection() = 0;
		break;
	case JVX_AUMEASURE_INPUT:
		jvx_bitZSet(genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.selection(), observationChannel.idChannel);
		break;
	case JVX_AUMEASURE_OUTPUT:
		jvx_bitZSet(genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.selection(), (observationChannel.idChannel + CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value));
		break;
	}
}

void
CjvxAuNMeasure::updateParamsMeasurements_toOutside()
{
	if(myMeasurements.selectionId >=  myMeasurements.theMeasurements.size())
	{
		myMeasurements.selectionId = myMeasurements.theMeasurements.size()-1;
	}

	if(JVX_CHECK_SIZE_UNSELECTED(myMeasurements.selectionId))
	{
		myMeasurements.selectionId = myMeasurements.theMeasurements.size()-1;
	}

	if(JVX_CHECK_SIZE_SELECTED(myMeasurements.selectionId))
	{
		jvx_bitZSet(genMeasure_node::measureir.all_Measurements.selectMeasurement.value.selection(), myMeasurements.selectionId);
		genMeasure_node::associate__measureir__selected_Measurement(static_cast<CjvxProperties*>(this), myMeasurements.theMeasurements[myMeasurements.selectionId].theIr,
			myMeasurements.theMeasurements[myMeasurements.selectionId].lIr);
	}
}

void
CjvxAuNMeasure::reportUpdate(jvxSize line)
{
	statAnalysis.theObj.getCurrentMeasurement(
		&genMeasure_node::measuretiming.resultsTiming.avrgdeltat_Timing.value,
		&genMeasure_node::measuretiming.resultsTiming.maxdeltat_Timing.value,
		&genMeasure_node::measuretiming.resultsTiming.mindeltat_Timing.value,
		NULL,
		&genMeasure_node::measuretiming.resultsTiming.vardeltat_Timing.value,
		statAnalysis.statAna_lineidx);

	statAnalysis.theObj.prepareUpdate(statAnalysis.statAna_lineidx);
}

jvxErrorType 
CjvxAuNMeasure::prepare()
{
	jvxErrorType res = CjvxAudioNode_logfile::prepare();
	if(res == JVX_NO_ERROR)
	{
		CjvxAudioNode_logfile::log_prepare_all_channels();

		// We may put the other logfile lanes here..
		/*
		JVX_AUDIONODE_CLASSNAME::theLogger.add_one_lane(1, 
		_common_set_node_params_1io.processing.buffersize,
		_common_set_node_params_1io.processing.format,
		"Whatever", myDataDescriptor, uniqueMyId);
		*/
		CjvxAudioNode_logfile::log_start_all_channels();
	}
	return(res);
}

jvxErrorType
CjvxAuNMeasure::postprocess()
{
	jvxErrorType res = CjvxAudioNode_logfile::postprocess();
	if(res == JVX_NO_ERROR)
	{
		CjvxAudioNode_logfile::log_stop_all_channels();
		CjvxAudioNode_logfile::log_postprocess_all_channels();
	}
	return(res);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNMeasure, set_buffer_post)
{
	if (
		(ident.id == genMeasure_node::measuretiming.runtimeTiming.linearBuffer_channel.globalIdx) &&
		(ident.cat == genMeasure_node::measuretiming.runtimeTiming.linearBuffer_channel.category))
	{
		if (genMeasure_node::measuretiming.runtimeTiming.linearBuffer_channel.ptr)
		{
			viewStruct_channel.theCircBufferHeader = genMeasure_node::measuretiming.runtimeTiming.linearBuffer_channel.ptr;
		}
	}
	if (
		(ident.id == genMeasure_node::measuretiming.runtimeTiming.linearBuffer_timing.globalIdx) &&
		(ident.cat == genMeasure_node::measuretiming.runtimeTiming.linearBuffer_timing.category))
	{
		if (genMeasure_node::measuretiming.runtimeTiming.linearBuffer_timing.ptr)
		{
			viewStruct_timing.theCircBufferHeader = genMeasure_node::measuretiming.runtimeTiming.linearBuffer_timing.ptr;
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNMeasure, set_buffer_pre)
{
	if (
		(ident.id == genMeasure_node::measuretiming.runtimeTiming.linearBuffer_channel.globalIdx) &&
		(ident.cat == genMeasure_node::measuretiming.runtimeTiming.linearBuffer_channel.category))
	{
		if (genMeasure_node::measuretiming.runtimeTiming.linearBuffer_channel.ptr)
		{
			// On uninstall
			viewStruct_channel.theCircBufferHeader = nullptr;
		}
	}
	if (
		(ident.id == genMeasure_node::measuretiming.runtimeTiming.linearBuffer_timing.globalIdx) &&
		(ident.cat == genMeasure_node::measuretiming.runtimeTiming.linearBuffer_timing.category))
	{
		if (genMeasure_node::measuretiming.runtimeTiming.linearBuffer_timing.ptr)
		{
			// On uninstall
			viewStruct_timing.theCircBufferHeader = genMeasure_node::measuretiming.runtimeTiming.linearBuffer_timing.ptr;
		}
	}
	return JVX_NO_ERROR;
}

JVX_OS_REACT_INTERFACE_PARAMETERS_DEFINE(CjvxAuNMeasure, CjvxAudioNode_logfile)
{
	jvxBool updateUi1 = false;
	jvxSize i;

	genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries.clear();
	genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries.clear();
	genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.entries.clear();
	for (i = 0; i < CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value; i++)
	{
		genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries.push_back(("Input Channel #" + jvx_size2String(i)).c_str());
		genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries.push_back(("Input Channel #" + jvx_size2String(i)).c_str());
		genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.entries.push_back(("Input Channel #" + jvx_size2String(i)).c_str());

	}
	for (i = 0; i < CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value; i++)
	{
		genMeasure_node::measureir.properties_active_higher_active_Lane.measureSelection_activeLane.value.entries.push_back(("Output Channel #" + jvx_size2String(i)).c_str());
		genMeasure_node::measureir.properties_active_higher_active_Lane.testSelection_activeLane.value.entries.push_back(("Output Channel #" + jvx_size2String(i)).c_str());
		genMeasure_node::measuretiming.runtimeTiming.selection_outputSignal.value.entries.push_back(("Output Channel #" + jvx_size2String(i)).c_str());
	}
	checkupdateParams_channels();
	updateParamsLanes_toOutside();
	updateParamsObservationTiming_toOutside();

	myWaveGenerators->setParameters(
		CjvxAudioNode_genpcg::properties_parameters.samplerate.value,
		CjvxAudioNode_genpcg::properties_parameters.buffersize.value,
		CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value,
		(jvxDataFormat)CjvxAudioNode_genpcg::properties_parameters.format.value,
		&updateUi1
	);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

