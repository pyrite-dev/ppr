#include <ppr.h>
#include <ppr_int.h>

char* ppr_strdup(const char* str) {
	char* r = malloc(strlen(str) + 1);

	strcpy(r, str);

	return r;
}

char* ppr_strvacat(const char* a, ...) {
	int	l = 1 + strlen(a);
	va_list va;
	char*	s;
	char*	r;

	va_start(va, a);
	while((s = va_arg(va, char*)) != NULL) {
		l += strlen(s);
	}
	va_end(va);

	r = malloc(l);

	strcpy(r, a);

	va_start(va, a);
	while((s = va_arg(va, char*)) != NULL) {
		strcat(r, s);
	}
	va_end(va);

	return r;
}

void ppr_strappend(char** dst, const char* src) {
	char* n = ppr_strvacat(*dst, src, NULL);

	free(*dst);
	*dst = n;
}

char* ppr_strsafehtml(const char* s) {
	char* r = malloc(strlen(s) * 5 + 1);
	int   i;
	int   n = 0;

	for(i = 0; s[i] != 0; i++) {
		if(s[i] == '&') {
			r[n++] = '&';
			r[n++] = 'a';
			r[n++] = 'm';
			r[n++] = 'p';
			r[n++] = ';';
		} else if(s[i] == '<') {
			r[n++] = '&';
			r[n++] = 'l';
			r[n++] = 't';
			r[n++] = ';';
		} else if(s[i] == '>') {
			r[n++] = '&';
			r[n++] = 'g';
			r[n++] = 't';
			r[n++] = ';';
		} else {
			r[n++] = s[i];
		}
	}
	r[n] = 0;

	return r;
}
