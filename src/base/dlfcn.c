#include <ppr.h>
#include <ppr_int.h>

void* ppr_dlopen(const char* path) {
#if defined(PPR_IS_WIN32)
	return LoadLibrary(path);
#elif defined(PPR_IS_UNIX)
	return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
#elif defined(PPR_IS_NETWARE)
	unsigned int handle = FindNLMHandle(path);

	if(handle == 0) {
		spawnlp(P_NOWAIT | P_SPAWN_IN_CURRENT_DOMAIN, path, NULL);
		handle = FindNLMHandle(path);
	}

	return (void*)handle;
#else
	return NULL;
#endif
}

void* ppr_dlsym(void* handle, const char* symbol) {
#if defined(PPR_IS_WIN32)
	return GetProcAddress(handle, symbol);
#elif defined(PPR_IS_UNIX)
	return dlsym(handle, symbol);
#elif defined(PPR_IS_NETWARE)
	return ImportSymbol((unsigned int)handle, symbol);
#else
	return NULL;
#endif
}

int ppr_dlclose(void* handle) {
#if defined(PPR_IS_WIN32)
	return FreeLibrary(handle) ? 0 : 1;
#elif defined(PPR_IS_UNIX)
	return dlclose(handle);
#elif defined(PPR_IS_NETWARE)
	return 0;
#else
	return -1;
#endif
}
