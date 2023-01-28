#ifndef __CONFIG_VOCODER__H__
#define __CONFIG_VOCODER__H__

// Vocoder configuration
#define MIN_REAL                (REAL)(1.0E-37)
#define MAX_REAL                (REAL)(1.0E+37)
#define MIN_SHORT               -32768L
#define MAX_SHORT               32767L
#define MAX_INT                 2147483000L
#define MIN_INT                 -2147482999L
#define SAMPLING_RATE			8000
#define LPC_ORDER_MIN               3
#define LPC_DIFF				2
#define LPC_ORDER_MAX               19
#define FRAME_SIZE             128
#define FRAME_SIZE_ANALYSIS             256
#define LOOKAHEAD				32
#define VUV_DECISION_BOUND               0.65
#define PPERIOD_MAX 80
#define PPERIOD_MIN 20
#define TIMEOUT_UPDATE   200
#define THRESHOLD_FAC 0.0005

#endif