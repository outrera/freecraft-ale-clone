##
##	A clone of a famous game.
##
##	Makefile	-	The make file (3 level).
##
##	(c) Copyright 1998,1999 by Lutz Sammer
##
##	$Id: Makefile,v 1.17 1999/11/26 17:42:21 root Exp $
##

TOPDIR= ../..
include $(TOPDIR)/Rules.make

MODULE	= include

OBJS	=

HDRS	= actions.h ai.h ccl.h clone.h construct.h cursor.h font.h icons.h \
	  image.h interface.h map.h minimap.h missile.h pathfinder.h player.h \
	  pud.h tileset.h unit.h unittype.h upgrade.h upgrade_structs.h \
	  ccl_sound.h sound.h sound_id.h unitsound.h wav.h sound_server.h \
	  video.h new_video.h network.h goal.h ui.h button.h \
	  etlib/generic.h etlib/xmalloc.h etlib/hash.h etlib/dllist.h rdtsc.h

all:
	echo done.

doc:	$(HDRS:.h=.doc)

distlist::
	#echo src/include/Makefile >>$(DISTLIST)
	@for i in $(HDRS) ; do echo src/include/$$i >>$(DISTLIST) ; done

ci::
	@ci -l Makefile
	@for i in $(HDRS) ; do ci -l $$i ; done

lockver::
	@for i in $(HDRS) ; do $(LOCKVER) $$i ; done
	@$(LOCKVER) Makefile

include $(TOPDIR)/Common.mk
