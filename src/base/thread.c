#include <ppr.h>
#include <ppr_int.h>

typedef struct arg {
	void (*entry)(void* param);
	void* param;
#if defined(PPR_IS_NETWARE)
	LONG waitsem;
#endif
} arg_t;

#if defined(PPR_IS_NETWARE)
typedef struct thread {
	int  thread;
	LONG waitsem;
} thread_t;

struct ppr_thread_usage {
	ppr_bool  used;
	thread_t* thread;
};

struct ppr_semaphore_usage ppr_semaphores[1024];
struct ppr_thread_usage	   ppr_threads[1024];
#endif

#if defined(PPR_IS_NETWARE)
long ppr_thread_open_semaphore(long initial) {
	long sem = OpenLocalSemaphore(initial);
	int  i;

	for(i = 0; i < sizeof(ppr_semaphores) / sizeof(ppr_semaphores[0]); i++) {
		if(!ppr_semaphores[i].used) {
			ppr_semaphores[i].used = ppr_true;
			ppr_semaphores[i].sem  = sem;
			break;
		}
	}

	return sem;
}

void ppr_thread_close_semaphore(long sem) {
	int i;

	for(i = 0; i < sizeof(ppr_semaphores) / sizeof(ppr_semaphores[0]); i++) {
		if(ppr_semaphores[i].used && ppr_semaphores[i].sem == sem) {
			ppr_semaphores[i].used = ppr_false;
			break;
		}
	}

	CloseLocalSemaphore(sem);
}
#endif

void ppr_thread_init(void) {
#if defined(PPR_IS_NETWARE)
	int i;

	for(i = 0; i < sizeof(ppr_semaphores) / sizeof(ppr_semaphores[0]); i++) {
		ppr_semaphores[i].used = ppr_false;
	}
	for(i = 0; i < sizeof(ppr_threads) / sizeof(ppr_threads[0]); i++) {
		ppr_threads[i].used = ppr_false;
	}
#endif
}

void ppr_thread_uninit(void) {
#if defined(PPR_IS_NETWARE)
	int i;

	for(i = 0; i < sizeof(ppr_threads) / sizeof(ppr_threads[0]); i++) {
		if(ppr_threads[i].used) {
			ppr_thread_join(ppr_threads[i].thread);
			ppr_thread_destroy(ppr_threads[i].thread);
		}
	}
	for(i = 0; i < sizeof(ppr_semaphores) / sizeof(ppr_semaphores[0]); i++) {
		if(ppr_semaphores[i].used) {
			ppr_thread_close_semaphore(ppr_semaphores[i].sem);
		}
	}
#endif
}

#if defined(PPR_IS_WIN32)
#if defined(PPR_USE_CREATETHREAD)
static DWORD WINAPI thread_entry(void* _param) {
#else
static unsigned int WINAPI thread_entry(void* _param) {
#endif
	arg_t* arg		   = _param;
	void (*entry)(void* param) = arg->entry;
	void* param		   = arg->param;

	free(arg);

	entry(param);

#if defined(PPR_USE_CREATETHREAD)
	ExitThread(0);
#else
	_endthreadex(0);
#endif

	return 0;
}
#elif defined(PPR_USE_PTHREAD)
static void* thread_entry(void* _param) {
	arg_t* arg		   = _param;
	void (*entry)(void* param) = arg->entry;
	void* param		   = arg->param;

	free(arg);

	entry(param);

	pthread_exit(NULL);

	return NULL;
}
#elif defined(PPR_IS_NETWARE)
static void thread_entry(void* _param) {
	arg_t* arg		   = _param;
	void (*entry)(void* param) = arg->entry;
	void* param		   = arg->param;
	LONG  waitsem		   = arg->waitsem;

	free(arg);

	entry(param);

	SignalLocalSemaphore(waitsem);
	ExitThread(EXIT_THREAD, 0);
}
#endif

void* ppr_thread_create(void (*entry)(void* param), void* param) {
#if defined(PPR_IS_WIN32)
	arg_t* arg = malloc(sizeof(*arg));
#if defined(PPR_USE_CREATETHREAD)
	DWORD id;
#else
	unsigned int id;
#endif

	arg->entry = entry;
	arg->param = param;

	return (void*)
#if defined(PPR_USE_CREATETHREAD)
	    CreateThread
#else
	    _beginthreadex
#endif
	    (NULL, 0, thread_entry, arg, 0, &id);
#elif defined(PPR_USE_PTHREAD)
	pthread_t* t   = malloc(sizeof(*t));
	arg_t*	   arg = malloc(sizeof(*arg));

	arg->entry = entry;
	arg->param = param;

	pthread_create(t, NULL, thread_entry, arg);

	return t;
#elif defined(PPR_IS_NETWARE)
	thread_t* t   = malloc(sizeof(*t));
	arg_t*	  arg = malloc(sizeof(*arg));

	arg->entry   = entry;
	arg->param   = param;
	arg->waitsem = ppr_thread_open_semaphore(0);

	t->waitsem = arg->waitsem;
	t->thread  = BeginThread(thread_entry, NULL, 8192, arg);

	return t;
#else
	(void)entry;
	(void)param;

	return NULL;
#endif
}

void ppr_thread_join(void* handle) {
#if defined(PPR_IS_WIN32)
	WaitForSingleObject(handle, INFINITE);
#elif defined(PPR_USE_PTHREAD)
	void* ret;

	pthread_join(*(pthread_t*)handle, &ret);
#elif defined(PPR_IS_NETWARE)
	thread_t* t = handle;

	WaitOnLocalSemaphore(t->waitsem);
#else
	(void)handle;
#endif
}

void ppr_thread_destroy(void* handle) {
#if defined(PPR_IS_WIN32)
	CloseHandle(handle);
#elif defined(PPR_USE_PTHREAD)
	free(handle);
#elif defined(PPR_IS_NETWARE)
	thread_t* t = handle;
	int	  i;

	ppr_thread_close_semaphore(t->waitsem);

	for(i = 0; i < sizeof(ppr_threads) / sizeof(ppr_threads[0]); i++) {
		if(ppr_threads[i].used && ppr_threads[i].thread == handle) {
			ppr_threads[i].used = ppr_false;
			break;
		}
	}

	free(handle);
#else
	(void)handle;
#endif
}
