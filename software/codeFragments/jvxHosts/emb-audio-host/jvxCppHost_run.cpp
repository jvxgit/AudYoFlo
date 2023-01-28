//
//  jvxCppHost_run.cpp
//  jvxRTProcHost
//
//  Created by Hauke Krüger on 12.01.16.
//  Copyright © 2016 Javox Solutions. All rights reserved.
//
#include "jvx.h"
#include "jvxIosCppHost.h"

jvxErrorType
jvxIosCppHost::start_run()
{
    jvxErrorType res;
    if(stateRun.running == false)
    {
        stateRun.theSeq = NULL;
        res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&stateRun.theSeq));
        if(res == JVX_NO_ERROR)
        {
            assert(res == JVX_NO_ERROR);
            assert(stateRun.theSeq);
            stateRun.running = true;
            
            res = stateRun.theSeq->run_process(200, this, -1, true); // Timeout etc. not used in operation mode
            assert(res == JVX_NO_ERROR);
            
            res = stateRun.theSeq->trigger_step_process_extern(0);
            stateRun.progress = 0;
        }
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::stop_run()
{
    jvxErrorType res;
    if(stateRun.running == true)
    {
        assert(stateRun.theSeq);
        res = stateRun.theSeq->trigger_abort_process();
        assert(res == JVX_NO_ERROR);
        while(res == JVX_NO_ERROR)
        {
            res = stateRun.theSeq->trigger_step_process_extern(0);
        }
        res = this->stateRun.theSeq->wait_completion_process(JVX_INFINITE);
        assert(res == JVX_NO_ERROR);
        stateRun.running = false;
    }
    else
    {
        res = JVX_ERROR_WRONG_STATE;
    }
    return(res);
}

jvxErrorType
jvxIosCppHost::state_run(jvxBool* isRunning)
{
    if(isRunning)
    {
        *isRunning = stateRun.running;
    }
    return(JVX_NO_ERROR);
}

jvxErrorType
jvxIosCppHost::report_event(jvxUInt64 event_mask, const char* description, jvxInt16 sequenceId, jvxInt16 stepId, jvxSequencerQueueType tp, jvxSequencerElementType stp)
{
    std::string txt;
    if(event_mask & JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE)
    {
        txt += "<STARTUP>";
        
        this->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
    }
    
    if(event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP)
    {
        txt += "<STEP> seq#" + jvx_int2String(sequenceId) + ", step#" + jvx_int2String(stepId) + ", ";
        switch(tp)
        {
            case JVX_SEQUENCER_QUEUE_TYPE_RUN:
                txt += "run";
                break;
            case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
                txt += "leave";
                break;
            default:
                txt += "none";
                break;
        }
        
        txt += ": ";
        txt += description;
        this->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
        
        if(stp == JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER)
        {
            this->report_command_request(JVX_REPORT_REQUEST_UPDATE_WINDOW);
        }
    }
    if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR)
    {
        txt += "<STEP FAIL> seq#" + jvx_int2String(sequenceId) + ", step#" + jvx_int2String(stepId) + ", ";
        switch(tp)
        {
            case JVX_SEQUENCER_QUEUE_TYPE_RUN:
                txt += "run";
                break;
            case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
                txt += "leave";
                break;
            default:
                txt += "none";
                break;
        }
        txt += ": ";
        txt += description;
        this->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_ERROR);
    }
    
    if(event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE)
    {
        txt += "<SEQ> seq#" + jvx_int2String(sequenceId) + ": ";
        txt += description;
        report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
    }
    if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR)
    {
        txt += "<SEQ FAIL> seq#" + jvx_int2String(sequenceId) +": ";
        txt += description;
        report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
    }
    
    if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT)
    {
        txt += "<SEQ ABORT> seq#"  + jvx_int2String(sequenceId) + ": ";
        txt += description;
        report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_INFO);
    }
    
    if(event_mask & JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS)
    {
        txt += "<PROC SUC>";
        report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_SUCCESS);
    }
    if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR)
    {
        txt += "<PROC FAIL>: ";
        txt += description;
        report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_ERROR);
    }
    if(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT)
    {
        txt += "<PROC ABORT>";
        report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
    }
    if(event_mask & JVX_SEQUENCER_EVENT_PROCESS_TERMINATED)
    {
        txt += "<PROC TERM>";
        report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
        this->report_command_request(JVX_REPORT_REQUEST_UPDATE_WINDOW);
    }
    return(JVX_NO_ERROR);
}

jvxErrorType
jvxIosCppHost::sequencer_callback(jvxInt16 sequenceId, jvxInt16 stepId, jvxSequencerQueueType tp, jvxInt32 functionId)
{
    std::string txt;
    txt += "<SEQ CB>: id=" + jvx_int2String(functionId);
    report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
    this->report_command_request(JVX_REPORT_REQUEST_UPDATE_WINDOW);
    return(JVX_NO_ERROR);
}
