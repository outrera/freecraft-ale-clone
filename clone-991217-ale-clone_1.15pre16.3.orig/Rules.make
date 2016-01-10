##
##      A clone of a famous game.
##	
##	Rules.make	-	Make RULES (GNU MAKE).
##
##	(c) Copyright 1998,1999 by Lutz Sammer
##
##	$Id: Rules.make,v 1.45 1999/12/17 15:30:54 root Exp root $
##

############################################################################
#	Configurable:
#			Choose what you want to include and the correct 
#			version.  Minimal is now the default.
############################################################################

# Uncomment next to get guile with gtk (and choose your gtklib version)

GUILEGTK	=
#GUILEGTK	= -DGUILE_GTK `gtk-config --cflags`
#GUILEGTK	= -DGUILE_GTK -I/usr/X11R6/include -I/usr/lib/glib/include
#GUILEGTKLIB	= -lguilegtk-1.0 `gtk-config --libs`
#GUILEGTKLIB	= -lguilegtk-1.2 `gtk-config --libs`
#GUILEGTKLIB	= -L/opt/gnome/lib -lguilegtk-1.1 `/opt/gnome/bin/gtk-config --libs`

#------------------------------------------------------------------------------

# Uncomment next for a version with guile (GNU scheme interpreter)
#	(and choose your guile version)

# guile 1.2	old version
# 	If you have unreferenced "gh_vector_ref" define LIBGUILE12
#GUILE		= -DLIBGUILE12 -DUSE_CCL $(GUILEGTK)
#GUILELIB	= $(GUILEGTKLIB) -lguile -ldl

# guile 1.3	latest version
#	-lreadline -lncurses are needed with my distribution SuSe 5.3
#GUILE_CFLAGS	= `guile-config compile`
#GUILE_CFLAGS	= $(shell guile-config compile)
#GUILE		= -DUSE_CCL $(GUILEGTK) $(GUILE_CFLAGS)
#GUILELIB	= $(GUILEGTKLIB) `guile-config link` -lreadline -lncurses
#GUILELIB	= $(GUILEGTKLIB) $(shell guile-config link)
#GUILELIB	= $(GUILEGTKLIB) -lguile -lqthreads -ldl -lm

#------------------------------------------------------------------------------

# Uncomment next to add threaded sound support 
#	You should have a thread safe X11 (libc6 or glibc)

#THREAD		= -D_REENTRANT -DUSE_THREAD
#THREADLIB	= -lpthread

#------------------------------------------------------------------------------

# Choose correct version of glib (needed for gtk)
# This currently required for the sound!!

# Where do you have installed your glib?
#GLIB		=	-DUSE_GLIB -I/usr/lib/glib/include/
#GLIBLIB 	=	-lglib

# Should work with >= 1.2
#GLIB_CFLAGS	=	`glib-config glib --cflags`
GLIB_CFLAGS	=	$(shell glib-config glib --cflags)
#GLIBLIB 	=	`glib-config glib --libs`
GLIBLIB 	=	$(shell glib-config glib --libs)
GLIB		=	-DUSE_GLIB $(GLIB_CFLAGS)

#------------------------------------------------------------------------------

# Video driver part

# Uncomment the next to get the support for SDL.

# Old SDL <1.0.0
SDL_CFLAGS	=	
SDLLIB		=	-lSDL -ldl -lpthread
# New SDL >=1.0.0
#SDL_CFLAGS	=	`sdl-config --cflags`
SDL_CFLAGS	=	$(shell sdl-config --cflags)
#SDLLIB		=	`sdl-config --libs`
SDLLIB		=	$(shell sdl-config --libs)

SDL		=	-DUSE_SDL $(SDL_CFLAGS)

# Uncomment the next for the SDL X11 support.

#VIDEO		=	$(SDL)
#VIDEOLIB	=	$(SDLLIB)

# Uncomment the next for the normal X11 support.

VIDEO		=	-DUSE_X11
VIDEOLIB	=	-lXext

# Choose next to get svgalib support.

#VIDEO		=	-DUSE_SVGALIB -DUSE_SVGALINEAR
# Use if your VidCard does not support linear addressing (i.e. S3)
#VIDEO		=	-DUSE_SVGALIB
#VIDEOLIB	=	-lvga

# Uncomment the next for the win32/cygwin support.
#VIDEO		=	-DUSE_WIN32 $(SDL)
#VIDEOLIB	=	$(SDLLIB)

#------------------------------------------------------------------------------

# Comment next if you want to remove sound support.

DSOUND		=	-DWITH_SOUND

#------------------------------------------------------------------------------

# Choose which compress you like

# None
#ZDEFS		=
#ZLIBS		=
# GZ compression
ZDEFS		=	-DUSE_ZLIB
ZLIBS		=	-lz
# BZ2 compression
#ZDEFS		=	-DUSE_BZ2LIB
#ZLIBS		=	-lbz2

#------------------------------------------------------------------------------

# May be required on some distributions for libpng and libz!
# extra linker flags and include directory
# -L/usr/lib 

XLDFLAGS	= -L/usr/X11R6/lib -L/usr/local/lib \
		  -L$(TOPDIR)/libpng-1.0.5 -L$(TOPDIR)/zlib-1.1.3
XIFLAGS		= -I/usr/X11R6/include -I/usr/local/include \
		  -I$(TOPDIR)/libpng-1.0.5 -I$(TOPDIR)/zlib-1.1.3

#------------------------------------------------------------------------------
 
# Uncomment next to profile
PROFILE=	-pg

# Version
VERSION=	'-DVERSION="1.16pre3"'

############################################################################
# below this, nothing should be changed!

# Libraries needed to build tools
TOOLLIBS=$(XLDFLAGS) -lpng -lz -lm $(THREADLIB)

# Libraries needed to build clone
CLONELIBS=$(XLDFLAGS) -lpng -lz -lm \
	$(THREADLIB) $(GUILELIB) $(GLIBLIB) $(VIDEOLIB) $(ZLIBS)

DISTLIST=$(TOPDIR)/distlist
TAGS=$(TOPDIR)/src/tags

OUTFILE=$(TOPDIR)/clone
ARCH=linux
OE=o

## architecture-dependant objects
#ARCHOBJS=stdmman.$(OE) svgalib.$(OE) unix_lib.$(OE) bitm_lnx.$(OE)

## include flags
IFLAGS=	-I$(TOPDIR)/src/include $(XIFLAGS)
## define flags
DFLAGS=	$(THREAD) $(GUILE) $(VERSION) $(GLIB) $(VIDEO) $(ZDEFS) $(DSOUND)

## choose optimise level
#CFLAGS=-g -O0 $(PROFILE) -pipe -Wall -Werror $(IFLAGS) $(DFLAGS)
CFLAGS=-g -O1 $(PROFILE) -pipe -Wall -Werror $(IFLAGS) $(DFLAGS)
#CFLAGS=-g -O2 $(PROFILE) -pipe -Wall -Werror $(IFLAGS)  $(DFLAGS)
#CFLAGS=-g -O3 $(PROFILE) -pipe -Wall -Werror $(IFLAGS)  $(DFLAGS)
#CFLAGS=-g -O6 -pipe -fconserve-space -fexpensive-optimizations -ffast-math  $(IFLAGS) $(DFLAGS)
#CFLAGS=-O6 -pipe -fomit-frame-pointer -fconserve-space -fexpensive-optimizations -ffast-math  $(IFLAGS) $(DFLAGS)

CC=gcc
RM=rm -f
MAKE=make

## JOHNS: my ctags didn't support
#CTAGSFLAGS=-i defmpstuvFS -a -f 
CTAGSFLAGS=-i defptvS -a -f 

#
#	Locks versions with symbolic name
#
LOCKVER=	rcs -n$(NAME)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	@ar cru $(TOPDIR)/src/libclone.a $@

#------------
#	Source code documentation
#
DOXYGEN=	doxygen
DOCIFY=		docify
DOCPP=		doc++
# Still didn't work
#DOCIFY=		/root/doc++-3.3.11/src/docify
#DOCPP=		/root/doc++-3.3.11/src/doc++

%.doc: %.c
	@$(TOPDIR)/tools/aledoc $< | $(DOCIFY) > $*-c.doc 2>/dev/null

%.doc: %.h
	@$(TOPDIR)/tools/aledoc $< | $(DOCIFY) > $*-h.doc 2>/dev/null
