/* 
 *****************************************************
 * Filename: mexCommandsFileReader.h
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

#define FLD_NAME_COPYRIGHT "COPYRIGHT"
#define	FLD_NAME_VERSION "VERSION"
#define FLD_NAME_MODULENAME "NAME"
#define FLD_NAME_STATE "STATE"
#define FLD_NAME_FILENAME "FILENAME"
#define FLD_NAME_SESSIONS "SESSIONS"

#define FLD_NAME_SESSION_TAG_NAME "TAG_NAME"
#define FLD_NAME_SESSION_TAG_VALUE "TAG_VALUE"

#define FLD_NAME_DATATYPE "DATATYPE"
#define FLD_NAME_USERID "USERID"
#define FLD_NAME_USERSUBID "SUBUSERID"
#define FLD_NAME_TICKTIME "TICKTIME"
#define FLD_NAME_DATAFIELD "DATAFIELD"
#define FLD_NAME_ID_DATASET "DATASET_ID"

#define FLD_NAME_DESCRIPTION_DATASET "DESCRIPTION"
#define	FLD_NAME_DATASET_DATATYPE "DATATYPE"
#define FLD_NAME_DATASET_DATATYPE_USER "DATATYPEUSER"
#define FLD_NAME_DATASET_ID "DATASET_ID"
#define FLD_NAME_DATASET_LENGTH "DATASET_CHUNKLENGTH_IFCONSTANT"

#define EXPRESSION_COPYRIGHT "copyright by Hauke Kruger, IND, RWTH Aachen"
#define EXPRESSION_VERSION "0.0.2"
#define EXPRESSION_MODULENAME "FILEREADER FOR LOGFILES"

#define EXCEPTION_ERROR_MESSAGE_ID -1
#define WRONG_DATATYPE_ID_ERROR_MESSAGE_ID -2
#define WRONG_ID_ERROR_MESSAGE_ID -3
#define WRONG_DATATYPE_CONTENT_ERROR_MESSAGE_ID -4
#define MISSING_CONTENT_ERROR_MESSAGE_ID -5
#define UNSUPPORTED_OPTION_ERROR_MESSAGE_ID -6
#define MISC_ERROR_MESSAGE_ID -7

#define JVX_FIO_STATE_UNINITIALIZED 0
#define JVX_FIO_STATE_INITIALIZED 1
#define JVX_FIO_STATE_SCANNED 2

//! Enum to specify the fileReaderMatlab commands in integer format
typedef enum
{ 
	//! Open the file to read from
	JVX_FIO_COMMAND_ACTIVATE = 0,

	//! Scan all available sessions
	JVX_FIO_COMMAND_PREPARE = 1,

	//! Return the number of sessions available in the file to be read from
	JVX_FIO_COMMAND_RETURN_NUMBER_SESSIONS = 2,

	JVX_FIO_COMMAND_RETURN_TAGS_SESSION = 3,

	//! Return the number of datasets for the current session
	JVX_FIO_COMMAND_RETURN_NUMBER_DATASETS_SESSION = 4,

	//! Return the dataset for a session, specified by data session ID
	JVX_FIO_COMMAND_RETURN_DATASET_SESSION = 5,

	//! return the number of datachunks for a session
	JVX_FIO_COMMAND_RETURN_NUMBER_DATACHUNKS_SESSION = 6,

	//! Return the number of datachunks for a session and for a dataset ID
	JVX_FIO_COMMAND_RETURN_NUMBER_DATACHUNKS_DATASET_SESSION = 7,

	//! Return the next datachunk for a session
	JVX_FIO_COMMAND_RETURN_NEXT_DATACHUNK_SESSION = 8,

	//! Return the next datachunk for a session and a specific dataset id
	JVX_FIO_COMMAND_RETURN_NEXT_DATACHUNK_DATASET_SESSION = 9,

	//! Return fields for a dataset, allocating an array for multiple datachunks
	JVX_FIO_COMMAND_RETURN_NEXT_DATACHUNK_DATASET_SESSION_COMPLETE_FIELD_DATA = 10,

	//! Rewind datachunk pointer
	JVX_FIO_COMMAND_REWIND_SESSION = 11,

	//! Rewind datachunk pointer for a specific session
	JVX_FIO_COMMAND_REWIND_DATASET_SESSION = 12,

	//! Return the progress in datachunks for the session access
	JVX_FIO_COMMAND_RETURN_PROGRESS_SESSION = 13,

	//! Return the progress in datachunks for the session and dataset access
	JVX_FIO_COMMAND_RETURN_PROGRESS_DATASET_SESSION = 14,

	//! Unscan the sessions
	JVX_FIO_COMMAND_POSTPROCESS = 15,

	//! Close the input file
	JVX_FIO_COMMAND_DEACTIVATE = 16,

	JVX_FIO_COMMAND_LIMIT = 17
} JVX_FIO_COMMAND;

 typedef struct
 {
	 JVX_FIO_COMMAND cmdId;
	 std::string cmdToken;
	 std::string cmdDescription;	 
 } jvxFioCmdMapper;

 static jvxFioCmdMapper mapperCommands[JVX_FIO_COMMAND_LIMIT] = 
 {
	 {JVX_FIO_COMMAND_ACTIVATE, "activate", "--"},
	 {JVX_FIO_COMMAND_PREPARE, "prepare", "--"},
	 {JVX_FIO_COMMAND_RETURN_NUMBER_SESSIONS, "number_sessions", "--"},	
	 {JVX_FIO_COMMAND_RETURN_TAGS_SESSION, "tags_sessions", "--"},	
	 {JVX_FIO_COMMAND_RETURN_NUMBER_DATASETS_SESSION, "number_datasets_session", "--"},	
	 {JVX_FIO_COMMAND_RETURN_DATASET_SESSION, "dataset_session", "--"},	
	 {JVX_FIO_COMMAND_RETURN_NUMBER_DATACHUNKS_SESSION, "number_datachunks_session", "--"},
	 {JVX_FIO_COMMAND_RETURN_NUMBER_DATACHUNKS_DATASET_SESSION, "number_datachunks_dataset_session", "--"},	
	 {JVX_FIO_COMMAND_RETURN_NEXT_DATACHUNK_SESSION, "next_datachunk_session", "--"},
	 {JVX_FIO_COMMAND_RETURN_NEXT_DATACHUNK_DATASET_SESSION, "next_datachunk_dataset_session", "--"},
	 {JVX_FIO_COMMAND_RETURN_NEXT_DATACHUNK_DATASET_SESSION_COMPLETE_FIELD_DATA, "next_datachunk_dataset_session_complete", "--"},
	 {JVX_FIO_COMMAND_REWIND_SESSION, "rewind_session", "--"},
	 {JVX_FIO_COMMAND_REWIND_DATASET_SESSION, "rewind_dataset_session", "--"},	
	 {JVX_FIO_COMMAND_RETURN_PROGRESS_SESSION, "progress_session", "--"},
	 {JVX_FIO_COMMAND_RETURN_PROGRESS_DATASET_SESSION, "progress_session_dataset", "--"},
	 {JVX_FIO_COMMAND_POSTPROCESS, "postprocess", "--"},	
	 {JVX_FIO_COMMAND_DEACTIVATE, "deactivate", "--"}
 };

 // ================================================================================================

//! Typedef specification for the access of fields for datachunks
typedef enum
{ 
	//! Copy rectangular fields of data (constant size x frames)
	JVX_FIO_COPY_DATA = 0,

	//! Copy rectangular fields of ids for the datachunks
	JVX_FIO_COPY_ID_CHUNK = 1,

	//! Copy rectangular fields of ids for the datatypes
	JVX_FIO_COPY_ID_DATA_TYPE = 2,

	//! Copy rectangular fields of ids for user ids
	JVX_FIO_COPY_ID_USER = 3,

	//! Copy rectangular fields of ids for user sub-ids
	JVX_FIO_COPY_SUB_ID_USER = 4,

	//! Copy rectangular fields of timing nformation for each datachunk (INT64)
	JVX_FIO_COPY_TIMING = 5,

	//! Copy rectangular fields of number of elements for each datachunk
	JVX_FIO_COPY_NUMBER_ELEMENTS = 6,

	JVX_FIO_COPY_LIMIT = 7
} JVX_FIO_COPY_TYPE;

typedef struct
 {
	 JVX_FIO_COPY_TYPE cmdId;
	 std::string cmdToken;
	 std::string cmdDescription;	 
 } jvxFioDataCopyTypeMapper;

static jvxFioDataCopyTypeMapper mapperDataCopyTypes[JVX_FIO_COPY_LIMIT] = 
{
	 {JVX_FIO_COPY_DATA, "copy_data", "--"},
	 {JVX_FIO_COPY_ID_CHUNK, "copy_id_chunk", "--"},
	 {JVX_FIO_COPY_ID_DATA_TYPE, "copy_id_data_type", "--"},
	 {JVX_FIO_COPY_ID_USER, "copy_id_user", "--"},
	 {JVX_FIO_COPY_SUB_ID_USER, "copy_sub_id_user", "--"},
	 {JVX_FIO_COPY_TIMING, "copy_timing", "--"},
	 {JVX_FIO_COPY_NUMBER_ELEMENTS, "copy_number_elements", "--"}
};