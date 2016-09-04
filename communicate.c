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

 This file contains the code (wrapper functions) that gets linked with
 the programes run from inside fakeroot. These programes then communicate
 with the fakeroot daemon, that keeps information about the "fake"
 ownerships etc. of the files etc.

 */

#ifdef __APPLE__
/*
   In this file, we want 'struct stat' to have a 32-bit 'ino_t'.
   We use 'struct stat64' when we need a 64-bit 'ino_t'.
*/
#define _DARWIN_NO_64_BIT_INODE
#endif

#include "communicate.h"
#include <dlfcn.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#ifdef STUPID_ALPHA_HACK
#include "stats.h"
#endif

#ifndef _UTSNAME_LENGTH
/* for LINUX libc5 */
#  define _UTSNAME_LENGTH _SYS_NMLN
#endif

const char *env_var_set(const char *env){
  const char *s;

  s=getenv(env);

  if(s && *s)
    return s;
  else
    return NULL;
}

void cpyfakemstat(struct fake_msg *f, const struct stat *st
#ifdef STUPID_ALPHA_HACK
		, int ver
#endif
		){

#ifndef STUPID_ALPHA_HACK
  f->st.mode =st->st_mode;
  f->st.ino  =st->st_ino ;
  f->st.uid  =st->st_uid ;
  f->st.gid  =st->st_gid ;
  f->st.dev  =st->st_dev ;
  f->st.rdev =st->st_rdev;

  /* DO copy the nlink count. Although the system knows this
     one better, we need it for unlink().
     This actually opens up a race condition, if another command
     makes a hardlink on a file, while we try to unlink it. This
     may cause the record to be deleted, while the link continues
     to live on the disk. But the chance is small, and unlikely
     to occur in practical fakeroot conditions. */

  f->st.nlink=st->st_nlink;
#else
  switch(ver) {
	  case _STAT_VER_KERNEL:
  f->st.mode  = ((struct fakeroot_kernel_stat *)st)->st_mode;
  f->st.ino   = ((struct fakeroot_kernel_stat *)st)->st_ino;
  f->st.uid   = ((struct fakeroot_kernel_stat *)st)->st_uid;
  f->st.gid   = ((struct fakeroot_kernel_stat *)st)->st_gid;
  f->st.dev   = ((struct fakeroot_kernel_stat *)st)->st_dev;
  f->st.rdev  = ((struct fakeroot_kernel_stat *)st)->st_rdev;
  f->st.nlink = ((struct fakeroot_kernel_stat *)st)->st_nlink;
  break;
	  case _STAT_VER_GLIBC2:
  f->st.mode  = ((struct fakeroot_glibc2_stat *)st)->st_mode;
  f->st.ino   = ((struct fakeroot_glibc2_stat *)st)->st_ino;
  f->st.uid   = ((struct fakeroot_glibc2_stat *)st)->st_uid;
  f->st.gid   = ((struct fakeroot_glibc2_stat *)st)->st_gid;
  f->st.dev   = ((struct fakeroot_glibc2_stat *)st)->st_dev;
  f->st.rdev  = ((struct fakeroot_glibc2_stat *)st)->st_rdev;
  f->st.nlink = ((struct fakeroot_glibc2_stat *)st)->st_nlink;
  break;
		  case _STAT_VER_GLIBC2_1:
  f->st.mode  = ((struct fakeroot_glibc21_stat *)st)->st_mode;
  f->st.ino   = ((struct fakeroot_glibc21_stat *)st)->st_ino;
  f->st.uid   = ((struct fakeroot_glibc21_stat *)st)->st_uid;
  f->st.gid   = ((struct fakeroot_glibc21_stat *)st)->st_gid;
  f->st.dev   = ((struct fakeroot_glibc21_stat *)st)->st_dev;
  f->st.rdev  = ((struct fakeroot_glibc21_stat *)st)->st_rdev;
  f->st.nlink = ((struct fakeroot_glibc21_stat *)st)->st_nlink;
  break;
		  default:
  f->st.mode  = st->st_mode;
  f->st.ino   = st->st_ino;
  f->st.uid   = st->st_uid;
  f->st.gid   = st->st_gid;
  f->st.dev   = st->st_dev;
  f->st.rdev  = st->st_rdev;
  f->st.nlink = st->st_nlink;
  break;
  }
#endif
}

void cpystatfakem(struct stat *st, const struct fake_msg *f
#ifdef STUPID_ALPHA_HACK
		, int ver
#endif
		){
#ifndef STUPID_ALPHA_HACK
  st->st_mode =f->st.mode;
  st->st_ino  =f->st.ino ;
  st->st_uid  =f->st.uid ;
  st->st_gid  =f->st.gid ;
  st->st_dev  =f->st.dev ;
  st->st_rdev =f->st.rdev;
  /* DON'T copy the nlink count! The system always knows
     this one better! */
  /*  st->st_nlink=f->st.nlink;*/
#else
  switch(ver) {
	  case _STAT_VER_KERNEL:
  ((struct fakeroot_kernel_stat *)st)->st_mode = f->st.mode;
  ((struct fakeroot_kernel_stat *)st)->st_ino  = f->st.ino;
  ((struct fakeroot_kernel_stat *)st)->st_uid  = f->st.uid;
  ((struct fakeroot_kernel_stat *)st)->st_gid  = f->st.gid;
  ((struct fakeroot_kernel_stat *)st)->st_dev  = f->st.dev;
  ((struct fakeroot_kernel_stat *)st)->st_rdev = f->st.rdev;
  break;
	  case _STAT_VER_GLIBC2:
  ((struct fakeroot_glibc2_stat *)st)->st_mode = f->st.mode;
  ((struct fakeroot_glibc2_stat *)st)->st_ino  = f->st.ino;
  ((struct fakeroot_glibc2_stat *)st)->st_uid  = f->st.uid;
  ((struct fakeroot_glibc2_stat *)st)->st_gid  = f->st.gid;
  ((struct fakeroot_glibc2_stat *)st)->st_dev  = f->st.dev;
  ((struct fakeroot_glibc2_stat *)st)->st_rdev = f->st.rdev;
  break;
		  case _STAT_VER_GLIBC2_1:
  ((struct fakeroot_glibc21_stat *)st)->st_mode = f->st.mode;
  ((struct fakeroot_glibc21_stat *)st)->st_ino  = f->st.ino;
  ((struct fakeroot_glibc21_stat *)st)->st_uid  = f->st.uid;
  ((struct fakeroot_glibc21_stat *)st)->st_gid  = f->st.gid;
  ((struct fakeroot_glibc21_stat *)st)->st_dev  = f->st.dev;
  ((struct fakeroot_glibc21_stat *)st)->st_rdev = f->st.rdev;
  break;
		  default:
  st->st_mode =f->st.mode;
  st->st_ino  =f->st.ino ;
  st->st_uid  =f->st.uid ;
  st->st_gid  =f->st.gid ;
  st->st_dev  =f->st.dev ;
  st->st_rdev =f->st.rdev;
  break;
  }
#endif
}

#ifdef STAT64_SUPPORT

void cpyfakemstat64(struct fake_msg *f,
                 const struct stat64 *st
#ifdef STUPID_ALPHA_HACK
                 , int ver
#endif
                 ){
#ifndef STUPID_ALPHA_HACK
  f->st.mode =st->st_mode;
  f->st.ino  =st->st_ino ;
  f->st.uid  =st->st_uid ;
  f->st.gid  =st->st_gid ;
  f->st.dev  =st->st_dev ;
  f->st.rdev =st->st_rdev;

  /* DO copy the nlink count. Although the system knows this
     one better, we need it for unlink().
     This actually opens up a race condition, if another command
     makes a hardlink on a file, while we try to unlink it. This
     may cause the record to be deleted, while the link continues
     to live on the disk. But the chance is small, and unlikely
     to occur in practical fakeroot conditions. */

  f->st.nlink=st->st_nlink;
#else
  switch(ver) {
	  case _STAT_VER_KERNEL:
  f->st.mode  = ((struct fakeroot_kernel_stat *)st)->st_mode;
  f->st.ino   = ((struct fakeroot_kernel_stat *)st)->st_ino;
  f->st.uid   = ((struct fakeroot_kernel_stat *)st)->st_uid;
  f->st.gid   = ((struct fakeroot_kernel_stat *)st)->st_gid;
  f->st.dev   = ((struct fakeroot_kernel_stat *)st)->st_dev;
  f->st.rdev  = ((struct fakeroot_kernel_stat *)st)->st_rdev;
  f->st.nlink = ((struct fakeroot_kernel_stat *)st)->st_nlink;
  break;
	  case _STAT_VER_GLIBC2:
  f->st.mode  = ((struct fakeroot_glibc2_stat *)st)->st_mode;
  f->st.ino   = ((struct fakeroot_glibc2_stat *)st)->st_ino;
  f->st.uid   = ((struct fakeroot_glibc2_stat *)st)->st_uid;
  f->st.gid   = ((struct fakeroot_glibc2_stat *)st)->st_gid;
  f->st.dev   = ((struct fakeroot_glibc2_stat *)st)->st_dev;
  f->st.rdev  = ((struct fakeroot_glibc2_stat *)st)->st_rdev;
  f->st.nlink = ((struct fakeroot_glibc2_stat *)st)->st_nlink;
  break;
	  case _STAT_VER_GLIBC2_1:
  f->st.mode  = ((struct fakeroot_glibc21_stat *)st)->st_mode;
  f->st.ino   = ((struct fakeroot_glibc21_stat *)st)->st_ino;
  f->st.uid   = ((struct fakeroot_glibc21_stat *)st)->st_uid;
  f->st.gid   = ((struct fakeroot_glibc21_stat *)st)->st_gid;
  f->st.dev   = ((struct fakeroot_glibc21_stat *)st)->st_dev;
  f->st.rdev  = ((struct fakeroot_glibc21_stat *)st)->st_rdev;
  f->st.nlink = ((struct fakeroot_glibc21_stat *)st)->st_nlink;
  break;
		  default:
  f->st.mode  = st->st_mode;
  f->st.ino   = st->st_ino;
  f->st.uid   = st->st_uid;
  f->st.gid   = st->st_gid;
  f->st.dev   = st->st_dev;
  f->st.rdev  = st->st_rdev;
  f->st.nlink = st->st_nlink;
  break;
  }
#endif
}
void cpystat64fakem(struct stat64 *st,
                 const struct fake_msg *f
#ifdef STUPID_ALPHA_HACK
                 , int ver
#endif
                 ){
#ifndef STUPID_ALPHA_HACK
  st->st_mode =f->st.mode;
  st->st_ino  =f->st.ino ;
  st->st_uid  =f->st.uid ;
  st->st_gid  =f->st.gid ;
  st->st_dev  =f->st.dev ;
  st->st_rdev =f->st.rdev;
  /* DON'T copy the nlink count! The system always knows
     this one better! */
  /*  st->st_nlink=f->st.nlink;*/
#else
  switch(ver) {
	  case _STAT_VER_KERNEL:
  ((struct fakeroot_kernel_stat *)st)->st_mode = f->st.mode;
  ((struct fakeroot_kernel_stat *)st)->st_ino  = f->st.ino;
  ((struct fakeroot_kernel_stat *)st)->st_uid  = f->st.uid;
  ((struct fakeroot_kernel_stat *)st)->st_gid  = f->st.gid;
  ((struct fakeroot_kernel_stat *)st)->st_dev  = f->st.dev;
  ((struct fakeroot_kernel_stat *)st)->st_rdev = f->st.rdev;
  break;
	  case _STAT_VER_GLIBC2:
  ((struct fakeroot_glibc2_stat *)st)->st_mode = f->st.mode;
  ((struct fakeroot_glibc2_stat *)st)->st_ino  = f->st.ino;
  ((struct fakeroot_glibc2_stat *)st)->st_uid  = f->st.uid;
  ((struct fakeroot_glibc2_stat *)st)->st_gid  = f->st.gid;
  ((struct fakeroot_glibc2_stat *)st)->st_dev  = f->st.dev;
  ((struct fakeroot_glibc2_stat *)st)->st_rdev = f->st.rdev;
  break;
		  case _STAT_VER_GLIBC2_1:
  ((struct fakeroot_glibc21_stat *)st)->st_mode = f->st.mode;
  ((struct fakeroot_glibc21_stat *)st)->st_ino  = f->st.ino;
  ((struct fakeroot_glibc21_stat *)st)->st_uid  = f->st.uid;
  ((struct fakeroot_glibc21_stat *)st)->st_gid  = f->st.gid;
  ((struct fakeroot_glibc21_stat *)st)->st_dev  = f->st.dev;
  ((struct fakeroot_glibc21_stat *)st)->st_rdev = f->st.rdev;
  break;
		  default:
  st->st_mode =f->st.mode;
  st->st_ino  =f->st.ino ;
  st->st_uid  =f->st.uid ;
  st->st_gid  =f->st.gid ;
  st->st_dev  =f->st.dev ;
  st->st_rdev =f->st.rdev;
  break;
  }
#endif
}

#endif /* STAT64_SUPPORT */

void cpyfakefake(struct fakestat *dest,
                 const struct fakestat *source){
  dest->mode =source->mode;
  dest->ino  =source->ino ;
  dest->uid  =source->uid ;
  dest->gid  =source->gid ;
  dest->dev  =source->dev ;
  dest->rdev =source->rdev;
  /* DON'T copy the nlink count! The system always knows
     this one better! */
  /*  dest->nlink=source->nlink;*/
}


#ifdef _LARGEFILE_SOURCE

void stat64from32(struct stat64 *s64, const struct stat *s32)
{
  /* I've added st_size and st_blocks here.
     Don't know why they were missing -- joost*/
   s64->st_dev = s32->st_dev;
   s64->st_ino = s32->st_ino;
   s64->st_mode = s32->st_mode;
   s64->st_nlink = s32->st_nlink;
   s64->st_uid = s32->st_uid;
   s64->st_gid = s32->st_gid;
   s64->st_rdev = s32->st_rdev;
   s64->st_size = s32->st_size;
   s64->st_blksize = s32->st_blksize;
   s64->st_blocks = s32->st_blocks;
   s64->st_atime = s32->st_atime;
   s64->st_mtime = s32->st_mtime;
   s64->st_ctime = s32->st_ctime;
}

/* This assumes that the 64 bit structure is actually filled in and does not
   down case the sizes from the 32 bit one.. */
void stat32from64(struct stat *s32, const struct stat64 *s64)
{
   s32->st_dev = s64->st_dev;
   s32->st_ino = s64->st_ino;
   s32->st_mode = s64->st_mode;
   s32->st_nlink = s64->st_nlink;
   s32->st_uid = s64->st_uid;
   s32->st_gid = s64->st_gid;
   s32->st_rdev = s64->st_rdev;
   s32->st_size = (long)s64->st_size;
   s32->st_blksize = s64->st_blksize;
   s32->st_blocks = (long)s64->st_blocks;
   s32->st_atime = s64->st_atime;
   s32->st_mtime = s64->st_mtime;
   s32->st_ctime = s64->st_ctime;
}

#endif

void send_stat(const struct stat *st,
	       func_id_t f
#ifdef STUPID_ALPHA_HACK
	       , int ver
#endif
	       ){
  struct fake_msg buf;

  if(init_get_msg()!=-1)
  {
#ifndef STUPID_ALPHA_HACK
    cpyfakemstat(&buf,st);
#else
    cpyfakemstat(&buf,st,ver);
#endif
    buf.id=f;
    send_fakem(&buf);
  }
}

#ifdef STAT64_SUPPORT
void send_stat64(const struct stat64 *st,
                 func_id_t f
#ifdef STUPID_ALPHA_HACK
                 , int ver
#endif
                 ){
  struct fake_msg buf;

  if(init_get_msg()!=-1)
  {
#ifndef STUPID_ALPHA_HACK
    cpyfakemstat64(&buf,st);
#else
    cpyfakemstat64(&buf,st,ver);
#endif
    buf.id=f;
    send_fakem(&buf);
  }
}
#endif /* STAT64_SUPPORT */

void send_get_stat(struct stat *st
#ifdef STUPID_ALPHA_HACK
		, int ver
#endif
		){
  struct fake_msg buf;

  if(init_get_msg()!=-1)
  {
#ifndef STUPID_ALPHA_HACK
    cpyfakemstat(&buf,st);
#else
    cpyfakemstat(&buf,st,ver);
#endif

    buf.id=stat_func;
    send_get_fakem(&buf);
#ifndef STUPID_ALPHA_HACK
    cpystatfakem(st,&buf);
#else
    cpystatfakem(st,&buf,ver);
#endif
  }
}

void send_get_xattr(struct stat *st
		, xattr_args *xattr
#ifdef STUPID_ALPHA_HACK
		, int ver
#endif
		){
  struct fake_msg buf;
  size_t in_size;
  size_t name_size;
  size_t total_size;

  if(init_get_msg()!=-1)
  {
#ifndef STUPID_ALPHA_HACK
    cpyfakemstat(&buf,st);
#else
    cpyfakemstat(&buf,st,ver);
#endif
    in_size = xattr->size;
    total_size = (xattr->func == setxattr_func) ? (in_size) : 0;
    if (xattr->name)
    {
      name_size = strlen(xattr->name);
      total_size += name_size + 1;
    }
    if (total_size > MAX_IPC_BUFFER_SIZE)
    {
      xattr->rc = ERANGE;
      return;
    }
    if (xattr->name) {
      strcpy(buf.xattr.buf, xattr->name);
      if (xattr->func == setxattr_func)
        memcpy(&buf.xattr.buf[name_size + 1], xattr->value, in_size);
    }
    buf.xattr.buffersize = total_size;
    buf.xattr.flags_rc = xattr->flags;
    buf.id=xattr->func;
    send_get_fakem(&buf);
    xattr->rc = buf.xattr.flags_rc;
    xattr->size = buf.xattr.buffersize;
    if (buf.xattr.buffersize) {
      if (!in_size) {
        /* Special case. Return size of required buffer */
        return;
      }
      if (xattr->size > in_size) {
        xattr->rc = ERANGE;
        return;
      }
      memcpy(xattr->value, buf.xattr.buf, xattr->size);
    }
  }
}

#ifdef STAT64_SUPPORT
void send_get_stat64(struct stat64 *st
#ifdef STUPID_ALPHA_HACK
                     , int ver
#endif
                    )
{
  struct fake_msg buf;

  if(init_get_msg()!=-1)
  {
#ifndef STUPID_ALPHA_HACK
    cpyfakemstat64(&buf,st);
#else
    cpyfakemstat64(&buf,st,ver);
#endif

    buf.id=stat_func;
    send_get_fakem(&buf);
#ifndef STUPID_ALPHA_HACK
    cpystat64fakem(st,&buf);
#else
    cpystat64fakem(st,&buf,ver);
#endif
  }
}

void send_get_xattr64(struct stat64 *st
		, xattr_args *xattr
#ifdef STUPID_ALPHA_HACK
		, int ver
#endif
		){
  struct fake_msg buf;
  size_t in_size;
  size_t name_size;
  size_t total_size;

  if(init_get_msg()!=-1)
  {
#ifndef STUPID_ALPHA_HACK
    cpyfakemstat64(&buf,st);
#else
    cpyfakemstat64(&buf,st,ver);
#endif
    in_size = xattr->size;
    total_size = (xattr->func == setxattr_func) ? (in_size) : 0;
    if (xattr->name)
    {
      name_size = strlen(xattr->name);
      total_size += name_size + 1;
    }
    if (total_size > MAX_IPC_BUFFER_SIZE)
    {
      xattr->rc = ERANGE;
      return;
    }
    if (xattr->name) {
      strcpy(buf.xattr.buf, xattr->name);
      if (xattr->func == setxattr_func)
        memcpy(&buf.xattr.buf[name_size + 1], xattr->value, in_size);
    }
    buf.xattr.buffersize = total_size;
    buf.xattr.flags_rc = xattr->flags;
    buf.id=xattr->func;
    send_get_fakem(&buf);
    xattr->rc = buf.xattr.flags_rc;
    xattr->size = buf.xattr.buffersize;
    if (buf.xattr.buffersize) {
      if (!in_size) {
        /* Special case. Return size of required buffer */
        return;
      }
      if (xattr->size > in_size) {
        xattr->rc = ERANGE;
        return;
      }
      memcpy(xattr->value, buf.xattr.buf, xattr->size);
    }
  }
}
#endif /* STAT64_SUPPORT */
