//
//  ViewController.m
//  myJvxApp
//
//  Created by Hauke Krüger on 22.12.15.
//  Copyright © 2015 Javox Solutions. All rights reserved.
//

#import "ViewController.h"
#import <AVFoundation/AVFoundation.h>

/*
 * C function based callback mechanism, c to obj c bridge
 *=======================================================*/
void myCallbackViewController(jvxIosBwdCommand commandId, void* params, void *context)
{
    ViewController* myController = (__bridge ViewController*)context;
    NSDictionary* theParaDSict = (__bridge NSDictionary*)params;
    
    if(myController)
    {
        [myController notificationFromAudio:commandId
                    withReason:theParaDSict];
    }
}

// ====================================================
// ====================================================
// ====================================================
// ====================================================

/*
 * View controller implementation */
@interface ViewController ()

@end

@implementation ViewController


/* 
 * Callback once the view has been loaded
 *=================================================*/
- (void)viewDidLoad
{
  
    // Initialize audio processor sub module
    theProcessor = [[jvxAudioProcessor alloc ] init];

/*
    // Add observers for specific events
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(notificationFromAudio:)
                                                 name:@"Report_newRoute"
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(notificationFromAudio:)
                                                 name:@"Report_interruption"
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(notificationFromAudio:)
                                                 name:@"Report_newRoute"
                                               object:nil];
    */
    // Very first draw of window
    [self updateWindow];
    
    // Call parent class callback function
    [super viewDidLoad];
    
    // Do any additional setup after loading the view, typically from a nib.
}

/*
 * Function call given by iOS code generation
 *===============================================*/
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
 * Callback to catch new selection of speaker output
 *===============================================*/
- (IBAction)speakerOnOff_valueChanged:(id)sender
{
    jvxState state = JVX_STATE_NONE;
    jvxErrorType res = [theProcessor status:&state];
    if(res == JVX_NO_ERROR)
    {
        if(state >= JVX_STATE_SELECTED)
        {
            NSInteger sel = speakerOnOff.selectedSegmentIndex;
            if(sel == 0)
            {
                [theProcessor activateSpeaker:false];
            }
            else
            {
                [theProcessor activateSpeaker:true];
            }
            [self updateWindow];
        }
    }
}

/*
 * Callback to catch start/stop button
 *==============================================*/
- (IBAction)onStartPushed:(id)sender
{
    jvxState state = JVX_STATE_NONE;
    jvxErrorType res = [theProcessor status:&state];
    if(res == JVX_NO_ERROR)
    {
        if(state == JVX_STATE_INIT)
        {
            // Audio is not running, we may start
            res = [theProcessor initAudioTechnology:(__bridge void *)(self)
                                        andCallback:(myCallbackViewController)
                                       andBluetooth:(true)
                                         andSpeaker:(false)];
            
            res = [theProcessor initAudioDevice:0];
            
            res = [theProcessor startAudio];
            
            
            [self updateWindow];
        }
        else
        {
            res = [theProcessor stopAudio];
            res = [theProcessor terminateAudioDevice];
            res = [theProcessor terminateAudioTechnology];
            
            [self updateWindow];
        }
    }
}

/*
 * Function to update the main window to post new infos
 *===============================================*/
- (void)updateWindow
{
//    NSLog(@"Update Window");
    jvxBool active = false;
    jvxState stat = JVX_STATE_NONE;
    jvxErrorType res = [theProcessor status:&stat];
    if(res == JVX_NO_ERROR)
    {
        if(stat == JVX_STATE_INIT)
        {
            [startStopButton setTitle: @"Start" forState: UIControlStateNormal];
            speakerOnOff.enabled = false;
        }
        else
        {
            [startStopButton setTitle: @"Stop" forState: UIControlStateNormal];
            
            // Set option in speaker selection
            speakerOnOff.enabled = true;
            res = [theProcessor statusActivateSpeaker:&active];
            if(active)
            {
                [speakerOnOff setSelectedSegmentIndex:1];
            }
            else
            {
                [speakerOnOff setSelectedSegmentIndex:0];
            }
        }
    }
}

/*
 * Callback initiated by audio subsystem to indicate different state changes
 *=================================================*/
- (void) notificationFromAudio:(jvxIosBwdCommand) cmd
                    withReason:(NSDictionary*) dict
{
    NSString* printText = @"Unknown Reason";
    switch(cmd)
    {
        case JVX_IOS_BWD_COMMAND_NEW_ROUTE:
            if(dict)
            {
                /*AVAudioSessionRouteDescription* reason = [dict objectForKey:AVAudioSessionRouteChangePreviousRouteKey];*/
                NSNumber* stateBeforeId = [dict objectForKey:AVAudioSessionRouteChangeReasonKey];
                if(stateBeforeId)
                {
                    int id = stateBeforeId.intValue;
                    switch(id)
                    {
                        case AVAudioSessionRouteChangeReasonUnknown:
                            printText = @"Unknown Reason";
                            break;
                        case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
                            printText = @"New Device Available";
                            break;
                        case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
                            printText = @"Old Device Unavailable";
                            break;
                        case AVAudioSessionRouteChangeReasonCategoryChange:
                            printText = @"Category Changed";
                            break;
                        case AVAudioSessionRouteChangeReasonOverride:
                            printText = @"Override Output";
                            break;
                        case AVAudioSessionRouteChangeReasonWakeFromSleep:
                            printText = @"Wake up From Sleep";
                            break;
                        case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory:
                            printText = @"No Suitable Route";
                            break;
                        case AVAudioSessionRouteChangeReasonRouteConfigurationChange:
                            printText = @"Route Configuration Change";
                            break;
                        default:
                            break;
                    }
                }
            
            }
            NSLog(@"New Route Reason: %@", printText);
            break;
        case JVX_IOS_BWD_COMMAND_MEDIACENTER_RESET:
            NSLog(@"Media Centre Reset Reason: %@", printText);
            break;
        case JVX_IOS_BWD_COMMAND_INTERRUPTION:
            NSLog(@"Interruption Reason: %@", printText);
            break;
        default:
            NSLog(@"Unknown reason.");
            break;
    }
    
    // Update Window
    [ self updateWindow];
}

@end
