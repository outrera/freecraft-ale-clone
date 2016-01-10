##
##	A clone of a famous game.
##
##	Makefile	-	Makefile for tools. (Level 2)
##
##	(c) Copyright 1998 by Lutz Sammer
##
##	$Id: Makefile,v 1.11 1999/12/09 16:23:00 root Exp $
##

TOPDIR= ..
include $(TOPDIR)/Rules.make

EXECS=	gfx2png$(EXE) gfu2png$(EXE) img2png$(EXE) cur2png$(EXE) fnt2png$(EXE) \
	tile2png$(EXE) extract$(EXE) aledoc$(EXE) wartool$(EXE)

all::	aledoc$(EXE) wartool$(EXE)

old::	$(EXECS)

clean::

clobber:: clean
	$(RM) $(EXECS)

gfx2png$(EXE): gfx2png.c
	$(CC) $(CFLAGS) $(TOOLLIBS) -DGFX -o $< gfx2png.c

gfu2png$(EXE): gfx2png.c
	$(CC) $(CFLAGS) $(TOOLLIBS) -DGFU -o $< gfx2png.c

img2png$(EXE): gfx2png.c
	$(CC) $(CFLAGS) $(TOOLLIBS) -DIMAGE -o $< gfx2png.c

cur2png$(EXE): gfx2png.c
	$(CC) $(CFLAGS) $(TOOLLIBS) -DCUR -o $< gfx2png.c

fnt2png$(EXE): gfx2png.c
	$(CC) $(CFLAGS) $(TOOLLIBS) -DFNT -o $< gfx2png.c

tile2png$(EXE): tile2png.c
	$(CC) $(CFLAGS) $(TOOLLIBS) -DTILE -o $< tile2png.c

extract$(EXE): extract.c
	$(CC) $(CFLAGS) $(TOOLLIBS) -o $< extract.c

aledoc$(EXE):	aledoc.c
	$(CC) $(CFLAGS) -o $@ $< $(TOOLLIBS)

wartool$(EXE): wartool.c
	$(CC) $(CFLAGS) -o $@ $< $(TOOLLIBS)

FILES= 	gfx2png.c tile2png.c extract.c build-old.sh \
	build.sh Makefile aledoc.c wartool.c build.bat

distlist:
	for i in $(FILES); do echo tools/$$i >>$(DISTLIST); done

ci:
	ci -l $(FILES)

lockver:
	$(LOCKVER) $(FILES)
