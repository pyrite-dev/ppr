#ifndef __PPR_URL_H__
#define __PPR_URL_H__

#include <ppr_machdep.h>

typedef struct ppr_url ppr_url_t;

struct ppr_url {
	char* scheme;
	char* userinfo;
	char* host;
	int   port;
	char* path;
	char* query;
	char* fragment;
};

ppr_bool ppr_url_decode(char* out, const char* input, int len);
ppr_bool ppr_url_encode(char* out, const char* input, int len);

void	 ppr_url_init(ppr_url_t* url);
ppr_bool ppr_url_parse(ppr_url_t* url, const char* str);
void	 ppr_url_deinit(ppr_url_t* url);

#endif
