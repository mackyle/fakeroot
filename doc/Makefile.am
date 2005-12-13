AUTOMAKE_OPTIONS=foreign
SUBDIRS = es fr sv

man_MANS = faked.1 fakeroot.1

#another automake bug workaround
EXTRA_DIST = $(man_MANS)

all-recursive: po4a

po4a:
	@$(NORMAL_INSTALL)
	po4a -k 0 --variable "srcdir=$(srcdir)/" $(srcdir)/po4a/po4a.cfg

clean-recursive: po4a-clean

po4a-clean:
	@$(NORMAL_INSTALL)
	po4a -k 0 --variable "srcdir=$(srcdir)/" \
	   --rm-translations $(srcdir)/po4a/po4a.cfg

.PHONY: po4a po4a-clean
