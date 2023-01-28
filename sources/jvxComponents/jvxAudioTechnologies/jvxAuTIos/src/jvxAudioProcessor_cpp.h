#ifndef __JVXAUDIOPROCESSORCPP_H__
#define __JVXAUDIOPROCESSORCPP_H__

#include "jvxAudioProcessor_env.h"

class jvxAudioProcessor_cpp
{
private:
    void* reference_obj_c;
    jvxState theState;
    IjvxHiddenInterface* theHostRef;
    jvxTimeStampData refTStamp;
    
public:
    
    jvxAudioProcessor_cpp();
    ~jvxAudioProcessor_cpp();
    
    jvxErrorType state(jvxState* stateOnReturn);
    
    // -- Technology specific stuff --
    
    jvxErrorType initializeTechnology(IjvxHiddenInterface* hostRef);
    jvxErrorType terminateTechnology();
    
    jvxErrorType get_number_input_ports(jvxSize* num);
    
    jvxErrorType get_description_input_port(jvxSize idx, std::string& DescrOnReturn);
    
    // -- Device specific stuff --
    
    jvxErrorType initializeDevice(jvxSize portId);
    jvxErrorType terminateDevice();
    
    void report_os_specific(jvxSize commandId, void *context);
    
    jvxErrorType get_number_input_channels(jvxSize* num);
    jvxErrorType get_description_input_channel(jvxSize idx, std::string& returnStr);
    
    jvxErrorType get_number_output_channels(jvxSize* num);
    jvxErrorType get_description_output_channel(jvxSize idx, std::string& returnStr);
    
    jvxErrorType get_number_samplerates(jvxSize* num);
    jvxErrorType get_samplerate(jvxSize idx, jvxInt32& val);
    
    jvxErrorType get_number_buffersizes(jvxSize* num);
    jvxErrorType get_buffersize(jvxSize idx, jvxSize& bsize);
    
    jvxErrorType get_number_dataformats(jvxSize* num);
    jvxErrorType get_dataformat(jvxSize idx, jvxDataFormat& form);
    
    jvxErrorType get_number_input_sources(jvxSize* num);
    jvxErrorType get_input_source(jvxSize idx, std::string& nm, jvxBool* isActive);
    
    jvxErrorType get_number_output_sources(jvxSize* num);
    jvxErrorType get_output_source(jvxSize idx, std::string& nm, jvxBool* isActive);
    
    jvxErrorType setInputOption(jvxSize idSet);
    jvxErrorType setOutputOption(jvxSize idSet);
    jvxErrorType setSpeakerOption(jvxSize idSet);
    
    jvxErrorType start(callbackProcessOneBuffer, jvxHandle* priv, 
        jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
        jvxBitField inChannelSelection, jvxBitField outChannelSelection,
        jvxSize maxInChans, jvxSize maxOutChans);
    //jvxSize idSRate, jvxSize idBsize, jvxSize idDataFormat, jvxSize numInChannels, jvxSize numOutChannels);
    jvxErrorType stop();
    
};

#endif
