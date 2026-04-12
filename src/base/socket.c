#include <ppr.h>
#include <ppr_int.h>

#if !defined(IPPROTO_TCP)
#define IPPROTO_TCP 0
#endif

#if !defined(IPPROTO_UDP)
#define IPPROTO_UDP 0
#endif

#if defined(PPR_IS_NETWARE)
struct in_addr {
	ppr_uint32_t s_addr;
};
struct sockaddr_in {
	ppr_uint16_t   sin_family;
	ppr_uint16_t   sin_port;
	struct in_addr sin_addr;
	ppr_uint8_t    sin_zero[8];
};
#endif

void ppr_socket_init(void) {
#if defined(PPR_IS_WIN32)
	WSADATA wsa;

	WSAStartup(MAKEWORD(2, 0), &wsa);
#elif defined(PPR_IS_UNIX)
	signal(SIGPIPE, SIG_IGN);
#endif
}

void ppr_socket_uninit(void) {
#if defined(PPR_IS_WIN32)
	WSACleanup();
#elif defined(PPR_IS_UNIX)
#endif
}

int ppr_socket(int domain, int type, int protocol) {
	int d = PF_UNSPEC, t = 0, p = 0;
	int s;
	int nbyt;

	if(domain == PPR_PF_INET) {
		d = PF_INET;
#if defined(PPR_HAS_IPV6)
	} else if(domain == PPR_PF_INET6) {
		d = PF_INET6;
#endif
#if defined(PPR_HAS_UNIX_SOCKET)
	} else if(domain == PPR_PF_UNIX) {
		d = PF_UNIX;
#endif
	}

	if(type == PPR_SOCK_STREAM) {
		t = SOCK_STREAM;
	} else if(type == PPR_SOCK_DGRAM) {
		t = SOCK_DGRAM;
	}

	if(protocol == PPR_IPPROTO_TCP) {
		p = IPPROTO_TCP;
	} else if(protocol == PPR_IPPROTO_UDP) {
		p = IPPROTO_UDP;
	}

	s = socket(d, t, p);

#if defined(PPR_IS_WIN32)
	if(s == INVALID_SOCKET) s = -1;
#endif

	if(s >= 0 && t == SOCK_STREAM && p == IPPROTO_TCP) {
		int yes = 1;
		setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void*)&yes, sizeof(yes));
	}

#if defined(PPR_HAS_IPV6)
	if(s >= 0 && d == PF_INET6) {
		int yes = 1;
		setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&yes, sizeof(yes));
	}
#endif

	if(s >= 0) {
		nbyt = 65535;
		setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&nbyt, sizeof(nbyt));
		nbyt = 65535;
		setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&nbyt, sizeof(nbyt));
	}

	return s;
}

/* we ignore flags for now */
int ppr_recv(int s, void* buf, int len, int flags) {
	(void)flags;

	return recv(s, buf, len, 0);
}

int ppr_send(int s, const void* msg, int len, int flags) {
	(void)flags;

	return send(s, (void*)msg, len, 0);
}

static struct sockaddr* conv_to_sa(int* outlen, const struct ppr_sockaddr* input, int namelen) {
	struct sockaddr* out = NULL;

	if(input->sa_family == PPR_AF_INET && namelen == sizeof(struct ppr_sockaddr_in)) {
		struct ppr_sockaddr_in* addr = (struct ppr_sockaddr_in*)input;
		struct sockaddr_in	addr4;

		addr4.sin_family      = AF_INET;
		addr4.sin_addr.s_addr = addr->sin_addr.u.addr32[0];
		addr4.sin_port	      = addr->sin_port;

		out = malloc(sizeof(addr4));
		memcpy(out, &addr4, sizeof(addr4));
		*outlen = sizeof(addr4);
#if defined(PPR_HAS_IPV6)
	} else if(input->sa_family == PPR_AF_INET6 && namelen == sizeof(struct ppr_sockaddr_in6)) {
		struct ppr_sockaddr_in6* addr = (struct ppr_sockaddr_in6*)input;
		struct sockaddr_in6	 addr6;

		addr6.sin6_family = AF_INET6;
		memcpy(addr6.sin6_addr.s6_addr, &addr->sin6_addr.u.addr8, 16);
		addr6.sin6_port = addr->sin6_port;

		out = malloc(sizeof(addr6));
		memcpy(out, &addr6, sizeof(addr6));
		*outlen = sizeof(addr6);
#endif
#if defined(PPR_HAS_UNIX_SOCKET)
	} else if(input->sa_family == PPR_AF_UNIX && namelen == sizeof(struct ppr_sockaddr_un)) {
		struct ppr_sockaddr_un* addr = (struct ppr_sockaddr_un*)input;
		struct sockaddr_un	addru;

		addru.sun_family = AF_UNIX;
		strcpy(addru.sun_path, addr->sun_path);

		out = malloc(sizeof(addru));
		memcpy(out, &addru, sizeof(addru));
		*outlen = sizeof(addru);
#endif
	}

	return out;
}

int ppr_bind(int s, const struct ppr_sockaddr* name, int namelen) {
	int		 st  = -1;
	int		 yes = 1;
	struct sockaddr* sa;
	int		 len;

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes));
	if((sa = conv_to_sa(&len, name, namelen)) != NULL) {
		st = bind(s, sa, len);
		free(sa);
	}

	return st;
}

int ppr_connect(int s, const struct ppr_sockaddr* name, int namelen) {
	int		 st = -1;
	struct sockaddr* sa;
	int		 len;

	if((sa = conv_to_sa(&len, name, namelen)) != NULL) {
		st = connect(s, sa, len);
		free(sa);
	}

	return st;
}

int ppr_listen(int s, int backlog) {
	return listen(s, backlog);
}

int ppr_accept(int s, struct ppr_sockaddr* addr, int* addrlen) {
	unsigned char	 buffer[256];
	struct sockaddr* sa = (struct sockaddr*)buffer;
	int		 r;
#if defined(PPR_USE_SOCKLEN_T)
	socklen_t l
#else
	int l
#endif
	    = *addrlen;

	r	 = accept(s, sa, &l);
	*addrlen = l;

#if defined(PPR_IS_WIN32)
	if(r == INVALID_SOCKET) r = -1;
#endif

	if(r < 0) return r;

	if(sa->sa_family == AF_INET) {
		struct ppr_sockaddr_in* taddr = (struct ppr_sockaddr_in*)addr;
		struct sockaddr_in*	addr4 = (struct sockaddr_in*)buffer;

		taddr->sin_family	    = PPR_AF_INET;
		taddr->sin_addr.u.addr32[0] = addr4->sin_addr.s_addr;
		taddr->sin_port		    = addr4->sin_port;

		*addrlen = sizeof(*taddr);
#if defined(PPR_HAS_IPV6)
	} else if(sa->sa_family == AF_INET6) {
		struct ppr_sockaddr_in6* taddr = (struct ppr_sockaddr_in6*)addr;
		struct sockaddr_in6*	 addr6 = (struct sockaddr_in6*)buffer;

		taddr->sin6_family = PPR_AF_INET6;
		memcpy(taddr->sin6_addr.u.addr32, addr6->sin6_addr.s6_addr, 16);
		taddr->sin6_port = addr6->sin6_port;

		*addrlen = sizeof(*taddr);
#endif
#if defined(PPR_HAS_UNIX_SOCKET)
	} else if(sa->sa_family == AF_UNIX) {
		struct ppr_sockaddr_un* taddr = (struct ppr_sockaddr_un*)addr;
		struct sockaddr_un*	addru = (struct sockaddr_un*)buffer;

		taddr->sun_family = PPR_AF_UNIX;
		strcpy(taddr->sun_path, addru->sun_path);

		*addrlen = sizeof(*taddr);
#endif
	}

	return r;
}

void ppr_socket_close(int d) {
#if defined(PPR_IS_WIN32)
	shutdown(d, SD_BOTH);
	closesocket(d);
#elif defined(PPR_IS_UNIX)
	shutdown(d, SHUT_RD);
	close(d);
#endif
}

ppr_bool ppr_socket_has_ipv6(void) {
#if defined(PPR_HAS_IPV6)
#if defined(PPR_IS_WIN32)
	DWORD v = GetVersion();
	DWORD majorv, minorv;

	majorv = LOBYTE(LOWORD(v));
	minorv = HIBYTE(LOWORD(v));

	if(majorv > 5 && (majorv == 5 && minorv >= 1)) return ppr_true;

	return ppr_false;
#else
	return ppr_true;
#endif
#else
	return ppr_false;
#endif
}

struct ppr_in_addr ppr_inaddr_any = {
    {{0, 0, 0, 0}}};

struct ppr_in6_addr ppr_in6addr_any = {
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};
