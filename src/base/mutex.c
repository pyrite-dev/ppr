#include <ppr.h>
#include <ppr_int.h>

void* ppr_mutex_create(void) {
#if defined(PPR_IS_WIN32)
	return CreateEvent(NULL, FALSE, TRUE, NULL);
#elif defined(PPR_USE_PTHREAD)
	pthread_mutex_t* mutex = malloc(sizeof(*mutex));

	pthread_mutex_init(mutex, NULL);

	return mutex;
#elif defined(PPR_IS_NETWARE)
	LONG* mutex = malloc(sizeof(*mutex));

	*mutex = ppr_thread_open_semaphore(1);

	return mutex;
#else
	return NULL;
#endif
}

void ppr_mutex_lock(void* handle) {
#if defined(PPR_IS_WIN32)
	WaitForSingleObject(handle, INFINITE);
#elif defined(PPR_USE_PTHREAD)
	pthread_mutex_lock(handle);
#elif defined(PPR_IS_NETWARE)
	WaitOnLocalSemaphore(*(LONG*)handle);
#else
	(void)handle;
#endif
}

void ppr_mutex_unlock(void* handle) {
#if defined(PPR_IS_WIN32)
	SetEvent(handle);
#elif defined(PPR_USE_PTHREAD)
	pthread_mutex_unlock(handle);
#elif defined(PPR_IS_NETWARE)
	SignalLocalSemaphore(*(LONG*)handle);
#else
	(void)handle;
#endif
}

void ppr_mutex_destroy(void* handle) {
#if defined(PPR_IS_WIN32)
	CloseHandle(handle);
#elif defined(PPR_USE_PTHREAD)
	pthread_mutex_destroy(handle);

	free(handle);
#elif defined(PPR_IS_NETWARE)
	ppr_thread_close_semaphore(*(LONG*)handle);

	free(handle);
#else
	(void)handle;
#endif
}
