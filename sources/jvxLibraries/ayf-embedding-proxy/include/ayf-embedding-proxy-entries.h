#ifndef __AYF_EMBEDDING_PROXY_ENTRIES_H__
#define __AYF_EMBEDDING_PROXY_ENTRIES_H__

#ifdef AYF_EMBEDDING_PROXY_VIA_IMPORTLIB

extern "C"
{
	jvxErrorType ayf_embedding_proxy_init(const char* nm, jvxSize* idRegistered, ayfHostBindingReferences* retReferences, const char* fNameIni);
	jvxErrorType ayf_embedding_proxy_terminate(jvxSize idRegistered);

	jvxErrorType ayf_embedding_proxy_host_set(IjvxHost* hostRefOnInit);
	jvxErrorType ayf_embedding_proxy_host_reset();
};

#else

typedef jvxErrorType (*ayf_embedding_proxy_init)(const char* nm, jvxSize* idRegistered, ayfHostBindingReferences* retReferences, const char* fNameIni);
typedef jvxErrorType (*ayf_embedding_proxy_terminate)(jvxSize idRegistered);
typedef jvxErrorType (*ayf_embedding_proxy_host_set)(IjvxHost* hostRefOnInit);
typedef jvxErrorType (*ayf_embedding_proxy_host_reset)();

struct ayfEmbeddingProxyReferences
{
	ayf_embedding_proxy_init ayf_embedding_proxy_init_call = nullptr;
	ayf_embedding_proxy_terminate ayf_embedding_proxy_terminate_call = nullptr;
	ayf_embedding_proxy_host_set ayf_embedding_proxy_host_set_call = nullptr;
	ayf_embedding_proxy_host_reset ayf_embedding_proxy_host_reset_call = nullptr;
};

#endif
#endif
