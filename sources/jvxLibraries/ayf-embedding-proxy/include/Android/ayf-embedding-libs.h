#ifndef __AYF_EMBEDDING_LIBS_H__
#define __AYF_EMBEDDING_LIBS_H__

// This library must be loaded by anyone using the proxy
#define AYF_EMBEDDING_PROXY_HOST "ayf-embedding-proxy.so"

// These libraries will be loaded by the proxy lib
#define MIN_HOST_DLL_NAME "ayf-shared-connection-min-host.so"
#define EMB_HOST_DLL_NAME "ayf-shared-connection-emb-host.so"

#endif

