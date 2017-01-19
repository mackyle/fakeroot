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
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

int msg_snd=-1;
int msg_get=-1;
int sem_id=-1;

void semaphore_up(){
  struct sembuf op;
  if(sem_id==-1)
    sem_id=semget(get_ipc_key(0)+2,1,IPC_CREAT|0600);
  op.sem_num=0;
  op.sem_op=-1;
  op.sem_flg=SEM_UNDO;
  init_get_msg();
  while (1) {
    if (semop(sem_id,&op,1)) {
      if (errno != EINTR) {
	perror("semop(1): encountered an error");
        exit(1);
      }
    } else {
      break;
    }
  }
}

void semaphore_down(){
  struct sembuf op;
  if(sem_id==-1)
    sem_id=semget(get_ipc_key(0)+2,1,IPC_CREAT|0600);
  op.sem_num=0;
  op.sem_op=1;
  op.sem_flg=SEM_UNDO;
  while (1) {
    if (semop(sem_id,&op,1)) {
      if (errno != EINTR) {
        perror("semop(2): encountered an error");
        exit(1);
      }
    } else {
      break;
    }
  }
}

void send_fakem(const struct fake_msg *buf)
{
  int r;

  if(init_get_msg()!=-1){
    ((struct fake_msg *)buf)->mtype=1;
    r=msgsnd(msg_snd, (struct fake_msg *)buf,
	     sizeof(*buf)-sizeof(buf->mtype), 0);
    if(r==-1)
      perror("libfakeroot, when sending message");
  }
}

void send_get_fakem(struct fake_msg *buf)
{
  /*
  send and get a struct fakestat from the daemon.
  We have to use serial/pid numbers in addidtion to
     the semaphore locking, to prevent the following:

  Client 1 locks and sends a stat() request to deamon.
  meantime, client 2 tries to up the semaphore too, but blocks.
  While client 1 is waiting, it recieves a KILL signal, and dies.
  SysV semaphores can eighter be automatically cleaned up when
  a client dies, or they can stay in place. We have to use the
  cleanup version, as otherwise client 2 will block forever.
  So, the semaphore is cleaned up when client 1 recieves the KILL signal.
  Now, client 1 falls through the semaphore_up, and
  sends a stat() request to the daemon --  it will now recieve
  the answer intended for client 1, and hell breaks lose (yes,
  this has actually happened, and yes, it was hell (to debug)).

  I realise that I may well do away with the semaphore stuff,
  if I put the serial/pid numbers in the mtype field. But I cannot
  store both PID and serial in mtype (just 32 bits on Linux). So
  there will always be some (small) chance it will go wrong.
  */

  int l;
  pid_t pid;
  static int serial=0;

  if(init_get_msg()!=-1){
    pid=getpid();
    semaphore_up();
    serial++;
    buf->serial=serial;
    buf->pid=pid;
    send_fakem(buf);

    do
      l=msgrcv(msg_get,
               (struct my_msgbuf*)buf,
               sizeof(*buf)-sizeof(buf->mtype),0,0);
    while((buf->serial!=serial)||buf->pid!=pid);

    semaphore_down();

    /*
    (nah, may be wrong, due to allignment)

    if(l!=sizeof(*buf)-sizeof(buf->mtype))
    printf("libfakeroot/fakeroot, internal bug!! get_fake: length=%i != l=%i",
    sizeof(*buf)-sizeof(buf->mtype),l);
    */

  }
}

key_t get_ipc_key(key_t new_key)
{
  const char *s;
  static key_t key=-1;

  if(key==-1){
    if(new_key!=0)
      key=new_key;
    else if((s=env_var_set(FAKEROOTKEY_ENV)))
      key=atoi(s);
    else
      key=0;
  };
  return key;
}


int init_get_msg(){
  /* a msgget call generates a fstat() call. As fstat() is wrapped,
     that call will in turn call semaphore_up(). So, before
     the semaphores are setup, we should make sure we already have
     the msg_get and msg_set id.
     This is why semaphore_up() calls this function.*/
  static int done=0;
  key_t key;

  if((!done)&&(msg_get==-1)){
    key=get_ipc_key(0);
    if(key){
      msg_snd=msgget(get_ipc_key(0),IPC_CREAT|00600);
      msg_get=msgget(get_ipc_key(0)+1,IPC_CREAT|00600);
    }
    else{
      msg_get=-1;
      msg_snd=-1;
    }
    done=1;
  }
  return msg_snd;
}

/* fake_get_owner() allows a process which has not set LD_PRELOAD to query
   the fake ownership etc. of files.  That process needs to know the key
   in use by faked - faked prints this at startup. */
int fake_get_owner(int is_lstat, const char *key, const char *path,
                  uid_t *uid, gid_t *gid, mode_t *mode){
  struct stat st;
  int i;

  if (!key || !strlen(key))
    return 0;

  /* Do the stat or lstat */
  i = (is_lstat ? lstat(path, &st) : stat(path, &st));
  if (i < 0)
    return i;

  /* Now pass it to faked */
  get_ipc_key(atoi(key));
#ifndef STUPID_ALPHA_HACK
  send_get_stat(&st);
#else
  send_get_stat(&st, _STAT_VER);
#endif

  /* Return the values inside the pointers */
  if (uid)
    *uid = st.st_uid;
  if (gid)
    *gid = st.st_gid;
  if (mode)
    *mode = st.st_mode;

  return 0;
}
