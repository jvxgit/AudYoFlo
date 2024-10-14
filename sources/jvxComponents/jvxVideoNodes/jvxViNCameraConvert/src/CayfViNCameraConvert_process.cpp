#include "CayfViNCameraConvert.h"
#include "HjvxDataLinkDescriptor.h"

#include <opencv2/imgproc.hpp>

jvxErrorType
CayfViNCameraConvert::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// This module decides itself if it is operated in zero copy or not!
	// If not, allocate the required number of buffers
	_common_set_ldslave.num_min_buffers_out = _common_set_icon.theData_in->con_data.number_buffers;

	if (jvx_check_in_out_params_match_test(_common_set_icon.theData_in, &_common_set_ocon.theData_out, JVX_CHECK_PARAM_ALL))
	{
		_common_set_ldslave.zeroCopyBuffering_cfg = true;
	}
	else
	{
		runtime.convertFormat = false;
		runtime.convertSize = false;

		if(_common_set_icon.theData_in->con_params.format_group != _common_set_ocon.theData_out.con_params.format_group)
		{ 
			runtime.convertFormat = true;
		}

		if (
			(_common_set_icon.theData_in->con_params.segmentation.x != _common_set_ocon.theData_out.con_params.segmentation.x) ||
			(_common_set_icon.theData_in->con_params.segmentation.y != _common_set_ocon.theData_out.con_params.segmentation.y))
		{
			runtime.convertSize = true;
		}

		if (runtime.convertFormat)
		{
			runtime.convert.from = _common_set_icon.theData_in->con_params.format_group;
			runtime.convert.to = _common_set_ocon.theData_out.con_params.format_group;
			if (runtime.convertSize)
			{
				jvxLinkDataDescriptor_con_params pars = _common_set_icon.theData_in->con_params;
				pars.format_group = runtime.convert.to;
				runtime.convert.szTmp = jvx_derive_buffersize(pars);
			}
		}

		switch (runtime.convert.to)
		{
		case JVX_DATAFORMAT_GROUP_VIDEO_RGBA32:
		{
			jvxSize szField = jvx_derive_buffersize(_common_set_ocon.theData_out.con_params);
			assert(szField == _common_set_ocon.theData_out.con_params.buffersize);
			runtime.convert.szFldOut = szField * jvxDataFormat_getsize(_common_set_icon.theData_in->con_params.format);

			// Currently, only output to RGBA
			switch (runtime.convert.from)
			{
			case JVX_DATAFORMAT_GROUP_VIDEO_NV12:
			{
				// Compute sizes proper conversion buffers!!
				szField = jvx_derive_buffersize(_common_set_icon.theData_in->con_params);
				assert(szField == _common_set_icon.theData_in->con_params.buffersize);
				runtime.convert.szFldIn = _common_set_icon.theData_in->con_params.buffersize * jvxDataFormat_getsize(_common_set_icon.theData_in->con_params.format);

				runtime.convert.width = _common_set_icon.theData_in->con_params.segmentation.x;
				runtime.convert.height = _common_set_icon.theData_in->con_params.segmentation.y;
				runtime.convert.NV12.plane1_Sz = _common_set_icon.theData_in->con_params.segmentation.x * _common_set_icon.theData_in->con_params.segmentation.y;
				runtime.convert.NV12.plane2_Sz = szField - runtime.convert.NV12.plane1_Sz;
				break;
			}
			case JVX_DATAFORMAT_GROUP_VIDEO_RGB24:

				szField = jvx_derive_buffersize(_common_set_icon.theData_in->con_params);
				assert(szField == _common_set_icon.theData_in->con_params.buffersize);
				runtime.convert.szFldIn = _common_set_icon.theData_in->con_params.buffersize * jvxDataFormat_getsize(_common_set_icon.theData_in->con_params.format);
				runtime.convert.width = _common_set_icon.theData_in->con_params.segmentation.x;
				runtime.convert.height = _common_set_icon.theData_in->con_params.segmentation.y; 
				break;

			case JVX_DATAFORMAT_GROUP_VIDEO_YUYV:
			{
				// Compute sizes proper conversion buffers!!
				szField = jvx_derive_buffersize(_common_set_icon.theData_in->con_params);
				assert(szField == _common_set_icon.theData_in->con_params.buffersize);
				runtime.convert.szFldIn = _common_set_icon.theData_in->con_params.buffersize * jvxDataFormat_getsize(_common_set_icon.theData_in->con_params.format);

				runtime.convert.width = _common_set_icon.theData_in->con_params.segmentation.x;
				runtime.convert.height = _common_set_icon.theData_in->con_params.segmentation.y;
				break;
			}
			default:
				assert(0);
			}
			break;
		}
		default:
			assert(0);
		}
	}
	return CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CayfViNCameraConvert::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	if (zeroCopyBuffering_rt)
	{
		return fwd_process_buffers_icon(mt_mask, idx_stage);
	}

	jvxByte** bufsIn = jvx_process_icon_extract_input_buffers<jvxByte>(_common_set_icon.theData_in, idx_stage);
	jvxByte** bufsOut = jvx_process_icon_extract_output_buffers<jvxByte>(_common_set_ocon.theData_out);

	if (runtime.convertFormat)
	{
		switch (runtime.convert.to)
		{
		case JVX_DATAFORMAT_GROUP_VIDEO_RGBA32:

			// Currently, only output to RGBA
			switch (runtime.convert.from)
			{
			case JVX_DATAFORMAT_GROUP_VIDEO_NV12:
			{

				// If we involve an NV12 mapping, we need to use openCV
				// In openCV we need to involve the cv data structs. I have just 
				// been rather robust in involving the Mat structs with raw memory without really taking into account
				// the openCVmatrix memory model with channels. However, it works!!

				// This pointer points to a field of bytes that contains Y in the first part and UV in the second part
				// We will need to make 2 planes out of that: 
				// - First plane, 1 channel: Y - size if h x w
				// - Second plane: 2 channels: U/V - size os h/2 x w/2
				jvxUInt8* ptrRead = (jvxUInt8*)bufsIn[0]; // channel 0
				auto planeY = cv::Mat(
					runtime.convert.height, runtime.convert.width,
					CV_8UC1, ptrRead);
				auto planeUV = cv::Mat(runtime.convert.height / 2,
					runtime.convert.width / 2, CV_8UC2,
					ptrRead + runtime.convert.NV12.plane1_Sz);

				// The output is a bufer containing h x w RGB entries. The RGB is handled in a three channel model.
				// WHAT IS NOT GOOD: The output buffer is always created by the cv library. There seems to be no 
				// option to provide memory space to directly render here. Therefore, we need to copy around the data :-(
				cv::Mat out;

				// Actually run the converter
				cv::cvtColorTwoPlane(planeY, planeUV, out, cv::COLOR_YUV2RGBA_NV12);

				// This is suboptimal: we need to copy the allocated memory. Hopefully, cv uses pre-allocated memory internally for higher efficiency!!
				memcpy(bufsOut[0], out.data, runtime.convert.szFldOut);

				break;
			}
			case JVX_DATAFORMAT_GROUP_VIDEO_RGB24:
			{
				jvxUInt8* ptrRead = (jvxUInt8*)bufsIn[0]; // channel 0
				auto planeRGB = cv::Mat(
					runtime.convert.height, runtime.convert.width,
					CV_8UC3, ptrRead);
				cv::Mat out;

				// Actually run the converter
				cv::cvtColor(planeRGB, out, cv::COLOR_RGB2RGBA);
				memcpy(bufsOut[0], out.data, runtime.convert.szFldOut);
#if 0
				// Future: use COLOR_RGB2RGBA
				for (i = 0; i < _common_set_ocon.theData_out.con_params.segmentation.y; i++)
				{
					jvxSize j;
					jvxUInt8* ptrFrom = src;
					jvxUInt8* ptrTo = dest;

					if (stride > 0)
					{
						for (j = 0; j < runtime.params_sw.szElementsLine; j += 4)
						{
							// This for RGB in RGBA
							for (jvxSize k = 0; k < 3; k++)
							{
								*ptrTo = *ptrFrom;
								ptrFrom++;
								ptrTo++;
							}

							// This for "A" in RGBA
							*ptrTo = 255;
							ptrTo++;
						}
					}
					else
					{
						// To last element
						ptrFrom -= stride;
						j = 0;

						while (ptrFrom > src)
						{

							// Backward copy		

							// This for RGB in RGBA
							for (jvxSize k = 0; k < 3; k++)
							{
								--ptrFrom;
								*ptrTo = *ptrFrom;
								ptrTo++;
							}
							// This for "A" in RGBA
							*ptrTo = 255;
							ptrTo++;
							j += 4;
			}
						assert(j == runtime.params_sw.szElementsLine);
		}
					src += stride;
					dest += runtime.params_sw.szElementsLine;
	};
#endif
				break;
			}
			case JVX_DATAFORMAT_GROUP_VIDEO_YUYV:
			{

				// If we involve an YUYV mapping, we need to use openCV

				// Each pixel is represented by two bytes, where one byte is a Y value, and the second byte contains U and V.
				// So, every two pixels share a U and V value.
				jvxUInt8* ptrRead = (jvxUInt8*)bufsIn[0]; // channel 0
				auto image = cv::Mat(
					runtime.convert.height, runtime.convert.width,
					CV_8UC2, ptrRead);

				// The output is a bufer containing h x w RGB entries. The RGB is handled in a three channel model.
				cv::Mat output = cv::Mat(
					runtime.convert.height, runtime.convert.width,
					CV_8UC4, bufsOut[0]);

				cv::cvtColor(image, output, cv::COLOR_YUV2RGBA_YUYV);

				break;
			}
			default:
				assert(0);
				break;
			}
			break;
		default:
			assert(0);
			break;
		}
	}
	return  fwd_process_buffers_icon(mt_mask, idx_stage);
}

