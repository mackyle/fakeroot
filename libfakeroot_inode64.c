/*
  Copyright: GPL. 
  Author: regis duchesne  (hpreg@vmware.com)
*/

#ifdef __APPLE__
/*
   This file is for symbols which have the "$INODE64" version, i.e. symbols
   which use a 64-bit ino_t.

   In this file, 'struct stat' is an alias for 'struct stat64'.
*/
#define _DARWIN_USE_64_BIT_INODE

#include "config.h"
#include "communicate.h"

#include <stdio.h>
#ifdef HAVE_SYS_ACL_H
#include <sys/acl.h>
#endif /* HAVE_SYS_ACL_H */
#if HAVE_FTS_H
#include <fts.h>
#endif /* HAVE_FTS_H */

#include "wrapped.h"

int lstat(const char *file_name,
          struct stat *st){

  int r;

  r=next_lstat$INODE64(file_name, st);

  if(r)
    return -1;

  send_get_stat64((struct stat64 *)st);
  return 0;
}


int stat(const char *file_name,
         struct stat *st){
  int r;

  r=next_stat$INODE64(file_name,st);
  if(r)
    return -1;
  send_get_stat64((struct stat64 *)st);
  return 0;
}


int fstat(int fd,
          struct stat *st){
  int r;

  r=next_fstat$INODE64(fd, st);
  if(r)
    return -1;
  send_get_stat64((struct stat64 *)st);

  return 0;
}

#ifdef HAVE_FTS_READ
FTSENT *fts_read(FTS *ftsp) {
  FTSENT *r;

  r=next_fts_read$INODE64(ftsp);
  if(r && r->fts_statp) {  /* Should we bother checking fts_info here? */
    send_get_stat64((struct stat64 *)r->fts_statp);
  }

  return r;
}

FTSENT *fts_children(FTS *ftsp,
                     int options) {
  FTSENT *first;
  FTSENT *r;

  first=next_fts_children$INODE64(ftsp, options);
  for(r = first; r; r = r->fts_link) {
    if(r->fts_statp) {  /* Should we bother checking fts_info here? */
      send_get_stat64((struct stat64 *)r->fts_statp);
    }
  }

  return first;
}
#endif /* HAVE_FTS_READ */
#endif /* ifdef __APPLE__ */
