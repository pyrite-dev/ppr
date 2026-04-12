#include <ppr.h>
#include <ppr_int.h>

ppr_bool ppr_wildcard(const char* wildcard, const char* target) {
	const char *pw = wildcard, *pt = target;

	while(1) {
		if(*pt == 0) {
			while(*pw == '*') pw++;
			return *pw == 0;
		} else if(*pw == 0) {
			return 0;
		} else if(*pw == '*') {
			return *(pw + 1) == 0 || ppr_wildcard(pw, pt + 1) || ppr_wildcard(pw + 1, pt);
		} else if(*pw == '?' || (tolower((int)*pw) == tolower((int)*pt))) {
			pw++;
			pt++;
			continue;
		} else {
			return 0;
		}
	}
}
