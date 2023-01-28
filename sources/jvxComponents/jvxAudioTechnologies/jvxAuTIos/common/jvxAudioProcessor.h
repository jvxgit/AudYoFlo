//
//  jvxAudioProcessor.h
//  myJvxApp
//
//  Created by Hauke Krüger on 22.12.15.
//  Copyright © 2015 Javox Solutions. All rights reserved.
//

// This file is included by ViewController.m. Therefore, it
// ust not contain the keyword "class"

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import "jvxAudioProcessor_env.h"
#import "jvxIosHost_typedefs.h"

// Activate to use minimum number of channels (not all device channels, allow 0 channels)
#define JVX_NUMBER_INPUT_CHANNELS_MIN

// Activate to use minimum number of channels (not all device channels, allow 0 channels)
#define JVX_NUMBER_OUTPUT_CHANNELS_MIN

//! Definition of data struct used while in processing
struct privateAudioProcessor
{
    AudioUnit* audioUnitInUse;
    AudioBufferList* ioData_input;
    jvxBitField inChannelMask;
    jvxBitField outChannelMask;
    jvxLinkDataDescriptor* theData_in;
    jvxLinkDataDescriptor* theData_out;
    callbackProcessOneBuffer bufferSwitchCallback;
    void* callback_priv;
    jvxSize numUsedChannelsInput;
    jvxSize numUsedChannelsOutput;
    jvxBool noInput;
    jvxHandle* inputDummy;
    jvxBool noOutput;
    jvxHandle* outputDummy;

};

// Callback function to pass data to obj c objects
typedef void (*callbackViewController)(jvxIosBwdReportEvent commandId, void* priv, void *context);

/*
 * Collect virtual audio devices. Each route may be mapped to one 
 * virtual audio device.
 *=======================================================*/
@interface oneDataSourceOption : NSObject

    @property (nonatomic, strong) NSString *name;
    @property NSInteger iPortId;
    @property NSInteger iSourceId;
@end


/*
 * Audio processing class: Audio processing may be started and stopped
 *=======================================================*/
@interface jvxAudioProcessor : NSObject
{
    AVAudioSession *sessionInstance;
    
    // Audio unit of type remote I/O to route audio samples
    AudioUnit               _rioUnit;
    
    // Private data to pass data into the render callback
    struct privateAudioProcessor* myPrivateData;

    struct
    {
    
        // Callback function to report audio events back to ViewController
        callbackViewController callback_fc;
    
        // Some private data to provide context to caller
        void* callback_priv;
    } bwd_references;
    
    struct
    {
        jvxState theState;
        
        // Status variables: is audio currently running?
        // bool audioIsRunning;
        
        // Status variable: Is speaker currently on?
        bool speakerIsOn;
        
    } state_machine;

    // Hold a list of exposed devices
    NSMutableArray* lstExposedDevices;
    jvxSize idPortDeviceName;
    
    // Should be in struct but obj c in structs is forbidden...
    NSMutableArray *detectedSamplerates;
    NSMutableArray *detectedBuffersizes;
    
    NSMutableArray *inputOptions;
    int idInputOptionSelected;
    
    NSMutableArray *outputOptions;
    int idOutputOptionSelected;
    
    NSMutableArray* inputChannels;
    NSMutableArray* outputChannels;
    
}

//! Init audio, that is, prepare audio session, start audio unit and setup rendering callback
- (jvxErrorType)initAudioTechnology:(void*)content
                        andCallback:(callbackViewController)callback;
//                       andBluetooth:(jvxBool)activateBluetooth
//                         andSpeaker:(jvxBool)activateSpeaker;

- (jvxErrorType)terminateAudioTechnology;

- (jvxErrorType) get_number_of_ports:(jvxSize*)num;

- (jvxErrorType) get_description_port:(jvxSize) idx
                      returnString:(NSString**)name;


- (jvxErrorType) initAudioDevice: (jvxSize) idDevice;

- (jvxErrorType) terminateAudioDevice;

//!
- (jvxErrorType) startAudio:(callbackProcessOneBuffer)theExtCallback
             withVoidBwdRef:(jvxHandle*)priv
            withLinkDataInDescriptorPtr:(jvxLinkDataDescriptor*) theData_in
            withLinkDataOutDescriptorPtr:(jvxLinkDataDescriptor*) theData_out
            withInputChannelSelector:(jvxBitField) inChannels
            withOutputChannelsSelector:(jvxBitField) outChannels
       withMaxInputChannels:(jvxSize)maxInChans
      withMaxOutputChannels:(jvxSize)maxOutChans;

//withSamplerateId:(jvxSize)idSRate
//           withBuffersizeId:(jvxSize) idBSize
//           withDataFormatId:(jvxSize)idDFormat
//       withNumberInChannels:(jvxSize)numInChannels
//      withNumberOutChannels:(jvxSize)numOutChannels;


//! Opposite of init function: Stop and release audio unit
- (jvxErrorType)stopAudio;


//! Status of audio subsystem
- (jvxErrorType)status:(jvxState*) statusOnReturn;


//! Current status of speaker
-(jvxErrorType) statusActivateSpeaker: (jvxBool*) status;



//! Thread portation callback to move events into main thread
- (void)handleInMainThread:(jvxIosBwdReportEvent)cmd
              withUserInfo:(NSDictionary*)dict;

- (void)handleInterruption:(NSNotification *)notification;
- (void)handleRouteChange:(NSNotification *)notification;
- (void)handleMediaServerReset:(NSNotification *)notification;


// Functions to interact with RTProc system
- (jvxErrorType) get_number_input_channels:(jvxSize*) num;
- (jvxErrorType) get_description_input_channel:(jvxSize) idx
                                  returnString:(NSString**)nm;

- (jvxErrorType) get_number_output_channels:(jvxSize*) num;
- (jvxErrorType) get_description_output_channel:(jvxSize) idx
                                   returnString:(NSString**)nm;

- (jvxErrorType) get_number_samplerates:(jvxSize*) num;
- (jvxErrorType) get_samplerate:(jvxSize) idx
                    returnValue:(float*)val;

- (jvxErrorType) get_number_buffersizes:(jvxSize*) num;
- (jvxErrorType) get_buffersize:(jvxSize) idx
                    returnValue:(jvxSize*)val;

- (jvxErrorType) get_number_dataformats:(jvxSize*) num;
- (jvxErrorType) get_dataformat:(jvxSize) idx
                    returnValue:(jvxDataFormat*)form;


- (jvxErrorType) get_number_input_sources:(jvxSize*) num;
- (jvxErrorType) get_input_source:(jvxSize) idx
                    returnValue:(NSString**)name
isSourceActive:(jvxBool*)isActive;

- (jvxErrorType) get_number_output_sources:(jvxSize*) num;
- (jvxErrorType) get_output_source:(jvxSize) idx
                     returnValue:(NSString**)name
isSourceActive:(jvxBool*)isActive;

// Ios signal routing
- (jvxErrorType) activateInputOptionId:(int) idSelect;
- (jvxErrorType) activateOutputOptionId:(int) idSelect;
- (jvxErrorType) activateSpeaker:(jvxBool)activateSpeaker;

- (jvxErrorType) activateInputOptionId_core:(int) idSelect withError:(NSError**) err;
- (jvxErrorType) activateOutputOptionId_core:(int) idSelect withError:(NSError**) err;



@end
