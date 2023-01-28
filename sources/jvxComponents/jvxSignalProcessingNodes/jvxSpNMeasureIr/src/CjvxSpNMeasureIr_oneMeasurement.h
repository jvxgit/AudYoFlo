#ifndef __CJVXSPNMEASUREIR_ONEMEASUREMENT_H__
#define __CJVXSPNMEASUREIR_ONEMEASUREMENT_H__

#include "jvx.h"
extern "C"
{
#include "jvx_generator_wave.h"
#include "jvx_measure_ir.h"
};

#include "jvxSpNMeasureIr_oneMeasurementChannel.h"

#define JVX_COMPOSE_CHANNEL_NAME(id) ("channel_" + jvx_size2String(id))
#define JVX_WAV_READ_WRITE_INTERNAL_BSIZE 1024
#define JVX_NORMALIZE_MIN 0.01
#define JVX_NORMALIZE_MAX_AMP 0.95

//===========================================================================

JVX_INTERFACE IjvxSpNMeasureIr_oneMeasurement_report
{
public:
	virtual ~IjvxSpNMeasureIr_oneMeasurement_report() {};
	virtual void jvxSpNMeasureIr_reportText(const std::string& txt) = 0;
};

class oneInput
{
public:
	jvxBool active;
	jvxSize id;
	jvxMeasureIr oneMeasure;
	jvxSize offBuf;
	oneInput()
	{
		active = false;
		id = 0;
		jvx_measure_ir_initConfig(&oneMeasure);
		offBuf = 0;
	};
};
	 
class oneOutput
{
public:
	jvxBool active;
	jvxSize id;
	oneOutput()
	{
		active = false;
		id = 0;
	};
};

class oneMeasurementResult
{
public:
	std::string nameMeasure;
	std::string tokenMeasure;
	jvxSize rate;
	jvxBool storeConfig;
	std::string pathStoreFolder;
	std::string pathStoreProto;
	std::list<oneMeasurementChannel> resultsChannel;
	jvxData* freq_buf;
	jvxData* gain_buf;
	jvxSize freq_gain_len;
	oneMeasurementResult()
	{
		rate = 0;
		storeConfig = true;
		freq_buf = NULL;
		gain_buf = NULL;
		freq_gain_len = 0;
	};
};

//===========================================================================

class CjvxSpNMeasureIr_oneMeasurement
{
public:
	typedef enum
	{
		JVX_ONE_MEASUREMENT_WAIT,
		JVX_ONE_MEASUREMENT_PRODUCE,
		JVX_ONE_MEASUREMENT_EVALUATE,
		JVX_ONE_MEASUREMENT_STORE
	} measurementState;

	measurementState currentStatus;
	std::map<jvxSize, oneInput> inputs;
	std::map<jvxSize, oneOutput> outputs;
	std::string description;
	jvxSize rate;
	jvxSize internal_buffersize;
	jvx_generatorWave theGenerator;
	std::string pathStoreFolder;
	std::string pathStoreProto;

	jvxData* freq_buf;
	jvxData* gain_buf;
	jvxSize freq_gain_len;

	jvxBool measurementComplete;
	struct
	{
		jvxGeneratorWaveType tpWave;
		jvxInt32 seed_noise;
		jvxData amplitude;
		jvxGeneratorWaveNoiseType noise_type;
		jvxData length_seconds;
		jvxSize loop_count;
		jvxData silence_start_seconds;
		jvxData silence_stop_seconds;
		jvxData freq_low_hz;
		jvxData freq_up_hz;
	} generator;

	struct
	{
		jvxSize skip_eval;
		jvxBool derive_ir;
		jvxBool store_data;
	} evaluation;


	CjvxSpNMeasureIr_oneMeasurement();

	void allocate_one_measurement(jvxSize rate_measure, jvxSize buffersize_store, jvxSize* numResults);

	void evaluate_measurement();

	jvxErrorType copy_measurement_channel(
		jvxSize cnt, 
		oneMeasurementChannel* oneChan);	
		jvxErrorType copy_measurement(
		oneMeasurementResult* res);

	void write_data(
		const std::string& token, 
		IjvxRtAudioFileWriter* fWriter, 
		IjvxConfigProcessor* proc, 
		IjvxSpNMeasureIr_oneMeasurement_report* rep);

	static jvxErrorType read_measurement(
		oneMeasurementResult* oneResult,
		const std::string& fNameNew, 
		IjvxRtAudioFileReader* theReader,
		IjvxConfigProcessor* confHdl, 
		jvxSize internal_bsize);

	void deallocate_one_measurement();

	void addData(
		jvxData** bufsOut,
		jvxSize numChansOut,
		jvxData** bufsIn,
		jvxSize numChansIn,
		jvxSize off, jvxSize* numW, 
		jvxSize bsize,
		jvxBool& src_complete,
		jvxData* progress_loop = NULL, 
		jvxData* progress_in_period = NULL);

private:

	jvxErrorType writeOneFile(
		const std::string& storeName, 
		const std::string& pathStore,
		const std::string& cf_token_prefix,
		jvxData* fldBufferTestSignal, 
		jvxSize lRecording, IjvxConfigProcessor* proc, 
		jvxConfigData* datSubSec,
		jvxSize rate, 
		IjvxRtAudioFileWriter* fWriter);

	static jvxErrorType readOneFile_allocate(
		const std::string& tokenPrefix,
		const std::string& pathExpr, 
		jvxSize* lBuf, jvxData** buf, jvxSize* rate,
		IjvxConfigProcessor* confHdl,
		jvxConfigData* datSubSec,
		IjvxRtAudioFileReader* theReader,
		jvxSize internal_bsize);
};


#endif
