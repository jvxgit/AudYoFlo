#include "jvxIosCppHost.h"

// Include the static host API
#import "jvxHJvx_nd.h"

/*
 * -----------------------------------------------
 * Initialization of jvx host
 * -----------------------------------------------
 */
jvxIosCppHost::jvxIosCppHost():
    viewer_props(static_allocateData, static_deallocateData, static_copyData),
    viewer_plots(static_allocateData, static_deallocateData, static_copyData)
{
    bwd_ref.cb_entry = NULL;
    bwd_ref.cb_private = NULL;
    stateRun.running = false;
    hasBeenInitializedBefore = false;
}

jvxIosCppHost::~jvxIosCppHost()
{
}

jvxErrorType
jvxIosCppHost::initialize(callback bwd_cb, void* priv_cb, const char* fName_config)
{
    jvxErrorType res = JVX_NO_ERROR;
    jvxComponentType cpType = JVX_COMPONENT_UNKNOWN;
    bool multObj = false;
    char* argv_loc[2];
    int argc_loc = 2;
    
    if(!hasBeenInitializedBefore)
    {
        bwd_ref.cb_entry = bwd_cb;
        bwd_ref.cb_private = priv_cb;
        
        /* ========================================================
         * Obtain access to host referece in library
         *========================================================*/
        res = jvxHJvx_nd_init(&theSystemRefs.hObject);
        
        // ======================================================
        // Get pointer access to host object
        // ======================================================
        theSystemRefs.hObject->request_specialization(
                                                      (jvxHandle**)&theSystemRefs.hHost,
                                                      &cpType, &multObj);
        
        // ======================================================
        // Check component type of specialization
        // ======================================================
        if(theSystemRefs.hHost && (cpType == JVX_COMPONENT_HOST))
        {
            
            // ======================================================
            // Initialize the host..
            // ======================================================
            
            theSystemRefs.hHost->initialize(NULL);
            theSystemRefs.hHost->select();
            
            // Set cross references such that all report callbacks end up in this class
            theSystemRefs.hHost->set_external_report_target(static_cast<IjvxReport*>(this));
            
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++
            // bootup phase BEFORE config file has been processed
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++
            
            // Project pecific part of boot code
            res = bootup_specific();
            if(res != JVX_NO_ERROR)
            {
                assert(0);
            }
            
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++
            // Activate the host
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++
            
            res = theSystemRefs.hHost->activate();
            if(res != JVX_NO_ERROR)
            {
                assert(0);
            }
            
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++
            // Handle confguration files
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++
            
            // Obtain access to tools interfaces
            res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theSystemRefs.hTools));
            if(res != JVX_NO_ERROR)
            {
                assert(0);
            }
            
            // If config file was specified, start configuration from file
            if(fName_config)
            {
                configureFromFile(fName_config);
            }
            
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++
            // bootup phase AFTER config file has been processed
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++
            
            res = bootup_finalize_specific();
            if(res != JVX_NO_ERROR)
            {
                assert(0);
            }
            hasBeenInitializedBefore = true;
        }
        else
        {
            res = JVX_ERROR_UNEXPECTED;
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::terminate()
{
    jvxErrorType res = JVX_ERROR_WRONG_STATE;
    if(hasBeenInitializedBefore)
    {
        if(theSystemRefs.hObject)
        {
            res = jvxHJvx_nd_terminate(theSystemRefs.hObject);
            bwd_ref.cb_private = NULL;
            bwd_ref.cb_entry = NULL;
            hasBeenInitializedBefore = false;
        }
    }
    return(res);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++

void
jvxIosCppHost::postMessage_outThread(const char* txt, jvxReportPriority prio)
{
    std::string txtS;
    
    switch(prio)
    {
        case JVX_REPORT_PRIORITY_ERROR:
            txtS = "ERR:";
            break;
        case JVX_REPORT_PRIORITY_WARNING:
            txtS = "WARN:";
            break;
        case JVX_REPORT_PRIORITY_INFO:
            txtS = "INFO:";
            break;
        case JVX_REPORT_PRIORITY_SUCCESS:
            txtS = "SUCCESS:";
            break;
        default:
            break;
    }
    
    txtS += " ";
    txtS += txt;
       
    // Pass string to obj c host, use thread alignment
    if(bwd_ref.cb_entry)
    {
        bwd_ref.cb_entry(JVX_IOS_BWD_COMMAND_TEXT_MESSAGE, bwd_ref.cb_private, (void*)txtS.c_str());
    }
/*    if(theObjCRef)
    {
        [(__bridge jvxIosHost*)theObjCRef report_to_viewport_omt:JVX_IOS_BWD_COMMAND_TEXT_MESSAGE
                                                 withVoidPointer:];
    }*/
}

jvxErrorType
jvxIosCppHost::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
    postMessage_outThread(txt, prio);
    return(JVX_NO_ERROR);
}

jvxErrorType
jvxIosCppHost::report_internals_have_changed(jvxComponentType thisismytype, IjvxObject* thisisme,
                                                                         jvxPropertyCategoryType cat, jvxInt16 propId, bool onlyContent, jvxComponentType tpTo,
                                                                         jvxPropertyCallPurpose callpurpose)
{
    // Check config lines..
    jvxErrorType res = JVX_NO_ERROR;
    
    // Follow deployment of properties as specified in the configuration lines
    IjvxConfigurationLine* theConfigLine = NULL;
    
    // Config parameters to config line
    res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLine));
    
    if((res == JVX_NO_ERROR) && theConfigLine)
    {
        theConfigLine->report_property_was_set(thisismytype, cat, propId, onlyContent, tpTo);
        
        theSystemRefs.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle*>(theConfigLine));
    }

    if(bwd_ref.cb_entry)
    {
        bwd_ref.cb_entry(JVX_IOS_BWD_COMMAND_UPDATE_WINDOW, bwd_ref.cb_private, NULL);
    }
    return(res);
}


jvxErrorType
jvxIosCppHost::report_take_over_property(jvxComponentType thisismytype, IjvxObject* thisisme,
                                                                     jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
                                                                     jvxPropertyCategoryType cat, jvxInt16 propId, jvxComponentType tpTo, jvxPropertyCallPurpose callpurpose)
{
    jvxErrorType res = JVX_NO_ERROR;
    jvx_propertyReferenceTriple theTriple;
    jvx_initPropertyReferenceTriple(&theTriple);
    jvx_getReferencePropertiesObject(theSystemRefs.hHost, &theTriple, tpTo);
    
    if(jvx_isValidPropertyReferenceTriple(&theTriple))
    {
        res = theTriple.theProps->request_takeover_property(thisismytype, thisisme,
                                                            fld, numElements, format, offsetStart, onlyContent,
                                                            cat, propId);
        jvx_returnReferencePropertiesObject(theSystemRefs.hHost, &theTriple, tpTo);
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::report_command_request(jvxCommandRequestType request)
{
    jvxErrorType res = JVX_ERROR_UNSUPPORTED;
    switch(request)
    {
        case JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER:
            // The "void*" carries an NSDictionary* here and should be forwarded to the viewport
            if(bwd_ref.cb_entry)
            {
                bwd_ref.cb_entry(JVX_IOS_BWD_COMMAND_START, bwd_ref.cb_private, NULL);
            }
            res = JVX_NO_ERROR;
            break;

        case JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER:
            // The "void*" carries an NSDictionary* here and should be forwarded to the viewport
            if(bwd_ref.cb_entry)
            {
                bwd_ref.cb_entry(JVX_IOS_BWD_COMMAND_STOP, bwd_ref.cb_private, NULL);
            }
            res = JVX_NO_ERROR;
            break;
        case JVX_REPORT_REQUEST_UPDATE_WINDOW:
            
            // The "void*" carries an NSDictionary* here and should be forwarded to the viewport
            if(bwd_ref.cb_entry)
            {
                bwd_ref.cb_entry(JVX_IOS_BWD_COMMAND_UPDATE_WINDOW, bwd_ref.cb_private, NULL);
            }
            res = JVX_NO_ERROR;
            break;
        default:
            assert(0);
            break;
    }
    return(res);
    
    return(JVX_ERROR_UNSUPPORTED);
}

jvxErrorType
jvxIosCppHost::report_os_specific(jvxSize commandId, void* context)
{
    jvxErrorType res = JVX_ERROR_UNSUPPORTED;
    jvxIosBwdReportEvent event = (jvxIosBwdReportEvent)commandId;
    switch(event)
    {
        case JVX_IOS_BWD_COMMAND_INTERRUPTION:
        case JVX_IOS_BWD_COMMAND_NEW_ROUTE:
        case JVX_IOS_BWD_COMMAND_MEDIACENTER_RESET:
            
            // The "void*" carries an NSDictionary* here and should be forwarded to the viewport
            if(bwd_ref.cb_entry)
            {
                bwd_ref.cb_entry(event, bwd_ref.cb_private, context);
            }
            res = JVX_NO_ERROR;
            break;
        default:
            assert(0);
            break;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::request_component_filewriter(jvxHandle** instOnReturn)
{
    jvxErrorType res = JVX_NO_ERROR;
    IjvxObject* theObject = NULL;
    IjvxRtAudioFileWriter * theFileWriter = NULL;
    
    if(instOnReturn)
    {
        if(hasBeenInitializedBefore)
        {
            if(theSystemRefs.hTools)
            {
                res = theSystemRefs.hTools->instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_WRITER, 0, &theObject);
                if((res == JVX_NO_ERROR) && theObject)
                {
                    theObject->request_specialization(reinterpret_cast<jvxHandle**>(&theFileWriter), NULL, NULL);
                    theFileWriter->initialize(theSystemRefs.hHost);
                    *instOnReturn = reinterpret_cast<jvxHandle*>(theFileWriter);
                }
                else
                {
                    res = JVX_ERROR_INTERNAL;
                }
            }
            else
            {
                res = JVX_ERROR_NOT_READY;
            }
        }
        else
        {
            res = JVX_ERROR_WRONG_STATE;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::request_component_filereader(jvxHandle** instOnReturn)
{
    jvxErrorType res = JVX_NO_ERROR;
    IjvxObject* theObject = NULL;
    IjvxRtAudioFileReader * theFileReader = NULL;
    
    if(instOnReturn)
    {
        if(hasBeenInitializedBefore)
        {
            if(theSystemRefs.hTools)
            {
                res = theSystemRefs.hTools->instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER, 0, &theObject);
                if((res == JVX_NO_ERROR) && theObject)
                {
                    theObject->request_specialization(reinterpret_cast<jvxHandle**>(&theFileReader), NULL, NULL);
                    theFileReader->initialize(theSystemRefs.hHost);
                    *instOnReturn = reinterpret_cast<jvxHandle*>(theFileReader);
                }
                else
                {
                    res = JVX_ERROR_INTERNAL;
                }
            }
            else
            {
                res = JVX_ERROR_NOT_READY;
            }
        }
        else
        {
            res = JVX_ERROR_WRONG_STATE;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::return_component_filewriter(jvxHandle* instToReturn)
{
    jvxErrorType res = JVX_NO_ERROR;
    IjvxObject* theObject = NULL;
    IjvxRtAudioFileWriter * theFileWriter = NULL;

    if(instToReturn)
    {
        if(hasBeenInitializedBefore)
        {
            if(theSystemRefs.hTools)
            {
                theFileWriter = reinterpret_cast<IjvxRtAudioFileWriter*>(instToReturn);
                theFileWriter->terminate();
                theObject = static_cast<IjvxObject*>(theFileWriter);
                res = theSystemRefs.hTools->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_WRITER, 0, theObject);
            }
            else
            {
                res = JVX_ERROR_NOT_READY;
            }
        }
        else
        {
            res = JVX_ERROR_WRONG_STATE;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::return_component_filereader(jvxHandle* instToReturn)
{
    jvxErrorType res = JVX_NO_ERROR;
    IjvxObject* theObject = NULL;
    IjvxRtAudioFileReader * theFileReader = NULL;
    
    if(instToReturn)
    {
        if(hasBeenInitializedBefore)
        {
            if(theSystemRefs.hTools)
            {
                theFileReader = reinterpret_cast<IjvxRtAudioFileReader*>(instToReturn);
                theFileReader->terminate();
                theObject = static_cast<IjvxObject*>(theFileReader);
                res = theSystemRefs.hTools->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER, 0, theObject);
            }
            else
            {
                res = JVX_ERROR_NOT_READY;
            }
        }
        else
        {
            res = JVX_ERROR_WRONG_STATE;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::open_filewriter_write_batchmode(jvxHandle* hdl, const char* fName, jvxSize channels, jvxInt32 sRate, jvxDataFormat format)
{
    
    IjvxRtAudioFileWriter * theFileWriter = NULL;
    jvxEndpointDescription descr = JVX_FILETYPE_WAV;
    jvxFileDescriptionEndpoint_open fileDescr_act;
    jvxState stat = JVX_STATE_NONE;

    jvxErrorType res = JVX_NO_ERROR;
    
    if(hdl)
    {
        theFileWriter = reinterpret_cast<IjvxRtAudioFileWriter*>(hdl);
        theFileWriter->state(&stat);
        if(stat == JVX_STATE_SELECTED)
        {
            fileDescr_act.fFormatFile = format;
            fileDescr_act.lengthFile = 0;
            fileDescr_act.littleEndian = false;
            fileDescr_act.numberChannels = channels;
            fileDescr_act.samplerate = sRate;
			fileDescr_act.TODOADDADDPROPS;
            
            res = theFileWriter->activate(fName, descr, &fileDescr_act);
        }
        else
        {
            res = JVX_ERROR_WRONG_STATE;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::set_tag_filewriter_write_batchmode(jvxHandle* hdl, jvxAudioFileTagType tp, const char* tagtext)
{
    IjvxRtAudioFileWriter * theFileWriter = NULL;
    jvxState stat = JVX_STATE_NONE;
    jvxErrorType res = JVX_NO_ERROR;
    if(hdl)
    {
        theFileWriter = reinterpret_cast<IjvxRtAudioFileWriter*>(hdl);
        theFileWriter->state(&stat);
        if(stat == JVX_STATE_ACTIVE)
        {
            res = theFileWriter->add_tag(tp, tagtext);
        }
        else
        {
            res = JVX_ERROR_WRONG_STATE;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::start_filewriter_write_batchmode(jvxHandle* hdl)
{
    IjvxRtAudioFileWriter * theFileWriter = NULL;
    jvxState stat = JVX_STATE_NONE;
    jvxErrorType res = JVX_NO_ERROR;
    jvxDataFormat format = JVX_DATAFORMAT_NONE;
    jvxFileDescriptionEndpoint_prepare fileDescr_prep;
    if(hdl)
    {
        theFileWriter = reinterpret_cast<IjvxRtAudioFileWriter*>(hdl);
        theFileWriter->state(&stat);
        if(stat == JVX_STATE_ACTIVE)
        {
            theFileWriter->get_properties_file(NULL, NULL, &format, NULL);
            
            
            fileDescr_prep.fFormatAppl = format;
            fileDescr_prep.numSamplesFrameMax_moveBgrd = 0;
            res = theFileWriter->prepare(0, false, &fileDescr_prep, JVX_FILEOPERATION_BATCH);
            res = theFileWriter->start();
        }
        else
        {
            res = JVX_ERROR_WRONG_STATE;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::add_buffer_filewriter_write_batch_mode_interleaved(jvxHandle* hdl, jvxHandle* buffer, jvxSize bsize)
{
    IjvxRtAudioFileWriter * theFileWriter = NULL;
    jvxErrorType res = JVX_NO_ERROR;
    if(hdl)
    {
        theFileWriter = reinterpret_cast<IjvxRtAudioFileWriter*>(hdl);
        res = theFileWriter->add_audio_buffer((void**)buffer, 0, bsize, false);
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::stop_filewriter_write_batchmode(jvxHandle* hdl)
{
    IjvxRtAudioFileWriter * theFileWriter = NULL;
    jvxState stat = JVX_STATE_NONE;
    jvxErrorType res = JVX_NO_ERROR;
    if(hdl)
    {
        theFileWriter = reinterpret_cast<IjvxRtAudioFileWriter*>(hdl);
        theFileWriter->state(&stat);
        if(stat == JVX_STATE_PROCESSING)
        {
            res = theFileWriter->stop();
            res = theFileWriter->postprocess();
        }
        else
        {
            res = JVX_ERROR_WRONG_STATE;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::close_filewriter_write_batchmode(jvxHandle* hdl)
{
    IjvxRtAudioFileWriter * theFileWriter = NULL;
    jvxState stat = JVX_STATE_NONE;
    jvxErrorType res = JVX_NO_ERROR;
    if(hdl)
    {
        theFileWriter = reinterpret_cast<IjvxRtAudioFileWriter*>(hdl);
        theFileWriter->state(&stat);
        if(stat == JVX_STATE_ACTIVE)
        {
            res = theFileWriter->deactivate();
        }
        else
        {
            res = JVX_ERROR_WRONG_STATE;
        }
    }
    else
    {
        res = JVX_ERROR_INVALID_ARGUMENT;
    }
    return(res);
    
}

jvxErrorType
jvxIosCppHost::produce_list_file_extension(std::string dirname, std::string ext,
                                           std::vector<std::string>& lstfileNames,
                                           std::vector<std::string>& lstTagsNames,
                                           std::vector<jvxSize>& lstChannels,
                                           std::vector<jvxInt32>& lstRates,
                                           std::vector<jvxSize>& lstLengths)
{
    JVX_HANDLE_DIR dirH = NULL;
    jvxBool suc = true;
    JVX_DIR_FIND_DATA fData;
    jvxErrorType res = JVX_NO_ERROR;
    int i;
    jvxString* str = NULL;
    
    lstTagsNames.clear();
    lstChannels.clear();
    lstRates.clear();
    lstLengths.clear();
    
    dirH = JVX_FINDFIRSTFILE_WC(dirH, dirname.c_str(), ext.c_str(), fData);
    if(dirH)
    {
        while(suc)
        {
            std::string fName = JVX_GETFILENAMEDIR(fData);
            std::string showTags;
            if(ext == ".wav")
            {
                IjvxRtAudioFileReader* iFreader = NULL;
                this->request_component_filereader(reinterpret_cast<jvxHandle**>(&iFreader));
                if(iFreader)
                {
                    jvxFileDescriptionEndpoint_open theEp;
                    theEp.numberChannels = -1;
                    theEp.samplerate = -1;
                    theEp.fFormatFile = JVX_DATAFORMAT_16BIT_LE;
                    theEp.littleEndian = true;
					theEp.TODOADDNEWPROPS;
                    
                    fName = dirname + "/" + fName;
                    res = iFreader->activate(fName.c_str(), JVX_FILETYPE_WAV, &theEp);
                    if(res == JVX_NO_ERROR)
                    {
                        //iFreader->properties_file(&newFile.common.number_channels, &newFile.common.samplerate, NULL, NULL, &newFile.common.length);
                        
                        jvxAssignedFileTags theTags;
                        jvx_initTagName(theTags);
                        
                        str = NULL;
                        for(i = 0; i < JVX_NUMBER_AUDIO_FILE_TAGS; i++)
                        {
                            iFreader->get_tag((jvxAudioFileTagType)((jvxBitField)1 << i), &str);
                            if(str)
                            {
                                theTags.tagsSet |= ((jvxBitField)1 << i);
                                theTags.tags[i] = str->bString;
                                iFreader->deallocate(str);
                            }
                        }
                        
                        jvx_tagStructToTagExpr(showTags, theTags, fName);
                        
                        lstfileNames.push_back(fName);
                        lstTagsNames.push_back(showTags);
                        lstChannels.push_back(theEp.numberChannels);
                        lstRates.push_back(theEp.samplerate);
                        lstLengths.push_back(theEp.lengthFile);
                        
                        iFreader->deactivate();
                    }
                    this->return_component_filereader(reinterpret_cast<jvxHandle*>(iFreader));
                }
            }
            suc = JVX_FINDNEXTFILE(dirH, fData, ext.c_str());
        }
        JVX_FINDCLOSE(dirH);
    }
    return(JVX_NO_ERROR);
}

jvxErrorType
jvxIosCppHost::set_external_config_entry(const char* entryname, jvxHandle* val, jvxConfigSectionTypes tp, jvxInt32 id)
{
    jvxErrorType res = JVX_NO_ERROR;
    
    if(hasBeenInitializedBefore)
    {
        res = this->theSystemRefs.hHost->set_configuration_entry(entryname, val, tp, id);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::get_external_config_entry(const char* entryname, jvxHandle** val, jvxConfigSectionTypes* tp, jvxInt32 id)
{
    jvxErrorType res = JVX_NO_ERROR;
    
    if(hasBeenInitializedBefore)
    {
        res = this->theSystemRefs.hHost->get_configuration_entry(entryname, val, tp, id);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::host_deallocate(jvxHandle* val, jvxConfigSectionTypes tp)
{
    jvxErrorType res = JVX_NO_ERROR;
    
    if(hasBeenInitializedBefore)
    {
        switch(tp)
        {
            case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
                res = this->theSystemRefs.hHost->deallocate((jvxString*)val);
                break;
            case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
                res = this->theSystemRefs.hHost->deallocate((jvxStringList*)val);
                break;
            case JVX_CONFIG_SECTION_TYPE_VALUELIST:
                res = this->theSystemRefs.hHost->deallocate((jvxDataList*)val);
                break;
            default:
                res = JVX_ERROR_INVALID_ARGUMENT;
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::removeFile(const char* fName)
{
    JVX_REMOVE_FILE_RETURN result = JVX_REMOVE_FILE(fName);
    if(result == JVX_REMOVE_FILE_POS)
    {
        return(JVX_NO_ERROR);
    }
    return(JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
}
