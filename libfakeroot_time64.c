#include "config.h"
#include "communicate.h"

#include <stdio.h>

#ifdef HAVE_SYS_ACL_H
#include <sys/acl.h>
#endif /* HAVE_SYS_ACL_H */
#ifdef HAVE_SYS_CAPABILITY_H
#include <sys/capability.h>
#endif
#if HAVE_FTS_H
#include <fts.h>
#endif /* HAVE_FTS_H */
#ifdef HAVE_SYS_SYSMACROS_H
# include <sys/sysmacros.h>
#endif

#include "wrapped.h"

extern void load_library_symbols(void);

#ifdef LIBFAKEROOT_DEBUGGING
extern int fakeroot_debug;
#endif /* LIBFAKEROOT_DEBUGGING */


extern void send_get_fakem(struct fake_msg *buf);

#ifdef TIME64_HACK

#ifdef STUPID_ALPHA_HACK
#define SEND_GET_STAT64_TIME64(a,b) send_get_stat64_time64(a,b)
#else
#define SEND_GET_STAT64_TIME64(a,b) send_get_stat64_time64(a)
#endif

void cpyfakemstat64_time64(struct fake_msg *f,
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
void cpystat64fakem_time64(struct stat64 *st,
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

void send_get_stat64_time64(struct stat64 *st
#ifdef STUPID_ALPHA_HACK
                     , int ver
#endif
                    )
{
  struct fake_msg buf;

#ifndef FAKEROOT_FAKENET
  if(init_get_msg()!=-1)
#endif /* ! FAKEROOT_FAKENET */
  {
#ifndef STUPID_ALPHA_HACK
    cpyfakemstat64_time64(&buf,st);
#else
    cpyfakemstat64_time64(&buf,st,ver);
#endif

    buf.id=stat_func;
    send_get_fakem(&buf);
#ifndef STUPID_ALPHA_HACK
    cpystat64fakem_time64(st,&buf);
#else
    cpystat64fakem_time64(st,&buf,ver);
#endif
  }
}

int WRAP_LSTAT64_TIME64 LSTAT64_TIME64_ARG(int ver,
		       const char *file_name,
		       struct stat64 *statbuf){

  int r;

#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "lstat[time64] file_name %s\n", file_name);
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  r=NEXT_LSTAT64_TIME64(ver, file_name, statbuf);
  if(r)
    return -1;
  SEND_GET_STAT64_TIME64(statbuf, ver);
  return 0;
}


int WRAP_STAT64_TIME64 STAT64_TIME64_ARG(int ver,
		       const char *file_name,
		       struct stat64 *st){
  int r;

#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "stat64[time64] file_name %s\n", file_name);
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  r=NEXT_STAT64_TIME64(ver, file_name, st);
  if(r)
    return -1;
  SEND_GET_STAT64_TIME64(st,ver);
  return 0;
}


int WRAP_FSTAT64_TIME64 FSTAT64_TIME64_ARG(int ver,
			int fd,
			struct stat64 *st){

  int r;

#ifdef LIBFAKEROOT_DEBUGGING
  if (fakeroot_debug) {
    fprintf(stderr, "fstat64[time64] fd %d\n", fd);
  }
#endif /* LIBFAKEROOT_DEBUGGING */
  r=NEXT_FSTAT64_TIME64(ver, fd, st);
  if(r)
    return -1;
  SEND_GET_STAT64_TIME64(st,ver);
  return 0;
}

int WRAP_FSTATAT64_TIME64 FSTATAT64_TIME64_ARG(int ver,
			     int dir_fd,
			     const char *path,
			     struct stat64 *st,
			     int flags){


  int r;

  r=NEXT_FSTATAT64_TIME64(ver, dir_fd, path, st, flags);
  if(r)
    return -1;
  SEND_GET_STAT64_TIME64(st,ver);
  return 0;
}

#endif /* TIME64_HACK */
