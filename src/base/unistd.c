#include <ppr.h>
#include <ppr_int.h>

int ppr_gethostname(char* name, int namelen) {
#if defined(PPR_IS_PS2)
	if(namelen < 4) return -1;

	strcpy(name, "PS2");
	return 0;
#elif defined(PPR_IS_NETWARE)
	if(namelen < 8) return -1;

	strcpy(name, "netware");
	return 0;
#else
	return gethostname(name, namelen);
#endif
}

void ppr_msleep(int ms) {
#if defined(PPR_IS_WIN32)
	Sleep(ms);
#elif defined(PPR_IS_NETWARE)
	delay(ms);
#else
	struct timespec ts;

	ts.tv_sec  = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;

	nanosleep(&ts, NULL);
#endif
}
