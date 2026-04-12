#include <ppr.h>
#include <ppr_int.h>

void* ppr_gmtime_mutex;
void* ppr_localtime_mutex;

ppr_time_t ppr_time(void) {
	time_t t = time(NULL);

	return t;
}

static void tm_to_ppr(struct ppr_tm* to, struct tm* from) {
	to->tm_sec   = from->tm_sec;
	to->tm_min   = from->tm_min;
	to->tm_hour  = from->tm_hour;
	to->tm_mday  = from->tm_mday;
	to->tm_mon   = from->tm_mon;
	to->tm_year  = from->tm_year;
	to->tm_wday  = from->tm_wday;
	to->tm_yday  = from->tm_yday;
	to->tm_isdst = from->tm_isdst;
}

void ppr_gmtime(struct ppr_tm* tm, ppr_time_t t) {
	struct tm from_tm;
	time_t	  from = t;

	ppr_mutex_lock(ppr_gmtime_mutex);
	from_tm = *gmtime(&from);
	ppr_mutex_unlock(ppr_gmtime_mutex);

	tm_to_ppr(tm, &from_tm);
}

void ppr_localtime(struct ppr_tm* tm, ppr_time_t t) {
	struct tm from_tm;
	time_t	  from = t;

	ppr_mutex_lock(ppr_localtime_mutex);
	from_tm = *localtime(&from);
	ppr_mutex_unlock(ppr_localtime_mutex);

	tm_to_ppr(tm, &from_tm);
}
