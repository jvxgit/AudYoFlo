#include <string.h>
#include "jvx_dsp_base.h"

//#define DEBUG_BUFFER

typedef struct
{
	jvxData minValue;
	jvxData maxValue;
	jvxData* minWeight_operation;
	jvxData* maxWeight_operation;
	jvxCBool valid;
} jvx_estimator_fillheight_one_section_struct;

typedef struct
{
	jvxData value;
	jvxData weight;
	jvxSize operate_id;
} jvx_estimator_fillheight_one_entry;

typedef struct
{
	struct
	{
		jvxSize numberEventsConsidered_perMinMaxSection;
		jvxSize num_MinMaxSections;
		jvxData recSmoothFactor;
		jvxInt32 numOperations;
	} config;

	struct
	{
		struct
		{
			jvxSize idxWrite;
			jvx_estimator_fillheight_one_entry* circBuffer;
			jvxSize bLength;
			jvxSize fHeight;
			jvxCBool fullyStarted;
		} circularBuffer;

		struct
		{
			jvx_estimator_fillheight_one_section_struct* sections;
			jvxSize numSections;
		} minMaxSections;

		struct
		{
			jvxData currentAverage_nom;
			jvxData currentAverage_den;
			jvxData* acc_weights_operations;
			jvxInt32* count_operations;
		} measurements;

		struct
		{
			jvxData smoothed_avrg;
			jvxCBool smoothingActive;
		} smoothing;

	} runtime;

	struct
	{
		jvxTimeStampData ref;
		jvxUInt64 start;
	} intime;

} jvx_estimator_fillheight_private_struct;

jvxDspBaseErrorType
jvx_estimate_buffer_fillheight_init(jvxHandle** hdlOnReturn, jvxInt32 numberEventsConsidered_perMinMaxSection,
                                    jvxInt32 num_MinMaxSections, jvxData recSmoothFactor,
                                    jvxInt32 numOperations)
{
	jvxSize i,j;
	if(hdlOnReturn)
	{
		jvx_estimator_fillheight_private_struct* newField = 0;

		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newField, jvx_estimator_fillheight_private_struct);
		memset(newField, 0, sizeof(jvx_estimator_fillheight_private_struct));

		// Store config data
		newField->config.numberEventsConsidered_perMinMaxSection = numberEventsConsidered_perMinMaxSection;
		newField->config.num_MinMaxSections = num_MinMaxSections;
		newField->config.recSmoothFactor = recSmoothFactor;
		newField->config.numOperations = numOperations;

		// Allocate all fields for circular buffer
		newField->runtime.circularBuffer.bLength = newField->config.numberEventsConsidered_perMinMaxSection * newField->config.num_MinMaxSections;
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newField->runtime.circularBuffer.circBuffer, jvx_estimator_fillheight_one_entry, newField->runtime.circularBuffer.bLength);
		for(i = 0; i < newField->runtime.circularBuffer.bLength; i++)
		{
			newField->runtime.circularBuffer.circBuffer[i].weight = 0.0;
			newField->runtime.circularBuffer.circBuffer[i].value = 0.0;
			newField->runtime.circularBuffer.circBuffer[i].operate_id = 0;
		}
		newField->runtime.circularBuffer.fHeight = 0;
		newField->runtime.circularBuffer.fullyStarted = false;
		newField->runtime.circularBuffer.idxWrite = 0;

		newField->runtime.measurements.currentAverage_den = 0;
		newField->runtime.measurements.currentAverage_nom = 0;

		newField->runtime.measurements.acc_weights_operations = NULL;
		newField->runtime.measurements.count_operations = NULL;
		if(newField->config.numOperations)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newField->runtime.measurements.acc_weights_operations, jvxData,
				newField->config.numOperations);
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newField->runtime.measurements.count_operations, jvxInt32,
				newField->config.numOperations);
			for(i = 0; i < newField->config.numOperations; i++)
			{
				newField->runtime.measurements.acc_weights_operations[i] = 0;
				newField->runtime.measurements.count_operations[i] = 0;
			}
		}

		newField->runtime.minMaxSections.numSections = newField->config.num_MinMaxSections;
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newField->runtime.minMaxSections.sections, jvx_estimator_fillheight_one_section_struct, newField->runtime.minMaxSections.numSections);
		for(i = 0; i < newField->runtime.minMaxSections.numSections; i++)
		{
			newField->runtime.minMaxSections.sections[i].maxValue = JVX_DATA_MAX_POS;
			newField->runtime.minMaxSections.sections[i].minValue = JVX_DATA_MAX_NEG;
			newField->runtime.minMaxSections.sections[i].minWeight_operation = NULL;
			newField->runtime.minMaxSections.sections[i].maxWeight_operation = NULL;
			if(newField->config.numOperations)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newField->runtime.minMaxSections.sections[i].minWeight_operation, jvxData, newField->config.numOperations);
				JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newField->runtime.minMaxSections.sections[i].maxWeight_operation, jvxData, newField->config.numOperations);
				for(j = 0; j < newField->config.numOperations; j++)
				{
					newField->runtime.minMaxSections.sections[i].minWeight_operation[j] = -1;
					newField->runtime.minMaxSections.sections[i].maxWeight_operation[j] = -1;
				}
			}
			newField->runtime.minMaxSections.sections[i].valid = false;
		}

		newField->runtime.smoothing.smoothed_avrg = 0.0;
		newField->runtime.smoothing.smoothingActive = false;

		*hdlOnReturn = newField;

		// Restart here..
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
jvx_estimate_buffer_fillheight_restart(jvxHandle* hdlOnEnter)
{
	jvxSize i,j;
	if(hdlOnEnter)
	{
		jvx_estimator_fillheight_private_struct* hdl = (jvx_estimator_fillheight_private_struct*)hdlOnEnter;
		hdl->runtime.circularBuffer.fHeight = 0;
		hdl->runtime.circularBuffer.fullyStarted = false;
		hdl->runtime.circularBuffer.idxWrite = 0;

		hdl->runtime.measurements.currentAverage_nom = 0;
		hdl->runtime.measurements.currentAverage_den = 0;

		for(i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
		{
			hdl->runtime.minMaxSections.sections[i].maxValue = JVX_DATA_MAX_NEG;
			hdl->runtime.minMaxSections.sections[i].minValue = JVX_DATA_MAX_POS;
			hdl->runtime.minMaxSections.sections[i].valid = false;
			for(j = 0; j < hdl->config.numOperations; j++)
			{
				hdl->runtime.minMaxSections.sections[i].minWeight_operation[j] = -1;
				hdl->runtime.minMaxSections.sections[i].maxWeight_operation[j] = -1;
			}
		}

		for(i = 0; i < hdl->config.numOperations; i++)
		{
			hdl->runtime.measurements.acc_weights_operations[i] = 0;
			hdl->runtime.measurements.count_operations[i] = 0;
		}

#ifdef DEBUG_BUFFER
		for(i = 0; i < hdl->runtime.circularBuffer.bLength; i++)
		{
			hdl->runtime.circularBuffer.circBuffer[i].value = 0;
			hdl->runtime.circularBuffer.circBuffer[i].weight = 0;
		}
#endif

		JVX_GET_TICKCOUNT_US_SETREF(&hdl->intime.ref);
		hdl->intime.start = JVX_SIZE_UNSELECTED;

		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}


jvxDspBaseErrorType
jvx_estimate_buffer_fillheight_terminate(jvxHandle* hdlOnEnter)
{
	jvxSize i;
	if(hdlOnEnter)
	{
		jvx_estimator_fillheight_private_struct* hdl = (jvx_estimator_fillheight_private_struct*)hdlOnEnter;

		for(i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
		{
			JVX_DSP_SAFE_DELETE_FIELD(hdl->runtime.minMaxSections.sections[i].minWeight_operation);
			JVX_DSP_SAFE_DELETE_FIELD(hdl->runtime.minMaxSections.sections[i].maxWeight_operation);
		}

		JVX_DSP_SAFE_DELETE_FIELD(hdl->runtime.measurements.acc_weights_operations);
		JVX_DSP_SAFE_DELETE_FIELD(hdl->runtime.measurements.count_operations);
		JVX_DSP_SAFE_DELETE_FIELD(hdl->runtime.circularBuffer.circBuffer);
		JVX_DSP_SAFE_DELETE_FIELD(hdl->runtime.minMaxSections.sections);

		JVX_DSP_SAFE_DELETE_OBJECT(hdl);

		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType
jvx_estimate_buffer_fillheight_process(jvxHandle* hdlOnEnter, jvxData in_value,
													jvxData in_weight, jvxSize in_operate_id,
													jvxData* out_average,
													jvxData* out_min,
													jvxData* out_max,
													jvxData* out_operate_average,
													jvxData* out_operate_min,
													jvxData* out_operate_max)
{
	jvxSize i;
	int inSectionCnt = -1;
	int sectionCnt = -1;
	int operateIdAccess = -1;
	int operateIdAccess_rem = -1;
	jvxCBool atLeastOne = false;
	jvxData relation = 0;

	if(hdlOnEnter)
	{
		jvx_estimator_fillheight_private_struct* hdl = (jvx_estimator_fillheight_private_struct*)hdlOnEnter;

#ifdef DEBUG_BUFFER
		// Debug code:
		jvxData ttn = 0;
		jvxData ttd = 0;
		for(i = 0; i < hdl->runtime.circularBuffer.bLength; i++)
		{
			ttn += hdl->runtime.circularBuffer.circBuffer[i].value * hdl->runtime.circularBuffer.circBuffer[i].weight;
			ttd += hdl->runtime.circularBuffer.circBuffer[i].weight;
		}

		if(ttn != hdl->runtime.measurements.currentAverage_nom)
		{
			int a = 0;
		}
#endif
		// Normal handling of inputs for average value - ALWAYS
		hdl->runtime.measurements.currentAverage_nom += in_value * in_weight;
		hdl->runtime.measurements.currentAverage_den += in_weight;
		relation = (hdl->runtime.measurements.currentAverage_nom / hdl->runtime.measurements.currentAverage_den);
		if(hdl->runtime.smoothing.smoothingActive)
		{
			hdl->runtime.smoothing.smoothed_avrg = (hdl->config.recSmoothFactor) * hdl->runtime.smoothing.smoothed_avrg  + (1.0 - hdl->config.recSmoothFactor) * relation;
		}
		else
		{
			hdl->runtime.smoothing.smoothed_avrg = relation;
			hdl->runtime.smoothing.smoothingActive = true;
		}

		operateIdAccess_rem = -1;
		if(hdl->runtime.circularBuffer.fullyStarted == true)
		{
			jvxData tmp = hdl->runtime.circularBuffer.circBuffer[hdl->runtime.circularBuffer.idxWrite].weight;
			hdl->runtime.measurements.currentAverage_nom -= hdl->runtime.circularBuffer.circBuffer[hdl->runtime.circularBuffer.idxWrite].value * tmp;
			hdl->runtime.measurements.currentAverage_den -= tmp;
			operateIdAccess_rem = (jvxInt32) hdl->runtime.circularBuffer.circBuffer[hdl->runtime.circularBuffer.idxWrite].operate_id - 1;

		}
		else
		{
			hdl->runtime.circularBuffer.fHeight ++;
			if(hdl->runtime.circularBuffer.fHeight == hdl->runtime.circularBuffer.bLength)
			{
				hdl->runtime.circularBuffer.fullyStarted = true;
			}
		}

		// Normal handling of input for min max values - ALWAYS
		sectionCnt = (jvxInt32)(hdl->runtime.circularBuffer.idxWrite / (int)hdl->config.numberEventsConsidered_perMinMaxSection);

		//printf("--> %i \n", sectionCnt);

		operateIdAccess = (jvxInt32)in_operate_id - 1; // <- 0 to indicate "dont care"
		if((operateIdAccess_rem >= 0) && (operateIdAccess_rem < hdl->config.numOperations))
		{
			hdl->runtime.measurements.count_operations[operateIdAccess_rem] --;
			hdl->runtime.measurements.acc_weights_operations[operateIdAccess_rem] -= in_weight;
		}

		if((operateIdAccess >= 0) && (operateIdAccess < hdl->config.numOperations))
		{
			hdl->runtime.measurements.count_operations[operateIdAccess]++;
			hdl->runtime.measurements.acc_weights_operations[operateIdAccess] += in_weight;

			// Operation specific minimum in section
			if(out_operate_average)
			{
				*out_operate_average = hdl->runtime.measurements.acc_weights_operations[operateIdAccess]/ (jvxData)hdl->runtime.measurements.count_operations[operateIdAccess];
			}
			if(in_weight < hdl->runtime.minMaxSections.sections[sectionCnt].minWeight_operation[operateIdAccess])
			{
				hdl->runtime.minMaxSections.sections[sectionCnt].minWeight_operation[operateIdAccess] = in_weight;
			}
			if(in_weight > hdl->runtime.minMaxSections.sections[sectionCnt].maxWeight_operation[operateIdAccess])
			{
				hdl->runtime.minMaxSections.sections[sectionCnt].maxWeight_operation[operateIdAccess] = in_weight;
			}
		}

		// Now, overwrite the current position
		hdl->runtime.circularBuffer.circBuffer[hdl->runtime.circularBuffer.idxWrite].value = in_value;
		hdl->runtime.circularBuffer.circBuffer[hdl->runtime.circularBuffer.idxWrite].weight = in_weight;
		hdl->runtime.circularBuffer.circBuffer[hdl->runtime.circularBuffer.idxWrite].operate_id = in_operate_id;


		// Local minimum in current section
		if(in_value < hdl->runtime.minMaxSections.sections[sectionCnt].minValue)
		{
			hdl->runtime.minMaxSections.sections[sectionCnt].minValue = in_value;
		}

		// Local maximum in current section
		if(in_value > hdl->runtime.minMaxSections.sections[sectionCnt].maxValue)
		{
			hdl->runtime.minMaxSections.sections[sectionCnt].maxValue = in_value;
		}

		// Processing done for now, approach to next entry in circular buffer, output and section update if required

		// Increment circular buffer counter
		hdl->runtime.circularBuffer.idxWrite = (hdl->runtime.circularBuffer.idxWrite + 1) % hdl->runtime.circularBuffer.bLength;

		// Output handling - average output
		if(out_average)
		{
			*out_average = hdl->runtime.smoothing.smoothed_avrg;
		}

		// Output handling - minimum output: Search minimum value in all sections
		if(out_min)
		{
			*out_min = JVX_DATA_MAX_POS;
			for(i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
			{
				if(hdl->runtime.minMaxSections.sections[i].minValue < *out_min)
				{
					*out_min = hdl->runtime.minMaxSections.sections[i].minValue;
				}
			}
		}

		// Output handling - minimum output: Search maximum value in all sections
		if(out_max)
		{
			*out_max = JVX_DATA_MAX_NEG;
			for(i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
			{
				if(hdl->runtime.minMaxSections.sections[i].maxValue > *out_max)
				{
					*out_max = hdl->runtime.minMaxSections.sections[i].maxValue;
				}
			}
		}

		if(operateIdAccess >= 0)
		{
			atLeastOne = false;
			if(out_operate_min)
			{
				*out_operate_min = JVX_DATA_MAX_POS;
				for(i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
				{
					if(hdl->runtime.minMaxSections.sections[i].minWeight_operation[operateIdAccess] >= 0) // ????
					{
						if(hdl->runtime.minMaxSections.sections[i].minWeight_operation[operateIdAccess] < *out_operate_min)
						{
							*out_operate_min  = hdl->runtime.minMaxSections.sections[i].minWeight_operation[operateIdAccess];
							atLeastOne = true;
						}
					}
				}
				if(!atLeastOne)
				{
					*out_operate_min = -1;
				}
			}

			atLeastOne = false;
			if(out_operate_max)
			{
				*out_operate_max = JVX_DATA_MAX_NEG;
				for(i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
				{
					if(hdl->runtime.minMaxSections.sections[i].maxWeight_operation[operateIdAccess] >= 0) // <- ????
					{
						if(hdl->runtime.minMaxSections.sections[i].maxWeight_operation[operateIdAccess] < *out_operate_max)
						{
							*out_operate_max  = hdl->runtime.minMaxSections.sections[i].maxWeight_operation[operateIdAccess];
							atLeastOne = true;
						}
					}
				}
				if(!atLeastOne)
				{
					*out_operate_max = -1;
				}
			}
		}
		// Now, set pointer one forward if necessary for next call
		if(hdl->runtime.circularBuffer.idxWrite % hdl->config.num_MinMaxSections == 0)
		{
			// In this case, the current section is complete...

			// reset new section values
			sectionCnt = (jvxInt32)(hdl->runtime.circularBuffer.idxWrite / (int)hdl->config.numberEventsConsidered_perMinMaxSection);
			hdl->runtime.minMaxSections.sections[sectionCnt].maxValue = JVX_DATA_MAX_NEG;
			hdl->runtime.minMaxSections.sections[sectionCnt].minValue = JVX_DATA_MAX_POS;
			for(i = 0; i < hdl->config.numOperations; i++)
			{
				hdl->runtime.minMaxSections.sections[sectionCnt].maxWeight_operation[i] = -1;
				hdl->runtime.minMaxSections.sections[sectionCnt].minWeight_operation[i] = -1;
			}
		}
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_estimate_buffer_fillheight_process_time_int(jvxHandle* hdlOnEnter, jvxSize fHeight, jvxSize in_operate_id)
{
	if (hdlOnEnter)
	{
		jvx_estimator_fillheight_private_struct* hdl = (jvx_estimator_fillheight_private_struct*)hdlOnEnter;
		jvxTick tStampStart = JVX_GET_TICKCOUNT_US_GET_CURRENT(&hdl->intime.ref);
		if (JVX_CHECK_SIZE_SELECTED(hdl->intime.start))
		{
			jvxData in_value = (jvxData)fHeight;
			jvxData in_weight = (jvxData)(tStampStart - hdl->intime.start);
			jvx_estimate_buffer_fillheight_process(hdl, in_value, in_weight, in_operate_id, NULL, NULL, NULL, NULL, NULL, NULL);
		}
		hdl->intime.start = tStampStart;
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_estimate_buffer_fillheight_process_time_ext(jvxHandle* hdlOnEnter, jvxSize fHeight, jvxSize in_operate_id,
	jvxUInt64 tStampStart)
{
	if (hdlOnEnter)
	{
		jvx_estimator_fillheight_private_struct* hdl = (jvx_estimator_fillheight_private_struct*)hdlOnEnter;
		if (JVX_CHECK_SIZE_SELECTED(hdl->intime.start))
		{
			jvxData in_value = (jvxData)fHeight;
			jvxData in_weight = (jvxData)(tStampStart - hdl->intime.start);
			jvx_estimate_buffer_fillheight_process(hdl, in_value, in_weight, in_operate_id, NULL, NULL, NULL, NULL, NULL, NULL);
		}
		hdl->intime.start = tStampStart;
		return(JVX_DSP_NO_ERROR);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_estimate_buffer_fillheight_read(jvxHandle* hdlOnEnter, 
	jvxSize in_operate_id,
	jvxData* out_average,
	jvxData* out_min, 
	jvxData* out_max, 
	jvxData* out_operate_average, 
	jvxData* out_operate_min,
	jvxData* out_operate_max)
{
	jvxInt32 operateIdAccess = -1;
	jvxSize i;
	jvxCBool atLeastOne = false;
	if (hdlOnEnter)
	{
		jvx_estimator_fillheight_private_struct* hdl = (jvx_estimator_fillheight_private_struct*)hdlOnEnter;
		operateIdAccess = (jvxInt32)in_operate_id - 1; // <- 0 to indicate "dont care"
		// Output handling - average output
		if (out_average)
		{
			*out_average = hdl->runtime.smoothing.smoothed_avrg;
		}

		// Output handling - minimum output: Search minimum value in all sections
		if (out_min)
		{
			*out_min = JVX_DATA_MAX_POS;
			for (i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
			{
				if (hdl->runtime.minMaxSections.sections[i].minValue < *out_min)
				{
					*out_min = hdl->runtime.minMaxSections.sections[i].minValue;
				}
			}
		}

		// Output handling - minimum output: Search maximum value in all sections
		if (out_max)
		{
			*out_max = JVX_DATA_MAX_NEG;
			for (i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
			{
				if (hdl->runtime.minMaxSections.sections[i].maxValue > *out_max)
				{
					*out_max = hdl->runtime.minMaxSections.sections[i].maxValue;
				}
			}
		}
		if (operateIdAccess >= 0)
		{
			atLeastOne = false;
			if (out_operate_min)
			{
				*out_operate_min = JVX_DATA_MAX_POS;
				for (i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
				{
					if (hdl->runtime.minMaxSections.sections[i].minWeight_operation[operateIdAccess] >= 0) // ???????
					{
						if (hdl->runtime.minMaxSections.sections[i].minWeight_operation[operateIdAccess] < *out_operate_min)
						{
							*out_operate_min = hdl->runtime.minMaxSections.sections[i].minWeight_operation[operateIdAccess];
							atLeastOne = true;
						}
					}
				}
				if (!atLeastOne)
				{
					*out_operate_min = -1;
				}
			}

			atLeastOne = false;
			if (out_operate_max)
			{
				*out_operate_max = JVX_DATA_MAX_NEG;
				for (i = 0; i < hdl->runtime.minMaxSections.numSections; i++)
				{
					if (hdl->runtime.minMaxSections.sections[i].maxWeight_operation[operateIdAccess] >= 0) // ????????
					{
						if (hdl->runtime.minMaxSections.sections[i].maxWeight_operation[operateIdAccess] < *out_operate_max)
						{
							*out_operate_max = hdl->runtime.minMaxSections.sections[i].maxWeight_operation[operateIdAccess];
							atLeastOne = true;
						}
					}
				}
				if (!atLeastOne)
				{
					*out_operate_max = -1;
				}
			}
		}
		return(JVX_DSP_NO_ERROR);
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}
