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
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <pthread.h>
#ifdef HAVE_ENDIAN_H
# include <endian.h>
#endif
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

volatile int comm_sd = -1;
static pthread_mutex_t comm_sd_mutex = PTHREAD_MUTEX_INITIALIZER;

static void fail(const char *msg)
{
  if (errno > 0)
    fprintf(stderr, "libfakeroot: %s: %s\n", msg, strerror(errno));
  else
    fprintf(stderr, "libfakeroot: %s\n", msg);

  exit(1);
}

static struct sockaddr *get_addr(void)
{
  static struct sockaddr_in addr = { 0, 0, { 0 } };

  if (!addr.sin_port) {
    char *str;
    int port;

    str = (char *) env_var_set(FAKEROOTKEY_ENV);
    if (!str) {
      errno = 0;
      fail("FAKEROOTKEY not defined in environment");
    }

    port = atoi(str);
    if (port <= 0 || port >= 65536) {
      errno = 0;
      fail("invalid port number in FAKEROOTKEY");
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);
  }

  return (struct sockaddr *) &addr;
}

static void open_comm_sd(void)
{
  if (comm_sd >= 0)
    return;

  comm_sd = socket(PF_INET, SOCK_STREAM, 0);
  if (comm_sd < 0)
    fail("socket");

  if (fcntl(comm_sd, F_SETFD, FD_CLOEXEC) < 0)
    fail("fcntl(F_SETFD, FD_CLOEXEC)");

  while (1) {
    if (connect(comm_sd, get_addr(), sizeof (struct sockaddr_in)) < 0) {
      if (errno != EINTR)
        fail("connect");
    } else
      break;
  }
}

void lock_comm_sd(void)
{
  pthread_mutex_lock(&comm_sd_mutex);
}

void unlock_comm_sd(void)
{
  pthread_mutex_unlock(&comm_sd_mutex);
}


static size_t write_all(int fd,const void*buf,size_t count) {
  ssize_t rc,remaining=count;
  while(remaining>0) {
	  rc= write(fd, buf+(count-remaining), remaining);
	  if(rc<=0) {
		  if(remaining==count) return rc;
		  else fail("partial write");
	  } else {
		  remaining-=rc;
	  }
  }
  return count-remaining;
}

static size_t read_all(int fd,void *buf,size_t count) {
  ssize_t rc,remaining=count;
  while(remaining>0) {
	  rc = read(fd,buf+(count-remaining),remaining);
	  if(rc<=0) {
		  if(remaining==count) return rc;
		  else fail("partial read");
	  } else {
		  remaining-=rc;
	  }
  }
  return count-remaining;
}

static void send_fakem_nr(const struct fake_msg *buf)
{
  struct fake_msg fm;

  fm.id = htonl(buf->id);
  fm.st.uid = htonl(buf->st.uid);
  fm.st.gid = htonl(buf->st.gid);
  fm.st.ino = htonll(buf->st.ino);
  fm.st.dev = htonll(buf->st.dev);
  fm.st.rdev = htonll(buf->st.rdev);
  fm.st.mode = htonl(buf->st.mode);
  fm.st.nlink = htonl(buf->st.nlink);
  fm.remote = htonl(0);
  fm.xattr.buffersize = htonl(buf->xattr.buffersize);
  fm.xattr.flags_rc = htonl(buf->xattr.flags_rc);
  memcpy(fm.xattr.buf, buf->xattr.buf, MAX_IPC_BUFFER_SIZE);

  while (1) {
    ssize_t len;

    len = write_all(comm_sd, &fm, sizeof (fm));
    if (len > 0)
      break;

    if (len == 0) {
      errno = 0;
      fail("write: socket is closed");
    }

    if (errno == EINTR)
      continue;

    fail("write");
  }
}

void send_fakem(const struct fake_msg *buf)
{
  lock_comm_sd();

  open_comm_sd();
  send_fakem_nr(buf);

  unlock_comm_sd();
}

static void get_fakem_nr(struct fake_msg *buf)
{
  while (1) {
    ssize_t len;

    len = read_all(comm_sd, buf, sizeof (struct fake_msg));
    if (len > 0)
      break;
    if (len == 0) {
      errno = 0;
      fail("read: socket is closed");
    }
    if (errno == EINTR)
      continue;
   fail("read");
  }

  buf->id = ntohl(buf->id);
  buf->st.uid = ntohl(buf->st.uid);
  buf->st.gid = ntohl(buf->st.gid);
  buf->st.ino = ntohll(buf->st.ino);
  buf->st.dev = ntohll(buf->st.dev);
  buf->st.rdev = ntohll(buf->st.rdev);
  buf->st.mode = ntohl(buf->st.mode);
  buf->st.nlink = ntohl(buf->st.nlink);
  buf->remote = ntohl(buf->remote);
  buf->xattr.buffersize = ntohl(buf->xattr.buffersize);
  buf->xattr.flags_rc = ntohl(buf->xattr.flags_rc);
}

void send_get_fakem(struct fake_msg *buf)
{
  lock_comm_sd();

  open_comm_sd();
  send_fakem_nr(buf);
  get_fakem_nr(buf);

  unlock_comm_sd();
}

/* Calls to init_get_msg are needed for the sysv transport.  TCP doesn't need
 * this so this is a noop */
int init_get_msg(){
  return 0;
}

