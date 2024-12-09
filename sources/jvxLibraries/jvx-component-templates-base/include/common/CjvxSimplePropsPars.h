#ifndef __CJVXSIMPLEPROPSPARSNODE_H__
#define __CJVXSIMPLEPROPSPARSNODE_H__

#include "pcg_CjvxSimplePropsAss_pcg.h"

/**
 * This class combines exposed properties and internal property values. In addition, it
 * associates the property containers directly to the actual values.
 */
class CjvxSimplePropsPars : public CjvxSimplePropsAss_genpcg
{
public: 
	struct _common_set_node_params_a_1io_t
	{
		jvxInt32 buffersize = JVX_SIZE_UNSELECTED_INT32;
		jvxSize number_channels = JVX_SIZE_UNSELECTED_INT32;
		jvxInt32 samplerate = JVX_SIZE_UNSELECTED_INT32;
		jvxInt16 format = JVX_DATAFORMAT_NONE;
		jvxInt16 subformat = JVX_DATAFORMAT_GROUP_NONE;
		jvxInt16 data_flow = jvxDataflow::JVX_DATAFLOW_DONT_CARE;
		std::string format_spec;
		struct
		{
			jvxInt32 x = JVX_SIZE_UNSELECTED_INT32;
			jvxInt32 y = JVX_SIZE_UNSELECTED_INT32;
		} segmentation;

		void reset()
		{
			buffersize = JVX_SIZE_UNSELECTED_INT32;
			number_channels = JVX_SIZE_UNSELECTED;
			samplerate = JVX_SIZE_UNSELECTED_INT32;
			format = JVX_DATAFORMAT_NONE;
			subformat = JVX_DATAFORMAT_GROUP_NONE;
			data_flow = jvxDataflow::JVX_DATAFLOW_DONT_CARE;
			segmentation.x = JVX_SIZE_UNSELECTED_INT32;
			segmentation.y = JVX_SIZE_UNSELECTED_INT32;
			format_spec.clear();
		};

		void derive_buffersize()
		{
			jvxSize szElement = jvxDataFormatGroup_getsize_mult(subformat);
			jvxSize szElementsLine = segmentation.x * szElement;
			jvxSize szElementsField = segmentation.y * szElementsLine;
			buffersize = szElementsField / jvxDataFormatGroup_getsize_div(subformat);
		};
	};
	_common_set_node_params_a_1io_t _common_set_node_params_a_1io;

protected:

	CjvxProperties* assProps = nullptr;

public:
	void initialize(CjvxProperties* theProps, const std::string& prefix = "", jvxBool force_nonsys = false, jvxBool v_register = false)
	{
		assProps = theProps;
		CjvxSimplePropsAss_genpcg::init_all();
		CjvxSimplePropsAss_genpcg::allocate_all();
		CjvxSimplePropsAss_genpcg::register_all(assProps, prefix, force_nonsys, v_register);
		CjvxSimplePropsAss_genpcg::associate__node(assProps,
			&_common_set_node_params_a_1io.buffersize, 1,
			&_common_set_node_params_a_1io.samplerate, 1,
			&_common_set_node_params_a_1io.format, 1,
			&_common_set_node_params_a_1io.number_channels, 1,
			&_common_set_node_params_a_1io.segmentation.x, 1,
			&_common_set_node_params_a_1io.segmentation.y, 1,
			&_common_set_node_params_a_1io.subformat, 1,
			&_common_set_node_params_a_1io.data_flow, 1,
			&_common_set_node_params_a_1io.format_spec);
	}

	void reset()
	{
		_common_set_node_params_a_1io.reset();
	}

	void terminate()
	{
		assert(assProps);
		CjvxSimplePropsAss_genpcg::deassociate__node(assProps);
		CjvxSimplePropsAss_genpcg::unregister_all(assProps);
		CjvxSimplePropsAss_genpcg::deallocate_all();
		assProps = nullptr;
	}

	void derive_buffersize()
	{
		// Buffersize is in elements of tyoe format!!
		jvxSize szElement = jvxDataFormatGroup_getsize_mult(_common_set_node_params_a_1io.subformat);
		jvxSize szLine = _common_set_node_params_a_1io.segmentation.x * szElement;
		jvxSize szRaw = _common_set_node_params_a_1io.segmentation.y * szLine;
		szRaw = szRaw / jvxDataFormatGroup_getsize_div(_common_set_node_params_a_1io.subformat);
		_common_set_node_params_a_1io.buffersize = szRaw;

	}
	void prepare_negotiate(CjvxNegotiate_common& neg)
	{
		neg.negBehavior = CjvxNegotiate_output::negBehaviorType::JVX_BEHAVIOR_VIDEO;
		neg._set_parameters_fixed(
			_common_set_node_params_a_1io.number_channels,
			_common_set_node_params_a_1io.buffersize, 
			_common_set_node_params_a_1io.samplerate,
			(jvxDataFormat)_common_set_node_params_a_1io.format,
			(jvxDataFormatGroup)_common_set_node_params_a_1io.subformat,
			(jvxDataflow)_common_set_node_params_a_1io.data_flow,
			_common_set_node_params_a_1io.segmentation.x,
			_common_set_node_params_a_1io.segmentation.y);
	}

	jvxBool check_difference(_common_set_node_params_a_1io_t* params)
	{
		jvxBool difference_found = (_common_set_node_params_a_1io.number_channels != params->number_channels);
		difference_found = difference_found || (_common_set_node_params_a_1io.buffersize != params->buffersize);
		difference_found = difference_found || (_common_set_node_params_a_1io.samplerate != params->samplerate);
		difference_found = difference_found || (_common_set_node_params_a_1io.format != params->format);
		difference_found = difference_found || (_common_set_node_params_a_1io.subformat != params->subformat);
		difference_found = difference_found || (_common_set_node_params_a_1io.data_flow != params->data_flow);
		difference_found = difference_found || (_common_set_node_params_a_1io.segmentation.x != params->segmentation.x);
		difference_found = difference_found || (_common_set_node_params_a_1io.segmentation.y != params->segmentation.y);
		return difference_found;
	}
};

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
