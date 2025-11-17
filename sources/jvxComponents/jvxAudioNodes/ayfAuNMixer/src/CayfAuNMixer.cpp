#include "jvx.h"
#include "CayfAuNMixer.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif
	
CayfAuNMixer::CayfAuNMixer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	AYF_AUDIO_NODE_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	

	memset(&processing_lib, 0, sizeof(processing_lib));
}

CayfAuNMixer::~CayfAuNMixer()
{
}

//! ================================ STEP II ============================================

jvxErrorType 
CayfAuNMixer::select(IjvxObject* owner)
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genMixer_node::init__mixer_config();
		genMixer_node::allocate__mixer_config();
		genMixer_node::register__mixer_config(static_cast<CjvxProperties*>(this));
		genMixer_node::register_callbacks(static_cast<CjvxProperties*>(this), cb_async_set, set_mixing, this);
	}
	return res;
}

jvxErrorType 
CayfAuNMixer::unselect() 
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::unselect();
	if (res == JVX_NO_ERROR)
	{
		genMixer_node::unregister_callbacks(static_cast<CjvxProperties*>(this));
		genMixer_node::unregister__mixer_config(static_cast<CjvxProperties*>(this));
		genMixer_node::deallocate__mixer_config();
	}

	return res;
}

//! ================================ STEP II ============================================
jvxErrorType 
CayfAuNMixer::activate()
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::activate();
	if (res == JVX_NO_ERROR)
	{
		ayf_mixer_init(&processing_lib, 128);

		// Added for step III
		genMixer_node::init__properties();
		genMixer_node::allocate__properties();
		genMixer_node::register__properties(static_cast<CjvxProperties*>(this));

		genMixer_node::associate__properties(static_cast<CjvxProperties*>(this),
			&processing_lib.prmAsync.volume, 1

#ifdef USE_ORC
			,&processing_lib.prmAsync.runorc, 1
#endif
		);

#ifdef JVX_EXTERNAL_CALL_ENABLED
		genMixer_node::associate__develop__config(static_cast<CjvxProperties*>(this), 
			&config.skip_involve_ccode, 1,
			&config.matlab_profiling_enabled, 1,
			&processing_lib.prmInit.bsize, 1);
#endif

	}
	return res;
}

jvxErrorType 
CayfAuNMixer::deactivate()
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
#ifdef JVX_EXTERNAL_CALL_ENABLED
		genMixer_node::deassociate__develop__config(static_cast<CjvxProperties*>(this));
#endif
		// Added for step III
		genMixer_node::deassociate__properties(static_cast<CjvxProperties*>(this));
		genMixer_node::unregister__properties(static_cast<CjvxProperties*>(this));
		genMixer_node::deallocate__properties();
			
		res = AYF_AUDIO_NODE_BASE_CLASS::deactivate();
	}
	return res;
}

/** 
 * Prepare processing, that is, allocate all buffers and required structures. We will use the allocated memory
 * during processing until postprocessing is run.
 */
jvxErrorType
CayfAuNMixer::local_prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// jvxErrorType res = AYF_AUDIO_NODE_NODE_CLASS::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	// if (res != JVX_NO_ERROR) return res; 
	// <- no longer in use as this call is in template class already
	processing_lib.prmInit.bsize = _common_set_icon.theData_in->con_params.buffersize;

#ifdef USE_ORC
	processing_lib.prmAsync.orcTokenBackend_ip = genMixer_node::properties.orcBackendIp.value.c_str();
	processing_lib.prmAsync.orcTokenBackend_op = genMixer_node::properties.orcBackendOp.value.c_str();
	processing_lib.prmAsync.orcTokenDebug = genMixer_node::properties.orcDebugLevel.value.c_str();
#endif

	return ayf_mixer_prepare(&processing_lib);
}

/**
 * Process frame-by-frame the inout signal into an output signal. We copy the input data to the output buffer.
 * Once, local processing is done, we forward processing towards the next component. Note that processing may be 
 * in-place - in that case we need to make sure that we do not destroy the data since input may be overriden while
 * producing the output data.
 */
jvxErrorType
CayfAuNMixer::local_process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxData** buffers_in = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData** buffers_out = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);

	// Involve the c-library for processing
	/*
	return ayf_mixer_process(&processing_lib, buffers_in, buffers_out,
		_common_set_icon.theData_in->con_params.number_channels, 
		_common_set_ocon.theData_out.con_params.number_channels,
		_common_set_icon.theData_in->con_params.buffersize);
	*/
	safeMixer.lock();	
	for (i = 0; i < _common_set_ocon.theData_out.con_params.number_channels; i++)
	{
		jvxData* sigTarget = buffers_out[i];
		memset(sigTarget, 0, jvxDataFormat_getsize(_common_set_ocon.theData_out.con_params.format) * _common_set_ocon.theData_out.con_params.buffersize);

		auto elm = safeMixer.v.find(i);
		if (elm != safeMixer.v.end())
		{
			auto lst = elm->second;
			for (auto elmLL : lst)
			{
				jvxData gain = elmLL.level;
				jvxSize idx = elmLL.chanNr;
				if((gain > 0) && (idx < _common_set_icon.theData_in->con_params.number_channels))
				{
					jvxData* srcPtr = buffers_in[idx];
					jvx_mixSamples_flp_gain(srcPtr, sigTarget, _common_set_ocon.theData_out.con_params.buffersize, gain);
				}
			}
		}
	}
	safeMixer.unlock();

	return JVX_NO_ERROR;
}

/**
 * Run the post processing, that is, deallocate all buffers and required structures which we allocated on prepare.
 */
jvxErrorType
CayfAuNMixer::local_postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// jvxErrorType res = AYF_AUDIO_NODE_NODE_CLASS::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	// if (res != JVX_NO_ERROR) return res; 
	// <-no longer in use as this call is in template class already
	return ayf_mixer_postprocess(&processing_lib);
}

// JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES
jvxErrorType 
CayfAuNMixer::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Try to find the names of the channels from the previous element
		jvxSelectionList selLst;

		// This is the definition
		jvx::propertyCallCompactElement<jPROSL, jPAD, jPD> allProps(jPROSL(selLst), jPAD("/system/sel_output_channels"), jPD());

		// Create the references
		// jvx::propertyCallCompactElementRef oneElm(callMan, all.rawPtrt1, all.identt1, all.detailt1);
		jvx::propertyCallCompactRefList cmpctProps;

		cmpctProps.propReqs.push_back(allProps.toRefElement());
		jvxErrorType resL = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES, &cmpctProps JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		safeMixer.lock();
		safeMixer.v = this->updateMixMatrix_inLock(genMixer_node::mixer_config.user_input.value);
		safeMixer.unlock();

		// We run this function from outside the lock also
		updateMixMatrixProperty();
	}
	return res;
}

void
CayfAuNMixer::updateMixMatrixProperty()
{
	jvxSize i, j;
	std::vector<std::vector<jvxData> > lstOut;
	if (
		JVX_CHECK_SIZE_SELECTED(node_output._common_set_node_params_a_1io.number_channels) &&
		JVX_CHECK_SIZE_SELECTED(node_inout._common_set_node_params_a_1io.number_channels))
	{
		for (i = 0; i < node_output._common_set_node_params_a_1io.number_channels; i++)
		{
			std::vector<jvxData> oneRow;
			for (j = 0; j < node_inout._common_set_node_params_a_1io.number_channels; j++)
			{
				oneRow.push_back(0);
			}
			auto elm = safeMixer.v.find(i);
			if (elm != safeMixer.v.end())
			{
				auto lstElm = elm->second;
				for (auto elmLL : lstElm)
				{
					if (elmLL.chanNr < oneRow.size())
					{
						oneRow[elmLL.chanNr] = elmLL.level;
					}
				}
			}
			lstOut.push_back(oneRow);
		}
	}
	genMixer_node::mixer_config.derived_output.value = jvx_data2NumericExpressionString(lstOut, 4);
}

std::map<int, std::list<CayfAuNMixer::oneEntryChannel> >
CayfAuNMixer::updateMixMatrix_inLock(const std::string& token)
{
	std::map<int, std::list<oneEntryChannel> > newConfigMixer = safeMixer.v;
	std::vector<std::vector< jvxData > > lstParsed;
	jvxBool isErr = false;
	jvx_parseNumericExpression(token, isErr, lstParsed);
	if (!isErr)
	{
		if (
			JVX_CHECK_SIZE_SELECTED(node_output._common_set_node_params_a_1io.number_channels) &&
			JVX_CHECK_SIZE_SELECTED(node_inout._common_set_node_params_a_1io.number_channels))
		{
			// Here we can be sure that this will produce a new matrix
			newConfigMixer.clear();
			for (jvxSize i = 0; i < node_output._common_set_node_params_a_1io.number_channels; i++)
			{
				if (i < lstParsed.size())
				{
					std::vector<jvxData>& entryChannel = lstParsed[i];
					std::list<oneEntryChannel> oneChanMix;
					jvxSize cnt = 0;
					for (auto& elm : entryChannel)
					{
						if (cnt < node_inout._common_set_node_params_a_1io.number_channels)
						{
							if (elm > 0)
							{
								oneEntryChannel newElm;
								newElm.chanNr = cnt;
								newElm.level = elm;
								oneChanMix.push_back(newElm);
							}
						}
						cnt++;
					}
					newConfigMixer[i] = oneChanMix;
				}
				else
				{
					if (genMixer_node::mixer_config.fillup_mixing.value)
					{
						std::list<oneEntryChannel> oneChanMix;
						oneEntryChannel newElm;
						newElm.chanNr = i;
						newElm.level = 1.0;
						oneChanMix.push_back(newElm);
						newConfigMixer[i] = oneChanMix;
					}
				}
			}
		}
	}

	return newConfigMixer;
}

// ===============================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CayfAuNMixer, cb_async_set)
{
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMixer_node::properties.volume))
	{
		ayf_mixer_update(&processing_lib, JVX_DSP_UPDATE_ASYNC, c_true);
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CayfAuNMixer, set_mixing)
{
	// This is called in the property lock
	safeMixer.lock();
	safeMixer.v = updateMixMatrix_inLock(genMixer_node::mixer_config.user_input.value);
	safeMixer.unlock();
	updateMixMatrixProperty();	
	return JVX_NO_ERROR;
}


#ifdef JVX_PROJECT_NAMESPACE
}
#endif

