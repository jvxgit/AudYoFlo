#ifndef __CJVXSIMPLEPROPSPARSPLUSOUTCHANNEL_H__
#define __CJVXSIMPLEPROPSPARSPLUSOUTCHANNEL_H__

#include "common/CjvxNegotiate.h"
#include "CjvxSimplePropsPars.h"
#include "pcg_CjvxChannelPlusAss_pcg.h"

/**
 * This class combines exposed properties and internal property values. In addition, it
 * associates the property containers directly to the actual values.
 */
class CjvxSimplePropsParsPlusOutChannel : public CjvxSimplePropsPars, public CjvxChannelPlusAss_genpcg
{
public: 
	struct _common_set_device_params_t
	{
		jvxInt32 number_channels_out = JVX_SIZE_UNSELECTED_INT32;
		void reset()
		{
			number_channels_out = JVX_SIZE_UNSELECTED_INT32;
		}
	};
	_common_set_device_params_t _common_set_device_params;

protected:

public:
	void initialize(CjvxProperties* theProps, const std::string& prefix = "", jvxBool force_nonsys = false, jvxBool v_register = false)
	{
		CjvxSimplePropsPars::initialize(theProps,  prefix, force_nonsys, v_register);
		
		CjvxChannelPlusAss_genpcg::init_all();
		CjvxChannelPlusAss_genpcg::allocate_all();
		CjvxChannelPlusAss_genpcg::register_all(assProps, prefix, force_nonsys, v_register);
		CjvxChannelPlusAss_genpcg::associate__node_plus(assProps, &_common_set_device_params.number_channels_out, 1);
	}

	void reset()
	{
		// Involves base class
		_common_set_node_params_a_1io.reset();
		_common_set_device_params.reset();
	}

	void terminate()
	{
		CjvxChannelPlusAss_genpcg::deassociate__node_plus(assProps);
		CjvxChannelPlusAss_genpcg::unregister_all(assProps);
		CjvxChannelPlusAss_genpcg::deallocate_all();
		CjvxSimplePropsPars::terminate();
	}
	
	void prepare_negotiate(CjvxNegotiate_common& neg)
	{
		neg.negBehavior = CjvxNegotiate_output::negBehaviorType::JVX_BEHAVIOR_VIDEO;
		neg._set_parameters_fixed(
			_common_set_device_params.number_channels_out,
			_common_set_node_params_a_1io.buffersize, 
			_common_set_node_params_a_1io.samplerate,
			(jvxDataFormat)_common_set_node_params_a_1io.format,
			(jvxDataFormatGroup)_common_set_node_params_a_1io.subformat,
			(jvxDataflow)_common_set_node_params_a_1io.data_flow,
			_common_set_node_params_a_1io.segmentation.x,
			_common_set_node_params_a_1io.segmentation.y);
	}

	jvxBool check_difference(_common_set_node_params_a_1io_t* params, _common_set_device_params_t* params_chan)
	{
		jvxBool difference_found = CjvxSimplePropsPars::check_difference(params);
		difference_found = (difference_found || (params_chan->number_channels_out != _common_set_device_params.number_channels_out));
		return difference_found;
	}
};

#if 0
// Some helper macros
#define JVX_LINKDATA_LOAD_FROM(to, from, flow) \
	(to)->con_params.buffersize = from.buffersize; \
	(to)->con_params.rate = from.samplerate; \
	(to)->con_params.data_flow = flow; \
	(to)->con_params.number_channels = from.number_channels; \
	(to)->con_params.format = (jvxDataFormat)from.format; \
	(to)->con_params.format_group = (jvxDataFormatGroup)from.subformat; \
	(to)->con_params.segmentation.x = from.segmentation.x; \
	(to)->con_params.segmentation.y = from.segmentation.y; 

#define JVX_LINKDATA_LOAD_TO(to, from) \
	to.buffersize = (from)->con_params.buffersize; \
	to.samplerate = (from)->con_params.rate; \
	to.number_channels = (from)->con_params.number_channels; \
	to.format = (from)->con_params.format; \
	to.subformat = (from)->con_params.format_group; \
	to.segmentation.x = (from)->con_params.segmentation.x; \
	to.segmentation.y = (from)->con_params.segmentation.y; 
#endif
#endif
