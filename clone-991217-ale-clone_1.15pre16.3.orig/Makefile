##
##	A clone of a famous game.
##
##	Makefile	-	The make file.
##
##	(c) Copyright 1998,1999 by Lutz Sammer
##
##	$Id: Makefile,v 1.35 1999/12/03 09:18:29 root Exp $
##

TOPDIR=	.

include $(TOPDIR)/Rules.make

MAKE=	make TOPDIR=`pwd`
MODULES= src tools

all:	src clone tools

help:
	@-echo "make cycle			clean,depend,tags,all"
	@-echo "make install			install all files"
	@-echo "make uninstall			uninstall all files"
	@-echo "make run			create and run"
	@-echo "make runp			create and run with profiler"
	@-echo "make clean			cleanup keep only executables"
	@-echo "make clobber			clean all files"
	@-echo "make ci				check in RCS"
	@-echo "make doc			make source documention with doxygen"
	@-echo "make doc++			make source documention with doc++"
	@-echo "make lockver NAME="version"	label current version with symbolic name"
	@-echo "make tags			create ctags"
	@-echo "make depend			create dependencies"
	@-echo "make dist			create distribution"
	@-echo "make small-dist			create small distribution"
	@-echo "make buildit			create data files from original data"
	@-echo "make buildclean			cleanup build data files"

cycle::
	@$(MAKE) clean
	@$(MAKE) depend
	@$(MAKE) tags
	@$(MAKE) all

run::
	@$(MAKE) && ./clone

runp::
	@$(MAKE) && ./clone && if [ -e gmon.sum ]; then \
		gprof -s clone gmon.out gmon.sum; \
	    else mv gmon.out gmon.sum; fi

doc::
	doxygen doxygen-clone.cfg

doc++::
	@$(MAKE) -C src doc
	@if [ ! -d srcdoc ]; then mkdir srcdoc; fi
	@$(DOCPP) -v -H -A -a -b -j -d srcdoc `find . -name "*.doc" -print`

src::
	@$(MAKE) -C src all

etlib/hash.o: etlib/hash.c

clone:	src etlib/hash.o src/libclone.a 
	$(CC) -Lsrc -lclone $(CLONELIBS) -o clone -I. $(CFLAGS)

# -L. -lefence 
# -Lccmalloc-0.2.3/src -lccmalloc -ldl 

tools::
	@$(MAKE) -C tools all

clean::
	@set -e; for i in $(MODULES) ; do $(MAKE) -C $$i clean ; done
	$(RM) core gmon.out *.doc etlib/*.o

clobber:	clean
	@set -e; for i in $(MODULES) ; do $(MAKE) -C $$i clobber ; done
	$(RM) clone gmon.sum
	$(RM) -r srcdoc/*

distclean:	clobber
	@echo

ci::
	@set -e; for i in $(MODULES) ; do $(MAKE) -C $$i ci ; done
	ci -l Makefile Common.mk Rules.make .indent.pro \
	doxygen-clone.cfg doxygen-0.4.diff \
	$(CCLS) $(DOCS)

lockver::
	@set -e; for i in $(MODULES) ; do $(MAKE) -C $$i lockver ; done
	$(LOCKVER) Makefile Common.mk Rules.make .indent.pro \
	doxygen-clone.cfg doxygen-0.4.diff \
	$(CCLS) $(DOCS)

tags::
	@$(MAKE) -C src tags

depend::
	@$(MAKE) -C src depend

##############################################################################
#	
##############################################################################

DOCS    = README doc/readme.html doc/install.html doc/clone.html doc/faq.html \
	  doc/ChangeLog.html doc/LICENSE doc/todo.html doc/clone.lsm \
	  doc/development.html \
	  make/README doc/graphic/*.html doc/graphic/*.png

PICS    = contrib/ale-title.png

CCLS	= data/ccl/clone.ccl data/ccl/units.ccl data/ccl/missiles.ccl \
	  data/ccl/tilesets.ccl data/ccl/sound.ccl

MISC    = Makefile Common.mk Rules.make doxygen-clone.cfg doxygen-0.4.diff \
	  .indent.pro make/common.scc make/rules.scc make/makefile.scc \
          contrib/cross.png contrib/health.png contrib/mana.png \
	  tools/udta.c tools/ugrd.c contrib/req.cm \
	  etlib/hash.c

distdir= clone-`date +%y%m%d`
dist::
	$(RM) $(DISTLIST)
	@set -e; for i in $(MODULES); do $(MAKE) -C $$i distlist ; done
	echo >>$(DISTLIST)
	echo $(PICS) >>$(DISTLIST)
	echo $(MISC) >>$(DISTLIST)
	echo $(CCLS) >>$(DISTLIST)
	echo $(DOCS) >>$(DISTLIST)
	rm -rf $(distdir)
	mkdir $(distdir)
	chmod 777 $(distdir)
	for i in `cat $(DISTLIST)`; do echo $$i; done | cpio -pdml $(distdir) 
	chmod -R a+r $(distdir)
	tar chzf $(distdir).tar.gz $(distdir)
	$(RM) $(DISTLIST)
	$(RM) -r $(distdir)

small-dist::
	@$(RM) $(DISTLIST)
	$(MAKE) -C src distlist
	$(MAKE) -C tools distlist
	echo $(MISC) >>$(DISTLIST)
	echo $(CCLS) >>$(DISTLIST)
	echo $(DOCS) >>$(DISTLIST)
	rm -rf $(distdir)
	mkdir $(distdir)
	chmod 777 $(distdir)
	for i in `cat $(DISTLIST)`; do echo $$i; done | cpio -pdml $(distdir) 
	chmod -R a+r $(distdir)
	tar chzf $(distdir).tar.gz $(distdir)
	$(RM) $(DISTLIST)
	$(RM) -r $(distdir)

difffile=	clone-`date +%y%m%d`.diff
diff:	
	@$(RM) $(difffile)
	@$(RM) $(DISTLIST)
	$(MAKE) -C src distlist
	$(MAKE) -C tools distlist
	echo $(MISC) >>$(DISTLIST)
	echo $(CCLS) >>$(DISTLIST)
	echo $(DOCS) >>$(DISTLIST)
	rcsdiff -u `cat $(DISTLIST)` > $(difffile)

buildit:	tools
	. tools/build.sh

buildclean:
	rm -rf data/*.rgb data/puds data/sound data/graphic \
	data/interface data/campaigns

##############################################################################
#	INSTALL/UNINSTALL	
##############################################################################

install:	all
	@echo "under construction: make it by hand :)"

uninstall:
	@echo "under construction: make it by hand :)"
