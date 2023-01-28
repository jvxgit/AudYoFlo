//
//  tabBarControllerViewController.m
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 06.01.16.
//  Copyright © 2016 Javox Solutions. All rights reserved.
//

#import "tabBarController_rtproc.h"
#import <AVFoundation/AVFoundation.h>
#import "jvxIosAudioMediaItem.h"
#import "jvxIosLocalMediaItem.h"

@interface tabBarController_rtproc ()

@end

@implementation tabBarController_rtproc

- (void)viewDidLoad
{
    int i;
    
    // +++++++++###########+++++++++###########+++++++++###########

    /* initilize the underlying host */
    _hostHandle = [[jvxIosHost alloc] init];
    
    // +++++++++###########+++++++++###########+++++++++###########
   
    lstOfDelegates = [[NSMutableArray alloc] init];

    // +++++++++###########+++++++++###########+++++++++###########

    // - - - - - - - - - - - - - - - - - - - - - - - - -
    // Storage for all parameters shown in sub ui I
    // - - - - - - - - - - - - - - - - - - - - - - - - -
    _lstTechs = [[NSMutableArray alloc] init];
    _lstDevs = [[NSMutableArray alloc] init];
    _lstInputChannels = [[NSMutableArray alloc] init];
    _lstOutputChannels = [[NSMutableArray alloc] init];
    _lstSamplerates = [[NSMutableArray alloc] init];
    _lstBuffersizes = [[NSMutableArray alloc] init];
    _lstDataformatsGW = [[NSMutableArray alloc] init];
    _lstDataformatsWD = [[NSMutableArray alloc] init];
    _idDeviceSelected = 0;
    _selBuffersizes = 0;
    _selSamplerates = 0;
    _selInputChannels = 0;
    _selOutputChannels = 0;
    _selDataFormatsGW = 0;
    _selDataFormatsWD = 0;
    _doRebuffer = false;
    _doResample = false;
    _currentBuffersize = 128;
    _currentSamplerate = 48000;
    _curSelectionInputChannels = -1;
    _curSelectionOutputChannels = -1;
    
    // - - - - - - - - - - - - - - - - - - - - - - - - -
    // Storage for all parameters shown in sub ui II
    // - - - - - - - - - - - - - - - - - - - - - - - - -
    _lstInputSources = [[NSMutableArray alloc] init];
    _lstOutputSources = [[NSMutableArray alloc] init];
    _lstOutputSpeaker = [[NSMutableArray alloc] init];
    _selInputSource = 0;
    _selOutputSource = 0;
    _selOutputSpeaker = 0;
    _lstInputGains = [[NSMutableArray alloc] init];
    _lstOutputGains = [[NSMutableArray alloc] init];
        
    // - - - - - - - - - - - - - - - - - - - - - - - - -
    // Storage for all parameters shown in sub ui I-IV
    // - - - - - - - - - - - - - - - - - - - - - - - - -
    _theStateHost = JVX_STATE_NONE;
    _theStateAudioTech = JVX_STATE_NONE;
    _theStateAudioDev = JVX_STATE_NONE;
    _theStateAudioNode = JVX_STATE_NONE;

    // Manage paths to "Documents" file entry point
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    _documentsDirectory = [paths objectAtIndex:0];
    
    // +++++++++###########+++++++++###########+++++++++###########

    /* Start the host by calling all required functions in jvxIosHost */
    [_hostHandle initialize:(NSObject*)self withFilePathSpec:_documentsDirectory];

    NSString* model = [[UIDevice currentDevice] model];
    
    if([model isEqualToString:@"iPad"])
    {
        _fontSizeLarge = 27;
        _fontSizeMedium = 24;
        _fontSizeSmall = 21;
        _fontSizeTiny = 15;
        _fieldWidthChannels = 30;
    }
    else
    {
        _fontSizeLarge = 16;
        _fontSizeMedium = 15;
        _fontSizeSmall = 12;
        _fontSizeTiny = 10;
        _fieldWidthChannels = 12;
    }

    [super viewDidLoad];
    
    // +++++++++###########+++++++++###########+++++++++###########

    // Do any additional setup after loading the view.
    // Show all sub uis to make sure that all "viewDidLoad" functions have been called
    for(i = 0; i < self.childViewControllers.count; i++)
    {
        
        UIViewController* oneController = (UIViewController*)[self.childViewControllers objectAtIndex:i];
        if(oneController)
        {
            [oneController view];
        }
    }
    
    _mediaRef = [[jvxIosAudioMedia alloc] init];
    [_mediaRef refreshMediaItems];
    _lstArtists = _mediaRef.allCurrentArtists;
    _lstItemsForArtist = [[NSMutableArray alloc] init];
    _selectedArtist = NULL;
    _selectionInputFile_artist = -1;
    _selectionInputFile_title = -1;
    
    
    _localLstItemsForArtist = [[NSMutableArray alloc] init];
    _selectionInputFile_localArtist = -1;
    _selectionInputFile_localTitle = -1;

    _localLstItemsForArtist_rem = [[NSMutableArray alloc] init];
    _selectionInputFile_localArtist_rem = -1;
    _selectionInputFile_localTitle_rem = -1;
    
    [self update_local_infiles];
    
    _selectionOutputFile = -1;
    _nameOutputFileAdd = @"Unknown";
    _numChannelOutputFile = 1;
    _rateOutputFile = 48000;
    
    /*
    AVAssetWriterInput *assetWriterInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio
                                        outputSettings:outputSettings];
    if ([assetWriter canAddInput:assetWriterInput]) {
        [assetWriter addInput:assetWriterInput];
    } else {
        NSLog (@"can't add asset writer input... die!");
        return;
    }
    assetWriterInput.expectsMediaDataInRealTime = NO;
    */
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

-(BOOL) report_to_viewport:(NSObject*)intId withObject:(NSObject *)info
{
    BOOL retVal = FALSE;
    NSNumber* intIdNum = (NSNumber*)intId;
    NSString* infoNs;
    NSDictionary* theDict = NULL;
    NSNumber* idReasonNum = NULL;
    int i;
    
    jvxIosBwdReportEvent commandId = (jvxIosBwdReportEvent)[intIdNum integerValue];
    
    switch(commandId)
    {
        case JVX_IOS_BWD_COMMAND_BOOTUP_COMPLETE:
            
            [self bootup_complete];
            
            [self updateWindow];
            
            retVal = true;
            break;
            
        case JVX_IOS_BWD_COMMAND_TEXT_MESSAGE:
            
            infoNs = (NSString*)info;
            // Trigger all other viewcontrollers
            for(i = 0; i < lstOfDelegates.count; i++)
            {
                id<JVXViewControllerDelegate> one_delegate = [lstOfDelegates objectAtIndex:i];
                if ([one_delegate respondsToSelector:@selector(textMessageD:)])
                {
                    [one_delegate textMessageD:infoNs];
                }
            }
            retVal = TRUE;
            break;
        case JVX_IOS_BWD_COMMAND_NEW_ROUTE:
            infoNs = @"REPORTED <New route>";
            theDict = (NSDictionary*)info;
            idReasonNum = [theDict objectForKey:@"AVAudioSessionRouteChangeReasonKey"];
            switch(idReasonNum.intValue)
            {
                case AVAudioSessionRouteChangeReasonUnknown:
                    infoNs = [infoNs stringByAppendingString:@"Reason Unknown"];
                    break;
                case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
                    infoNs = [infoNs stringByAppendingString:@"New Device"];
                    break;
                case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
                    infoNs = [infoNs stringByAppendingString:@"Old Device Gone"];
                    break;
                case AVAudioSessionRouteChangeReasonCategoryChange:
                    infoNs = [infoNs stringByAppendingString:@"Change Category"];
                    break;
                case AVAudioSessionRouteChangeReasonOverride:
                    infoNs = [infoNs stringByAppendingString:@"Override"];
                    break;
                case AVAudioSessionRouteChangeReasonWakeFromSleep:
                    infoNs = [infoNs stringByAppendingString:@"Wake from Sleep"];
                    break;
                case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory:
                    infoNs = [infoNs stringByAppendingString:@"No route for category"];
                    break;
                case AVAudioSessionRouteChangeReasonRouteConfigurationChange:
                    infoNs = [infoNs stringByAppendingString:@"Route Config Change"];
                    break;
            }
            NSLog(@"%@", infoNs);
            for(i = 0; i < lstOfDelegates.count; i++)
            {
                id<JVXViewControllerDelegate> one_delegate = [lstOfDelegates objectAtIndex:i];
                if ([one_delegate respondsToSelector:@selector(textMessageD:)])
                {
                    [one_delegate textMessageD:infoNs];
                }
            }
            break;
        case JVX_IOS_BWD_COMMAND_INTERRUPTION:
            NSLog(@"TABBARCONTROLLER: REPORTED <Interruption>");
            break;
        case JVX_IOS_BWD_COMMAND_MEDIACENTER_RESET:
            NSLog(@"TABBARCONTROLLER: REPORTED <Mediacenter reset>");
            break;
        case JVX_IOS_BWD_COMMAND_START:
            NSLog(@"TABBARCONTROLLER: REPORTED <start request>");
            break;
        case JVX_IOS_BWD_COMMAND_STOP:
            NSLog(@"TABBARCONTROLLER: REPORTED <stop request>");
            break;
        case JVX_IOS_BWD_COMMAND_UPDATE_WINDOW:
            [self updateWindow];
            break;
        default:
            break;
    }
    return(retVal);
}

/*
 * -----------------------------------------------
 * Update the window to show all project relevant settings
 * The functions defined here are intended for "import"
 * in the "real" ViewController implementation
 * -----------------------------------------------
 */
- (void) handle_states_components
{
    jvxErrorType res = JVX_NO_ERROR;
    _theStateHost = JVX_STATE_NONE;
    _theStateAudioTech = JVX_STATE_NONE;
    _theStateAudioDev = JVX_STATE_NONE;
    _theStateAudioNode = JVX_STATE_NONE;
    
    
    res = [_hostHandle state_component:&_theStateHost forComponentType:JVX_COMPONENT_HOST];
    JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC1");
    
    if(_theStateHost >= JVX_STATE_ACTIVE)
    {
        _theStateAudioTech = JVX_STATE_NONE;
        res = [_hostHandle state_component:&_theStateAudioTech forComponentType:JVX_COMPONENT_AUDIO_TECHNOLOGY];
        JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC2");
        if(_theStateAudioTech >= JVX_STATE_SELECTED)
        {
            [_hostHandle init_properties:JVX_COMPONENT_AUDIO_TECHNOLOGY];
        }
        else
        {
            [_hostHandle terminate_properties:JVX_COMPONENT_AUDIO_TECHNOLOGY];
        }
        
        _theStateAudioDev = JVX_STATE_NONE;
        res = [_hostHandle state_component:&_theStateAudioDev forComponentType:JVX_COMPONENT_AUDIO_DEVICE];
        JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC3");
        if(_theStateAudioDev >= JVX_STATE_SELECTED)
        {
            [_hostHandle init_properties:JVX_COMPONENT_AUDIO_DEVICE];
        }
        else
        {
            [_hostHandle terminate_properties:JVX_COMPONENT_AUDIO_DEVICE];
        }
        
        _theStateAudioNode = JVX_STATE_NONE;
        res = [_hostHandle state_component:&_theStateAudioNode forComponentType:JVX_COMPONENT_AUDIO_NODE];
        JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC4");
        if(_theStateAudioNode >= JVX_STATE_SELECTED)
        {
            [_hostHandle init_properties:JVX_COMPONENT_AUDIO_NODE];
        }
        else
        {
            [_hostHandle terminate_properties:JVX_COMPONENT_AUDIO_NODE];
        }
    }
    else
    {
        JVX_ASSERT((0), @"TABBARC5");
    }
}

/* Callback on bootup complete */
- (void) bootup_complete
{
    NSLog(@"Bootup complete");
    jvxErrorType res = JVX_NO_ERROR;
    int i;
    
    // What we expect when arriving here
    // 1) Host in state active
    // 2) Technology should be the generic wrapper
    // 3) Technology should be in state selected
    // 4) AudioNode should be the one as specified for this project
    // 5) AudioNode should be at least activated
    [self handle_states_components];
    [self updateWindow];
    
    NSMutableArray* lstLoc = NULL;
    NSString* artist = NULL;
    NSString* title = NULL;
    
    res = [_hostHandle get_entry_config_file:@"ios_host_in_levels" dataListToExport:&lstLoc];
    if(res == JVX_NO_ERROR)
    {
        for(i = 0; i < lstLoc.count; i++)
        {
            if(i < _lstInputGains.count)
            {
                [_lstInputGains setObject:[lstLoc objectAtIndex:i] atIndexedSubscript:i];
            }
        }
    }
    
    res = [_hostHandle get_entry_config_file:@"ios_host_out_levels" dataListToExport:&lstLoc];
    if(res == JVX_NO_ERROR)
    {
        for(i = 0; i < lstLoc.count; i++)
        {
            if(i < _lstOutputGains.count)
            {
                [_lstOutputGains setObject:[lstLoc objectAtIndex:i] atIndexedSubscript:i];
            }
        }
    }
    [self updateWindow];

    res = [_hostHandle get_entry_config_file:@"ios_host_in_file_artist" stringToExport:&artist];
    if(res == JVX_NO_ERROR)
    {
        for(i = 0; i < _localLstArtists.count; i++)
        {
            if([artist isEqualToString:[_localLstArtists objectAtIndex:i]])
            {
                _selectionInputFile_localArtist = i;
                _selectionInputFile_localTitle = -1;
                [self refreshLocalTitles:false];
                break;
            }
        }
    }
    
    res = [_hostHandle get_entry_config_file:@"ios_host_in_file_title" stringToExport:&title];
    if(res == JVX_NO_ERROR)
    {
        for(i = 0; i < _localLstItemsForArtist.count; i++)
        {
            jvxIosLocalMediaItem* it = [_localLstItemsForArtist objectAtIndex:i];
            if([title isEqualToString:it.nmTitle])
            {
                _selectionInputFile_localTitle = i;
                break;
            }
        }
    }

    res = [_hostHandle get_entry_config_file:@"ios_host_out_file_name" stringToExport:&title];
    if(res == JVX_NO_ERROR)
    {
        for(i = 0; i < _lstOutputFileNames.count; i++)
        {
            if([title isEqualToString:[_lstOutputFileNames objectAtIndex:i]])
            {
                _selectionOutputFile = i;
            }
        }
    }

    [self updateWindow];

}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Update the view controller window here */
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

- (void) updateWindow
{
    jvxErrorType res = JVX_NO_ERROR;
    NSString* prop_name = NULL;
    NSLog(@"Update Window");
    NSMutableArray* lstLoc = NULL;
    jvxBitField selection = 0;
    jvxInt32 valI32;
    jvxInt16 valI16;
    float val;
    int i;
    
    res = [_hostHandle state_run:&_statusIsRunning];
    JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC6");
    
    // First, obtain states
    if(_theStateAudioTech == JVX_STATE_SELECTED)
    {
        prop_name = @"JVX_GENW_TECHNOLOGIES";
        
        res = [_hostHandle get_property_selection_list:&lstLoc
                                    returnedSelection:&selection
                                     withNameProperty: prop_name
                                         forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY
                                          contentOnly:false];
        JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC7");
        
        _lstTechs = lstLoc;
        lstLoc = NULL;

    }
    else
    {
        if(_theStateAudioTech == JVX_STATE_ACTIVE)
        {
            // Empty list
            [_lstTechs removeAllObjects];
            
            // Obtain names of all exposed devices
            res = [_hostHandle list_component:&lstLoc forComponentType:JVX_COMPONENT_AUDIO_DEVICE withIdSelected:&valI16];
            JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC8");
            _lstDevs = lstLoc;
            _idDeviceSelected = valI16;
            lstLoc = NULL;


            if(_theStateAudioDev >= JVX_STATE_ACTIVE)
            {
                // Obtain list of input file names (property of technology)
                prop_name = @"JVX_GENW_INPUT_FILES";
                
                res = [_hostHandle get_property_selection_list:&lstLoc
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY
                                                   contentOnly:false];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC9_0");
                _lstInputFileNames = lstLoc;
                
                // Obtain list of input file names (property of technology)
                prop_name = @"JVX_GENW_OUTPUT_FILES_F";
                
                res = [_hostHandle get_property_string_list:&lstLoc
                                              withNameProperty: prop_name
                                               forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC9_0");
                _lstOutputFileNames = lstLoc;
                if(_selectionOutputFile > _lstOutputFileNames.count)
                {
                    _selectionOutputFile = _lstOutputFileNames.count - 1;
                }
                
                if(_selectionOutputFile >= 0)
                {
                    NSString* theTagsExpr =[_lstOutputFileNames objectAtIndex:_selectionOutputFile];
                    _nameOutputFileAdd = [_hostHandle helper_tagExprToTitle:theTagsExpr];
                    prop_name = @"JVX_GENW_OUTFILE_CHANS";
                    res = [_hostHandle get_property_numeric:&valI16
                           forDataFormat:JVX_DATAFORMAT_16BIT_LE
                                            withOffsetArray:_selectionOutputFile
                                           withNameProperty:prop_name
                                                      forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
                    JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC9_1");
                    _numChannelOutputFile = valI16;
                    
                    prop_name = @"JVX_GENW_OUTFILE_RATE";
                    res = [_hostHandle get_property_numeric:&valI32
                                              forDataFormat:JVX_DATAFORMAT_32BIT_LE
                                            withOffsetArray:_selectionOutputFile
                                           withNameProperty:prop_name
                                               forComponent:JVX_COMPONENT_AUDIO_TECHNOLOGY];
                    JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC9_1");
                    _rateOutputFile = valI32;

                }
                
                

                // - - - - - - - - - - - - - - - - - - - - - - - - - -
                // Properties for first tab
                // - - - - - - - - - - - - - - - - - - - - - - - - - -

                // Obtain list of input channels
                prop_name = @"JVX_AUDEV_INPUT_CHANNELS";
                
                res = [_hostHandle get_property_selection_list:&lstLoc
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                   contentOnly:false];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC9");
                _lstInputChannels = lstLoc;
                _selInputChannels = selection;
                lstLoc = NULL;

                prop_name = @"JVX_GENW_DEV_IGAIN";
                lstLoc = [[NSMutableArray alloc] init];
                valI32 = 0;
                for(i = 0; i < _lstInputChannels.count; i++)
                {
                    val = 1.0;
                    
                    if(_statusIsRunning)
                    {
                        if(_selInputChannels & (jvxBitField) 1 << i)
                        {
                            [_hostHandle get_property_numeric:&val
                                                forDataFormat:JVX_DATAFORMAT_DATA
                                              withOffsetArray:valI32
                                             withNameProperty:prop_name
                                                 forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                            valI32++;
                        }
                    }
                    else
                    {
                        if(i < _lstInputGains.count)
                        {
                            val = [(NSNumber*)[_lstInputGains objectAtIndex:i] floatValue];
                        }
                    }
                    [lstLoc addObject:[NSNumber numberWithFloat:val]];
                }
                
                _lstInputGains = lstLoc;
                lstLoc = NULL;
                
                
                // Obtain list of input channels
                prop_name = @"JVX_AUDEV_OUTPUT_CHANNELS";
                
                res = [_hostHandle get_property_selection_list:&lstLoc
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                   contentOnly:false];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC10");
                _lstOutputChannels = lstLoc;
                _selOutputChannels = selection;
                lstLoc = NULL;

                prop_name = @"JVX_GENW_DEV_OGAIN";
                lstLoc = [[NSMutableArray alloc] init];
                valI32 = 0;
                for(i = 0; i < _lstOutputChannels.count; i++)
                {
                    val = 1.0;
                    
                    if(_statusIsRunning)
                    {
                        if(_selOutputChannels & (jvxBitField) 1 << i)
                        {
                            [_hostHandle get_property_numeric:&val
                                            forDataFormat:JVX_DATAFORMAT_DATA
                                          withOffsetArray:valI32
                                         withNameProperty:prop_name
                                             forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                            valI32++;
                        }
                    }
                    else
                    {
                        if(i < _lstOutputGains.count)
                        {
                            val = [(NSNumber*)[_lstOutputGains objectAtIndex:i] floatValue];
                        }
                    }
                    [lstLoc addObject:[NSNumber numberWithFloat:val]];
                }
                
                _lstOutputGains = lstLoc;
                lstLoc = NULL;
                
                // ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++
                // ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++ ++
                
                // Obtain current value of samplerate
                prop_name = @"JVX_AUDEV_SRATE";
                
                valI32 = 0;
                res = [_hostHandle get_property_numeric:(jvxHandle*)(&valI32)
                                          forDataFormat:JVX_DATAFORMAT_32BIT_LE
                                       withNameProperty: prop_name
                                           forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC11");
                _currentSamplerate = valI32;
                
                
                
                // Obtain list of samplerates from wrapped module
                prop_name = @"JVX_GENW/JVX_AUDEV_RATES";
                
                res = [_hostHandle get_property_selection_list:&lstLoc
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                   contentOnly:false];
                if(res == JVX_NO_ERROR)
                {
                    _lstSamplerates = lstLoc;
                    _selSamplerates = selection;
                    lstLoc = NULL;

                }
                else
                {
                    [_lstSamplerates removeAllObjects];
                     _selSamplerates = 0;
                }
                
                
                // Obtain current value of samplerate
                prop_name = @"JVX_AUDEV_BSIZE";
                
                valI32 = 0;
                res = [_hostHandle get_property_numeric:(jvxHandle*)(&valI32)
                                          forDataFormat:JVX_DATAFORMAT_32BIT_LE
                                       withNameProperty: prop_name
                                           forComponent:JVX_COMPONENT_AUDIO_DEVICE];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC12");
                _currentBuffersize = valI32;
                
                
                // Obtain list of buffersizes from wrapped module
                prop_name = @"JVX_GENW/JVX_AUDEV_BSIZES";
                
                res = [_hostHandle get_property_selection_list:&lstLoc
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                   contentOnly:false];
                if(res == JVX_NO_ERROR)
                {
                    _lstBuffersizes = lstLoc;
                    _selBuffersizes = selection;
                    lstLoc = NULL;

                }
                else{
                    [_lstBuffersizes removeAllObjects];
                    _selBuffersizes = 0;
                }
                
                // Obtain list of buffersizes from wrapped module
                prop_name = @"JVX_GENW/JVX_AUDEV_FORMATS";
                
                res = [_hostHandle get_property_selection_list:&lstLoc
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                   contentOnly:false];
                if(res == JVX_NO_ERROR)
                {
                    _showWrappedFormats = true;
                    _lstDataformatsWD = lstLoc;
                    _selDataFormatsWD = selection;
                    lstLoc = NULL;

                }
                else
                {
                    _showWrappedFormats = false;
                    [_lstDataformatsWD removeAllObjects];
                    _selDataFormatsWD = 0;
                }
                
                prop_name = @"JVX_AUDEV_FORMATS";
                res = [_hostHandle get_property_selection_list:&lstLoc
                                                 returnedSelection:&selection
                                                  withNameProperty: prop_name
                                                      forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                       contentOnly:false];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC13");
                _lstDataformatsGW = lstLoc;
                _selDataFormatsGW = selection;
                lstLoc = NULL;
                
                
                prop_name = @"JVX_GENW_DEV_REBUFFER";
                res = [_hostHandle get_property_selection_list:NULL
                                            returnedSelection:&selection
                                             withNameProperty: prop_name
                                                 forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                  contentOnly:true];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC14");
                _doRebuffer = (selection & 0x1);
                
                prop_name = @"JVX_GENW_DEV_RESAMPLER";
                res = [_hostHandle get_property_selection_list:NULL
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                   contentOnly:true];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC15");
                _doResample = (selection & 0x1);
                
                // - - - - - - - - - - - - - - - - - - - - - - - - - -
                // Properties for second tab
                // - - - - - - - - - - - - - - - - - - - - - - - - - -
                prop_name = @"JVX_GENW/JVX_IOS_AUDEV_INPUT_SOURCE";
                res = [_hostHandle get_property_selection_list:&lstLoc
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                   contentOnly:false];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC16");
                _lstInputSources = lstLoc;
                _selInputSource = selection;
                lstLoc = NULL;

                prop_name = @"JVX_GENW/JVX_IOS_AUDEV_OUTPUT_SOURCE";
                res = [_hostHandle get_property_selection_list:&lstLoc
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                   contentOnly:false];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC17");
                _lstOutputSources = lstLoc;
                _selOutputSource = selection;
                lstLoc = NULL;


                prop_name = @"JVX_GENW/JVX_IOS_AUDEV_OUTPUT_SPEAKER";
                res = [_hostHandle get_property_selection_list:&lstLoc
                                             returnedSelection:&selection
                                              withNameProperty: prop_name
                                                  forComponent:JVX_COMPONENT_AUDIO_DEVICE
                                                   contentOnly:false];
                JVX_ASSERT((res == JVX_NO_ERROR), @"TABBARC18");
                _lstOutputSpeaker = lstLoc;
                _selOutputSpeaker = selection;
                lstLoc = NULL;


            }
        }
        else
        {
            JVX_ASSERT((0), @"TABBARC19");
        }
    }

    // Trigger all other viewcontrollers
    for(i = 0; i < lstOfDelegates.count; i++)
    {
        id<JVXViewControllerDelegate> one_delegate = [lstOfDelegates objectAtIndex:i];
        if ([one_delegate respondsToSelector:@selector(updateWindowD:)])
        {
            [one_delegate updateWindowD:self];
        }
    }   
}

- (BOOL) register_delegate:(id<JVXViewControllerDelegate>) theNewDelegate
{
    int i;
    BOOL foundDelegateInList = false;
    for(i = 0; i< lstOfDelegates.count; i++)
    {
        if(theNewDelegate == [lstOfDelegates objectAtIndex:i])
        {
            foundDelegateInList = true;
            break;
        }
    }
    
    if(!foundDelegateInList)
    {
        [lstOfDelegates addObject:theNewDelegate];
    }
    return(!foundDelegateInList);
}

- (BOOL) unregister_delegate:(id<JVXViewControllerDelegate>) theDelegateToBeRemoved
{
    int i;
    BOOL foundDelegateInList = false;
    for(i = 0; i< lstOfDelegates.count; i++)
    {
        if(theDelegateToBeRemoved == [lstOfDelegates objectAtIndex:i])
        {
            foundDelegateInList = true;
            break;
        }
    }
    
    if(foundDelegateInList)
    {
        [lstOfDelegates removeObjectAtIndex:i];
    }
    return(foundDelegateInList);
}

- (void) start_timer:(float)int_secs
{
    int i;
    
    // Trigger all other viewcontrollers
    for(i = 0; i < lstOfDelegates.count; i++)
    {
        id<JVXViewControllerDelegate> one_delegate = [lstOfDelegates objectAtIndex:i];
        if ([one_delegate respondsToSelector:@selector(startRun:)])
        {
            [one_delegate startRun:self];
        }
    }
    
    _myTimer = [NSTimer scheduledTimerWithTimeInterval:int_secs
                                     target:self
                                   selector:@selector(updateWindowP:)
                                   userInfo:nil
                                    repeats:YES];
}

- (void) stop_timer
{
    [_myTimer invalidate];
    
    int i;
    
    // Trigger all other viewcontrollers
    for(i = 0; i < lstOfDelegates.count; i++)
    {
        id<JVXViewControllerDelegate> one_delegate = [lstOfDelegates objectAtIndex:i];
        if ([one_delegate respondsToSelector:@selector(stopRun:)])
        {
            [one_delegate stopRun:self];
        }
    }

}

- (void) updateWindowP:(NSTimer*)timer
{
    int i;
    
    // Trigger all other viewcontrollers
    for(i = 0; i < lstOfDelegates.count; i++)
    {
        id<JVXViewControllerDelegate> one_delegate = [lstOfDelegates objectAtIndex:i];
        if ([one_delegate respondsToSelector:@selector(updateWindowP:)])
        {
            [one_delegate updateWindowP:self];
        }
    }
}

- (UILabel*) createLabelEntryPickerViewer:(NSString*) rowItem
                   withWidth:(float)wd
                  withHeight:(float)ht
                  doShowBold:(BOOL) showBold
                doShowItalic:(BOOL) showIt
                   withColor:(UIColor*)theColor
{
    UILabel *lblRow = [[UILabel alloc] initWithFrame:CGRectMake(0.0f, 0.0f, wd, ht)];
    
    // Center the text.
    [lblRow setTextAlignment:NSTextAlignmentLeft];
    
    // Make the text color red.
    [lblRow setTextColor: theColor];
    
    UIFontDescriptorSymbolicTraits tt = 0;
    if(showBold)
    {
        tt |= UIFontDescriptorTraitBold;
    }
    if(showIt)
    {
        tt |= UIFontDescriptorTraitItalic;
    }
    
    UIFontDescriptor * fontD = [lblRow.font.fontDescriptor
                                fontDescriptorWithSymbolicTraits:tt];
    
    lblRow.font = [UIFont fontWithDescriptor:fontD size:_fontSizeMedium];
    
    
    // Add the text.
    [lblRow setText:rowItem];
    
    // Clear the background color to avoid problems with the display.
    [lblRow setBackgroundColor:[UIColor clearColor]];
    
    // Return the label.
    return lblRow;

}

- (void) refreshTitles
{
    NSMutableArray* lstloc;
    [_lstItemsForArtist removeAllObjects];
    [_mediaRef getItemsForArtist:_selectedArtist returnList:&lstloc];
    _lstItemsForArtist = lstloc;
}


- (void) refreshLocalTitles:(jvxCBool)removeList
{
    NSMutableArray* lstloc;
    
    if(removeList)
    {
        [_localLstItemsForArtist_rem removeAllObjects];
        if(_selectionInputFile_localArtist_rem >= 0)
        {
            [_localMediaRef getItemsForArtist:[_localLstArtists objectAtIndex:_selectionInputFile_localArtist_rem] returnList:&lstloc];
            _localLstItemsForArtist_rem = lstloc;
        }
    }
    else
    {
        [_localLstItemsForArtist removeAllObjects];
        if(_selectionInputFile_localArtist >= 0)
        {
            [_localMediaRef getItemsForArtist:[_localLstArtists objectAtIndex:_selectionInputFile_localArtist] returnList:&lstloc];
            _localLstItemsForArtist = lstloc;
        }
    }
}

- (void) update_local_infiles
{
    jvxIosLocalMedia* lstLocalMediaItems = NULL;
    [_hostHandle produce_list_filetypes:_documentsDirectory withFileExtension:@".wav" returnedList:&lstLocalMediaItems];
    _localMediaRef = lstLocalMediaItems;
    _localLstArtists = _localMediaRef.allCurrentArtists;
}

- (NSString*) idInInputFileListTech: (jvxCBool) removeList
                     withIdOnReturn:(jvxInt32*) idOnReturn;
{
    jvxInt32 retVal = -1;
    NSString* date = NULL;
    jvxIosLocalMediaItem* it = NULL;

    if(removeList)
    {
        if(_selectionInputFile_localTitle_rem >= 0)
        {
            jvxCBool foundit = c_false;
            int i;
            
            it = [_localLstItemsForArtist_rem objectAtIndex:_selectionInputFile_localTitle_rem ];
            date = it.nmDate;
            
            for(i = 0; i < _lstInputFileNames.count; i++)
            {
                if([it.nmPath isEqualToString:[_lstInputFileNames objectAtIndex:i]])
                {
                    foundit = true;
                    break;
                }
            }
            if(foundit)
            {
                retVal = i;
            }
        }
    }
    else
    {
        if(_selectionInputFile_localTitle >= 0)
        {
            jvxCBool foundit = c_false;
            int i;
            
            it = [_localLstItemsForArtist objectAtIndex:_selectionInputFile_localTitle ];
            date = it.nmDate;
            
            for(i = 0; i < _lstInputFileNames.count; i++)
            {
                if([it.nmPath isEqualToString:[_lstInputFileNames objectAtIndex:i]])
                {
                    foundit = true;
                    break;
                }
            }
            if(foundit)
            {
                retVal = i;
            }
        }
    }
    if(idOnReturn)
    {
        *idOnReturn = retVal;
    }
    return(date);
}
@end
