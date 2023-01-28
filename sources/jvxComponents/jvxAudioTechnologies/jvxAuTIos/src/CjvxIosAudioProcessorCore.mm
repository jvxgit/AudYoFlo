#include "CjvxIosAudioProcessorCore.h"

@implementation CjvxIosAudioProcessorCore

- (BOOL)initialize
{
    BOOL success = NO;
    NSError *error = nil;
    
    // Get instance to audio session
    myAudioSession = [AVAudioSession sharedInstance];
    success = [myAudioSession setCategory:AVAudioSessionCategoryPlayAndRecord
                               withOptions:AVAudioSessionCategoryOptionAllowBluetooth
                                    error:&error];
    
    if (!success)
    {
        NSLog(@"%@ Error setting category: %@",
              NSStringFromSelector(_cmd), [error localizedDescription]);
    }
    
    // Activate this audio session
    success = [myAudioSession setActive:YES error:&error];
    if (!success) {
        NSLog(@"%@", [error localizedDescription]);
    }

    // Request record permissions
    if([myAudioSession respondsToSelector:@selector(requestRecordPermission:)])
    {
        [myAudioSession requestRecordPermission:^(BOOL granted)
         {
            if(granted)
            {
                NSLog(@"Access to mic granted!");
            }
             else
             {
                 NSLog(@"Access to mic NOT granted!");
             }
         }];
        
    }
    
    long numInChannels = [myAudioSession inputNumberOfChannels];
    long numInChannelsMax = [myAudioSession maximumInputNumberOfChannels];
    long numInChannelsPref = [myAudioSession preferredInputNumberOfChannels];

    long numOutChannels = [myAudioSession outputNumberOfChannels];
    long numOutChannelsMax = [myAudioSession maximumOutputNumberOfChannels];
    long numOutChannelsPref = [myAudioSession preferredOutputNumberOfChannels];

    AVAudioSessionRouteDescription* currentRoutePtr = [myAudioSession currentRoute];
    BOOL inputAvailable = myAudioSession.inputAvailable;
    BOOL otherAudioPlaying = myAudioSession.otherAudioPlaying;
    
    NSArray<AVAudioSessionPortDescription*>* inputs = [myAudioSession availableInputs];
    
    // overrideOutputAudioPort AVAudioSessionPortOverrideSpeaker + error
    ///inputDataSources

    return(success);
    
}

- (BOOL)terminate
{
    NSError *deactivationError = nil;
    BOOL success = [myAudioSession setActive:NO error:&deactivationError];
    if (!success)
    {
        NSLog(@"%@", [deactivationError localizedDescription]);
    }
    return success;
}

- (void)beginInterruption
{
    [self terminate];
}

- (void)endInterruption
{
    [self initialize];
}

@end
