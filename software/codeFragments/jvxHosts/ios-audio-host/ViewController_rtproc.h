//
//  ViewController.h
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 27.12.15.
//  Copyright © 2015 Javox Solutions. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "tabBarController_rtproc.h"


@interface ViewController_rtproc : UIViewController<UIPickerViewDataSource, UIPickerViewDelegate, UITextFieldDelegate,
JVXViewControllerDelegate>
{
}

@property (weak, nonatomic) IBOutlet UIPickerView *technologySelection;
@property (weak, nonatomic) IBOutlet UIPickerView *deviceSelection;
@property (weak, nonatomic) IBOutlet UIPickerView *inputChannelSelection;
@property (weak, nonatomic) IBOutlet UIPickerView *outputChannelSelection;
@property (weak, nonatomic) IBOutlet UITextField *txtFldSamplerate;
@property (weak, nonatomic) IBOutlet UIPickerView *samplerateSelection;
@property (weak, nonatomic) IBOutlet UIPickerView *buffersizeSelection;
@property (weak, nonatomic) IBOutlet UIPickerView *daformatSelection;
@property (weak, nonatomic) IBOutlet UITextField *txtFldBuffersize;
@property (weak, nonatomic) IBOutlet UILabel *labelResampling;
@property (weak, nonatomic) IBOutlet UILabel *labelRebuffering;
@property (weak, nonatomic) IBOutlet UILabel *labelDataConversion;
@property (weak, nonatomic) IBOutlet UIButton *buttonStart;
@property (weak, nonatomic) IBOutlet UIButton *buttonSave;
@property (weak, nonatomic) IBOutlet UILabel *labelReasonFail;
@property (weak, nonatomic) IBOutlet UIButton *inChansDownButton;
@property (weak, nonatomic) IBOutlet UIButton *inChansUpButton;
@property (weak, nonatomic) IBOutlet UISwitch *inChansToggleSelector;
@property (weak, nonatomic) IBOutlet UIButton *outChansDownButton;
@property (weak, nonatomic) IBOutlet UIButton *outChansUpButton;
@property (weak, nonatomic) IBOutlet UISwitch *outChansToggleSelector;
@property (weak, nonatomic) IBOutlet UIButton *buttonInChanDown;
@property (weak, nonatomic) IBOutlet UIButton *buttonInChanUp;
@property (weak, nonatomic) IBOutlet UIButton *buttonOutChanDown;
@property (weak, nonatomic) IBOutlet UIButton *buttonOutChanUp;
@property (weak, nonatomic) IBOutlet UILabel *label_1;
@property (weak, nonatomic) IBOutlet UILabel *label_2;
@property (weak, nonatomic) IBOutlet UILabel *label_3;
@property (weak, nonatomic) IBOutlet UILabel *label_4;
@property (weak, nonatomic) IBOutlet UILabel *labelSRate;
@property (weak, nonatomic) IBOutlet UILabel *labelBSize;
@property (weak, nonatomic) IBOutlet UILabel *labelDFormat;

// Pick Viewer callbacks
- (int)numberOfComponentsInPickerView:(UIPickerView *)pickerView;
- (int)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component;
- (UIView *)pickerView:(UIPickerView *)pickerView viewForRow:(NSInteger)row forComponent:(NSInteger)component reusingView:(UIView *)view;
- (NSString*)whatToShow:(UIPickerView *)pickerView
            titleForRow:(NSInteger)row
           forComponent:(NSInteger)component
         returnShowBold:(BOOL*)showBold
       returnShowItalic:(BOOL*)showIt
        returnPlotColor:(UIColor**) theColor;
- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
;
- (IBAction)inChansDown:(id)sender;
- (IBAction)inChansUp:(id)sender;
- (IBAction)inChanToggled:(id)sender;
- (IBAction)outChansDown:(id)sender;
- (IBAction)outChansUp:(id)sender;
- (IBAction)outChansToggled:(id)sender;

- (IBAction)saveConfigTriggered:(id)sender;
- (IBAction)startStopTriggered:(id)sender;

// Delegate function
- (void) updateWindowD:(tabBarController_rtproc*) hRef;

@end

