#include <ppr.h>
#include <ppr_int.h>

ppr_uint16_t ppr_htons(ppr_uint16_t host16) {
#if defined(PPR_IS_NETWARE)
	unsigned char buf[2];

	buf[0] = (host16 >> 8) & 0xff;
	buf[1] = (host16 >> 0) & 0xff;

	return *(ppr_uint16_t*)buf;
#else
	return htons(host16);
#endif
}

static int sort_v6(const void* _a, const void* _b) {
	int* a = (int*)_a;
	int* b = (int*)_b;

	if(a[1] > b[1]) return -1;
	if(a[1] < b[1]) return 1;

	return 0;
}

const char* ppr_inet_ntop(struct ppr_sockaddr* src, char* dst) {
	if(src->sa_family == PPR_AF_INET) {
		struct ppr_sockaddr_in* addr = (struct ppr_sockaddr_in*)src;
		int			i;

		dst[0] = 0;

		for(i = 0; i < 4; i++) {
			if(i > 0) strcat(dst, ".");
			sprintf(dst + strlen(dst), "%d", (int)addr->sin_addr.u.addr8[i]);
		}
		return dst;
	} else if(src->sa_family == PPR_AF_INET6) {
		struct ppr_sockaddr_in6* addr = (struct ppr_sockaddr_in6*)src;
		int			 i;
		char			 v6[8][5];
		int			 l[8][2]; /* index, count */
		int			 c   = -1;
		int			 z   = 0;
		int			 max = 8;

		dst[0] = 0;

		for(i = 0; i < 16; i += 2) {
			v6[i / 2][0] = 0;

			if(addr->sin6_addr.u.addr8[i + 0] > 0) {
				sprintf(v6[i / 2], "%x", (int)addr->sin6_addr.u.addr8[i + 0]);
			}
			if(addr->sin6_addr.u.addr8[i + 0] > 0 && !(addr->sin6_addr.u.addr8[i + 1] & 0xf0)) strcat(v6[i / 2], "0");
			sprintf(v6[i / 2] + strlen(v6[i / 2]), "%x", (int)addr->sin6_addr.u.addr8[i + 1]);
		}

		for(i = 0; i < 8; i++) l[i][0] = l[i][1] = 0;

		for(i = 7; i >= 0; i--) {
			if(z && strcmp(v6[i], "0") == 0) {
				l[c][0] = i;
				l[c][1]++;
				continue;
			} else if(z) {
				z = 0;
			}

			if(strcmp(v6[i], "0") == 0) {
				z = 1;

				c++;

				l[c][0] = i;
				l[c][1] = 1;
			} else {
			}
		}

		qsort(l, 8, sizeof(int) * 2, sort_v6);

		dst[0] = 0;
		for(i = 0; i < max; i++) {
			if(i > 0 || l[0][1] == 8) strcat(dst, ":");
			if(i == l[0][0] && l[0][1] > 0) {
				if(i == 0 || (i + l[0][1] - 1) == (max - 1)) strcat(dst, ":");

				i += l[0][1] - 1;
			} else {
				strcat(dst, v6[i]);
			}
		}

		if(memcmp(dst, "::ffff:", 7) == 0) {
			struct ppr_sockaddr_in in;

			dst[7] = 0;

			in.sin_family = PPR_AF_INET;
			memcpy(&in.sin_addr, &addr->sin6_addr.u.addr8[12], 4);

			ppr_inet_ntop((struct ppr_sockaddr*)&in, dst + 7);
		}

		return dst;
	}

	return NULL;
}

struct ppr_sockaddr* ppr_inet_addr(const char* addr, int* len) {
	int	    d = 0;
	int	    i;
	const char* v4[4];

	v4[0] = addr;

	for(i = 0; addr[i] != 0; i++) {
		if(addr[i] == '.') {
			d++;

			v4[d] = &addr[i + 1];
		} else if('0' <= addr[i] && addr[i] <= '9') {
		} else {
			break;
		}
	}

	/* IPv4 */
	if(d == 3 && addr[i] == 0) {
		struct ppr_sockaddr_in* in = malloc(sizeof(*in));

		in->sin_family = PPR_AF_INET;
		for(i = 0; i < 4; i++) in->sin_addr.u.addr8[i] = atoi(v4[i]);

		*len = sizeof(*in);

		return (struct ppr_sockaddr*)in;
	}

	for(i = 0; addr[i] != 0; i++) {
		if(addr[i] == '[' && i == 0) {
		} else if(addr[i] == ']' && i == (strlen(addr) - 1)) {
		} else if(addr[i] == ':') {
		} else if('0' <= addr[i] && addr[i] <= '9') {
		} else if('a' <= addr[i] && addr[i] <= 'f') {
		} else if('A' <= addr[i] && addr[i] <= 'F') {
		} else {
			break;
		}
	}

	/* IPv6 */
	if(addr[i] == 0) {
		const char*		 b    = addr;
		int			 e    = 0;
		struct ppr_sockaddr_in6* in6  = malloc(sizeof(*in6));
		int			 incr = 0;

		in6->sin6_family = PPR_AF_INET6;

		memset(in6->sin6_addr.u.addr8, 0, 16);

		for(i = 0;; i++) {
			if(addr[i] == 0 || addr[i] == ']' || addr[i] == ':') {
				char* n = malloc(&addr[i] - b + 1);

				n[&addr[i] - b] = 0;
				memcpy(n, b, &addr[i] - b);

				if(strlen(n) == 0) {
					e++;
				} else {
					int j;
					int f	= strlen(n) - 1;
					int old = incr;
					int inc = 0;

					e = 0;

					incr++;
					for(j = f; j >= 0; j--) {
						char byte = n[j];

						if('0' <= byte && byte <= '9') {
							byte = byte - '0';
						} else if('a' <= byte && byte <= 'f') {
							byte = byte - 'a' + 10;
						} else if('A' <= byte && byte <= 'F') {
							byte = byte - 'A' + 10;
						}

						in6->sin6_addr.u.addr8[incr] = in6->sin6_addr.u.addr8[incr] | (byte << (4 * inc));
						inc++;

						if(j == (f - 1)) {
							incr--;
							inc = 0;
						}
					}

					incr = old + 2;

					if(incr > 16) {
						free(n);
						free(in6);
						return NULL;
					}
				}

				free(n);

				if(e == 2) {
					int len = 16 - incr;
					int c	= 0;
					int j;

					for(j = i + 1; addr[j] != ']' && addr[j] != 0; j++) {
						if(j == (i + 1) && !(addr[j] == ']' || addr[j] == 0)) c++;
						if(addr[j] == ':') c++;
					}

					incr += len - c * 2;

					if(incr > 16) {
						free(in6);
						return NULL;
					}
				}

				b = &addr[i + 1];

				if(addr[i] == 0 || addr[i] == ']') break;
			} else if(addr[i] == '[') {
				b = &addr[i + 1];
			}
		}

		*len = sizeof(*in6);

		return (struct ppr_sockaddr*)in6;
	}

	return NULL;
}
