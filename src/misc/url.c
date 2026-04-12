#include <ppr.h>
#include <ppr_int.h>

static int hex(const char in) {
	if('0' <= in && in <= '9') return in - '0';
	if('a' <= in && in <= 'f') return in - 'a' + 10;
	if('A' <= in && in <= 'F') return in - 'A' + 10;

	return 0;
}

ppr_bool ppr_url_decode(char* out, const char* input, int len) {
	int i;

	for(i = 0; input[i] != 0; i++) {
		if((int)strlen(out) == len) {
			return ppr_false;
		} else if(input[i] == '%') {
			if(strlen(input + i) >= 3) {
				int n	   = strlen(out);
				out[n]	   = (hex(input[i + 1]) << 4) | (hex(input[i + 2]));
				out[n + 1] = 0;

				i += 2;
			}
		} else {
			int n	   = strlen(out);
			out[n]	   = input[i];
			out[n + 1] = 0;
		}
	}

	return ppr_true;
}

ppr_bool ppr_url_encode(char* out, const char* input, int len) {
	int i;

	for(i = 0; input[i] != 0; i++) {
		if(strlen(out) == len) {
			return ppr_false;
		} else if(input[i] == '%') {
			if(strlen(input + i) >= 3 && strlen(out) <= (len - 3)) {
				int n = strlen(out);

				out[n]	   = '%';
				out[n + 1] = '2';
				out[n + 2] = '5';
				out[n + 3] = 0;
			}
		} else {
			int n	   = strlen(out);
			out[n]	   = input[i];
			out[n + 1] = 0;
		}
	}

	return ppr_true;
}

void ppr_url_init(ppr_url_t* url) {
	memset(url, 0, sizeof(*url));
}

enum STATE {
	USERINFO = 0,
	HOST,
	PORT,
	PATH,
	QUERY,
	FRAGMENT
};

ppr_bool ppr_url_parse(ppr_url_t* url, const char* str) {
	const char* st; /* tmp */
	const char* sp; /* pointer */
	ppr_bool    has_userinfo = ppr_false;
	int	    state	 = 0;
	ppr_bool    br		 = ppr_false;

	if((st = strstr(str, "://")) == NULL) return ppr_false;

	for(sp = str; sp != st; sp++) {
		char c = *sp;

		if('A' <= c && c <= 'Z') continue;
		if('a' <= c && c <= 'z') continue;
		if(sp == str) {
			ppr_url_deinit(url);
			return ppr_false;
		}

		if('0' <= c && c <= '9') continue;
		if(c == '+' || c == '-' || c == '.') continue;

		ppr_url_deinit(url);
		return ppr_false;
	}

	url->scheme = malloc(sp - str + 1);
	memcpy(url->scheme, str, sp - str);
	url->scheme[sp - str] = 0;

	str = st + 3;

	if((st = strchr(str, '/')) == NULL) st = str + strlen(str);

	for(sp = str; sp != st; sp++) {
		if((*sp) == '@') {
			has_userinfo = ppr_true;
			break;
		}
	}

	if(has_userinfo) {
		state = USERINFO;
	} else {
		state = HOST;
	}

	for(sp = str;; sp++) {
		char c = *sp;

		if(state == USERINFO && c == '@') {
			url->userinfo = malloc(sp - str + 1);
			memcpy(url->userinfo, str, sp - str);
			url->userinfo[sp - str] = 0;

			str = sp + 1;

			state = HOST;
		} else if(state == HOST && !br && c == '[') {
			br = ppr_true;
		} else if(state == HOST && br && c == ']') {
			br = ppr_false;
		} else if(state == HOST && !br && (c == 0 || c == ':' || c == '/')) {
			url->host = malloc(sp - str + 1);
			memcpy(url->host, str, sp - str);
			url->host[sp - str] = 0;

			if(c == 0) {
				break;
			} else if(c == ':') {
				str = sp + 1;

				state = PORT;
			} else {
				str = sp;

				state = PATH;
			}
		} else if(state == PORT && (c == 0 || c == '/')) {
			char* p = malloc(sp - str + 1);
			memcpy(p, str, sp - str);
			p[sp - str] = 0;

			url->port = atoi(p);

			free(p);

			if(c == 0) {
				break;
			} else {
				str = sp; /* intended */

				state = PATH;
			}
		} else if(state == PATH && (c == 0 || c == '?' || c == '#')) {
			url->path = malloc(sp - str + 1);
			memcpy(url->path, str, sp - str);
			url->path[sp - str] = 0;

			str = sp + 1;

			if(c == 0) {
				break;
			} else if(c == '?') {
				state = QUERY;
			} else {
				state = FRAGMENT;
			}
		} else if(state == QUERY && (c == 0 || c == '#')) {
			url->query = malloc(sp - str + 1);
			memcpy(url->query, str, sp - str);
			url->query[sp - str] = 0;

			str = sp + 1;

			if(c == 0) {
				break;
			} else {
				state = FRAGMENT;
			}
		} else if(state == FRAGMENT && c == 0) {
			url->fragment = malloc(sp - str + 1);
			memcpy(url->fragment, str, sp - str);
			url->fragment[sp - str] = 0;

			break;
		} else if(c == 0) {
			ppr_url_deinit(url);
			return ppr_false;
		}
	}

	return ppr_true;
}

void ppr_url_deinit(ppr_url_t* url) {
	if(url->scheme != NULL) free(url->scheme);
	if(url->userinfo != NULL) free(url->userinfo);
	if(url->host != NULL) free(url->host);
	if(url->path != NULL) free(url->path);
	if(url->query != NULL) free(url->query);
	if(url->fragment != NULL) free(url->fragment);

	memset(url, 0, sizeof(*url));
}
