//
//  tabBarControllerViewController.h
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 06.01.16.
//  Copyright © 2016 Javox Solutions. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "jvxIosAudioMedia.h"
#import "jvxIosLocalMedia.h"
#import "jvxIosHost.h"
#import "jvxIosHost_fineTuning.h"

#define SET_FONT_SIZE_ONE_ELEMENT(elm,sz) \
    fontD = elm.font.fontDescriptor; \
    elm.font = [UIFont fontWithDescriptor:fontD size:sz];

// Forward devlaration
@class tabBarController_rtproc;

// Define the delegate protocol
@protocol JVXViewControllerDelegate <NSObject>

@optional

- (void) startRun:(tabBarController_rtproc*) hRef;
- (void) stopRun:(tabBarController_rtproc*) hRef;
- (void) updateWindowP:(tabBarController_rtproc*) hRef;
- (void) updateWindowD:(tabBarController_rtproc*) hRef;
- (void) textMessageD:(NSString*) txt;


@end

@interface tabBarController_rtproc: UITabBarController
{
    jvxDataFormat localDataFormat;
    NSMutableArray<id<JVXViewControllerDelegate> > *lstOfDelegates;
}

// Reference to the main host
@property (strong, nonatomic) jvxIosHost* hostHandle;

// =========================================================
// Properties as used by the first control sub view
// =========================================================
@property (strong, nonatomic) NSMutableArray* lstTechs;
@property (strong, nonatomic) NSMutableArray* lstDevs;
@property (strong, nonatomic) NSMutableArray* lstInputFileNames;
@property (strong, nonatomic) NSMutableArray* lstOutputFileNames;

@property (assign) jvxInt16 idDeviceSelected;
@property (strong, nonatomic) NSMutableArray* lstInputChannels;
@property (assign) jvxBitField selInputChannels;
@property (strong, nonatomic) NSMutableArray* lstOutputChannels;
@property (assign) jvxBitField selOutputChannels;
@property (strong, nonatomic) NSMutableArray* lstSamplerates;
@property (assign) jvxBitField selSamplerates;
@property (strong, nonatomic) NSMutableArray* lstBuffersizes;
@property (assign) jvxBitField selBuffersizes;
@property (strong, nonatomic) NSMutableArray* lstDataformatsGW;
@property (strong, nonatomic) NSMutableArray* lstDataformatsWD;
@property (assign) jvxBitField selDataFormatsGW;
@property (assign) jvxBitField selDataFormatsWD;
@property (assign) jvxCBool showWrappedFormats;
@property (assign) jvxInt32 currentBuffersize;
@property (assign) jvxInt32 currentSamplerate;
@property (assign) jvxDataFormat currentDataformat;
@property (assign) jvxInt32 curSelectionInputChannels;
@property (assign) jvxInt32 curSelectionOutputChannels;
@property (assign) jvxCBool doResample;
@property (assign) jvxCBool doRebuffer;

@property (assign) jvxInt32 fontSizeLarge;
@property (assign) jvxInt32 fontSizeMedium;
@property (assign) jvxInt32 fontSizeSmall;
@property (assign) jvxInt32 fontSizeTiny;
@property (assign) jvxInt32 fieldWidthChannels;

// ALL MEDIA
@property (strong, nonatomic) jvxIosAudioMedia* mediaRef;
@property (strong, nonatomic) NSMutableArray* lstArtists;
@property (strong, nonatomic) NSString* selectedArtist;
@property (strong, nonatomic) NSMutableArray* lstItemsForArtist;
@property (assign) jvxInt32 selectionInputFile_artist;
@property (assign) jvxInt32 selectionInputFile_title;

@property (strong, nonatomic) NSString* documentsDirectory;

@property (strong, nonatomic) jvxIosLocalMedia* localMediaRef;
@property (strong, nonatomic) NSMutableArray* localLstArtists;
//@property (strong, nonatomic) NSString* selectedLocalArtist;
@property (strong, nonatomic) NSMutableArray* localLstItemsForArtist;
@property (assign) jvxInt32 selectionInputFile_localArtist;
@property (assign) jvxInt32 selectionInputFile_localTitle;

@property (strong, nonatomic) NSMutableArray* localLstItemsForArtist_rem;
@property (assign) jvxInt32 selectionInputFile_localArtist_rem;
@property (assign) jvxInt32 selectionInputFile_localTitle_rem;


@property (assign) jvxInt32 selectionOutputFile;
@property (strong, nonatomic) NSString* nameOutputFileAdd;
@property (assign) jvxInt32 numChannelOutputFile;
@property (assign) jvxInt32 rateOutputFile;

// =========================================================
// =========================================================
@property (strong, nonatomic) NSMutableArray* lstInputSources;
@property (assign) jvxBitField selInputSource;
@property (strong, nonatomic) NSMutableArray* lstOutputSources;
@property (assign) jvxBitField selOutputSource;
@property (strong, nonatomic) NSMutableArray* lstOutputSpeaker;
@property (assign) jvxBitField selOutputSpeaker;
@property (strong, nonatomic) NSMutableArray* lstInputGains;
@property (strong, nonatomic) NSMutableArray* lstOutputGains;

@property (strong, nonatomic) NSTimer* myTimer;

// =========================================================
// States for all sub controllers
// =========================================================
@property (assign) jvxState theStateHost;
@property (assign) jvxState theStateAudioTech;
@property (assign) jvxState theStateAudioDev;
@property (assign) jvxState theStateAudioNode;
@property (assign) jvxCBool statusIsRunning;

- (void) updateWindow;
- (void) bootup_complete;

- (BOOL) report_to_viewport:(NSObject*)commandId
                 withObject:(NSObject*)info;

// Project specific functions - not mandatory
- (void) handle_states_components;

- (BOOL) register_delegate:(id<JVXViewControllerDelegate>) theNewDelegate;
- (BOOL) unregister_delegate:(id<JVXViewControllerDelegate>) theDelegateToBeRemoved;

- (void) start_timer:(float)int_secs;
- (void) stop_timer;

- (UILabel*) createLabelEntryPickerViewer:(NSString*) rowItem
                    withWidth:(float)wd
                   withHeight:(float)ht
                   doShowBold:(BOOL) showBold
                 doShowItalic:(BOOL) showIt
                                withColor:(UIColor*)theColor;

- (void) refreshTitles;
- (void) refreshLocalTitles:(jvxCBool)removeList;
- (void) update_local_infiles;
- (NSString*) idInInputFileListTech: (jvxCBool) removeLust
                     withIdOnReturn:(jvxInt32*) idOnReturn;
@end
