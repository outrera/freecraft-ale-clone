##
##	A clone of a famous game.
##
##	Makefile	-	The make file.
##
##	(c) Copyright 1998,1999 by Lutz Sammer
##
##	$Id: Makefile,v 1.16 1999/11/26 17:42:33 root Exp $
##

TOPDIR=	..
include $(TOPDIR)/Rules.make

OBJS=

MODULES=	clone map unit action ai ui sound video network pathfinder

all::
	@set -e; for i in $(MODULES) ; do make -C $$i all ; done

doc::	
	@set -e; for i in $(MODULES) include ; do make -C $$i doc ; done

clean::
	@set -e; for i in $(MODULES) include ; do make -C $$i clean ; done
	$(RM) $(OBJS) libclone.a

clobber::	clean
	@set -e; for i in $(MODULES) include ; do make -C $$i clobber ; done
	$(RM) .depend tags

depend::
	@echo -n >.depend
	@set -e; for i in $(MODULES) ; do make -C $$i depend ; done

tags::
	@set -e; for i in $(MODULES) ; do cd $$i ; make tags ; cd .. ; done

distlist::
	echo >>$(DISTLIST)
	echo src/Makefile >>$(DISTLIST)
	@for i in include $(MODULES) ; do make -C $$i distlist ; done

ci::
	@for i in include $(MODULES) ; do make -C $$i ci ; done
	ci -l Makefile

lockver::
	@for i in include $(MODULES) ; do make -C $$i lockver ; done
	$(LOCKVER) Makefile

ifeq (.depend,$(wildcard .depend))
include .depend
endif
