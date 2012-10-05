ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src/helpers src/interpolation src/statisticalmean src test

ebrowse:
	make -C src ebrowse

EXTRA_DIST = kvqc2d.1 debian singlecheck.sh

libexec_SCRIPTS = plu-update-from-stinfosys plu-autoupdate-from-stinfosys
