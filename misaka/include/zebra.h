#ifndef _ZEBRA_H
#define _ZEBRA_H

#define assert(x) (x)

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

#include <sys/fcntl.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif /* HAVE_SYS_SELECT_H */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#ifdef HAVE_SYS_SYSCTL_H
#ifdef GNU_LINUX
#include <linux/types.h>
#endif
#include <sys/sysctl.h>
#endif /* HAVE_SYS_SYSCTL_H */
#include <sys/ioctl.h>
#ifdef HAVE_SYS_CONF_H
#include <sys/conf.h>
#endif /* HAVE_SYS_CONF_H */
#ifdef HAVE_SYS_KSYM_H
#include <sys/ksym.h>
#endif /* HAVE_SYS_KSYM_H */
#include <syslog.h>
#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif /* TIME_WITH_SYS_TIME */
#include <sys/uio.h>
#include <sys/utsname.h>
#ifdef HAVE_RUSAGE
#include <sys/resource.h>
#endif /* HAVE_RUSAGE */
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif /* HAVE_LIMITS_H */
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif /* HAVE_INTTYPES_H */

/* machine dependent includes */
#ifdef SUNOS_5
#include <strings.h>
#endif /* SUNOS_5 */

/* machine dependent includes */
#ifdef HAVE_LINUX_VERSION_H
#include <linux/version.h>
#endif /* HAVE_LINUX_VERSION_H */

#ifdef HAVE_ASM_TYPES_H
#include <asm/types.h>
#endif /* HAVE_ASM_TYPES_H */

/* misc include group */
#include <stdarg.h>
#if !(defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
/* Not C99; do we need to define va_copy? */
#ifndef va_copy
#ifdef __va_copy
#define va_copy(DST,SRC) __va_copy(DST,SRC)
#else
/* Now we are desperate; this should work on many typical platforms. 
   But this is slightly dangerous, because the standard does not require
   va_copy to be a macro. */
#define va_copy(DST,SRC) memcpy(&(DST), &(SRC), sizeof(va_list))
#warning "Not C99 and no va_copy macro available, falling back to memcpy"
#endif /* __va_copy */
#endif /* !va_copy */
#endif /* !C99 */

typedef signed char  s8;
typedef signed short s16;
typedef signed int   s32;
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#ifdef ABI_N64
typedef signed long  s64;
typedef unsigned long  u64;
#else
typedef signed long long s64;
typedef unsigned long long u64;
#endif


#include <sys/socket.h>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <net/if.h>

#ifdef HAVE_NET_ROUTE_H
#include <net/route.h>
#endif /* HAVE_NET_ROUTE_H */

#ifdef HAVE_NETLINK
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/filter.h>
#include <stddef.h>
#else
#define RT_TABLE_MAIN		0
#endif /* HAVE_NETLINK */

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif /* HAVE_NETDB_H */

#include <arpa/inet.h>

#ifdef HAVE_GLIBC_BACKTRACE
#include <execinfo.h>
#endif /* HAVE_GLIBC_BACKTRACE */

/* Local includes: */
#if !(defined(__GNUC__) || defined(VTYSH_EXTRACT_PL)) 
#define __attribute__(x)
#endif  /* !__GNUC__ || VTYSH_EXTRACT_PL */

//#include "zassert.h"

#ifdef HAVE_BROKEN_CMSG_FIRSTHDR
/* This bug is present in Solaris 8 and pre-patch Solaris 9 <sys/socket.h>;
   please refer to http://bugzilla.quagga.net/show_bug.cgi?id=142 */

/* Check that msg_controllen is large enough. */
#define ZCMSG_FIRSTHDR(mhdr) \
  (((size_t)((mhdr)->msg_controllen) >= sizeof(struct cmsghdr)) ? \
   CMSG_FIRSTHDR(mhdr) : (struct cmsghdr *)NULL)

#warning "CMSG_FIRSTHDR is broken on this platform, using a workaround"

#else /* HAVE_BROKEN_CMSG_FIRSTHDR */
#define ZCMSG_FIRSTHDR(M) CMSG_FIRSTHDR(M)
#endif /* HAVE_BROKEN_CMSG_FIRSTHDR */

/*
 * Internal defines, for use only in this file.
 * These are likely wrong on other than ILP32 machines, so warn.
 */
#ifndef _CMSG_DATA_ALIGN
#define _CMSG_DATA_ALIGN(n)           (((n) + 3) & ~3)
#endif /* _CMSG_DATA_ALIGN */

#ifndef _CMSG_HDR_ALIGN
#define _CMSG_HDR_ALIGN(n)            (((n) + 3) & ~3)
#endif /* _CMSG_HDR_ALIGN */

/*
 * CMSG_SPACE and CMSG_LEN are required in RFC3542, but were new in that
 * version.
 */
#ifndef CMSG_SPACE
#define CMSG_SPACE(l)       (_CMSG_DATA_ALIGN(sizeof(struct cmsghdr)) + \
                              _CMSG_HDR_ALIGN(l))
#warning "assuming 4-byte alignment for CMSG_SPACE"
#endif  /* CMSG_SPACE */


#ifndef CMSG_LEN
#define CMSG_LEN(l)         (_CMSG_DATA_ALIGN(sizeof(struct cmsghdr)) + (l))
#warning "assuming 4-byte alignment for CMSG_LEN"
#endif /* CMSG_LEN */


/*  The definition of struct in_pktinfo is missing in old version of
    GLIBC 2.1 (Redhat 6.1).  */
#if defined (GNU_LINUX) && ! defined (HAVE_STRUCT_IN_PKTINFO)
struct in_pktinfo
{
  int ipi_ifindex;
  struct in_addr ipi_spec_dst;
  struct in_addr ipi_addr;
};
#endif

/* 
 * OSPF Fragmentation / fragmented writes
 *
 * ospfd can support writing fragmented packets, for cases where
 * kernel will not fragment IP_HDRINCL and/or multicast destined
 * packets (ie TTBOMK all kernels, BSD, SunOS, Linux). However,
 * SunOS, probably BSD too, clobber the user supplied IP ID and IP
 * flags fields, hence user-space fragmentation will not work.
 * Only Linux is known to leave IP header unmolested.
 * Further, fragmentation really should be done the kernel, which already
 * supports it, and which avoids nasty IP ID state problems.
 *
 * Fragmentation of OSPF packets can be required on networks with router
 * with many many interfaces active in one area, or on networks with links
 * with low MTUs.
 */
#ifdef GNU_LINUX
#define WANT_OSPF_WRITE_FRAGMENT
#endif

/* 
 * IP_HDRINCL / struct ip byte order
 *
 * Linux: network byte order
 * *BSD: network, except for length and offset. (cf Stevens)
 * SunOS: nominally as per BSD. but bug: network order on LE.
 * OpenBSD: network byte order, apart from older versions which are as per 
 *          *BSD
 */
#if defined(__NetBSD__) || defined(__FreeBSD__) \
   || (defined(__OpenBSD__) && (OpenBSD < 200311)) \
   || (defined(SUNOS_5) && defined(WORDS_BIGENDIAN))
#define HAVE_IP_HDRINCL_BSD_ORDER
#endif

/* Define BYTE_ORDER, if not defined. Useful for compiler conditional
 * code, rather than preprocessor conditional.
 * Not all the world has this BSD define.
 */
#ifndef BYTE_ORDER
#define BIG_ENDIAN	4321	/* least-significant byte first (vax, pc) */
#define LITTLE_ENDIAN	1234	/* most-significant byte first (IBM, net) */
#define PDP_ENDIAN	3412	/* LSB first in word, MSW first in long (pdp) */

#if defined(WORDS_BIGENDIAN)
#define BYTE_ORDER	BIG_ENDIAN
#else /* !WORDS_BIGENDIAN */
#define BYTE_ORDER	LITTLE_ENDIAN
#endif /* WORDS_BIGENDIAN */

#endif /* ndef BYTE_ORDER */

/* MAX / MIN are not commonly defined, but useful */
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif 
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define ZEBRA_FAMILY_IPV4                1
#define ZEBRA_FAMILY_IPV6                2
#define ZEBRA_FAMILY_MAX                 3

#ifndef INADDR_LOOPBACK
#define	INADDR_LOOPBACK	0x7f000001	/* Internet address 127.0.0.1.  */
#endif

/* Address family numbers from RFC1700. */
#define AFI_IP                    1
#define AFI_IP6                   2
#define AFI_MAX                   3

/* Subsequent Address Family Identifier. */
#define SAFI_UNICAST              1
#define SAFI_MULTICAST            2
#define SAFI_UNICAST_MULTICAST    3
#define SAFI_MPLS_VPN             4
#define SAFI_MAX                  5

/* Flag manipulation macros. */
#define CHECK_FLAG(V,F)      ((V) & (F))
#define SET_FLAG(V,F)        (V) |= (F)
#define UNSET_FLAG(V,F)      (V) &= ~(F)

/* AFI and SAFI type. */
typedef u16 afi_t;
typedef u8 safi_t;

/* Zebra types. Used in Zserv message header. */
typedef u16 zebra_size_t;
typedef u16 zebra_command_t;

/* FIFO -- first in first out structure and macros.  */
struct fifo
{
  struct fifo *next;
  struct fifo *prev;
};

#define FIFO_INIT(F)                                  \
  do {                                                \
    struct fifo *Xfifo = (struct fifo *)(F);          \
    Xfifo->next = Xfifo->prev = Xfifo;                \
  } while (0)

#define FIFO_ADD(F,N)                                 \
  do {                                                \
    struct fifo *Xfifo = (struct fifo *)(F);          \
    struct fifo *Xnode = (struct fifo *)(N);          \
    Xnode->next = Xfifo;                              \
    Xnode->prev = Xfifo->prev;                        \
    Xfifo->prev = Xfifo->prev->next = Xnode;          \
  } while (0)

#define FIFO_DEL(N)                                   \
  do {                                                \
    struct fifo *Xnode = (struct fifo *)(N);          \
    Xnode->prev->next = Xnode->next;                  \
    Xnode->next->prev = Xnode->prev;                  \
  } while (0)

#define FIFO_HEAD(F)                                  \
  ((((struct fifo *)(F))->next == (struct fifo *)(F)) \
  ? NULL : (F)->next)

#define FIFO_EMPTY(F)                                 \
  (((struct fifo *)(F))->next == (struct fifo *)(F))

#define FIFO_TOP(F)                                   \
  (FIFO_EMPTY(F) ? NULL : ((struct fifo *)(F))->next)

#endif 
