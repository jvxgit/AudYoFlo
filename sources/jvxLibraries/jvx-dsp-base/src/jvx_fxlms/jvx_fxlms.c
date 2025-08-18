#include "jvx_fxlms/jvx_fxlms.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
#include <math.h>

typedef struct
{
	struct jvx_fxlms_init initCpy;
	struct jvx_fxlms_async asyncCpy;
	struct jvx_fxlms_sync syncCpy;

	struct
	{
		struct
		{
			jvx_circbuffer* states_adaptive_filter;
			jvx_circbuffer* states_secPath_regenerate;
			jvx_circbuffer* states_secPath_fxlms;
			jvx_circbuffer* states_secPath_real;
			jvx_circbuffer* lms_update;
		} circbuffer_states;

		struct
		{
			jvx_circbuffer* delay_secpath_real;
			jvx_circbuffer* delay_secpath_regenerate;
			jvx_circbuffer* delay_secpath_fxlms;
		} cirbuffer_delay;

		struct
		{
			jvxData* nom;
			jvxData* den;
			jvx_circbuffer* states;
		} selectFrequency_fxh;

		struct
		{
			jvxData* nom;
			jvxData* den;
			jvx_circbuffer* states;
		} selectFrequency_fe;

		struct
		{
			jvxData* nom;
			jvxData* den;
			jvx_circbuffer* states;
		} selectFrequency_fx;

		jvxData S_e;
	}runtime;

} fxlms_feedback_private;


static
jvxErrorType  jvx_fxlms_config_init(struct jvx_fxlms* hdlOnReturn)
{
	if (hdlOnReturn)
	{
		hdlOnReturn->async.mu = 1.0;
		hdlOnReturn->async.alpha = 1.0;
		hdlOnReturn->async.alpha_smooth = 0.99;
		hdlOnReturn->async.effectiveDelaySecPath = 1;
		hdlOnReturn->async.eps = JVX_DATA_EPS;
		hdlOnReturn->async.leakage = 1.0;
		hdlOnReturn->async.maxValAmplitude = 1.0;
		hdlOnReturn->async.stepsizeStrategy = RTP_FXLMS_UPDATE_STRATEGY_CONV;

		hdlOnReturn->init.delay_realSecPath = 1;
		hdlOnReturn->init.delay_estimSecPath = 1;
		hdlOnReturn->init.length_realSecPath = 1;
		hdlOnReturn->init.length_estimSecPath = 1;
		hdlOnReturn->init.length_adapFilter = 256;
		hdlOnReturn->init.freqSelect_omegaMin = 0.0;
		hdlOnReturn->init.freqSelect_omegaMax = M_PI;

		hdlOnReturn->sync.coeffs_adaptiveFilter = NULL;
		hdlOnReturn->sync.coeffs_estimSecPath = NULL;
		hdlOnReturn->sync.coeffs_realSecPath = NULL;

		hdlOnReturn->sync.frequencySelectionFilter_fex.omega_freq = 0;
		hdlOnReturn->sync.frequencySelectionFilter_fex.qfactor = 0;
		hdlOnReturn->sync.frequencySelectionFilter_fxh.omega_freq = 0;
		hdlOnReturn->sync.frequencySelectionFilter_fxh.qfactor = 0;

		hdlOnReturn->prv = NULL;

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

// ===================================================================================================

static void
jvx_compute_coeffs_frequency_selection(jvxData* nom, jvxData* den, jvxData omega_f, jvxData qfactor)
{
	jvxData alpha = 0;
	jvxData beta = 0;
	jvxData gamma = 0;

	beta = (qfactor - omega_f / 2) / (2 * qfactor + omega_f);
	gamma = ((0.5 + beta) * cos(omega_f));
	alpha = ((0.5 - beta) / 2.0);

	nom[0] = (2.0 * alpha);
	nom[1] = 0;
	nom[2] = (-2.0 * alpha);

	den[0] = 1.0;
	den[1] = (-2.0 * gamma);
	den[2] = (2.0 * beta);
}

// ===================================================================================================

jvxErrorType jvx_fxlms_feedback_init(struct jvx_fxlms* hdlIn)
{
	fxlms_feedback_private* theHandle = NULL;
	if (hdlIn)
	{
		JVX_SAFE_ALLOCATE_OBJECT_Z(theHandle, fxlms_feedback_private);
		//fxlms_feedback_private* theHandle = new fxlms_feedback_private;

		// Copy init struct
		theHandle->initCpy = hdlIn->init;
		theHandle->asyncCpy = hdlIn->async;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(theHandle->syncCpy.coeffs_adaptiveFilter, jvxData, theHandle->initCpy.length_adapFilter);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(theHandle->syncCpy.coeffs_realSecPath, jvxData, theHandle->initCpy.length_realSecPath);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(theHandle->syncCpy.coeffs_estimSecPath, jvxData, theHandle->initCpy.length_estimSecPath);

		theHandle->syncCpy.frequencySelectionFilter_fxh.omega_freq = 1;
		theHandle->syncCpy.frequencySelectionFilter_fxh.qfactor = 0.0;

		theHandle->syncCpy.frequencySelectionFilter_fex.omega_freq = 1;
		theHandle->syncCpy.frequencySelectionFilter_fex.qfactor = 0.0;

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(theHandle->runtime.selectFrequency_fxh.nom, jvxData, 3);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(theHandle->runtime.selectFrequency_fxh.den, jvxData, 3);
		jvx_compute_coeffs_frequency_selection(theHandle->runtime.selectFrequency_fxh.nom, theHandle->runtime.selectFrequency_fxh.den,
			theHandle->syncCpy.frequencySelectionFilter_fxh.omega_freq, theHandle->syncCpy.frequencySelectionFilter_fxh.qfactor);
		jvx_circbuffer_allocate_1chan(&theHandle->runtime.selectFrequency_fxh.states, 2);

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(theHandle->runtime.selectFrequency_fe.nom, jvxData, 3);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(theHandle->runtime.selectFrequency_fe.den, jvxData, 3);
		jvx_compute_coeffs_frequency_selection(theHandle->runtime.selectFrequency_fe.nom, theHandle->runtime.selectFrequency_fe.den,
			theHandle->syncCpy.frequencySelectionFilter_fex.omega_freq, theHandle->syncCpy.frequencySelectionFilter_fex.qfactor);
		jvx_circbuffer_allocate_1chan(&theHandle->runtime.selectFrequency_fe.states, 2);

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(theHandle->runtime.selectFrequency_fx.nom, jvxData, 3);
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(theHandle->runtime.selectFrequency_fx.den, jvxData, 3);
		jvx_compute_coeffs_frequency_selection(theHandle->runtime.selectFrequency_fx.nom, theHandle->runtime.selectFrequency_fx.den,
			theHandle->syncCpy.frequencySelectionFilter_fex.omega_freq, theHandle->syncCpy.frequencySelectionFilter_fex.qfactor);
		jvx_circbuffer_allocate_1chan(&theHandle->runtime.selectFrequency_fx.states, 2);

		jvx_circbuffer_allocate_1chan(&theHandle->runtime.circbuffer_states.states_adaptive_filter, theHandle->initCpy.length_adapFilter);
		jvx_circbuffer_allocate_1chan(&theHandle->runtime.circbuffer_states.lms_update, theHandle->initCpy.length_adapFilter);
		jvx_circbuffer_allocate_1chan(&theHandle->runtime.circbuffer_states.states_secPath_regenerate, theHandle->initCpy.length_estimSecPath);
		jvx_circbuffer_allocate_1chan(&theHandle->runtime.circbuffer_states.states_secPath_fxlms, theHandle->initCpy.length_estimSecPath);

		jvx_circbuffer_allocate_1chan(&theHandle->runtime.circbuffer_states.states_secPath_real, theHandle->initCpy.length_realSecPath);


		jvx_circbuffer_allocate_1chan(&theHandle->runtime.cirbuffer_delay.delay_secpath_fxlms, theHandle->initCpy.delay_estimSecPath);
		jvx_circbuffer_fill(theHandle->runtime.cirbuffer_delay.delay_secpath_fxlms, 0.0, theHandle->initCpy.delay_estimSecPath);

		jvx_circbuffer_allocate_1chan(&theHandle->runtime.cirbuffer_delay.delay_secpath_regenerate, theHandle->initCpy.delay_estimSecPath);
		jvx_circbuffer_fill(theHandle->runtime.cirbuffer_delay.delay_secpath_regenerate, 0.0, theHandle->initCpy.delay_estimSecPath);

		jvx_circbuffer_allocate_1chan(&theHandle->runtime.cirbuffer_delay.delay_secpath_real, theHandle->initCpy.delay_realSecPath);
		jvx_circbuffer_fill(theHandle->runtime.cirbuffer_delay.delay_secpath_real, 0.0, theHandle->initCpy.delay_realSecPath);

		theHandle->runtime.S_e = 0.0;
		hdlIn->sync = theHandle->syncCpy;

		hdlIn->prv = theHandle;

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType jvx_fxlms_feedback_terminate(struct jvx_fxlms* hdlIn)
{
	fxlms_feedback_private* theHandle = hdlIn->prv;

	if (theHandle)
	{
		theHandle->runtime.S_e = 0.0;

		jvx_circbuffer_deallocate(theHandle->runtime.cirbuffer_delay.delay_secpath_real);
		jvx_circbuffer_deallocate(theHandle->runtime.cirbuffer_delay.delay_secpath_regenerate);
		jvx_circbuffer_deallocate(theHandle->runtime.cirbuffer_delay.delay_secpath_fxlms);

		jvx_circbuffer_deallocate(theHandle->runtime.circbuffer_states.states_secPath_real);
		jvx_circbuffer_deallocate(theHandle->runtime.circbuffer_states.states_secPath_fxlms);

		jvx_circbuffer_deallocate(theHandle->runtime.circbuffer_states.states_secPath_regenerate);
		jvx_circbuffer_deallocate(theHandle->runtime.circbuffer_states.lms_update);
		jvx_circbuffer_deallocate(theHandle->runtime.circbuffer_states.states_adaptive_filter);

		jvx_circbuffer_deallocate(theHandle->runtime.selectFrequency_fx.states);
		JVX_DSP_SAFE_DELETE_FIELD(theHandle->runtime.selectFrequency_fx.nom);
		JVX_DSP_SAFE_DELETE_FIELD(theHandle->runtime.selectFrequency_fx.den);

		jvx_circbuffer_deallocate(theHandle->runtime.selectFrequency_fe.states);
		JVX_DSP_SAFE_DELETE_FIELD(theHandle->runtime.selectFrequency_fe.nom);
		JVX_DSP_SAFE_DELETE_FIELD(theHandle->runtime.selectFrequency_fe.den);

		jvx_circbuffer_deallocate(theHandle->runtime.selectFrequency_fxh.states);
		JVX_DSP_SAFE_DELETE_FIELD(theHandle->runtime.selectFrequency_fxh.nom);
		JVX_DSP_SAFE_DELETE_FIELD(theHandle->runtime.selectFrequency_fxh.den);

		memset(&hdlIn->sync, 0, sizeof(hdlIn->sync));

		JVX_SAFE_DELETE_OBJECT(theHandle);
		return JVX_NO_ERROR;
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

// ===================================================================================================

jvxErrorType jvx_fxlms_feedback_process(struct jvx_fxlms* hdlIn, jvxData input_e, jvxData* output_x, 
	jvxCBool* limiter, jvxData* ret_energy_filter, jvxData overwrite_leakage, jvxData* output_xh_filtered)
{
	jvxData out_delay_secpath_real = 0.0;
	jvxData out_secpath_real = 0.0;
	jvxData out_delay_secpath_regenerate = 0.0;
	jvxData out_secpath_regenerate = 0.0;
	jvxData out_delay_secpath_fxlms = 0.0;
	jvxData out_secpath_fxlms = 0.0;
	jvxData out_adaptive_filter = 0.0;	
	jvxData norm = 0.0;
	jvxData norm_per_sample = 0.0;
	jvxData beta = 0;
	jvxData tmp1 = 0, tmp2 = 0;
	jvxData energy_filter = 0;
	jvxData leakage_local = 0.0;
	jvxCBool active_fxh = false;
	jvxCBool active_fe = false;
	jvxData* ptrTmp = NULL;

	if(hdlIn)
	{
		fxlms_feedback_private* hdl = (fxlms_feedback_private*) hdlIn;

		// Use the default leakage
		leakage_local = JVX_MIN(hdl->asyncCpy.leakage, overwrite_leakage);
		active_fxh = hdl->asyncCpy.frequencySelectionFilter.active_fxh;
		active_fe = hdl->asyncCpy.frequencySelectionFilter.active_fe;

		// Input: e(k) comes from soundcard

		// ========================================
		// ============= Regeneration =============
		// ========================================

		// Read from delay buffer to regenerate signal x(k)
		jvx_circbuffer_read_update_1chan(hdl->runtime.cirbuffer_delay.delay_secpath_regenerate, &out_delay_secpath_regenerate, 1);

		// Feed output into convolution
		jvx_circbuffer_write_convolve_update_1chan(hdl->runtime.circbuffer_states.states_secPath_regenerate, 
			out_delay_secpath_regenerate, hdl->syncCpy.coeffs_realSecPath, hdl->initCpy.length_realSecPath, 
			&out_secpath_regenerate);

		// This is the "regenerated" noise signal
		out_secpath_regenerate += input_e;

		// ========================================
		// Employ the frequency selection filter here!
		// ========================================
		if(active_fxh)
		{
			ptrTmp = &out_secpath_regenerate;
			jvx_circbuffer_iir_1can_1io(hdl->runtime.selectFrequency_fxh.states, hdl->runtime.selectFrequency_fxh.nom, 
				hdl->runtime.selectFrequency_fxh.den, &ptrTmp, 1);
		}

		if(output_xh_filtered)
		{
			*output_xh_filtered = out_secpath_regenerate;
		}

		// ========================================
		// ========== FXLMS FXLMS FXLMS  ==========
		// ========================================

		jvx_circbuffer_read_update_1chan(hdl->runtime.cirbuffer_delay.delay_secpath_fxlms, &out_delay_secpath_fxlms, 1);

		// Feed output into convolution
		jvx_circbuffer_write_convolve_update_1chan(hdl->runtime.circbuffer_states.states_secPath_fxlms, 
			out_delay_secpath_fxlms, hdl->syncCpy.coeffs_realSecPath, hdl->initCpy.length_realSecPath, &out_secpath_fxlms);

		if(active_fe)
		{
			// This is the Felms part: Filter e_k as well as the FxLMS secopndary path filtering output
			ptrTmp = &out_secpath_fxlms;
			jvx_circbuffer_iir_1can_1io(hdl->runtime.selectFrequency_fx.states, hdl->runtime.selectFrequency_fx.nom, 
				hdl->runtime.selectFrequency_fx.den, &ptrTmp, 1);

			ptrTmp = &input_e;
			jvx_circbuffer_iir_1can_1io(hdl->runtime.selectFrequency_fe.states, hdl->runtime.selectFrequency_fe.nom, 
				hdl->runtime.selectFrequency_fe.den, &ptrTmp, 1);
		}

		// Feed output into convolution
		jvx_circbuffer_write_norm_no_update_1chan(hdl->runtime.circbuffer_states.lms_update, out_secpath_fxlms, &norm);

		jvx_circbuffer_write_convolve_update_1chan(hdl->runtime.circbuffer_states.states_adaptive_filter,
			out_secpath_regenerate, hdl->syncCpy.coeffs_adaptiveFilter, hdl->initCpy.length_adapFilter, &out_adaptive_filter);


		// ========================================
		// ======= Adaptive Filter update  ========
		// ========================================
		// Derive per sample average norm
		norm_per_sample = norm / (jvxData)hdl->initCpy.length_adapFilter;

		// Stepsize update rule
		switch(hdl->asyncCpy.stepsizeStrategy)
		{
		case RTP_FXLMS_UPDATE_STRATEGY_CONV:
			beta = hdl->asyncCpy.mu/(norm_per_sample * (hdl->initCpy.length_adapFilter * hdl->asyncCpy.effectiveDelaySecPath) + hdl->asyncCpy.eps);
			break;
		case RTP_FXLMS_UPDATE_STRATEGY_MODIFIED:
			tmp1 = input_e * input_e;
			tmp1 *= (1 - hdl->asyncCpy.alpha_smooth);
			tmp2 = hdl->asyncCpy.alpha_smooth * hdl->runtime.S_e;
			hdl->runtime.S_e = tmp1 + tmp2;

			tmp1 = (hdl->asyncCpy.alpha * norm_per_sample + hdl->runtime.S_e);
			tmp1 = tmp1 * (hdl->initCpy.length_adapFilter * hdl->asyncCpy.effectiveDelaySecPath);
			tmp1 += hdl->asyncCpy.eps;
			tmp2 = hdl->asyncCpy.mu * hdl->asyncCpy.alpha;
			beta = tmp2/tmp1;
			break;
		}

/*		jvx_circbuffer_fld_mult_fac_add_circbuffer_fac)(hdl->used_area_s->coeffs_adaptiveFilter, hdl->the_setup->length_adapFilter,
			hdl->used_area_a->leakage, (beta * input_e), hdl->runtime.circbuffer_states.lms_update);
			*/
		jvx_circbuffer_fld_mult_fac_add_circbuffer_fac_ret_energy(hdl->syncCpy.coeffs_adaptiveFilter, hdl->initCpy.length_adapFilter,
			leakage_local, (beta * input_e), hdl->runtime.circbuffer_states.lms_update, &energy_filter);

		// Store current energy inherent to adaptive filter filter coefficients
		hdl->asyncCpy.energy_filter = energy_filter;
		// Now update circular buffer for update
		jvx_circbuffer_write_norm_only_update_1chan(hdl->runtime.circbuffer_states.lms_update);
		jvx_circbuffer_write_update_1chan(hdl->runtime.cirbuffer_delay.delay_secpath_fxlms, &out_secpath_regenerate, 1);
		jvx_circbuffer_write_update_1chan(hdl->runtime.cirbuffer_delay.delay_secpath_regenerate, &out_adaptive_filter, 1);

		*limiter = false;

		// Output limiter, there is one also 
		if(out_adaptive_filter > hdl->asyncCpy.maxValAmplitude)
		{
			*limiter = true;
			out_adaptive_filter = hdl->asyncCpy.maxValAmplitude;
		}
		if(out_adaptive_filter < -hdl->asyncCpy.maxValAmplitude)
		{
			*limiter = true;
			out_adaptive_filter = -hdl->asyncCpy.maxValAmplitude;
		}

		// Write output
		*output_x = out_adaptive_filter;

		// If desired, return the current energy in adaptive filter
		if(ret_energy_filter)
		{
			*ret_energy_filter = energy_filter;
		}

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
