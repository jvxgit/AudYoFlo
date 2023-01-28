#import "jvxAudioProcessor.h"
#import "jvxAudioProcessor_cpp.h"

void my_callback(jvxIosBwdReportEvent commandId, void* priv, void *context)
{
    jvxAudioProcessor_cpp* this_is_me = reinterpret_cast<jvxAudioProcessor_cpp*>(priv);
    if(this_is_me)
    {
        this_is_me->report_os_specific((jvxSize)commandId, context);
    }
}

jvxAudioProcessor_cpp::jvxAudioProcessor_cpp()
{
    reference_obj_c = NULL;
    theState = JVX_STATE_INIT;
}

jvxAudioProcessor_cpp::~jvxAudioProcessor_cpp()
{

}

jvxErrorType
jvxAudioProcessor_cpp::state(jvxState* stateOnReturn)
{
    if(stateOnReturn)
    {
        *stateOnReturn = theState;
    }
  return(JVX_NO_ERROR);
}

jvxErrorType
jvxAudioProcessor_cpp::initializeTechnology(IjvxHiddenInterface* hostRef)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_INIT)
    {
        jvxAudioProcessor* theProc = [[jvxAudioProcessor alloc] init];
        assert(theProc);
        
        theHostRef = hostRef;
        assert(res == JVX_NO_ERROR);
        JVX_GET_TICKCOUNT_US_SETREF(&refTStamp);
        
        res = [theProc initAudioTechnology:(void*)this
                               andCallback:my_callback];

        assert(res == JVX_NO_ERROR);
                
        
        reference_obj_c = (__bridge jvxHandle *)(theProc);
        theState = JVX_STATE_SELECTED;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::get_number_input_ports(jvxSize* num)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState >= JVX_STATE_SELECTED)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc get_number_of_ports:num];
        assert(res == JVX_NO_ERROR);
            }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
 
}

jvxErrorType
jvxAudioProcessor_cpp::get_description_input_port(jvxSize idx, std::string& DescrOnReturn)
{
    jvxErrorType res = JVX_NO_ERROR;
    DescrOnReturn.clear();
    
    if(theState >= JVX_STATE_SELECTED)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        NSString* str = NULL;
        res = [theProc get_description_port:idx returnString:&str];
        
        assert(str);
        assert(res == JVX_NO_ERROR);
        
        DescrOnReturn = [str UTF8String];
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
 
}

jvxErrorType
jvxAudioProcessor_cpp::terminateTechnology()
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc terminateAudioTechnology];
        assert(res == JVX_NO_ERROR);
        
        theHostRef = NULL;
        theState = JVX_STATE_INIT;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
  return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::initializeDevice(jvxSize portId)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_SELECTED)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc initAudioDevice:portId];
        assert(res == JVX_NO_ERROR);
        
         theState = JVX_STATE_ACTIVE;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::terminateDevice()
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc terminateAudioDevice];
        assert(res == JVX_NO_ERROR);
        
        theState = JVX_STATE_SELECTED;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

void
jvxAudioProcessor_cpp::report_os_specific(jvxSize commandId, void *context)
{
    jvxErrorType res = JVX_NO_ERROR;
    IjvxReport* theReport = NULL;;
    NSString* myText;
    res = this->theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
    if(res == JVX_NO_ERROR)
    {
        if(theReport)
        {
            if(commandId == JVX_IOS_BWD_COMMAND_TEXT_MESSAGE)
            {
                myText = (__bridge NSString*)context;
                std::string txt = [myText UTF8String];
                jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&refTStamp);
                //txt = "<" + jvx_data2String((jvxData)tt/1000000.0, 6) + " sec>:" + txt;
                theReport->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
            }
            else
            {
                res = theReport->report_os_specific((jvxSize)commandId, context);
                if(res != JVX_NO_ERROR)
                {
                    theReport->report_simple_text_message("Failed to report event to host.", JVX_REPORT_PRIORITY_INFO);
                }
            }
            res = this->theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
        }
    }
}



jvxErrorType
jvxAudioProcessor_cpp::get_number_input_channels(jvxSize* num)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc get_number_input_channels:num];
        assert(res == JVX_NO_ERROR);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::get_description_input_channel(jvxSize idx, std::string& returnStr)
{
    jvxErrorType res = JVX_NO_ERROR;
    returnStr.clear();
    
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        NSString* str = NULL;
        res = [theProc get_description_input_channel:idx returnString:&str];
        
        assert(str);
        assert(res == JVX_NO_ERROR);
        
        returnStr = [str UTF8String];
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::get_number_output_channels(jvxSize* num)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc get_number_output_channels:num];
        assert(res == JVX_NO_ERROR);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::get_description_output_channel(jvxSize idx, std::string& returnStr)
{
    jvxErrorType res = JVX_NO_ERROR;
    returnStr.clear();
    
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        NSString* str = NULL;
        res = [theProc get_description_output_channel:idx returnString:&str];
        
        assert(str);
        assert(res == JVX_NO_ERROR);
        
        returnStr = [str UTF8String];
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
 
}

jvxErrorType
jvxAudioProcessor_cpp::get_number_samplerates(jvxSize* num)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc get_number_samplerates:num];
        assert(res == JVX_NO_ERROR);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::get_samplerate(jvxSize idx, jvxInt32& val)
{
    jvxErrorType res = JVX_NO_ERROR;
    
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        float valF = 0;
        res = [theProc get_samplerate:idx returnValue:&valF];
        
        assert(res == JVX_NO_ERROR);
        
        val = JVX_DATA2INT32(valF);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
    
}

jvxErrorType
jvxAudioProcessor_cpp::get_number_buffersizes(jvxSize* num)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc get_number_buffersizes:num];
        assert(res == JVX_NO_ERROR);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
    jvxAudioProcessor_cpp::get_buffersize(jvxSize idx, jvxSize& bsize)
{
    
    jvxErrorType res = JVX_NO_ERROR;
    
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        float valF = 0;
        res = [theProc get_buffersize:idx returnValue:&bsize];
        
        assert(res == JVX_NO_ERROR);
        
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
    
}


jvxErrorType
jvxAudioProcessor_cpp::get_number_dataformats(jvxSize* num)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc get_number_dataformats:num];
        assert(res == JVX_NO_ERROR);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
    jvxAudioProcessor_cpp::get_dataformat(jvxSize idx, jvxDataFormat& form)
{
    jvxErrorType res = JVX_NO_ERROR;
    
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        float valF = 0;
        res = [theProc get_dataformat:idx returnValue:&form];
        
        assert(res == JVX_NO_ERROR);
        
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
    
}

jvxErrorType
jvxAudioProcessor_cpp::get_number_input_sources(jvxSize* num)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState >= JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc get_number_input_sources:num];
        assert(res == JVX_NO_ERROR);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::get_input_source(jvxSize idx, std::string& nm, jvxBool* isActive)
{
    jvxErrorType res = JVX_NO_ERROR;
    NSString* nsstr = NULL;
    if(theState >= JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        float valF = 0;
        res = [theProc get_input_source:idx returnValue:&nsstr isSourceActive:isActive];
        assert(res == JVX_NO_ERROR);
        assert(nsstr);
        nm = [nsstr UTF8String];
        
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}


jvxErrorType
jvxAudioProcessor_cpp::get_number_output_sources(jvxSize* num)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState >= JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        
        res = [theProc get_number_output_sources:num];
        assert(res == JVX_NO_ERROR);
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::get_output_source(jvxSize idx, std::string& nm, jvxBool* isActive)
{
    jvxErrorType res = JVX_NO_ERROR;
    NSString* nsstr = NULL;
    if(theState >= JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        float valF = 0;
        res = [theProc get_output_source:idx returnValue:&nsstr isSourceActive:isActive];
        assert(res == JVX_NO_ERROR);
        assert(nsstr);
        nm = [nsstr UTF8String];
        
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::setInputOption(jvxSize idSet)
{
    jvxErrorType res = JVX_NO_ERROR;
    NSString* nsstr = NULL;
    NSError* err;
    BOOL resL;
    if(theState >= JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        res = [theProc activateInputOptionId:idSet];
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::setOutputOption(jvxSize idSet)
{
    jvxErrorType res = JVX_NO_ERROR;
    NSString* nsstr = NULL;
    NSError* err;
    BOOL resL;
    if(theState >= JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        res = [theProc activateOutputOptionId:idSet];
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::setSpeakerOption(jvxSize idSet)
{
    jvxErrorType res = JVX_NO_ERROR;
    NSString* nsstr = NULL;
    if(theState >= JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        if(idSet == 0)
        {
            res = [theProc activateSpeaker:true];
        }
        else
        {
            res = [theProc activateSpeaker:false];
        }
        
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::start(callbackProcessOneBuffer theCb, jvxHandle* priv, jvxLinkDataDescriptor* theData_in, 
    jvxLinkDataDescriptor* theData_out, jvxBitField inChannelSelection, jvxBitField outChannelSelection,
    jvxSize maxInChans, jvxSize maxOutChans)
//jvxSize idSRate, jvxSize idBsize, jvxSize idDataFormat, jvxSize numInChannels, jvxSize numOutChannels)
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_ACTIVE)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        //res = [theProc startAudio:theCb withVoidBwdRef:priv withSamplerateId:idSRate withBuffersizeId:idBsize withDataFormatId:idDataFormat withNumberInChannels:numInChannels withNumberOutChannels:numOutChannels];
        res = [theProc startAudio:theCb withVoidBwdRef:priv
            withLinkDataInDescriptorPtr:theData_in
            withLinkDataOutDescriptorPtr:theData_out
            withInputChannelSelector:inChannelSelection
            withOutputChannelsSelector:outChannelSelection
             withMaxInputChannels:maxInChans
            withMaxOutputChannels:maxOutChans];
        theState = JVX_STATE_PROCESSING;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxAudioProcessor_cpp::stop()
{
    jvxErrorType res = JVX_NO_ERROR;
    if(theState == JVX_STATE_PROCESSING)
    {
        jvxAudioProcessor* theProc = (__bridge jvxAudioProcessor*)reference_obj_c;
        res = [theProc stopAudio];
        theState = JVX_STATE_ACTIVE;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

