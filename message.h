/*
  Copyright Ⓒ 1997, 1998, 1999, 2000, 2001  joost witteveen
  Copyright Ⓒ 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009  Clint Adams

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#ifndef FAKEROOT_MESSAGE_H
#define FAKEROOT_MESSAGE_H

#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
# ifdef HAVE_INTTYPES_H
# include <inttypes.h>
# else
#  error Problem
# endif
#endif

/* On Solaris, use the native htonll(n)/ntohll(n) */
#if !defined(sun) && !defined(_NETINET_IN_H)
#if __BYTE_ORDER == __BIG_ENDIAN
# define htonll(n)  (n)
# define ntohll(n)  (n)
#elif __BYTE_ORDER == __LITTLE_ENDIAN
# define htonll(n)  ((((uint64_t) htonl(n)) << 32LL) | htonl((n) >> 32LL))
# define ntohll(n)  ((((uint64_t) ntohl(n)) << 32LL) | ntohl((n) >> 32LL))
#endif
#endif /* !defined(sun) && !defined(_NETINET_IN_H) */

#define FAKEROOTKEY_ENV "FAKEROOTKEY"

typedef uint32_t func_id_t;

typedef uint64_t fake_ino_t;
typedef uint64_t fake_dev_t;
typedef uint32_t fake_uid_t;
typedef uint32_t fake_gid_t;
typedef uint32_t fake_mode_t;
typedef uint32_t fake_nlink_t;

#if __SUNPRO_C
#pragma pack(4)
#endif
struct fakestat {
	fake_uid_t   uid;
	fake_gid_t   gid;
	fake_ino_t   ino;
	fake_dev_t   dev;
	fake_dev_t   rdev;
	fake_mode_t  mode;
	fake_nlink_t nlink;
} FAKEROOT_ATTR(packed);
#if __SUNPRO_C
#pragma pack()
#endif

#define MAX_IPC_BUFFER_SIZE 1024

#if __SUNPRO_C
#pragma pack(4)
#endif
struct fakexattr {
	uint32_t   buffersize;
	char       buf[MAX_IPC_BUFFER_SIZE];
	int32_t    flags_rc; /* flags from setxattr. Return code on round trip */
} FAKEROOT_ATTR(packed);
#if __SUNPRO_C
#pragma pack()
#endif

#if __SUNPRO_C
#pragma pack(4)
#endif
struct fake_msg {
#ifndef FAKEROOT_FAKENET
	long mtype; /* message type in SYSV message sending */
#endif
	func_id_t       id; /* the requested function */
#ifndef FAKEROOT_FAKENET
	pid_t pid;
	int serial;
#endif
	struct fakestat st;
	struct fakexattr xattr;
	uint32_t        remote;
} FAKEROOT_ATTR(packed);
#if __SUNPRO_C
#pragma pack()
#endif

#endif
