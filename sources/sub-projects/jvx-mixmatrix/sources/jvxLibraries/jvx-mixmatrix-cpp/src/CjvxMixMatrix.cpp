#include "CjvxMixMatrix.h"

jvxErrorType jvx_factory_allocate_jvxMixMatrix(IjvxMixMatrix** returnedObj)
{
	CjvxMixMatrix* retObj = NULL;
	JVX_SAFE_NEW_OBJ(retObj, CjvxMixMatrix);
	*returnedObj = static_cast<IjvxMixMatrix*>(retObj);
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_factory_deallocate_jvxMixMatrix(IjvxMixMatrix* returnedObj)
{
	JVX_SAFE_DELETE_OBJ(returnedObj);
	return(JVX_NO_ERROR);
}

// ===========================================================================

CjvxMixMatrix::CjvxMixMatrix()
{
	params.bsize = 0;
	theState = JVX_STATE_INIT;
	JVX_INITIALIZE_MUTEX(safeAccessState);
	numInChannels = 0;
	numOutChannels = 0;
}

CjvxMixMatrix::~CjvxMixMatrix()
{
	JVX_TERMINATE_MUTEX(safeAccessState);
}

jvxErrorType
CjvxMixMatrix::activate(CjvxProperties* theProps, const char* prefix)
{
	this->propRef = theProps;
	this->propPrefix = prefix;

	MixMatrix::init_all();
	MixMatrix::allocate_all();
	MixMatrix::register_all(this->propRef, prefix);
	/*
	myGenerators.selectionId = JVX_SIZE_UNSELECTED;
	*/
	theState = JVX_STATE_ACTIVE;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxMixMatrix::deactivate()
{

	params.bsize = 0;
	theState = JVX_STATE_INIT;

	MixMatrix::unregister_all(this->propRef);
	MixMatrix::deallocate_all();
	/*IntegrateWaveGenerators::unregister__properties_active(propRef);
	IntegrateWaveGenerators::deallocate__properties_active();
	IntegrateWaveGenerators::unregister__properties_active_higher_one_generator(propRef);
	IntegrateWaveGenerators::deallocate__properties_active_higher_one_generator();
	IntegrateWaveGenerators::unregister__properties_active_higher_all_generators(propRef);
	IntegrateWaveGenerators::deallocate__properties_active_higher_all_generators();
	*/
	propRef = NULL;
	propPrefix = "";

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxMixMatrix::set_property_local(
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	jvxSize propId, jvxPropertyCategoryType category,
	jvxSize offsetStart, jvxBool contentOnly, jvxPropertyCallPurpose callPurpose,
	jvxBool* updateUi)
{
	std::string command;
	std::string f_expr;
	std::string descr;
	std::vector<std::string> args;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idx;

	if (
		(propId == MixMatrix::mixmatrix.properties_selected_intersect.intersect_gain.globalIdx) &&
		(category == MixMatrix::mixmatrix.properties_selected_intersect.intersect_gain.category))
	{
		if ((MixMatrix::mixmatrix.select_intersection.idx_input.value < numInChannels) && (MixMatrix::mixmatrix.select_intersection.idx_output.value < numOutChannels))
		{
			idx = MixMatrix::mixmatrix.select_intersection.idx_input.value * numOutChannels + MixMatrix::mixmatrix.select_intersection.idx_output.value;
			theInterSections[idx].factor = MixMatrix::mixmatrix.properties_selected_intersect.intersect_gain.value;
		}
		else
		{
			if (MixMatrix::mixmatrix.select_intersection.idx_input.value < numInChannels)
			{
				MixMatrix::mixmatrix.select_intersection.idx_input.value = numInChannels - 1;
			}
			if (MixMatrix::mixmatrix.select_intersection.idx_output.value < numOutChannels)
			{
				MixMatrix::mixmatrix.select_intersection.idx_output.value = numOutChannels - 1;
			}
		}
	}
	if (
		(propId == MixMatrix::mixmatrix.properties_selected_intersect.intersect_mute.globalIdx) &&
		(category == MixMatrix::mixmatrix.properties_selected_intersect.intersect_mute.category))
	{
		if ((MixMatrix::mixmatrix.select_intersection.idx_input.value < numInChannels) && (MixMatrix::mixmatrix.select_intersection.idx_output.value < numOutChannels))
		{
			idx = MixMatrix::mixmatrix.select_intersection.idx_input.value * numOutChannels + MixMatrix::mixmatrix.select_intersection.idx_output.value;
			theInterSections[idx].mute = MixMatrix::mixmatrix.properties_selected_intersect.intersect_mute.value;
		}
		else
		{
			if (MixMatrix::mixmatrix.select_intersection.idx_input.value < numInChannels)
			{
				MixMatrix::mixmatrix.select_intersection.idx_input.value = numInChannels - 1;
			}
			if (MixMatrix::mixmatrix.select_intersection.idx_output.value < numOutChannels)
			{
				MixMatrix::mixmatrix.select_intersection.idx_output.value = numOutChannels - 1;
			}
		}
	}

	if (
		(propId == MixMatrix::mixmatrix.general.main_out.globalIdx) && (category == MixMatrix::mixmatrix.general.main_out.category)
		)
	{
		JVX_LOCK_MUTEX(safeAccessState);
		if (theState == JVX_STATE_PROCESSING)
		{
			*myMixMatrixHandle.prm_sync.main_out = MixMatrix::mixmatrix.general.main_out.value;
		}
		JVX_UNLOCK_MUTEX(safeAccessState);
	}

	if (
		(propId == MixMatrix::mixmatrix.properties_selected_intersect.level_out_max.globalIdx) && (category == MixMatrix::mixmatrix.properties_selected_intersect.level_out_max.category)
		)
	{
		if ((MixMatrix::mixmatrix.select_intersection.idx_input.value < numInChannels) && (MixMatrix::mixmatrix.select_intersection.idx_output.value < numOutChannels))
		{
			idx = MixMatrix::mixmatrix.select_intersection.idx_input.value * numOutChannels + MixMatrix::mixmatrix.select_intersection.idx_output.value;
			MixMatrix::mixmatrix.properties_selected_intersect.intersect_gain.value = theInterSections[idx].factor;
			JVX_LOCK_MUTEX(safeAccessState);
			if (theState == JVX_STATE_PROCESSING)
			{
				myMixMatrixHandle.prm_sync.out_levels_max[MixMatrix::mixmatrix.select_intersection.idx_output.value] =
					MixMatrix::mixmatrix.properties_selected_intersect.level_out_max.value;
			}
			JVX_UNLOCK_MUTEX(safeAccessState);
		}
	}

	if (
		((propId == MixMatrix::mixmatrix.select_intersection.idx_input.globalIdx) &&
		(category == MixMatrix::mixmatrix.select_intersection.idx_input.category)) ||
			((propId == MixMatrix::mixmatrix.select_intersection.idx_output.globalIdx) &&
		(category == MixMatrix::mixmatrix.select_intersection.idx_output.category)))
	{
		if ((MixMatrix::mixmatrix.select_intersection.idx_input.value < numInChannels) && (MixMatrix::mixmatrix.select_intersection.idx_output.value < numOutChannels))
		{
			idx = MixMatrix::mixmatrix.select_intersection.idx_input.value * numOutChannels + MixMatrix::mixmatrix.select_intersection.idx_output.value;
			MixMatrix::mixmatrix.properties_selected_intersect.intersect_gain.value = theInterSections[idx].factor;
			JVX_LOCK_MUTEX(safeAccessState);
			if (theState == JVX_STATE_PROCESSING)
			{
				myMixMatrixHandle.prm_sync.lin_array_gain_factors[idx] = MixMatrix::mixmatrix.properties_selected_intersect.intersect_gain.value;
				myMixMatrixHandle.prm_sync.lin_array_mute[idx] = MixMatrix::mixmatrix.properties_selected_intersect.intersect_mute.value;

				MixMatrix::mixmatrix.properties_selected_intersect.level_in.value =
					myMixMatrixHandle.prm_sync.in_levels[MixMatrix::mixmatrix.select_intersection.idx_input.value];
				
				MixMatrix::mixmatrix.properties_selected_intersect.level_out.value =
					myMixMatrixHandle.prm_sync.out_levels[MixMatrix::mixmatrix.select_intersection.idx_output.value];

				MixMatrix::mixmatrix.properties_selected_intersect.level_out_max.value =
					myMixMatrixHandle.prm_sync.out_levels_max[MixMatrix::mixmatrix.select_intersection.idx_output.value];

			}
			JVX_UNLOCK_MUTEX(safeAccessState);
		}
		else
		{
			if (MixMatrix::mixmatrix.select_intersection.idx_input.value >= numInChannels)
			{
				MixMatrix::mixmatrix.select_intersection.idx_input.value = numInChannels - 1;
			}
			if (MixMatrix::mixmatrix.select_intersection.idx_output.value >= numOutChannels)
			{
				MixMatrix::mixmatrix.select_intersection.idx_output.value = numOutChannels - 1;
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxMixMatrix::setParameters(jvxSize bSize_param, jvxSize numchansin_param, jvxSize numchansout_param, jvxBool* updateUi)
{
	jvxSize i,j;

	params.bsize = bSize_param;
	if (updateUi)
	{
		*updateUi = false;
	}

	if (numchansin_param != numInChannels)
	{
		if (updateUi)
		{
			*updateUi = false;
		}
	}
	
	if (numchansout_param != numOutChannels)
	{
		if (updateUi)
		{
			*updateUi = false;
		}
	}

	std::vector<jvxOneMixIntersection> theNewInterSections;
	jvxOneMixIntersection theNewIntersect;
	for (i = 0; i < numchansin_param; i++)
	{
		for (j = 0; j < numchansout_param; j++)
		{
			if ((i < numInChannels) && (j < numOutChannels))
			{ 
				theNewIntersect = theInterSections[ i * numOutChannels + j ];
			}
			else
			{
				if (i == j)
				{
					theNewIntersect.factor = 1.0;
				}
				else
				{
					theNewIntersect.factor = 0.0;
				}
				theNewIntersect.cbHandle.privData = NULL;
				theNewIntersect.cbHandle.theCallback = NULL;
			}
			theNewInterSections.push_back(theNewIntersect);
		}
	}
	numInChannels = numchansin_param;
	numOutChannels = numchansout_param;
	theInterSections = theNewInterSections;

	if (MixMatrix::mixmatrix.select_intersection.idx_input.value >= numInChannels)
	{
		MixMatrix::mixmatrix.select_intersection.idx_input.value = numInChannels - 1;
	}
	if (MixMatrix::mixmatrix.select_intersection.idx_output.value >= numOutChannels)
	{
		MixMatrix::mixmatrix.select_intersection.idx_output.value = numOutChannels - 1;
	}

	if ((MixMatrix::mixmatrix.select_intersection.idx_input.value < numInChannels) && (MixMatrix::mixmatrix.select_intersection.idx_output.value < numOutChannels))
	{
		jvxSize idx = MixMatrix::mixmatrix.select_intersection.idx_input.value * numOutChannels + MixMatrix::mixmatrix.select_intersection.idx_output.value;
		MixMatrix::mixmatrix.properties_selected_intersect.intersect_gain.value = theInterSections[idx].factor;
	}

	// Align matrix sizes
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMixMatrix::start()
{
	jvxSize i, j, cnt;
	jvxErrorType res = JVX_ERROR_WRONG_STATE;

	JVX_LOCK_MUTEX(safeAccessState);
	if (theState == JVX_STATE_ACTIVE)
	{
		jvx_mixmatrix_initConfig(&myMixMatrixHandle);

		myMixMatrixHandle.prm_init.numInputChannels = numInChannels;
		myMixMatrixHandle.prm_init.numOutputChannels = numOutChannels;
		myMixMatrixHandle.prm_init.buffersize = params.bsize;

		jvx_mixmatrix_prepare(&myMixMatrixHandle);

		myMixMatrixHandle.prm_async.threshold_mix = MixMatrix::mixmatrix.general.zero_threshold.value;

		myMixMatrixHandle.prm_sync.ll_flds = myMixMatrixHandle.prm_init.numInputChannels * myMixMatrixHandle.prm_init.numOutputChannels;

		if (myMixMatrixHandle.prm_sync.ll_flds)
		{
			JVX_SAFE_NEW_FLD(myMixMatrixHandle.prm_sync.lin_array_gain_factors, jvxData, myMixMatrixHandle.prm_sync.ll_flds);
			memset(myMixMatrixHandle.prm_sync.lin_array_gain_factors, 0, sizeof(jvxData)* myMixMatrixHandle.prm_sync.ll_flds);

			JVX_SAFE_NEW_FLD(myMixMatrixHandle.prm_sync.lin_array_mute, jvxCBool, myMixMatrixHandle.prm_sync.ll_flds);
			for (i = 0; i < myMixMatrixHandle.prm_sync.ll_flds; i++)
			{
				myMixMatrixHandle.prm_sync.lin_array_mute[i] = false;
			}

			JVX_SAFE_NEW_FLD(myMixMatrixHandle.prm_sync.lin_array_callbacks, jvx_mix_matrix_process_samples, myMixMatrixHandle.prm_sync.ll_flds);
			memset(myMixMatrixHandle.prm_sync.lin_array_callbacks, 0, sizeof(jvx_mix_matrix_process_samples) * myMixMatrixHandle.prm_sync.ll_flds);

			JVX_SAFE_NEW_FLD(myMixMatrixHandle.prm_sync.lin_array_contexts, jvxHandle*, myMixMatrixHandle.prm_sync.ll_flds);
			memset(myMixMatrixHandle.prm_sync.lin_array_contexts, 0, sizeof(jvxHandle*) * myMixMatrixHandle.prm_sync.ll_flds);

			JVX_SAFE_NEW_FLD(myMixMatrixHandle.prm_sync.main_out, jvxData, 1);
			*myMixMatrixHandle.prm_sync.main_out = 1.0;

		}

		JVX_SAFE_NEW_FLD(myMixMatrixHandle.prm_sync.out_levels, jvxData, myMixMatrixHandle.prm_init.numOutputChannels);
		memset(myMixMatrixHandle.prm_sync.out_levels, 0, sizeof(jvxData) * myMixMatrixHandle.prm_init.numOutputChannels);

		JVX_SAFE_NEW_FLD(myMixMatrixHandle.prm_sync.in_levels, jvxData, myMixMatrixHandle.prm_init.numInputChannels);
		memset(myMixMatrixHandle.prm_sync.in_levels, 0, sizeof(jvxData) * myMixMatrixHandle.prm_init.numInputChannels);

		JVX_SAFE_NEW_FLD(myMixMatrixHandle.prm_sync.out_levels_max, jvxData, myMixMatrixHandle.prm_init.numOutputChannels);
		memset(myMixMatrixHandle.prm_sync.out_levels_max, 0, sizeof(jvxData) * myMixMatrixHandle.prm_init.numOutputChannels);

		cnt = 0;
		for (i = 0; i < myMixMatrixHandle.prm_init.numInputChannels; i++)
		{
			for (j = 0; j < myMixMatrixHandle.prm_init.numOutputChannels; j++)
			{
				myMixMatrixHandle.prm_sync.lin_array_gain_factors[cnt] = theInterSections[cnt].factor;
				myMixMatrixHandle.prm_sync.lin_array_callbacks[cnt] = theInterSections[cnt].cbHandle.theCallback;
				myMixMatrixHandle.prm_sync.lin_array_contexts[cnt] = theInterSections[cnt].cbHandle.privData;
				cnt++;
			}
			// myMixMatrixHandle.prm_sync.lin_array_contexts[cnt] = theInterSections[cnt].cbHandle.privData;
		}

		jvx_mixmatrix_update(&myMixMatrixHandle, JVX_DSP_UPDATE_SYNC, true);
		theState = JVX_STATE_PROCESSING;
		res = JVX_NO_ERROR;
	}
	JVX_UNLOCK_MUTEX(safeAccessState);
	return res;
}

jvxErrorType
CjvxMixMatrix::process(jvxHandle** inputs, jvxHandle** outputs)
{
	//jvxSize i, j, k;

	jvx_mixmatrix_process(&myMixMatrixHandle, inputs, outputs);

	MixMatrix::mixmatrix.properties_selected_intersect.level_in.value =
		myMixMatrixHandle.prm_sync.in_levels[MixMatrix::mixmatrix.select_intersection.idx_input.value];

	MixMatrix::mixmatrix.properties_selected_intersect.level_out.value =
		myMixMatrixHandle.prm_sync.out_levels[MixMatrix::mixmatrix.select_intersection.idx_output.value];

	return JVX_NO_ERROR;
	
}

jvxErrorType
CjvxMixMatrix::stop()
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	JVX_LOCK_MUTEX(safeAccessState);
	if (theState == JVX_STATE_PROCESSING)
	{
		if (myMixMatrixHandle.prm_sync.ll_flds)
		{
			JVX_SAFE_DELETE_FLD(myMixMatrixHandle.prm_sync.lin_array_gain_factors, jvxData);
			JVX_SAFE_DELETE_FLD(myMixMatrixHandle.prm_sync.lin_array_mute, jvxCBool);
			JVX_SAFE_DELETE_FLD(myMixMatrixHandle.prm_sync.lin_array_callbacks, jvx_mix_matrix_process_samples);
			JVX_SAFE_DELETE_FLD(myMixMatrixHandle.prm_sync.lin_array_contexts, jvxHandle*);
			JVX_SAFE_DELETE_FLD(myMixMatrixHandle.prm_sync.main_out, jvxData);
		}

		myMixMatrixHandle.prm_sync.ll_flds = 0;

		JVX_SAFE_DELETE_FLD(myMixMatrixHandle.prm_sync.in_levels, jvxData);
		JVX_SAFE_DELETE_FLD(myMixMatrixHandle.prm_sync.out_levels, jvxData);

		JVX_SAFE_DELETE_FLD(myMixMatrixHandle.prm_sync.out_levels_max, jvxData);

		jvx_mixmatrix_postprocess(&myMixMatrixHandle);
		theState = JVX_STATE_ACTIVE;
		res = JVX_NO_ERROR;
	}
	JVX_UNLOCK_MUTEX(safeAccessState);
	return res;
}
// ==============================================================

jvxErrorType
CjvxMixMatrix::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* datf = NULL;
	jvxConfigData* datm = NULL;

	res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datf, "JVX_MIXMATRIX_INTERSECTION_GAINS");
	res = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datm, "JVX_MIXMATRIX_INTERSECTION_MUTE");

	if (datf || datm)
	{
		jvxSize num = 0;
		if(datf)
		{
			processor->getNumberValueLists(datf, &num);
			if (num == 1)
			{
				jvxApiValueList lst;
				processor->getValueList_id(datf, &lst, 0);
				for (i = 0; i < theInterSections.size(); i++)
				{
					if (i < lst.ll())
					{
						lst.elm_at(i).toContent(&theInterSections[i].factor);
					}
				}
				MixMatrix::mixmatrix.general.config_data_available.value = c_true;
			}
		}
		num = 0;
		if (datm)
		{
			processor->getNumberValueLists(datf, &num);
			if (num == 1)
			{
				jvxApiValueList lst;
				processor->getValueList_id(datf, &lst, 0);
					for (i = 0; i < theInterSections.size(); i++)
					{
						if (i < lst.ll())
						{
							 lst.elm_at(i).toContent(&theInterSections[i].mute);
						}
					}
					MixMatrix::mixmatrix.general.config_data_available.value = c_true;
			}
		}

	}

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxMixMatrix::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* dat = NULL;

	res = processor->createAssignmentValueList(&dat, "JVX_MIXMATRIX_INTERSECTION_GAINS");
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < theInterSections.size(); i++)
		{
			processor->addAssignmentValueToList(dat, 0, theInterSections[i].factor);
		}
	}
	processor->addSubsectionToSection(sectionWhereToAddAllSubsections, dat);

	res = processor->createAssignmentValueList(&dat, "JVX_MIXMATRIX_INTERSECTION_MUTE");
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < theInterSections.size(); i++)
		{
			if (theInterSections[i].mute)
			{
				processor->addAssignmentValueToList(dat, 0, c_true);
			}
			else
			{
				processor->addAssignmentValueToList(dat, 0, c_false);
			}
		}
	}
	processor->addSubsectionToSection(sectionWhereToAddAllSubsections, dat);

	return(JVX_NO_ERROR);
}
