#ifndef __PPR_INT_H__
#define __PPR_INT_H__

/* generic section */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>

#if defined(PPR_IS_NETWARE)
#include <sys/stat.h>

#include <nwnamspc.h>
#include <nwadv.h>
#endif

#if defined(PPR_IS_UNIX)
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dlfcn.h>
#endif

#if defined(PPR_IS_UNIX) || defined(PPR_IS_PSP) || defined(PPR_IS_PS2) || defined(PPR_IS_NETWARE)
#include <unistd.h>
#include <dirent.h>
#endif

/* thread section */
#if defined(PPR_IS_WIN32)
#if !defined(PPR_USE_CREATETHREAD)
#include <process.h>
#endif
#elif defined(PPR_IS_UNIX) || defined(PPR_IS_PSP) || defined(PPR_IS_PS2)
#include <pthread.h>
#define PPR_USE_PTHREAD
#elif defined(PPR_IS_NETWARE)
#include <nwthread.h>
#include <nwsemaph.h>
#endif

/* socket section */
#if defined(PPR_IS_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>

#ifndef IPPROTO_IPV6
#define IPPROTO_IPV6 41
#endif

#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27
#endif
#else
#if defined(PPR_HAS_POLL)
#include <poll.h>
#else

#if !defined(PPR_IS_NETWARE)
#include <sys/select.h>
#endif
#endif

#include <sys/socket.h>
#include <sys/types.h>
#if defined(PPR_IS_NETWARE)
#include <sys/bsdskt.h>
#else
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif

#if defined(PPR_HAS_UNIX_SOCKET)
#include <sys/un.h>
#endif
#endif

/* windows.h wants to be the last one included */
#if defined(PPR_IS_WIN32)
#include <windows.h>
#endif

#endif
