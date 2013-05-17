AUTOMAKE_OPTIONS=foreign

TESTS =						\
  t.chmod_dev					\
  t.echoarg					\
  t.falsereturn					\
  t.mknod					\
  t.no_ld_preload				\
  t.no_ld_preload_link				\
  t.option					\
  t.tar						\
  t.touchinstall				\
  t.truereturn \
  t.xattr

suffix =
TESTS_ENVIRONMENT =				\
  libfakeroot=libfakeroot-0$(DLSUFFIX)		\
  srcdir=$(srcdir)				\
  suffix=$(suffix)				\
  posixshell=$(SHELL)

CLEANFILES = tmp* tartest.tar.gz hda3

check_SCRIPTS = $(TESTS) echo_arg tartest compare-tar defs
EXTRA_DIST = $(check_SCRIPTS) tartest.tar.gz.uue
