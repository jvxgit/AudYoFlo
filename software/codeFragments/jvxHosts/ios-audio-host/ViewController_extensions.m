//
//  ViewController_extensions.m
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 06.01.16.
//  Copyright © 2016 Javox Solutions. All rights reserved.
//

#import "ViewController_extensions.h"
#import "jvxIosLocalMediaItem.h"

@interface ViewController_extensions ()

@end

@implementation ViewController_extensions

- (void)viewDidLoad
{
    UIFontDescriptor * fontD = NULL;
    
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.selectionSourcesRoutes.dataSource = self;
    self.selectionSourcesRoutes.delegate = self;

    self.selectionInputFiles.dataSource = self;
    self.selectionInputFiles.delegate = self;

    self.selectionOutputFiles.dataSource = self;
    self.selectionOutputFiles.delegate = self;

    self.txtFldSamplerateOutfile.delegate = self;
    self.textFieldChannelsOutfile.delegate = self;
    self.textFieldTitleOutfile.delegate = self;
    
    // Do any additional setup after loading the view.
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    SET_FONT_SIZE_ONE_ELEMENT(_label_1, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_2, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_3, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_4, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_5, hRef.fontSizeLarge);
    
    SET_FONT_SIZE_ONE_ELEMENT(_labelProgressFiles, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_txtFldSamplerateOutfile, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_textFieldTitleOutfile, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_labelInLevel, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_labelOutLevel, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_labelInLevelCurrent, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_labelOutLevelCurrent, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_labelChannels, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_labelSamplerate, hRef.fontSizeMedium);
    
    SET_FONT_SIZE_ONE_ELEMENT(_labelCurrentLoad, hRef.fontSizeSmall);
    SET_FONT_SIZE_ONE_ELEMENT(_label_txtLoad, hRef.fontSizeSmall);
    SET_FONT_SIZE_ONE_ELEMENT(_label_txtRemote, hRef.fontSizeSmall);

    [hRef register_delegate:self];

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void) setInGain:(float) val
         withHRef:(tabBarController_rtproc*) hRef
{
    int i;
    if(hRef.lstInputGains.count > 0)
    {
        val -= 0.5;
        float valDB = val * JVX_MAX_VAL_DB_SLIDER/0.5;
        val = pow(10.0, valDB/20.0);
        
        if(hRef.curSelectionInputChannels >= 0)
        {
            jvxSize idSet = (hRef.curSelectionInputChannels);
            [hRef.lstInputGains setObject:[NSNumber numberWithFloat:val] atIndexedSubscript:idSet];
            if(hRef.statusIsRunning)
            {
                if(hRef.selInputChannels & ((jvxBitField)1 << idSet))
                {
                    int cnt = 0;
                    for(i = 0; i < idSet; i++)
                    {
                        if(hRef.selInputChannels & (jvxBitField) 1 << i)
                        {
                            cnt++;
                        }
                    }
                    NSString* prop_name = @"JVX_GENW_DEV_IGAIN";
                    [hRef.hostHandle set_property_numeric:&val forDataFormat:JVX_DATAFORMAT_DATA withOffsetArray:cnt
                                         withNameProperty:prop_name forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                }
            }
        }
        else
        {
            int cnt = 0;
            for(i = 0; i < hRef.lstInputGains.count; i++)
            {
                [hRef.lstInputGains setObject:[NSNumber numberWithFloat:val] atIndexedSubscript:i];
                
                if(hRef.statusIsRunning)
                {
                    if(hRef.selInputChannels & ((jvxBitField)1 << i))
                    {
                        NSString* prop_name = @"JVX_GENW_DEV_IGAIN";
                        [hRef.hostHandle set_property_numeric:&val forDataFormat:JVX_DATAFORMAT_DATA withOffsetArray:cnt
                                             withNameProperty:prop_name forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                        cnt++;
                    }
                }
            }
        }
    }
}

- (void) setOutGain:(float) val
           withHRef:(tabBarController_rtproc*) hRef;
{
    int i;
    
    if(hRef.lstOutputGains.count > 0)
    {
        
        val -= 0.5;
        float valDB = val * JVX_MAX_VAL_DB_SLIDER/0.5;
        val = pow(10.0, valDB/20.0);
        
        if(hRef.curSelectionOutputChannels >= 0)
        {
            jvxSize idSet = (hRef.curSelectionOutputChannels);
            
            [hRef.lstOutputGains setObject:[NSNumber numberWithFloat:val] atIndexedSubscript:idSet];
            if(hRef.statusIsRunning)
            {
                if(hRef.selOutputChannels & ((jvxBitField)1 << idSet))
                {
                    int cnt = 0;
                    for(i = 0; i < idSet; i++)
                    {
                        if(hRef.selOutputChannels & (jvxBitField) 1 << i)
                        {
                            cnt++;
                        }
                    }
                    
                    
                    NSString* prop_name = @"JVX_GENW_DEV_OGAIN";
                    [hRef.hostHandle set_property_numeric:&val forDataFormat:JVX_DATAFORMAT_DATA withOffsetArray:cnt
                                         withNameProperty:prop_name forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                }
            }
        }
        else
        {
            int cnt = 0;
            for(i = 0; i < hRef.lstOutputGains.count; i++)
            {
                [hRef.lstOutputGains setObject:[NSNumber numberWithFloat:val] atIndexedSubscript:i];
                
                if(hRef.statusIsRunning)
                {
                    if(hRef.selOutputChannels & ((jvxBitField)1 << i))
                    {
                        NSString* prop_name = @"JVX_GENW_DEV_OGAIN";
                        [hRef.hostHandle set_property_numeric:&val forDataFormat:JVX_DATAFORMAT_DATA withOffsetArray:cnt
                                             withNameProperty:prop_name forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                        cnt++;
                    }
                }
            }
        }
    }
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (int)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    int retVal = 0;
    if(pickerView == _selectionSourcesRoutes)
    {
        retVal = 3;
    }
    
    if(pickerView == _selectionInputFiles)
    {
        retVal = 2;
    }

    if(pickerView == _selectionOutputFiles)
    {
        retVal = 1;
    }
    
    return(retVal);
}

- (int)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    int retVal = 1;
    
    if(pickerView == _selectionSourcesRoutes)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            switch(component)
            {
                case 0:
                    retVal = (int)hRef.lstInputSources.count;
                    break;
                case 1:
                    retVal = (int)hRef.lstOutputSources.count;
                    break;
                case 2:
                    retVal = (int)hRef.lstOutputSpeaker.count;
                    break;
                defaut:
                    JVX_ASSERT((0), @"VCEX6");
            }
        }
    }
    
    if(pickerView == _selectionInputFiles)
    {
        if(component == 0)
        {
            retVal += hRef.localLstArtists.count;
        }
        if(component == 1)
        {
            retVal += hRef.localLstItemsForArtist.count;
        }
    }
    
    if(pickerView == _selectionOutputFiles)
    {
        retVal += hRef.lstOutputFileNames.count;
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
    
    
    if(pickerView == _selectionSourcesRoutes)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            switch(component)
            {
                case 0:
                    retVal = [hRef.lstInputSources objectAtIndex:row];
                    if(hRef.selInputSource & (jvxBitField)1 << row)
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
                    break;
                case 1:
                    retVal = [hRef.lstOutputSources objectAtIndex:row];
                    if(hRef.selOutputSource & (jvxBitField)1 << row)
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
                    break;
                case 2:
                    retVal = [hRef.lstOutputSpeaker objectAtIndex:row];
                    if(hRef.selOutputSpeaker & (jvxBitField)1 << row)
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
                    break;
                default:
                    JVX_ASSERT((0), @"VCEX1");
            }
        }
    }
    
    if(pickerView == _selectionInputFiles)
    {
        retVal = @"None";
        row -= 1;
        if(component == 0)
        {
            if(row < hRef.localLstArtists.count)
            {
                retVal = [hRef.localLstArtists objectAtIndex:row];
            }
        }
        if(component == 1)
        {
            if(row < hRef.localLstItemsForArtist.count)
            {
                jvxIosLocalMediaItem* oneItem = [hRef.localLstItemsForArtist objectAtIndex:row];
                retVal = oneItem.nmTitle;
            }
        }
    }

    if(pickerView == _selectionOutputFiles)
    {
        retVal = @"None";
        row -= 1;
        
        if(row >= 0)
        {
            if(row < hRef.lstOutputFileNames.count)
            {
                retVal = [hRef.lstOutputFileNames objectAtIndex:row];
                retVal = [hRef.hostHandle helper_tagExprToTitle:retVal];
            }
        }
    }

    return(retVal);
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
    wd = wd / (float) divisor;
    float ht = hRef.fontSizeMedium;
    UILabel* lab = [hRef createLabelEntryPickerViewer:rowItem
                                            withWidth:wd
                                           withHeight:ht
                                           doShowBold:showBold
                                         doShowItalic:showIt
                                            withColor:theColor];
    // Return the label.
    return lab;
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    jvxErrorType res = JVX_NO_ERROR;
    NSString* prop_name;
    
    if(pickerView == _selectionSourcesRoutes)
    {
        jvxBitField selection = (jvxBitField)1 << row;
        switch(component)
        {
            case 0:
                prop_name = @"JVX_GENW/JVX_IOS_AUDEV_INPUT_SOURCE";
                res = [hRef.hostHandle set_property_selection_list:NULL
                                                      newSelection:selection
                                                  withNameProperty:prop_name
                                                      forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                JVX_ASSERT((res == JVX_NO_ERROR), @"VCEX2");
                break;
            case 1:
                prop_name = @"JVX_GENW/JVX_IOS_AUDEV_OUTPUT_SOURCE";
                res = [hRef.hostHandle set_property_selection_list:NULL
                                                      newSelection:selection
                                                  withNameProperty:prop_name
                                                      forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                JVX_ASSERT((res == JVX_NO_ERROR), @"VCEX3");
                break;
            case 2:
                prop_name = @"JVX_GENW/JVX_IOS_AUDEV_OUTPUT_SPEAKER";
                res = [hRef.hostHandle set_property_selection_list:NULL
                                                      newSelection:selection
                                                  withNameProperty:prop_name
                                                      forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                JVX_ASSERT((res == JVX_NO_ERROR), @"VCEX4");
                break;
            default:
                JVX_ASSERT((0), @"VCEX5");
        }
        
        [hRef updateWindow];
    }
    
    if(pickerView == _selectionInputFiles)
    {
        row -= 1;
        if(component == 0)
        {
            hRef.selectionInputFile_localArtist = row;
            hRef.selectionInputFile_localTitle = -1;
            [hRef refreshLocalTitles:false];
        }
        if(component == 1)
        {
            hRef.selectionInputFile_localTitle = row;
        }
        [hRef updateWindow];
    }
    
    if(pickerView == _selectionOutputFiles)
    {
        row -= 1;
        hRef.selectionOutputFile = row;
        [hRef updateWindow];
    }
}


- (IBAction)triggerRemoveOutputFile:(id)sender
{
    jvxErrorType res = JVX_NO_ERROR;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    // Obtain list of input file names (property of technology)
    NSString* prop_name = @"JVX_GENW_COMMAND";
    if(hRef.selectionOutputFile >= 0)
    {
        NSString* command = [NSString stringWithFormat:@"remOutputFile(%i);", hRef.selectionOutputFile];
        res = [hRef.hostHandle set_property_string :command
                               withNameProperty: prop_name
                                   forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
        if(res == JVX_NO_ERROR)
        {
            hRef.selectionOutputFile = -1; // Select new file
            hRef.curSelectionOutputChannels = -1;
        }
        [hRef updateWindow];
    }
}

- (IBAction)triggerAddOutputFile:(id)sender
{
    jvxErrorType res = JVX_NO_ERROR;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    // Obtain list of input file names (property of technology)
    NSString* prop_name = @"JVX_GENW_COMMAND";
    
    NSString* txt = [hRef.hostHandle helper_tagsToExpr:NULL
                             withTitle:hRef.nameOutputFileAdd
                              withDate:NULL];
    NSString* command = [NSString stringWithFormat:@"addOutputFile(%@,%@);", @"", txt];
    res = [hRef.hostHandle set_property_string :command
                               withNameProperty: prop_name
                                   forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
    if(res == JVX_NO_ERROR)
    {
        hRef.selectionOutputFile = hRef.lstOutputFileNames.count;
    }
    else
    {
        NSString* errorMessage = NULL;
        if(res == JVX_ERROR_DUPLICATE_ENTRY)
        {
            errorMessage = [NSString stringWithFormat:@"Error: File with tag %@ is already in the list of active outputs.",txt];
        }
        else
        {
            errorMessage = [NSString stringWithFormat:@"Error: %@", [hRef.hostHandle helper_errorToErrorDescrption:res]];
        }
        [hRef report_to_viewport:[NSNumber numberWithInteger:JVX_IOS_BWD_COMMAND_TEXT_MESSAGE]
                     withObject:errorMessage];
    }
    [hRef updateWindow];
}

- (IBAction)selectionRemoteOnOff:(id)sender
{
}

- (IBAction)inGainChanged:(id)sender
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    float val = [_sliderInLevel value];
    if(val == 1.0)
    {
        val = 0.5;
    }
    [self setInGain:val withHRef:hRef];
    [self updateWindowS:hRef];
}

- (IBAction)outGainChanged:(id)sender
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    float val = [_sliderOutLevel value];
    if(val == 1.0)
    {
        val = 0.5;
    }
    [self setOutGain: val withHRef:hRef];
    [self updateWindowS:hRef];
}

- (void) updateWindowD:(tabBarController_rtproc*) hRef;
{
    int idRow;
    
    [_selectionSourcesRoutes reloadAllComponents];
    
    idRow = [hRef.hostHandle helper_bitField_2_selection:hRef.selInputSource];
    if(idRow < 0)
    {
        idRow = 0;
    }
    [_selectionSourcesRoutes selectRow:idRow inComponent:0 animated:NO];

    idRow = [hRef.hostHandle helper_bitField_2_selection:hRef.selOutputSource];
    if(idRow < 0)
    {
        idRow = 0;
    }
    [_selectionSourcesRoutes selectRow:idRow inComponent:1 animated:NO];
    
    idRow = [hRef.hostHandle helper_bitField_2_selection:hRef.selOutputSpeaker];
    if(idRow < 0)
    {
        idRow = 0;
    }
    [_selectionSourcesRoutes selectRow:idRow inComponent:2 animated:NO];
    
    
    NSString* date = NULL;
        
    [_switchActivateInput setUserInteractionEnabled:false];
    [_selectionInputFiles setUserInteractionEnabled:false];
    _switchActivateInput.on = NO;

    [_selectionOutputFiles setUserInteractionEnabled:false];
    
    _textFieldChannelsOutfile.text = @"--";
    _txtFldSamplerateOutfile.text = @"--";
    _textFieldTitleOutfile.text = hRef.nameOutputFileAdd;
    
    [_selectionInputFiles reloadAllComponents];
    [_selectionOutputFiles reloadAllComponents];
    
    // New list is not visible before
    [_selectionOutputFiles selectRow:hRef.selectionOutputFile + 1 inComponent:0 animated:NO];

    // First, obtain states
    if(hRef.theStateAudioTech == JVX_STATE_ACTIVE)
    {
        if(hRef.theStateAudioDev >= JVX_STATE_ACTIVE)
        {
            [_selectionInputFiles setUserInteractionEnabled:true];
            [_selectionInputFiles selectRow:hRef.selectionInputFile_localArtist + 1 inComponent:0 animated:NO];
            [_selectionInputFiles selectRow:hRef.selectionInputFile_localTitle + 1 inComponent:1 animated:NO];

            jvxInt32 idInInputFileList = -1;
            date = [hRef idInInputFileListTech:false
                    withIdOnReturn:&idInInputFileList];
            
            [_switchActivateInput setUserInteractionEnabled:true];
            [_selectionInputFiles setUserInteractionEnabled:true];
            
            if(idInInputFileList >= 0)
            {
                _switchActivateInput.on = YES;
            }
            
            
            [_selectionOutputFiles setUserInteractionEnabled:true];
            
            if(hRef.selectionOutputFile >= 0)
            {
                //[_selectionOutputFiles selectRow:hRef.selectionOutputFile + 1 inComponent:0 animated:NO];
                _textFieldChannelsOutfile.text = [NSString stringWithFormat:@"%i", hRef.numChannelOutputFile];
                _txtFldSamplerateOutfile.text = [NSString stringWithFormat:@"%i", hRef.rateOutputFile];
            }
        }
    }
    if(date == NULL)
    {
        date = @"--";
    }
    _labelProgressFiles.text = date;
    
    
    [_levelMeterInput setTintColor:[UIColor greenColor]];
    [_levelMeterInput setProgress:0.0];
 
    [_levelMeterOutput setTintColor:[UIColor greenColor]];
    [_levelMeterOutput setProgress:0.0];
 
    [_labelInLevelCurrent setText:@"--"];
    [_labelOutLevelCurrent setText:@"--"];

    [self updateWindowS:hRef];
}

- (void) updateWindowS:(tabBarController_rtproc*) hRef;
{
    NSString* str = NULL;
    int idSelect = 0;
    
    // View level of channels
    idSelect = hRef.curSelectionInputChannels;
    if(idSelect < 0)
    {
        if(hRef.lstInputGains.count > 0)
        {
            // If no channel is selected, show the level of the first channel
            idSelect = 0;
        }
        else
        {
            idSelect = -1;
        }
    }
    
    if(idSelect >= 0)
    {
        float val = [(NSNumber*)[hRef.lstInputGains objectAtIndex:idSelect] floatValue];
        val = 20.0 * log10(val + JVX_EPS);
        str = [NSString stringWithFormat:@"%.1f dB", val];
        val = val / JVX_MAX_VAL_DB_SLIDER * 0.5;
        _sliderInLevel.value = val + 0.5;

    }
    else
    {
        str = @"--";
    }
    
    [_labelInLevel setText:str];
    
    // View level of channels
    idSelect = hRef.curSelectionOutputChannels;
    if(idSelect < 0)
    {
        if(hRef.lstOutputGains.count > 0)
        {
            // If no channel is selected, show the level of the first channel
            idSelect = 0;
        }
        else
        {
            idSelect = -1;
        }
    }
    
    if(idSelect >= 0)
    {
        float val = [(NSNumber*)[hRef.lstOutputGains objectAtIndex:idSelect] floatValue];
        val = 20.0 * log10(val + JVX_EPS);
        str = [NSString stringWithFormat:@"%.1f dB", val];
        val = val / JVX_MAX_VAL_DB_SLIDER * 0.5;
        _sliderOutLevel.value = val + 0.5;
    }
    else
    {
        str = @"--";
    }
    
    [_labelOutLevel setText:str];
}

- (void) startRun:(tabBarController_rtproc*) hRef
{
    jvxErrorType res = JVX_NO_ERROR;
    NSString* prop_name = @"";
    
    thePropHandle = NULL;
    id_inputLevelAvrg = 0;
    id_outputLevelAvrg = 0;
    id_inputLevelMax = 0;
    id_outputLevelMax = 0;
    id_currentLoad = 0;

    prop_name = @"JVX_GENW_DEV_ILEVEL";
    res = [hRef.hostHandle get_property_id:prop_name
                              forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                returnedId:&id_inputLevelAvrg
                            returnedHandle:&thePropHandle];
    prop_name = @"JVX_GENW_DEV_OLEVEL";
    res = [hRef.hostHandle get_property_id:prop_name
                              forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                returnedId:&id_outputLevelAvrg
                            returnedHandle:NULL];

    prop_name = @"JVX_GENW_DEV_ILEVEL_MAX";
    res = [hRef.hostHandle get_property_id:prop_name
                              forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                returnedId:&id_inputLevelMax
                            returnedHandle:NULL];
    prop_name = @"JVX_GENW_DEV_OLEVEL_MAX";
    res = [hRef.hostHandle get_property_id:prop_name
                              forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                returnedId:&id_outputLevelMax
                            returnedHandle:NULL];
    
    prop_name = @"JVX_GENW_DEV_LOAD";
    res = [hRef.hostHandle get_property_id:prop_name
                              forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                returnedId:&id_currentLoad
                            returnedHandle:NULL];
}

- (void) stopRun:(tabBarController_rtproc*) hRef
{
    thePropHandle = NULL;
    id_inputLevelAvrg = 0;
    id_outputLevelAvrg = 0;
    id_inputLevelMax = 0;
    id_outputLevelMax = 0;
    id_currentLoad = 0;
}

- (void) updateWindowP:(tabBarController_rtproc*) hRef
{
    int i;
    float val = 0;
    jvxInt32 offsetIn = -1;
    jvxInt32 offsetOut = -1;
    int cnt;
    jvxBool overload = false;
    jvxErrorType res = JVX_NO_ERROR;
    
    if(hRef.curSelectionInputChannels >= 0)
    {
        offsetIn = hRef.curSelectionInputChannels;
    }
    if(offsetIn >= 0)
    {
        if(hRef.selInputChannels & ((jvxBitField)1 << offsetIn))
        {
            cnt = 0;
            for(i = 0; i < offsetIn; i++)
            {
                if(hRef.selInputChannels & ((jvxBitField)1 << i))
                {
                    cnt++;
                }
            }
            offsetIn = cnt;
        }
        else
        {
            offsetIn = -2;
        }
    }
    
    if(hRef.curSelectionOutputChannels >= 0)
    {
        offsetOut = hRef.curSelectionOutputChannels;
    }
    if(offsetOut >= 0)
    {
        if(hRef.selOutputChannels & ((jvxBitField)1 << offsetOut))
        {
            cnt = 0;
            for(i = 0; i < offsetOut; i++)
            {
                if(hRef.selOutputChannels & ((jvxBitField)1 << i))
                {
                    cnt++;
                }
            }
            offsetOut = cnt;
        }
        else
        {
            offsetOut = -2;
        }
    }

    if(offsetIn == -1)
    {
        offsetIn = 0;
    }

    if(offsetOut == -1)
    {
        offsetOut = 0;
    }

    val = 0;
    
    if(offsetIn >= 0)
    {
        // Read input params
        res = [hRef.hostHandle get_property_direct:&val
                                       forCategory:false
                                   withNumberElems:1
                                          ofFormat:JVX_DATAFORMAT_DATA
                                            withId:id_inputLevelAvrg
                                        withOffset:offsetIn
                                 targetOnlyContent:true
                                 withValidReturned:NULL
                                        withHandle:thePropHandle];
    }
    val = 10 * log10(val+JVX_EPS);
    val = val/60 + 1;
    val = JVX_MIN(val, 1.0);
    val = JVX_MAX(val, 0);
    [_levelMeterInput setProgress:val];
    
    val = 0;
    if(offsetIn >= 0)
    {
    
        // Read input params
        res = [hRef.hostHandle get_property_direct:&val
                                       forCategory:false
                                   withNumberElems:1
                                          ofFormat:JVX_DATAFORMAT_DATA
                                            withId:id_inputLevelMax
                                        withOffset:offsetIn
                                 targetOnlyContent:true
                                 withValidReturned:NULL
                                        withHandle:thePropHandle];
    }
    if(val > JVX_MAX_LEVEL)
    {
        overload = true;
    }
    
    val = 10 * log10(val+JVX_EPS);
    
    [_labelInLevelCurrent setText:[NSString stringWithFormat:@"%.1f dB", val]];
    
    if(overload)
    {
        [_labelInLevelCurrent setTextColor:[UIColor colorWithRed:1.0 green:0.0 blue:0.0 alpha:1.0]];
        [_levelMeterInput setTintColor:[UIColor redColor]];
        [_levelMeterInput setProgress:1.0];
    }
    else
    {
        [_labelInLevelCurrent setTextColor:[UIColor blackColor]];
        [_levelMeterInput setTintColor:[UIColor greenColor]];
    }
    if(offsetIn >= 0)
    {
        val = 0.0;
        
        // Read input params
        res = [hRef.hostHandle set_property_direct:&val
                                       forCategory:false
                                   withNumberElems:1
                                          ofFormat:JVX_DATAFORMAT_DATA
                                            withId:id_inputLevelMax
                                        withOffset:offsetIn
                                 targetOnlyContent:true
                                 withValidReturned:NULL
                                        withHandle:thePropHandle];
    }
    
    
    val = 0;
    
    if(offsetOut >= 0)
    {
        // Read output params
        res = [hRef.hostHandle get_property_direct:&val
                                       forCategory:false
                                   withNumberElems:1
                                          ofFormat:JVX_DATAFORMAT_DATA
                                            withId:id_outputLevelAvrg
                                        withOffset:offsetOut
                                 targetOnlyContent:true
                                 withValidReturned:NULL
                                        withHandle:thePropHandle];
    }
    val = 10 * log10(val+JVX_EPS);
    val = val/60 + 1;
    val = JVX_MIN(val, 1.0);
    val = JVX_MAX(val, 0);
    [_levelMeterOutput setProgress:val];

    
    val = 0;
    if(offsetOut >= 0)
    {
        
        // Read input params
        res = [hRef.hostHandle get_property_direct:&val
                                       forCategory:false
                                   withNumberElems:1
                                          ofFormat:JVX_DATAFORMAT_DATA
                                            withId:id_outputLevelMax
                                        withOffset:offsetOut
                                 targetOnlyContent:true
                                 withValidReturned:NULL
                                        withHandle:thePropHandle];
    }
    
    overload = false;
    if(val > JVX_MAX_LEVEL)
    {
        overload = true;
    }
    
    val = 10 * log10(val+JVX_EPS);
    
    [_labelOutLevelCurrent setText:[NSString stringWithFormat:@"%.1f dB", val]];
    
    if(overload)
    {
        [_labelOutLevelCurrent setTextColor:[UIColor colorWithRed:1.0 green:0.0 blue:0.0 alpha:1.0]];
        [_levelMeterOutput setTintColor:[UIColor redColor]];
        [_levelMeterOutput setProgress:1.0];
    }
    else
    {
        [_labelOutLevelCurrent setTextColor:[UIColor blackColor]];
        [_levelMeterOutput setTintColor:[UIColor greenColor]];

    }
    if(offsetOut >= 0)
    {
        val = 0.0;
        
        // Read input params
        res = [hRef.hostHandle set_property_direct:&val
                                       forCategory:false
                                   withNumberElems:1
                                          ofFormat:JVX_DATAFORMAT_DATA
                                            withId:id_outputLevelMax
                                        withOffset:offsetOut
                                 targetOnlyContent:true
                                 withValidReturned:NULL
                                        withHandle:thePropHandle];
    }
    
    val = 0;
    
    // Read output params
    res = [hRef.hostHandle get_property_direct:&val
                                   forCategory:false
                               withNumberElems:1
                                      ofFormat:JVX_DATAFORMAT_DATA
                                        withId:id_currentLoad
                                    withOffset:0
                             targetOnlyContent:true
                             withValidReturned:NULL
                                    withHandle:thePropHandle];
    [_labelCurrentLoad setText:[NSString stringWithFormat:@"%.0f %%", val]];
    
    
    // Input file progress
    jvxInt32 idInInputFileList = -1;
    [hRef idInInputFileListTech:false
                 withIdOnReturn:&idInInputFileList];
    if(idInInputFileList >= 0)
    {
        
        if(hRef.statusIsRunning)
        {
            NSString* prop_name = @"JVX_GENW_INFILE_PROGRESS";
            res = [hRef.hostHandle get_property_numeric:&val
                                          forDataFormat:JVX_DATAFORMAT_DATA
                                        withOffsetArray:idInInputFileList
                                       withNameProperty:prop_name
                                           forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY ];
            [_labelProgressFiles setText:[NSString stringWithFormat:@"Progress %.0f %%", val*100.0]];
        }
    }
}

- (IBAction)activatedCurrentFileInput:(id)sender
{
    jvxErrorType res = JVX_NO_ERROR;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    // Obtain list of input file names (property of technology)
    NSString* prop_name = @"JVX_GENW_COMMAND";

    jvxInt32 idInInputFileList = -1;
    [hRef idInInputFileListTech:false
                 withIdOnReturn:&idInInputFileList];
    
    // Check if the current file id points to a file that was added to the list of active files before
    if(idInInputFileList >= 0)
    {
        
        if(hRef.statusIsRunning)
        {
            jvxInt16 val = 1;
            // Play/pause switch
            if(_switchActivateInput.on)
            {
                NSString* prop_name = @"JVX_GENW_INFILE_PLAY";
                res = [hRef.hostHandle set_property_numeric:&val
                                              forDataFormat:JVX_DATAFORMAT_16BIT_LE
                                            withOffsetArray:idInInputFileList
                                           withNameProperty:prop_name
                                               forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY ];
            }
            else
            {
                NSString* prop_name = @"JVX_GENW_INFILE_PAUSE";
                res = [hRef.hostHandle set_property_numeric:&val
                                              forDataFormat:JVX_DATAFORMAT_16BIT_LE
                                            withOffsetArray:idInInputFileList
                                           withNameProperty:prop_name
                                               forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY ];
            }
            // JVX_GENW_INFILE_PROGRESS
        }
        else
        {
            NSString* command = [NSString stringWithFormat:@"remInputFile(%i);", idInInputFileList];
            res = [hRef.hostHandle set_property_string :command
                                       withNameProperty: prop_name
                                           forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
            
            hRef.curSelectionInputChannels = -1;
            
            [hRef updateWindow];
        }
    }
    else
    {
        if(!hRef.statusIsRunning)
        {
            // Add input file
            NSString* fName = NULL;
            if(hRef.selectionInputFile_localTitle >= 0)
            {
                if(hRef.selectionInputFile_localTitle < hRef.localLstItemsForArtist.count)
                {
                    jvxIosLocalMediaItem* it = [hRef.localLstItemsForArtist objectAtIndex:hRef.selectionInputFile_localTitle];
                    fName = it.nmPath;
                }
            }
            
            if(fName)
            {
                NSString* command = [NSString stringWithFormat:@"addInputFile(%@);", fName];
                res = [hRef.hostHandle set_property_string :command
                                           withNameProperty: prop_name
                                               forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
                
                [hRef updateWindow];
            }
        }
    }
}

// Make the keyboard disappear
- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    [textField resignFirstResponder];
    
    if(textField == _txtFldSamplerateOutfile)
    {
        if(_selectionOutputFiles >= 0)
        {
            jvxInt32 srate = [_txtFldSamplerateOutfile.text intValue];
            NSString* prop_name = @"JVX_GENW_OUTFILE_RATE";
        
            [hRef.hostHandle set_property_numeric:&srate
                                    forDataFormat:JVX_DATAFORMAT_32BIT_LE
                                  withOffsetArray:hRef.selectionOutputFile
                                 withNameProperty:prop_name
                                     forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
            [hRef updateWindow];
        }
    }
    
    if(textField == _textFieldChannelsOutfile)
    {
        
        if(_selectionOutputFiles >= 0)
        {
            jvxInt16 chans = [_textFieldChannelsOutfile.text intValue];
            NSString* prop_name = @"JVX_GENW_OUTFILE_CHANS";
            
            [hRef.hostHandle set_property_numeric:&chans
                                    forDataFormat:JVX_DATAFORMAT_16BIT_LE
                                  withOffsetArray:hRef.selectionOutputFile
                                 withNameProperty:prop_name
                                     forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
            [hRef updateWindow];
        }

    }

    if(textField == _textFieldTitleOutfile)
    {
        hRef.nameOutputFileAdd = _textFieldTitleOutfile.text;
        // Do not update window to prevent removal of text
    }

    return YES;
}

// Deactivate if in wrong state
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
    BOOL retVal = YES;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    if(textField == _txtFldSamplerateOutfile)
    {
        if(hRef.theStateAudioDev < JVX_STATE_SELECTED)
        {
            retVal = NO;
        }
    }
    
    if(textField == _textFieldTitleOutfile)
    {
        if(hRef.theStateAudioDev < JVX_STATE_SELECTED)
        {
            retVal = NO;
        }
    }

    if(textField == _textFieldChannelsOutfile)
    {
        if(hRef.theStateAudioDev < JVX_STATE_SELECTED)
        {
            retVal = NO;
        }
    }
    return retVal;
}

@end
