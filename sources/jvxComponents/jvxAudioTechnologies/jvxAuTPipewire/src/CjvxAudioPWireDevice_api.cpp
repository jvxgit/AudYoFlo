#include "CjvxAudioPWireDevice.h"
#include "CjvxAudioPWireTechnology.h"

void 
CjvxAudioPWireDevice::set_references_api(oneDevice* theDevicehandleArg, CjvxAudioPWireTechnology* parentArg)
{
    assert(theDevicehandle == nullptr);
    theDevicehandle = theDevicehandleArg;
    if(theDevicehandle)
    {
        switch(theDevicehandle->opMode)
        {
        case operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_INPUT:
            _common_set_device.caps.capsFlags = ((jvxCBitField)1 << (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_INPUT_SHIFT);
        break;
        case operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_OUTPUT:
            _common_set_device.caps.capsFlags = ((jvxCBitField)1 << (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_OUTPUT_SHIFT);
        break;
        case operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_DUPLEX:
             _common_set_device.caps.capsFlags = ((jvxCBitField)1 << (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT);
        break;
        }        
    }
    else
    {
        assert(this->_common_set_device.caps.proxy);
        _common_set_device.caps.capsFlags = ((jvxCBitField)1 << (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_PROXY_SHIFT);
    }
    parent = parentArg;
}

oneDevice* 
CjvxAudioPWireDevice::references_api()
{
    oneDevice* retVal = theDevicehandle;
    theDevicehandle = nullptr;
	return retVal;
}

jvxErrorType 
CjvxAudioPWireDevice::activate_device_api()
{
    jvxErrorType res = JVX_NO_ERROR;
  
    jvxSize samplerate = parent->system.samplerate_force;
    if(samplerate == 0)
    {
        samplerate = parent->system.samplerate;
    }

    jvxSize buffersize = parent->system.buffersize_force;
    if(buffersize == 0)
    {
        buffersize = parent->system.buffersize;
    }

    inout_params._common_set_node_params_a_1io.samplerate = samplerate;
   inout_params._common_set_node_params_a_1io.data_flow = JVX_DATAFLOW_PUSH_ACTIVE;
   inout_params._common_set_node_params_a_1io.segmentation.x = buffersize;
   inout_params._common_set_node_params_a_1io.segmentation.y = 1;
   inout_params._common_set_node_params_a_1io.subformat = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;

   inout_params._common_set_node_params_a_1io.derive_buffersize();

   if (
       (theDevicehandle->opMode == operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_INPUT) ||
       (theDevicehandle->opMode == operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_DUPLEX))
   {
       CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.clear();

       auto elmSS = theDevicehandle->sources.begin();
       oneNode *theNode = *elmSS;
       jvxSize cnt = 0;
       jvxBool foundOne = true;

       // This is the CAPTURING side!!!
       jvx_bitFClear(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection());
       while (foundOne)
       {
           foundOne = false;
           for (auto elmP : theNode->out_ports)
           {
               onePort *thePort = elmP;
               if (thePort->physical == "true")
               {
                   if (thePort->port_id == cnt)
                   {
                       std::string chan_name = thePort->nick;
                       if (chan_name.empty())
                       {
                           chan_name = thePort->name;
                       }
                       CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.entries.push_back(chan_name);
                       jvx_bitSet(CjvxAudioMasterDevice_genpcg::properties_active.inputchannelselection.value.selection(), cnt);
                       foundOne = true;
                       cnt++;
                       break;
                   }
               }
           }
       }      
   }

   if (
       (theDevicehandle->opMode == operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_OUTPUT) ||
       (theDevicehandle->opMode == operationModeDevice::AYF_PIPEWIRE_OPERATION_DEVICE_DUPLEX))
   {   
       // This is the RENDERING side!!!
       auto elmSS = theDevicehandle->sinks.begin();
       oneNode* theNode = *elmSS;
       jvxSize cnt = 0;
       jvxBool foundOne = true;
       jvx_bitFClear(CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.selection());
       while (foundOne)
       {
           foundOne = false;
           for (auto elmP : theNode->in_ports)
           {
               onePort *thePort = elmP;
               if (thePort->physical == "true")
               {
                   if (thePort->port_id == cnt)
                   {
                       std::string chan_name = thePort->nick;
                       if (chan_name.empty())
                       {
                           chan_name = thePort->name;
                       }
                       CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.entries.push_back(chan_name);
                       jvx_bitSet(CjvxAudioMasterDevice_genpcg::properties_active.outputchannelselection.value.selection(), cnt);
                       foundOne = true;
                       cnt++;
                       break;
                   }
               }
           }
       }
   }

   // Update denpendent variables but do not request any chain update - we are in activate!!
   this->updateDependentVariables(true);
   return res;
}

jvxErrorType
CjvxAudioPWireDevice::deactivate_device_api()
{
    jvxErrorType res = JVX_NO_ERROR;

    // All clear operations are realized in the base class <CjvxAudioMasterDevice>
    return res;
}

jvxErrorType
CjvxAudioPWireDevice::try_match_settings_backward_ocon(jvxLinkDataDescriptor *ld_con JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
    JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject *>(this),
                                                   _common_set_io_common.descriptor.c_str(), "Entering CjvxAudioPWireDevice default output connector");

    std::list<std::string> modProps;
    if (
        (ld_con->con_params.segmentation.x != inout_params._common_set_node_params_a_1io.segmentation.x) ||
        (ld_con->con_params.segmentation.y != inout_params._common_set_node_params_a_1io.segmentation.y))
    {
        inout_params._common_set_node_params_a_1io.segmentation.x = ld_con->con_params.segmentation.x;
        inout_params._common_set_node_params_a_1io.segmentation.y = ld_con->con_params.segmentation.y;
        inout_params.derive_buffersize();

        neg_output._update_parameter_fixed(jvxAddressLinkDataEntry::JVX_ADDRESS_SEGX_SHIFT,
                                           inout_params._common_set_node_params_a_1io.segmentation.x,
                                           &_common_set_ocon.theData_out);
        neg_output._update_parameter_fixed(jvxAddressLinkDataEntry::JVX_ADDRESS_SEGY_SHIFT,
                                           inout_params._common_set_node_params_a_1io.segmentation.y,
                                           &_common_set_ocon.theData_out);
        neg_output._update_parameter_fixed(jvxAddressLinkDataEntry::JVX_ADDRESS_BUFFERSIZE_SHIFT,
                                           inout_params._common_set_node_params_a_1io.buffersize,
                                           &_common_set_ocon.theData_out);

        modProps.push_back(inout_params.node.buffersize.descriptor.c_str());
        modProps.push_back(inout_params.node.segmentsize_x.descriptor.c_str());
        modProps.push_back(inout_params.node.segmentsize_y.descriptor.c_str());
    }

    if (ld_con->con_params.rate != inout_params._common_set_node_params_a_1io.samplerate)
    {
        inout_params._common_set_node_params_a_1io.samplerate = ld_con->con_params.rate;
        modProps.push_back(inout_params.node.samplerate.descriptor.c_str());
        neg_output._update_parameter_fixed(jvxAddressLinkDataEntry::JVX_ADDRESS_SAMPLERATE_SHIFT,
                                           inout_params._common_set_node_params_a_1io.samplerate,
                                           &_common_set_ocon.theData_out);
    }

    return CjvxMixDevicesAudioDevice<CjvxAudioPWireDevice>::try_match_settings_backward_ocon(ld_con JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

