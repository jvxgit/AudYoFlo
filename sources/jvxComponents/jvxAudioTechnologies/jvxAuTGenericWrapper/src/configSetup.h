/* 
 *****************************************************
 * Filename: configSetup.h
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

#ifndef __CONFIGSETUP_H__
#define __CONFIGSETUP_H__

#define DEVISOR_8
#define INTERNAL_BUFFERSIZE 10024
#define BUFFERSIZE_DEF 1024
#define FILE_FORMAT_DEF fileIOBase::IO_16BIT
#define APPL_FORMAT_DEF JVX_DATAFORMAT_DATA

#define JVX_DELETEFLD(a) if(a) delete[](a);
#define JVX_DELETE(a) if(a) delete(a);

#endif