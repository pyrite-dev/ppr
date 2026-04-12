#include <ppr.h>
#include <ppr_int.h>

void ppr_init(void) {
	ppr_socket_init();
	ppr_thread_init();

#if defined(PPR_IS_NETWARE)
	SetCurrentNameSpace(NW_NS_LONG);
#endif

	ppr_gmtime_mutex    = ppr_mutex_create();
	ppr_localtime_mutex = ppr_mutex_create();
}

void ppr_uninit(void) {
	ppr_mutex_destroy(ppr_localtime_mutex);
	ppr_mutex_destroy(ppr_gmtime_mutex);

	ppr_thread_uninit();
	ppr_socket_uninit();
}
