// This code was provided by x.AI due to my question:
// "Wie programme ich unter Android eine Audioengine mit Ein- und Ausgabe mit geringer Latenz? Der Code soll auf dem NDK basieren."
// It can not be used as it is but we can start from this source.

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#include <jni.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define LOG_TAG "AudioEngine"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

#define SAMPLE_RATE 44100
#define BUFFER_FRAMES 256 // Kleine Puffergröße für geringe Latenz
#define CHANNELS 1        // Mono

// Globale OpenSL ES Objekte
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine = NULL;
static SLObjectItf outputMixObject = NULL;
static SLObjectItf recorderObject = NULL;
static SLObjectItf playerObject = NULL;

// Puffer für Ein- und Ausgabe
static int16_t *audioBuffer = NULL;
static bool isRunning = false;

// Callback für Audioeingabe
void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    if (!isRunning) return;

    // Audioeingabe in den Puffer schreiben
    SLresult result = (*bq)->Enqueue(bq, audioBuffer, BUFFER_FRAMES * CHANNELS * sizeof(int16_t));
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Recorder Enqueue failed: %d", result);
    }
}

// Callback für Audioausgabe
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    if (!isRunning) return;

    // Audioausgabe aus dem Puffer lesen
    SLresult result = (*bq)->Enqueue(bq, audioBuffer, BUFFER_FRAMES * CHANNELS * sizeof(int16_t));
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Player Enqueue failed: %d", result);
    }
}

// Initialisierung der Audioengine
jboolean Java_com_example_audioengine_AudioEngine_startAudio(JNIEnv *env, jobject thiz) {
    SLresult result;

    // Engine erstellen
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("slCreateEngine failed: %d", result);
        return JNI_FALSE;
    }

    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Engine Realize failed: %d", result);
        return JNI_FALSE;
    }

    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Get Engine Interface failed: %d", result);
        return JNI_FALSE;
    }

    // Output Mix erstellen
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("CreateOutputMix failed: %d", result);
        return JNI_FALSE;
    }

    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("OutputMix Realize failed: %d", result);
        return JNI_FALSE;
    }

    // Audioeingabe (Recorder) konfigurieren
    SLDataLocator_IODevice loc_dev = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
    SLDataSource audioSrc = {&loc_dev, NULL};

    SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {
        SL_DATAFORMAT_PCM,
        CHANNELS,
        SL_SAMPLINGRATE_44_1,
        SL_PCMSAMPLEFORMAT_FIXED_16,
        SL_PCMSAMPLEFORMAT_FIXED_16,
        CHANNELS == 1 ? SL_SPEAKER_FRONT_CENTER : SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
        SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSink audioSnk = {&loc_bq, &format_pcm};

    const SLInterfaceID ids_rec[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req_rec[] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioRecorder(engineEngine, &recorderObject, &audioSrc, &audioSnk, 1, ids_rec, req_rec);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("CreateAudioRecorder failed: %d", result);
        return JNI_FALSE;
    }

    result = (*recorderObject)->Realize(recorderObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Recorder Realize failed: %d", result);
        return JNI_FALSE;
    }

    SLAndroidSimpleBufferQueueItf recorderBufferQueue;
    result = (*recorderObject)->GetInterface(recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &recorderBufferQueue);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Recorder Buffer Queue Interface failed: %d", result);
        return JNI_FALSE;
    }

    // Audioausgabe (Player) konfigurieren
    SLDataLocator_AndroidSimpleBufferQueue loc_play_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataSource audioSrc_play = {&loc_play_bq, &format_pcm};

    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk_play = {&loc_outmix, NULL};

    const SLInterfaceID ids_play[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_VOLUME};
    const SLboolean req_play[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audioSrc_play, &audioSnk_play, 2, ids_play, req_play);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("CreateAudioPlayer failed: %d", result);
        return JNI_FALSE;
    }

    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Player Realize failed: %d", result);
        return JNI_FALSE;
    }

    SLAndroidSimpleBufferQueueItf playerBufferQueue;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &playerBufferQueue);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Player Buffer Queue Interface failed: %d", result);
        return JNI_FALSE;
    }

    // Puffer initialisieren
    audioBuffer = (int16_t *)malloc(BUFFER_FRAMES * CHANNELS * sizeof(int16_t));
    if (!audioBuffer) {
        LOGD("Buffer allocation failed");
        return JNI_FALSE;
    }
    memset(audioBuffer, 0, BUFFER_FRAMES * CHANNELS * sizeof(int16_t));

    // Callbacks registrieren
    result = (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue, bqRecorderCallback, NULL);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Recorder Callback Register failed: %d", result);
        return JNI_FALSE;
    }

    result = (*playerBufferQueue)->RegisterCallback(playerBufferQueue, bqPlayerCallback, NULL);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Player Callback Register failed: %d", result);
        return JNI_FALSE;
    }

    // Audio starten
    isRunning = true;

    result = (*playerBufferQueue)->Enqueue(playerBufferQueue, audioBuffer, BUFFER_FRAMES * CHANNELS * sizeof(int16_t));
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Player Enqueue failed: %d", result);
        return JNI_FALSE;
    }

    result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue, audioBuffer, BUFFER_FRAMES * CHANNELS * sizeof(int16_t));
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Recorder Enqueue failed: %d", result);
        return JNI_FALSE;
    }

    SLPlayItf recorderPlay;
    result = (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD, &recorderPlay);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Recorder Play Interface failed: %d", result);
        return JNI_FALSE;
    }

    result = (*recorderPlay)->SetRecordState(recorderPlay, SL_RECORDSTATE_RECORDING);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("SetRecordState failed: %d", result);
        return JNI_FALSE;
    }

    SLPlayItf playerPlay;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("Player Play Interface failed: %d", result);
        return JNI_FALSE;
    }

    result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("SetPlayState failed: %d", result);
        return JNI_FALSE;
    }

    LOGD("Audio engine started successfully");
    return JNI_TRUE;
}

// Audioengine stoppen
void Java_com_example_audioengine_AudioEngine_stopAudio(JNIEnv *env, jobject thiz) {
    isRunning = false;

    if (recorderObject != NULL) {
        SLRecordItf recorderRecord;
        (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD, &recorderRecord);
        (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_STOPPED);
        (*recorderObject)->Destroy(recorderObject);
        recorderObject = NULL;
    }

    if (playerObject != NULL) {
        SLPlayItf playerPlay;
        (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
        (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_STOPPED);
        (*playerObject)->Destroy(playerObject);
        playerObject = NULL;
    }

    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
    }

    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    if (audioBuffer != NULL) {
        free(audioBuffer);
        audioBuffer = NULL;
    }

    LOGD("Audio engine stopped");
}