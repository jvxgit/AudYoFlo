#include "jvx_iir_fb/jvx_iir_fb.h"
#include "jvx_dsp_base.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
#include "jvx_math/jvx_complex.h"
#include <math.h>

extern int jvxprintf(const char * format, ...);

#define JVX_CIRCBUFFER_FILTER_1IO jvx_circbuffer_iir_1can_1io
#define JVX_CIRCBUFFER_FILTER_2IO jvx_circbuffer_iir_1can_2io

// ====================================================================
// ====================================================================
// ====================================================================
// Private data structs
struct jvxIirFbOneFilter
{
	jvxData* num_poly;
	jvxData* den_poly;
	jvxSize num_coeffs;
	jvx_circbuffer* states;
};

struct jvxIirFbPrefilter
{
	struct jvxIirFbOneFilter B;
} jvxIirFbPrefilter;

struct jvx_iir_fb_one_stage
{
	jvxSize frequency;
	jvxSize order;

	struct
	{
		struct jvxIirFbOneFilter A;
		jvxSize oBufId;
		struct jvx_iir_fb_one_stage* next;
	} lp;

	struct
	{
		struct jvxIirFbOneFilter A;
		jvxSize oBufId;
		struct jvx_iir_fb_one_stage* next;
	} hp;

	jvxSize iBufId;
	struct jvxIirFbPrefilter* prefilters;
	jvxSize num_prefilters;
};

typedef struct
{
	jvx_iir_fb_prmInit init_copy;
	struct jvx_iir_fb_one_stage entryTree;

} jvx_iir_fb_private;
// ====================================================================
// ====================================================================

// ====================================================================
// ====================================================================
// ====================================================================
void jvx_iir_fb_init_one_stage(struct jvx_iir_fb_one_stage* theS);

jvxDspBaseErrorType jvx_iir_fb_reset_stage(struct jvx_iir_fb_one_stage* this_stage);
jvxDspBaseErrorType jvx_iir_fb_construct_tree(jvxSize* splitFrequencies, jvxSize* orders, jvxSize numSplitFrequencies,
	jvxSize levels, jvxSize idxLevel, struct jvx_iir_fb_one_stage* this_stage,
	jvxData minf, jvxData maxf, jvxSize bufIdxIn, jvxSize fs, jvxIirFbType ftype, jvxSize* numSubNodes_return);
jvxDspBaseErrorType jvx_iir_fb_destroy_tree(struct jvx_iir_fb_one_stage* this_stage);
jvxDspBaseErrorType jvx_iir_fb_design_butterworth(jvxData fc, jvxData fs, jvxSize order, jvxDataCplx* tmpBufP);
jvxDspBaseErrorType jvx_iir_fb_design_filter(struct jvx_iir_fb_one_stage* this_stage, jvxSize fs, jvxIirFbType fbTp, 
	jvxDataCplx* tmpBufP, jvxSize numChannels);
jvxDspBaseErrorType jvx_iir_fb_copy_filter_coeffs(struct jvx_iir_fb_one_stage* tree, jvxData* coeffs_num, jvxData* coeffs_den, jvxData** memory, jvxSize* array_coeff_numbers, jvxSize num_filters, jvxSize* filter_counter, jvxSize* coeff_counter, jvxSize numChannels);
jvxDspBaseErrorType jvx_iir_fb_design_filter_coeffs(jvxSize fc, jvxSize order, struct jvxIirFbOneFilter* lp_A, struct jvxIirFbOneFilter* hp_A, 
	jvxSize fs, jvxIirFbType tp, jvxDataCplx* tmpBuf1, jvxSize numChannels);
jvxDspBaseErrorType jvx_iir_fb_link_prefilter_allocate(struct jvx_iir_fb_one_stage* this_stage, jvxSize* retVal, jvxSize* retValMax);
jvxDspBaseErrorType jvx_iir_fb_link_prefilter_deallocate(struct jvx_iir_fb_one_stage* this_stage);
jvxDspBaseErrorType jvx_iir_fb_link_prefilter_set(struct jvx_iir_fb_one_stage* this_stage, struct jvxIirFbPrefilter* bufRet, jvxSize* retVal,
	jvxSize numChannels);
jvxDspBaseErrorType jvx_iir_fb_show_filter(struct jvx_iir_fb_one_stage* hdl, jvxSize lev);
jvxDspBaseErrorType jvx_iir_fb_link_prefilter(struct jvx_iir_fb_one_stage* this_stage, jvxSize numChannels);
jvxDspBaseErrorType jvx_iir_fb_filter_deallocate(struct jvx_iir_fb_one_stage* this_stage);
jvxDspBaseErrorType jvx_iir_fb_process_tree(struct jvx_iir_fb_one_stage* this_stage, jvxData*** bufOut, jvxSize bsize, jvxSize numChannels);

// ====================================================================
// ====================================================================

// ====================================================================
// ====================================================================
// ====================================================================

jvxDspBaseErrorType
jvx_iir_fb_initConfig(jvx_iir_fb* handle)
{
	handle->prm_init.fbType = JVX_MBC_FILTER_DESIGN_BUTTERWORTH;
	handle->prm_init.fs = 48000;
	handle->prm_init.numChannels = 2;
	handle->prm_init.numSplitFrequencies = 3;
	
	handle->prm_init.orders = NULL;
	handle->prm_init.splitFrequencies = NULL;
	handle->prm_init.numSplitFrequencies = 0;

	handle->prm_init.verbose = c_false;

	handle->prm_derived.numSubNodes = 0;

	handle->prv = NULL;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType 
jvx_iir_fb_prepare(jvx_iir_fb* handleOnReturn, const char* prefix_verbose)
{
	jvxSize i;
	jvxSize levels = 0;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize orderMax = 0;
	jvxDataCplx* tmpDesignBufP = NULL;

	if (handleOnReturn->prv == NULL)
	{
		jvx_iir_fb_private* hdl = NULL;

		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(hdl, jvx_iir_fb_private);
		hdl->init_copy = handleOnReturn->prm_init;

		// Make some corrections: Only odd orders
		for (i = 0; i < hdl->init_copy.numSplitFrequencies; i++)
		{
			if ((hdl->init_copy.orders[i] % 2) == 0)
				hdl->init_copy.orders[i]++;
			if (hdl->init_copy.orders[i] > orderMax)
			{
				orderMax = hdl->init_copy.orders[i];
			}
		}
		
		if (hdl->init_copy.verbose)
		{
			jvxprintf("%s = jvx_iir_fb<C>: =====================================", prefix_verbose);
			jvxprintf("%s = jvx_iir_fb<C>: ========== FB Design ================", prefix_verbose);
			jvxprintf("%s = jvx_iir_fb<C>: =====================================", prefix_verbose);
		}

		// Compute tree depth
		levels = JVX_DATA2SIZE( ceil(log((jvxData)hdl->init_copy.numSplitFrequencies + 1.0) / log(2.0)));

		// Initialize tree
		jvx_iir_fb_init_one_stage(&hdl->entryTree);
		
		if (hdl->init_copy.verbose)
		{
			jvxprintf("%s = jvx_iir_fb<C>: Deriving filterbank tree", prefix_verbose);
		}

		// Recursive tree design
		resL = jvx_iir_fb_construct_tree(hdl->init_copy.splitFrequencies, hdl->init_copy.orders, hdl->init_copy.numSplitFrequencies,
			levels, 1, &hdl->entryTree, 0, (jvxData)hdl->init_copy.fs / 2.0, 0, hdl->init_copy.fs, hdl->init_copy.fbType, 
			&handleOnReturn->prm_derived.numSubNodes);

		if (hdl->init_copy.verbose) 
		{
			jvxprintf("%s = jvx_iir_fb<C>: Designing stage trees in C\n", prefix_verbose);
		}

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(tmpDesignBufP, jvxDataCplx, orderMax);

		// Recursive filter design
		resL = jvx_iir_fb_design_filter(&hdl->entryTree, hdl->init_copy.fs, hdl->init_copy.fbType, tmpDesignBufP, hdl->init_copy.numChannels);

		JVX_DSP_SAFE_DELETE_FIELD(tmpDesignBufP);

		// Recursive prefilter linkage
		resL = jvx_iir_fb_link_prefilter(&hdl->entryTree, hdl->init_copy.numChannels);

		// Recursive filter design
		if (hdl->init_copy.verbose)
		{
			resL = jvx_iir_fb_show_filter(&hdl->entryTree, 0);
		}
		handleOnReturn->prv = hdl;
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_iir_fb_reset(jvx_iir_fb* handle)
{
	jvxDspBaseErrorType resL = JVX_DSP_ERROR_WRONG_STATE;

	if (handle->prv)
	{
		jvx_iir_fb_private* hdl = (jvx_iir_fb_private*)handle->prv;
		resL = jvx_iir_fb_reset_stage(&hdl->entryTree);
	}
	return resL;
}

jvxDspBaseErrorType jvx_iir_fb_postprocess(jvx_iir_fb* handleOnReturn)
{
	if (handleOnReturn->prv)
	{
		jvx_iir_fb_private* hdl = (jvx_iir_fb_private*)handleOnReturn->prv;
		jvx_iir_fb_link_prefilter_deallocate(&hdl->entryTree);
		jvx_iir_fb_filter_deallocate(&hdl->entryTree);
		jvx_iir_fb_destroy_tree(&hdl->entryTree);
		JVX_DSP_SAFE_DELETE_OBJECT(hdl);
		handleOnReturn->prv = NULL;
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_iir_fb_update(jvx_iir_fb* handle, jvxUInt16 whatToUpdate, jvxCBool do_set)
{
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType 
jvx_iir_fb_process(jvx_iir_fb* hhandle, jvxData** bufIn, jvxData*** bufOut, jvxSize bsize)
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize i;
	if (hhandle->prv)
	{
		jvx_iir_fb_private* hdl = (jvx_iir_fb_private*)hhandle->prv;

		for (i = 0; i < hdl->init_copy.numChannels; i++)
		{
			memcpy(bufOut[0][i], bufIn[i], sizeof(jvxData)*bsize);
		}

		resL = jvx_iir_fb_process_tree(&hdl->entryTree, bufOut, bsize, hdl->init_copy.numChannels);
	}

	return resL;
}

jvxDspBaseErrorType
jvx_iir_fb_process_tree(struct jvx_iir_fb_one_stage* this_stage, jvxData*** bufOut, jvxSize bsize, jvxSize numChannels)
{
	jvxSize i,j;
	jvxSize idIn = this_stage->iBufId;
	jvxData accu1, accu2;
	jvxData** buf0, **buf1;
	
	for (i = 0; i < this_stage->num_prefilters; i++)
	{
		JVX_CIRCBUFFER_FILTER_1IO(this_stage->prefilters[i].B.states, this_stage->prefilters[i].B.num_poly, this_stage->prefilters[i].B.den_poly,
			bufOut[idIn], bsize);
	}
	
	if (JVX_CHECK_SIZE_SELECTED(this_stage->frequency))
	{
		buf0 = bufOut[this_stage->hp.oBufId];
		JVX_CIRCBUFFER_FILTER_2IO(this_stage->hp.A.states, this_stage->hp.A.num_poly, this_stage->hp.A.den_poly, bufOut[idIn], buf0, bsize);

		buf1 = bufOut[this_stage->lp.oBufId];
		JVX_CIRCBUFFER_FILTER_1IO(this_stage->lp.A.states, this_stage->lp.A.num_poly, this_stage->lp.A.den_poly, bufOut[idIn], bsize);

		for (j = 0; j < numChannels; j++)
		{
			for (i = 0; i < bsize; i++)
			{
				accu1 = (buf1[j][i] + buf0[j][i]) * 0.5;
				accu2 = (buf1[j][i] - buf0[j][i]) * 0.5;

				buf1[j][i] = accu1;
				buf0[j][i] = accu2;
			}
		}
	}

	if (this_stage->lp.next)
	{
		jvx_iir_fb_process_tree(this_stage->lp.next, bufOut, bsize, numChannels);
	}
	if (this_stage->hp.next)
	{
		jvx_iir_fb_process_tree(this_stage->hp.next, bufOut, bsize, numChannels);
	}
	return JVX_DSP_NO_ERROR;
}

// =================================================================================
// =================================================================================
// =================================================================================
// =================================================================================
void
jvx_iir_fb_init_one_stage(struct jvx_iir_fb_one_stage* theS)
{
	theS->frequency = -1;
	theS->iBufId = -1;
	theS->prefilters = NULL;
	theS->num_prefilters = 0;
	theS->order = 0;

	theS->lp.A.num_coeffs = 0;
	theS->lp.A.num_poly = NULL;
	theS->lp.A.den_poly = NULL;
	theS->lp.A.states = NULL;

	theS->lp.next = NULL;
	theS->lp.oBufId = JVX_SIZE_UNSELECTED;

	theS->hp.A.num_coeffs = 0;
	theS->hp.A.num_poly = NULL;
	theS->hp.A.den_poly = NULL;
	theS->hp.A.states = NULL;
	theS->hp.next = NULL;
	theS->hp.oBufId = JVX_SIZE_UNSELECTED;
}; 

jvxDspBaseErrorType
jvx_iir_fb_reset_stage(struct jvx_iir_fb_one_stage* this_stage)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	for (i = 0; i < this_stage->num_prefilters; i++)
	{
		jvx_circbuffer_reset(this_stage->prefilters[i].B.states);
	}
	if (JVX_CHECK_SIZE_SELECTED(this_stage->frequency))
	{
		jvx_circbuffer_reset(this_stage->lp.A.states);
		jvx_circbuffer_reset(this_stage->hp.A.states);
	}
	if (this_stage->lp.next)
	{
		resL = jvx_iir_fb_reset_stage(this_stage->lp.next);
		assert(resL == JVX_DSP_NO_ERROR);
	}
	if (this_stage->hp.next)
	{
		resL = jvx_iir_fb_reset_stage(this_stage->hp.next);
		assert(resL == JVX_DSP_NO_ERROR);
	}
	return(resL);
}

jvxDspBaseErrorType
jvx_iir_fb_construct_tree(jvxSize* splitFrequencies, jvxSize* orders, jvxSize numSplitFrequencies,
	jvxSize levels, jvxSize idxLevel, struct jvx_iir_fb_one_stage* this_stage, 
	jvxData minf, jvxData maxf, jvxSize bufIdxIn, jvxSize fs, jvxIirFbType ftype, jvxSize* numSubNodes_return)
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize idxSelect = 0;
	jvxData midf = 0, absdiff = 0;
	jvxSize i;
	jvxSize subNodes_lp;
	jvxSize subNodes_hp;
	jvxSize tmp;

	if (numSplitFrequencies == 0)
	{

		// Special rule for the leaf sections.
		this_stage->lp.oBufId = bufIdxIn;
		this_stage->iBufId = bufIdxIn;
		this_stage->frequency = JVX_SIZE_UNSELECTED;

		if (idxLevel < levels)
		{
			this_stage->lp.next = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(this_stage->lp.next, struct jvx_iir_fb_one_stage);
			resL = jvx_iir_fb_construct_tree(splitFrequencies, orders, numSplitFrequencies,
				levels, idxLevel + 1, this_stage->lp.next,
				minf, maxf, bufIdxIn, fs, ftype, numSubNodes_return);
		}
		else
		{
			*numSubNodes_return = 1;
		}
	}
	else
	{

		// Separate all possible frequencies into equivalent halfs; idxSelect is an index starting with 1!
		idxSelect = JVX_DATA2SIZE(ceil(((jvxData)numSplitFrequencies + 1.0) / 2.0));

		// On the very first level, we may modify the tree to better separate
		// spectrum into halfs
		if (idxLevel == 1)
		{
			midf = (maxf + minf) / 2.0;
			absdiff = maxf;
			for (i = 0; i < numSplitFrequencies; i++)
			{
				if (fabs(splitFrequencies[i] - midf) < absdiff)
				{
					idxSelect = i + 1;
					absdiff = fabs(splitFrequencies[i] - midf);
				}
			}

			// Make sure the tree is somewhat symmetric
			tmp = (jvxSize)(1 << (levels - idxLevel));
			if (idxSelect > tmp)
			{
				idxSelect = tmp;
			}
			tmp = numSplitFrequencies + 1 - (jvxSize)(1 << (levels - idxLevel));
			if (idxSelect < tmp)
			{
				idxSelect = tmp;
			}
		}

		// Branches or leafs
		if (idxSelect > 0)
		{
			// This is a branch in which the spectrum is divided into halfs
			this_stage->frequency = splitFrequencies[idxSelect - 1];
			this_stage->order = orders[idxSelect - 1];
			this_stage->iBufId = bufIdxIn;

			// Add the filters here
			// TODO [oneStage.lp.A, oneStage.hp.A] = jvx_design_allpasses(oneStage.frequency, fs, oneStage.order, fType);

			// Alloate space for the states
			// TODO oneStage.lp.states = [];
			// TODO oneStage.hp.states = [];

			if (idxLevel < levels)
			{
				// Branch to low pass and high pass subtrees
				JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(this_stage->lp.next, struct jvx_iir_fb_one_stage);
				jvx_iir_fb_init_one_stage(this_stage->lp.next);

				resL = jvx_iir_fb_construct_tree(splitFrequencies, orders, idxSelect - 1,
					levels, idxLevel + 1, this_stage->lp.next,
					minf, (jvxData)splitFrequencies[idxSelect - 1], bufIdxIn, fs, ftype, &subNodes_lp);

				this_stage->lp.oBufId = bufIdxIn;

				bufIdxIn = bufIdxIn + subNodes_lp;
				JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(this_stage->hp.next, struct jvx_iir_fb_one_stage);
				jvx_iir_fb_init_one_stage(this_stage->hp.next);

				resL = jvx_iir_fb_construct_tree(splitFrequencies + idxSelect, orders + idxSelect,
					numSplitFrequencies - idxSelect,
					levels, idxLevel + 1, this_stage->hp.next,
					(jvxData)splitFrequencies[idxSelect - 1], maxf, bufIdxIn, fs, ftype, &subNodes_hp);

				this_stage->hp.oBufId = bufIdxIn;

				// Number of subnodes in subtree
				*numSubNodes_return = subNodes_lp + subNodes_hp;
			}
			else
			{
				// This is the last stage in the tree
				this_stage->lp.oBufId = bufIdxIn;
				bufIdxIn = bufIdxIn + 1;
				this_stage->hp.oBufId = bufIdxIn;
				*numSubNodes_return = 2;
			}
		}
	}

	return resL;
}

jvxDspBaseErrorType 
jvx_iir_fb_destroy_tree(struct jvx_iir_fb_one_stage* this_stage)
{
	if (this_stage->lp.next)
	{
		jvx_iir_fb_destroy_tree(this_stage->lp.next);
		JVX_DSP_SAFE_DELETE_OBJECT(this_stage->lp.next);
		this_stage->lp.next = NULL;
	}
	if (this_stage->hp.next)
	{
		jvx_iir_fb_destroy_tree(this_stage->hp.next);
		JVX_DSP_SAFE_DELETE_OBJECT(this_stage->hp.next);
		this_stage->hp.next = NULL;
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType 
jvx_iir_fb_design_filter(struct jvx_iir_fb_one_stage* this_stage, jvxSize fs, jvxIirFbType fbTp, jvxDataCplx* tmpBufP, jvxSize numChannels)
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	if (this_stage->frequency > 0)
	{
		if (JVX_CHECK_SIZE_SELECTED(this_stage->frequency))
		{
			// Design filter only if filter design required
			jvx_iir_fb_design_filter_coeffs(this_stage->frequency, this_stage->order, &this_stage->lp.A, &this_stage->hp.A, fs, fbTp, tmpBufP, numChannels);
		}
		if (this_stage->lp.next)
		{
			resL = jvx_iir_fb_design_filter(this_stage->lp.next, fs, fbTp, tmpBufP,  numChannels);
		}
		if (this_stage->hp.next)
		{
			resL = jvx_iir_fb_design_filter(this_stage->hp.next, fs, fbTp, tmpBufP,  numChannels);
		}
	}
	return(resL);
}

jvxDspBaseErrorType
jvx_iir_fb_filter_deallocate(struct jvx_iir_fb_one_stage* this_stage)
{
	if (this_stage->lp.A.num_coeffs > 0)
	{
		JVX_DSP_SAFE_DELETE_FIELD(this_stage->lp.A.num_poly);
		this_stage->lp.A.num_poly = NULL;
		JVX_DSP_SAFE_DELETE_FIELD(this_stage->lp.A.den_poly);
		this_stage->lp.A.den_poly = NULL;
		jvx_circbuffer_deallocate(this_stage->lp.A.states);
		this_stage->lp.A.states = NULL;
	}
	if (this_stage->hp.A.num_coeffs > 0)
	{
		JVX_DSP_SAFE_DELETE_FIELD(this_stage->hp.A.num_poly);
		this_stage->hp.A.num_poly = NULL;
		JVX_DSP_SAFE_DELETE_FIELD(this_stage->hp.A.den_poly);
		this_stage->hp.A.den_poly = NULL;
		jvx_circbuffer_deallocate(this_stage->hp.A.states);
		this_stage->hp.A.states = NULL;
	}
	if (this_stage->lp.next)
	{
		jvx_iir_fb_filter_deallocate(this_stage->lp.next);
	}
	if (this_stage->hp.next)
	{
		jvx_iir_fb_filter_deallocate(this_stage->hp.next);
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_iir_fb_link_prefilter(struct jvx_iir_fb_one_stage* this_stage, jvxSize numChannels)
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize retVal = 0;
	jvxSize retValMax = 0;
	struct jvxIirFbPrefilter* buf = NULL;
	resL = jvx_iir_fb_link_prefilter_allocate(this_stage, &retVal, &retValMax);

	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(buf, struct jvxIirFbPrefilter, retVal);

	retVal = 0;
	resL = jvx_iir_fb_link_prefilter_set(this_stage, buf, &retVal, numChannels);

	JVX_DSP_SAFE_DELETE_FIELD(buf);
	return(resL);
}


jvxDspBaseErrorType
jvx_iir_fb_link_prefilter_allocate(struct jvx_iir_fb_one_stage* this_stage, jvxSize* retVal, jvxSize* retValMax)
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize retValLocLp = 0, retValLocHp = 0;
	if (JVX_CHECK_SIZE_SELECTED(this_stage->frequency))
	{
		if (this_stage->lp.next && this_stage->hp.next)
		{

			// Recursively propagate through the tree
			jvx_iir_fb_link_prefilter_allocate(this_stage->lp.next, &retValLocLp, retValMax);
			jvx_iir_fb_link_prefilter_allocate(this_stage->hp.next, &retValLocHp, retValMax);

			// Cross link
			this_stage->lp.next->num_prefilters = retValLocHp;
			if (this_stage->lp.next->num_prefilters)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(this_stage->lp.next->prefilters, struct jvxIirFbPrefilter, this_stage->lp.next->num_prefilters);
			}
			this_stage->hp.next->num_prefilters = retValLocLp;
			if (this_stage->hp.next->num_prefilters)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(this_stage->hp.next->prefilters, struct jvxIirFbPrefilter, this_stage->hp.next->num_prefilters);
			}

			*retVal = 1 + retValLocHp + retValLocLp;
		}
		else
		{
			*retVal = 1;
		}
	}
	else
	{
		*retVal = 0;
	}
	return(resL);
}

jvxDspBaseErrorType
jvx_iir_fb_link_prefilter_deallocate(struct jvx_iir_fb_one_stage* this_stage)
{
	jvxSize i;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize retValLocLp = 0, retValLocHp = 0;
	if (JVX_CHECK_SIZE_SELECTED(this_stage->frequency))
	{
		if (this_stage->lp.next && this_stage->hp.next)
		{

			// Recursively propagate through the tree
			jvx_iir_fb_link_prefilter_deallocate(this_stage->lp.next);
			jvx_iir_fb_link_prefilter_deallocate(this_stage->hp.next);

			for (i = 0; i < this_stage->lp.next->num_prefilters; i++)
			{
				jvx_circbuffer_deallocate(this_stage->lp.next->prefilters[i].B.states);
				this_stage->lp.next->prefilters[i].B.states = NULL;
			}
			JVX_DSP_SAFE_DELETE_FIELD(this_stage->lp.next->prefilters);
			this_stage->lp.next->prefilters = NULL;
			this_stage->lp.next->num_prefilters = 0;

			for (i = 0; i < this_stage->hp.next->num_prefilters; i++)
			{
				jvx_circbuffer_deallocate(this_stage->hp.next->prefilters[i].B.states);
				this_stage->hp.next->prefilters[i].B.states = NULL;
			}
			JVX_DSP_SAFE_DELETE_FIELD(this_stage->hp.next->prefilters);
			this_stage->hp.next->prefilters = NULL;
			this_stage->hp.next->num_prefilters = 0;
		}
	}
	return(resL);
}

jvxDspBaseErrorType
jvx_iir_fb_link_prefilter_set(struct jvx_iir_fb_one_stage* this_stage, struct jvxIirFbPrefilter* bufRet, jvxSize* retVal, jvxSize numChannels)
{
	jvxSize i, cnt;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize retValLocLp = 0, retValLocHp = 0;
	if (JVX_CHECK_SIZE_SELECTED(this_stage->frequency))
	{
		if (this_stage->lp.next && this_stage->hp.next)
		{
			// Recursively propagate through the tree
			jvx_iir_fb_link_prefilter_set(this_stage->lp.next, bufRet, &retValLocLp, numChannels);
			assert(retValLocLp == this_stage->hp.next->num_prefilters);
			for (i = 0; i < retValLocLp; i++)
			{
				this_stage->hp.next->prefilters[i] = bufRet[i];
				jvx_circbuffer_allocate(&this_stage->hp.next->prefilters[i].B.states, this_stage->hp.next->prefilters[i].B.num_coeffs-1, 1, numChannels);
			}


			jvx_iir_fb_link_prefilter_set(this_stage->hp.next, bufRet, &retValLocHp, numChannels);
			assert(retValLocHp == this_stage->lp.next->num_prefilters);
			for (i = 0; i < retValLocHp; i++)
			{
				this_stage->lp.next->prefilters[i] = bufRet[i];
				jvx_circbuffer_allocate(&this_stage->lp.next->prefilters[i].B.states, this_stage->lp.next->prefilters[i].B.num_coeffs - 1, 1, numChannels);
			}

			// Return for next higher stage level
			cnt = 0;
			bufRet[0].B = this_stage->lp.A;
			cnt++;
			for (i = 0; i < retValLocLp; i++)
			{
				bufRet[cnt + i].B = this_stage->hp.next->prefilters[i].B;
			}
			cnt += retValLocLp;
			for (i = 0; i < retValLocHp; i++)
			{
				bufRet[cnt + i].B = this_stage->lp.next->prefilters[i].B;
			}
			cnt += retValLocLp;

			*retVal = cnt;
		}
		else
		{
			*retVal = 1;
			bufRet[0].B = this_stage->lp.A;
		}
	}
	else
	{
		*retVal = 0;
	}
	return(resL);
}

void jvx_poly(jvxData* poly1, jvxSize polyCnt1, jvxData* poly2, jvxSize polyCnt2)
{
	jvxSize j,k;
	jvxSize idx_lo = 0;
	jvxSize idx_lor = 0;
	jvxSize idx_hi = 0;
	jvxSize idx_hir = 0;
	jvxSize lls1 = 0;
	jvxSize numL = 0;
	idx_lo = polyCnt1;
	idx_hi = polyCnt2;

	lls1 = idx_lo + idx_hi - 1;
	for (j = 0; j < lls1; j++)
	{
		jvxData accu = 0;
		idx_lor = idx_lo - 1;
		idx_hir = idx_hi - 1;

		numL = JVX_MIN(lls1 - j, j + 1);
		numL = JVX_MIN(numL, polyCnt1);
		numL = JVX_MIN(numL, polyCnt2);

		for (k = 0; k < numL; k++)
		{
			accu += poly1[idx_lor] * poly2[idx_hir];
			idx_lor++;
			idx_hir--;
		}
		poly1[lls1 - j - 1] = accu;
		if (idx_lo > 1)
		{
			idx_lo--;
		}
		else
		{
			idx_hi--;
		}
	}
}

jvxDspBaseErrorType 
jvx_iir_fb_design_filter_coeffs(jvxSize fc, jvxSize order, struct jvxIirFbOneFilter* lp_A, struct jvxIirFbOneFilter* hp_A, 
	jvxSize fs, jvxIirFbType tp, jvxDataCplx* tmpBuf1, jvxSize numChannels)
{
	jvxSize i;
	jvxSize order_lp;
	jvxSize order_hp;
	jvxData polyTmp[3];
	jvxData accu;
	jvxSize polyCnt = 0;

	assert(order % 2);

	switch (tp)
	{
	case JVX_MBC_FILTER_DESIGN_BUTTERWORTH:

		jvx_iir_fb_design_butterworth((jvxData)fc, (jvxData)fs, order, tmpBuf1);

		break;
	default:
		assert(0);
	}

	order_lp = JVX_DATA2SIZE(ceil((jvxData)order / 2.0));
	lp_A->num_coeffs = order_lp + 1;
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(lp_A->num_poly, jvxData, lp_A->num_coeffs);
	memset(lp_A->num_poly, 0, sizeof(jvxData) *lp_A->num_coeffs);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(lp_A->den_poly, jvxData, lp_A->num_coeffs);
	memset(lp_A->den_poly, 0, sizeof(jvxData) *lp_A->num_coeffs);

	order_hp = JVX_DATA2SIZE(floor((jvxData)order / 2.0));
	hp_A->num_coeffs = order_hp + 1;
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(hp_A->num_poly, jvxData, hp_A->num_coeffs);
	memset(hp_A->num_poly, 0, sizeof(jvxData) *hp_A->num_coeffs);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(hp_A->den_poly, jvxData, hp_A->num_coeffs);
	memset(hp_A->den_poly, 0, sizeof(jvxData) *hp_A->num_coeffs);

	lp_A->den_poly[0] = 1.0;
	lp_A->num_poly[0] = 1.0;
	hp_A->den_poly[0] = 1.0;
	hp_A->num_poly[0] = 1.0;

	/*
	 Here is what we do:
	 bufo = [];
	 buf1 = [1 2 3 4 5 6 7 8 9 10];
	 buf2 = [5];

	 ll1 = size(buf1,2);
	 ll2 = size(buf2,2);
	 lls1 = ll1 + ll2 - 1;

	 idx_lo = ll1;
	 idx_hi = ll2;

	 for(idxo = lls1:-1:1)
	 bufo(idxo) = 0;
	 num = idx_hi - idx_lo + 1;

	 idx_lor = idx_lo;
	 idx_hir = idx_hi;

	 num = min(idxo, lls1- idxo + 1);
	 num = min(num, ll1);
	 num = min(num, ll2);

	 for(ind =1:num)
	 bufo(idxo) = bufo(idxo)  + (buf1(idx_lor) * buf2(idx_hir));
	 idx_lor = idx_lor + 1;
	 idx_hir = idx_hir - 1;
	 end
	 if(idx_lo > 1)
	 idx_lo = idx_lo - 1;
	 else
	 idx_hi = idx_hi - 1;
	 end
	 end

	 bufo
	 conv(buf1, buf2)
	*/

	// lp filter
	polyCnt = 1;
	for (i = 0; i < order_lp - 1; i += 2)
	{
		polyTmp[0] = 1;
		polyTmp[1] = -2 * tmpBuf1[i].re;
		polyTmp[2] = tmpBuf1[i].re * tmpBuf1[i].re + tmpBuf1[i].im * tmpBuf1[i].im;

		jvx_poly(lp_A->den_poly, polyCnt, polyTmp, 3);
		polyCnt += 2;

	}

	// Since we have odd poles, treat last pole independently
	polyTmp[0] = 1;
	polyTmp[1] = -tmpBuf1[i].re;

	jvx_poly(lp_A->den_poly, polyCnt, polyTmp, 2);
	polyCnt += 1;

	// hp filter
	polyCnt = 1;
	for (i = 1; i < order_lp - 1 ; i += 2)
	{
		polyTmp[0] = 1;
		polyTmp[1] = -2 * tmpBuf1[i].re;
		polyTmp[2] = tmpBuf1[i].re * tmpBuf1[i].re + tmpBuf1[i].im * tmpBuf1[i].im;

		jvx_poly(hp_A->den_poly, polyCnt, polyTmp, 3);
		polyCnt += 2;
	}

	// Find the roots by division
	for (i = 0; i < order_lp; i++)
	{
		jvx_complex_cart_2_polar(tmpBuf1[i].re, tmpBuf1[i].im, &tmpBuf1[i].re, &tmpBuf1[i].im);
		tmpBuf1[i].re = 1.0 / (tmpBuf1[i].re + mat_epsMin);
		jvx_complex_polar_2_cart(tmpBuf1[i].re, tmpBuf1[i].im, &tmpBuf1[i].re, &tmpBuf1[i].im);
#if 0 
		// The following works but is not practical if imag is near zero due to div by 0
		// Complex division: (a3+jb3) /(a2 + jb2))
		//a1 = ((a3*a2) + (b2*b3)) / (a2*a2 + b2*b2);
		//b1 = (a1 * a2 - a3) / b2;
		// a3 = 1, b3 = 0
		// a2 = tmpBuf1[i].re
		// b2 = tmpBuf1[i].im
		accu = ((tmpBuf1[i].re)) / (tmpBuf1[i].re*tmpBuf1[i].re + tmpBuf1[i].im*tmpBuf1[i].im);
		tmpBuf1[i].im = (1 - accu * tmpBuf1[i].re) / tmpBuf1[i].im;
		tmpBuf1[i].re = accu;
#endif
	}

	// lp filter, num
	polyCnt = 1;
	for (i = 0; i < order_lp-1; i += 2)
	{
		polyTmp[0] = 1;
		polyTmp[1] = -2 * tmpBuf1[i].re;
		polyTmp[2] = tmpBuf1[i].re * tmpBuf1[i].re + tmpBuf1[i].im * tmpBuf1[i].im;

		jvx_poly(lp_A->num_poly, polyCnt, polyTmp, 3);
		polyCnt += 2;

	}

	// Since we have odd poles, treat last poles independently
	polyTmp[0] = 1;
	polyTmp[1] = -tmpBuf1[i].re;

	jvx_poly(lp_A->num_poly, polyCnt, polyTmp, 2);
	polyCnt += 1;

	// hp filter
	polyCnt = 1;
	for (i = 1; i < order_lp - 1; i += 2)
	{
		polyTmp[0] = 1;
		polyTmp[1] = -2 * tmpBuf1[i].re;
		polyTmp[2] = tmpBuf1[i].re * tmpBuf1[i].re + tmpBuf1[i].im * tmpBuf1[i].im;

		jvx_poly(hp_A->num_poly, polyCnt, polyTmp, 3);
		polyCnt += 2;
	}

	// Normalize coefficients
	accu = 1.0 / lp_A->num_poly[lp_A->num_coeffs-1];
	for (i = 0; i < lp_A->num_coeffs; i++)
	{
		lp_A->num_poly[i] *= accu;
	}

	// Normalize coefficients
	accu = 1.0 / hp_A->num_poly[hp_A->num_coeffs - 1];
	for (i = 0; i < hp_A->num_coeffs; i++)
	{
		hp_A->num_poly[i] *= accu;
	}

	// Filter states
	jvx_circbuffer_allocate(&lp_A->states, lp_A->num_coeffs - 1, 1, numChannels);
	jvx_circbuffer_allocate(&hp_A->states, hp_A->num_coeffs - 1, 1, numChannels);
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType 
jvx_iir_fb_design_butterworth(jvxData fc, jvxData fs, jvxSize order,  jvxDataCplx* tmpBufP)
{
	jvxSize i;

	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	// Frequency warp
	jvxData omegac = tan(M_PI*fc / fs) / M_PI;
	jvxData tmp = 2 * M_PI*omegac;
	jvxData angle = 0;
	jvxData accu1, accu2, accu3;
	jvxData abs1, abs2;
	jvxData angle1, angle2;

	// Compute analog poles
	for (i = 0; i < order; i++)
	{
		angle = M_PI * (1.0 / 2.0 + (0.5 + (jvxData)i) / (jvxData)order);
		accu1 = tmp * cos(angle);
		accu2 = tmp * sin(angle); // b3; -b2
		accu3 = 2 + accu1; // a3
		accu1 = 2 - accu1; // a2

		jvx_complex_cart_2_polar(accu3, accu2, &abs1, &angle1);
		jvx_complex_cart_2_polar(accu1, -accu2, &abs2, &angle2);

		abs1 = abs1 / (abs2 + mat_epsMin);
		angle1 -= angle2;

		jvx_complex_polar_2_cart(abs1, angle1, &tmpBufP[i].re, &tmpBufP[i].im);
#if 0  // Following not practical due to 0/0 problems
		// Complex division: (a3+jb3) /(a2 + jb2))
		//a1 = ((a3*a2) + (b2*b3)) / (a2*a2 + b2*b2);
		//b1 = (a1 * a2 - a3) / b2;
		accu4 = ((accu3*accu1) - (accu2*accu2)) / (accu1*accu1 + accu2*accu2);
		tmpBufP[i].im = (accu3 - accu4 * accu1) / accu2; // accu2 must be 
		tmpBufP[i].re = accu4;
#endif
	}

	return resL;
}

#if 0




	for(i = 0; i < )

	if (fType == 1)

		% Butterworth filter design
		[b_rootszc, b_rootszs, b_poleszc, b_poleszs, H0] = butterworth_lp(order, fc, fs);
	poles_all = [b_poleszc b_poleszs fliplr(conj(b_poleszc))];
	poles_A0 = poles_all(1:2 : end);
	poles_A1 = poles_all(2:2 : end);

	% From the poles, create the polynomial in z
		A0.den = real(poly(poles_A0));
	A1.den = real(poly(poles_A1));

	% For the allpass, create the zeros
		roots_A0 = conj(1. / poles_A0);
	roots_A1 = conj(1. / poles_A1);

	% Find the numerator term
		A0.num = real(poly(roots_A0));
	A1.num = real(poly(roots_A1));

	% Normalize such that we have a zero dB pass area
		A0.num = A0.num . / A0.num(end);
	%A1.num = [A1.num . / A1.num(end) 0];
	A1.num = A1.num . / A1.num(end);
	return(JVX_DSP_NO_ERROR);
}



function[b_rootszc, b_rootszs, b_poleszc, b_poleszs, H0] = butterworth_lp(order, fc, fs)

% Frequency warp
omegac = tan(pi*fc / fs) / pi;
num_gen_conj = floor(order / 2);
num_gen_single = ceil(order / 2) - num_gen_conj;

% Poles
% b_poles = 2 * pi*omegac*exp(j*pi * (([0:order - 1] + 0.5) / order + 1 / 2));
% Design buterworth-- if (strcmp(design, 'butter'))
pole_abs = ones(1, order);
pole_angles = pi*(1 / 2 + (0.5 + [0:order - 1]) / order);
% end

% Design poles in s - plane
b_poles = pole_abs.*exp(j*pole_angles);

% Do some adaptation
b_poles = 2 * pi*omegac* b_poles;
b_roots = ones(1, order) * 1e12;

% Convert into z - domain
b_polesz = (2 + b_poles). / (2 - b_poles);
den = real(poly(b_polesz));

% Select only the poles from upper
b_poleszc = b_polesz(1:num_gen_conj);
b_poleszs = b_polesz(num_gen_conj + 1:num_gen_conj + num_gen_single);

% Zeros
%b_rootsz = -ones(1, order);
b_rootsz = (2 + b_roots) . / (2 - b_roots);
num = real(poly(b_rootsz));

b_rootszc = b_rootsz(1:num_gen_conj);
b_rootszs = b_rootsz(num_gen_conj + 1:num_gen_conj + num_gen_single);

% Normalization
H0 = sum(den) / sum(num);
end

#endif

jvxDspBaseErrorType jvx_iir_fb_show_filter(struct jvx_iir_fb_one_stage* hdl, jvxSize lev)
{
	jvxSize i;
	//char strBuf[128];
	//memset(strBuf, 0, 128);
    char* strBuf = NULL;
    JVX_DSP_SAFE_ALLOCATE_FIELD_Z(strBuf, char, 128);
    
	for (i = 0; i < lev; i++)
	{
		strBuf[i] = '\t';
	}
	strBuf[i] = '|';

	if (hdl->lp.next)
	{
		jvx_iir_fb_show_filter(hdl->lp.next, lev + 1);
	}
	// == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =

	if (JVX_CHECK_SIZE_SELECTED(hdl->frequency))
	{
		jvxprintf("%s-obuf<%i>", (const char*)strBuf, (int)hdl->lp.oBufId);
		jvxprintf("%s-Filter LP", (const char*)strBuf);
		jvxprintf("%s# <num> ## <den> #", (const char*)strBuf);
		for (i = 0; i < hdl->lp.A.num_coeffs; i++)
		{
			jvxprintf("%s# %f ## %f #", (const char*)strBuf, hdl->lp.A.num_poly[i], hdl->lp.A.den_poly[i]);
		}
		jvxprintf("%s-Filter HP<%i>", (const char*)strBuf, (int)hdl->lp.oBufId);
		for (i = 0; i < hdl->hp.A.num_coeffs; i++)
		{
			jvxprintf("%s# %f ## %f #", (const char*)strBuf, hdl->hp.A.num_poly[i], hdl->hp.A.den_poly[i]);
		}
		jvxprintf("%s-ibuf<%i>-f<%i>-o<%i>", (const char*)strBuf, (int)hdl->iBufId, (int)hdl->frequency, (int)hdl->order);
		jvxprintf("%s-obuf<%i>", (const char*)strBuf, (int)hdl->hp.oBufId);

		//if (stages.frequency >= 0)
		//disp([tokenTab '-------->oBufId(lp)=' num2str(stages.lp.oBufId)]);
		//disp([tokenTab 'F=' num2str(stages.frequency) ',O=' num2str(stages.order) ...
		//',iBufId=' num2str(stages.iBufId) ',PFs=' show_prefilters(stages.prefilter) ...
		//',A(lp):[' sprintf('%f ', stages.lp.A.num) '] -- [' sprintf('%f ', stages.lp.A.den) ']'...
		//',A(hp):[' sprintf('%f ', stages.hp.A.num) '] -- [' sprintf('%f ', stages.hp.A.den) ']' ...
		//]);
		//disp([tokenTab '-------->oBufId(hp)=' num2str(stages.hp.oBufId)]);
	}
	else
	{
		jvxprintf("%s-obuf<%i>", (const char*)strBuf, (int)hdl->lp.oBufId);
	}

	// == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =

	if (hdl->lp.next)
	{
		jvx_iir_fb_show_filter(hdl->lp.next, lev + 1);
	}
    
    JVX_DSP_SAFE_DELETE_FIELD(strBuf);
	return JVX_DSP_NO_ERROR;

}

//Beispielaufruf dieser Funktion. Hinweis: Matlab-generierte .h-Datei muss eingebunden werden.
//In der .h-Datei sind alle notwendigen Parameter für den Aufruf statisch deklariert.
//jvx_iir_fb_prepare_direct(filterbank_hdls, JVX_FIXED_IIR_FB_SPLITFREQUENCIES, JVX_FIXED_IIR_FB_ORDERS, JVX_FIXED_IIR_FB_NUMSPLITFREQUENCIES, JVX_FIXED_IIR_FB_NUMCHANNELS, JVX_FIXED_IIR_FB_FS, JVX_MBC_FILTER_DESIGN_BUTTERWORTH, true, JVX_FIXED_IIR_FB_COEFFSNUM, JVX_FIXED_IIR_FB_COEFFSDEN, JVX_FIXED_IIR_FB_ARRAYCOEFFNUMBERS, filterbank_memory, JVX_FIXED_IIR_FB_NUMBERFILTERS);
jvxDspBaseErrorType jvx_iir_fb_prepare_direct(jvx_iir_fb* handleOnReturn, jvxSize* splitFrequencies, jvxSize* orders, jvxSize numSplitFrequencies, jvxSize numChannels, jvxSize fs, jvxIirFbType fbType, jvxCBool verbose, jvxData* coeffs_num, jvxData* coeffs_den, jvxSize* array_coeff_numbers, jvxData* state_buffers, jvxSize num_filters)
{
	jvxSize i;
	jvxSize levels = 0;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize orderMax = 0;
	jvxDataCplx* tmpDesignBufP = NULL;
	jvxSize filter_counter;
	jvxSize coeff_counter;
	jvxData* state_buffers_copy;

	if (handleOnReturn->prv == NULL)
	{
		jvx_iir_fb_private* hdl = NULL;

		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(hdl, jvx_iir_fb_private);
		hdl->init_copy.splitFrequencies = splitFrequencies;
		hdl->init_copy.orders = orders;
		hdl->init_copy.numSplitFrequencies = numSplitFrequencies;
		hdl->init_copy.numChannels = numChannels;
		hdl->init_copy.fs = fs;
		hdl->init_copy.fbType = fbType;
		hdl->init_copy.verbose = verbose;

		// Make some corrections: Only odd orders
		for (i = 0; i < hdl->init_copy.numSplitFrequencies; i++)
		{
			if ((hdl->init_copy.orders[i] % 2) == 0)
				hdl->init_copy.orders[i]++;
			if (hdl->init_copy.orders[i] > orderMax)
			{
				orderMax = hdl->init_copy.orders[i];
			}
		}

		if (hdl->init_copy.verbose)
		{
			jvxprintf("jvx_iir_fb<C>: =====================================");
			jvxprintf("jvx_iir_fb<C>: ===== FB Design ================");
			jvxprintf("jvx_iir_fb<C>: =====================================");
		}

		// Compute tree depth
		levels = JVX_DATA2SIZE(ceil(log((jvxData)hdl->init_copy.numSplitFrequencies + 1.0) / log(2.0)));

		// Initialize tree
		jvx_iir_fb_init_one_stage(&hdl->entryTree);

		if (hdl->init_copy.verbose)
		{
			jvxprintf("jvx_iir_fb<C>: Deriving filterbank tree");
		}

		// Recursive tree design
		resL = jvx_iir_fb_construct_tree(hdl->init_copy.splitFrequencies, hdl->init_copy.orders, hdl->init_copy.numSplitFrequencies,
			levels, 1, &hdl->entryTree, 0, (jvxData)hdl->init_copy.fs / 2.0, 0, hdl->init_copy.fs, hdl->init_copy.fbType,
			&handleOnReturn->prm_derived.numSubNodes);

		if (hdl->init_copy.verbose)
		{
			jvxprintf("jvx_iir_fb<C>: Designing stage trees in C\n");
		}

		//Copy filter coefficients into the tree
		filter_counter = 0;
		coeff_counter = 0;
		state_buffers_copy = state_buffers;
		resL = jvx_iir_fb_copy_filter_coeffs(&hdl->entryTree, coeffs_num, coeffs_den, &state_buffers_copy, array_coeff_numbers, num_filters, &filter_counter, &coeff_counter, numChannels);
		assert(filter_counter == num_filters);

		// Recursive prefilter linkage
		resL = jvx_iir_fb_link_prefilter(&hdl->entryTree, hdl->init_copy.numChannels);

		// Output recursive filter design
		// Achtung! Kann je nach Plattform zu Stack Overflow führen.
		if (hdl->init_copy.verbose)
		{
			//resL = jvx_iir_fb_show_filter(&hdl->entryTree, 0);
		}
		handleOnReturn->prv = hdl;
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_iir_fb_postprocess_direct(jvx_iir_fb* handleOnReturn)
{
	if (handleOnReturn->prv)
	{
		jvx_iir_fb_private* hdl = (jvx_iir_fb_private*)handleOnReturn->prv;
		jvx_iir_fb_link_prefilter_deallocate(&hdl->entryTree);
		jvx_iir_fb_destroy_tree(&hdl->entryTree);
		JVX_DSP_SAFE_DELETE_OBJECT(hdl);
		handleOnReturn->prv = NULL;
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_iir_fb_copy_filter_coeffs(struct jvx_iir_fb_one_stage* tree, jvxData* coeffs_num, jvxData* coeffs_den, jvxData** memory, jvxSize* array_coeff_numbers, jvxSize num_filters, jvxSize* filter_counter, jvxSize* coeff_counter, jvxSize numChannels)
{
	jvxSize i;

	assert(*filter_counter < num_filters -1);
	// copy filter coeffs to root (lowpass)
	tree->lp.A.num_coeffs = array_coeff_numbers[*filter_counter];
	tree->lp.A.num_poly = &coeffs_num[*coeff_counter];
	tree->lp.A.den_poly = &coeffs_den[*coeff_counter];
	*coeff_counter = (*coeff_counter) + array_coeff_numbers[*filter_counter];
	*filter_counter = (*filter_counter) +1;

	// copy filter coeffs to root (highpass)
	tree->hp.A.num_coeffs = array_coeff_numbers[*filter_counter];
	tree->hp.A.num_poly = &coeffs_num[*coeff_counter];
	tree->hp.A.den_poly = &coeffs_den[*coeff_counter];
	*coeff_counter = (*coeff_counter) + array_coeff_numbers[*filter_counter];
	*filter_counter = (*filter_counter) +1;

	//Use given memory for states (lowpass)
	jvxData** state_buffer;
	state_buffer = (jvxData**)*memory;
	*memory = (jvxData*)(state_buffer + numChannels);
	for (i = 0; i < numChannels; i++)
	{
		state_buffer[i] = *memory;
		*memory = state_buffer[i] + (tree->lp.A.num_coeffs - 1);
	}
	jvx_circbuffer_allocate_extbuf(&tree->lp.A.states, tree->lp.A.num_coeffs - 1, 1, numChannels, state_buffer);

	//Use given memory for states (highpass)
	state_buffer = (jvxData**)*memory;
	*memory = (jvxData*)(state_buffer + numChannels);
	for (i = 0; i < numChannels; i++)
	{
		state_buffer[i] = *memory;
		*memory = state_buffer[i] + (tree->hp.A.num_coeffs - 1);
	}
	jvx_circbuffer_allocate_extbuf(&tree->hp.A.states, tree->hp.A.num_coeffs - 1, 1, numChannels, state_buffer);

	// copy children
	if (tree->lp.next)
		jvx_iir_fb_copy_filter_coeffs(tree->lp.next, coeffs_num, coeffs_den, memory, array_coeff_numbers, num_filters, filter_counter, coeff_counter, numChannels);
	if (tree->hp.next)
		jvx_iir_fb_copy_filter_coeffs(tree->hp.next, coeffs_num, coeffs_den, memory, array_coeff_numbers, num_filters, filter_counter, coeff_counter, numChannels);
	return JVX_DSP_NO_ERROR;
}
