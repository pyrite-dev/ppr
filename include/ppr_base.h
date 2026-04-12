#ifndef __PPR_BASE_H__
#define __PPR_BASE_H__

#include <ppr_machdep.h>

/* poll.c definitions */
#define PPR_POLLIN (1 << 0)
#define PPR_POLLPRI (1 << 1)
#define PPR_POLLOUT (1 << 2)

struct ppr_pollfd {
	int   fd;
	short events;
	short revents;

	void* user;
};

/* file.c definitions */
typedef void PPR_FILE;

/* socket.c definitions */
enum ppr_socket_protocol {
	PPR_PF_UNSPEC = 0,
	PPR_PF_INET,
	PPR_PF_INET6,
	PPR_PF_UNIX
};

enum ppr_socket_address {
	PPR_AF_UNSPEC = 0,
	PPR_AF_INET,
	PPR_AF_INET6,
	PPR_AF_UNIX
};

enum ppr_socket_type {
	PPR_SOCK_STREAM = 0,
	PPR_SOCK_DGRAM
};

enum ppr_socket_ip_protocol {
	PPR_IPPROTO_TCP = 1,
	PPR_IPPROTO_UDP
};

struct ppr_sockaddr {
	unsigned short sa_family;
	char	       sa_data[14];
};

union ppr_in_addr_union {
	ppr_uint8_t  addr8[4];
	ppr_uint16_t addr16[2];
	ppr_uint32_t addr32[1];
};

struct ppr_in_addr {
	union ppr_in_addr_union u;
};

struct ppr_sockaddr_in {
	unsigned short	   sin_family;
	ppr_uint16_t	   sin_port;
	struct ppr_in_addr sin_addr;
};

union ppr_in6_addr_union {
	ppr_uint8_t  addr8[16];
	ppr_uint16_t addr16[8];
	ppr_uint32_t addr32[4];
};

struct ppr_in6_addr {
	union ppr_in6_addr_union u;
};

struct ppr_sockaddr_in6 {
	unsigned short	    sin6_family;
	ppr_uint16_t	    sin6_port;
	struct ppr_in6_addr sin6_addr;
};

struct ppr_sockaddr_storage {
	unsigned short ss_family;
	char	       ss_pad[128];
};

struct ppr_sockaddr_un {
	unsigned short sun_family;
	char	       sun_path[104];
};

/* stat.c definitions */
#define PPR_S_IFREG (1 << 0)
#define PPR_S_IFDIR (1 << 1)
#define PPR_S_ISREG(x) ((x) & PPR_S_IFREG)
#define PPR_S_ISDIR(x) ((x) & PPR_S_IFDIR)

struct ppr_stat {
	ppr_size_t st_size;
	ppr_time_t st_modtime;
	int	   st_mode;
};

/* dirent.c definitions */
typedef void PPR_DIR;

struct ppr_dirent {
	char		d_name[512];
	char		d_fullname[1024];
	struct ppr_stat d_stat;
};

/* time.c definitions */
struct ppr_tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

/* thread.c definitions */
#if defined(PPR_IS_NETWARE)
struct ppr_semaphore_usage {
	long	 sem;
	ppr_bool used;
};
#endif

/* core.c */
void ppr_init(void);
void ppr_uninit(void);

/* poll.c */
int ppr_poll(struct ppr_pollfd* fds, int nfds, int timeout);

/* file.c */
PPR_FILE* ppr_fopen(const char* path, const char* mode);
int	  ppr_fread(void* ptr, int size, int nmemb, PPR_FILE* stream);
int	  ppr_fwrite(const void* ptr, int size, int nmemb, PPR_FILE* stream);
void	  ppr_fclose(PPR_FILE* stream);

/* socket.c */
extern struct ppr_in_addr  ppr_inaddr_any;
extern struct ppr_in6_addr ppr_in6addr_any;

void	 ppr_socket_init(void);
int	 ppr_socket(int domain, int type, int protocol);
int	 ppr_recv(int s, void* buf, int len, int flags);
int	 ppr_send(int s, const void* msg, int len, int flags);
int	 ppr_bind(int s, const struct ppr_sockaddr* name, int namelen);
int	 ppr_connect(int s, const struct ppr_sockaddr* name, int namelen);
int	 ppr_listen(int s, int backlog);
int	 ppr_accept(int s, struct ppr_sockaddr* addr, int* addrlen);
void	 ppr_socket_close(int d);
void	 ppr_socket_uninit(void);
ppr_bool ppr_socket_has_ipv6(void);

/* arpa.c */
ppr_uint16_t	     ppr_htons(ppr_uint16_t host16);
const char*	     ppr_inet_ntop(struct ppr_sockaddr* src, char* dst);
struct ppr_sockaddr* ppr_inet_addr(const char* addr, int* len);

/* string.c */
char* ppr_strdup(const char* str);
char* ppr_strvacat(const char* a, ...);
void  ppr_strappend(char** dst, const char* src);
char* ppr_strsafehtml(const char* s);

/* dlfcn.c */
void* ppr_dlopen(const char* path);
void* ppr_dlsym(void* handle, const char* symbol);
int   ppr_dlclose(void* handle);

/* unistd.c */
int  ppr_gethostname(char* name, int namelen);
void ppr_msleep(int ms);

/* stat.c */
int ppr_stat(const char* path, struct ppr_stat* s);

/* thread.c */
#if defined(PPR_IS_NETWARE)
extern struct ppr_semaphore_usage ppr_semaphores[];

long ppr_thread_open_semaphore(long initial);
void ppr_thread_close_semaphore(long sem);
#endif

void* ppr_thread_create(void (*entry)(void* param), void* param);
void  ppr_thread_join(void* handle);
void  ppr_thread_destroy(void* handle);
void  ppr_thread_init(void);
void  ppr_thread_uninit(void);

/* mutex.c */
void* ppr_mutex_create(void);
void  ppr_mutex_lock(void* handle);
void  ppr_mutex_unlock(void* handle);
void  ppr_mutex_destroy(void* handle);

/* process.c */
void* ppr_process_create(const char* exec, char** env);
void  ppr_process_close(void* handle);
int   ppr_process_write(void* handle, const void* data, int len);
int   ppr_process_read(void* handle, void* data, int len);
void  ppr_process_destroy(void* handle);

/* dirent.c */
PPR_DIR*	   ppr_opendir(const char* path);
struct ppr_dirent* ppr_readdir(PPR_DIR* handle);
void		   ppr_closedir(PPR_DIR* handle);
int		   ppr_scandir(const char* dirname, struct ppr_dirent*** namelist, int (*selectfn)(const struct ppr_dirent* d), int (*compar)(const struct ppr_dirent** d1, const struct ppr_dirent** d2));
int		   ppr_alphasort(const struct ppr_dirent** d1, const struct ppr_dirent** d2);

/* time.c */
extern void* ppr_gmtime_mutex;
extern void* ppr_localtime_mutex;

ppr_time_t ppr_time(void);
void	   ppr_gmtime(struct ppr_tm* tm, ppr_time_t t);
void	   ppr_localtime(struct ppr_tm* tm, ppr_time_t t);

#endif
