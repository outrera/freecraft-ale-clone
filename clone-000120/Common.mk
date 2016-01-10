##
##	A clone of a famous game.
##
##	Common.mk        -       Common make (GNU Make)
##
##	(c) Copyright 1998 by Lutz Sammer
##
##	$Id: Common.mk,v 1.7 1999/05/04 17:00:30 root Exp $
##

all:	$(OBJS)

doc:	$(OBJS:.o=.doc)

clean::
	$(RM) $(OBJS) core *.doc

clobber: clean
	$(RM) depend

depend::
	@echo -n >.depend
	@for i in $(OBJS:.o=.c) ; do $(CC) -MM $(GLIB) $(IFLAGS) $$i >>.depend ; done

tags::
	@for i in $(OBJS:.o=.c) ; do\
	cd .. ;\
	ctags $(CTAGSFLAGS) $(TAGS) $(MODULE)/$$i ;\
	cd $(MODULE) ;\
	done

ci::
	ci -l $(OBJS:.o=.c) Makefile

lockver::
	$(LOCKVER) $(OBJS:.o=.c) Makefile

distlist::
	echo >>$(DISTLIST)
	@for i in `echo $(OBJS:.o=.c)` ; do \
	echo src/$(MODULE)/$$i >>$(DISTLIST) ; done
	echo src/$(MODULE)/Makefile >>$(DISTLIST)

$(OBJS): $(TOPDIR)/Rules.make

#
#	include dependency files they exist
#
ifeq (.depend,$(wildcard .depend))
include .depend
endif
