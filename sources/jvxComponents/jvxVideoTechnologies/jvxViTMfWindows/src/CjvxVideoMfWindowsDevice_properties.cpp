#include "CjvxVideoMfWindowsDevice.h"

// ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ##

void 
CjvxVideoMfWindowsDevice::scanProperties(IMFSourceReader* reader)
{
	HRESULT hr;
	HRESULT nativeTypeErrorCode = S_OK;
	DWORD count = 0;
	int besterror = 0xfffffff;
	int bestfit = 0;

	while (nativeTypeErrorCode == S_OK && besterror)
	{
		jvxVideoSetting oneForm;
		oneForm.form = JVX_DATAFORMAT_NONE;
		oneForm.subform = JVX_DATAFORMAT_GROUP_NONE;
		oneForm.height = 0;
		oneForm.width = 0;
		oneForm.id = count;
		oneForm.fps_min = 0;
		oneForm.fps_max = 0;
		oneForm.fps = 0;
		UINT32 w = 0, h = 0;

		IMFMediaType * nativeType = NULL;

		// Get the native media types
		nativeTypeErrorCode = reader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, count, &nativeType);
		if (nativeTypeErrorCode != S_OK)
		{
			// No more native media types
			break;
		}

		UINT32 numerator = 0, denominator  = 0, frameRateMin = 0, frameRateMax = 0;
		hr = MFGetAttributeRatio( nativeType, MF_MT_FRAME_RATE, &numerator, &denominator);
		JVX_ASSERT_X("MFGetAttributeRatio", 0, (hr == S_OK));
		oneForm.fps = (jvxData)numerator / (jvxData)denominator;
		oneForm.fps_min = oneForm.fps;
		oneForm.fps_max = oneForm.fps;

		hr = MFGetAttributeRatio(nativeType, MF_MT_FRAME_RATE_RANGE_MIN, &frameRateMin, &denominator);
		if (hr == S_OK)
		{
			oneForm.fps_min = (jvxData)frameRateMin / (jvxData)denominator;
		}

		hr = MFGetAttributeRatio(nativeType, MF_MT_FRAME_RATE_RANGE_MAX, &frameRateMax, &denominator);
		if (hr == S_OK)
		{
			oneForm.fps_max = (jvxData)frameRateMax / (jvxData)denominator;
		}
		// get the media type 
		GUID nativeGuid = { 0 };
		hr = nativeType->GetGUID(MF_MT_MAJOR_TYPE, &nativeGuid);
		if (nativeGuid == MFMediaType_Audio)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type Audio." << std::endl;
#endif
		}
		else if (nativeGuid == MFMediaType_Binary)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type Binary Stream." << std::endl;
#endif
		}
		else if (nativeGuid == MFMediaType_FileTransfer)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type File Transfer." << std::endl;
#endif
		}
		else if (nativeGuid == MFMediaType_HTML)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type HTML Stream." << std::endl;
#endif
		}
		else if (nativeGuid == MFMediaType_Image)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type Image." << std::endl;
#endif
		}
		else if (nativeGuid == MFMediaType_Protected)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type Protected Content." << std::endl;
#endif
		}
#if _MSC_VER >= 1910
		else if (nativeGuid == MFMediaType_Perception)
		{
			std::cout << "--> Media Type Streams from a camera sensor or processing unit that reasons and understands raw video data and provides understanding of the environment or humans in it." << std::endl;
		}
#endif
		else if (nativeGuid == MFMediaType_SAMI)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type Synchronized Accessible Media Interchange(SAMI) captions." << std::endl;
#endif
		}
		else if (nativeGuid == MFMediaType_Script)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type Script Stream." << std::endl;
#endif
		}
		else if (nativeGuid == MFMediaType_Stream)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type Stream." << std::endl;
#endif
		}
		else if (nativeGuid == MFMediaType_Video)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Type Video." << std::endl;
#endif
		}
		else
		{
			std::cout << "--> Media Type UNKNOWN." << std::endl;
		}

		// Make sure it is video!
		JVX_ASSERT_X("Check media GUID", 0, nativeGuid == MFMediaType_Video);

		// == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

		GUID nativeSubGuid = { 0 };
		hr = nativeType->GetGUID(MF_MT_SUBTYPE, &nativeSubGuid);

		if(nativeSubGuid == MFVideoFormat_RGB8)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype RGB, 8 bits per pixel (bpp). (Same memory layout as D3DFMT_P8.)" << std::endl;
#endif
		}
		else if(nativeSubGuid == MFVideoFormat_RGB555)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype RGB 555, 16 bpp. (Same memory layout as D3DFMT_X1R5G5B5.)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_RGB565)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype RGB 565, 16 bpp. (Same memory layout as D3DFMT_R5G6B5.)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_RGB24)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype RGB, 24 bpp" << std::endl;
#endif
			oneForm.form = JVX_DATAFORMAT_BYTE;
			oneForm.subform = JVX_DATAFORMAT_GROUP_VIDEO_RGB24;
		}
		else if (nativeSubGuid == MFVideoFormat_RGB32)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype RGB, 32 bpp" << std::endl;
#endif
			oneForm.form = JVX_DATAFORMAT_BYTE;
			oneForm.subform = JVX_DATAFORMAT_GROUP_VIDEO_RGBA32;
		}
		else if(nativeSubGuid == MFVideoFormat_ARGB32)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype RGB, 32 bpp with alpha channel" << std::endl;
#endif
		}
#if _MSC_VER >= 1910
		else if (nativeSubGuid == MFVideoFormat_A2R10G10B10)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype RGB, 10 bpp for each color and 2 bpp for alpha" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_A16B16G16R16F)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype RGB, 16 bpp with alpha channel. (Same memory layout as D3DFMT_A16B16G16R16F)" << std::endl;
#endif
		}
#endif
		else if (nativeSubGuid == MFVideoFormat_AI44)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype AI44	4:4:4	Packed	Palettized" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_I420)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype I420	4:2:0	Planar	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_AYUV)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype AYUV	4:4:4	Packed	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_IYUV)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype IYUV	4:2:0	Planar	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_NV11)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype NV11	4:1:1	Planar	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_NV12)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype NV12	4:2:0	Planar	8" << std::endl;
#endif
			oneForm.form = JVX_DATAFORMAT_BYTE;
			oneForm.subform = JVX_DATAFORMAT_GROUP_VIDEO_NV12;
		}
		else if (nativeSubGuid == MFVideoFormat_UYVY)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype UYVY	4:2:2	Packed	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_Y41P)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype Y41P	4:1:1	Packed	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_Y41T)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype Y41T	4:1:1	Packed	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_Y42T)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype Y42T	4:2:2	Packed	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_YUY2)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype YUY2	4:2:2	Packed	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_YVU9)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype YVU9	8:4:4	Planar	9" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_YV12)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype YV12	4:2:0	Planar	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_YVYU)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype YVYU	4:2:2	Packed	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_YVYU)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype YVYU	4:2:2	Packed	8" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_P010)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype P010	4:2:0	Planar	10" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_P016)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype P016	4:2:0	Planar	16" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_P210)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype P210	4:2:2	Planar	10" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_P216)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype P216	4:2:2	Planar	16" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_v210)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype v210	4:2:2	Packed	10" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_v216)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype v216	4:2:2	Packed	16" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_v410)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype v40	4:4:4	Packed	10" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_Y210)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype Y210	4:2:2	Packed	10" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_Y216)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype Y216	4:2:2	Packed	16" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_Y410)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype Y40	4:4:4	Packed	10" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_Y416)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype Y416	4:4:4	Packed	16" << std::endl;
#endif
		}
#if _MSC_VER >= 1910
		else if (nativeSubGuid == MFVideoFormat_L8)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype 8-bit luminance only. (bpp). (Same memory layout as D3DFMT_L8.)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_L16)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype 16-bit luminance only. (Same memory layout as D3DFMT_L16.)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_D16)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype 16-bit z-buffer depth. (Same memory layout as D3DFMT_D16.)" << std::endl;
#endif
		}
#endif
		else if (nativeSubGuid == MFVideoFormat_DV25)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype dv25 DVCPRO 25 (525-60 or 625-50)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_DV50)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype dv50 DVCPRO 50 (525-60 or 625-50)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_DVC)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype dvc DVC/DV Video" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_DVH1)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype dvh1'	DVCPRO 100 (1080/60i, 1080/50i, or 720/60P)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_DVHD)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype dvhd	HD-DVCR (1125-60 or 1250-50)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_DVSD)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype dvsd SDL - DVCR(525 - 60 or 625 - 50)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_DVSL)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype dvsl SD-DVCR (525-60 or 625-50)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_H263)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype H263	H.263 video" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_H264)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype H264 H.264 video" << std::endl;
#endif
		}
#if _MSC_VER >= 1910
		else if (nativeSubGuid == MFVideoFormat_H265)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype H265 H.265 video" << std::endl;
#endif
		}
#endif
		else if (nativeSubGuid == MFVideoFormat_H264_ES)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MFVideoFormat_H264_ES" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_HEVC)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype HEVC" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_HEVC_ES)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MFVideoFormat_HEVC_ES" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_M4S2)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype M4S2	MPEG-4 part 2 video" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_MJPG)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MJPG	Motion JPEG" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_MP43)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MP43	Microsoft MPEG 4 codec version 3. This codec is no longer supported." << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_MP4S)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MP4S	ISO MPEG 4 codec version 1" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_MP4V)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MP4V	MPEG-4 part 2 video" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_MPEG2)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MPEG-2 video. (Equivalent to MEDIASUBTYPE_MPEG2_VIDEO in DirectShow.)" << std::endl;
#endif
		}
#if _MSC_VER >= 1910
		else if (nativeSubGuid == MFVideoFormat_VP80)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MPG1	VP8 video" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_VP90)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MPG1	VP9 video" << std::endl;
#endif
		}
#endif
		else if (nativeSubGuid == MFVideoFormat_MPG1)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MPG1	MPEG-1 video" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_MSS2)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MSS2	Windows Media Video 9 Screen codec" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_MSS1)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MSS1	Windows Media Screen codec version 1" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_WMV1)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype WMV1'	Windows Media Video 7 codec" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_WMV2)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype WMV2	Windows Media Video 8 codec" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_WMV3)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype WMV3	Windows Media Video 9 codec" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_WVC1)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype WVC1	SMPTE 421M (VC - 1)" << std::endl;
#endif
		}
		else if (nativeSubGuid == MFVideoFormat_420O)
		{
#ifdef VERBOSE_2
			std::cout << "--> Media Subtype MFVideoFormat_420O" << std::endl;
#endif
		}
		else
		{
			std::cout << "--> Media Subtype UNKNOWN." << std::endl;
		}

		hr = MFGetAttributeSize(nativeType, MF_MT_FRAME_SIZE, &w, &h);

		oneForm.width = w;
		oneForm.height = h;

		if (oneForm.form == JVX_DATAFORMAT_BYTE)
		{
			oneForm.descriptor = "Mode #" + jvx_size2String(lstModes.size()) + ": " + jvx_size2String(oneForm.width) + "x" + jvx_size2String(oneForm.height) + ", ";
			oneForm.descriptor += jvxDataFormatGroup_txt(oneForm.subform);
			
#ifdef VERBOSE_1
			std::cout << "Mode #" << lstModes.size() << ": " << oneForm.width << "x" << oneForm.height << ", " << std::flush;
			std::cout << jvxDataFormatGroup_txt(oneForm.subform) << std::flush;
			std::cout << "Rate: " << oneForm.fps_min << "::" << oneForm.fps_max << "--" << oneForm.fps << std::endl;
#endif
			lstModes.push_back(oneForm);
		}
		nativeType->Release();
		count++;
	}
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxVideoMfWindowsDevice, on_mode_selected)
{
	// If we changed the mode, we need to run the test again
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMf_device::configuration_mf.mode_selection))
	{
		if (JVX_CHECK_SIZE_SELECTED(_common_set_ocon.theData_out.con_link.uIdConn))
		{
			_request_test_chain_master(_common_set_ocon.theData_out.con_link.uIdConn);
		}
	}
	return JVX_NO_ERROR;
}
