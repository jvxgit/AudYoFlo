#ifndef _JVX_CRC16_H_
#define _JVX_CRC16_H_

/* Taken from here: https://www.menie.org/georges/embedded/crc16.html */
/* Copyright 2001 - 2022 Georges Menie(www.menie.org) */

#include "jvx_system.h"

jvxUInt16 jvx_crc16_ccitt(const jvxByte *buf, jvxSize numBytes);

#endif 
