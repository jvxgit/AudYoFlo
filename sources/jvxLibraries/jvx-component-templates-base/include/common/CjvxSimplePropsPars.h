#ifndef __CJVXNODESPROPSPARS_H__
#define __CJVXNODESPROPSPARS_H__

#include "pcg_CjvxNodeAss_pcg.h"

/**
 * This class combines exposed properties and internal property values. In addition, it
 * associates the property containers directly to the actual values.
 */
class CjvxSimplePropsPars : public CjvxNodeAss_genpcg
{
public: 
	struct _common_set_node_params_a_1io_t
	{
		jvxInt32 buffersize = JVX_SIZE_UNSELECTED_INT32;
		jvxInt32 dimX = JVX_SIZE_UNSELECTED_INT32;
		jvxInt32 dimY = JVX_SIZE_UNSELECTED_INT32;
		jvxInt32 number_channels = JVX_SIZE_UNSELECTED_INT32;
		jvxInt32 samplerate = JVX_SIZE_UNSELECTED_INT32;
		jvxInt16 format = JVX_DATAFORMAT_NONE;
		jvxInt16 subformat = JVX_DATAFORMAT_GROUP_NONE;

		void reset()
		{
			buffersize = JVX_SIZE_UNSELECTED_INT32;
			dimX = JVX_SIZE_UNSELECTED_INT32;
			dimY = JVX_SIZE_UNSELECTED_INT32;
			number_channels = JVX_SIZE_UNSELECTED_INT32;
			samplerate = JVX_SIZE_UNSELECTED_INT32;
			format = JVX_DATAFORMAT_NONE;
			subformat = JVX_DATAFORMAT_GROUP_NONE;
		}
	};
	_common_set_node_params_a_1io_t _common_set_node_params_a_1io;

protected:

	CjvxProperties* assProps = nullptr;

public:
	void initialize(CjvxProperties* theProps, const std::string& prefix = "", jvxBool force_nonsys = false, jvxBool v_register = false)
	{
		assProps = theProps;
		CjvxNodeAss_genpcg::init_all();
		CjvxNodeAss_genpcg::allocate_all();
		CjvxNodeAss_genpcg::register_all(assProps, prefix, force_nonsys, v_register);
		CjvxNodeAss_genpcg::associate__node(assProps,
			&_common_set_node_params_a_1io.buffersize, 1,
			&_common_set_node_params_a_1io.samplerate, 1,
			&_common_set_node_params_a_1io.format, 1,
			&_common_set_node_params_a_1io.number_channels, 1,
			&_common_set_node_params_a_1io.dimX, 1,
			&_common_set_node_params_a_1io.dimY, 1,
			&_common_set_node_params_a_1io.subformat, 1);
	}

	void reset()
	{
		_common_set_node_params_a_1io.reset();
	}

	void terminate()
	{
		assert(assProps);
		CjvxNodeAss_genpcg::deassociate__node(assProps);
		CjvxNodeAss_genpcg::unregister_all(assProps);
		CjvxNodeAss_genpcg::deallocate_all();
		assProps = nullptr;
	}
};

#endif
