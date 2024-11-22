#ifndef __JVX_CIRCBUFFER_H__
#define __JVX_CIRCBUFFER_H__

// Need to define the internally used data for inline functions
#include "jvx_system.h"
#include "jvx_dsp_base.h"

JVX_DSP_LIB_BEGIN

/**
 * Circular buffer structure used in circular buffer realizations
 *///==============================================================
typedef struct
{
	jvxSize length;
	jvxSize channels;
	jvxSize idxRead;
	jvxSize fHeight;
	jvxSize nUnits; // for SOS IIR filters
	jvxDataFormat format;
	size_t szElement;
	struct
	{
		jvxData** field;
	} ram;
} jvx_circbuffer;

// ===============================================================================
// Multi channel circbuffers
// ===============================================================================

jvxDspBaseErrorType jvx_circbuffer_allocate(jvx_circbuffer** hdlOnReturn,
					    jvxSize numberElements,
					    jvxSize nSections,
					    jvxSize channels);

jvxDspBaseErrorType
jvx_circbuffer_allocate_extbuf(jvx_circbuffer** hdlOnReturn,
	jvxSize numberElements,
	jvxSize nSections,
	jvxSize channels,
	jvxData** extBuffer);

jvxDspBaseErrorType jvx_circbuffer_deallocate(jvx_circbuffer* hdlReturn);

jvxDspBaseErrorType jvx_circbuffer_deallocate_extbuf(jvx_circbuffer* hdlReturn);

jvxDspBaseErrorType jvx_circbuffer_reset(jvx_circbuffer* hdl);

jvxDspBaseErrorType jvx_circbuffer_access(jvx_circbuffer* hdl, jvxData** outPtr, jvxSize* outLen, jvxSize idx);

jvxDspBaseErrorType jvx_circbuffer_fill(jvx_circbuffer* hdl,
					jvxData toFillWith,
					jvxSize numberValuesFill); 

jvxDspBaseErrorType jvx_circbuffer_remove(jvx_circbuffer* hdl,
	jvxSize numberValuesRemove); 

jvxDspBaseErrorType jvx_circbuffer_write_update(jvx_circbuffer* hdl,
	const jvxData** fieldFill,
	jvxSize numberValuesFill);

jvxDspBaseErrorType jvx_circbuffer_write_update_wrap(jvx_circbuffer* hdl,
	const jvxData** fieldFill,
	jvxSize numberValuesFill);

jvxDspBaseErrorType jvx_circbuffer_read_update(jvx_circbuffer* hdl,
					       jvxData** fieldRead,
	jvxSize numberValuesRead);

jvxDspBaseErrorType
jvx_circbuffer_read_update_offset(jvx_circbuffer* hdl,
	jvxData** fieldRead,
	jvxSize numberValuesRead,
	jvxSize offset);

jvxDspBaseErrorType jvx_circbuffer_read_update_conv(jvx_circbuffer* hdl,
	jvxHandle** fieldRead, jvxDataFormat form, jvxData scaleBeforeOut,
	jvxSize numberValuesRead);

jvxDspBaseErrorType jvx_circbuffer_copy_update_buf2buf(jvx_circbuffer* hdl_copyTo,
						       jvx_circbuffer* hdl_copyFrom,
	jvxSize numberValuesFill); 

jvxDspBaseErrorType jvx_circbuffer_write_update_ignore(jvx_circbuffer* hdl,
						       jvxData** fieldFill,
	jvxSize numberValuesFill);

jvxDspBaseErrorType jvx_circbuffer_read_update_ignore(jvx_circbuffer* hdl,
						      jvxData** fieldRead,
	jvxSize numberValuesRead);

jvxDspBaseErrorType jvx_circbuffer_read_noupdate(jvx_circbuffer* hdl,
						 jvxData** fieldRead,
	jvxSize numberValuesRead);

// Advance read index by numberValuesRead elements, as if they'd been read.
jvxDspBaseErrorType jvx_circbuffer_advance_read_index(jvx_circbuffer* hdl,
	jvxSize numberValuesRead);

jvxDspBaseErrorType
jvx_circbuffer_get_write_phase(jvx_circbuffer* hdl, jvxSize* phase);

jvxDspBaseErrorType jvx_circbuffer_fir_1can_1io(jvx_circbuffer* hdlIn,
						jvxData* fCoeffs_fw,
						jvxData** fieldInOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_fir_2can_1io(jvx_circbuffer* hdlIn,
						jvxData* fCoeffs_fw,
						jvxData** fieldInOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_iir_1can_1io(jvx_circbuffer* hdlIn,
						jvxData* fCoeffs_fw,
						jvxData* fCoeffs_bw,
						jvxData** fieldInOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_iir_2can_1io(jvx_circbuffer* hdlIn,
						jvxData* fCoeffs_fw,
						jvxData* fCoeffs_bw,
						jvxData** fieldInOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_iir_1can_1io(jvx_circbuffer* hdlIn,
						jvxData* fCoeffs_fw,
						jvxData* fCoeffs_bw,
						jvxData** fieldInOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_iir_sos1can_1io(jvx_circbuffer* hdlIn,
						   jvxData* fCoeffs_fw,
						   jvxData* fCoeffs_bw,
						   jvxData* gains,
						   jvxInt16 shiftFactor,
						   jvxData** fieldInOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_iir_sos2can_1io(jvx_circbuffer* hdlIn,
						   jvxData* fCoeffs_fw,
						   jvxData* fCoeffs_bw,
						   jvxData* gains,
						   jvxInt16 shiftFactor,
						   jvxData** fieldInOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_fir_1can_2io(jvx_circbuffer* hdl,
						jvxData* fCoeffs_fw,
						jvxData** fieldIn,
						jvxData** fieldOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_fir_2can_2io(jvx_circbuffer* hdlIn,
						jvxData* fCoeffs_fw,
						jvxData** fieldIn,
						jvxData** fieldOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_iir_1can_2io(jvx_circbuffer* hdlIn,
						jvxData* fCoeffs_fw,
						jvxData* fCoeffs_bw,
						jvxData** fieldIn,
						jvxData** fieldOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_iir_2can_2io(jvx_circbuffer* hdlIn,
						jvxData* fCoeffs_fw,
						jvxData* fCoeffs_bw,
						jvxData** fieldIn,
						jvxData** fieldOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_iir_sos1can_2io(jvx_circbuffer* hdlIn,
						   jvxData* fCoeffs_fw,
						   jvxData* fCoeffs_bw,
						   jvxData* gains,
						   jvxInt16 shiftFactor,
						   jvxData** fieldIn,
						   jvxData** fieldOut,
	jvxSize bSize);

jvxDspBaseErrorType jvx_circbuffer_iir_sos2can_2io(jvx_circbuffer* hdlIn,
						   jvxData* fCoeffs_fw,
						   jvxData* fCoeffs_bw,
						   jvxData* gains,
						   jvxInt16 shiftFactor,
						   jvxData** fieldIn,
						   jvxData** fieldOut,
	jvxSize bSize);

// ===============================================================================
// Single channel circbuffers
// ===============================================================================

jvxDspBaseErrorType jvx_circbuffer_allocate_1chan(jvx_circbuffer** hdlOnReturn,
	jvxSize numberElements);

jvxDspBaseErrorType jvx_circbuffer_write_update_1chan(jvx_circbuffer* hdl,
						     const  jvxData* fieldFill,
	jvxSize numberValuesFill);

jvxDspBaseErrorType jvx_circbuffer_read_update_1chan(jvx_circbuffer* hdl,
						     jvxData* fieldRead,
	jvxSize numberValuesRead);

jvxDspBaseErrorType jvx_circbuffer_read_update_1chan_conv(jvx_circbuffer* hdl,
	jvxHandle* fieldRead, jvxDataFormat form, jvxData scaleBeforeOut,
	jvxSize numberValuesRead);

jvxDspBaseErrorType jvx_circbuffer_read_noupdate_1chan(jvx_circbuffer* hdl,
						       jvxData* fieldRead,
	jvxSize numberValuesRead);

jvxDspBaseErrorType jvx_circbuffer_write_convolve_update_1chan(jvx_circbuffer* hdl,
							       jvxData input,
							       jvxData* coeffs,
							       jvxSize lengthCoeffBuffer,
							       jvxData* output);

jvxDspBaseErrorType jvx_circbuffer_write_convolve_no_update_1chan(jvx_circbuffer* hdl,
								  jvxData input,
								  jvxData* coeffs,
								  jvxSize lengthCoeffBuffer,
								  jvxData* output);

jvxDspBaseErrorType jvx_circbuffer_write_convolve_only_update_1chan(jvx_circbuffer* hdl);

jvxDspBaseErrorType jvx_circbuffer_write_norm_update_1chan(jvx_circbuffer* hdl,
							   jvxData input,
							   jvxData* output);

jvxDspBaseErrorType jvx_circbuffer_write_norm_no_update_1chan(jvx_circbuffer* hdl,
							      jvxData input,
							      jvxData* output);

jvxDspBaseErrorType jvx_circbuffer_write_norm_only_update_1chan(jvx_circbuffer* hdl);

jvxDspBaseErrorType jvx_circbuffer_fld_mult_fac_add_circbuffer_fac(jvxData* fldInOut,
								   jvxSize sz,
								   jvxData fac1,
								   jvxData fac2,
								   jvx_circbuffer* hdl);

jvxDspBaseErrorType jvx_circbuffer_fld_mult_fac_add_circbuffer_fac_ret_energy(jvxData* fldInOut,
									      jvxSize sz,
									      jvxData fac1,
									      jvxData fac2,
									      jvx_circbuffer* hdl,
									      jvxData* energyFilter);

/** Compute cross correlation function based on recursive smoothing */
jvxDspBaseErrorType
jvx_circbuffer_ccorr_1chan(jvx_circbuffer* hdl1,
			   jvx_circbuffer* hdl2,
			   jvxData* fieldUpdate,
			   jvxData* fieldRead1,
			   jvxData* fieldRead2,
	jvxSize numValues,
			   jvxData smooth);

jvxDspBaseErrorType jvx_circbuffer_filter_cascade_1can(jvx_circbuffer* hdl, jvxData* a, jvxData *b, jvxData** inout, jvxSize nSections, jvxSize order, jvxSize buffersize);
jvxDspBaseErrorType jvx_circbuffer_filter_cascade_1can_inv(jvx_circbuffer* hdl, jvxData* a, jvxData *b, jvxData** inout, jvxSize nSections, jvxSize order, jvxSize buffersize);

jvxDspBaseErrorType jvx_circbuffer_filter_cascade_2can_cb(jvx_circbuffer* hdl, jvxData* a, jvxData *b, jvxData** inout, jvxSize nSections, jvxSize order, jvxSize buffersize);
jvxDspBaseErrorType jvx_circbuffer_filter_cascade_2can_cb_inv(jvx_circbuffer* hdl, jvxData* a, jvxData *b, jvxData** inout, jvxSize nSections, jvxSize order, jvxSize buffersize);

jvxDspBaseErrorType jvx_circbuffer_energy(jvx_circbuffer* hdl, jvxData* energyOut, jvxCBool update);

JVX_DSP_LIB_END

#endif
