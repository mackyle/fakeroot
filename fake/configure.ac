dnl Process this file with autoconf to produce a configure script.
AC_INIT([fakeroot],[FAKEROOT_VERSION],[schizo@debian.org],[fakeroot])
AC_CONFIG_SRCDIR([libfakeroot.c])
AC_CANONICAL_TARGET
AC_CONFIG_AUX_DIR(..)

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl AM_CONFIG_HEADER(config.h)

AM_PROG_LIBTOOL
AC_PROG_CC
dnl Checks for programs.

dnl Checks for libraries.
dnl Replace `main' with a function in -ldl:
AC_CHECK_LIB(dl, dlopen)


AC_CONFIG_FILES([Makefile])
AC_OUTPUT

dnl Local variables:
dnl mode: m4
dnl End:
