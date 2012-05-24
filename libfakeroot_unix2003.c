/*
  Copyright Ⓒ 2009  Regis Duchesne

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*/

#if defined __APPLE__ && !defined __LP64__
/*
   In this file, we want 'struct stat' to have a 32-bit 'ino_t'.
   We use 'struct stat64' when we need a 64-bit 'ino_t'.
*/
#define _DARWIN_NO_64_BIT_INODE

/*
   This file is for 32-bit symbols which have the "$UNIX2003" version, i.e.
   32-bit symbols whose semantics adhere to the SUSv3 standard.
*/
#define _DARWIN_C_SOURCE

#include "config.h"
#include "communicate.h"

#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5

#ifdef STUPID_ALPHA_HACK
#define SEND_STAT(a,b,c) send_stat(a,b,c)
#define SEND_STAT64(a,b,c) send_stat64(a,b,c)
#define SEND_GET_STAT(a,b) send_get_stat(a,b)
#define SEND_GET_STAT64(a,b) send_get_stat64(a,b)
#else
#define SEND_STAT(a,b,c) send_stat(a,b)
#define SEND_STAT64(a,b,c) send_stat64(a,b)
#define SEND_GET_STAT(a,b) send_get_stat(a)
#define SEND_GET_STAT64(a,b) send_get_stat64(a)
#endif

/*
   These INT_* (which stands for internal) macros should always be used when
   the fakeroot library owns the storage of the stat variable.
*/
#ifdef STAT64_SUPPORT
#define INT_STRUCT_STAT struct stat64
#define INT_NEXT_STAT(a,b) NEXT_STAT64(_STAT_VER,a,b)
#define INT_NEXT_LSTAT(a,b) NEXT_LSTAT64(_STAT_VER,a,b)
#define INT_NEXT_FSTAT(a,b) NEXT_FSTAT64(_STAT_VER,a,b)
#define INT_NEXT_FSTATAT(a,b,c,d) NEXT_FSTATAT64(_STAT_VER,a,b,c,d)
#define INT_SEND_STAT(a,b) SEND_STAT64(a,b,_STAT_VER)
#else
#define INT_STRUCT_STAT struct stat
#define INT_NEXT_STAT(a,b) NEXT_STAT(_STAT_VER,a,b)
#define INT_NEXT_LSTAT(a,b) NEXT_LSTAT(_STAT_VER,a,b)
#define INT_NEXT_FSTAT(a,b) NEXT_FSTAT(_STAT_VER,a,b)
#define INT_NEXT_FSTATAT(a,b,c,d) NEXT_FSTATAT(_STAT_VER,a,b,c,d)
#define INT_SEND_STAT(a,b) SEND_STAT(a,b,_STAT_VER)
#endif

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <spawn.h>
#ifdef HAVE_SYS_ACL_H
#include <sys/acl.h>
#endif /* HAVE_SYS_ACL_H */
#if HAVE_FTS_H
#include <fts.h>
#endif /* HAVE_FTS_H */

#include "patchattr.h"
#include "wrapped.h"
#include "wraptmpf.h"
#include "wrapdef.h"

extern int fakeroot_disabled;

#ifdef LCHOWN_SUPPORT
extern int dont_try_chown() __attribute__((visibility("hidden")));

int lchown$UNIX2003(const char *path, uid_t owner, gid_t group){
  INT_STRUCT_STAT st;
  int r=0;

#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "lchown$UNIX2003 path %s owner %d group %d\n", path, owner, group);
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  r=INT_NEXT_LSTAT(path, &st);
  if(r)
    return r;
  st.st_uid=owner;
  st.st_gid=group;
  INT_SEND_STAT(&st,chown_func);
  if(!dont_try_chown())
    r=next_lchown$UNIX2003(path,owner,group);
  else
    r=0;
  if(r&&(errno==EPERM))
    r=0;

  return r;
}
#endif

int chmod$UNIX2003(const char *path, mode_t mode){
  INT_STRUCT_STAT st;
  int r;

#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "chmod$UNIX2003 path %s\n", path);
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  r=INT_NEXT_STAT(path, &st);
  if(r)
    return r;

  st.st_mode=(mode&ALLPERMS)|(st.st_mode&~ALLPERMS);

  INT_SEND_STAT(&st, chmod_func);

  /* if a file is unwritable, then root can still write to it
     (no matter who owns the file). If we are fakeroot, the only
     way to fake this is to always make the file writable, readable
     etc for the real user (who started fakeroot). Also holds for
     the exec bit of directories.
     Yes, packages requering that are broken. But we have lintian
     to get rid of broken packages, not fakeroot.
  */
  mode |= 0600;
  if(S_ISDIR(st.st_mode))
    mode |= 0100;

  r=next_chmod$UNIX2003(path, mode);
  if(r&&(errno==EPERM))
    r=0;
#ifdef EFTYPE		/* available under FreeBSD kernel */
  if(r&&(errno==EFTYPE))
    r=0;
#endif
  return r;
}

int fchmod$UNIX2003(int fd, mode_t mode){
  int r;
  INT_STRUCT_STAT st;


#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "fchmod$UNIX2003 fd %d\n", fd);
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  r=INT_NEXT_FSTAT(fd, &st);

  if(r)
    return(r);

  st.st_mode=(mode&ALLPERMS)|(st.st_mode&~ALLPERMS);
  INT_SEND_STAT(&st,chmod_func);

  /* see chmod() for comment */
  mode |= 0600;
  if(S_ISDIR(st.st_mode))
    mode |= 0100;

  r=next_fchmod$UNIX2003(fd, mode);
  if(r&&(errno==EPERM))
    r=0;
#ifdef EFTYPE		/* available under FreeBSD kernel */
  if(r&&(errno==EFTYPE))
    r=0;
#endif
  return r;
}

extern int set_faked_reuid(uid_t ruid, uid_t euid) __attribute__((visibility("hidden")));

int setreuid$UNIX2003(SETREUID_ARG ruid, SETREUID_ARG euid){
#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "setreuid$UNIX2003\n");
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  if (fakeroot_disabled)
    return next_setreuid$UNIX2003(ruid, euid);
  return set_faked_reuid(ruid, euid);
}

extern int set_faked_regid(gid_t rgid, gid_t egid) __attribute__((visibility("hidden")));

int setregid$UNIX2003(SETREGID_ARG rgid, SETREGID_ARG egid){
#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "setregid$UNIX2003\n");
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  if (fakeroot_disabled)
    return next_setregid$UNIX2003(rgid, egid);
  return set_faked_regid(rgid, egid);
}

int
getattrlist$UNIX2003(const char *path, void *attrList, void *attrBuf,
            size_t attrBufSize, unsigned long options)
{
  int r;
  struct stat st;

#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "getattrlist$UNIX2003 path %s\n", path);
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  r=next_getattrlist$UNIX2003(path, attrList, attrBuf, attrBufSize, options);
  if (r) {
    return r;
  }
  if (options & FSOPT_NOFOLLOW) {
    r=WRAP_LSTAT(path, &st);
  } else {
    r=WRAP_STAT(path, &st);
  }
  if (r) {
    return r;
  }
  patchattr(attrList, attrBuf, st.st_uid, st.st_gid, st.st_mode);

  return 0;
}
#endif /* MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 */
#endif /* if defined __APPLE__ && !defined __LP64__ */
