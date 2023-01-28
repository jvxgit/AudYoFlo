//
//  jvxIosHost.h
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 27.12.15.
//  Copyright © 2015 Javox Solutions. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "jvx_dataformats.h"
#import "jvx_dataformats.h"
#import "jvx_dsp_base.h"
#import "typedefs/TjvxTypes.h"
#import "jvxIosHost_typedefs.h"
#import "typedefs/jvxTools/TjvxGenericWrapperApi.h"
#import "jvxIosLocalMedia.h"

#define JVX_IOS_FNAME_CONFIG @"configios.jvx"

@interface jvxIosHost : NSObject
{
    void* hostRef;
    NSObject* refViewport;
    
    NSMutableArray* arrayProperties;
    NSString* fileWorkspacePath;
}

- (jvxErrorType) initialize: bwdObj
           withFilePathSpec:(NSString*)fileDomainPath;

- (jvxErrorType) terminate;

// ===========================================================
// Handle components
// ===========================================================

- (jvxErrorType) state_component:(jvxState*) stateOnReturn
                forComponentType:(jvxComponentType) tp;

- (jvxErrorType) list_component:(NSMutableArray**) lstOnReturn
               forComponentType:(jvxComponentType) tp
                 withIdSelected:(jvxInt16*) idOnReturn;

- (jvxErrorType) select_component:(jvxComponentType)tp
                  withSelectionId:(jvxSize)idxSelect;

- (jvxErrorType) activate_component:(jvxComponentType) tp;

- (jvxErrorType) deactivate_component:(jvxComponentType) tp;

- (jvxErrorType) unselect_component:(jvxComponentType)tp;

- (jvxErrorType) check_ready:(jvxComponentType) cpTp
           returnIsReadyHere:(jvxCBool*)ready
     withReasonIfNotOnReturn:(NSString**) reason;

// ===========================================================
// General functions
// ===========================================================

-(jvxErrorType) save_config:(NSString**) errOnReturn;

-(jvxErrorType) start_run;

-(jvxErrorType) stop_run;

-(jvxErrorType) state_run:(jvxCBool*)isRunning;

// ===========================================================
// Handle properties
// ===========================================================

- (jvxErrorType) init_properties:(jvxComponentType)cpTp;

- (jvxErrorType) terminate_properties:(jvxComponentType)cpTp;

- (jvxErrorType) get_property_id:(NSString*) propertyName
                    forComponent:(jvxComponentType) cpTp
                      returnedId:(jvxSize*) idOnReturn
                  returnedHandle:(jvxHandle**)hdlOnReturn;

- (jvxErrorType) get_property_direct:(jvxHandle*) fld
                         forCategory:(jvxCBool) isCategorySpecific
                     withNumberElems:(jvxSize)num
                            ofFormat:(jvxDataFormat)form
                              withId:(jvxSize)idProp
                          withOffset:(jvxSize)offset
                   targetOnlyContent:(jvxCBool)onlyContent
                   withValidReturned:(jvxCBool*)isValid
                          withHandle:(jvxHandle*) hdl;

- (jvxErrorType) set_property_direct:(jvxHandle*) fld
                         forCategory:(jvxCBool) isCategorySpecific
                     withNumberElems:(jvxSize)num
                            ofFormat:(jvxDataFormat)form
                              withId:(jvxSize)idProp
                          withOffset:(jvxSize)offset
                   targetOnlyContent:(jvxCBool)onlyContent
                   withValidReturned:(jvxCBool*)isValid
                          withHandle:(jvxHandle*) hdl;

- (jvxErrorType) get_property_string: (NSString**) returnedPropValue
                    withNameProperty:(NSString*) propertyName
                        forComponent:(jvxComponentType) cpTp;

- (jvxErrorType) set_property_string: (NSString*) value
                    withNameProperty:(NSString*) propertyName
                        forComponent:(jvxComponentType) cpTp;

- (jvxErrorType) get_property_numeric:(jvxHandle*) retVal
            forDataFormat:(jvxDataFormat)form
                     withNameProperty:(NSString*) prop_name
                         forComponent:(jvxComponentType) tp;

- (jvxErrorType) get_property_numeric:(jvxHandle*) retVal
                        forDataFormat:(jvxDataFormat)form
                      withOffsetArray:(jvxSize) offset
                     withNameProperty:(NSString*) propertyName
                         forComponent:(jvxComponentType) cpTp;

- (jvxErrorType) set_property_numeric:(jvxHandle*) inVal
                        forDataFormat:(jvxDataFormat)form
                     withNameProperty:(NSString*) prop_name
                         forComponent:(jvxComponentType) tp;

- (jvxErrorType) set_property_numeric:(jvxHandle*) inVal
                        forDataFormat:(jvxDataFormat)form
                      withOffsetArray:(jvxSize) offset
                     withNameProperty:(NSString*) propertyName
                         forComponent:(jvxComponentType) cpTp;

- (jvxErrorType) get_property_string_list: (NSMutableArray**) returnedList
                         withNameProperty:(NSString*) propertyName
                             forComponent:(jvxComponentType) cpTp;

- (jvxErrorType) get_property_selection_list: (NSMutableArray**) returnedList
                           returnedSelection:(jvxBitField*) selection
                            withNameProperty:(NSString*) propertyName
                                forComponent:(jvxComponentType) cpTp
                                 contentOnly:(jvxCBool) onlyContent;

- (jvxErrorType) set_property_selection_list: (NSMutableArray*) passList
                           newSelection:(jvxBitField) selection
                            withNameProperty:(NSString*) propertyName
                                forComponent:(jvxComponentType) cpTp;


/*
 * ---------------------------------------
 * Helper functions
 * ---------------------------------------*/

- (int) helper_bitField_2_selection:(jvxBitField) fld;

- (NSString*) helper_classify_channel_token:(NSString*) token
               withReturnChannelType:(jvxGenericWrapperChannelType*) tp
                  withReturnCodedId1:(int*)id1
                             withFieldWidth:(int)width;

- (NSString*) helper_tagExprToTitle:(NSString*) expr;

- (NSString*) helper_tagsToExpr:(NSString*)artist
                        withTitle:(NSString*)title
                       withDate:(NSString*)date;

- (NSString*) helper_errorToErrorDescrption:(jvxErrorType)err;

/*
 * ---------------------------------------
 * Backward reference entry points
 * ---------------------------------------*/

// Report whatever to viewport class - thread safe entry

- (jvxErrorType) report_to_viewport_omt: (jvxIosBwdReportEvent) reportEnum
                         withVoidPointer: (void*) info;

// Report whatever to viewport class - in main thread implementation

- (void) report_to_viewport_imt: (jvxIosBwdReportEvent) reportEnum
                 withObjPointer: (NSObject*) info;

/* Wav file writer */
- (jvxErrorType) open_wav_writer_from_audiolib: (jvxSize) channels
                                      withRate:(jvxInt32) sRate
                                    withFormat:(jvxDataFormat) format
                                 withArtistTag:(NSString*) artist
                                  withTitleTag:(NSString*) title
                                  returnHandle:(jvxHandle**) hdl;

- (jvxErrorType) close_wav_writer_from_audiolib: (jvxHandle*) hdl;

- (jvxErrorType) removeFile:(NSString*) fName;

- (jvxErrorType) add_buffer_wav_writer: (jvxHandle*) hdl
                        withInput:(jvxHandle*) buffer
                      withNumberSamples:(jvxSize) bsize;


- (jvxErrorType) produce_list_filetypes: (NSString*) directoryName
                      withFileExtension:(NSString*) extension
                           returnedList:(jvxIosLocalMedia**)mediaOnReturn;

- (jvxErrorType) set_entry_config_file:(NSString*)token
                       dataListToStore:(NSMutableArray*)lst;

- (jvxErrorType) set_entry_config_file:(NSString*)token
                         stringToStore:(NSString*)str;

- (jvxErrorType) get_entry_config_file:(NSString*)token
                      dataListToExport:(NSMutableArray**)lst;

- (jvxErrorType) get_entry_config_file:(NSString*)token
                      stringToExport:(NSString**)str;

@end

