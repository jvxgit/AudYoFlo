//
//  ViewController_messages.h
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 08.01.16.
//  Copyright © 2016 Javox Solutions. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "tabBarController_rtproc.h"

@interface ViewController_messages : UIViewController <UIPickerViewDataSource, UIPickerViewDelegate, JVXViewControllerDelegate>
{
}

@property (weak, nonatomic) IBOutlet UIButton *buttonClear;
@property (weak, nonatomic) IBOutlet UITextView *theTextField;
@property (weak, nonatomic) IBOutlet UILabel *label_1;
@property (weak, nonatomic) IBOutlet UILabel *label_2;
@property (weak, nonatomic) IBOutlet UILabel *label_3;
@property (weak, nonatomic) IBOutlet UILabel *label_4;
@property (weak, nonatomic) IBOutlet UIButton *buttonAddFile;
@property (weak, nonatomic) IBOutlet UIButton *buttonRemoveFile;
@property (weak, nonatomic) IBOutlet UIPickerView *selectionFileLib;
@property (weak, nonatomic) IBOutlet UIPickerView *selectionFileLocal;
@property (weak, nonatomic) IBOutlet UILabel *labelRemote;


- (IBAction)clearButtonPushed:(id)sender;
- (void) textMessageD:(NSString*) txt;
- (IBAction)buttonAddContent:(id)sender;
- (IBAction)buttonRemoveContent:(id)sender;

- (void) updateWindowD:(tabBarController_rtproc*) hRef;

@end
