/* 
 *****************************************************
 * Filename: TrtpWriteLogfiles.h
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

#ifndef __TJVXDATALOGGER_H__
#define __TJVXDATALOGGER_H__

// In order to function propperly, the header must be aligned to 4 Bytes!!!
#define JVX_HEADER_F_IO_SIZE_HEADER 4
#define JVX_HEADER_F_IO_SESSION_BEGIN "STIN"
#define JVX_HEADER_F_IO_SESSION_END "STOU"
#define JVX_HEADER_F_IO_DATACHUNK "INDR"
#define JVX_HEADER_F_IO_ONETAG "TAGG"

#define JVX_FILE_IO_SIZE_NOTE 128

typedef struct
{
	std::string tagName;
	std::string tagValue;
} oneTagEntry;

// ==================================================================================

typedef enum
{
	JVX_DATATRECOVERY_EVENT_TYPE_PROGRESS = 0
} TjvxDatarecoveryEventType;

typedef void (*jvxDataRecoveryCallback)(TjvxDatarecoveryEventType tp, jvxHandle* content, jvxSize szFld);

// ==================================================================================

/** Datafield to store each datachunk on file */
typedef struct 
{
	//! Field to place the header in.
	jvxUInt8 fldReserved[JVX_HEADER_F_IO_SIZE_HEADER];

	//! Size of this datachunk, including the LOAD field
	jvxUInt32 szBytes;

	//! ID of this datachunk to identify the usage of a dataset
	jvxInt32 idDataChunk;

	//! Datatype for the dataset
	jvxInt32 dataTypeGlobal;

	//! Field that can be used by the user freely.
	jvxInt32 idUser;

	//! Another user field for additional information
	jvxInt32 subIdUser;

	//! Bytefield for ticktime
	jvxInt64 ticktime_us;

	//! Number of elements stored in the load field
	jvxUInt32 numberElements;
} jvxLogFileDataChunkHeader;

/**
* Datafield to store the datasets for recording session. */
typedef struct
{
	//! Size of this struct in bytes
	jvxUInt32 szBytes;

	//! ID for the datachunks associated with the dataset
	jvxInt32 idDataChunk;

	//! Datatype for the dataset
	jvxInt32 dataTypeGlobal;

	//! Specific datatype that can be used by the user
	jvxInt32 dataDescriptorUser;

	//! Number of elements for each datachunk, -1 if this number is not constant
	jvxInt32 numberElementsIfConstant;

	//! Specific note that can be used for better description
	char noteOnType[JVX_FILE_IO_SIZE_NOTE];

} jvxLogFileDataSetHeader;

/** Type to store arbitrary information as file header, still open so far.*/
typedef struct
{
	//! Field to store the overall header
	jvxUInt8 fldReserved[JVX_HEADER_F_IO_SIZE_HEADER];

	//! Format to store the number of sets
	jvxUInt32 numberSets;

	//! Reserved field for future, be sure to be 4 byte aligned on win32
	jvxUInt8 reserved[1024];
} jvxSessionHeader;

#endif
