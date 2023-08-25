#ifndef __AYF_EMBEDDING_PROXY_ENTRIES_H__
#define __AYF_EMBEDDING_PROXY_ENTRIES_H__

extern "C"
{
	jvxErrorType ayf_embedding_proxy_init(const char* nm, jvxSize* idRegistered, ayfHostBindingReferences* retReferences, const char* fNameIni);
	jvxErrorType ayf_embedding_proxy_terminate(jvxSize idRegistered);

	jvxErrorType ayf_embedding_proxy_host_set(IjvxHost* hostRefOnInit);
	jvxErrorType ayf_embedding_proxy_host_reset();
};

#endif
