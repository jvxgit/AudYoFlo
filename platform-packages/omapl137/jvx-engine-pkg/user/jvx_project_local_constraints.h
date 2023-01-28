#ifndef __JVX_PROJECT_LOCAL_CONSTRAINTS_H__
#define __JVX_PROJECT_LOCAL_CONSTRAINTS_H__

// Maximum number of largest buffer in system in parallel
#define MAX_NUM_INOUT_BUFFERS 4

// Largest buffer to be transmitted via remote call. Here: 1024 samples plus header overhead
#define MAX_LENGTH_SINGLE_PACKET (513 * sizeof(jvxData) + sizeof(jvxRcProtocol_data_message)) 

#endif
