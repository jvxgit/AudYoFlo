/* 
 *****************************************************
 * Filename: configConfiguration.h
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description:                                      *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/

#ifndef _CONFIGCONFIGURATION_H__
#define _CONFIGCONFIGURATION_H__

#define CONFIG_NAME_SECTION_TECHNOLOGY_SELECTED "genericWrapper_technology_selected"
#define CONFIG_NAME_SECTION_TECHNOLOGY_ACTIVE "genericWrapper_technology_active"

#define CONFIG_NAME_SECTION_DEVICE_SELECTED "genericWrapper_device_selected"
#define CONFIG_NAME_SECTION_DEVICE_ACTIVE "genericWrapper_device_active"

#define CONFIG_NAME_PATH_DLLS "pathDlls"
#define CONFIG_NAME_DIRECTORY_LOADED "directoryLoaded"
#define CONFIG_NAME_SELECTED_TECHNOLOGY "selectedTechnologyName"

#define CONFIG_NAME_INPUT_FILENAMES "filenamesInput"
#define CONFIG_NAME_OUTPUT_FILENAMES "filenamesOutput"

#define CONFIG_NAME_INPUT_LOOPFILES "loopInput"
#define CONFIG_NAME_INPUT_BOOSTPRIORITY "boostInput"
#define CONFIG_NAME_INPUT_SAMPLERATES "sampleratesInput"
#define CONFIG_NAME_INPUT_CHANNELSFILES "channelsInput" 
#define CONFIG_NAME_INPUT_NUMBUFFERS "numBuffersInput"
#define CONFIG_NAME_INPUT_ACTIVE_IDS "activeIdsChannelsInput"

#define CONFIG_NAME_OUTPUT_CHANNELSFILES "channelsOutput"
#define CONFIG_NAME_OUTPUT_SAMPLERATES "sampleratesOutput"
#define CONFIG_NAME_OUTPUT_32BITFILES "files32BitOutput"
#define CONFIG_NAME_OUTPUT_BOOSTPRIORITY "boostOutput"
#define CONFIG_NAME_OUTPUT_NUMBUFFERS "numBuffersOutput"
#define CONFIG_NAME_OUTPUT_ACTIVE_IDS "activeIdsChannelsOutput"

#define CONFIG_NAME_BSIZES_HW "buffersizesHW"
#define CONFIG_NAME_USED_BSIZE_HW "usedBuffersizeHW"
#define CONFIG_NAME_SRATES_HW "sampleratesHW"
#define CONFIG_NAME_USED_SRATE_HW "usedSamplerateHW"
#define CONFIG_NAME_FORMATS_HW "formatsHW"
#define CONFIG_NAME_USED_FORMAT_HW "usedFormatHW"

#define CONFIG_NAME_FORCESYNCMODE "stratForceSyncMode"
#define CONFIG_NAME_DEVIATIONMAX "stratDeviationMax"
#define CONFIG_NAME_LOOPMAX "stratLoopsMaxforceSyncMode"
#define CONFIG_NAME_DISADVANTAGE_LOOP "stratDisadvantageLoops"
#define CONFIG_NAME_SAMPLERATEMAX "stratSamplerateMax"
#define CONFIG_NAME_AUTOFS "stratAutoFs"

#define CONFIG_NAME_RESAMPLER_QUALITY_INPUT "qualityResamplerInput"
#define CONFIG_NAME_RESAMPLER_QUALITY_OUTPUT "qualityResamplerOutput"

#define CONFIG_NAME_LOADESTIMATE "loadEstimate"

#define CONFIG_NAME_CHANNELMAP_INPUT "channelMappingInput"
#define CONFIG_NAME_CHANNELMAP_OUTPUT "channelMappingOutput"

#define CONFIG_NAME_GEOMETRY_QT "geometryQT"

#define CONFIG_NAME_SAMPLERATE_ALL_DEVICES "tech_samplerate_selected"
#define CONFIG_NAME_BUFFERSIZE_ALL_DEVICES "tech_buffersize_selected"
#define CONFIG_NAME_PROCESSINGFORMAT_ALL_DEVICES "tech_format_selected"					

#endif