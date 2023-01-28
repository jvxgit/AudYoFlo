//
//  ViewController_messages.m
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 08.01.16.
//  Copyright © 2016 Javox Solutions. All rights reserved.
//

#import "ViewController_messages.h"
#import "tabBarController_rtproc.h"
#import "jvxIosAudioMediaItem.h"
#import <AVFoundation/AVAssetReader.h>
#import <AVFoundation/AVAssetReaderOutput.h>
#import <AVFoundation/AVAssetTrack.h>
#import <AVFoundation/AVAudioSettings.h>
#import "jvxIosLocalMediaItem.h"

@interface ViewController_messages ()

@end

@implementation ViewController_messages

- (void)viewDidLoad
{
    UIFontDescriptor * fontD = NULL;
    
//    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleNotification:) name:@"textMessage" object:nil];
    
    [super viewDidLoad];
    

    // Do any additional setup after loading the view.
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    SET_FONT_SIZE_ONE_ELEMENT(_buttonClear.titleLabel, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_theTextField, hRef.fontSizeTiny);
    
    SET_FONT_SIZE_ONE_ELEMENT(_label_1, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_2, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_3, hRef.fontSizeLarge);
    SET_FONT_SIZE_ONE_ELEMENT(_label_3, hRef.fontSizeLarge);
    
    SET_FONT_SIZE_ONE_ELEMENT(_buttonAddFile.titleLabel, hRef.fontSizeMedium);
    SET_FONT_SIZE_ONE_ELEMENT(_buttonRemoveFile.titleLabel, hRef.fontSizeMedium);

    SET_FONT_SIZE_ONE_ELEMENT(_labelRemote, hRef.fontSizeMedium);
    
    self.selectionFileLib.dataSource = self;
    self.selectionFileLib.delegate = self;
    
    self.selectionFileLocal.dataSource = self;
    self.selectionFileLocal.delegate = self;

    self.buttonRemoveFile.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
    
    [hRef register_delegate:self];

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (int)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    int retVal = 0;

    if(pickerView == _selectionFileLib)
    {
        retVal = 2;
    }
    if(pickerView == _selectionFileLocal)
    {
        retVal = 2;
    }

    return(retVal);
}

- (int)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    int retVal = 1;
    
    if(pickerView == _selectionFileLib)
    {
        if(component == 0)
        {
            retVal += hRef.lstArtists.count;
        }
        if(component == 1)
        {
            retVal += hRef.lstItemsForArtist.count;
        }
    }
    
    if(pickerView == _selectionFileLocal)
    {
        if(component == 0)
        {
            retVal += hRef.localLstArtists.count;
        }
        if(component == 1)
        {
            retVal += hRef.localLstItemsForArtist_rem.count;
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
    
    if(pickerView == _selectionFileLib)
    {
        retVal = @"None";
        row -= 1;
        if(component == 0)
        {
            if(row < hRef.lstArtists.count)
            {
                retVal = [hRef.lstArtists objectAtIndex:row];
            }
        }
        if(component == 1)
        {
            if(row < hRef.lstItemsForArtist.count)
            {
                jvxIosAudioMediaItem* oneItem = [hRef.lstItemsForArtist objectAtIndex:row];
                retVal = oneItem.nmTitle;
            }
        }
    }
    
    if(pickerView == _selectionFileLocal)
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
            if(row < hRef.localLstItemsForArtist_rem.count)
            {
                jvxIosAudioMediaItem* oneItem = [hRef.localLstItemsForArtist_rem objectAtIndex:row];
                retVal = oneItem.nmTitle;
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
    
    if(pickerView == _selectionFileLib)
    {
        row -= 1;
        if(component == 0)
        {
            if(row< 0)
            {
                hRef.selectedArtist = NULL;
            }
            else
            {
                if(row < hRef.lstArtists.count)
                {
                    hRef.selectedArtist = [hRef.lstArtists objectAtIndex:row];
                }
                else
                {
                    assert(0);
                }
            }
            hRef.selectionInputFile_artist = row;
            hRef.selectionInputFile_title = -1;
            [hRef refreshTitles];
        }
        if(component == 1)
        {
            hRef.selectionInputFile_title = row;
        }
        [hRef updateWindow];
    }
    
    if(pickerView == _selectionFileLocal)
    {
        row -= 1;
        if(component == 0)
        {
            hRef.selectionInputFile_localArtist_rem = row;
            hRef.selectionInputFile_localTitle_rem = -1;
            [hRef refreshLocalTitles: true];
        }
        if(component == 1)
        {
            hRef.selectionInputFile_localTitle_rem = row;
        }
        [hRef updateWindow];
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

/*
- (void) updateListMessages
{
    int i;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    for(i = 0; i < hRef.storedMessages.count; i++)
    {
        NSString* theText = [hRef.storedMessages objectAtIndex:i];
        // Add entry to textview
    }
}*/

- (void) textMessageD:(NSString*) txt;
{
    _theTextField.text = [_theTextField.text stringByAppendingString:txt];
    _theTextField.text = [_theTextField.text stringByAppendingString:@"\n"];
    
    // This could help to scroll to the end of the text field - but this does not work on iPad
    //NSRange range = NSMakeRange(_theTextField.text.length - 1, 1);
    //[_theTextField scrollRangeToVisible:range];
}

- (IBAction)buttonAddContent:(id)sender
{
    int i;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];
    
    NSString* theArtist = NULL;
    NSMutableArray* theListsOfItems = NULL;
    jvxIosAudioMediaItem* it = NULL;
    NSError *assetError = nil;
    
    double rate = -1;
    int numChannels = -1;
    jvxInt64 lengthSamples = -1;
    jvxInt32 cntSamples = 0;
    jvxSize bytesSample = 2;
    
    if((hRef.selectionInputFile_artist >= 0) && (hRef.selectionInputFile_title >= 0))
    {
        if(hRef.selectionInputFile_artist < hRef.mediaRef.allCurrentArtists.count)
        {
            theArtist = [hRef.mediaRef.allCurrentArtists objectAtIndex:(hRef.selectionInputFile_artist)];
    
            // Return list items for current artist
            [hRef.mediaRef getItemsForArtist:theArtist returnList:&theListsOfItems];
        
            if(hRef.selectionInputFile_title < theListsOfItems.count)
            {
                it = [theListsOfItems objectAtIndex:hRef.selectionInputFile_title];
                if(it)
                {
                    NSURL *assetURL = it.fHandle;
                    /*
                    NSString* str = [assetURL absoluteString];
                    NSURL* copyUrl = [NSURL URLWithString:str];
                    */
                    
                    // Get song assets
                    AVURLAsset *songAsset = [AVURLAsset URLAssetWithURL:assetURL options:nil];
                    
                    // Define an asset reader
                    AVAssetReader *assetReader = [AVAssetReader assetReaderWithAsset:songAsset error:&assetError];
                    if (assetError)
                    {
                        NSLog (@"error: %@", assetError);
                        return;
                    }
                    
                    
                    NSMutableDictionary* audioReadSettings = [[NSMutableDictionary alloc] init];
                    [audioReadSettings setValue:[NSNumber numberWithInt:kAudioFormatLinearPCM]
                                         forKey:AVFormatIDKey];
                    [audioReadSettings setValue:[NSNumber numberWithInt:bytesSample * 8] forKey:AVLinearPCMBitDepthKey];
                    [audioReadSettings setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsBigEndianKey];
                    [audioReadSettings setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsFloatKey];
                    [audioReadSettings setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsNonInterleaved];
                    
                    // Define an output reader asset instance
                    AVAssetReaderOutput *assetReaderOutput = [AVAssetReaderAudioMixOutput
                                                              assetReaderAudioMixOutputWithAudioTracks:songAsset.tracks
                                                              audioSettings: audioReadSettings
                                                              ];
                    if (! [assetReader canAddOutput: assetReaderOutput])
                    {
                        NSLog (@"can't add reader output... die!");
                        return;
                    }
                    
                    // Link output reader and reader asset
                    [assetReader addOutput:assetReaderOutput];
                    
                    // Get properties of the song
                    // It seems that output is always available only as given by the track description
                    NSArray<AVAssetTrack*>* tr = songAsset.tracks;
                    AVAssetTrack* trr = [tr objectAtIndex:0];
                    NSArray* arr2 = trr.formatDescriptions;
                    for(i = 0; i < arr2.count; i++)
                    {
                        CMAudioFormatDescriptionRef item = (__bridge CMAudioFormatDescriptionRef)[arr2 objectAtIndex:i];
                        const AudioStreamBasicDescription *asDesc = (AudioStreamBasicDescription*)CMAudioFormatDescriptionGetStreamBasicDescription(item);
                        rate = asDesc->mSampleRate;
                        numChannels = asDesc->mChannelsPerFrame;
                        
                    }
                    CMTimeRange rr = trr.timeRange;
                    lengthSamples = rr.duration.value;
                    
                    if((rate >= 0) && (numChannels >= 0) && (lengthSamples >0))
                    {
                        jvxHandle* theHdl = NULL;
                        
                        jvxErrorType res = [hRef.hostHandle open_wav_writer_from_audiolib:numChannels
                                                                                  withRate:rate
                                                                                withFormat:JVX_DATAFORMAT_16BIT_LE
                                                                            withArtistTag:it.nmArtist withTitleTag:it.nmTitle
                                                                             returnHandle:&theHdl];
                        if(res == JVX_NO_ERROR)
                        {
                            
                            [assetReader startReading];
                        
                            CMSampleBufferRef nextBuffer = NULL;
                        
                            AVAssetReaderStatus stat = [assetReader status];
                            switch(stat)
                            {
                                case AVAssetReaderStatusUnknown:
                                    break;
                                case AVAssetReaderStatusReading:
                                    break;
                                case AVAssetReaderStatusCompleted:
                                    break;
                                case AVAssetReaderStatusFailed:
                                    break;
                                case AVAssetReaderStatusCancelled:
                                    break;
                                default:
                                    break;
                            }
                            while(1)
                            {
                                nextBuffer = [assetReaderOutput copyNextSampleBuffer];
                                if (nextBuffer)
                                {
                                    CMItemCount cnt = CMSampleBufferGetNumSamples ( nextBuffer );
                                    
                                    // Get block buffer representation
                                    CMBlockBufferRef buffer = CMSampleBufferGetDataBuffer( nextBuffer );
                                    
                                    // Some variables
                                    size_t lengthAtOffset;
                                    size_t totalLength;
                                    size_t layoutSize = 0;
                                    char* data; // <- actual data pointer
                                    
                                    if( CMBlockBufferGetDataPointer( buffer, 0, &lengthAtOffset, &totalLength, &data ) != noErr )
                                    {
                                        NSLog( @"error!" );
                                    }
                                    
                                    // Write interleaved buffer
                                    assert(lengthAtOffset == (numChannels * cnt * bytesSample));
                                    cntSamples += cnt;
                                    
                                    CMFormatDescriptionRef ref = CMSampleBufferGetFormatDescription ( nextBuffer)	;
                                    const AudioChannelLayout * l = CMAudioFormatDescriptionGetChannelLayout ( ref, &layoutSize );
                                    //const AudioFormatListItem * f = CMAudioFormatDescriptionGetFormatList ( ref, &layoutSize );
                                    switch(l->mChannelLayoutTag)
                                    {
                                        case kAudioChannelLayoutTag_Mono:
                                            assert(numChannels == 1);
                                            break;
                                        case kAudioChannelLayoutTag_Stereo:
                                            assert(numChannels == 2);
                                            break;
                                    }
                                    
                                    [hRef.hostHandle add_buffer_wav_writer:theHdl
                                                                 withInput:data
                                                         withNumberSamples:cnt];

                                    /*
                                     
                                     NSLog(@"%@", [NSString stringWithFormat:@"%li", cnt]);
                                     */
                                    
                                    CFRelease(nextBuffer);
                                }
                                else
                                {
                                    break;
                                }
                            }
                            res = [hRef.hostHandle close_wav_writer_from_audiolib:theHdl];
                        }
                    }
                }
            }
        }
    }
    [hRef update_local_infiles];
    hRef.selectionInputFile_localArtist = -1;
    hRef.selectionInputFile_localTitle = -1;
    hRef.selectionInputFile_localArtist_rem = -1;
    hRef.selectionInputFile_localTitle_rem = -1;
    
    
    [hRef refreshLocalTitles:false];
    [hRef updateWindow];
}

- (IBAction)buttonRemoveContent:(id)sender
{
    jvxErrorType res = JVX_NO_ERROR;
    tabBarController_rtproc* hRef = (tabBarController_rtproc*)[self parentViewController];

    if( (hRef.selectionInputFile_localArtist_rem >= 0) &&
        (hRef.selectionInputFile_localTitle_rem >= 0))
    {
        jvxIosLocalMediaItem* it = [hRef.localLstItemsForArtist_rem objectAtIndex:hRef.selectionInputFile_localTitle_rem];
        res = [hRef.hostHandle removeFile:it.nmPath];
        if(res == JVX_NO_ERROR)
        {
            [hRef update_local_infiles];
            hRef.selectionInputFile_localArtist = -1;
            hRef.selectionInputFile_localTitle = -1;
            hRef.selectionInputFile_localArtist_rem = -1;
            hRef.selectionInputFile_localTitle_rem = -1;
            [hRef updateWindow];
        }
    }
}

- (IBAction)clearButtonPushed:(id)sender
{
    _theTextField.text = @"";
}

- (void) updateWindowD:(tabBarController_rtproc*) hRef;
{
   
    [_selectionFileLib reloadAllComponents];
    [_selectionFileLib selectRow:hRef.selectionInputFile_artist + 1 inComponent:0 animated:NO];
    [_selectionFileLib selectRow:hRef.selectionInputFile_title + 1 inComponent:1 animated:NO];

    [_selectionFileLocal reloadAllComponents];
    [_selectionFileLocal selectRow:hRef.selectionInputFile_localArtist_rem + 1 inComponent:0 animated:NO];
    [_selectionFileLocal selectRow:hRef.selectionInputFile_localTitle_rem + 1 inComponent:1 animated:NO];
    
    jvxInt32 idInputFileList = -1;
    [hRef idInInputFileListTech:true withIdOnReturn:&idInputFileList];
    if(idInputFileList < 0)
    {
        _buttonRemoveFile.titleLabel.text = @"Remove";
        [_buttonRemoveFile setUserInteractionEnabled:true];
    }
    else
    {
        _buttonRemoveFile.titleLabel.text = @"--";
        [_buttonRemoveFile setUserInteractionEnabled:false];
    }
}

#if 0
//NSString* mt = assetReaderOutput.mediaType;

AudioChannelLayout channelLayout;
memset(&channelLayout, 0, sizeof(AudioChannelLayout));
channelLayout.mChannelLayoutTag = kAudioChannelLayoutTag_Stereo;
NSDictionary *outputSettings =
[NSDictionary dictionaryWithObjectsAndKeys:
 [NSNumber numberWithInt:kAudioFormatLinearPCM], AVFormatIDKey,
 [NSNumber numberWithFloat:44100.0], AVSampleRateKey,
 [NSNumber numberWithInt:2], AVNumberOfChannelsKey,
 [NSData dataWithBytes:&channelLayout length:sizeof(AudioChannelLayout)],
 AVChannelLayoutKey,
 [NSNumber numberWithInt:16], AVLinearPCMBitDepthKey,
 [NSNumber numberWithBool:NO], AVLinearPCMIsNonInterleaved,
 [NSNumber numberWithBool:NO],AVLinearPCMIsFloatKey,
 [NSNumber numberWithBool:NO], AVLinearPCMIsBigEndianKey,
 nil];

#endif

@end
