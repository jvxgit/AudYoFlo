/*
 * File:   priorityList.h
 * Author: hauke
 *
 * Created on 11. Dezember 2011, 21:49
 */

#ifndef PRIORITYLIST_H
#define	PRIORITYLIST_H

#define numberPriorityAccesses 5
static snd_pcm_access_t priorityAccesses[numberPriorityAccesses] =
{
    SND_PCM_ACCESS_RW_INTERLEAVED,
    SND_PCM_ACCESS_RW_NONINTERLEAVED,
    SND_PCM_ACCESS_MMAP_INTERLEAVED,
    SND_PCM_ACCESS_MMAP_NONINTERLEAVED,
    SND_PCM_ACCESS_MMAP_COMPLEX
};

static std::string text_priorityAccesses[numberPriorityAccesses] =
{
	"SND_PCM_ACCESS_RW_INTERLEAVED",
    "SND_PCM_ACCESS_RW_NONINTERLEAVED",
    "SND_PCM_ACCESS_MMAP_INTERLEAVED",
    "SND_PCM_ACCESS_MMAP_NONINTERLEAVED"
	"SND_PCM_ACCESS_MMAP_COMPLEX"
};

#define numberPriorityFormats 2
static snd_pcm_format_t priorityFormats[2] =
{
        SND_PCM_FORMAT_S32,
        SND_PCM_FORMAT_S16
};

static std::string text_priorityFormats[2] =
{
        "SND_PCM_FORMAT_S32",
        "SND_PCM_FORMAT_S16"
};

/*
#define numberPrioritySamplerates 9
static unsigned int prioritySamplerates[9] =
{
    8000,
    11025,
    16000,
    22050,
    32000,
    44100,
    48000
};
*/
#define period_default 2
#define buffersize_default 128

#endif	/* PRIORITYLIST_H */
