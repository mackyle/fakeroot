AUTOMAKE_OPTIONS=foreign
ACLOCAL_AMFLAGS = -I build-aux
SUBDIRS=doc scripts test

noinst_LTLIBRARIES = libcommunicate.la libmacosx.la libfakeroot_time64.la
libcommunicate_la_SOURCES = communicate.c

libmacosx_la_SOURCES = libfakeroot_inode64.c libfakeroot_unix2003.c patchattr.h

libfakeroot_time64_la_SOURCES = libfakeroot_time64.c
libfakeroot_time64_la_CPPFLAGS = -D_FILE_OFFSET_BITS=64 -D_TIME_BITS=64

lib_LTLIBRARIES=libfakeroot.la
libfakeroot_la_SOURCES=libfakeroot.c statconv/glibc/linux/alpha/stats.h wrapdef.h  wrapstruct.h communicate.h
libfakeroot_la_LDFLAGS=-release 0
if MACOSX
libfakeroot_la_DEPENDENCIES=wrapdef.h wrapstruct.h libfakeroot_time64.la libcommunicate.la libmacosx.la $(LTLIBOBJS)
libfakeroot_la_LIBADD = libfakeroot_time64.la libcommunicate.la libmacosx.la $(LTLIBOBJS)
else !MACOSX
libfakeroot_la_DEPENDENCIES=wrapdef.h wrapstruct.h libfakeroot_time64.la libcommunicate.la $(LTLIBOBJS)
libfakeroot_la_LIBADD = libfakeroot_time64.la libcommunicate.la $(LTLIBOBJS)
endif !MACOSX

bin_PROGRAMS=faked
faked_SOURCES = faked.c
faked_LDADD = libcommunicate.la

simple_SOURCES=simple.c
noinst_PROGRAMS=simple

CLEANFILES= wrapdef.h wrapstruct.h wrapped.h wraptmpf.h
DISTCLEANFILES = fakerootconfig.h

EXTRA_DIST=wrapawk wrapawk_macosx wrapfunc.inp           \
	   message.h \
	   DEBUG BUGS README_MACOSX.txt \
	   setenv.c \
	   bootstrap \
	   preroll \
	   contrib/Fakeroot-Stat-1.8.8/MANIFEST \
	   contrib/Fakeroot-Stat-1.8.8/META.yml \
	   contrib/Fakeroot-Stat-1.8.8/Makefile.PL \
	   contrib/Fakeroot-Stat-1.8.8/README \
	   contrib/Fakeroot-Stat-1.8.8/Stat.xs \
	   contrib/Fakeroot-Stat-1.8.8/lib/Fakeroot/Stat.pm \
	   contrib/Fakeroot-Stat-1.8.8/t/Fakeroot-Stat.t

CLEAN_FILES=fakerootconfig.h

if MACOSX
wrapped.h wrapdef.h wrapstruct.h wraptmpf.h:wrapawk_macosx wrapfunc.inp
	awk -f $(srcdir)/wrapawk_macosx < $(srcdir)/wrapfunc.inp
else !MACOSX
wrapped.h wrapdef.h wrapstruct.h wraptmpf.h:wrapawk wrapfunc.inp
	awk -f $(srcdir)/wrapawk < $(srcdir)/wrapfunc.inp
endif !MACOSX

libfakeroot.lo:libfakeroot.c wrapdef.h wrapstruct.h wraptmpf.h

fakerootconfig.h: ./config.status
	CONFIG_FILES= CONFIG_HEADERS= /bin/sh ./config.status


#Local variables:
#mode: makefile
#End:
