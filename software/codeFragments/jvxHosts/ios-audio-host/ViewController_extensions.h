//
//  ViewController_extensions.h
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 06.01.16.
//  Copyright © 2016 Javox Solutions. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "tabBarController_rtproc.h"


@interface ViewController_extensions : UIViewController <UIPickerViewDataSource, UIPickerViewDelegate, UITextFieldDelegate,
JVXViewControllerDelegate>
{
    jvxHandle* thePropHandle;
    jvxSize id_inputLevelAvrg;
    jvxSize id_outputLevelAvrg;
    jvxSize id_inputLevelMax;
    jvxSize id_outputLevelMax;
    jvxSize id_currentLoad;
}
@property (weak, nonatomic) IBOutlet UISwitch *switchActivateInput;
@property (weak, nonatomic) IBOutlet UIPickerView *selectionSourcesRoutes;
@property (weak, nonatomic) IBOutlet UIPickerView *selectionInputFiles;
@property (weak, nonatomic) IBOutlet UIPickerView *selectionOutputFiles;


@property (weak, nonatomic) IBOutlet UILabel *labelProgressFiles;
@property (weak, nonatomic) IBOutlet UITextField *txtFldSamplerateOutfile;
@property (weak, nonatomic) IBOutlet UITextField *textFieldTitleOutfile;
@property (weak, nonatomic) IBOutlet UITextField *textFieldChannelsOutfile;

@property (weak, nonatomic) IBOutlet UISlider *sliderInLevel;
@property (weak, nonatomic) IBOutlet UISlider *sliderOutLevel;

@property (weak, nonatomic) IBOutlet UILabel *labelInLevel;
@property (weak, nonatomic) IBOutlet UILabel *labelOutLevel;

@property (weak, nonatomic) IBOutlet UILabel *labelInLevelCurrent;
@property (weak, nonatomic) IBOutlet UILabel *labelOutLevelCurrent;

@property (weak, nonatomic) IBOutlet UILabel *labelChannels;
@property (weak, nonatomic) IBOutlet UILabel *labelSamplerate;

@property (weak, nonatomic) IBOutlet UILabel *labelCurrentLoad;

@property (weak, nonatomic) IBOutlet UILabel *label_1;
@property (weak, nonatomic) IBOutlet UILabel *label_2;
@property (weak, nonatomic) IBOutlet UILabel *label_3;
@property (weak, nonatomic) IBOutlet UILabel *label_4;
@property (weak, nonatomic) IBOutlet UILabel *label_5;
@property (weak, nonatomic) IBOutlet UILabel *label_txtLoad;
@property (weak, nonatomic) IBOutlet UILabel *label_txtRemote;
@property (weak, nonatomic) IBOutlet UIProgressView *levelMeterInput;
@property (weak, nonatomic) IBOutlet UIProgressView *levelMeterOutput;
- (IBAction)triggerRemoveOutputFile:(id)sender;
- (IBAction)triggerAddOutputFile:(id)sender;

- (IBAction)selectionRemoteOnOff:(id)sender;

- (IBAction)inGainChanged:(id)sender;
- (IBAction)outGainChanged:(id)sender;

- (int)numberOfComponentsInPickerView:(UIPickerView *)pickerView;
- (int)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component;
-(UIView *)pickerView:(UIPickerView *)pickerView viewForRow:(NSInteger)row forComponent:(NSInteger)component reusingView:(UIView *)view;
- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component;

- (void) startRun:(tabBarController_rtproc*) hRef;
- (void) stopRun:(tabBarController_rtproc*) hRef;
- (void) updateWindowP:(tabBarController_rtproc*) hRef;

- (void) updateWindowD:(tabBarController_rtproc*) hRef;
- (IBAction)activatedCurrentFileInput:(id)sender;

-(void) setInGain:(float) val
         withHRef:(tabBarController_rtproc*) hRef;
-(void) setOutGain:(float) val
          withHRef:(tabBarController_rtproc*) hRef;

@end
