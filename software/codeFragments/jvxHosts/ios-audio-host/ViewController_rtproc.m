//
//  ViewController.m
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 27.12.15.
//  Copyright © 2015 Javox Solutions. All rights reserved.
//

#import "ViewController_rtproc.h"
#import "jvxIosLocalMediaItem.h"

/*
 * -----------------------------------------------
 * From here, interface implementation.
 * -----------------------------------------------
 */
@interface ViewController_rtproc ()


@end

@implementation ViewController_rtproc


- (void)viewDidLoad
{
    
    UIFontDescriptor * fontD = NULL;

    [super viewDidLoad];
    
    // Do any additional setup after loading the view, typically from a nib.
    self.technologySelection.dataSource = self;
    self.technologySelection.delegate = self;
    
    self.deviceSelection.dataSource = self;
    self.deviceSelection.delegate = self;
    
    self.inputChannelSelection.dataSource = self;
    self.inputChannelSelection.delegate = self;
    
    self.outputChannelSelection.dataSource = self;
    self.outputChannelSelection.delegate = self;

    self.samplerateSelection.dataSource = self;
    self.samplerateSelection.delegate = self;
    
    self.buffersizeSelection.dataSource = self;
    self.buffersizeSelection.delegate = self;
    
    self.daformatSelection.dataSource = self;
    self.daformatSelection.delegate = self;
    
    self.txtFldBuffersize.delegate = self;
    self.txtFldSamplerate.delegate = self;
    
    // Register this object with the parent view controller for broadcast messages
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    [hRef register_delegate:self];
        
    _inChansDownButton.layer.cornerRadius = 8;
    _inChansDownButton.clipsToBounds = YES;

    _inChansUpButton.layer.cornerRadius = 8;
    _inChansUpButton.clipsToBounds = YES;

    _outChansDownButton.layer.cornerRadius = 8;
    _outChansDownButton.clipsToBounds = YES;

    _outChansUpButton.layer.cornerRadius = 8;
    _outChansUpButton.clipsToBounds = YES;

    _buttonSave.layer.cornerRadius = 8;
    _buttonSave.clipsToBounds = YES;

    _buttonStart.layer.cornerRadius = 8; 
    _buttonStart.clipsToBounds = YES;

    // Font size of these objects are handled in delegates
    //_technologySelection;
    //_deviceSelection;
    //_inputChannelSelection;
    //_outputChannelSelection;
    //_samplerateSelection;
    //_buffersizeSelection;
    //_daformatSelection;
    
    SET_FONT_SIZE_ONE_ELEMENT(_label_1, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_2, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_3, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_4, hRef.fontSizeLarge);

    SET_FONT_SIZE_ONE_ELEMENT(_txtFldSamplerate, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_txtFldBuffersize, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_buttonSave.titleLabel, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_labelSRate, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_labelBSize, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_labelDFormat, hRef.fontSizeMedium);

    SET_FONT_SIZE_ONE_ELEMENT(_labelResampling, hRef.fontSizeSmall);
    SET_FONT_SIZE_ONE_ELEMENT(_labelRebuffering, hRef.fontSizeSmall);

    SET_FONT_SIZE_ONE_ELEMENT(_labelReasonFail, hRef.fontSizeTiny);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


// +++++++++++++++++++++++++++++++++++++++++++++++
// UIPickViewer delegate functions
// +++++++++++++++++++++++++++++++++++++++++++++++

// The number of columns of data
- (int)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    int retVal = 0;
    if(pickerView == _technologySelection)
    {
        retVal = 1;
    }
    
    if(pickerView == _deviceSelection)
    {
        retVal = 1;
    }
    
    if(pickerView == _inputChannelSelection)
    {
        retVal = 1;
    }

    if(pickerView == _outputChannelSelection)
    {
        retVal = 1;
    }

    if(pickerView == _samplerateSelection)
    {
        retVal = 1;
    }
    
    if(pickerView == _buffersizeSelection)
    {
        retVal = 1;
    }
    
    if(pickerView == _daformatSelection)
    {
        retVal = 1;
    }
    
    return(retVal);
}

// The number of rows of data
- (int)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    int retVal = 1;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    if(pickerView == _technologySelection)
    {
        if(hRef.theStateAudioTech == JVX_STATE_SELECTED)
        {
            retVal += hRef.lstTechs.count;
        }
        else
        {
            retVal += 1;
        }
    }
    
    if(pickerView == _deviceSelection)
    {
        if(hRef.theStateAudioTech == JVX_STATE_ACTIVE)
        {
            retVal += hRef.lstDevs.count;
        }
    }
    
    if(pickerView == _inputChannelSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
                retVal += hRef.lstInputChannels.count;
        }
    }
    
    if(pickerView == _outputChannelSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            retVal += hRef.lstOutputChannels.count;
        }
    }

    if(pickerView == _samplerateSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            retVal = (int)hRef.lstSamplerates.count;
        }
    }
    
    if(pickerView == _buffersizeSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            retVal = (int)hRef.lstBuffersizes.count;
        }
    }
    
    if(pickerView == _daformatSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            if(hRef.showWrappedFormats)
            {
                retVal = (int)hRef.lstDataformatsWD.count;
            }
            else
            {
                retVal = (int)hRef.lstDataformatsGW.count;
            }
        }
    }
    
    
    return(retVal);
}

// The data to return for the row and component (column) that's being passed in
- (NSString*)whatToShow:(UIPickerView *)pickerView
                 titleForRow:(NSInteger)row
                forComponent:(NSInteger)component
              returnShowBold:(BOOL*)showBold
                returnShowItalic:(BOOL*)showIt
        returnPlotColor:(UIColor**) theColor;
{
    NSString* retVal = @"--";
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    jvxGenericWrapperChannelType tpC;
    int idC1 = -1;
    int idC2 = -1;

    if(pickerView == _technologySelection)
    {
        if(hRef.theStateAudioTech == JVX_STATE_SELECTED)
        {
            if(row == 0)
            {
                retVal = @"None";
            }
            else
            {
                row--;
                retVal = [hRef.lstTechs objectAtIndex:row];
            }
        }
        else
        {
            // Case for higher than selected
            if(row == 0)
            {
                retVal = @"Technology Active";
            }
            else
            {
                retVal = @"Restart";
            }
        }
    }
    
    if(pickerView == _deviceSelection)
    {
        if(hRef.theStateAudioTech == JVX_STATE_ACTIVE)
        {
            if(row == 0)
            {
                retVal = @"None";
            }
            else
            {
                row--;
                if(row < hRef.lstDevs.count)
                {
                    retVal = [hRef.lstDevs objectAtIndex:row];
                }
            }
        }
    }
    
    if(pickerView == _inputChannelSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            if(row > 0)
            {
                row--;
                if(row < hRef.lstInputChannels.count)
                {
                    retVal = [hRef.lstInputChannels objectAtIndex:row];
                    retVal = [hRef.hostHandle helper_classify_channel_token:retVal
                                                      withReturnChannelType:&tpC
                                                         withReturnCodedId1:&idC1
                              withFieldWidth:hRef.fieldWidthChannels];
                    retVal = [retVal stringByAppendingString:[NSString stringWithFormat:@"(%i)", idC1]];

                    if(hRef.selInputChannels & (jvxBitField)1 << row)
                    {
                        *showBold = true;
                        *showIt = false;
                        *theColor = [UIColor blackColor];
                    }
                    else
                    {
                        *showBold = false;
                        *showIt = true;
                        *theColor = [UIColor grayColor];
                    }
                }
            }
            else
            {
                retVal = @"All";
            }
        }
    }
    
    if(pickerView == _outputChannelSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            if(row > 0)
            {
                row--;
                if(row < hRef.lstOutputChannels.count)
                {
                    retVal = [hRef.lstOutputChannels objectAtIndex:row];
                    retVal = [hRef.hostHandle helper_classify_channel_token:retVal
                                                      withReturnChannelType:&tpC
                                                         withReturnCodedId1:&idC1
                              withFieldWidth:hRef.fieldWidthChannels];
                    retVal = [retVal stringByAppendingString:[NSString stringWithFormat:@"(%i)", idC1]];
                    if(hRef.selOutputChannels & (jvxBitField)1 << row)
                    {
                        *showBold = true;
                        *showIt = false;
                        *theColor = [UIColor blackColor];
                    }
                    else
                    {
                        *showBold = false;
                        *showIt = true;
                        *theColor = [UIColor grayColor];
                    }
                }
            }
            else
            {
                retVal = @"All";
            }
        }
    }

    if(pickerView == _samplerateSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            if(row < hRef.lstSamplerates.count)
            {
                retVal = [hRef.lstSamplerates objectAtIndex:row];
                if(hRef.selSamplerates & (jvxBitField)1 << row)
                {
                    *showBold = true;
                    *showIt = false;
                    *theColor = [UIColor blackColor];
                }
                else
                {
                    *showBold = false;
                    *showIt = true;
                    *theColor = [UIColor grayColor];
                }

            }
        }
    }
    
    if(pickerView == _buffersizeSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            if(row < hRef.lstBuffersizes.count)
            {
                retVal = [hRef.lstBuffersizes objectAtIndex:row];
                if(hRef.selBuffersizes & (jvxBitField)1 << row)
                {
                    *showBold = true;
                    *showIt = false;
                    *theColor = [UIColor blackColor];
                }
                else
                {
                    *showBold = false;
                    *showIt = true;
                    *theColor = [UIColor grayColor];
                }

            }
        }
    }
    
    if(pickerView == _daformatSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            if(hRef.showWrappedFormats)
            {
                if(row < hRef.lstDataformatsWD.count)
                {
                    retVal = [hRef.lstDataformatsWD objectAtIndex:row];
                    if(hRef.selDataFormatsWD & (jvxBitField)1 << row)
                    {
                        *showBold = true;
                        *showIt = false;
                        *theColor = [UIColor blackColor];
                    }
                    else
                    {
                        *showBold = false;
                        *showIt = true;
                        *theColor = [UIColor grayColor];
                    }
                    
                }
            }
            else
            {
                if(row < hRef.lstDataformatsGW.count)
                {
                    retVal = [hRef.lstDataformatsGW objectAtIndex:row];
                    if(hRef.selDataFormatsGW & (jvxBitField)1 << row)
                    {
                        *showBold = true;
                        *showIt = false;
                        *theColor = [UIColor blackColor];
                    }
                    else
                    {
                        *showBold = false;
                        *showIt = true;
                        *theColor = [UIColor grayColor];
                    }
                }
            }
        }
    }
    
    JVX_ASSERT((retVal != NULL), @"VCRTP9");
    return(retVal);
}

- (IBAction)inChansDown:(id)sender
{
    int i;
    NSString* channelName = NULL;
    jvxBitField selectionNew = 0;
    
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
    {
        if(hRef.lstInputChannels.count > 1)
        {
            NSString* prop_name = @"JVX_AUDEV_INPUT_CHANNELS";
            
            if(hRef.curSelectionInputChannels >= 0)
            {
                NSMutableArray* passChannels = [[NSMutableArray alloc] init];
                jvxSize rSelection = hRef.curSelectionInputChannels;
                
                if(rSelection < (hRef.lstInputChannels.count-1))
                {
                    for(i = 0; i < rSelection; i++)
                    {
                        channelName = (NSString*)[hRef.lstInputChannels objectAtIndex:i];
                        [passChannels addObject:channelName];
                        selectionNew |= (hRef.selInputChannels & ((jvxBitField)1 << i));
                    }
                    channelName = (NSString*)[hRef.lstInputChannels objectAtIndex:rSelection+1];
                    [passChannels addObject:channelName];
                    selectionNew |= (hRef.selInputChannels & ((jvxBitField)1 << (rSelection+1)));
                    channelName = (NSString*)[hRef.lstInputChannels objectAtIndex:rSelection];
                    [passChannels addObject:channelName];
                    selectionNew |= (hRef.selInputChannels & ((jvxBitField)1 << (rSelection)));
                    for(i = (rSelection+2); i < hRef.lstInputChannels.count; i++)
                    {
                        channelName = (NSString*)[hRef.lstInputChannels objectAtIndex:i];
                        [passChannels addObject:channelName];
                        selectionNew |= (hRef.selInputChannels & ((jvxBitField)1 << i));
                    }
                    hRef.curSelectionInputChannels++;
                    
                    [hRef.hostHandle set_property_selection_list:passChannels
                                                    newSelection:selectionNew
                                                withNameProperty:prop_name
                                                    forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                    [hRef updateWindow];
                }
            }
        }
    }
}

- (IBAction)inChansUp:(id)sender
{
    int i;
    NSString* channelName = NULL;
    jvxBitField selectionNew = 0;
    
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
    {
        if(hRef.lstInputChannels.count > 1)
        {
            NSString* prop_name = @"JVX_AUDEV_INPUT_CHANNELS";
            
            if(hRef.curSelectionInputChannels >= 0)
            {
                NSMutableArray* passChannels = [[NSMutableArray alloc] init];
                jvxSize rSelection = hRef.curSelectionInputChannels;
                
                if(rSelection > 0)
                {
                    for(i = 0; i < rSelection-1; i++)
                    {
                        channelName = (NSString*)[hRef.lstInputChannels objectAtIndex:i];
                        [passChannels addObject:channelName];
                        selectionNew |= (hRef.selInputChannels & ((jvxBitField)1 << i));
                    }
                    channelName = (NSString*)[hRef.lstInputChannels objectAtIndex:rSelection];
                    [passChannels addObject:channelName];
                    selectionNew |= (hRef.selInputChannels & ((jvxBitField)1 << (rSelection)));
                    channelName = (NSString*)[hRef.lstInputChannels objectAtIndex:rSelection-1];
                    [passChannels addObject:channelName];
                    selectionNew |= (hRef.selInputChannels & ((jvxBitField)1 << (rSelection-1)));
                    
                    for(i = (rSelection+2); i < rSelection; i++)
                    {
                        channelName = (NSString*)[hRef.lstInputChannels objectAtIndex:i];
                        [passChannels addObject:channelName];
                        selectionNew |= (hRef.selInputChannels & ((jvxBitField)1 << i));
                    }
                    
                    hRef.curSelectionInputChannels--;
                    
                    [hRef.hostHandle set_property_selection_list:passChannels
                                                    newSelection:selectionNew
                                                withNameProperty:prop_name
                                                    forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                    [hRef updateWindow];
                }
            }
        }
    }
}

- (IBAction)inChanToggled:(id)sender
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
    {
        if(hRef.lstInputChannels.count > 0)
        {
            NSString* prop_name = @"JVX_AUDEV_INPUT_CHANNELS";
            BOOL isOn = _inChansToggleSelector.on;
            jvxBitField selection = 0;
            if(hRef.curSelectionInputChannels < 0)
            {
                if(isOn)
                {
                    selection = ((jvxBitField)1 << (hRef.lstInputChannels.count + 1) ) - 1;
                }
            }
            else
            {
                selection = hRef.selInputChannels;
                jvxInt32 idx = hRef.curSelectionInputChannels;
                if(isOn)
                {
                    selection |= ((jvxBitField)1 << idx);
                }
                else
                {
                    selection &= ~((jvxBitField)1 << idx);
                }
            }
            
            [hRef.hostHandle set_property_selection_list:NULL
                                            newSelection:selection
                                        withNameProperty:prop_name
                                            forComponent:JVX_COMPONENT_AUDIO_DEVICE];
            [hRef updateWindow];
        }
    }
}

- (IBAction)outChansDown:(id)sender
{
    int i;
    NSString* channelName = NULL;
    jvxBitField selectionNew = 0;
    
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
    {
        if(hRef.lstOutputChannels.count > 1)
        {
            NSString* prop_name = @"JVX_AUDEV_OUTPUT_CHANNELS";
            
            if(hRef.curSelectionOutputChannels >= 0)
            {
                NSMutableArray* passChannels = [[NSMutableArray alloc] init];
                jvxSize rSelection = hRef.curSelectionOutputChannels;
                
                if(rSelection < (hRef.lstOutputChannels.count-1))
                {
                    for(i = 0; i < rSelection; i++)
                    {
                        channelName = (NSString*)[hRef.lstOutputChannels objectAtIndex:i];
                        [passChannels addObject:channelName];
                        selectionNew |= (hRef.selOutputChannels & ((jvxBitField)1 << i));
                    }
                    channelName = (NSString*)[hRef.lstOutputChannels objectAtIndex:rSelection+1];
                    [passChannels addObject:channelName];
                    selectionNew |= (hRef.selOutputChannels & ((jvxBitField)1 << (rSelection+1)));
                    channelName = (NSString*)[hRef.lstOutputChannels objectAtIndex:rSelection];
                    [passChannels addObject:channelName];
                    selectionNew |= (hRef.selOutputChannels & ((jvxBitField)1 << (rSelection)));
                    for(i = (rSelection+2); i < hRef.lstOutputChannels.count; i++)
                    {
                        channelName = (NSString*)[hRef.lstOutputChannels objectAtIndex:i];
                        [passChannels addObject:channelName];
                        selectionNew |= (hRef.selOutputChannels & ((jvxBitField)1 << i));
                    }
                    hRef.curSelectionOutputChannels++;
                    
                    [hRef.hostHandle set_property_selection_list:passChannels
                                                    newSelection:selectionNew
                                                withNameProperty:prop_name
                                                    forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                    [hRef updateWindow];
                }
            }
        }
    }
}

- (IBAction)outChansUp:(id)sender
{
    int i;
    NSString* channelName = NULL;
    jvxBitField selectionNew = 0;
    
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
    {
        if(hRef.lstOutputChannels.count > 1)
        {
            NSString* prop_name = @"JVX_AUDEV_OUTPUT_CHANNELS";
            
            if(hRef.curSelectionOutputChannels >= 0)
            {
                NSMutableArray* passChannels = [[NSMutableArray alloc] init];
                jvxSize rSelection = hRef.curSelectionOutputChannels;
                
                if(rSelection > 0)
                {
                    for(i = 0; i < rSelection-1; i++)
                    {
                        channelName = (NSString*)[hRef.lstOutputChannels objectAtIndex:i];
                        [passChannels addObject:channelName];
                        selectionNew |= (hRef.selOutputChannels & ((jvxBitField)1 << i));
                    }
                    channelName = (NSString*)[hRef.lstOutputChannels objectAtIndex:rSelection];
                    [passChannels addObject:channelName];
                    selectionNew |= (hRef.selOutputChannels & ((jvxBitField)1 << (rSelection)));
                    channelName = (NSString*)[hRef.lstOutputChannels objectAtIndex:rSelection-1];
                    [passChannels addObject:channelName];
                    selectionNew |= (hRef.selOutputChannels & ((jvxBitField)1 << (rSelection-1)));
                    
                    for(i = (rSelection+2); i < rSelection; i++)
                    {
                        channelName = (NSString*)[hRef.lstOutputChannels objectAtIndex:i];
                        [passChannels addObject:channelName];
                        selectionNew |= (hRef.selOutputChannels & ((jvxBitField)1 << i));
                    }
                    
                    hRef.curSelectionOutputChannels--;
                    
                    [hRef.hostHandle set_property_selection_list:passChannels
                                                    newSelection:selectionNew
                                                withNameProperty:prop_name
                                                    forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                    [hRef updateWindow];
                }
            }
        }
    }
}

- (IBAction)outChansToggled:(id)sender
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
    {
        if(hRef.lstOutputChannels.count > 0)
        {
            NSString* prop_name = @"JVX_AUDEV_OUTPUT_CHANNELS";
            BOOL isOn = _outChansToggleSelector.on;
            jvxBitField selection = 0;
            if(hRef.curSelectionOutputChannels < 0)
            {
                if(isOn)
                {
                    selection = ((jvxBitField)1 << (hRef.lstOutputChannels.count + 1) ) - 1;
                }
            }
            else
            {
                selection = hRef.selOutputChannels;
                jvxInt32 idx = hRef.curSelectionOutputChannels;
                if(isOn)
                {
                    selection |= ((jvxBitField)1 << idx);
                }
                else
                {
                    selection &= ~((jvxBitField)1 << idx);
                }
            }
            [hRef.hostHandle set_property_selection_list:NULL
                                            newSelection:selection
                                        withNameProperty:prop_name
                                            forComponent:JVX_COMPONENT_AUDIO_DEVICE];
            [hRef updateWindow];
        }
    }
}

- (IBAction)saveConfigTriggered:(id)sender
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    NSString* reportTxt = NULL;
    
    [hRef.hostHandle set_entry_config_file:@"ios_host_in_levels" dataListToStore:hRef.lstInputGains];
    [hRef.hostHandle set_entry_config_file:@"ios_host_out_levels" dataListToStore:hRef.lstOutputGains];
    
    if(hRef.selectionInputFile_localArtist >= 0)
    {
        NSString* artist = [hRef.localLstArtists objectAtIndex:hRef.selectionInputFile_localArtist];
        [hRef.hostHandle set_entry_config_file:@"ios_host_in_file_artist" stringToStore:artist];
        
    }
    if(hRef.selectionInputFile_localTitle >= 0)
    {
        jvxIosLocalMediaItem* it = [hRef.localLstItemsForArtist objectAtIndex:hRef.selectionInputFile_localTitle];
        NSString* title = it.nmTitle;
        [hRef.hostHandle set_entry_config_file:@"ios_host_in_file_title" stringToStore:title];
    }
    
    if(hRef.selectionOutputFile >= 0)
    {
        NSString* name = [hRef.lstOutputFileNames objectAtIndex:hRef.selectionOutputFile];
        [hRef.hostHandle set_entry_config_file:@"ios_host_out_file_name" stringToStore:name];
    }
    
    jvxErrorType res = [hRef.hostHandle save_config:&reportTxt];
    if(res != JVX_NO_ERROR)
    {
        [hRef report_to_viewport:[NSNumber numberWithInteger:JVX_IOS_BWD_COMMAND_TEXT_MESSAGE]
            withObject:reportTxt];
    }
    else
    {
        [hRef report_to_viewport:[NSNumber numberWithInteger:JVX_IOS_BWD_COMMAND_TEXT_MESSAGE]
                      withObject:@"Successfully stored configuration"];
    }
}

- (IBAction)startStopTriggered:(id)sender
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    jvxErrorType res = JVX_NO_ERROR;
    int i;
    
    if(hRef.statusIsRunning == false)
    {
        // Activate lebel control in generic wrapper device
        res = [hRef.hostHandle set_property_selection_list:NULL
                                              newSelection:((jvxBitField)1 << 0)
                                          withNameProperty:@"JVX_GENW_ACT_ILEVEL"
                                              forComponent:JVX_COMPONENT_AUDIO_DEVICE];
        JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP5.1");
        
        res = [hRef.hostHandle set_property_selection_list:NULL
                                              newSelection:((jvxBitField)1 << 0)
                                          withNameProperty:@"JVX_GENW_ACT_OLEVEL"
                                              forComponent:JVX_COMPONENT_AUDIO_DEVICE];
        
                    
        
        res = [hRef.hostHandle set_property_string:hRef.documentsDirectory
                                  withNameProperty:@"JVX_GENW_DEFAULT_PATH_WAVS"
                                              forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
        
        JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP5.2");

        [hRef.hostHandle start_run];
        
        // Pass input gains
        NSString* prop_name = @"JVX_GENW_DEV_IGAIN";
        for(i = 0; i < hRef.lstInputGains.count; i++)
        {
            jvxData val = [[hRef.lstInputGains objectAtIndex:i] floatValue];
            if(hRef.selInputChannels & ((jvxBitField)1 << i))
            {
                res = [hRef.hostHandle set_property_numeric:&val
                                        forDataFormat:JVX_DATAFORMAT_DATA
                                      withOffsetArray:i
                                     withNameProperty:prop_name
                                         forComponent:JVX_COMPONENT_AUDIO_DEVICE];
            }
        }
        
        // Pass output gains
        prop_name = @"JVX_GENW_DEV_OGAIN";
        for(i = 0; i < hRef.lstOutputGains.count; i++)
        {
            jvxData val = [[hRef.lstOutputGains objectAtIndex:i] floatValue];
            if(hRef.selOutputChannels & ((jvxBitField)1 << i))
            {
                res = [hRef.hostHandle set_property_numeric:&val
                                              forDataFormat:JVX_DATAFORMAT_DATA
                                            withOffsetArray:i
                                 withNameProperty:prop_name
                                     forComponent:JVX_COMPONENT_AUDIO_DEVICE];
            }
        }

        [hRef start_timer:JVX_TIMER_SEC];
    }
    else
    {
        [hRef stop_timer];
        [hRef.hostHandle stop_run];
        [hRef update_local_infiles];

    }
    [hRef updateWindow];
}

-(UIView *)pickerView:(UIPickerView *)pickerView viewForRow:(NSInteger)row forComponent:(NSInteger)component reusingView:(UIView *)view
{
    UIColor* theColor = [UIColor blackColor];
    BOOL showBold = false;
    BOOL showIt = false;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];

    // Get the text of the row.
    NSString *rowItem = [self whatToShow:pickerView
                                  titleForRow:row
                                 forComponent:component
                               returnShowBold:&showBold
                             returnShowItalic:&showIt
                              returnPlotColor:&theColor];
    
    NSInteger divisor = [self numberOfComponentsInPickerView:pickerView];
    
 //   NSInteger fontSize = 16;
    
    // Create and init a new UILabel.
    // We must set our label's width equal to our picker's width.
    // We'll give the default height in each row.
    float wd = [pickerView bounds].size.width;
    float ht = hRef.fontSizeMedium;
    wd = wd / (float) divisor;

    UILabel* lab = [hRef createLabelEntryPickerViewer:rowItem
                                            withWidth:wd
                                           withHeight:ht
                                           doShowBold:showBold
                                         doShowItalic:showIt
                                            withColor:theColor];
    return(lab);
}




- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    jvxErrorType res = JVX_NO_ERROR;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    // This method is triggered whenever the user makes a change to the picker selection.
    // The parameter named row and component represents what was selected.
    if(pickerView == _technologySelection)
    {
        
        if(hRef.theStateAudioTech == JVX_STATE_SELECTED)
        {
            if(row > 0)
            {
                jvxInt32 idxSelect = (jvxInt32)row - 1;
                if(idxSelect >= 0)
                {
                    res = [hRef.hostHandle set_property_selection_list:NULL
                                                     newSelection:((jvxBitField)1 << idxSelect)
                                                 withNameProperty:@"JVX_GENW_TECHNOLOGIES"
                                                     forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
                    
                    JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP1");
                    
                    res = [hRef.hostHandle activate_component:JVX_COMPONENT_AUDIO_TECHNOLOGY];
                    JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP2");
                    
                    [hRef handle_states_components];
                    [hRef updateWindow];
                }
            }
        }
        else
        {
            if(row > 0)
            {
                // If in processing, stop processing
                
                // If device is active, deactivate it
                
                // If device is selected, unselect it
                
                res = [hRef.hostHandle deactivate_component:JVX_COMPONENT_AUDIO_TECHNOLOGY];
                JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP3");
                
                [hRef handle_states_components];
            }
        }
    }
    if(pickerView == _deviceSelection)
    {
        if(hRef.theStateAudioTech == JVX_STATE_ACTIVE)
        {
            jvxInt32 idxSelect = (jvxInt32) row - 1;
            if(hRef.theStateAudioDev < JVX_STATE_SELECTED)
            {
                if(idxSelect >= 0)
                {
                    res = [hRef.hostHandle select_component:JVX_COMPONENT_AUDIO_DEVICE withSelectionId:idxSelect];
                    JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP4");
                    
                    res = [hRef.hostHandle activate_component:JVX_COMPONENT_AUDIO_DEVICE];
                    JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP5");
                    
                    // Refresh properties for device before continuing
                    [hRef handle_states_components];
                    
                    res = [hRef.hostHandle set_property_selection_list:NULL
                                                          newSelection:((jvxBitField)1 << JVX_QUALITY_RESAMPLER_ID_IN)
                                                      withNameProperty:@"JVX_GENW_DEV_QRESAMPLER_IN"
                                                          forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                    JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP5.3");
                    
                    res = [hRef.hostHandle set_property_selection_list:NULL
                                                          newSelection:((jvxBitField)1 << JVX_QUALITY_RESAMPLER_ID_OUT)
                                                      withNameProperty:@"JVX_GENW_DEV_QRESAMPLER_OUT"
                                                          forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                    JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP5.4");

                }
            }
            else
            {
                if(idxSelect == -1)
                {
                    // TODO: Stop Processing
                    
                    res = [hRef.hostHandle deactivate_component:JVX_COMPONENT_AUDIO_DEVICE];
                    JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP6");
                    
                    res = [hRef.hostHandle unselect_component:JVX_COMPONENT_AUDIO_DEVICE];
                    JVX_ASSERT((res == JVX_NO_ERROR), @"VCRTP7");
                    
                    [hRef handle_states_components];
                    [hRef updateWindow];
                }

            }
            [hRef handle_states_components];
        }
    }
    
    if(pickerView == _inputChannelSelection)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            hRef.curSelectionInputChannels = row - 1;
        }
    }

    if(pickerView == _outputChannelSelection)
    {
        if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
        {
            hRef.curSelectionOutputChannels = row-1;
        }
    }
    
    if(pickerView == _samplerateSelection)
    {
        if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
        {
            NSString* srate_txt = (NSString*)[hRef.lstSamplerates objectAtIndex:row];
            jvxInt32 srate = [srate_txt intValue];
            NSString* prop_name = @"JVX_AUDEV_SRATE";
            
            [hRef.hostHandle set_property_numeric:&srate
                                    forDataFormat:JVX_DATAFORMAT_32BIT_LE
                                 withNameProperty:prop_name
                                     forComponent:JVX_COMPONENT_AUDIO_DEVICE];
        }
    }
    
    if(pickerView == _buffersizeSelection)
    {
        if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
        {
            NSString* bsize_txt = (NSString*)[hRef.lstBuffersizes objectAtIndex:row];
            jvxInt32 bsize = [bsize_txt intValue];
            NSString* prop_name = @"JVX_AUDEV_BSIZE";
            
            [hRef.hostHandle set_property_numeric:&bsize
                                    forDataFormat:JVX_DATAFORMAT_32BIT_LE
                                 withNameProperty:prop_name
                                     forComponent:JVX_COMPONENT_AUDIO_DEVICE];
        }
    }
    
    if(pickerView == _daformatSelection)
    {
        if(hRef.theStateAudioDev == JVX_STATE_ACTIVE)
        {
            jvxBitField select = ((jvxBitField)1 << row);
            
            NSString* prop_name = @"JVX_AUDEV_FORMATS";
            
            [hRef.hostHandle set_property_selection_list:NULL
                                            newSelection:select
                                        withNameProperty:prop_name
                                            forComponent:JVX_COMPONENT_AUDIO_DEVICE];
        }
    }
    [hRef updateWindow];
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// UITextField Delegate functions
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Make the keyboard disappear
- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];

    [textField resignFirstResponder];

    if(textField == _txtFldSamplerate)
    {
        jvxInt32 srate = [textField.text intValue];
        NSString* prop_name = @"JVX_AUDEV_SRATE";
        
        [hRef.hostHandle set_property_numeric:&srate
                                forDataFormat:JVX_DATAFORMAT_32BIT_LE
                             withNameProperty:prop_name
                                 forComponent:JVX_COMPONENT_AUDIO_DEVICE];
        
    }
    
    if(textField == _txtFldBuffersize)
    {
        jvxInt32 bsize = [textField.text intValue];
        NSString* prop_name = @"JVX_AUDEV_BSIZE";
        
        [hRef.hostHandle set_property_numeric:&bsize
                                forDataFormat:JVX_DATAFORMAT_32BIT_LE
                             withNameProperty:prop_name
                                 forComponent:JVX_COMPONENT_AUDIO_DEVICE];
        
    }

    
    [hRef updateWindow];
    
    return YES;
}

// Deactivate if in wrong state
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
    BOOL retVal = YES;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    if(textField == _txtFldBuffersize)
    {
        if(hRef.theStateAudioDev < JVX_STATE_SELECTED)
        {
            retVal = NO;
        }
    }
    if(textField == _txtFldSamplerate)
    {
        if(hRef.theStateAudioDev < JVX_STATE_SELECTED)
        {
            retVal = NO;
        }
    }
    return retVal;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Update the view controller window here */
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

- (void) updateWindowD:(tabBarController_rtproc*) hRef;
{
    //    jvxErrorType res = JVX_NO_ERROR;
    //    NSString* prop_name = NULL;
    NSLog(@"Update Window - viewController_rtproc");
    //  NSMutableArray* lstLoc = NULL;
    //  jvxBitField selection = 0;
    //tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    int idRow;
    
    //[_txtFldSamplerate setTextColor:[UIColor redColor]];
    //[_txtFldBuffersize setTextColor:[UIColor redColor]];
    BOOL actElems = YES;
    if(hRef.statusIsRunning)
    {
        actElems = NO;
    }
    
    [_daformatSelection setUserInteractionEnabled:actElems];
    [_technologySelection setUserInteractionEnabled:actElems];
    [_deviceSelection setUserInteractionEnabled:actElems];
    [_samplerateSelection setUserInteractionEnabled:actElems];
    [_buffersizeSelection setUserInteractionEnabled:actElems];
    [_buttonSave setUserInteractionEnabled:actElems];
    [_buttonInChanDown setUserInteractionEnabled:actElems];
    [_buttonInChanUp setUserInteractionEnabled:actElems];
    [_buttonOutChanDown setUserInteractionEnabled:actElems];
    [_buttonOutChanUp setUserInteractionEnabled:actElems];
    [_inChansToggleSelector setUserInteractionEnabled:actElems];
    [_outChansToggleSelector setUserInteractionEnabled:actElems];
    
    /* Activate / Deactivate all elements */
    
    if(actElems)
    {
        _txtFldSamplerate.text = @"--";
        _txtFldBuffersize.text = @"--";
        
        // First, obtain states
        if(hRef.theStateAudioTech == JVX_STATE_SELECTED)
        {
        }
        else
        {
            if(hRef.theStateAudioTech == JVX_STATE_ACTIVE)
            {
                if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
                {
                    _txtFldSamplerate.text = [NSString stringWithFormat:@"%i",hRef.currentSamplerate];
                    _txtFldBuffersize.text = [NSString stringWithFormat:@"%i",hRef.currentBuffersize];
                }
                else
                {
                }
            }
            else
            {
                JVX_ASSERT((0), @"VCRTP8");
            }
        }
        
        // Update the viewers
        
        [_technologySelection reloadAllComponents];
        [_technologySelection selectRow:0 inComponent:0 animated:NO];
        
        [_deviceSelection reloadAllComponents];
        if(hRef.idDeviceSelected < 0)
        {
            [_deviceSelection selectRow:0 inComponent:0 animated:NO];
        }
        else
        {
            // First entry: None!
            [_deviceSelection selectRow:(hRef.idDeviceSelected+1) inComponent:0 animated:NO];
        }
        
        [_inputChannelSelection reloadAllComponents];
        [_inputChannelSelection selectRow:hRef.curSelectionInputChannels+1 inComponent:0 animated:NO];
        
        if(hRef.curSelectionInputChannels >= 0)
        {
            if(hRef.selInputChannels & ((jvxBitField)1 << (hRef.curSelectionInputChannels)))
            {
                _inChansToggleSelector.on = YES;
            }
            else
            {
                _inChansToggleSelector.on = NO;
            }
        }
        
        [_outputChannelSelection reloadAllComponents];
        [_outputChannelSelection selectRow:hRef.curSelectionOutputChannels+1 inComponent:0 animated:NO];
        if(hRef.curSelectionOutputChannels >= 0)
        {
            if(hRef.selOutputChannels & ((jvxBitField)1 << (hRef.curSelectionOutputChannels)))
            {
                _outChansToggleSelector.on = YES;
            }
            else
            {
                _outChansToggleSelector.on = NO;
            }
        }
        
        
        [_samplerateSelection reloadAllComponents];
        idRow = [hRef.hostHandle helper_bitField_2_selection:hRef.selSamplerates];
        if(idRow < 0)
        {
            idRow = 0;
        }
        [_samplerateSelection selectRow:idRow inComponent:0 animated:NO];
        
        [_buffersizeSelection reloadAllComponents];
        idRow = [hRef.hostHandle helper_bitField_2_selection:hRef.selBuffersizes];
        if(idRow < 0)
        {
            idRow = 0;
        }
        [_buffersizeSelection selectRow:idRow inComponent:0 animated:NO];
        
        [_daformatSelection reloadAllComponents];
        if(hRef.showWrappedFormats)
        {
            idRow = [hRef.hostHandle helper_bitField_2_selection:hRef.selDataFormatsWD];
        }
        else
        {
            idRow = [hRef.hostHandle helper_bitField_2_selection:hRef.selDataFormatsGW];
        }
        
        if(idRow < 0)
        {
            idRow = 0;
        }
        [_daformatSelection selectRow:idRow inComponent:0 animated:NO];
        
        
        if(hRef.doRebuffer)
        {
            [_labelRebuffering setTextColor:[UIColor redColor]];
        }
        else
        {
            [_labelRebuffering setTextColor:[UIColor whiteColor]];
        }
        
        if(hRef.doResample)
        {
            [_labelResampling setTextColor:[UIColor redColor]];
        }
        else
        {
            [_labelResampling setTextColor:[UIColor whiteColor]];
        }
        
        // Check current system status
        jvxBool isReady = true;
        jvxBool isReadySingle = false;
        NSString* textPostIfNot = NULL;
        
        _labelReasonFail.text = @"READY";
        [hRef.hostHandle check_ready:JVX_COMPONENT_AUDIO_DEVICE returnIsReadyHere:&isReadySingle withReasonIfNotOnReturn:&textPostIfNot];
        if(!isReadySingle)
        {
            _labelReasonFail.text = @"DEVICE NOT READY: ";
            _labelReasonFail.text = [_labelReasonFail.text stringByAppendingString:textPostIfNot];
            isReady = false;
        }
        
        isReadySingle = false;
        textPostIfNot = NULL;
        
        [hRef.hostHandle check_ready:JVX_COMPONENT_AUDIO_NODE returnIsReadyHere:&isReadySingle withReasonIfNotOnReturn:&textPostIfNot];
        if(!isReadySingle)
        {
            _labelReasonFail.text = @"NODE NOT READY: ";
            _labelReasonFail.text = [_labelReasonFail.text stringByAppendingString:textPostIfNot];
            isReady = false;
        }
        
        if(isReady)
        {
            [_labelReasonFail setTextColor:[UIColor greenColor]];
            
            // Set start button to be active
            UIImage *btnImage = [UIImage imageNamed:@"jvx_play.png"];
            [_buttonStart setBackgroundImage:btnImage forState:UIControlStateNormal];
        }
        else
        {
            [_labelReasonFail setTextColor:[UIColor redColor]];
            // Set start button to be inactive
            UIImage *btnImage = [UIImage imageNamed:@"jvx_notready.png"];
            [_buttonStart setBackgroundImage:btnImage forState:UIControlStateNormal];
        }
    }
    else
    {
        [_inputChannelSelection selectRow:hRef.curSelectionInputChannels+1 inComponent:0 animated:NO];
        [_outputChannelSelection selectRow:hRef.curSelectionOutputChannels+1 inComponent:0 animated:NO];
        
        UIImage *btnImage = [UIImage imageNamed:@"jvx_stop.png"];
        [_buttonStart setBackgroundImage:btnImage forState:UIControlStateNormal];
    }
}


@end
