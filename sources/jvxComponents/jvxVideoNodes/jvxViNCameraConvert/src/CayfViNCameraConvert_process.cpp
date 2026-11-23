#include "CayfViNCameraConvert.h"
#include "HjvxDataLinkDescriptor.h"

#include <opencv2/imgproc.hpp>

#include <tracy/Tracy.hpp>

jvxErrorType
CayfViNCameraConvert::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// This module decides itself if it is operated in zero copy or not!
	// If not, allocate the required number of buffers
	_common_set_ldslave.num_min_buffers_out = _common_set_icon.theData_in->con_data.number_buffers;

	runtime.convertFormat = false;
	runtime.convertSize = false;

	if (jvx_check_in_out_params_match_test(_common_set_icon.theData_in, &_common_set_ocon.theData_out, JVX_CHECK_PARAM_ALL))
	{
		_common_set_ldslave.zeroCopyBuffering_cfg = true;
	}
	else
	{
		runtime.convert.from = _common_set_icon.theData_in->con_params.format_group;
		runtime.convert.to = _common_set_ocon.theData_out.con_params.format_group;

		// Input image size
		runtime.convert.width = _common_set_icon.theData_in->con_params.segmentation.x;
		runtime.convert.height = _common_set_icon.theData_in->con_params.segmentation.y;

		// Output image size
		runtime.convert.widthOut = _common_set_ocon.theData_out.con_params.segmentation.x;
		runtime.convert.heightOut = _common_set_ocon.theData_out.con_params.segmentation.y;

		if (runtime.convert.from != runtime.convert.to)
		{
			runtime.convertFormat = true;
		}

		if (
			(runtime.convert.width != runtime.convert.widthOut) ||
			(runtime.convert.height != runtime.convert.heightOut))
		{
			// This happens if the camera can not deliver the size requested by the output (e.g. Kinect 4K camera
			// connected to a mixer with 640x480): the image is scaled in the last step of the conversion
			runtime.convertSize = true;
		}

		// Currently, only output to RGBA
		if (runtime.convert.to != JVX_DATAFORMAT_GROUP_VIDEO_RGBA32)
		{
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, "Camera converter only supports output format RGBA32.", JVX_ERROR_UNSUPPORTED);
			return JVX_ERROR_UNSUPPORTED;
		}

		jvxSize szField = jvx_derive_buffersize(_common_set_ocon.theData_out.con_params);
		assert(szField == _common_set_ocon.theData_out.con_params.buffersize);
		runtime.convert.szFldOut = szField * jvxDataFormat_getsize(_common_set_ocon.theData_out.con_params.format);

		szField = jvx_derive_buffersize(_common_set_icon.theData_in->con_params);
		assert(szField == _common_set_icon.theData_in->con_params.buffersize);
		runtime.convert.szFldIn = szField * jvxDataFormat_getsize(_common_set_icon.theData_in->con_params.format);

		switch (runtime.convert.from)
		{
		case JVX_DATAFORMAT_GROUP_VIDEO_NV12:
			runtime.convert.NV12.plane1_Sz = runtime.convert.width * runtime.convert.height;
			runtime.convert.NV12.plane2_Sz = szField - runtime.convert.NV12.plane1_Sz;
			break;
		case JVX_DATAFORMAT_GROUP_VIDEO_RGB24:
		case JVX_DATAFORMAT_GROUP_VIDEO_YUYV:
		case JVX_DATAFORMAT_GROUP_VIDEO_RGBA32:
			break;
		default:
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, "Camera converter does not support the input format.", JVX_ERROR_UNSUPPORTED);
			return JVX_ERROR_UNSUPPORTED;
		}

		if (runtime.convertFormat && runtime.convertSize)
		{
			// Format conversion runs in input size into a temporary RGBA buffer, the scaling is done from there
			jvxLinkDataDescriptor_con_params pars = _common_set_icon.theData_in->con_params;
			pars.format_group = runtime.convert.to;
			pars.format = _common_set_ocon.theData_out.con_params.format;
			runtime.convert.szTmp = jvx_derive_buffersize(pars) * jvxDataFormat_getsize(pars.format);
			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.convert.fldTmp, jvxByte, runtime.convert.szTmp);
		}
	}

	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		release_conversion_buffers();
	}
	return res;
}

jvxErrorType
CayfViNCameraConvert::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	release_conversion_buffers();
	return res;
}

void
CayfViNCameraConvert::release_conversion_buffers()
{
	JVX_SAFE_DELETE_FIELD(runtime.convert.fldTmp);
	runtime.convert.fldTmp = nullptr;
	runtime.convert.szTmp = 0;
	runtime.convertFormat = false;
	runtime.convertSize = false;
}

jvxErrorType
CayfViNCameraConvert::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	ZoneScopedN("CameraConvert");
	if (zeroCopyBuffering_rt)
	{
		return fwd_process_buffers_icon(mt_mask, idx_stage);
	}

	jvxByte** bufsIn = jvx_process_icon_extract_input_buffers<jvxByte>(_common_set_icon.theData_in, idx_stage);
	jvxByte** bufsOut = jvx_process_icon_extract_output_buffers<jvxByte>(_common_set_ocon.theData_out);

	// RGBA image in input size: the output buffer if no scaling is involved, the temporary buffer otherwise.
	// The output buffer must never be addressed with the input size - that would write beyond its end!
	jvxByte* ptrRgbaInSize = bufsIn[0];
	if (runtime.convertFormat)
	{
		ptrRgbaInSize = runtime.convertSize ? runtime.convert.fldTmp : bufsOut[0];
		assert(ptrRgbaInSize);

		// The output is a buffer containing h x w RGBA entries.
		cv::Mat out = cv::Mat(
			(int)runtime.convert.height, (int)runtime.convert.width,
			CV_8UC4, ptrRgbaInSize);

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
				(int)runtime.convert.height, (int)runtime.convert.width,
				CV_8UC1, ptrRead);
			auto planeUV = cv::Mat((int)runtime.convert.height / 2,
				(int)runtime.convert.width / 2, CV_8UC2,
				ptrRead + runtime.convert.NV12.plane1_Sz);

			// Actually run the converter
			cv::cvtColorTwoPlane(planeY, planeUV, out, cv::COLOR_YUV2RGBA_NV12);
			break;
		}
		case JVX_DATAFORMAT_GROUP_VIDEO_RGB24:
		{
			jvxUInt8* ptrRead = (jvxUInt8*)bufsIn[0]; // channel 0
			auto planeRGB = cv::Mat(
				(int)runtime.convert.height, (int)runtime.convert.width,
				CV_8UC3, ptrRead);

			// Actually run the converter
			cv::cvtColor(planeRGB, out, cv::COLOR_RGB2RGBA);
			break;
		}
		case JVX_DATAFORMAT_GROUP_VIDEO_YUYV:
		{
			// If we involve an YUYV mapping, we need to use openCV

			// Each pixel is represented by two bytes, where one byte is a Y value, and the second byte contains U and V.
			// So, every two pixels share a U and V value.
			jvxUInt8* ptrRead = (jvxUInt8*)bufsIn[0]; // channel 0
			auto image = cv::Mat(
				(int)runtime.convert.height, (int)runtime.convert.width,
				CV_8UC2, ptrRead);

			cv::cvtColor(image, out, cv::COLOR_YUV2RGBA_YUYV);
			break;
		}
		default:
			assert(0);
			break;
		}

		// The target memory is given by the buffer, OpenCV must not have reallocated
		assert(out.data == (uchar*)ptrRgbaInSize);
	}

	if (runtime.convertSize)
	{
		// Scale the RGBA image (converted or original input) to the output size
		cv::Mat from = cv::Mat(
			(int)runtime.convert.height, (int)runtime.convert.width,
			CV_8UC4, ptrRgbaInSize);
		cv::Mat to = cv::Mat(
			(int)runtime.convert.heightOut, (int)runtime.convert.widthOut,
			CV_8UC4, bufsOut[0]);

		// Can be changed during processing, therefore evaluated per frame
		jvxSize idResize = jvx_bitfieldSelection2Id(genCameraConvert_node::config.resize_mode.value.selection(),
			genCameraConvert_node::config.resize_mode.value.entries.size());
		if (idResize == 1)
		{
			// Crop: cut the largest centered region with the aspect ratio of the output
			int wIn = (int)runtime.convert.width;
			int hIn = (int)runtime.convert.height;
			int wCrop = wIn;
			int hCrop = hIn;
			if ((jvxData)wIn * (jvxData)runtime.convert.heightOut > (jvxData)hIn * (jvxData)runtime.convert.widthOut)
			{
				// Input is wider than output: cut left and right
				wCrop = (int)((jvxData)hIn * (jvxData)runtime.convert.widthOut / (jvxData)runtime.convert.heightOut);
			}
			else
			{
				// Input is higher than output: cut top and bottom
				hCrop = (int)((jvxData)wIn * (jvxData)runtime.convert.heightOut / (jvxData)runtime.convert.widthOut);
			}
			wCrop = JVX_MAX(1, JVX_MIN(wCrop, wIn));
			hCrop = JVX_MAX(1, JVX_MIN(hCrop, hIn));
			from = from(cv::Rect((wIn - wCrop) / 2, (hIn - hCrop) / 2, wCrop, hCrop));
		}
		cv::resize(from, to, to.size(), 0, 0, cv::INTER_AREA);
		assert(to.data == (uchar*)bufsOut[0]);
	}
	return  fwd_process_buffers_icon(mt_mask, idx_stage);
}
