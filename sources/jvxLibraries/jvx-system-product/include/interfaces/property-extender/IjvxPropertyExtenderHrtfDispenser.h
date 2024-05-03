#ifndef _IJVXPROPERTYEXTENDERHRTFDISPENSER_H_
#define _IJVXPROPERTYEXTENDERHRTFDISPENSER_H_

JVX_INTERFACE IjvxPropertyExtenderHrtfDispenser_report
{
public:
	virtual ~IjvxPropertyExtenderHrtfDispenser_report() {};
	virtual jvxErrorType report_database_changed(jvxSize slotId) = 0;
};

JVX_INTERFACE IjvxPropertyExtenderHrtfDispenser
{
public:
	virtual ~IjvxPropertyExtenderHrtfDispenser() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION init(jvxSize* samplerate, jvxSize numSlots) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_length_hrir(jvxSize& length_hrir, jvxSize* loadId, jvxSize slotId) = 0;
	/* Get HRIR pair from SOFA database. 
	* @param inclination_deg: inclination angle of source direction in degree.
	* @param azimuth_deg: azimuth angle of source direction in degree.
	* @param hrir_left: pointer to array of length received by get_length_hrir(jvxSize& length_hrir). Returns the HRIR for the left ear.
	* @param hrir_left: pointer to array of length received by get_length_hrir(jvxSize& length_hrir). Returns the HRIR for the right ear.
	*/
	virtual jvxErrorType JVX_CALLINGCONVENTION copy_closest_hrir_pair(jvxData azimuth_deg, jvxData inclination_deg, 
		jvxData* hrir_left, jvxData* hrir_right, jvxSize length_hrir, jvxSize dataBaseId, jvxSize slotId) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_closest_direction(jvxData& azimuth_deg, jvxData& inclination_deg, jvxSize slotId) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION register_change_listener(IjvxPropertyExtenderHrtfDispenser_report* ref) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_change_listener(IjvxPropertyExtenderHrtfDispenser_report* ref) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* isReady, jvxApiString* reason) = 0;
};

#endif
