#ifndef __PPR_MACHDEP_H__
#define __PPR_MACHDEP_H__

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__linux__) || defined(__sun__)
#define PPR_IS_UNIX
#elif defined(_WIN32)
#define PPR_IS_WIN32
#elif defined(_PSP)
#define PPR_IS_PSP
#elif defined(_EE)
#define PPR_IS_PS2
#elif defined(__NETWARE__)
#define PPR_IS_NETWARE
#endif

/*** Platform differences */

#if defined(PPR_IS_WIN32) || defined(PPR_IS_NETWARE)
#define ppr_newline "\r\n"
#else
#define ppr_newline "\n"
#endif

#undef PPR_HAS_IPV6
#undef PPR_HAS_POLL
#undef PPR_HAS_FORK
#undef PPR_HAS_UNIX_SOCKET
#undef PPR_USE_SOCKLEN_T

/* Windows */
#if defined(PPR_IS_WIN32)
#define PPR_HAS_IPV6
#endif

/* PSP/PS2 */
#if defined(PPR_IS_PSP) || defined(PPR_IS_PS2)
#define PPR_USE_SOCKLEN_T
#endif

/* NetWare */
#if defined(PPR_IS_NETWARE)
/* Nothing */
#endif

/* BSD family */
#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__linux__) || defined(__sun__)
#define PPR_HAS_IPV6
#define PPR_HAS_POLL
#define PPR_HAS_FORK
#define PPR_HAS_UNIX_SOCKET
#define PPR_USE_SOCKLEN_T
#endif

/*** Compiler differences ***/

#undef PPR_HAS_PACK

#if defined(__WATCOMC__) || defined(__GNUC__)
#define PPR_HAS_PACK
#endif

typedef unsigned char ppr_bool;
#define ppr_false 0
#define ppr_true 1

#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int8	 ppr_uint8_t;
typedef unsigned __int16 ppr_uint16_t;
typedef unsigned __int32 ppr_uint32_t;

typedef __int8	ppr_int8_t;
typedef __int16 ppr_int16_t;
typedef __int32 ppr_int32_t;

typedef __int64 ppr_size_t;
#elif !defined(__STRICT_ANSI__) && (defined(__GNUC__) || defined(__clang__))
typedef unsigned char  ppr_uint8_t;
typedef unsigned short ppr_uint16_t;
typedef unsigned int   ppr_uint32_t;

typedef signed char ppr_int8_t;
typedef short	    ppr_int16_t;
typedef int	    ppr_int32_t;

typedef long long ppr_size_t;
#else
typedef unsigned char  ppr_uint8_t;
typedef unsigned short ppr_uint16_t;
typedef unsigned int   ppr_uint32_t;

typedef signed char ppr_int8_t;
typedef short	    ppr_int16_t;
typedef int	    ppr_int32_t;

typedef long ppr_size_t;
#endif

typedef ppr_size_t ppr_time_t;

#endif
