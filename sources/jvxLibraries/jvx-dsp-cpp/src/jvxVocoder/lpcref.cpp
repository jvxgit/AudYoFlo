

#include "jvxVocoder/lpcref.h"

void ref2LpcRecursion(jvxData* a_n, const jvxData* refCoeffs, 
					  int maxAmount, int current);

void lpc2RefRecursion(jvxData* lpcCoeffs, jvxData* refCoeffs, int current);

/* ========================================================================= */
/* ============================== REF2LPC REF2LPC =========================== */

/*!
 * Functions to convert reflection coefficients into LPC parameters.
 * Be aware that the amount of LPC-parameters is one more than the amount of
 * reflection coeffs.
 * Furthermore keep in mind that the LPCs are stored as specified in the transfer-
 * function:
 *  1 +a1 z^(-1) + a2 z^(-2) ... =>    [1 a1 a2 ...]
 *==========================================================================*/
void 
ref2Lpc(const jvxData* refCoeffs, int amountRefCoeffs,
		jvxData* lpcCoeffs)
{
	int i = 0;
	int amountLPCCoeffs = amountRefCoeffs +1;

	/* Set the initial LPCs to 0 */	
	for(i = 1; i< amountLPCCoeffs;i++)
		lpcCoeffs[i] = 0;
	
	/* Start value for a(0) is 1 */
	lpcCoeffs[0] = 1;
	
	/* Call recursive function */
	ref2LpcRecursion(lpcCoeffs, refCoeffs, amountRefCoeffs, 1);

}

/*! Actual recursive algorithm: Described in GeorgiaTech kinkos Professors
 * publishing,EE6414 Advanced DSP by M.Hayes.
 * After initialization, the LPC-coefficients are updated and thus modified 
 * and finally include the values for the matching LPC-coeffs
 *======================================================================*/
void 
ref2LpcRecursion(jvxData* a_n, const jvxData* refCoeffs, int maxAmount, int current)
{
	int i = 0;
	jvxData* a_n_copy = NULL;

	a_n_copy = (jvxData*) malloc(sizeof(jvxData)*current);
	memcpy(a_n_copy, a_n, current*sizeof(jvxData));
	for(i = 0;i<current; i++)
	{
		a_n[i+1] = a_n[i+1] + refCoeffs[current-1]*a_n_copy[current-1-i];
	}
	if(current != maxAmount)
		ref2LpcRecursion(a_n, refCoeffs, maxAmount, current+1);
	free(a_n_copy);
}

/*========================================================================== */
/*============================== LPC2REF LPC2REF =========================== */

/*!
 * Functions to convert LP coefficients into reflection parameters.
 * Be aware that the amount of LPC-parameters is one more than the amount of
 * reflection coeffs.
 * Furthermore keep in mind that the LPCs are stored as specified in the transfer-
 * function:
 *  1 +a1 z^(-1) + a2 z^(-2) ... =>    [1 a1 a2 ...]
 *==========================================================================*/
void 
lpc2Ref(const jvxData* lpcCoeffs, int amountLPCCoeffs, jvxData* refCoeffs)
{
	/* Copy LPCs in order to modify the values (lPc is const!)*/
	jvxData* lpcCoeffsCopy = NULL;
	lpcCoeffsCopy = (jvxData*) malloc(sizeof(jvxData)*amountLPCCoeffs);
	memcpy(lpcCoeffsCopy, lpcCoeffs, sizeof(jvxData)*amountLPCCoeffs);

	/* Call actual recursive algorithm for evaluation */
	lpc2RefRecursion(lpcCoeffsCopy, refCoeffs, amountLPCCoeffs-1);

	free(lpcCoeffsCopy);
}

/*!
 * Actual recursive algorithm described in the same literature as the
 * inverse function 
 *======================================================================*/
void
lpc2RefRecursion(jvxData* lpcCoeffs, jvxData* refCoeffs, int current)
{
	int i = 0;

	/* Copy the LPC values as the values are used that exist at the BEGINNING of
	   this iteration / recursion */
	jvxData* lpcCoeffsCopy = NULL;

	/* Determine current reflection coeff */
	jvxData r_current;
	jvxData fac_r;

	lpcCoeffsCopy = (jvxData*) malloc(sizeof(jvxData)*(current+1));


	memcpy(lpcCoeffsCopy, lpcCoeffs, sizeof(jvxData)*(current+1));

	r_current = lpcCoeffs[current];
	fac_r = 1/(1.0 - r_current * r_current);

	/* Store new reflection coeff */
	refCoeffs[current-1] = r_current;

	/* Determine values for the upcoming recursion-iteration*/
	for(i=1; i< current;i++)
	{
		lpcCoeffs[i] = fac_r * (lpcCoeffsCopy[i] - r_current * lpcCoeffsCopy[current-i]);
	}
	
	if(current != 1)
		lpc2RefRecursion(lpcCoeffs, refCoeffs, current-1);
	free(lpcCoeffsCopy);
}
	




