AUTOMAKE_OPTIONS=foreign

TESTS = t.echoarg t.mknod t.tar t.truereturn t.falsereturn t.option \
	t.touchinstall t.no_ld_preload t.no_ld_preload_link t.chmod_dev

suffix =
TESTS_ENVIRONMENT = libfakeroot=libfakeroot-0.so suffix=$(suffix) posixshell=$(SHELL)

CLEANFILES = tmp* tartest.tar.gz hda3

check_SCRIPTS = $(TESTS) echo_arg tartest compare-tar
EXTRA_DIST = $(check_SCRIPTS) tartest.tar.gz.uue
