AUTOMAKE_OPTIONS=foreign
SUBDIRS = de es fr nl pt sv

man_MANS = faked.1 fakeroot.1

#another automake bug workaround
EXTRA_DIST = $(man_MANS) README.saving

dist-hook:
	cp -r $(srcdir)/po4a $(distdir)
	rm -rf `find $(distdir) -name .arch-ids`
	cd $(distdir) && po4a -k 0 --rm-backups --variable "srcdir=$(realpath $(distdir))/" po4a/po4a.cfg

#all-recursive: po4a

#po4a:
#	@$(NORMAL_INSTALL)
#	po4a -k 0 --variable "srcdir=$(srcdir)/" $(srcdir)/po4a/po4a.cfg
#
#clean-recursive: po4a-clean
#
#po4a-clean:
#	@$(NORMAL_INSTALL)
#	po4a -k 0 --variable "srcdir=$(srcdir)/" \
#	   --rm-translations $(srcdir)/po4a/po4a.cfg
#
#.PHONY: po4a po4a-clean
