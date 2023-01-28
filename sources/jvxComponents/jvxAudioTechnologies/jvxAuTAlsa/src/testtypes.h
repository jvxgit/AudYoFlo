/*
 * File:   testtypes.h
 * Author: hauke
 *
 * Created on 10. Dezember 2011, 22:06
 */

#ifndef TESTTYPES_H
#define	TESTTYPES_H

#define desiredAccessType SND_PCM_ACCESS_MMAP_NONINTERLEAVED
#define numAccessTypes 5
static snd_pcm_access_t accessTypes[numAccessTypes] =
{
        SND_PCM_ACCESS_MMAP_INTERLEAVED,
        SND_PCM_ACCESS_MMAP_NONINTERLEAVED,
        SND_PCM_ACCESS_MMAP_COMPLEX,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        SND_PCM_ACCESS_RW_NONINTERLEAVED
};
static std::string accessTypesStrings[numAccessTypes] =
{
        "SND_PCM_ACCESS_MMAP_INTERLEAVED",
        "SND_PCM_ACCESS_MMAP_NONINTERLEAVED",
        "SND_PCM_ACCESS_MMAP_COMPLEX",
        "SND_PCM_ACCESS_RW_INTERLEAVED",
        "SND_PCM_ACCESS_RW_NONINTERLEAVED"
};

#define desiredDataFormat
#define numFormatTypes 47
static snd_pcm_format_t formatTypes[numFormatTypes] =
{
//        SND_PCM_FORMAT_UNKNOWN,
        SND_PCM_FORMAT_S8,
        SND_PCM_FORMAT_U8,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_FORMAT_S16_BE,
        SND_PCM_FORMAT_U16_LE,
        SND_PCM_FORMAT_U16_BE,
        SND_PCM_FORMAT_S24_LE,
        SND_PCM_FORMAT_S24_BE,
        SND_PCM_FORMAT_U24_LE,
        SND_PCM_FORMAT_U24_BE,
        SND_PCM_FORMAT_S32_LE,
        SND_PCM_FORMAT_S32_BE,
        SND_PCM_FORMAT_U32_LE,
        SND_PCM_FORMAT_U32_BE,
        SND_PCM_FORMAT_FLOAT_LE,
        SND_PCM_FORMAT_FLOAT_BE,
        SND_PCM_FORMAT_FLOAT64_LE,
        SND_PCM_FORMAT_FLOAT64_BE,
        SND_PCM_FORMAT_IEC958_SUBFRAME_LE,
        SND_PCM_FORMAT_IEC958_SUBFRAME_BE,
        SND_PCM_FORMAT_MU_LAW,
        SND_PCM_FORMAT_A_LAW,
        SND_PCM_FORMAT_IMA_ADPCM,
        SND_PCM_FORMAT_MPEG,
        SND_PCM_FORMAT_GSM,
        SND_PCM_FORMAT_SPECIAL,
        SND_PCM_FORMAT_S24_3LE,
        SND_PCM_FORMAT_S24_3BE,
        SND_PCM_FORMAT_U24_3LE,
        SND_PCM_FORMAT_U24_3BE,
        SND_PCM_FORMAT_S20_3LE,
        SND_PCM_FORMAT_S20_3BE,
        SND_PCM_FORMAT_U20_3LE,
        SND_PCM_FORMAT_U20_3BE,
        SND_PCM_FORMAT_S18_3LE,
        SND_PCM_FORMAT_S18_3BE,
        SND_PCM_FORMAT_U18_3LE,
        SND_PCM_FORMAT_U18_3BE,
        SND_PCM_FORMAT_S16,
        SND_PCM_FORMAT_U16,
        SND_PCM_FORMAT_S24,
        SND_PCM_FORMAT_U24,
        SND_PCM_FORMAT_S32,
        SND_PCM_FORMAT_U32,
        SND_PCM_FORMAT_FLOAT,
        SND_PCM_FORMAT_FLOAT64,
        SND_PCM_FORMAT_IEC958_SUBFRAME
};
static std::string formatTypesStrings[numFormatTypes] =
{
//        "SND_PCM_FORMAT_UNKNOWN",
        "SND_PCM_FORMAT_S8",
        "SND_PCM_FORMAT_U8",
        "SND_PCM_FORMAT_S16_LE",
        "SND_PCM_FORMAT_S16_BE",
        "SND_PCM_FORMAT_U16_LE",
        "SND_PCM_FORMAT_U16_BE",
        "SND_PCM_FORMAT_S24_LE",
        "SND_PCM_FORMAT_S24_BE",
        "SND_PCM_FORMAT_U24_LE",
        "SND_PCM_FORMAT_U24_BE",
        "SND_PCM_FORMAT_S32_LE",
        "SND_PCM_FORMAT_S32_BE",
        "SND_PCM_FORMAT_U32_LE",
        "SND_PCM_FORMAT_U32_BE",
        "SND_PCM_FORMAT_FLOAT_LE",
        "SND_PCM_FORMAT_FLOAT_BE",
        "SND_PCM_FORMAT_FLOAT64_LE",
        "SND_PCM_FORMAT_FLOAT64_BE",
        "SND_PCM_FORMAT_IEC958_SUBFRAME_LE",
        "SND_PCM_FORMAT_IEC958_SUBFRAME_BE",
        "SND_PCM_FORMAT_MU_LAW",
        "SND_PCM_FORMAT_A_LAW",
        "SND_PCM_FORMAT_IMA_ADPCM",
        "SND_PCM_FORMAT_MPEG",
        "SND_PCM_FORMAT_GSM",
        "SND_PCM_FORMAT_SPECIAL",
        "SND_PCM_FORMAT_S24_3LE",
        "SND_PCM_FORMAT_S24_3BE",
        "SND_PCM_FORMAT_U24_3LE",
        "SND_PCM_FORMAT_U24_3BE",
        "SND_PCM_FORMAT_S20_3LE",
        "SND_PCM_FORMAT_S20_3BE",
        "SND_PCM_FORMAT_U20_3LE",
        "SND_PCM_FORMAT_U20_3BE",
        "SND_PCM_FORMAT_S18_3LE",
        "SND_PCM_FORMAT_S18_3BE",
        "SND_PCM_FORMAT_U18_3LE",
        "SND_PCM_FORMAT_U18_3BE",
        "SND_PCM_FORMAT_S16",
        "SND_PCM_FORMAT_U16",
        "SND_PCM_FORMAT_S24",
        "SND_PCM_FORMAT_U24",
        "SND_PCM_FORMAT_S32",
        "SND_PCM_FORMAT_U32",
        "SND_PCM_FORMAT_FLOAT",
        "SND_PCM_FORMAT_FLOAT64",
        "SND_PCM_FORMAT_IEC958_SUBFRAME"
};

/*
#define numTestSamplerates 9
static int testSamplerates[9] =
{
    8000,
    11025,
    16000,
    22050,
    32000,
    44100,
    48000,
    96000,
    192000
};
*/

#define testPeriodMax 1024

#define testBuffersizeMax 1024

#endif	/* TESTTYPES_H */
