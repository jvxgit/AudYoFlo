//
//  ViewController.h
//  myJvxApp
//
//  Created by Hauke Krüger on 22.12.15.
//  Copyright © 2015 Javox Solutions. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "jvxAudioProcessor.h"

/*
 * Viewcontroller to realize the - very simple - viewcontroller for this
 * RTproc test application 
 * ==============================================*/
@interface ViewController : UIViewController
{
    // Link to obj c audio processing handler
    jvxAudioProcessor *theProcessor;
    
    // Start & stop button
    __weak IBOutlet UIButton *startStopButton;
    
    // Slider to activate/deactivate speaker
    __weak IBOutlet UISegmentedControl *speakerOnOff;
}

// Callback in case someone used slider to activate/deactivate speaker
- (IBAction)speakerOnOff_valueChanged:(id)sender;

// Callback oce start has been pushed
- (IBAction)onStartPushed:(id)sender;

// Update the main window
- (void)updateWindow;

// Backward report function (realized based on C callback)
-(void) notificationFromAudio:(jvxIosBwdCommand) cmd
                   withReason:(NSDictionary*) dict;

@end

