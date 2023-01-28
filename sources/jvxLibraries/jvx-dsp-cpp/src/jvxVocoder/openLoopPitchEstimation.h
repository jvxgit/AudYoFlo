/* ============================================================	*
 * LABORATORY COURSES FOR OBJECT ORIENTED PROGRAMMING,			*
 * INSTITUTE OF COMMUNICATION SYSTEMS AND DATA PROCESSING,		*
 * IND, RWTH-AACHEN												*/

/* DESCRIPTION:													*
 *	Class for an openloop pitch estimation. This class receives *
 *  frames of input speech and finds the best matching pitch 	*
 * 	in a specific range of possible pitch-related delays.		*
 *  The passed samples for the pitch analysis are stored 		*
 * 	internally. Pitch doubling can be avoided by the module		*
 *																*/

/* CLASS/FUNCTION:												*
 *	openLoopPitchEstimation										*
 *============================================================	*/

#ifndef OPENLOOPPITCHESTIMATION_H__
#define OPENLOOPPITCHESTIMATION_H__

#include "jvxVocoder/vocoder.h"

bool shift(jvxData *dest, int shiftCoeff,int size);

bool memcopy(const jvxData *src, jvxData *dest, 
	 unsigned int startSrc, unsigned int startDest, unsigned int length);

/*!	Class for an openloop pitch estimation. This class receives *
 *  frames of input speech and finds the best matching pitch 	*
 * 	in a specific range of possible pitch-related delays.		*
 *  The passed samples for the pitch analysis are stored 		*
 * 	internally. Pitch doubling can be avoided by the module		*/
class openLoopPitchEstimation
{
	/*! Boundaries for pitch delay: Maximum pitch-delay */
	int maxTimeIntervall;

	/*! Boundaries for pitch delay: Minimum pitch-delay */
	int minTimeIntervall;
	
	/*! The pitch estimation is based on samples from frames 	*
	 *	having been procesed in the past. The buffer size is in *
	 * 	accordance to the max LTP delay.*/
	jvxData *buffer;
	
	/*! Max value for framelength for sample buffers*/
	int frameLengthMax;

	/*! Weighting coefficients to avoid pitch doubling */ 
	jvxData weightPitchDoubling;

    int bufferLength;

	/*! Delta coefficient to search pitch period at the end of algorithm*/
	int deltaPitchDoubling;
	
public:

	/*! Constructor: allocate buffer for internal samples (from the past)
	 *  and copy specification of parameters */
	openLoopPitchEstimation(int maxInter, int minInter,
							int flength_max,
							jvxData weight, int delta);
							
	~openLoopPitchEstimation();

	//! Pass another frame to the class to update internal buffers 
	bool setInput(const jvxData *subframe);

	/*! Detemine the openloop pitch delay based on the passed input *
	 *	and the samples from the past, also return criterion for	*
	 * voiced/unvoiced. */
	int determineOpenLoopPitch(jvxData& excitationCrit);

};
	
#endif
