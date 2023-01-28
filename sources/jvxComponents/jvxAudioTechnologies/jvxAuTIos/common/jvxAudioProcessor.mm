#import <AVFoundation/AVFoundation.h>

#import "CAXException.h"
#import "CAStreamBasicDescription.h"
#import "jvxAudioProcessor.h"

#define JVX_ACTIVATE_BLUETOOTH true
#define JVX_ACTIVATE_SPEAKER false

#define JVX_ASSERT(cond, descr) \
    if(!cond) \
    { \
        NSLog(descr); \
        assert(0); \
    }

@interface oneDataSourceOption ()

@end

@implementation oneDataSourceOption
@end


/*
 * Rendering core functionality
 *========================================================*/
// Render callback function
static OSStatus	performRender (void                         *inRefCon,
                               AudioUnitRenderActionFlags 	*ioActionFlags,
                               const AudioTimeStamp 		*inTimeStamp,
                               UInt32 						inBusNumber,
                               UInt32 						inNumberFrames,
                               AudioBufferList              *ioData)
{
    OSStatus err = noErr;
    int idx;
    int cnt = 0;
    privateAudioProcessor* theAudioProc = (privateAudioProcessor*)inRefCon;

    //Get data from input
    if(!theAudioProc->noInput)
    {
        err = AudioUnitRender(*theAudioProc->audioUnitInUse, ioActionFlags, inTimeStamp, 1, inNumberFrames, theAudioProc->ioData_input);
    }
    
    // Bufferswitch callback in sub system
    theAudioProc->bufferSwitchCallback(theAudioProc->callback_priv);
    
    for(idx = 0; idx < theAudioProc->numUsedChannelsOutput; idx++)
    {
        if(jvx_bitTest(theAudioProc->outChannelMask, idx))
        {
            Float32* bufIn = (Float32*)theAudioProc->theData_in->con_data.buffers[0][cnt];
            memcpy(ioData->mBuffers[idx].mData, bufIn, ioData->mBuffers[idx].mDataByteSize);
            cnt++;
        }
        else
        {
            memset(ioData->mBuffers[idx].mData, 0, ioData->mBuffers[idx].mDataByteSize);
        }
    }
    
    /*
     Float32* buf = (Float32*);
    Float32* buf2 = (Float32*)ioData->mBuffers[1].mData;
    for(idx= 0; idx < inNumberFrames; idx++)
    {
        Float32 tmp =*bufIn;
        bufIn++;

        tmp *= 3.0;
        
        *buf = tmp;
        buf++;
        *buf2 = tmp;
        buf2++;
    }*/
    return err;
}

/*
 * Rendering core functionality
 *========================================================*/
// Render callback function
static OSStatus	performCapture (void                         *inRefCon,
                               AudioUnitRenderActionFlags 	*ioActionFlags,
                               const AudioTimeStamp 		*inTimeStamp,
                               UInt32 						inBusNumber,
                               UInt32 						inNumberFrames,
                               AudioBufferList              *ioData)
{
    OSStatus err = noErr;
    int idx = 0;
    int cnt = 0;
    privateAudioProcessor* theAudioProc = (privateAudioProcessor*)inRefCon;
    
    //Get data from input
    if(!theAudioProc->noInput)
    {
        err = AudioUnitRender(*theAudioProc->audioUnitInUse, ioActionFlags, inTimeStamp, 1, inNumberFrames, theAudioProc->ioData_input);
    }
    
    // Bufferswitch callback in sub system
    theAudioProc->bufferSwitchCallback(theAudioProc->callback_priv);
    
    memset(ioData->mBuffers[idx].mData, 0, ioData->mBuffers[idx].mDataByteSize);

    return err;
}

/*
 * Interface realizing audio subsystem
 *=========================================================*/
@interface jvxAudioProcessor ()

@end

@implementation jvxAudioProcessor

/*
 * Initialization of sub module for audio processing
 *========================================================*/
- (id)init
{
    self = [super init];
    
    state_machine.theState = JVX_STATE_INIT;
    
    sessionInstance = NULL;
    
    myPrivateData = NULL;
    
    bwd_references.callback_priv = NULL;
    bwd_references.callback_fc = NULL;
    
    lstExposedDevices = [[NSMutableArray alloc ] init];
    idPortDeviceName = 0;
    
    // Check the device capabilities
    detectedSamplerates = [[NSMutableArray alloc ] init];
    detectedBuffersizes = [[NSMutableArray alloc ] init];

    inputOptions = [[NSMutableArray alloc] init];
    outputOptions = [[NSMutableArray alloc] init];
    
    inputChannels = [[NSMutableArray alloc] init];
    outputChannels = [[NSMutableArray alloc] init];
    
    return self;
}

- (void) displayInfo:(NSString*)token
{
    bwd_references.callback_fc(JVX_IOS_BWD_COMMAND_TEXT_MESSAGE, bwd_references.callback_priv, (__bridge void*)token);
    NSLog(@"%@", token);
}


/*
 * Bootup and start audio processing
 *========================================================*/
- (jvxErrorType)initAudioTechnology:(void*)content
      andCallback:(callbackViewController)callback
{
    BOOL resL = FALSE;
    jvxErrorType res = JVX_NO_ERROR;
    NSError *error = nil;
    int idx, jdx;
    NSArray <AVAudioSessionPortDescription *>* iPorts = nil;
    NSArray <AVAudioSessionDataSourceDescription *> *iSources = nil;
    NSArray <AVAudioSessionDataSourceDescription *> *oSources = nil;
    
    NSString* reportTxt;
    
    if(state_machine.theState == JVX_STATE_INIT)
    {
        
        bwd_references.callback_fc = callback;
        bwd_references.callback_priv = content;

        [self displayInfo:@"Initialized audio"];
        
        // Init the audio session
        res = [self activateAudioSession:NULL];
        
        // Free the list of exposed devices at first
        [lstExposedDevices removeAllObjects];
        [self findInputPorts];

        
        state_machine.theState = JVX_STATE_SELECTED;

        // Install callbacks, need to do that AFTER state change since the callback route requires state higher than INIT
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(handleInterruption:)
                                                     name:AVAudioSessionInterruptionNotification
                                                   object:sessionInstance];
        
        // we don't do anything special in the route change notification
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(handleRouteChange:)
                                                     name:AVAudioSessionRouteChangeNotification
                                                   object:sessionInstance];
        
        // if media services are reset, we need to rebuild our audio chain
        [[NSNotificationCenter defaultCenter]	addObserver:	self
                                                 selector:	@selector(handleMediaServerReset:)
                                                     name:	AVAudioSessionMediaServicesWereResetNotification
                                                   object:	sessionInstance];
        
        res = [self deactivateAudioSession];

    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

- (jvxErrorType) activateAudioSession:(NSString*)portName
{
    int i;
    jvxBool activateBluetooth = JVX_ACTIVATE_BLUETOOTH;
    jvxBool activateSpeaker = JVX_ACTIVATE_SPEAKER;
    jvxErrorType res = JVX_NO_ERROR;
    NSArray <AVAudioSessionPortDescription *>* iPorts = nil;

    BOOL resL = FALSE;
    NSError* error;

    [self displayInfo:[NSString stringWithFormat:@"activateAudioSession with port name %@", portName]];
    
    // ==================================================
    // Configure the audio session
    // ==================================================
    sessionInstance = [AVAudioSession sharedInstance];
    
    // ==================================================
    // Configure for Duplex audio
    // ==================================================
    NSInteger flagOptions = 0;
    if(activateBluetooth)
    {
        flagOptions |= AVAudioSessionCategoryOptionAllowBluetooth;
    }
    
    state_machine.speakerIsOn = false;
    if(activateSpeaker)
    {
        flagOptions |= AVAudioSessionCategoryOptionDefaultToSpeaker;
        state_machine.speakerIsOn = true;
    }
    
    resL = [sessionInstance      setCategory:AVAudioSessionCategoryPlayAndRecord
                                 withOptions:flagOptions
                                       error:&error];
    if(resL == FALSE)
    {
        //XThrowIfError((OSStatus)error.code, "couldn't set session's audio category");
        [self displayInfo:@"setCategory:AVAudioSessionCategoryPlayAndRecord failed!"];
        res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
    }
    
    // ====================================================
    // Request record permissions
    // ====================================================
    if([sessionInstance respondsToSelector:@selector(requestRecordPermission:)])
    {
        [sessionInstance requestRecordPermission:^(BOOL granted)
         {
             if(granted)
             {
                 [self displayInfo:@"Access to mic granted!"];
             }
             else
             {
                 [self displayInfo:@"Access to mic NOT granted!"];
             }
         }];
        
    }
    
    // =================================================
    // activate the audio session
    // =================================================
    resL = [sessionInstance setActive:YES error:&error];
    if(resL == FALSE)
    {
        [self displayInfo:@"setCategory:AVAudioSessionCategoryPlayAndRecord failed!"];
        //XThrowIfError((OSStatus)error.code, "couldn't set session active");
        res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
    }
    
    if(portName != NULL)
    {
        [self displayInfo:[@"Searching for port with name " stringByAppendingString:portName]];
        
        // ======================================
        // Activate input port
        // ======================================
        iPorts = sessionInstance.availableInputs;
        if(iPorts)
        {
            int idFound = -1;
            for(i = 0; i < iPorts.count; i++)
            {
                AVAudioSessionPortDescription* oneElm = [iPorts objectAtIndex:i];
                
                [self displayInfo:[NSString stringWithFormat:@"Option %i: %@", i, oneElm.portName]];
                if([portName isEqualToString:oneElm.portName])
                {
                    [self displayInfo:@"Found it!"];
                    idFound = i;
                }
            }
            if(idFound >= 0)
            {
                AVAudioSessionPortDescription* oneElm = [iPorts objectAtIndex:idFound];
                resL = [sessionInstance setPreferredInput:oneElm error:&error];
                if(resL == FALSE)
                {
                    res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
                    [self displayInfo:@"setPreferredInput failed!"];
                }
            }
            else
            {
                res = JVX_ERROR_ELEMENT_NOT_FOUND;
            }
        }
        else
        {
            JVX_ASSERT(0, @"JVXAP0");
        }
    }

    return(res);
}

- (jvxErrorType) deactivateAudioSession;
{
    jvxErrorType res = JVX_NO_ERROR;
    BOOL resL = FALSE;
    NSError* error;
    
    // Deactivate the audio session.
    // If we do not deactivate, we may not be able to set the samplerate and bufersize in the following
    resL = [sessionInstance setActive:NO error:&error];
    if(resL == FALSE)
    {
        res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
    }
    return(res);
}

- (jvxErrorType) get_number_of_ports:(jvxSize*)num
{
    if(state_machine.theState >= JVX_STATE_SELECTED)
    {
        if(num)
        {
            *num = lstExposedDevices.count;
        }
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) get_description_port:(jvxSize) idx
                      returnString:(NSString **)nm
{

    if(state_machine.theState >= JVX_STATE_SELECTED)
    {
        if(idx < lstExposedDevices.count)
        {
            if(nm)
            {
                *nm = [lstExposedDevices objectAtIndex:idx];
            }
            return(JVX_NO_ERROR);
        }
        return(JVX_ERROR_ELEMENT_NOT_FOUND);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType)terminateAudioTechnology
{
    jvxErrorType res = JVX_NO_ERROR;
    BOOL resL = FALSE;
    NSError *error;
    
    if(state_machine.theState == JVX_STATE_PROCESSING)
    {
        res = [self stopAudio];
    }
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        res = [self terminateAudioDevice];
    }
    if(state_machine.theState == JVX_STATE_SELECTED)
    {
        
        resL = [sessionInstance setActive:FALSE error:&error];
        if(resL == FALSE)
        {
            [self displayInfo:@"setCategory:AVAudioSessionCategoryPlayAndRecord failed!"];
            //XThrowIfError((OSStatus)error.code, "couldn't set session active");
        }

        bwd_references.callback_fc = NULL;
        bwd_references.callback_priv = NULL;
        
        /* Rebuild list of devices*/
        sessionInstance = NULL;
         
        state_machine.theState = JVX_STATE_INIT;
    }
    return(res);
}

- (jvxErrorType) initAudioDevice: (jvxSize) idDevice
{
    BOOL resL = FALSE;
    jvxErrorType res = JVX_NO_ERROR;
    NSError *error = nil;
    NSString* reportTxt;
    int idx, jdx;
    NSArray <AVAudioSessionDataSourceDescription *> *iSources = nil;
    NSArray <AVAudioSessionDataSourceDescription *> *oSources = nil;

    if(state_machine.theState == JVX_STATE_SELECTED)
    {
        NSString* portName = [lstExposedDevices objectAtIndex:idDevice];
        idPortDeviceName = idDevice;
        
        // Activate the audio session.
        //resL = [sessionInstance setActive:YES error:&error];
        res = [self activateAudioSession:portName];

        
        // ======================================
        // Detect the number of available inputs and outputs and derive logical devices
        // ======================================
        [self findInputOutputOptions];

        // ======================================
        // Use very first entry in routing options input and output
        // ======================================
        idInputOptionSelected = 0;
        idOutputOptionSelected = 0;
        
        // Select default routing
        res = [self activateInputOptionId_core:idInputOptionSelected withError:&error];
        JVX_ASSERT((res == JVX_NO_ERROR), @"JVXAP1");
        
        res = [self activateOutputOptionId_core:idOutputOptionSelected withError:&error];
        JVX_ASSERT((res == JVX_NO_ERROR), @"JVXAP2");
        
        // ======================================
        // Derive number of channels
        // ======================================
        [self currentRoutingChannels];

        
        if(sessionInstance.inputAvailable)
        {
            [self displayInfo:@"Input available"];
        }
        
        
        if(sessionInstance.otherAudioPlaying)
        {
            [self displayInfo:@"Other Audio playing"];
        }
        
        // =============================================================
        long numInChannels = [sessionInstance inputNumberOfChannels];
        [self displayInfo:[NSString stringWithFormat:@"Num in chans: %li", numInChannels]];
        
        long numInChannelsMax = [sessionInstance maximumInputNumberOfChannels];
        [self displayInfo:[NSString stringWithFormat:@"Max num in chans: %li", numInChannelsMax]];
        
        resL = [sessionInstance setPreferredInputNumberOfChannels:numInChannelsMax error:&error];
                                                           
        long numInChannelsPref = [sessionInstance preferredInputNumberOfChannels];
        [self displayInfo:[NSString stringWithFormat:@"Pref num in chans: %li", numInChannelsPref]];
        if(numInChannelsPref != inputChannels.count)
        {
            NSLog(@"Channel name mismatch, rebuilding channel name base");
            NSString* newname = @"channel#";
            if(inputChannels.count == 1)
            {
                newname = [inputChannels objectAtIndex:0];
            }
            [inputChannels removeAllObjects];
            for(idx = 0; idx < numInChannelsPref; idx++)
            {
                newname = [newname stringByAppendingString:[NSString stringWithFormat:@"%i", idx]];
                NSLog(@"New channel name: %@", newname);
                [inputChannels addObject:newname];
            }
        }

        
        // =============================================================
        long numOutChannels = [sessionInstance outputNumberOfChannels];
        [self displayInfo:[NSString stringWithFormat:@"Num out chans: %li", numOutChannels]];
        
        long numOutChannelsMax = [sessionInstance maximumOutputNumberOfChannels];
        [self displayInfo:[NSString stringWithFormat:@"Max num out chans: %li", numOutChannelsMax]];
        
        resL = [sessionInstance setPreferredOutputNumberOfChannels:numOutChannelsMax error:&error];
        long numOutChannelsPref = [sessionInstance preferredOutputNumberOfChannels];
        
        [self displayInfo:[NSString stringWithFormat:@"Pref num out chans: %li", numOutChannelsPref]];
        if(numOutChannelsPref != outputChannels.count)
        {
            NSLog(@"Channel name mismatch, rebuilding channel name base");
            NSString* newname = @"channel#";
            if(outputChannels.count == 1)
            {
                newname = [outputChannels objectAtIndex:0];
            }
            [outputChannels removeAllObjects];
            for(idx = 0; idx < numOutChannelsPref; idx++)
            {
                newname = [newname stringByAppendingString:[NSString stringWithFormat:@"%i", idx]];
                NSLog(@"New channel name: %@", newname);
                [outputChannels addObject:newname];
            }
        }

        
        
        // This to be done on device activation
        // =================================================
        // Obtain samplerates
        // =================================================
        long sRateTest = -1;
        idx = 0;
        while(1)
        {
            double setThisVal = (double)jvxUsefulSRates[idx];
            if(setThisVal >= 0)
            {
                resL = [sessionInstance setPreferredSampleRate:setThisVal error:&error];
                if(resL == FALSE)
                {
                    [self displayInfo:[NSString stringWithFormat:@"setPreferredSampleRate for value %f failed!", setThisVal]];
                    //XThrowIfError((OSStatus)error.code, "couldn't set session's preferred sample rate");
                }
                if(setThisVal == sessionInstance.sampleRate)
                {
                    // Test with the greatest samplerate
                    sRateTest = setThisVal;
                    [detectedSamplerates addObject:[NSNumber numberWithDouble:setThisVal]];
                }
                idx ++;
            }
            else
            {
                break;
            }
        }
        
        // Finally set the
        resL = [sessionInstance setPreferredSampleRate:sRateTest error:&error];
        if(resL == FALSE)
        {
            [self displayInfo:[NSString stringWithFormat:@"Final setting of setPreferredSampleRate for value %li failed!", sRateTest]];
            //XThrowIfError((OSStatus)error.code, "couldn't set session's preferred sample rate");
        }
        
        // =================================================
        // Obtain buffersizes
        // =================================================
        idx = 0;
        while(1)
        {
            long testme = jvxUsefulBSizes[idx];
            if(testme > 0)
            {
                NSTimeInterval ioBufferDuration = (double)testme / (double) sRateTest;
                resL = [sessionInstance setPreferredIOBufferDuration:ioBufferDuration error:&error];
                if(resL == FALSE)
                {
                    [self displayInfo:[NSString stringWithFormat:@"setPreferredIOBufferDuration for value %f failed!", ioBufferDuration]];
                    //XThrowIfError((OSStatus)error.code, "couldn't set session's I/O buffer duration");
                }
                ioBufferDuration = sessionInstance.IOBufferDuration;
                long gotthis = round(ioBufferDuration * sRateTest);
                if(gotthis == testme)
                {
                    [detectedBuffersizes addObject:[NSNumber numberWithLong:testme]];
                }
                idx ++;
            }
            else
            {
                break;
            }
        }
        
        [self displayInfo:@"Buffersizes:"];
        for(idx = 0; idx < detectedBuffersizes.count; idx++)
        {
            [self displayInfo:[NSString stringWithFormat:@"+ Bsize #%i: %li", idx, [[detectedBuffersizes objectAtIndex:idx] longValue]]];
        }
        [self displayInfo:@"Samplerates:"];
        for(idx = 0; idx < detectedSamplerates.count; idx++)
        {
            [self displayInfo:[NSString stringWithFormat:@"+ Srate #%i: %f", idx, [[detectedSamplerates objectAtIndex:idx] doubleValue]]];
        }

        // Deactivate the audio session.
        // If we do not deactivate, we may not be able to set the samplerate and bufersize in the following
        //resL = [sessionInstance setActive:NO error:&error];
        [self deactivateAudioSession];
        
        state_machine.theState = JVX_STATE_ACTIVE;
        
        res = JVX_NO_ERROR;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
        
    }
    return(res);
}

- (jvxErrorType) terminateAudioDevice
{
    jvxErrorType res = JVX_NO_ERROR;
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        [inputOptions removeAllObjects];
        [outputOptions removeAllObjects];
        [inputChannels removeAllObjects];
        [outputChannels removeAllObjects];
        [detectedSamplerates removeAllObjects];
        [detectedBuffersizes removeAllObjects];

        state_machine.theState = JVX_STATE_SELECTED;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);

}

- (jvxErrorType) startAudio:(callbackProcessOneBuffer)theExtCallback
             withVoidBwdRef:(jvxHandle *)priv
  withLinkDataInDescriptorPtr:(jvxLinkDataDescriptor*) theData_in
  withLinkDataOutDescriptorPtr:(jvxLinkDataDescriptor*) theData_out
   withInputChannelSelector:(jvxBitField) inChannels
 withOutputChannelsSelector:(jvxBitField) outChannels
       withMaxInputChannels:(jvxSize)maxInChans
      withMaxOutputChannels:(jvxSize)maxOutChans
{
    int i;
    BOOL resL = FALSE;
    jvxErrorType res = JVX_NO_ERROR;
    NSError *error = nil;
    OSStatus errStat;
    
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        NSString* portName = [lstExposedDevices objectAtIndex:idPortDeviceName];
        
        // Reactivate audio session
        //resL = [sessionInstance setActive:YES error:&error];
        res = [self activateAudioSession:portName]; // <- always, the current route is the
        
        [self currentRoutingChannels];
        
        [self displayInfo:@"10"];

        myPrivateData = new privateAudioProcessor;
        myPrivateData->audioUnitInUse = NULL;
        myPrivateData->inChannelMask = inChannels;
        myPrivateData->outChannelMask = outChannels;
        myPrivateData->ioData_input = NULL;
        myPrivateData->theData_in = theData_in;
        myPrivateData->theData_out = theData_out;
        myPrivateData->bufferSwitchCallback = theExtCallback;
        myPrivateData->callback_priv = priv;
        myPrivateData->numUsedChannelsInput = 0;
        myPrivateData->numUsedChannelsOutput = 0;

        // Final setting of samplerate and buffersize
        long bSizeUsed = 0;
        double sRateUsed = 0;
        /*if(
           (detectedBuffersizes.count > 0) &&
           (detectedSamplerates.count > 0) &&
           (idSRate < detectedSamplerates.count) &&
            (idBSize < detectedBuffersizes.count))
        {*/
            //bSizeUsed = [[detectedBuffersizes objectAtIndex:idBSize] longValue];
            //sRateUsed = [[detectedSamplerates objectAtIndex:idSRate] doubleValue];
        if(theData_in)
        {
            bSizeUsed = theData_in->con_params.buffersize;
            sRateUsed = theData_in->con_params.rate;
        }
        else
        {
            bSizeUsed = theData_out->con_params.buffersize;
            sRateUsed = theData_out->con_params.rate;
        }
        
        // TODO: Make sure that bsizeand srate are among the detected props
        NSTimeInterval bufferDuration = (double)bSizeUsed / sRateUsed;
        
        resL = [sessionInstance setPreferredSampleRate:sRateUsed error:&error];
        if(resL == FALSE)
        {
            [self displayInfo:@"setCategory:AVAudioSessionCategoryPlayAndRecord failed!"];
            //XThrowIfError((OSStatus)error.code, "couldn't set session's I/O buffer duration");
            res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
        }
        
        resL = [sessionInstance setPreferredIOBufferDuration:bufferDuration error:&error];
        if(resL == FALSE)
        {
            [self displayInfo:@"setCategory:AVAudioSessionCategoryPlayAndRecord failed!"];
            //XThrowIfError((OSStatus)error.code, "couldn't set session's I/O buffer duration");
            res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
        }
        /*
         }
        else
        {
            assert(0);
        }
        */
        
        myPrivateData->noInput = false;
        myPrivateData->inputDummy = NULL;
        if(res == JVX_NO_ERROR)
        {
#ifdef JVX_NUMBER_INPUT_CHANNELS_MIN
            myPrivateData->numUsedChannelsInput = jvx_bitfieldSelectionMax(myPrivateData->inChannelMask);
#else
            myPrivateData->numUsedChannelsInput = maxInChans;
#endif
            
            // Check number of input channels
            long numInChannelsMax = [sessionInstance maximumInputNumberOfChannels];
            if(myPrivateData->numUsedChannelsInput <= numInChannelsMax)
            {
                if(myPrivateData->numUsedChannelsInput > 0)
                {
                    resL = [sessionInstance setPreferredInputNumberOfChannels:myPrivateData->numUsedChannelsInput error:&error];
                    long numInChannelsPref = [sessionInstance inputNumberOfChannels];
                    if(numInChannelsPref != myPrivateData->numUsedChannelsInput)
                    {
                        res = JVX_ERROR_INTERNAL;
                        [self displayInfo:@"setPreferredInputNumberOfChannels failed!"];
                    }
                }
                else
                {
                    resL = [sessionInstance setPreferredInputNumberOfChannels:1 error:&error]; // one dummy channel
                    long numInChannelsPref = [sessionInstance inputNumberOfChannels];
                    if(numInChannelsPref != 1)
                    {
                        res = JVX_ERROR_INTERNAL;
                        [self displayInfo:@"setPreferredInputNumberOfChannels failed!"];
                    }
                    myPrivateData->noInput = true;
                }
            }
            else
            {
                res = JVX_ERROR_INVALID_SETTING;
                [self displayInfo:[NSString stringWithFormat:@"Channel mismatch: %li vs %li", myPrivateData->numUsedChannelsInput, numInChannelsMax]];
            }
        }
        
        myPrivateData->noOutput = false;
        myPrivateData->outputDummy = NULL;

        if(res == JVX_NO_ERROR)
        {
#ifdef JVX_NUMBER_OUTPUT_CHANNELS_MIN
            myPrivateData->numUsedChannelsOutput = jvx_bitfieldSelectionMax(myPrivateData->outChannelMask);
#else
            myPrivateData->numUsedChannelsOutput = maxOutChans;
#endif
            long numOutChannelsMax = [sessionInstance maximumOutputNumberOfChannels];
            if(myPrivateData->numUsedChannelsOutput <= numOutChannelsMax)
            {
                if(myPrivateData->numUsedChannelsOutput > 0)
                {
                    resL = [sessionInstance setPreferredOutputNumberOfChannels:myPrivateData->numUsedChannelsOutput error:&error];
                    long numOutChannelsPref = [sessionInstance outputNumberOfChannels];
                    if(numOutChannelsPref != myPrivateData->numUsedChannelsOutput)
                    {
                        res = JVX_ERROR_INTERNAL;
                        [self displayInfo:@"setPreferredOutputNumberOfChannels failed!"];
                    }
                }
                else
                {
                    myPrivateData->noOutput = true;
                }
            }
            else
            {
                res = JVX_ERROR_INVALID_SETTING;
            }
        }
        
        if(res == JVX_NO_ERROR)
        {
            // Now it is the time to set the input and output option at startup
            [self activateInputOptionId_core:idInputOptionSelected withError:&error];
            [self activateOutputOptionId_core:idOutputOptionSelected withError:&error];
        }
        
        if(res == JVX_NO_ERROR)
        {
            [self displayInfo:@"Proc Params"];
            [self displayInfo:[NSString stringWithFormat:@"srate: %f", sessionInstance.sampleRate]];
            [self displayInfo:[NSString stringWithFormat:@"bsize in secs %f", sessionInstance.IOBufferDuration]];
            [self displayInfo:[NSString stringWithFormat:@"in channels %li", myPrivateData->numUsedChannelsInput]];
            [self displayInfo:[NSString stringWithFormat:@"out channels %li", myPrivateData->numUsedChannelsOutput]];
            
        }
        
        if(res == JVX_NO_ERROR)
        {
            /* Audio unit coupling */
            AudioComponentDescription desc;
            desc.componentType = kAudioUnitType_Output;
            desc.componentSubType = kAudioUnitSubType_RemoteIO;
            desc.componentManufacturer = kAudioUnitManufacturer_Apple;
            desc.componentFlags = 0;
            desc.componentFlagsMask = 0;
            
            AudioComponent comp = AudioComponentFindNext(NULL, &desc);
            errStat = AudioComponentInstanceNew(comp, &_rioUnit);
            if(errStat != errSecSuccess)
            {
                [self displayInfo:@"couldn't create a new instance of AURemoteIO"];
                res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
            }
        }
        
        //myLink.audioUnitInUse = _rioUnit;
        
        //  Enable input and output on AURemoteIO
        //  Input is enabled on the input scope of the input element
        //  Output is enabled on the output scope of the output element
        if(res == JVX_NO_ERROR)
        {
            UInt32 one = 1;
            
            if(!myPrivateData->noInput)
            {
                errStat = AudioUnitSetProperty(_rioUnit,
                                           kAudioOutputUnitProperty_EnableIO,
                                           kAudioUnitScope_Input, 1,
                                           &one, sizeof(one));
                if(errStat != noErr)
                {
                    [self displayInfo:@"could not enable input on AURemoteIO"];
                    res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
                }
            }
            
            if(!myPrivateData->noOutput)
            {
                errStat = AudioUnitSetProperty(_rioUnit,
                                               kAudioOutputUnitProperty_EnableIO,
                                               kAudioUnitScope_Output, 0, &one,
                                               sizeof(one));
                if(errStat != noErr)
                {
                    [self displayInfo:@"could not enable output on AURemoteIO"];
                    res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
                }
            }
            
            // Explicitly set the input and output client formats
            CAStreamBasicDescription ioFormatI = CAStreamBasicDescription(sRateUsed, (UInt32)myPrivateData->numUsedChannelsInput, CAStreamBasicDescription::kPCMFormatFloat32, false);
            CAStreamBasicDescription ioFormatO = CAStreamBasicDescription(sRateUsed, (UInt32)myPrivateData->numUsedChannelsOutput, CAStreamBasicDescription::kPCMFormatFloat32, false);
            
            if(!myPrivateData->noInput)
            {
                errStat = AudioUnitSetProperty(_rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &ioFormatI, sizeof(ioFormatI));
                if(errStat != noErr)
                {
                    [self displayInfo:@"couldn't set the input client format on AURemoteIO"];
                    res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
                }
            }
            
            if(!myPrivateData->noOutput)
            {
                errStat = AudioUnitSetProperty(_rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &ioFormatO, sizeof(ioFormatO));
                if(errStat != noErr)
                {
                    [self displayInfo:@"couldn't set the output client format on AURemoteIO"];
                    res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
                }
            }
            
            // Set the MaximumFramesPerSlice property. This property is used to describe to an audio unit the maximum number
            // of samples it will be asked to produce on any single given call to AudioUnitRender
            UInt32 maxFramesPerSlice = (UInt32)bSizeUsed;
            errStat = AudioUnitSetProperty(_rioUnit,
                                           kAudioUnitProperty_MaximumFramesPerSlice,
                                           kAudioUnitScope_Global, 0, &maxFramesPerSlice,
                                           sizeof(UInt32));
            if(errStat != noErr)
            {
                [self displayInfo:@"couldn't set max frames per slice on AURemoteIO"];
                res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
            }
            
            // Get the property value back from AURemoteIO. We are going to use this value to allocate buffers accordingly
            UInt32 propSize = sizeof(UInt32);
            errStat = AudioUnitGetProperty(_rioUnit,
                                           kAudioUnitProperty_MaximumFramesPerSlice,
                                           kAudioUnitScope_Global, 0,
                                           &maxFramesPerSlice, &propSize);
            if(errStat != noErr)
            {
                [self displayInfo:@"couldn't get max frames per slice on AURemoteIO"];
                res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
            }
        }
        
        if(res == JVX_NO_ERROR)
        {
           
            myPrivateData->audioUnitInUse = &_rioUnit;

            // Set the render callback on AURemoteIO
            if(!myPrivateData->noOutput)
            {
                AURenderCallbackStruct renderCallback;
                renderCallback.inputProc = performRender;
                renderCallback.inputProcRefCon = myPrivateData;
                errStat = AudioUnitSetProperty(_rioUnit,
                                               kAudioUnitProperty_SetRenderCallback,
                                               kAudioUnitScope_Input, 0, &renderCallback,
                                               sizeof(renderCallback));
                if(errStat != noErr)
                {
                    [self displayInfo:@"couldn't set render callback on AURemoteIO"];
                }
            }
            else
            {
                AURenderCallbackStruct captureCallback;
                captureCallback.inputProc = performCapture;
                captureCallback.inputProcRefCon = myPrivateData;
                errStat = AudioUnitSetProperty(_rioUnit,
                                               kAudioUnitProperty_SetRenderCallback,
                                               kAudioUnitScope_Input, 0, &captureCallback,
                                               sizeof(captureCallback));
                if(errStat != noErr)
                {
                    [self displayInfo:@"couldn't set render callback on AURemoteIO"];
                }

            }
        }
        
        if(res == JVX_NO_ERROR)
        {
            int cnt = 0;
            
            jvxSize sz = offsetof(AudioBufferList,mBuffers) + myPrivateData->numUsedChannelsInput + sizeof(AudioBufferList);
            AudioBufferList* newABL = (AudioBufferList*)malloc(sz);
            newABL->mNumberBuffers = (UInt32)myPrivateData->numUsedChannelsInput;

            for(i = 0; i < myPrivateData->numUsedChannelsInput; i++)
            {
                newABL->mBuffers[i].mNumberChannels = 1;
                newABL->mBuffers[i].mDataByteSize = (UInt32)
                    (myPrivateData->theData_in->con_params.buffersize *
                     jvxDataFormat_size[myPrivateData->theData_in->con_params.format]);

                if(jvx_bitTest(myPrivateData->inChannelMask, i))
                {
                    // Create cross reference
                    newABL->mBuffers[i].mData = myPrivateData->theData_in->con_data.buffers[0][cnt];
                    cnt++;
                }
                else
                {
                    newABL->mBuffers[i].mData = malloc(newABL->mBuffers[i].mDataByteSize);
                }
            }
            
            myPrivateData->ioData_input = newABL;
        }

        if(res == JVX_NO_ERROR)
        {
            // Initialize the AURemoteIO instance
            errStat = AudioUnitInitialize(_rioUnit);
            if(errStat != noErr)
            {
                [self displayInfo:@"couldn't initialize AURemoteIO instance"];
            }
        }
        
        if(res == JVX_NO_ERROR)
        {
            errStat = AudioOutputUnitStart(_rioUnit);
            if(errStat != noErr)
            {
                [self displayInfo:[NSString stringWithFormat:@"couldn't start AURemoteIO: %d", (int)errStat]];
            }
        }
        
        state_machine.theState = JVX_STATE_PROCESSING;

        if(res != JVX_NO_ERROR)
        {
            [self stopAudio];
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

/*
 * Terminate and shutdown audio processing
 *========================================================*/
- (jvxErrorType)stopAudio
{
    int i;
    BOOL resL = FALSE;
    OSStatus errStat;
    jvxErrorType res = JVX_NO_ERROR;
    NSError* error;
    
    if(state_machine.theState == JVX_STATE_PROCESSING)
    {
        errStat = AudioOutputUnitStop(_rioUnit);
        if(errStat != noErr)
        {
            [self displayInfo:[NSString stringWithFormat:@"couldn't stop AURemoteIO: %d", (int)errStat]];
        }

        // Terminate the AURemoteIO instance
        errStat = AudioUnitUninitialize(_rioUnit);
        if(errStat != noErr)
        {
            [self displayInfo:[NSString stringWithFormat:@"couldn't uninitialize AURemoteIO instance"]];
        }

        AudioComponentInstanceDispose((_rioUnit));

        if(res == JVX_NO_ERROR)
        {
            for(i = 0; i < myPrivateData->numUsedChannelsInput; i++)
            {
                if(!(jvx_bitTest(myPrivateData->inChannelMask, i)))
                {
                    // Only free the allocated buffers, the other buffers are cross-referenced
                    free(myPrivateData->ioData_input->mBuffers[i].mData);
                }
            }
            
            free(myPrivateData->ioData_input);
        }

        delete myPrivateData;
        myPrivateData = NULL;
        
        [self deactivateAudioSession];
        //resL = [sessionInstance setActive:NO error:&error];
        
        state_machine.theState = JVX_STATE_ACTIVE;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

/*
 * Return current audio processing status
 *========================================================*/
- (jvxErrorType)status:(jvxState*) statusOnReturn;
{
    if(statusOnReturn)
    {
        *statusOnReturn = state_machine.theState;
    }
    return(JVX_NO_ERROR);
}

/*
 * Function to return the current status of the speaker selection
 *=======================================================*/
-(jvxErrorType) statusActivateSpeaker: (jvxBool*) status
{
    jvxErrorType res = JVX_NO_ERROR;
    if(state_machine.theState >= JVX_STATE_SELECTED)
    {
        if(status)
        {
            *status = state_machine.speakerIsOn;
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

/*
 * Find the available input ports, outputs are not available.
 * Output ports can not be listed.
 */
- (void) findInputPorts
{
    int idx, jdx;
    NSArray <AVAudioSessionPortDescription *>* iPorts = nil;
    BOOL resL;
    NSError* error;
    
    iPorts = sessionInstance.availableInputs;
    
    if(iPorts)
    {
        [self displayInfo:@"Input Ports Scan"];
        for(idx = 0; idx < iPorts.count; idx++)
        {
            [self displayInfo:[NSString stringWithFormat:@"+ Input %i", idx]];
            AVAudioSessionPortDescription* oneElm = [iPorts objectAtIndex:idx];
            NSString* portName = oneElm.portName;
            
            [self displayInfo:[NSString stringWithFormat:@"++ Portname %@", portName]];
            [self displayInfo:[NSString stringWithFormat:@"++ PortType %@", oneElm.portType]];
            
            [lstExposedDevices addObject:portName];
        }
    }
}

- (void) findInputOutputOptions
{
    int idx, jdx;
    NSArray <AVAudioSessionPortDescription *>* iPorts = nil;
    NSArray <AVAudioSessionDataSourceDescription *> *iSources = nil;
    NSArray <AVAudioSessionPortDescription *>* oPorts = nil;
    NSArray <AVAudioSessionDataSourceDescription *> *oSources = nil;
    BOOL resL;
    NSError* error;
    
     AVAudioSessionRouteDescription *currentRoute = sessionInstance.currentRoute;
    //prefiPort = sessionInstance.preferredInput;
    
    [inputOptions removeAllObjects];
    [outputOptions removeAllObjects];
    
    if(currentRoute)
    {
        iPorts = [currentRoute inputs];
        
        [self displayInfo:@"Input Ports"];
        for(idx = 0; idx < iPorts.count; idx++)
        {
            [self displayInfo:[NSString stringWithFormat:@"+ Input %i", idx]];
            AVAudioSessionPortDescription* oneElm = [iPorts objectAtIndex:idx];
            
            [self displayInfo:[NSString stringWithFormat:@"++ Portname %@", oneElm.portName]];
            [self displayInfo:[NSString stringWithFormat:@"++ PortType %@", oneElm.portType]];
            NSArray <AVAudioSessionChannelDescription *>* chans = [oneElm channels];
            for(jdx = 0; jdx < chans.count; jdx++)
            {
                AVAudioSessionChannelDescription* theChan = [chans objectAtIndex:jdx];
                [self displayInfo:[NSString stringWithFormat:@"+++ Channel name: %@", theChan.channelName]];
            }
        }

        oPorts = currentRoute.outputs;
/*        if(oSources)
    {
        [self displayInfo:@"Output Data Sources"];
        AVAudioSessionDataSourceDescription *theOSource = sessionInstance.outputDataSource;
        for(idx = 0; idx < oSources.count; idx++)
        {
            theNewDevice = [[jvxOneDevice alloc] init];
            [theNewDevice setName:theOSource.dataSourceName
                       withPortId:-1
                     withSourceId:idx];
            [outputOptions addObject:theNewDevice];
            
            [self displayInfo:[NSString stringWithFormat:@"+ Output Data Source #%i", idx]];
            AVAudioSessionDataSourceDescription* currentSource = [oSources objectAtIndex:idx];
            if(currentSource == theOSource)
            {
                [self displayInfo:[NSString stringWithFormat:@"++ Name Data Source*: %@", currentSource.dataSourceName]];
            }
            else
            {
                [self displayInfo:[NSString stringWithFormat:@"++ Name Data Source: %@", currentSource.dataSourceName]];
            }
        }
    }
    else
    {
        theNewDevice = [[jvxOneDevice alloc] init];
        [theNewDevice setName:@"Default"
                   withPortId:-1
                 withSourceId:0];
        [outputOptions addObject:theNewDevice];
        [self displayInfo:@"No Output Data Sources"];
    }*/
    }
    
    iSources = sessionInstance.inputDataSources;
    if(iSources)
    {
        
        // Get the current source
        AVAudioSessionDataSourceDescription *theISource = sessionInstance.inputDataSource;
        
        [self displayInfo:@"++++ Corr. input Data Sources"];
        for(jdx = 0; jdx < iSources.count; jdx++)
        {
            [self displayInfo:[NSString stringWithFormat:@"++++ Input Data Source #%i", jdx]];
            AVAudioSessionDataSourceDescription* currentSource = [iSources objectAtIndex:jdx];
            
            oneDataSourceOption* theNewSource = [[oneDataSourceOption alloc] init];
            
            theNewSource.name = currentSource.dataSourceName;
            theNewSource.iPortId = idx;
            theNewSource.iSourceId = jdx;
            [inputOptions addObject:theNewSource];
            
            if(currentSource == theISource)
            {
                [self displayInfo:[NSString stringWithFormat:@"+++++ Name Data Source*: %@", currentSource.dataSourceName]];
            }
            else
            {
                [self displayInfo:[NSString stringWithFormat:@"+++++ Name Data Source: %@", currentSource.dataSourceName]];
            }
        }
    }
    else
    {
        [self displayInfo:@"No Input Data Sources" ];
        
        oneDataSourceOption* theNewSource = [[oneDataSourceOption alloc] init];
        
        theNewSource.name = @"Default";
        theNewSource.iPortId = 0;
        theNewSource.iSourceId = 0;
        
        [inputOptions addObject:theNewSource];
    }

    oSources = sessionInstance.outputDataSources;
    if(oSources)
    {
        
        // Get the current source
        AVAudioSessionDataSourceDescription *theOSource = sessionInstance.outputDataSource;
        
        [self displayInfo:@"++++ Corr. output Data Sources"];
        for(jdx = 0; jdx < oSources.count; jdx++)
        {
            [self displayInfo:[NSString stringWithFormat:@"++++ Output Data Source #%i", jdx]];
            AVAudioSessionDataSourceDescription* currentSource = [oSources objectAtIndex:jdx];
         
            oneDataSourceOption* theNewSource = [[oneDataSourceOption alloc] init];
            
            theNewSource.name = currentSource.dataSourceName;
            theNewSource.iPortId = idx;
            theNewSource.iSourceId = jdx;
            [outputOptions addObject:theNewSource];
            
            if(currentSource == theOSource)
            {
                [self displayInfo:[NSString stringWithFormat:@"+++++ Name Data Source*: %@", currentSource.dataSourceName]];
            }
            else
            {
                [self displayInfo:[NSString stringWithFormat:@"+++++ Name Data Source: %@", currentSource.dataSourceName]];
            }
        }
    }
    else
    {
        [self displayInfo:@"No Output Data Sources" ];
        
        oneDataSourceOption* theNewSource = [[oneDataSourceOption alloc] init];
        
        theNewSource.name = @"Default";
        theNewSource.iPortId = 0;
        theNewSource.iSourceId = 0;
        
        [outputOptions addObject:theNewSource];
    }

}

- (void) currentRoutingChannels
{
    int idx, jdx;
    NSArray <AVAudioSessionPortDescription *>* iPorts = nil;
    NSArray <AVAudioSessionDataSourceDescription *> *iSources = nil;
    NSArray <AVAudioSessionDataSourceDescription *> *oSources = nil;

    // Current Route
    [self displayInfo:@"++++++++ Current Route ++++++++++++"];
    AVAudioSessionRouteDescription* currentRoutePtr = [sessionInstance currentRoute];
    iPorts = currentRoutePtr.inputs;
    
    if(iPorts)
    {
        [self displayInfo:@"Input route ports"];
        for(idx = 0; idx < iPorts.count; idx++)
        {
            [self displayInfo:[NSString stringWithFormat:@"+ Input route port #%i", idx]];
            AVAudioSessionPortDescription* oneElm = [iPorts objectAtIndex:idx];
            [self displayInfo:[NSString stringWithFormat:@"++ Portname %@", oneElm.portName]];
            
            [self displayInfo:[NSString stringWithFormat:@"++ PortType %@", oneElm.portType]];
            NSArray <AVAudioSessionChannelDescription *>* chans = [oneElm channels];
            for(jdx = 0; jdx < chans.count; jdx++)
            {
                AVAudioSessionChannelDescription* theChan = [chans objectAtIndex:jdx];
                [self displayInfo:[NSString stringWithFormat:@"+++ Channel name: %@", theChan.channelName]];
                [inputChannels addObject:theChan.channelName];
            }
        }
    }
    else
    {
        [self displayInfo:@"No input route ports"];
        [inputChannels addObject:@"Channel"];
    }


    NSArray <AVAudioSessionPortDescription *>* oPorts = currentRoutePtr.outputs;
    if(oPorts)
    {
        [self displayInfo:@"Output route ports"];
        for(idx = 0; idx < oPorts.count; idx++)
        {
            [self displayInfo:[NSString stringWithFormat:@"+ Output route port #%i", idx]];
            AVAudioSessionPortDescription* oneElm = [oPorts objectAtIndex:idx];
            
            [self displayInfo:[NSString stringWithFormat:@"++ Portname %@", oneElm.portName]];
            
            [self displayInfo:[NSString stringWithFormat:@"++ PortType %@", oneElm.portType]];
            NSArray <AVAudioSessionChannelDescription *>* chans = [oneElm channels];
            for(jdx = 0; jdx < chans.count; jdx++)
            {
                AVAudioSessionChannelDescription* theChan = [chans objectAtIndex:jdx];
                [self displayInfo:[NSString stringWithFormat:@"+++ Channel name: %@", theChan.channelName]];
                [outputChannels addObject:theChan.channelName];
            }
        }
    }
    else
    {
        [self displayInfo:@"No output route ports"];
        [outputChannels addObject:@"Channel"];
    }
}


// ====================================================
// ====================================================
// ====================================================
// ====================================================
// ====================================================
// ====================================================
// ====================================================
// ====================================================
// ====================================================

/*
 * Callback entry for reporting of audio events within the main thread
 *========================================================*/

- (void)handleInMainThread:(jvxIosBwdReportEvent)cmd
              withUserInfo:(NSDictionary*)dict
{
    if(state_machine.theState > JVX_STATE_INIT)
    {
        if(bwd_references.callback_fc)
        {
            bwd_references.callback_fc(cmd, bwd_references.callback_priv, (__bridge void*)dict);
        }

        /*
         * THE FOLLOWING CODES IS PROBLEMATIC:
         * https://blackpixel.com/writing/2013/11/performselectoronmainthread-vs-dispatch-async.html
         
         switch(cmd)
         {
         case JVX_IOS_BWD_COMMAND_NEW_ROUTE:
         [ [NSNotificationCenter defaultCenter] postNotificationName:@"Report_newRoute" object:self userInfo:dict];
         break;
         case JVX_IOS_BWD_COMMAND_INTERRUPTION:
         [ [NSNotificationCenter defaultCenter] postNotificationName:@"Report_interruption" object:self userInfo:dict];
         break;
         case JVX_IOS_BWD_COMMAND_MEDIACENTER_RESET:
         [ [NSNotificationCenter defaultCenter] postNotificationName:@"Report_mediacentre_reset" object:self userInfo:dict];
         break;
         default:
         JVX_DISPLAY_INFO(@"Unexpected bwd command.");
         }
         */
    }
}

/*
 * Function to accept report of an interruption
 *========================================================*/
- (void)handleInterruption:(NSNotification *)notification
{
    NSLog(@"IOS AUDIO: Interruption");
    
#ifdef JVX_MAP_CBS_MAIN_THREAD
    // Process in main thread to allow modificatin of UI
    dispatch_async(dispatch_get_main_queue(), ^{
        [self handleInMainThread:JVX_IOS_BWD_COMMAND_INTERRUPTION withUserInfo:notification.userInfo];
    });
#else
    [self handleInMainThread:JVX_IOS_BWD_COMMAND_INTERRUPTION withUserInfo:notification.userInfo];
#endif
    
    /*
     try {
     UInt8 theInterruptionType = [[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue];
     JVX_DISPLAY_INFO(@"Session interrupted > --- %s ---\n", theInterruptionType == AVAudioSessionInterruptionTypeBegan ? "Begin Interruption" : "End Interruption");
     
     if (theInterruptionType == AVAudioSessionInterruptionTypeBegan) {
     [self stopIOUnit];
     }
     
     if (theInterruptionType == AVAudioSessionInterruptionTypeEnded) {
     // make sure to activate the session
     NSError *error = nil;
     [[AVAudioSession sharedInstance] setActive:YES error:&error];
     if (nil != error) JVX_DISPLAY_INFO(@"AVAudioSession set active failed with error: %@", error);
     
     [self startIOUnit];
     }
     } catch (CAXException e) {
     char buf[256];
     fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
     }
     */
}

/*
 * Function to accept report of a route changed event
 *========================================================*/
- (void)handleRouteChange:(NSNotification *)notification
{
    NSLog(@"IOS AUDIO: Route change event");
    
#ifdef JVX_MAP_CBS_MAIN_THREAD
    // Process in main thread to allow modificatin of UI
    dispatch_async(dispatch_get_main_queue(), ^{
        [self handleInMainThread:JVX_IOS_BWD_COMMAND_NEW_ROUTE withUserInfo:notification.userInfo];
    });
#else
    [self handleInMainThread:JVX_IOS_BWD_COMMAND_NEW_ROUTE withUserInfo:notification.userInfo];
#endif
    
}

/*
 * Function to accect a media server reset event
 *========================================================*/
- (void)handleMediaServerReset:(NSNotification *)notification
{
    NSLog(@"IOS AUDIO: Media Server reset");
    
    // Process in main thread to allow modificatin of UI
#ifdef JVX_MAP_CBS_MAIN_THREAD
    dispatch_async(dispatch_get_main_queue(), ^{
        [self handleInMainThread:JVX_IOS_BWD_COMMAND_MEDIACENTER_RESET withUserInfo:notification.userInfo];
    });
#else
    [self handleInMainThread:JVX_IOS_BWD_COMMAND_MEDIACENTER_RESET withUserInfo:notification.userInfo];
#endif
    
    // What to do in this case? I have never seen this happen..
    
    /*
     JVX_DISPLAY_INFO(@"Media server has reset");
     _audioChainIsBeingReconstructed = YES;
     
     usleep(25000); //wait here for some time to ensure that we don't delete these objects while they are being accessed elsewhere
     
     // rebuild the audio chain
     delete _bufferManager;      _bufferManager = NULL;
     delete _dcRejectionFilter;  _dcRejectionFilter = NULL;
     [_audioPlayer release]; _audioPlayer = nil;
     
     [self setupAudioChain];
     [self startIOUnit];
     
     _audioChainIsBeingReconstructed = NO;
     */
}


// ===========================================================
// Functions to interact with RTProc system
// ===========================================================

- (jvxErrorType) get_number_input_channels:(jvxSize*) num
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(num)
            *num = inputChannels.count;
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_WRONG_STATE);
}
- (jvxErrorType) get_description_input_channel:(jvxSize) idx
                                  returnString:(NSString**)nm
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(idx < inputChannels.count)
        {
            if(nm)
            {
                *nm = [inputChannels objectAtIndex:idx];
            }
            return(JVX_NO_ERROR);
        }
        return(JVX_ERROR_ELEMENT_NOT_FOUND);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) get_number_output_channels:(jvxSize*) num
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(num)
            *num = outputChannels.count;
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_WRONG_STATE);
}
- (jvxErrorType) get_description_output_channel:(jvxSize) idx
                                  returnString:(NSString**)nm
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(idx < outputChannels.count)
        {
            if(nm)
            {
                *nm = [outputChannels objectAtIndex:idx];
            }
            return(JVX_NO_ERROR);
        }
        return(JVX_ERROR_ELEMENT_NOT_FOUND);
    }
    return(JVX_ERROR_WRONG_STATE);
}


- (jvxErrorType) get_number_samplerates:(jvxSize*) num
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(num)
            *num = detectedSamplerates.count;
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) get_samplerate:(jvxSize) idx
                    returnValue:(float*)val
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(idx < detectedSamplerates.count)
        {
            if(val)
            {
                *val = [[detectedSamplerates objectAtIndex:idx] doubleValue];
            }
            return(JVX_NO_ERROR);
        }
        return(JVX_ERROR_ELEMENT_NOT_FOUND);
    }
    return(JVX_ERROR_WRONG_STATE);
}


- (jvxErrorType) get_number_buffersizes:(jvxSize*) num
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(num)
            *num = detectedBuffersizes.count;
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) get_buffersize:(jvxSize) idx
                    returnValue:(jvxSize*)val
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(idx < detectedBuffersizes.count)
        {
            if(val)
            {
                *val = [[detectedBuffersizes objectAtIndex:idx] longValue];
            }
            return(JVX_NO_ERROR);
        }
        return(JVX_ERROR_ELEMENT_NOT_FOUND);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) get_number_dataformats:(jvxSize *)num
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(num)
            *num = 2;
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_WRONG_STATE);
}


- (jvxErrorType) get_dataformat:(jvxSize) idx
                    returnValue:(jvxDataFormat*)form
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(form)
        {
            switch(idx)
            {
                case 0:
                    *form = JVX_DATAFORMAT_DATA;
                    break;
                case 1:
                    *form = JVX_DATAFORMAT_16BIT_LE;
                    break;
                default:
                    return(JVX_ERROR_ELEMENT_NOT_FOUND);
            }
            return(JVX_NO_ERROR);
        }
        return(JVX_ERROR_ELEMENT_NOT_FOUND);
    }
    return(JVX_ERROR_WRONG_STATE);
}


- (jvxErrorType) get_number_input_sources:(jvxSize*) num
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(num)
            *num = inputOptions.count;
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) get_input_source:(jvxSize) idx
                     returnValue:(NSString**)name
                   isSourceActive:(jvxBool*)isActive
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(idx < inputOptions.count)
        {
            if(name)
            {
                oneDataSourceOption* theDev = NULL;
                theDev = (oneDataSourceOption*)[inputOptions objectAtIndex:idx];
                *name = theDev.name;
            }
            if(isActive)
            {
                if(idx == idInputOptionSelected)
                {
                    *isActive = true;
                }
                else
                {
                    *isActive = false;
                }
            }
            return(JVX_NO_ERROR);
        }
        return(JVX_ERROR_ELEMENT_NOT_FOUND);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) get_number_output_sources:(jvxSize*) num
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(num)
            *num = outputOptions.count;
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) get_output_source:(jvxSize) idx
                      returnValue:(NSString**)name
                    isSourceActive:(jvxBool*)isActive
{
    if(state_machine.theState == JVX_STATE_ACTIVE)
    {
        if(idx < outputOptions.count)
        {
            if(name)
            {
                oneDataSourceOption* theDev = NULL;
                theDev = (oneDataSourceOption*)[outputOptions objectAtIndex:idx];
                *name = theDev.name;
            }
            if(isActive)
            {
                if(idx == idOutputOptionSelected)
                {
                    *isActive = true;
                }
                else
                {
                    *isActive = false;
                }
            }

            return(JVX_NO_ERROR);
        }
        return(JVX_ERROR_ELEMENT_NOT_FOUND);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) activateInputOptionId:(int) idSelect
{
    jvxErrorType res = JVX_NO_ERROR;
    NSError* error = NULL;
    if(state_machine.theState >= JVX_STATE_ACTIVE)
    {
        idInputOptionSelected = idSelect;
        if(state_machine.theState > JVX_STATE_ACTIVE)
        {
            res = [self activateInputOptionId_core:idSelect withError:&error];
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

- (jvxErrorType) activateInputOptionId_core:(int) idSelect withError:(NSError**) err
{
    int idx;
    int portId = 0;
    int sourceId = 0;
    NSArray* iPorts = NULL;
    NSArray* iSources = NULL;
    jvxErrorType res = JVX_NO_ERROR;
    BOOL resL = FALSE;
    
    NSLog(@"idSelect=%i", idSelect);
    if(idSelect < inputOptions.count)
    {
        oneDataSourceOption* oneElm = [inputOptions objectAtIndex:idSelect];
        
        /*
        iPorts = [sessionInstance availableInputs];
        if(iPorts)
        {
            if(oneElm.iPortId < iPorts.count)
            {
                AVAudioSessionPortDescription* setme = (AVAudioSessionPortDescription*)
                [iPorts objectAtIndex:oneElm.iPortId];
                resL = [sessionInstance setPreferredInput:setme error:err];
                if(resL)
                {
                    res = JVX_NO_ERROR;
                }
                else
                {
                    NSLog(@"AP::ACT1");
                    res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
                }
            }
            else
            {
                NSLog(@"AP::ACT2");
                res = JVX_ERROR_ID_OUT_OF_BOUNDS;
            }
        }
        else
        {
            if(idSelect == 0)
            {
                res = JVX_NO_ERROR;
            }
            else
            {
                NSLog(@"AP::ACT3");
                res = JVX_ERROR_ID_OUT_OF_BOUNDS;
            }
        }*/
        
        //        if(res == JVX_NO_ERROR)
        //        {
        
        iSources = [sessionInstance inputDataSources];
        if(iSources)
        {
            if(oneElm.iSourceId < iSources.count)
            {
                AVAudioSessionDataSourceDescription* setme = (AVAudioSessionDataSourceDescription*)
                [iSources objectAtIndex:oneElm.iSourceId];
                resL = [sessionInstance setInputDataSource:setme error:err];
                if(resL)
                {
                    res = JVX_NO_ERROR;
                }
                else
                {
                    NSLog(@"AP::ACT4");
                    res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
                }
            }
            else
            {
                NSLog(@"AP::ACT5");
                res = JVX_ERROR_ID_OUT_OF_BOUNDS;
            }
        }
        else
        {
            if(oneElm.iSourceId == 0)
            {
                res = JVX_NO_ERROR;
            }
            else
            {
                NSLog(@"AP::ACT6");
                res = JVX_ERROR_ID_OUT_OF_BOUNDS;
            }
        }
        //}
    }
    else
    {
        NSLog(@"AP::ACT7");
        res = JVX_ERROR_ID_OUT_OF_BOUNDS;
    }
    return(res);
}


- (jvxErrorType) activateOutputOptionId:(int) idSelect
{
    jvxErrorType res = JVX_NO_ERROR;
    NSError* error = NULL;
    
    if(state_machine.theState >= JVX_STATE_ACTIVE)
    {
        idOutputOptionSelected = idSelect;
        if(state_machine.theState > JVX_STATE_ACTIVE)
        {
            res = [self activateOutputOptionId_core:idSelect withError:&error];
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

- (jvxErrorType) activateOutputOptionId_core:(int) idSelect withError:(NSError**)err
{
    int idx;
    int portId = 0;
    int sourceId = 0;
    NSArray* iPorts = NULL;
    NSArray* iSources = NULL;
    BOOL resL = TRUE;
    jvxErrorType res = JVX_NO_ERROR;
    if(idSelect < outputOptions.count)
    {
        oneDataSourceOption* oneElm = [outputOptions objectAtIndex:idSelect];
        
        iSources = [sessionInstance outputDataSources];
        if(iSources)
        {
            if(oneElm.iSourceId < iSources.count)
            {
                AVAudioSessionDataSourceDescription* setme = (AVAudioSessionDataSourceDescription*)
                [iSources objectAtIndex:oneElm.iSourceId];
                resL = [sessionInstance setInputDataSource:setme error:err];
                if(resL)
                {
                    res = JVX_NO_ERROR;
                }
                else
                {
                    res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
                }
            }
            else
            {
                res = JVX_ERROR_ID_OUT_OF_BOUNDS;
            }
        }
        else
        {
            if(idSelect == 0)
            {
                res = JVX_NO_ERROR;
            }
            else
            {
                res = JVX_ERROR_ID_OUT_OF_BOUNDS;
            }
        }
    }
    else
    {
        res = JVX_ERROR_ID_OUT_OF_BOUNDS;
    }
    return(res);
}
/*
 * Function to activate/deactivate the devices speaker
 *========================================================*/
-(jvxErrorType) activateSpeaker:(jvxBool)activateSpeaker
{
    jvxErrorType res = JVX_NO_ERROR;
    NSError *error = NULL;
    
    if(state_machine.theState >= JVX_STATE_SELECTED)
    {
        if(activateSpeaker)
        {
            [[AVAudioSession sharedInstance] overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:&error];
            state_machine.speakerIsOn = true;
        }
        else
        {
            [[AVAudioSession sharedInstance] overrideOutputAudioPort:AVAudioSessionPortOverrideNone error:&error];
            state_machine.speakerIsOn = false;
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

@end

