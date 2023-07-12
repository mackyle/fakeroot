dnl Process this file with autoconf to produce a configure script.
AC_INIT([fakeroot],[1.32.1],[clint@debian.org],[fakeroot])
AC_PREREQ([2.71])
AC_CONFIG_MACRO_DIR([build-aux])
LT_PREREQ(2.1a)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_CONFIG_HEADERS([config.h])
AC_PROG_CPP
AC_PROG_MAKE_SET
LT_INIT
LT_LANG(C)

AH_BOTTOM([#if ! HAVE_BUILTIN_EXPECT
#define __builtin_expect(x, expected_value) (x)
#endif])

AC_ARG_WITH([ipc],
	    AS_HELP_STRING([--with-ipc@<:@=IPCTYPE@:>@],
			   [method of IPC to use: either sysv (default) or tcp]),
	    [ac_cv_use_ipc=$withval],
	    [ac_cv_use_ipc=sysv])

AC_CACHE_CHECK([which IPC method to use],
	       [ac_cv_use_ipc],
	       [ac_cv_use_ipc=sysv])

if test $ac_cv_use_ipc = "sysv"; then
  AC_MSG_CHECKING([whether SysV IPC message queues are actually working on the host])

  AC_LANG_PUSH(C)
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>

int main() {

  srandom(time(NULL)+getpid()*33151);
  key_t msg_key = random();
  int msg_get = msgget(msg_key, IPC_CREAT|0600);

  if (msg_get==-1) {
    return 1;
  } else {
    msgctl(msg_get, IPC_RMID, NULL);
    return 0;
  }

}]])],[ac_cv_use_ipc=sysv],[ac_cv_use_ipc=tcp],[ac_cv_use_ipc=cross])

  if test $ac_cv_use_ipc = cross; then
    if test "$host_os" = linux-gnu; then
      ac_cv_use_ipc=sysv
      AC_MSG_RESULT([cross, guessing yes])
    else
      (set -o posix; set)
      AC_MSG_ERROR([cross compiling, unknown result for $host_os])
    fi
  elif test $ac_cv_use_ipc = "tcp"; then
    AC_MSG_RESULT([No, using TCP])
  else
    AC_MSG_RESULT([Yes])
  fi

  AC_LANG_POP(C)
fi

AC_ARG_WITH([dbformat],
	    AS_HELP_STRING([--with-dbformat@<:@=DBFORMAT@:>@],
			   [database format to use: either inode (default) or path]),
	    [ac_cv_dbformat=$withval],
	    [ac_cv_dbformat=inode])

AC_CACHE_CHECK([which database format to use],
	       [ac_cv_dbformat],
	       [ac_cv_dbformat=inode])

AH_TEMPLATE([FAKEROOT_DB_PATH], [store path in the database instead of inode and device])
if test $ac_cv_dbformat = "path"; then
AC_DEFINE_UNQUOTED(FAKEROOT_DB_PATH, [1])
fi

dnl Checks for programs.

dnl Checks for libraries.
dnl Replace `main' with a function in -ldl:
AC_CHECK_LIB(dl, dlopen)
AH_TEMPLATE([FAKEROOT_FAKENET], [use TCP instead of SysV IPC])
if test $ac_cv_use_ipc = "tcp"; then
AC_DEFINE_UNQUOTED(FAKEROOT_FAKENET, [TCP])
AC_CHECK_LIB(pthread, pthread_self)
AC_CHECK_LIB(socket, connect)
signal=HUP
else
signal=TERM
fi

AC_SUBST(signal)

dnl Checks for header files.
AC_HEADER_DIRENT
AC_CHECK_HEADERS(fcntl.h unistd.h features.h sys/feature_tests.h pthread.h stdint.h inttypes.h grp.h endian.h sys/sysmacros.h sys/socket.h sys/acl.h sys/capability.h sys/xattr.h fts.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_CHECK_TYPE(mode_t, int)
AC_CHECK_TYPE(off_t, long)
AC_CHECK_TYPE(size_t, unsigned)

AH_TEMPLATE([FAKEROOT_ATTR], [for packed])
if test -n "$GCC";
then
AC_DEFINE_UNQUOTED(FAKEROOT_ATTR(x), [__attribute__ ((x))])
else
AC_DEFINE_UNQUOTED(FAKEROOT_ATTR(x), [])
fi

dnl The parameters to the wrapped functions have to match
dnl those in the system header files. I don't know how to
dnl really get the names of the paramters, but this seems to work.
AC_MSG_CHECKING([for return value and second and third argument of readlink])
readlink_buf_arg=unknown
readlink_bufsize_arg=unknown
for zeroth in ssize_t int; do
  for second in void char; do
    for third in size_t int; do
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <unistd.h>
#include <stdio.h>
        $zeroth readlink(const char *path, $second *buf, $third bufsiz);]], [[puts("hello, world");]])],
	[readlink_retval=$zeroth
	 readlink_buf_arg=$second
	 readlink_bufsize_arg=$third
        ],[])
    done
  done
done
AC_MSG_RESULT([$readlink_retval, $readlink_buf_arg, $readlink_bufsize_arg])
AH_TEMPLATE([READLINK_RETVAL_TYPE], [type of readlink return value])
AH_TEMPLATE([READLINK_BUF_TYPE], [type of readlink buf])
AH_TEMPLATE([READLINK_BUFSIZE_TYPE], [type of readlink bufsize])
AC_DEFINE_UNQUOTED(READLINK_RETVAL_TYPE, $readlink_retval)
AC_DEFINE_UNQUOTED(READLINK_BUF_TYPE, $readlink_buf_arg)
AC_DEFINE_UNQUOTED(READLINK_BUFSIZE_TYPE, $readlink_bufsize_arg)

AC_MSG_CHECKING([for first argument of setgroups])
setgroups_size_arg=unknown
for first in size_t int; do
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#define _BSD_SOURCE
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <unistd.h>
#include <stdio.h>
#ifdef HAVE_GRP_H
#include <grp.h>
#endif
  int setgroups($first size, const gid_t *list);]], [[puts("hello, world");]])],[setgroups_size_arg=$first],[])
done
AC_MSG_RESULT([$setgroups_size_arg])
AH_TEMPLATE([SETGROUPS_SIZE_TYPE], [type of setgroups size])
AC_DEFINE_UNQUOTED(SETGROUPS_SIZE_TYPE, $setgroups_size_arg)


AH_TEMPLATE([HAVE_SEMUN_DEF], [have the semun union])
AC_MSG_CHECKING([for union semun])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       # include <sys/types.h>
       # include <sys/ipc.h>
       # include <sys/sem.h>
  ]], [[
       union semun s;
  ]])],[AC_DEFINE(HAVE_SEMUN_DEF)
   AC_MSG_RESULT([yes])
  ],[ AC_MSG_RESULT([no])
  ])

AH_TEMPLATE([XMKNOD_FRTH_ARG], [fourth argument of __xmknod])
dnl glibc uses `* dev' as fourth argument of __xmknod.
dnl Although the test below should probably be more general
dnl (not just __xmknod, but also mknod etc), at the moment this
dnl seems enough, as probably glibc is the only that does this.
AC_CHECK_FUNC(__xmknod,[
AC_MSG_CHECKING([for type of arg of __xmknod])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <unistd.h>
  ]], [[
#ifndef __GLIBC__
#error no extra *
#endif
       int __xmknod  ( int ver,
                       const char *pathname ,
                       mode_t  mode ,  dev_t *dev);
  ]])],[
   dnl XXX: __xmknod is not declared in public headers in glibc 2.36 and newer.
   dnl Assume __xkmnod takes a dev_t pointer this case anyway.
   AC_DEFINE(XMKNOD_FRTH_ARG,[*])
   AC_MSG_RESULT([needs *])
  ],[
   AC_DEFINE(XMKNOD_FRTH_ARG,)
   AC_MSG_RESULT([no extra *])

  ])
],
[
 dnl glibc does not export  __xmknod symbol. In this case WRAP_MKNOD expands
 dnl to mknod, and XMKNOD_FRTH_ARG is acutally the 3rd argument of mknod,
 dnl which must be dev_t (not the pointer)
 AC_DEFINE(XMKNOD_FRTH_ARG,)
 AC_MSG_RESULT([no extra *])
])


dnl Possibly this should only be done if we actually have mknodat
dnl on the system.  Nothing  breaks by running the test itself though.
AH_TEMPLATE([XMKNODAT_FIFTH_ARG], [fifth argument of __xmknodat])
dnl glibc uses `* dev' as fifth argument of __xmknodat.
dnl Although the test below should probably be more general
dnl (not just __xmknodat, but also mknod etc), at the moment this
dnl seems enough, as probably glibc is the only that does this.

AC_CHECK_FUNC(__xmknodat,[
AC_MSG_CHECKING([for type of arg of __xmknodat])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <unistd.h>
  ]], [[
#ifndef __GLIBC__
#error no extra *
#endif
       int __xmknodat  ( int ver,
                         int dirfd,
                         const char *pathname ,
                         mode_t  mode ,  dev_t *dev);
  ]])],[
   AC_DEFINE(XMKNODAT_FIFTH_ARG,[*])
   AC_MSG_RESULT([needs *])
  ],[
   AC_DEFINE(XMKNODAT_FIFTH_ARG,)
   AC_MSG_RESULT([no extra *])

  ])
],
[
 dnl glibc does not export  __xmknodat symbol. In this case WRAP_MKNODAT expands
 dnl to mknodat, and XMKNODAT_FIFTH_ARG is acutally the 4th argument of mknodat,
 dnl which must be dev_t (not the pointer)
 AC_DEFINE(XMKNODAT_FIFTH_ARG,)
 AC_MSG_RESULT([no extra *])
])

AH_TEMPLATE([INITGROUPS_SECOND_ARG], [second argument of initgroups])
dnl FreeBSD 4.7 uses int instead of gid_t
AC_MSG_CHECKING([for type of arg of initgroups])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       #include <sys/types.h>
       #include <unistd.h>
  ]], [[
       int initgroups ( const char *user, gid_t group );
  ]])],[
   AC_DEFINE(INITGROUPS_SECOND_ARG, gid_t)
   AC_MSG_RESULT([gid_t])
  ],[
   AC_DEFINE(INITGROUPS_SECOND_ARG, int)
   AC_MSG_RESULT([not gid_t; will assume int])
  ])

AH_TEMPLATE([SETREUID_ARG], [argument of setreuid])
dnl OpenBSD 2.8 uses int instead of uid_t
AC_MSG_CHECKING([for type of arg of setreuid])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       #include <sys/types.h>
       #include <unistd.h>
  ]], [[
       int setreuid ( uid_t ruid, uid_t euid );
  ]])],[
   AC_DEFINE(SETREUID_ARG, gid_t)
   AC_MSG_RESULT([gid_t])
  ],[
   AC_DEFINE(SETREUID_ARG, int)
   AC_MSG_RESULT([not uid_t; will assume int])
  ])

AH_TEMPLATE([SETREGID_ARG], [argument of setregid])
dnl OpenBSD 2.8 uses int instead of gid_t
AC_MSG_CHECKING([for type of arg of setregid])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       #include <sys/types.h>
       #include <unistd.h>
  ]], [[
       int setreuid ( gid_t rgid, gid_t egid );
  ]])],[
   AC_DEFINE(SETREGID_ARG, gid_t)
   AC_MSG_RESULT([gid_t])
  ],[
   AC_DEFINE(SETREGID_ARG, int)
   AC_MSG_RESULT([not gid_t; will assume int])
  ])

AH_TEMPLATE([STAT_SECOND_ARG], [second argument of stat])
dnl Tru64 or something uses stat * instead of struct stat *
AC_MSG_CHECKING([for type of second arg to stat])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       #include <sys/stat.h>
       #include <sys/types.h>
       #include <unistd.h>
  ]], [[
       int stat ( const char *file_name, struct stat *buf);
  ]])],[
   AC_DEFINE(STAT_SECOND_ARG, struct stat *)
   AC_MSG_RESULT([struct stat *])
  ],[
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       #include <sys/stat.h>
       #include <sys/types.h>
       #include <unistd.h>
  ]], [[
       int stat ( const char *file_name, stat *buf);
  ]])],[
   AC_DEFINE(STAT_SECOND_ARG, stat *)
   AC_MSG_RESULT([stat *])
  ],[
  AC_MSG_ERROR(cannot determine second stat argument)
  ])
])

for field in d_off d_type; do
AC_MSG_CHECKING([for ${field} in struct dirent])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       #include <sys/types.h>
       #include <dirent.h>
  ]], [[

       struct dirent d;
       d.${field}=0
  ]])],[AC_DEFINE_UNQUOTED(STAT_HAS_${field},1)
   AC_MSG_RESULT([yes])
  ],[ AC_MSG_RESULT([no])
  ])
done

AH_TEMPLATE([HAVE_ACL_T], [acl_t data type and associated functions are provided by OS])
AC_MSG_CHECKING([for acl_t struct])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
       #include <sys/types.h>
       #include <sys/acl.h>
  ]], [[
       acl_t t;
  ]])],[AC_DEFINE_UNQUOTED(HAVE_ACL_T,1)
   AC_MSG_RESULT([yes])
  ],[ AC_MSG_RESULT([no])
  ])

AC_CHECK_FUNCS(fchmodat fchownat fstatat mkdirat mknodat openat renameat unlinkat lchmod fgetattrlist)

save_LIBS="$LIBS"
# Linux
AC_SEARCH_LIBS(acl_get_fd, acl)
AC_CHECK_FUNCS(acl_get_fd)

# Illumos
AC_SEARCH_LIBS(acl_trivial, sec)
AC_CHECK_FUNCS(acl_trivial)
LIBS="$save_LIBS"

AC_CHECK_FUNCS(capset listxattr llistxattr flistxattr getxattr lgetxattr fgetxattr setxattr lsetxattr fsetxattr removexattr lremovexattr fremovexattr)

AC_CHECK_FUNCS(statx)

dnl find out how stat() etc are called. On linux systems, we really
dnl need to wrap (IIRC):
dnl  Linux       : __xstat
dnl  Solaris <=9 : _stat
dnl  Solaris 10  : _xstat
dnl  Digital Unix: stat

time64_hack=no
AH_TEMPLATE([TIME64_HACK], [time64 shuffle])
AC_MSG_CHECKING([if we need to cope with time64])
AC_EGREP_CPP([time64],[
#include <bits/wordsize.h>
#if __WORDSIZE == 32
#define __USE_TIME_BITS64 1
#include <sys/stat.h>
stat
#else
NO
#endif
],
       [AC_MSG_RESULT([yes]); AC_DEFINE(TIME64_HACK)
time64_hack=yes
],
       [AC_MSG_RESULT([no]);])

:>fakerootconfig.h.tmp

dnl Before glibc 2.33 the stat family of functions (and mknod)
dnl used to be inline wrappers around calls to xstat, fxstat,
dnl lxstat, xmknod, which all take a leading version number
dnl argument designating the data structure and bits used.
dnl Thus libfakeroot needs to wrap xstat, fxstat, xmknod only.
dnl 
dnl In glibc 2.33 inline wrapper functions have been removed.
dnl Instead libc.so.6 exports stat, stat64, etc symbols.
dnl xstat, fxstat, xmknod still exist (for compatibility reasons).
dnl Thus libfakeroot must wrap both stat (fstat, statat, fstatat, etc)
dnl and internal xstat (fxstat, etc) functions.
dnl
dnl However some new architectures (such as LoongArch lp64d ABI)
dnl decided to be 64-bit only since the day 0 and don't use any wrappers.
dnl Here libfakeroot should wrap only stat (statat, fstat, fstatat).
dnl A special care should be taken to avoid the double definition.

for SEARCH in %stat f%stat l%stat f%statat %stat64 f%stat64 l%stat64 f%statat64 %mknod %mknodat; do
  FUNC=`echo $SEARCH|sed -e 's/.*%//'`
  PRE=`echo $SEARCH|sed -e 's/%.*//'`
  FOUND=
  for WRAPPED in __${PRE}x${FUNC} _${PRE}x${FUNC} __${PRE}${FUNC}13 ${PRE}${FUNC} __${PRE}${FUNC}; do
    AC_CHECK_FUNCS($WRAPPED,FOUND=$WRAPPED)
dnl
dnl to unconditionally define only the _* functions, comment out the 2 lines above,
dnl and uncomment the 2 lines below.
dnl
dnl  for WRAPPED in _${PRE}${FUNC}; do
dnl    FOUND=$WRAPPED
    if test -n "$FOUND"; then
      PF=[`echo ${PRE}${FUNC}| tr '[a-z]' '[A-Z]'`]
      DEFINE_WRAP=[`echo wrap_${PRE}${FUNC}| tr '[a-z]' '[A-Z]'`]
      DEFINE_NEXT=[`echo wrap_${FOUND}| tr '[a-z]' '[A-Z]'`]
      DEFINE_ARG=[`echo wrap_${FOUND}| tr '[a-z]' '[A-Z]'`]
      AC_DEFINE_UNQUOTED(WRAP_${PF}, $FOUND)
      AC_DEFINE_UNQUOTED(WRAP_${PF}_RAW, $FOUND)
      AC_DEFINE_UNQUOTED(WRAP_${PF}_QUOTE, "$FOUND")
      AC_DEFINE_UNQUOTED(TMP_${PF}, tmp_$FOUND)
      AC_DEFINE_UNQUOTED(NEXT_${PF}_NOARG, next_$FOUND)
      if test __"${PRE}x${FUNC}" != "${WRAPPED}" && test _"${PRE}x${FUNC}" != "${WRAPPED}" ; then
        DEF_BEGIN=""
      else
        DEF_BEGIN="a,"
      fi
      if test "${FUNC}" = "mknod"; then
        DEF_END=",d"
      elif test "${FUNC}" = "mknodat"; then
        DEF_END=",d,e"
      elif test "${FUNC}" = "statat"; then
        DEF_END=",d,e"
      elif test "${FUNC}" = "statat64"; then
        DEF_END=",d,e"
      else
        DEF_END=""
      fi
      dnl no matter what I do, the resulting define looks like
      dnl #define macro (a,b,c) (a,b,c)
      dnl with a space in between "macro" and "(". To prevent this,
      dnl I leave the space here, and remove it later with sed
      dnl at (end of configure.in)
      dnl AC_DEFINE_UNQUOTED(NEXT_${PF}(a,b,c${DEF_END}),
      dnl		     next_$FOUND(${DEF_BEGIN}b,c${DEF_END}))
      dnl AC_DEFINE_UNQUOTED(${PF}[_ARG(a,b,c${DEF_END})],
      dnl		     (${DEF_BEGIN}b,c${DEF_END}))

      dnl Anyway the trickery above also leads to automake problems
      dnl (tries to remake config.h.in, but fails). So, as a way
      dnl out, Yann DIRSON wrote this:
      {
       echo "#define NEXT_${PF}(a,b,c${DEF_END}) next_$FOUND(${DEF_BEGIN}b,c${DEF_END})"
       echo "#define ${PF}_ARG(a,b,c${DEF_END}) (${DEF_BEGIN}b,c${DEF_END})"
      } >>fakerootconfig.h.tmp

      if test "x${FOUND}" = "x${PRE}${FUNC}"; then
         dnl Some architectures are 64-bit only from day 0 and don't have xstat, etc
         dnl internal functions. For such architectures WRAP_STAT evaluates to stat,
         dnl so an extra wrapper is NOT required.
	 AC_MSG_RESULT([Extra wrapper for ${PRE}${FUNC} is not necessary])
	 AC_DEFINE_UNQUOTED(NO_WRAP_${PF}_SYMBOL, 1)
      else
	 AC_MSG_RESULT([Extra wrapper for ${PRE}${FUNC} is required because of ${FOUND}])
      fi
      break
    fi
  done
done

if test "x$time64_hack" = "xyes"; then
for SEARCH in stat64_time64 fstat64_time64 lstat64_time64 fstatat64_time64; do
  FOUND=
  for WRAPPED in __${SEARCH} ${SEARCH}; do
    AC_CHECK_FUNCS($WRAPPED,FOUND=$WRAPPED)
    if test -n "$FOUND"; then
      PF=[`echo ${SEARCH}| tr '[a-z]' '[A-Z]'`]
      DEFINE_WRAP=[`echo wrap_${SEARCH}| tr '[a-z]' '[A-Z]'`]
      DEFINE_NEXT=[`echo wrap_${FOUND}| tr '[a-z]' '[A-Z]'`]
      DEFINE_ARG=[`echo wrap_${FOUND}| tr '[a-z]' '[A-Z]'`]
      AC_DEFINE_UNQUOTED(WRAP_${PF}, $FOUND)
      AC_DEFINE_UNQUOTED(WRAP_${PF}_RAW, $FOUND)
      AC_DEFINE_UNQUOTED(WRAP_${PF}_QUOTE, "$FOUND")
      AC_DEFINE_UNQUOTED(TMP_${PF}, tmp_$FOUND)
      AC_DEFINE_UNQUOTED(NEXT_${PF}_NOARG, next_$FOUND)
      DEF_BEGIN=""
      case "$SEARCH" in
	     (*statat64_time64)
	     DEF_END=",d,e"
	     ;;
	     (*)
	     DEF_END=""
	     ;;
      esac
      {
       echo "#define NEXT_${PF}(a,b,c${DEF_END}) next_$FOUND(${DEF_BEGIN}b,c${DEF_END})"
       echo "#define ${PF}_ARG(a,b,c${DEF_END}) (${DEF_BEGIN}b,c${DEF_END})"
      } >>fakerootconfig.h.tmp

      break
    fi
  done
done
fi

if test -r fakerootconfig.h
then
       if test "`diff fakerootconfig.h fakerootconfig.h.tmp`" = ""
       then
               AC_MSG_RESULT([fakerootconfig.h not changed])
               rm fakerootconfig.h.tmp
       else
               AC_MSG_RESULT([recreated fakerootconfig.h])
               rm fakerootconfig.h ; mv fakerootconfig.h.tmp fakerootconfig.h
       fi
else
       AC_MSG_RESULT([created fakerootconfig.h])
       mv fakerootconfig.h.tmp fakerootconfig.h
fi

dnl This should really be done intelligently.
DLSUFFIX=".so"
LDLIBPATHVAR="LD_LIBRARY_PATH"
LDPRELOADVAR="LD_PRELOAD"
LDPRELOADABS=0
LDEXTRAVAR=""
case $target_cpu:$target_os in
	(alpha*:linux*|ia64*:linux*)
		libcpath="/lib/libc.so.6.1"
		;;
	(*:linux*)
	        libcpath="/lib/libc.so.6"
		;;
	(*:k*bsd*-gnu)
	        libcpath="/lib/libc.so.0.1"
		;;
	(*:freebsd*)
		libcpath="/usr/lib/libc.so.4"
		;;
	(*:netbsd*)
		libcpath="/usr/lib/libc.so.12"
		;;
	(*:openbsd*|*:mirbsd*)
		libcpath=$(/bin/ls -1 /usr/lib/libc.so.* | \
		    sort -nt. -k3 | tail -1)
		;;
	(*:midnightbsd*)
		libcpath=$(/bin/ls -1 /lib/libc.so.* | \
		    sort -nt. -k3 | tail -1)
		;;
	(*:hpux*)
		libcpath="/usr/lib/hpux32/libc.so.1"
		;;
	(*:osf*)
		libcpath="/shlib/libc.so"
		;;
	(*:solaris*)
		libcpath="/lib/libc.so.1"
		;;
	(*:darwin*)
		libcpath="/usr/lib/libSystem.dylib"
		DLSUFFIX=".dylib"
		LDLIBPATHVAR="DYLD_LIBRARY_PATH"
		LDPRELOADVAR="DYLD_INSERT_LIBRARIES"
		LDPRELOADABS=1
		;;
	(*)
		AC_MSG_WARN([don't know where libc is for $target_os on
			     $target_cpu, setting to /lib/libc.so])
		libcpath="/lib/libc.so"
		;;
esac

AC_DEFINE_UNQUOTED([LIBCPATH], "$libcpath", [path to libc shared object])
AC_SUBST(DLSUFFIX)
AC_SUBST(LDLIBPATHVAR)
AC_SUBST(LDPRELOADVAR)
AC_SUBST(LDPRELOADABS)
AC_SUBST(LDEXTRAVAR)

dnl Checks for library functions.
AC_CHECK_FUNCS(strdup strstr getresuid setresuid getresgid setresgid setfsuid setfsgid fts_read fts_children)

AC_CHECK_DECLS([setenv, unsetenv])
AC_REPLACE_FUNCS([setenv])

dnl Checks for __builtin_expect
AC_LINK_IFELSE([AC_LANG_PROGRAM([[int foo (int a) { a = __builtin_expect (a, 10); return a == 10 ? 0 : 1; }]],
     [[]])],[AC_DEFINE([HAVE_BUILTIN_EXPECT], 1,
     [Define to 1 if the compiler understands __builtin_expect.])],[])

dnl kludge
AH_VERBATIM([WRAP_STAT],
[/* Stuff.  */
#define WRAP_STAT  __astat
#define WRAP_STAT_QUOTE  __astat
#define WRAP_STAT_RAW  __astat
#define TMP_STAT  __astat
#define NEXT_STAT_NOARG  next___astat
#undef NO_WRAP_STAT_SYMBOL

#define WRAP_LSTAT_QUOTE  __astat
#define WRAP_LSTAT  __astat
#define WRAP_LSTAT_RAW  __astat
#define TMP_LSTAT  __astat
#define NEXT_LSTAT_NOARG  next___astat
#undef NO_WRAP_LSTAT_SYMBOL

#define WRAP_FSTAT_QUOTE  __astat
#define WRAP_FSTAT  __astat
#define WRAP_FSTAT_RAW  __astat
#define TMP_FSTAT  __astat
#define NEXT_FSTAT_NOARG  next___astat
#undef NO_WRAP_FSTAT_SYMBOL

#define WRAP_FSTATAT_QUOTE  __astatat
#define WRAP_FSTATAT  __astatat
#define WRAP_FSTATAT_RAW  __astatat
#define TMP_FSTATAT  __astatat
#define NEXT_FSTATAT_NOARG  next___astatat
#undef NO_WRAP_FSTATAT_SYMBOL

#define WRAP_STAT64_QUOTE  __astat64
#define WRAP_STAT64  __astat64
#define WRAP_STAT64_RAW  __astat64
#define TMP_STAT64  __astat64
#define NEXT_STAT64_NOARG  next___astat64
#undef NO_WRAP_STAT64_SYMBOL

#define WRAP_LSTAT64_QUOTE  __astat64
#define WRAP_LSTAT64  __astat64
#define WRAP_LSTAT64_RAW  __astat64
#define TMP_LSTAT64  __astat64
#define NEXT_LSTAT64_NOARG  next___astat64
#undef NO_WRAP_LSTAT64_SYMBOL

#define WRAP_FSTAT64_QUOTE  __astat64
#define WRAP_FSTAT64  __astat64
#define WRAP_FSTAT64_RAW  __astat64
#define TMP_FSTAT64  __astat64
#define NEXT_FSTAT64_NOARG  next___astat64
#undef NO_WRAP_FSTAT64_SYMBOL

#define WRAP_FSTATAT64_QUOTE  __astatat64
#define WRAP_FSTATAT64  __astatat64
#define WRAP_FSTATAT64_RAW  __astatat64
#define TMP_FSTATAT64  __astatat64
#define NEXT_FSTATAT64_NOARG  next___astatat64
#undef NO_WRAP_FSTATAT64_SYMBOL

#define WRAP_MKNOD_QUOTE  __amknod
#define WRAP_MKNOD  __amknod
#define WRAP_MKNOD_RAW  __amknod
#define TMP_MKNOD  __amknod
#define NEXT_MKNOD_NOARG  next___amknod
#undef NO_WRAP_MKNOD_SYMBOL

#define WRAP_MKNODAT_QUOTE  __amknodat
#define WRAP_MKNODAT  __amknodat
#define WRAP_MKNODAT_RAW  __amknodat
#define TMP_MKNODAT  __amknodat
#define NEXT_MKNODAT_NOARG  next___amknodat
#undef NO_WRAP_MKNODAT_SYMBOL

#define WRAP_STAT64_TIME64_QUOTE  __astat64_time64
#define WRAP_STAT64_TIME64  __astat64_time64
#define WRAP_STAT64_TIME64_RAW  __astat64_time64
#define TMP_STAT64_TIME64  __astat64_time64
#define NEXT_STAT64_TIME64_NOARG  next___astat64_time64

#define WRAP_LSTAT64_TIME64_QUOTE  __astat64_time64
#define WRAP_LSTAT64_TIME64  __astat64_time64
#define WRAP_LSTAT64_TIME64_RAW  __astat64_time64
#define TMP_LSTAT64_TIME64  __astat64_time64
#define NEXT_LSTAT64_TIME64_NOARG  next___astat64_time64

#define WRAP_FSTAT64_TIME64_QUOTE  __astat64_time64
#define WRAP_FSTAT64_TIME64  __astat64_time64
#define WRAP_FSTAT64_TIME64_RAW  __astat64_time64
#define TMP_FSTAT64_TIME64  __astat64_time64
#define NEXT_FSTAT64_TIME64_NOARG  next___astat64_time64

#define WRAP_FSTATAT64_TIME64_QUOTE  __astatat64_time64
#define WRAP_FSTATAT64_TIME64  __astat64_time64
#define WRAP_FSTATAT64_TIME64_RAW  __astat64_time64
#define TMP_FSTATAT64_TIME64  __astat64_time64
#define NEXT_FSTATAT64_TIME64_NOARG  next___astat64_time64

])
dnl kludge end

case "$target_cpu:$target_os" in
	(alpha*:linux*)
	AH_TEMPLATE([STUPID_ALPHA_HACK], [stat-struct conversion hackery])
	AC_MSG_CHECKING([if we need to do stat-struct conversion hackery])
	AC_EGREP_CPP([3:3],[
#include <sys/stat.h>
_STAT_VER:_STAT_VER_GLIBC2_3_4
],
       [AC_MSG_RESULT([yes]); AC_DEFINE(STUPID_ALPHA_HACK)
CPPFLAGS="$CPPFLAGS -I\$(srcdir)/statconv/glibc/linux/alpha"
],
       [AC_MSG_RESULT([no]);])
        ;;
esac

dnl AH_TEMPLATE([MACOSX], [is __APPLE__ defined by the compiler])
AC_MSG_CHECKING([for macOS/Darwin])
  AC_PREPROC_IFELSE(
    [AC_LANG_PROGRAM([[
       #ifndef __APPLE__
       #error Not Apple
       #endif
  ]])],
    [macosx=true
     AC_MSG_RESULT([yes])],
    [AC_MSG_RESULT([no])
     macosx=false]
  )
AM_CONDITIONAL([MACOSX], [test x$macosx = xtrue])

AC_CONFIG_FILES([
   Makefile
   scripts/Makefile
   doc/Makefile
   doc/de/Makefile doc/es/Makefile doc/fr/Makefile doc/nl/Makefile doc/pt/Makefile doc/sv/Makefile
   test/Makefile test/defs])
AC_OUTPUT

dnl Local variables:
dnl mode: m4
dnl End:
