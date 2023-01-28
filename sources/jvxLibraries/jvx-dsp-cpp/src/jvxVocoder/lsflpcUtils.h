#ifndef LSFLPCUTILS_H__
#define LSFLPCUTILS_H__

#include "jvxVocoder/vocoder.h"


int 
convertLpc2Lsf(const jvxData *lpcs, jvxData *lsf, int amountLsfs);

int 
convertLsf2Lpc(const jvxData *lsf, jvxData *lpcs, int amountLsfs);

void
warp_Lsfs(jvxData* lsfs, int number_lsfs, jvxData warping_coeff);

#endif
