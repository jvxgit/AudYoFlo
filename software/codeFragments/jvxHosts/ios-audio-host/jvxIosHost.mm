#import "jvxIosHost.h"
#import "jvx.h"
#import "jvxIosCppHost.h"
#import "jvxOnePropertyInstance.h"
#import "jvxIosLocalMedia.h"
#import "jvxIosLocalMediaItem.h"

/*
 * -----------------------------------------------
 * this implementation realizes the obj-c/iOs part of the rtproc host.
 * 
 * iOS <-> jvxIosHost <-> jvxIosCppHost
 * -- obj C ----------    ---- c++ ----
 *
 * -----------------------------------------------
 */

jvxErrorType bwd_callback(jvxIosBwdReportEvent commandId, void* priv_data, void *context)
{
    jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
    jvxIosHost* this_is_me = (__bridge jvxIosHost*)priv_data;
    if(this_is_me)
    {
        res = [this_is_me report_to_viewport_omt:commandId
                                 withVoidPointer:(void *)context];
    }
    return(res);
}


/*
 * -----------------------------------------------
 * Implementation of obj C frontend of host
 * -----------------------------------------------
 */

@interface jvxIosHost()

@end

@implementation jvxIosHost

/*
 * -----------------------------------------------
 * Initialization (like the constructor)
 * -----------------------------------------------
 */
- (id)init
{
    self = [super init];
    hostRef = NULL;
    refViewport = NULL;
    arrayProperties = NULL;
    return self;
}

// ===============================================================
// Helper functions
// ===============================================================

- (jvxErrorType) get_id_ref_array_comp_list:(CjvxPropertiesSimple**) ptrOnReturn
                                   returnId:(jvxInt32*)idOnReturn
                               forComponent:(jvxComponentType) cpTp
{
    jvxInt32 idx = -1;
    for(idx = 0; idx < arrayProperties.count; idx++)
    {
        jvxOnePropertyInstance* inst = [arrayProperties objectAtIndex:idx];
        if(inst)
        {
            if(inst.cp_type == cpTp)
            {
                if(ptrOnReturn)
                {
                    *ptrOnReturn = inst.ref_props;
                }
                if(idOnReturn)
                {
                    *idOnReturn = idx;
                }
                return(JVX_NO_ERROR);
            }
        }
    }
    
    if(ptrOnReturn)
    {
        *ptrOnReturn = NULL;
    }
    if(idOnReturn)
    {
        *idOnReturn = -1;
    }
    return(JVX_ERROR_ELEMENT_NOT_FOUND);
}

/*
 * -----------------------------------------------
 * Start the cpp host
 * -----------------------------------------------
 */
- (jvxErrorType) initialize:(NSObject *)bwdObj
           withFilePathSpec:(NSString *)fileDomainPath;
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theIosCppHostRef = NULL;

    // Very simple state management: NULL pointer -> uninitialized
    if(hostRef == NULL)
    {
        refViewport = bwdObj;
        fileWorkspacePath = fileDomainPath;
        
        // Allocate the cpp host
        theIosCppHostRef = new jvxIosCppHost();
        assert(theIosCppHostRef);
        
        NSString *configfile = [fileWorkspacePath stringByAppendingPathComponent:JVX_IOS_FNAME_CONFIG];

        // run bootup sequence
        theIosCppHostRef->initialize(bwd_callback, (__bridge void*)self, [configfile UTF8String]);
        
        // Store reference
        hostRef = reinterpret_cast<void*>(theIosCppHostRef);
        
        // Allocate list of property instances
        arrayProperties = [[NSMutableArray alloc] init];

    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

/*
 * -----------------------------------------------
 * Shutdown the cpp host
 * -----------------------------------------------
 */
- (jvxErrorType) terminate
{
    jvxInt32 idx;
    jvxErrorType res = JVX_NO_ERROR;
    if(hostRef)
    {
        for(idx=0; idx < arrayProperties.count; idx++)
        {
            NSObject* theObj = [arrayProperties objectAtIndex:idx];
            if(theObj)
            {
                jvxOnePropertyInstance* theInst = (jvxOnePropertyInstance*)theObj;
                if(theInst)
                {
                    [theInst deallocate_me];
                }
            }
        }
        
        [arrayProperties removeAllObjects];
        
        jvxIosCppHost* theIosCppHostRef = reinterpret_cast<jvxIosCppHost*>(hostRef);
        theIosCppHostRef->terminate();
        delete(theIosCppHostRef);
        hostRef = NULL;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

//  ===========================================================
//  ===========================================================

- (jvxErrorType) state_component:(jvxState*) stateOnReturn
                forComponentType:(jvxComponentType) tp
{
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    IjvxHost* theHostRef = theHostRefIf->get_host_ref();
    if(theHostRef)
    {
        if(tp == JVX_COMPONENT_HOST)
        {
            return(theHostRef->state(stateOnReturn));
        }
        return(theHostRef->state_component(tp, stateOnReturn));
    }
    if(stateOnReturn)
    {
        *stateOnReturn = JVX_STATE_NONE;
    }
    return(JVX_NO_ERROR);
}

- (jvxErrorType) list_component:(NSMutableArray**) lstOnReturn
                forComponentType:(jvxComponentType) tp
                 withIdSelected:(jvxInt16*)idOnReturn
{
    jvxSize i;
    jvxSize num = 0;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    IjvxHost* theHostRef = theHostRefIf->get_host_ref();
    jvxString* str = NULL;
    if(theHostRef)
    {
        theHostRef->number_components(tp, &num);
        
        if(lstOnReturn)
        {
            *lstOnReturn = [[NSMutableArray alloc] init];
            for(i = 0; i < num; i++)
            {
                str = NULL;
                theHostRef->description_component(tp, i, &str);
                if(str)
                {
                    NSString* newString = [NSString stringWithUTF8String:str->bString];
                    [*lstOnReturn addObject:newString];
                    theHostRef->deallocate(str);
                }
            }
        }
        
        if(idOnReturn)
        {
            *idOnReturn = -1;
            theHostRef->selection_component(tp, idOnReturn);
        }
 
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) select_component:(jvxComponentType) tp
                  withSelectionId:(jvxSize)idxSelect;
{
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    IjvxHost* theHostRef = theHostRefIf->get_host_ref();
    if(theHostRef)
    {
        return(theHostRef->select_component(tp, idxSelect));
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) activate_component:(jvxComponentType) tp
{
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    IjvxHost* theHostRef = theHostRefIf->get_host_ref();
    if(theHostRef)
    {
        return(theHostRef->activate_component(tp));
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) deactivate_component:(jvxComponentType) tp
{
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    IjvxHost* theHostRef = theHostRefIf->get_host_ref();
    if(theHostRef)
    {
        return(theHostRef->deactivate_component(tp));
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) unselect_component:(jvxComponentType)tp;
{
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    IjvxHost* theHostRef = theHostRefIf->get_host_ref();
    if(theHostRef)
    {
        return(theHostRef->unselect_component(tp));
    }
    return(JVX_ERROR_WRONG_STATE);
}

- (jvxErrorType) check_ready:(jvxComponentType) cpTp
      returnIsReadyHere:(jvxBool*)ready
        withReasonIfNotOnReturn:(NSString**) reason
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    IjvxHost* theHostRef = theHostRefIf->get_host_ref();
    jvxString* str = NULL;
    *reason = NULL;
    
    if(theHostRef)
    {
        res = theHostRef->is_ready_component(cpTp,ready,&str);
        if(res != JVX_NO_ERROR)
        {
            *reason = @"Component Not Ready";
        }
        else
        {
            if(!ready)
            {
                if(str)
                {
                    *reason = [NSString stringWithUTF8String:str->bString];
                }
            }
        }
        if(str)
        {
            theHostRef->deallocate(str);
        }
    }
    return(JVX_ERROR_WRONG_STATE);
    
}

-(jvxErrorType) save_config:(NSString**) errOnReturn;
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    
    NSString *configfile = [fileWorkspacePath stringByAppendingPathComponent:JVX_IOS_FNAME_CONFIG];
    if(theHostRefIf)
    {
        res = theHostRefIf->configurationToFile([configfile UTF8String]);
        if(res != JVX_NO_ERROR)
        {
            *errOnReturn = [NSString stringWithUTF8String:(jvxErrorType_descr(res).c_str())];
        }
    }
    return(res);
}
//  ===========================================================
//  ===========================================================

- (jvxErrorType) init_properties:(jvxComponentType)cpTp
{
    int idx;
    jvxErrorType res = JVX_NO_ERROR;
    jvxOnePropertyInstance* oneProp = NULL;
    jvxBool foundType = false;
    for(idx = 0; idx < arrayProperties.count; idx++)
    {
        jvxOnePropertyInstance* theExInst = (jvxOnePropertyInstance*)[arrayProperties objectAtIndex:idx];
        if(theExInst)
        {
            jvxComponentType cpTpL = theExInst.cp_type;
            if(cpTp == cpTpL)
            {
                foundType = true;
                break;
            }
        }
    }
    
    if(foundType == false)
    {
        oneProp = [[jvxOnePropertyInstance alloc] init];
        [oneProp allocate_me:cpTp withHostRef:hostRef];
        [arrayProperties addObject:oneProp];
    }
    else
    {
        res = JVX_ERROR_ALREADY_IN_USE;
    }
    
    return(res);
}

- (jvxErrorType) terminate_properties:(jvxComponentType)cpTp
{
    int idx;
    jvxErrorType res = JVX_NO_ERROR;
    jvxOnePropertyInstance* oneProp = NULL;
    jvxBool foundType = false;
    for(idx = 0; idx < arrayProperties.count; idx++)
    {
        jvxOnePropertyInstance* theExInst = (jvxOnePropertyInstance*)[arrayProperties objectAtIndex:idx];
        if(theExInst)
        {
            jvxComponentType cpTpL = theExInst.cp_type;
            if(cpTp == cpTpL)
            {
                foundType = true;
                break;
            }
        }
    }
    
    if(foundType == true)
    {
        oneProp = [arrayProperties objectAtIndex:idx];
        if(oneProp)
        {
            [oneProp deallocate_me];
        }
        
        [arrayProperties removeObjectAtIndex:idx];
    }
    else
    {
        res = JVX_ERROR_ELEMENT_NOT_FOUND;
    }
    
    return(res);
}

// ===============================================================
// Get access to properties
// ===============================================================

- (jvxErrorType) get_property_id:(NSString*) propertyName
                    forComponent:(jvxComponentType) cpTp
                      returnedId:(jvxSize*) idOnReturn
                  returnedHandle:(jvxHandle**)hdlOnReturn;
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    
    std::string prop_name;
    std::string prop_returned;
    
    res = [self get_id_ref_array_comp_list:&ptrIf
                                  returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        // Get the content
        prop_name = [propertyName UTF8String];
        res = ptrIf->get_property_id_handle(prop_name, idOnReturn, reinterpret_cast<IjvxProperties**>(hdlOnReturn));
    }
    return(res);
}

- (jvxErrorType) get_property_direct:(jvxHandle*) fld
                      forCategory:(jvxBool) isCategoryPredefined
                     withNumberElems:(jvxSize)num
                            ofFormat:(jvxDataFormat)form
                              withId:(jvxSize)idProp
                        withOffset:(jvxSize)offset
                   targetOnlyContent:(jvxBool)onlyContent
                   withValidReturned:(jvxBool*)isValid
                          withHandle:(jvxHandle*) hdl
{
    jvxErrorType res = JVX_NO_ERROR;
    IjvxProperties* theProps = reinterpret_cast<IjvxProperties*>(hdl);
    jvxPropertyCategoryType cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
    if(isCategoryPredefined)
    {
        cat = JVX_PROPERTY_CATEGORY_PREDEFINED;
    }
    
    if(theProps)
    {
        res = theProps->get_property(fld, num, form, idProp, cat, offset, onlyContent, isValid, NULL);
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

- (jvxErrorType) set_property_direct:(jvxHandle*) fld
                         forCategory:(jvxBool) isCategoryPredefined
                     withNumberElems:(jvxSize)num
                            ofFormat:(jvxDataFormat)form
                              withId:(jvxSize)idProp
                          withOffset:(jvxSize)offset
                   targetOnlyContent:(jvxBool)onlyContent
                   withValidReturned:(jvxBool*)isValid
                          withHandle:(jvxHandle*) hdl
{
    jvxErrorType res = JVX_NO_ERROR;
    IjvxProperties* theProps = reinterpret_cast<IjvxProperties*>(hdl);
    jvxPropertyCategoryType cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
    if(isCategoryPredefined)
    {
        cat = JVX_PROPERTY_CATEGORY_PREDEFINED;
    }
    
    if(theProps)
    {
        res = theProps->set_property(fld, num, form, idProp, cat, offset, onlyContent);
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

- (jvxErrorType) get_property_string: (NSString**) returnedPropValue
                 withNameProperty:(NSString*) propertyName
                 forComponent:(jvxComponentType) cpTp
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    
    std::string prop_name;
    std::string prop_returned;
    
    res = [self get_id_ref_array_comp_list:&ptrIf
                                        returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        // Get the content
        prop_name = [propertyName UTF8String];
        res = ptrIf->get_property(prop_name, prop_returned);
        if(res == JVX_NO_ERROR)
        {
            if(returnedPropValue)
            {
                *returnedPropValue = [NSString stringWithUTF8String:prop_returned.c_str()];
            }
        }
    }
    return(res);
}

- (jvxErrorType) set_property_string: (NSString*) value
                    withNameProperty:(NSString*) propertyName
                        forComponent:(jvxComponentType) cpTp
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    
    std::string prop_name;
    std::string value_name;
    res = [self get_id_ref_array_comp_list:&ptrIf
                                  returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        // Get the content
        prop_name = [propertyName UTF8String];
        value_name = [value UTF8String];
        jvxString str;
        str.bString = (char*)value_name.c_str();
        str.lString = value_name.length();
        
        res = ptrIf->set_property(prop_name, &str);
    }
    return(res);
}

// =======================================================
// =======================================================


- (jvxErrorType) get_property_numeric:(jvxHandle*) retVal
                        forDataFormat:(jvxDataFormat)form
                     withNameProperty:(NSString*) propertyName
                         forComponent:(jvxComponentType) cpTp
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    
    std::string prop_name;
    
    res = [self get_id_ref_array_comp_list:&ptrIf
                                  returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        // Get the content
        prop_name = [propertyName UTF8String];
        res = ptrIf->get_property_typesafe(prop_name,retVal,form,1,0);
    }
    return(res);
}

- (jvxErrorType) get_property_numeric:(jvxHandle*) retVal
                        forDataFormat:(jvxDataFormat)form
                           withOffsetArray:(jvxSize) offset
                     withNameProperty:(NSString*) propertyName
                         forComponent:(jvxComponentType) cpTp
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    
    std::string prop_name;
    
    res = [self get_id_ref_array_comp_list:&ptrIf
                                  returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        // Get the content
        prop_name = [propertyName UTF8String];
        res = ptrIf->get_property_typesafe(prop_name,retVal,form,1,offset);
    }
    return(res);
}

- (jvxErrorType) set_property_numeric:(jvxHandle*) inVal
                        forDataFormat:(jvxDataFormat)form
                     withNameProperty:(NSString*) propertyName
                         forComponent:(jvxComponentType) cpTp
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    
    std::string prop_name;
    
    res = [self get_id_ref_array_comp_list:&ptrIf
                                  returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        // Get the content
        prop_name = [propertyName UTF8String];
        res = ptrIf->set_property_typesafe(prop_name, inVal,form,1,0);
    }
    return(res);
}

- (jvxErrorType) set_property_numeric:(jvxHandle*) inVal
                        forDataFormat:(jvxDataFormat)form
                      withOffsetArray:(jvxSize) offset
                     withNameProperty:(NSString*) propertyName
                         forComponent:(jvxComponentType) cpTp
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    
    std::string prop_name;
    
    res = [self get_id_ref_array_comp_list:&ptrIf
                                  returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        // Get the content
        prop_name = [propertyName UTF8String];
        res = ptrIf->set_property_typesafe(prop_name, inVal,form,1,offset);
    }
    return(res);
}

// =======================================================
// =======================================================


- (jvxErrorType) get_property_string_list: (NSMutableArray**) returnedList
                    withNameProperty:(NSString*) propertyName
                             forComponent:(jvxComponentType) cpTp;
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    jvxStringList* strList = NULL;
    int i;
    
    std::string prop_name;
    std::string prop_returned;
    
    res = [self get_id_ref_array_comp_list:&ptrIf
                                  returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        // Get the content
        prop_name = [propertyName UTF8String];
        res = ptrIf->get_property_typesafe(prop_name, &strList, JVX_DATAFORMAT_STRING_LIST, 1, 0, false);
        if(res == JVX_NO_ERROR)
        {
            if(strList)
            {
                if(returnedList)
                {
                    *returnedList = [[NSMutableArray alloc] init];
                    for(i=0; i < strList->lStrings; i++)
                    {
                        NSString* newStr = [NSString stringWithUTF8String:strList->bStrings[i].bString];
                        [*returnedList addObject:newStr];
                    }
                }
            }
            ptrIf->deallocate(strList);
        }
    }
    return(res);
}

- (jvxErrorType) get_property_selection_list: (NSMutableArray**) returnedList
                           returnedSelection:(jvxBitField*) selection
                            withNameProperty:(NSString*) propertyName
                                forComponent:(jvxComponentType) cpTp
                                 contentOnly:(jvxBool) onlyContent
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    jvxSelectionList selList;
    selList.strList = NULL;
    selList.bitFieldSelected = 0;
    int i;
    
    std::string prop_name;
    std::string prop_returned;
    
    res = [self get_id_ref_array_comp_list:&ptrIf
                                  returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        // Get the content
        prop_name = [propertyName UTF8String];
        res = ptrIf->get_property_typesafe(prop_name, &selList, JVX_DATAFORMAT_SELECTION_LIST, 1, 0, onlyContent);
        if(res == JVX_NO_ERROR)
        {
            if(selList.strList)
            {
                if(returnedList)
                {
                    *returnedList = [[NSMutableArray alloc] init];
                    for(i=0; i < selList.strList->lStrings; i++)
                    {
                        NSString* newStr = [NSString stringWithUTF8String:selList.strList->bStrings[i].bString];
                        [*returnedList addObject:newStr];
                    }
                }
            }
            if(selection)
            {
                *selection = selList.bitFieldSelected;
            }
            ptrIf->deallocate(selList.strList);
        }
    }
    return(res);
}


- (jvxErrorType) set_property_selection_list: (NSArray*) passList
                                newSelection:(jvxBitField) selection
                            withNameProperty:(NSString*) propertyName
                                forComponent:(jvxComponentType) cpTp
{
    jvxErrorType res = JVX_NO_ERROR;
    CjvxPropertiesSimple* ptrIf = NULL;
    jvxInt32 idx = -1;
    jvxSelectionList selList;
    selList.strList = NULL;
    selList.bitFieldSelected = 0;
    int i;
    jvxBool onlyContent = false;
    
    std::string prop_name;
    std::string prop_returned;
    
    res = [self get_id_ref_array_comp_list:&ptrIf
                                  returnId:&idx
                              forComponent:cpTp];
    
    if(res == JVX_NO_ERROR)
    {
        if(passList == NULL)
        {
            onlyContent = true;
        }
        else
        {
            std::vector<std::string> entries;
            for(i = 0; i < passList.count; i++)
            {
                NSString* str = [passList objectAtIndex:i];
                entries.push_back([str UTF8String]);
            }
            onlyContent = false;
            HjvxObject_produceStringList(&selList.strList, entries);
        }
        
        selList.bitFieldSelected = selection;
        
        // Get the content
        prop_name = [propertyName UTF8String];
        
        res = ptrIf->set_property_typesafe(prop_name, &selList, JVX_DATAFORMAT_SELECTION_LIST, 1, 0, onlyContent);
        
        if(passList)
        {
            HjvxObject_deallocate(selList.strList);
        }
    }
    return(res);
}

-(jvxErrorType) start_run
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    
    if(theHostRefIf)
    {
        res = theHostRefIf->start_run();
    }
    return(res);
}

-(jvxErrorType) stop_run
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    
    if(theHostRefIf)
    {
        res = theHostRefIf->stop_run();
    }
    return(res);
}

-(jvxErrorType) state_run:(jvxBool*)isRunning
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    
    if(theHostRefIf)
    {
        res = theHostRefIf->state_run(isRunning);
    }
    return(res);
}

// ===============================================================
// HELPERS
// ===============================================================

- (int) helper_bitField_2_selection:(jvxBitField) fld
{
    int i;
    int retVal = -1;
    for(i = 0; i < (sizeof(jvxBitField)*8); i++)
    {
        if(fld & ((jvxBitField)1 << i))
        {
            retVal = i;
            break;
        }
    }
    return(retVal);
}

- (NSString*) helper_classify_channel_token:(NSString*) token
          withReturnChannelType:(jvxGenericWrapperChannelType*) tp
              withReturnCodedId1:(int*)id1
                             withFieldWidth:(int)width
{
     NSString* retToken = token;
    
    jvxGenericWrapperChannelType tpC = JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE;
    int idC1 = -1;
    jvxErrorType resL = JVX_NO_ERROR;
    std::string txt = [token UTF8String];
    std::string nmChan;
    
    resL = jvx_genw_decodeChannelName(txt, tpC, nmChan, idC1);
    
    if(tpC == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE)
    {
        jvxAssignedFileTags tag;
        std::string path;
        jvx_tagExprToTagStruct(nmChan, tag, path);
        if(tag.tagsSet)
        {
            tag.tagsSet = tag.tagsSet & (~JVX_AUDIO_FILE_TAG_BTFLD_CREATION_DATA);
            nmChan = jvx_tagStructToUserView(tag);
        }
        else
        {
            nmChan = path;
        }
        
    }
    
    if(resL == JVX_NO_ERROR)
    {
        nmChan = jvx_shortenStringName(width, nmChan);
        retToken = [NSString stringWithUTF8String:nmChan.c_str()];
        if(tp)
        {
            *tp = tpC;
        }
        
        if(id1)
        {
            *id1 = idC1;
        }
    }
    return(retToken);
}

- (NSString*) helper_tagExprToTitle:(NSString*) expr;
{
    std::string tag = [expr UTF8String];
    jvxAssignedFileTags tags;
    std::string path;
    
    jvx_tagExprToTagStruct(tag, tags, path);
    if(tags.tagsSet & JVX_AUDIO_FILE_TAG_BTFLD_TITLE)
    {
        return([NSString stringWithUTF8String:tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE, JVX_NUMBER_AUDIO_FILE_TAGS)].c_str()]);
    }
    return([NSString stringWithUTF8String:path.c_str()]);
}

- (NSString*) helper_tagsToExpr:(NSString*)artist
                      withTitle:(NSString*)title
                       withDate:(NSString*)date;
{
    jvxAssignedFileTags tags;
    jvx_initTagName(tags);
    std::string outStr, path;
    
    if(artist)
    {
        tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_ARTIST;
        tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST,JVX_NUMBER_AUDIO_FILE_TAGS)] = [artist UTF8String];
    }
    if(title)
    {
        tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_TITLE;
        tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE,JVX_NUMBER_AUDIO_FILE_TAGS)] = [title UTF8String];
    }
    if(date)
    {
        tags.tagsSet |= JVX_AUDIO_FILE_TAG_BTFLD_CREATION_DATA;
        tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_CREATION_DATA,JVX_NUMBER_AUDIO_FILE_TAGS)] = [date UTF8String];
    }
    jvx_tagStructToTagExpr(outStr, tags, "");
    return([NSString stringWithUTF8String:outStr.c_str()]);
}

- (NSString*) helper_errorToErrorDescrption:(jvxErrorType)err;
{
    NSString* retVal = [NSString stringWithUTF8String:jvxErrorType_descr(err).c_str()];
    return retVal;
}

// ===============================================================
// BACKWARD CALLBACKS
// ===============================================================

/* 
 * Incoming messages that will be aligned into the main thread 
 */
 
- (jvxErrorType) report_to_viewport_omt: (jvxIosBwdReportEvent) reportEnum
                         withVoidPointer:(void *)info;
{
    char* txt = NULL;
    NSString * txtNs = NULL;
    NSDictionary* infoNs = NULL;
    NSObject* dispatchThis = NULL;
    jvxBool dispatchIt = false;
    
    switch(reportEnum)
    {
        case JVX_IOS_BWD_COMMAND_TEXT_MESSAGE:
            txt = (char*) info;
            txtNs = [NSString stringWithCString:txt encoding:[NSString defaultCStringEncoding]];
            dispatchThis = (NSObject*)txtNs;
            dispatchIt = true;
            break;
        case JVX_IOS_BWD_COMMAND_BOOTUP_COMPLETE:
            dispatchIt = true;
            break;
        case JVX_IOS_BWD_COMMAND_NEW_ROUTE:
        case JVX_IOS_BWD_COMMAND_INTERRUPTION:
        case JVX_IOS_BWD_COMMAND_MEDIACENTER_RESET:
            NSLog(@"IOS EVENTS ARRIVED!");
            dispatchIt = true;
            infoNs = (__bridge NSDictionary*)info;
            dispatchThis = (NSObject*)infoNs;
            break;
            
        default:
            break;
    }
    
    if(dispatchIt)
    {
        // Unload the event to the main thread
        dispatch_async(dispatch_get_main_queue(), ^{
            [self report_to_viewport_imt:reportEnum withObjPointer:dispatchThis];
        });
    }
    
    return(JVX_NO_ERROR);
}

/*
 
- (jvxErrorType) report_simple_text_message_otm:(NSString*) txt
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self report_simple_text_message_imt:txt];
    });
    return(JVX_NO_ERROR);
}
    
- (jvxErrorType) report_request_action_omt:(jvxIosRequestAction) act
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self report_request_action_imt:act];
    });
    return(JVX_NO_ERROR);
}
*/

// ===============================================================
// ===============================================================

/* Target for all incoming thread events - mapped to main thread */
- (void) report_to_viewport_imt: (jvxIosBwdReportEvent) reportEnum
                withObjPointer: (NSObject*) info;
{
    jvxBool reportToVp = false;
    switch(reportEnum)
    {
        case JVX_IOS_BWD_COMMAND_NONE:
            reportToVp = true;
            break;
        case JVX_IOS_BWD_COMMAND_NEW_ROUTE:
            reportToVp = true;
            break;
        case JVX_IOS_BWD_COMMAND_INTERRUPTION:
            reportToVp = true;
            break;
        case JVX_IOS_BWD_COMMAND_MEDIACENTER_RESET:
            reportToVp = true;
            break;
        case JVX_IOS_BWD_COMMAND_BOOTUP_COMPLETE:
            NSLog(@"Report: Bootup complete");
            reportToVp = true;
            break;
        case JVX_IOS_BWD_COMMAND_TEXT_MESSAGE:
            //NSLog(@"Report: Text message");
            reportToVp = true;
            break;
        case JVX_IOS_BWD_COMMAND_START:
            reportToVp = true;
            break;
        case JVX_IOS_BWD_COMMAND_STOP:
            reportToVp = true;
            break;
        case JVX_IOS_BWD_COMMAND_UPDATE_WINDOW:
            reportToVp = true;
            break;
        default:
            NSLog(@"Report: Unknwon reason");
            break;
    }
    if(reportToVp)
    {
        if(refViewport)
        {
            if([refViewport respondsToSelector:@selector(report_to_viewport:withObject:)])
            {
                [refViewport performSelector:@selector(report_to_viewport:withObject:)
                                  withObject:[NSNumber numberWithInteger:reportEnum]
                                  withObject:info];
            }
        }
        /*
        if(cb_entry)
        {
            cb_entry(reportEnum, cb_private_data, info);
        }
         */
    }
}

- (jvxErrorType) open_wav_writer_from_audiolib: (jvxSize) channels
                                      withRate:(jvxInt32) sRate
                                    withFormat:(jvxDataFormat) format
                                 withArtistTag:(NSString*) artist
                                  withTitleTag:(NSString*) title
                                  returnHandle:(jvxHandle**) hdl;
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    
    if(hdl)
    {
        if(theHostRefIf)
        {
            
            res = theHostRefIf->request_component_filewriter(hdl);
            if(res == JVX_NO_ERROR)
            {
                std::string fNameOut;
                std::string path = [fileWorkspacePath UTF8String];
                jvx_produceFilenameWavOut(fNameOut, path );
                
                res = theHostRefIf->open_filewriter_write_batchmode(*hdl, fNameOut.c_str(), channels, sRate, format);
                if(res == JVX_NO_ERROR)
                {
                    if(artist)
                    {
                        theHostRefIf->set_tag_filewriter_write_batchmode(*hdl, JVX_AUDIO_FILE_TAG_BTFLD_ARTIST, [artist UTF8String]);
                    }
                    if(title)
                    {
                        theHostRefIf->set_tag_filewriter_write_batchmode(*hdl, JVX_AUDIO_FILE_TAG_BTFLD_TITLE, [title UTF8String]);
                    }
                    
                    res = theHostRefIf->start_filewriter_write_batchmode(*hdl);
                }
            }
            else
            {
                res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
            }
        }
        else
        {
            res = JVX_ERROR_NOT_READY;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

- (jvxErrorType) close_wav_writer_from_audiolib: (jvxHandle*) hdl;
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    
    if(hdl)
    {
        if(theHostRefIf)
        {
            res = theHostRefIf->stop_filewriter_write_batchmode(hdl);
            res = theHostRefIf->return_component_filewriter(hdl);
        }
        else
        {
            res = JVX_ERROR_NOT_READY;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

- (jvxErrorType) add_buffer_wav_writer: (jvxHandle*) hdl
                             withInput:(jvxHandle*) buffer
                     withNumberSamples:(jvxSize) bsize;
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    if(hdl)
    {
        if(theHostRefIf)
        {
            res = theHostRefIf->add_buffer_filewriter_write_batch_mode_interleaved(hdl, buffer, bsize);
        }
        else
        {
            res = JVX_ERROR_NOT_READY;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

- (jvxErrorType) produce_list_filetypes: (NSString*) directoryName
                      withFileExtension:(NSString*) extension
                           returnedList:(jvxIosLocalMedia**)mediaOnReturn;
{
    jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    int i,j;
    
    if(theHostRefIf)
    {
        std::vector<std::string> lstfileNames;
        std::vector<std::string> lstTagsNames;
        std::vector<jvxSize> lstChannels;
        std::vector<jvxInt32> lstRates;
        std::vector<jvxSize> lstLengths;
        res = theHostRefIf->produce_list_file_extension([directoryName UTF8String], [extension UTF8String],
                                                        lstfileNames,
                                                        lstTagsNames,
                                                        lstChannels,
                                                        lstRates,
                                                        lstLengths);
        
        if(mediaOnReturn)
        {
            *mediaOnReturn = [[jvxIosLocalMedia alloc] init];
            for(i = 0; i <lstfileNames.size(); i++)
            {
                jvxIosLocalMediaItem* newItem = [[jvxIosLocalMediaItem alloc] init];
                jvxAssignedFileTags tags;
                std::string pathnm;
                
                newItem.nmArtist = @"Unknown";
                newItem.nmTitle = @"Unknown";
                newItem.nmDate = @"Unknown";
                
                resL = jvx_tagExprToTagStruct(lstTagsNames[i], tags, pathnm);
                if(resL == JVX_NO_ERROR)
                {
                    if(tags.tagsSet & JVX_AUDIO_FILE_TAG_BTFLD_ARTIST)
                    {
                        newItem.nmArtist = [NSString stringWithUTF8String:tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST,
                                                                                                             JVX_NUMBER_AUDIO_FILE_TAGS)].c_str()];
                    }
                    if(tags.tagsSet & JVX_AUDIO_FILE_TAG_BTFLD_TITLE)
                    {
                        newItem.nmTitle = [NSString stringWithUTF8String:tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_TITLE,
                                                                                                             JVX_NUMBER_AUDIO_FILE_TAGS)].c_str()];
                    }
                    if(tags.tagsSet & JVX_AUDIO_FILE_TAG_BTFLD_CREATION_DATA)
                    {
                        newItem.nmDate = [NSString stringWithUTF8String:tags.tags[jvx_bitfieldSelection2Id(JVX_AUDIO_FILE_TAG_BTFLD_CREATION_DATA,
                                                                                                             JVX_NUMBER_AUDIO_FILE_TAGS)].c_str()];
                    }
                }
                newItem.nmPath = [NSString stringWithUTF8String:lstfileNames[i].c_str()];
                newItem.channels = lstChannels[i];
                newItem.rate = lstRates[i];
                newItem.length = lstLengths[i];
                
                [(*mediaOnReturn).allCurrentItems addObject:newItem];
                
                jvxBool foundit = false;
                for(j= 0; j < (*mediaOnReturn).allCurrentArtists.count; j++)
                {
                    if([newItem.nmArtist isEqualToString:[(*mediaOnReturn).allCurrentArtists objectAtIndex:j]])
                    {
                        foundit = true;
                        break;
                    }
                }
                if(!foundit)
                {
                    [(*mediaOnReturn).allCurrentArtists addObject:newItem.nmArtist];
                }
            }
        }
    }
    else
    {
        res = JVX_ERROR_NOT_READY;
    }
    return(res);
}

- (jvxErrorType) set_entry_config_file:(NSString*)token
                       dataListToStore:(NSMutableArray*)lst
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    int i;
    
    if(theHostRefIf)
    {
        if(lst.count > 0)
        {
            jvxDataList newList;
            newList.lDblList = lst.count;
            JVX_DSP_SAFE_ALLOCATE_FIELD(newList.bDblList, jvxData, newList.lDblList);
            for(i = 0; i < lst.count; i++)
            {
                NSNumber* lstVal = [lst objectAtIndex:i];
                
#ifdef JVX_DATA_FORMAT_FLOAT
                newList.bDblList[i] = lstVal.floatValue;
#else
                newList.bDblList[i] = lstVal.doubleValue;
#endif
                
            }
            
            theHostRefIf->set_external_config_entry([token UTF8String], (jvxHandle*)&newList, JVX_CONFIG_SECTION_TYPE_VALUELIST, 0);
            
            JVX_DSP_SAFE_DELETE_FIELD(newList.bDblList);
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

- (jvxErrorType) set_entry_config_file:(NSString*)token
                       stringToStore:(NSString*)str
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    
    if(theHostRefIf)
    {
        jvxString fldStr;
        std::string txt;
        txt = [str UTF8String];
        
        fldStr.bString = (char*)txt.c_str();
        fldStr.lString = txt.size();
        
        
        theHostRefIf->set_external_config_entry([token UTF8String], (jvxHandle*)&fldStr, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING, 0);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

- (jvxErrorType) removeFile:(NSString*) fName;
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    jvxString* fldStr = NULL;
    jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
    
    if(theHostRefIf)
    {
        res = theHostRefIf->removeFile([fName UTF8String]);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}


- (jvxErrorType) get_entry_config_file:(NSString*)token
                       dataListToExport:(NSMutableArray**)lst
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    int i;
    jvxDataList* fldLst = NULL;
    jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
    
    if(theHostRefIf)
    {
        theHostRefIf->get_external_config_entry([token UTF8String], (jvxHandle**)&fldLst, &tp, 0);
        if(fldLst)
        {
            switch(tp)
            {
                case JVX_CONFIG_SECTION_TYPE_VALUELIST:
                    if(fldLst->lDblList > 0)
                    {
                        *lst = [[NSMutableArray alloc] init];
                        for(i = 0; i < fldLst->lDblList; i++)
                        {
#ifdef JVX_DATA_FORMAT_FLOAT
                            [*lst addObject:[NSNumber numberWithFloat:fldLst->bDblList[i] ] ];
#else
                            [*lst addObject:[NSNumber numberWithDouble:[fldLst->bDblList[i] ] ];
                            
#endif
                        }
                    }
                    break;
                default:
                    break;
            }
            theHostRefIf->host_deallocate(fldLst, tp);
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

                             
- (jvxErrorType) get_entry_config_file:(NSString*)token
                        stringToExport:(NSString**)str;
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxIosCppHost* theHostRefIf = (jvxIosCppHost*)hostRef;
    jvxString* fldStr = NULL;
    jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
    
    if(theHostRefIf)
    {
        res = theHostRefIf->get_external_config_entry([token UTF8String], (jvxHandle**)&fldStr, &tp, 0);
        if(fldStr)
        {
            switch(tp)
            {
                case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
                    *str = [NSString stringWithUTF8String:fldStr->bString];
                    
                    break;
                default:
                    break;
            }
            theHostRefIf->host_deallocate(fldStr, tp);
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}
                             


/*
 - (void) report_simple_text_message_imt:(NSString*) txt
 {
 NSLog(@"--> %@", txt);
 }
 - (void) report_request_action_imt:(jvxIosRequestAction) act
 {
 NSLog(@"--> Request Action");
 
 }
 */

@end
