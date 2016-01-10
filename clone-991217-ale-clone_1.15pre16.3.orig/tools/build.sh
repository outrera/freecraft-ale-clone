#!/bin/sh
##
##	A clone of a famous game.
##
##	build.sh	-	The graphics and sound extractor.
##
##	(c) Copyright 1999 by Lutz Sammer
##
##	$Id: build.sh,v 1.6 1999/12/03 09:18:27 root Exp $
##

###
### Change the next, to meet your requirements:
###

#
#	Choose your compression
COMPRESS="gzip --force --best"
#COMPRESS="bzip2 --force"

#
#	Cdrom autodetection
#
CDROM="/cdrom"
[ -d $CDROM ] || CDROM="/mnt/cdrom"

#
#	Here are the input files for sounds, graphics and texts.
#	WARNING: only version 1.3, 1.33 and 1.50 are supported!
#		First choice:	installed on dos parition
#		Second choise:	installed in current directory
#		Third choise:	uninstalled on original cdrom
#
#DATADIR="/dos/c/games/war2/data/"
#DATADIR="./"
DATADIR=$CDROM"/data/"

#
#	Here are my executeables
#
BINPATH="tools"
#BINPATH="/usr/local/lib/clone/bin"

#
#	Here is the destination for the generated files
#
DIR="data"
#DIR="/usr/local/lib/clone/data"

#
#	Here are the extra files contributed
#
CONTRIB="contrib"
#CONTRIB="/usr/local/lib/clone/contrib"

###
###	Below this point, you should only search bugs. :-)
###

# Create the directory structure

[ -d $DIR ] || mkdir $DIR

[ -d $DIR/graphic ] || mkdir $DIR/graphic
[ -d $DIR/graphic/tileset ] || mkdir $DIR/graphic/tileset
[ -d $DIR/graphic/interface ] || mkdir $DIR/graphic/interface

[ -d $DIR/sound ] || mkdir $DIR/sound 

[ -d $DIR/campaigns ] || mkdir $DIR/campaigns
[ -d $DIR/campaigns/human ] || mkdir $DIR/campaigns/human
[ -d $DIR/campaigns/orc ] || mkdir $DIR/campaigns/orc
[ -d $DIR/campaigns/human-exp ] || mkdir $DIR/campaigns/human-exp
[ -d $DIR/campaigns/orc-exp ] || mkdir $DIR/campaigns/orc-exp
[ -d $DIR/text ] || mkdir $DIR/text

[ -d $DIR/puds ] || mkdir $DIR/puds 
[ -d $DIR/puds/internal ] || mkdir $DIR/puds/internal
[ -d $DIR/puds/demo ] || mkdir $DIR/puds/demo

###############################################################################
##	Extract
###############################################################################

$BINPATH/wartool $DATADIR

# own supplied files

cp $CONTRIB/cross.png $DIR/graphic
cp $CONTRIB/mana.png $DIR
cp $CONTRIB/health.png $DIR
cp $CONTRIB/ale-title.png $DIR

###############################################################################
##	MISC
###############################################################################

#
#	Compress the sounds
#
find $DIR/sound -type f -name "*.wav" -print -exec $COMPRESS {} \;

#
#	Compress the texts
#
find $DIR/text -type f -name "*.txt" -print -exec $COMPRESS {} \;

#
#	Copy original puds into data directory
#
echo "Copy puds and compressing"
[ -d $DATADIR/../puds ] && cp -r $DATADIR/../puds/ $DIR/puds
[ -f $DATADIR/../alamo.pud ] && cp -r $DATADIR/../*.pud $DIR/puds
chmod -R +w $DIR/puds
find $DIR/puds -type f -name "*.pud" -print -exec $COMPRESS {} \;

#
##	The default pud.
#
[ -f $DIR/puds/internal/internal12.pud.gz ] \
	&& ln -s puds/internal/internal12.pud.gz $DIR/default.pud.gz
[ -f $DIR/puds/internal/internal12.pud.bz2 ] \
	&& ln -s puds/internal/internal12.pud.bz2 $DIR/default.pud.bz2

#
# 	Some checks
#
[ -s $DIR/swamp.rgb ] || echo "This script isn't tested with the non-expansion CD!"
