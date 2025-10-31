#include "jvx.h"
#include "jvxIosCppHost.h"
#include "jvxRtprocHost_configureAlgorithm.h"
#include "jvxAudioHost_common.h"

#include "jvxTconfigProcessor.h"
#include "jvxTDataConverter.h"
#include "jvxTDataLogger.h"
#include "jvxTResampler.h"
#include "jvxAuTGenericWrapper.h"
#include "jvxAuTIos.h"
#include "jvxTSimpleDotfuscation.h"
#include "jvxTrtAudioFileReader.h"
#include "jvxTrtAudioFileWriter.h"

jvxErrorType
jvxIosCppHost::bootup_specific()
{
    jvxSize i;
    jvxErrorType res = JVX_NO_ERROR;
    oneAddedStaticComponent comp;
    
    //=================================================================
    // Add the configuration lines here
    //=================================================================
    IjvxConfigurationLine* theConfigLines = NULL;
    
    res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLines));
    if((res == JVX_NO_ERROR) && theConfigLines)
    {
        theConfigLines->add_line(JVX_COMPONENT_AUDIO_DEVICE, "Use the audio parameters from the audio device in the audio node and the camera");
        theConfigLines->add_slave_line(0, JVX_COMPONENT_AUDIO_NODE);
        
        for(i = 0; i < JVX_NUM_CONFIGURATIONLINE_PROPERTIES_AUDIO; i++)
        {
            jvxSize id = 0;
            if(jvx_findPropertyDescriptor(jvxProperties_configLine_audio[i], &id, NULL, NULL, NULL))
            {
                theConfigLines->add_configuration_property_line(0, id);
            }
            
        }
        
        theSystemRefs.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle*>(theConfigLines));
    }
    
    //=================================================================
    // Add the types to be handled by host
    //=================================================================
    IjvxHostTypeHandler* theTypeHandler = NULL;
    res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle**>(&theTypeHandler));
    if((res == JVX_NO_ERROR) && theTypeHandler)
    {
        jvxComponentType tp[2];
        
        tp[0] = JVX_COMPONENT_AUDIO_TECHNOLOGY;
        tp[1] = JVX_COMPONENT_AUDIO_DEVICE;
        res = theTypeHandler->add_type_host(tp, 2, "Jvx Audio Technologies", "audio", JVX_COMPONENT_TYPE_TECHNOLOGY);
        assert(res == JVX_NO_ERROR);
        
        tp[0] = JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY;
        tp[1] = JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE;
        res = theTypeHandler->add_type_host(tp, 2, "Jvx External Controller Technologies", "ex_control", JVX_COMPONENT_TYPE_TECHNOLOGY);
        assert(res == JVX_NO_ERROR);
        
        tp[0] = JVX_COMPONENT_AUDIO_NODE;
        tp[1] = JVX_COMPONENT_UNKNOWN;
        res = theTypeHandler->add_type_host(tp, 1, "Jvx Audio Node", "audio_node", JVX_COMPONENT_TYPE_NODE);
        assert(res == JVX_NO_ERROR);
        
        theSystemRefs.hHost->return_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle*>(theTypeHandler));
    }
    
    //=================================================================
    // Open all static libraries and add to host
    // Do not add before since the accepted types must have been specified
    //=================================================================
    
    // Open all tool components
    LOAD_ONE_MODULE_LIB_FULL(jvxTconfigProcessor_init, jvxTconfigProcessor_terminate, "Config Parser",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTGlobalLock_init, jvxTGlobalLock_terminate, "GlobalLock",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
    LOAD_ONE_MODULE_LIB_FULL(jvxTDataConverter_init, jvxTDataConverter_terminate, "Data Converter",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
    LOAD_ONE_MODULE_LIB_FULL(jvxTDataLogger_init, jvxTDataLogger_terminate, "Data Logger",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
    LOAD_ONE_MODULE_LIB_FULL(jvxTResampler_init, jvxTResampler_terminate, "Resampler",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
    LOAD_ONE_MODULE_LIB_FULL(jvxTSimpleDotfuscation_init, jvxTSimpleDotfuscation_terminate, "Dotfuscator",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);

    LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileReader_init, jvxTrtAudioFileReader_terminate, "RT Audio Reader",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
    LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileWriter_init, jvxTrtAudioFileWriter_terminate, "RT Audio Writer",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
    LOAD_ONE_MODULE_LIB_FULL(jvxAuTIos_init, jvxAuTIos_terminate, "Ios Audio",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
    LOAD_ONE_MODULE_LIB_FULL(jvxAuTGenericWrapper_init, jvxAuTGenericWrapper_terminate, "Generic Wrapper Audio",
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
    LOAD_ONE_MODULE_LIB_FULL(JVX_ALGORITHM_INIT, JVX_ALGORITHM_TERMINATE,JVX_ALGORITHM_DESCRIPTION,
                             theSystemRefs.addedStaticObjects, theSystemRefs.hHost);
    
	//=================================================================
    // Do not allow that host components are loaded via DLL
    //=================================================================
    theSystemRefs.hHost->add_component_load_blacklist(JVX_COMPONENT_HOST);
    return(res);
}

// ======================================================================

jvxErrorType
jvxIosCppHost::shutdown_specific()
{
    jvxSize i;
    jvxErrorType res = JVX_NO_ERROR;
    
    // Do not allow that host components are loaded via DLL
    theSystemRefs.hHost->remove_component_load_blacklist(JVX_COMPONENT_HOST);
    
    // Remove all library objects
    for(i = 0; i < theSystemRefs.addedStaticObjects.size(); i++)
    {
        theSystemRefs.hHost->remove_external_component(theSystemRefs.addedStaticObjects[i].theStaticObject);
    }
    theSystemRefs.addedStaticObjects.clear();
    
    //=================================================================
    // Remove the types to be handled by host
    //=================================================================
    IjvxHostTypeHandler* theTypeHandler = NULL;
    res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle**>(&theTypeHandler));
    if((res == JVX_NO_ERROR) && theTypeHandler)
    {
        res = theTypeHandler->remove_all_host_types();
        assert(res == JVX_NO_ERROR);
        theSystemRefs.hHost->return_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle*>(theTypeHandler));
    }
    
    //=================================================================
    // Remove the configuration lines
    //=================================================================
    IjvxConfigurationLine* theConfigLines = NULL;
    
    res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLines));
    if((res == JVX_NO_ERROR) && theConfigLines)
    {
        res = theConfigLines->remove_all_lines();
        assert(res == JVX_NO_ERROR);
    }
    // This interface is handled separately.
    // this->involvedComponents.theHost.hHost->remove_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxQtWidgetHost*>(this)), JVX_INTERFACE_QT_WIDGET_HOST);
    return(JVX_NO_ERROR);
}

jvxErrorType
jvxIosCppHost::bootup_finalize_specific()
{
    jvxState stat = JVX_STATE_NONE;
    jvxErrorType res = JVX_NO_ERROR;
    IjvxObject* theObject = NULL;
    jvxString* fldStr = NULL;
    jvxSize num = 0;
    int foundComp = -1;
    jvxSize i;
    jvxBool requiresNewStart = true;
    
    theSystemRefs.hHost->state_component(JVX_COMPONENT_AUDIO_TECHNOLOGY, &stat);
    if(stat >= JVX_STATE_SELECTED)
    {
        theObject = NULL;
        res = theSystemRefs.hHost->request_object_component(JVX_COMPONENT_AUDIO_TECHNOLOGY, &theObject);
        assert(res == JVX_NO_ERROR);
        assert(theObject != NULL);
        
        fldStr = NULL;
        theObject->descriptor(&fldStr);
        assert(fldStr != NULL);
        
        if((std::string)fldStr->bString != "JVX_GENERIC_WRAPPER")
        {
            
            res = theSystemRefs.hHost->deactivate_component(JVX_COMPONENT_AUDIO_TECHNOLOGY);
            assert(res == JVX_NO_ERROR);
            
            res = theSystemRefs.hHost->unselect_component(JVX_COMPONENT_AUDIO_TECHNOLOGY);
            assert(res == JVX_NO_ERROR);
        }
        else
        {
            requiresNewStart = false;
        }
        theObject->deallocate(fldStr);
    }
    
    if(requiresNewStart)
    {
        res = theSystemRefs.hHost->number_components(JVX_COMPONENT_AUDIO_TECHNOLOGY, &num);
        foundComp = -1;
        
        for(i = 0; i< num; i++)
        {
            fldStr = NULL;
            theSystemRefs.hHost->descriptor_component(JVX_COMPONENT_AUDIO_TECHNOLOGY, i, &fldStr);
            if(fldStr->bString == (std::string)"JVX_GENERIC_WRAPPER")
            {
                foundComp = (int)i;
                break;
            }
        }
        
        if(foundComp < 0)
        {
            assert(0);
        }
        
        theSystemRefs.hHost->select_component(JVX_COMPONENT_AUDIO_TECHNOLOGY, foundComp);                
    }
    
    //=================================================================
    // Add a default sequencer sequence if desired
    //=================================================================
    addDefaultSequence(true);
    activateDefaultAlgorithm();
    
    if(bwd_ref.cb_entry)
    {
        bwd_ref.cb_entry(JVX_IOS_BWD_COMMAND_BOOTUP_COMPLETE, bwd_ref.cb_private, NULL);
    }
/*    if(theObjCRef)
    {
        jvxIosHost* theObjcHost = (__bridge jvxIosHost*)theObjCRef;
        [theObjcHost report_to_viewport_omt:JVX_IOS_BWD_COMMAND_BOOTUP_COMPLETE withVoidPointer:NULL];
    }*/
    return(res);
}

jvxErrorType
jvxIosCppHost::prepare_shutdown_specific()
{
    deactivateDefaultAlgorithm();
    removeAllSequencer();
    return(JVX_NO_ERROR);
}

void
jvxIosCppHost::addDefaultSequence(bool	onlyIfNoSequence)
{
    IjvxSequencer* sequencer = NULL;
    jvxErrorType res = JVX_NO_ERROR;
    jvxSize numSeq = 0;
    jvxSize numStepsRun = 0;
    jvxSize numStepsLeave = 0;
    jvxSize i,k;
    
    jvxSequencerElementType tpElm = JVX_SEQUENCER_TYPE_COMMAND_STD_PREPARE;
    jvxComponentType tpComp = JVX_COMPONENT_AUDIO_NODE;
    std::string descr = "description";
    std::string label_step;
    std::string label_cond1;
    std::string label_cond2;
    jvxInt32 idSeqActivate = -1;
    
    // Check that we are in the mode to allow adding a sequence
    // TODO
    
    // If so, go ahead
    res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
    if((res == JVX_NO_ERROR) && sequencer)
    {
        // First check if such a sequence is already there..
        sequencer->number_sequences(&numSeq);
        
        // jump out if there is already one sequence which may e.g. be taken from config file
        if(!
           ((numSeq > 0) && onlyIfNoSequence))
        {
            
            // Deactivate all sequences
            for(i = 0; i < numSeq; i++)
            {
                sequencer->mark_sequence_process(i, false);
            }
            
            // Check if there is a sequence to realize default sequencing
            for(i = 0; i < numSeq; i++)
            {
                sequencer->description_sequence(i, NULL, &numStepsRun, &numStepsLeave, NULL);
                if(
                   (numStepsRun == JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_RUN) &&
                   (numStepsLeave == JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_LEAVE))
                {
                    jvxBool runQueueMatches = true;
                    jvxBool leaveQueueMatches = true;
                    
                    for(k = 0; k < JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_RUN; k++)
                    {
                        sequencer->description_step_sequence(i, k, JVX_SEQUENCER_QUEUE_TYPE_RUN, NULL, &tpElm, &tpComp, NULL, NULL, NULL, NULL, NULL);
                        if( !
                           (defaultSteps_run[k].tpElm == tpElm)&&
                           (defaultSteps_run[k].tpComp == tpComp))
                        {
                            runQueueMatches = false;
                            break;
                        }
                    }
                    for(k = 0; k < JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_LEAVE; k++)
                    {
                        sequencer->description_step_sequence(i, k, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, NULL, &tpElm, &tpComp, NULL, NULL, NULL, NULL, NULL);
                        if( !
                           (defaultSteps_leave[k].tpElm == tpElm)&&
                           (defaultSteps_leave[k].tpComp == tpComp))
                        {
                            leaveQueueMatches = false;
                            break;
                        }
                    }
                    
                    if(runQueueMatches && leaveQueueMatches)
                    {
                        idSeqActivate = (jvxInt32)i;
                        break;
                    }
                }
            }
            
            if(idSeqActivate >= 0)
            {
                sequencer->mark_sequence_process(idSeqActivate, true);
            }
            else
            {
                // If no instance of the default sequence is present, add the appropriate default steps
                sequencer->add_sequence("Default Sequence");
                for(k= 0; k < JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_RUN; k++)
                {
                    sequencer->insert_step_sequence_at(numSeq, JVX_SEQUENCER_QUEUE_TYPE_RUN, defaultSteps_run[k].tpElm, defaultSteps_run[k].tpComp,
                                                       defaultSteps_run[k].descr.c_str(),(jvxInt32)defaultSteps_run[k].funcId, defaultSteps_run[k].timeout, defaultSteps_run[k].label_step.c_str(),
                                                       defaultSteps_run[k].label_cond1.c_str(), defaultSteps_run[k].label_cond2.c_str(), 
													   false, -1);
                }
                for(k= 0; k < JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_LEAVE; k++)
                {
                    sequencer->insert_step_sequence_at(numSeq, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, defaultSteps_leave[k].tpElm, defaultSteps_leave[k].tpComp,
                                                       defaultSteps_leave[k].descr.c_str(), (jvxInt32)defaultSteps_leave[k].funcId, defaultSteps_leave[k].timeout,  defaultSteps_leave[k].label_step.c_str(),
                                                       defaultSteps_leave[k].label_cond1.c_str(), defaultSteps_leave[k].label_cond2.c_str(), false, -1);
                }
                sequencer->mark_sequence_process(numSeq, true);
            }
        }
        theSystemRefs.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
    }
}

void
jvxIosCppHost::activateDefaultAlgorithm()
{
    jvxState stat = JVX_STATE_NONE;
    jvxSize num = 0;
    
    jvxErrorType res = theSystemRefs.hHost->state_component(JVX_COMPONENT_AUDIO_NODE, &stat);
    assert(res == JVX_NO_ERROR);
    
    switch(stat)
    {
        case JVX_STATE_NONE:
            theSystemRefs.hHost->number_components(JVX_COMPONENT_AUDIO_NODE, &num);
            assert(num == 1);
            
            res = theSystemRefs.hHost->select_component(JVX_COMPONENT_AUDIO_NODE, 0);
            assert(res == JVX_NO_ERROR);
        case JVX_STATE_SELECTED:
            theSystemRefs.hHost->activate_component(JVX_COMPONENT_AUDIO_NODE);
            assert(res == JVX_NO_ERROR);
            break;
        default:
            break;
    }
}

void
jvxIosCppHost::deactivateDefaultAlgorithm()
{
    jvxState stat = JVX_STATE_NONE;
    //jvxSize num = 0;
    
    jvxErrorType res = theSystemRefs.hHost->state_component(JVX_COMPONENT_AUDIO_NODE, &stat);
    assert(res == JVX_NO_ERROR);
    
    if(stat == JVX_STATE_ACTIVE)
    {
        res = theSystemRefs.hHost->deactivate_component(JVX_COMPONENT_AUDIO_NODE);
        assert(res == JVX_NO_ERROR);
    }
    
    if(stat > JVX_STATE_NONE)
    {
        theSystemRefs.hHost->unselect_component(JVX_COMPONENT_AUDIO_NODE);
    }
}

void
jvxIosCppHost::removeAllSequencer()
{
    IjvxSequencer* sequencer = NULL;
    jvxErrorType res = JVX_NO_ERROR;
    
    // If so, go ahead
    res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
    if((res == JVX_NO_ERROR) && sequencer)
    {
        // First check if such a sequence is already there..
        sequencer->reset_sequences();
        theSystemRefs.hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
    }
}

