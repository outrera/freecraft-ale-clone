#!/bin/sh
##
##	A clone of a famous game.
##
##	build.sh	-	The graphics and sound extractor.
##
##	(c) Copyright 1998,1999 by Lutz Sammer
##
##	$Id: build-old.sh,v 1.18 1999/12/20 20:45:34 root Exp $
##

###
### Change the next, to meet your requirements:
###

#
#	Here are the input files for sounds, graphics and texts.
#	WARNING: only version 1.3, 1.33 and 1.50 are supported!
#		First choice:	installed on dos parition
#		Second choise:	installed in current directory
#		Third choise:	uninstalled on original cdrom
#
#DATADIR="/dos/c/games/war2/data/"
#DATADIR="./"
DATADIR="/cdrom/data/"

MAINDAT=$DATADIR"maindat.war"
#MAINDAT="maindat.war"

SFXDAT=$DATADIR"sfxdat.sud"
#SFXDAT="sfxdat.sud"

REZDAT=$DATADIR"rezdat.war"
#REZDAT="rezdat.war"

STRDAT=$DATADIR"strdat.war"
#STRDAT="strdat.war"

SNDDAT=$DATADIR"snddat.war"
#SNDDAT="snddat.war"

MUDDAT=$DATADIR"muddat.cud"
#MUDDAT="muddat.cud"

PUDS=$DATADIR"../puds"
#PUDS="puds"

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

DATAFILE=$MAINDAT

extract_palette()
{
    $BINPATH/extract $DATAFILE "$1" "$DIR/$2"
}

extract_gfx()
{
    $BINPATH/extract $DATAFILE "$1" "$DIR/graphic/$2.gfx"
    $BINPATH/gfx2png "$DIR/$PALETTE" "$DIR/graphic/$2.gfx"
    rm "$DIR/graphic/$2.gfx"
}

extract_gfu()
{
    $BINPATH/extract $DATAFILE "$1" "$DIR/graphic/$2.gfu"
    $BINPATH/gfu2png "$DIR/$PALETTE" "$DIR/graphic/$2.gfu"
    rm "$DIR/graphic/$2.gfu"
}

extract_img()
{
    $BINPATH/extract $DATAFILE "$1" "$DIR/graphic/$2.img"
    $BINPATH/img2png "$DIR/$PALETTE" "$DIR/graphic/$2.img"
    rm "$DIR/graphic/$2.img"
}

extract_fnt()
{
    $BINPATH/extract $DATAFILE "$1" "$DIR/graphic/$2.fnt"
    $BINPATH/fnt2png "$DIR/$PALETTE" "$DIR/graphic/$2.fnt"
    rm "$DIR/graphic/$2.fnt"
}

extract_cur()
{
    $BINPATH/extract $DATAFILE "$1" "$DIR/graphic/$2.cur"
    $BINPATH/cur2png "$DIR/$PALETTE" "$DIR/graphic/$2.cur"
    rm "$DIR/graphic/$2.cur"
}

extract_tile()
{
    $BINPATH/extract $DATAFILE "$1" "$DIR/graphic/tileset/$4.meg"
    $BINPATH/extract $DATAFILE "$2" "$DIR/graphic/tileset/$4.min"
    $BINPATH/extract $DATAFILE "$3" "$DIR/graphic/tileset/$4.map"
    $BINPATH/tile2png "$DIR/$PALETTE" "$DIR/graphic/tileset/$4.min" "$DIR/graphic/tileset/$4.meg" "$DIR/graphic/tileset/$4.map"
    rm "$DIR/graphic/tileset/$4.meg" "$DIR/graphic/tileset/$4.min" "$DIR/graphic/tileset/$4.map"
}

extract_pud()
{
    $BINPATH/extract $DATAFILE "$1" "$DIR/$2.pud"
}

extract_wav1()
{
    $BINPATH/extract $DATAFILE "$1" "$DIR/sound/$2.wav"
}

extract_wav()
{
    $BINPATH/extract $SFXDAT "$1" "$DIR/sound/$2.wav"
}

extract_txt()
{
    $BINPATH/extract $STRDAT "$1" "$DIR/text/$2.txt"
}

##
##	Here starts the conversion
##

# Check if data files are available
[ -r $MAINDAT ] || echo "Can't open" $MAINDAT || exit 99
[ -r $SFXDAT ] || echo "Can't open" $SFXDAT || exit 99

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
##	GRAPHICS
###############################################################################

extract_palette 2 summer.rgb
extract_palette 10 wasteland.rgb
extract_palette 18 winter.rgb
extract_palette 438 swamp.rgb

PALETTE=summer.rgb
extract_tile 3 4 5 "summer"
PALETTE=wasteland.rgb
extract_tile 11 12 13 "wasteland"
PALETTE=winter.rgb
extract_tile 19 20 21 "winter"
extract_gfx 33 "dwarves"
extract_gfx 34 "goblin sapper"
extract_gfx 35 "gryphon rider"
extract_gfx 36 "dragon"
extract_gfx 37 "eye of kilrogg"
extract_gfx 38 "gnomish flying machine"
extract_gfx 39 "human transport"
extract_gfx 40 "orc transport"
extract_gfx 41 "battleship"
extract_gfx 42 "juggernaught"
extract_gfx 43 "gnomish submarine (summer,winter)"
extract_gfx 44 "giant turtle (summer,winter)"
extract_gfx 45 "footman"
extract_gfx 46 "grunt"
extract_gfx 47 "peasant"
extract_gfx 48 "peon"
extract_gfx 49 "ballista"
extract_gfx 50 "catapult"
extract_gfx 51 "knight"
extract_gfx 52 "ogre"
extract_gfx 53 "archer"
extract_gfx 54 "axethrower"
extract_gfx 55 "mage"
extract_gfx 58 "death knight"
extract_gfx 59 "human tanker (empty)"
extract_gfx 60 "orc tanker (empty)"
extract_gfx 61 "elven destroyer"
extract_gfx 62 "troll destroyer"
extract_gfx 63 "goblin zeppelin"
extract_gfx 64 "critter (summer)"
PALETTE=wasteland.rgb
extract_gfx 65 "critter (wasteland)"
PALETTE=winter.rgb
extract_gfx 66 "critter (winter)"
PALETTE=summer.rgb
extract_gfx 69 "skeleton"
extract_gfx 70 "daemon"
# 71-79 unknown
extract_gfx 80 "human guard tower (summer)"
extract_gfx 81 "orc guard tower (summer)"
extract_gfx 82 "human cannon tower (summer)"
extract_gfx 83 "orc cannon tower (summer)"
extract_gfx 84 "mage tower (summer)"
extract_gfx 85 "temple of the damned (summer)"
extract_gfx 86 "keep (summer)"
extract_gfx 87 "stronghold (summer)"
extract_gfx 88 "gryphon aviary (summer)"
extract_gfx 89 "dragon roost (summer)"
extract_gfx 90 "gnomish inventor (summer)"
extract_gfx 91 "goblin alchemist (summer)"
extract_gfx 92 "farm (summer)"
extract_gfx 93 "pig farm (summer)"
extract_gfx 94 "human barracks (summer)"
extract_gfx 95 "orc barracks (summer)"
extract_gfx 96 "church (summer)"
extract_gfx 97 "altar of storms (summer)"
extract_gfx 98 "human scout tower (summer)"
extract_gfx 99 "orc scout tower (summer)"
extract_gfx 100 "town hall (summer)"
extract_gfx 101 "great hall (summer)"
extract_gfx 102 "elven lumber mill (summer)"
extract_gfx 103 "troll lumber mill (summer)"
extract_gfx 104 "stables (summer)"
extract_gfx 105 "ogre mound (summer)"
extract_gfx 106 "human blacksmith (summer)"
extract_gfx 107 "orc blacksmith (summer)"
extract_gfx 108 "human shipyard (summer)"
extract_gfx 109 "orc shipyard (summer)"
extract_gfx 110 "human foundry (summer)"
extract_gfx 111 "orc foundry (summer)"
extract_gfx 112 "human refinery (summer)"
extract_gfx 113 "orc refinery (summer)"
extract_gfx 114 "human oil well (summer)"
extract_gfx 115 "orc oil well (summer)"
extract_gfx 116 "castle (summer)"
extract_gfx 117 "fortress (summer)"
extract_gfx 118 "oil patch (summer)"
extract_gfx 119 "gold mine (summer)"
extract_gfx 120 "corpses"
extract_gfx 121 "destroyed site (summer)"
extract_gfx 122 "peasant with wood"
extract_gfx 123 "peon with wood"
extract_gfx 124 "peasant with gold"
extract_gfx 125 "peon with gold"
extract_gfx 126 "human tanker (full)"
extract_gfx 127 "orc tanker (full)"
PALETTE=winter.rgb
extract_gfx 128 "keep (winter)"
extract_gfx 129 "stronghold (winter)"
extract_gfx 130 "gryphon aviary (winter)"
extract_gfx 131 "dragon roost (winter)"
extract_gfx 132 "gnomish inventor (winter)"
extract_gfx 133 "goblin alchemist (winter)"
extract_gfx 134 "farm (winter)"
extract_gfx 135 "pig farm (winter)"
extract_gfx 136 "human barracks (winter)"
extract_gfx 137 "orc barracks (winter)"
extract_gfx 138 "church (winter)"
extract_gfx 139 "altar of storms (winter)"
extract_gfx 140 "human scout tower (winter)"
extract_gfx 141 "orc scout tower (winter)"
extract_gfx 142 "town hall (winter)"
extract_gfx 143 "great hall (winter)"
extract_gfx 144 "elven lumber mill (winter)"
extract_gfx 145 "troll lumber mill (winter)"
extract_gfx 146 "stables (winter)"
extract_gfx 147 "ogre mound (winter)"
extract_gfx 148 "human blacksmith (winter)"
extract_gfx 149 "orc blacksmith (winter)"
extract_gfx 150 "human shipyard (winter)"
extract_gfx 151 "orc shipyard (winter)"
extract_gfx 152 "human foundry (winter)"
extract_gfx 153 "orc foundry (winter)"
extract_gfx 154 "human refinery (winter)"
extract_gfx 155 "orc refinery (winter)"
extract_gfx 156 "human oil well (winter)"
extract_gfx 157 "orc oil well (winter)"
extract_gfx 158 "castle (winter)"
extract_gfx 159 "fortress (winter)"
extract_gfx 160 "mage tower (winter)"
extract_gfx 161 "temple of the damned (winter)"
extract_gfx 162 "gold mine (winter)"
extract_gfx 163 "destroyed site (winter)"
PALETTE=summer.rgb
extract_gfx 164 "x (human player startpoint)"
extract_gfx 165 "o (orc player startpoint)"
extract_gfx 166 "circle of power"
extract_gfx 167 "dark portal (summer)"
extract_gfx 168 "wall (summer)"
PALETTE=winter.rgb
extract_gfx 169 "human guard tower (winter)"
extract_gfx 170 "orc guard tower (winter)"
extract_gfx 171 "human cannon tower (winter)"
extract_gfx 172 "orc cannon tower (winter)"
PALETTE=wasteland.rgb
extract_gfx 173 "farm (wasteland)"
extract_gfx 174 "pig farm (wasteland)"
extract_gfx 175 "elven lumber mill (wasteland)"
extract_gfx 176 "troll lumber mill (wasteland)"
extract_gfx 177 "human oil well (wasteland)"
extract_gfx 178 "orc oil well (wasteland)"
extract_gfx 179 "gold mine (wasteland)"
extract_gfx 180 "oil patch (wasteland)"
PALETTE=summer.rgb
extract_gfx 181 "runestone (summer,wasteland)"
PALETTE=wasteland.rgb
extract_gfx 182 "gnomish submarine (wasteland)"
extract_gfx 183 "giant turtle (wasteland)"
extract_gfx 184 "dark portal (winter)"
PALETTE=wasteland.rgb
extract_gfx 185 "dark portal (wasteland)"
PALETTE=winter.rgb
extract_gfx 186 "runestone (winter)"
PALETTE=summer.rgb
extract_gfu 187 "gold,wood,oil,mana"
PALETTE=wasteland.rgb
extract_gfx 188 "small destroyed site (wasteland)"
PALETTE=summer.rgb
extract_gfx 189 "small destroyed site (summer)"
PALETTE=winter.rgb
extract_gfx 190 "small destroyed site (winter)"
PALETTE=wasteland.rgb
extract_gfx 191 "destroyed site (wasteland)"
# --------------------------------------------------
extract_pud 192 "campaigns/human/level01h"
extract_pud 193 "campaigns/orc/level01o"
extract_pud 194 "campaigns/human/level02h"
extract_pud 195 "campaigns/orc/level02o"
extract_pud 196 "campaigns/human/level03h"
extract_pud 197 "campaigns/orc/level03o"
extract_pud 198 "campaigns/human/level04h"
extract_pud 199 "campaigns/orc/level04o"
extract_pud 200 "campaigns/human/level05h"
extract_pud 201 "campaigns/orc/level05o"
extract_pud 202 "campaigns/human/level06h"
extract_pud 203 "campaigns/orc/level06o"
extract_pud 204 "campaigns/human/level07h"
extract_pud 205 "campaigns/orc/level07o"
extract_pud 206 "campaigns/human/level08h"
extract_pud 207 "campaigns/orc/level08o"
extract_pud 208 "campaigns/human/level09h"
extract_pud 209 "campaigns/orc/level09o"
extract_pud 210 "campaigns/human/level10h"
extract_pud 211 "campaigns/orc/level10o"
extract_pud 212 "campaigns/human/level11h"
extract_pud 213 "campaigns/orc/level11o"
extract_pud 214 "campaigns/human/level12h"
extract_pud 215 "campaigns/orc/level12o"
extract_pud 216 "campaigns/human/level13h"
extract_pud 217 "campaigns/orc/level13o"
extract_pud 218 "campaigns/human/level14h"
extract_pud 219 "campaigns/orc/level14o"
# --------------------------------------------------
extract_pud 220 "puds/internal/internal01"		# need better names
extract_pud 221 "puds/internal/internal02"
extract_pud 222 "puds/internal/internal03"
extract_pud 223 "puds/internal/internal04"
extract_pud 224 "puds/internal/internal05"
extract_pud 225 "puds/internal/internal06"
extract_pud 226 "puds/internal/internal07"
extract_pud 227 "puds/internal/internal08"
extract_pud 228 "puds/internal/internal09"
extract_pud 229 "puds/internal/internal10"
extract_pud 230 "puds/internal/internal11"
extract_pud 231 "puds/internal/internal12"
extract_pud 232 "puds/internal/internal13"
extract_pud 233 "puds/internal/internal14"
extract_pud 234 "puds/internal/internal15"
extract_pud 235 "puds/internal/internal16"
extract_pud 236 "puds/internal/internal17"
extract_pud 237 "puds/internal/internal18"
extract_pud 238 "puds/internal/internal19"
extract_pud 239 "puds/internal/internal20"
extract_pud 240 "puds/internal/internal21"
extract_pud 241 "puds/internal/internal22"
extract_pud 242 "puds/internal/internal23"
extract_pud 243 "puds/internal/internal24"
extract_pud 244 "puds/internal/internal25"
extract_pud 245 "puds/internal/internal26"
extract_pud 246 "puds/internal/internal27"
extract_pud 247 "puds/internal/internal28"
# --------------------------------------------------
extract_pud 248 "puds/demo/demo01"
extract_pud 249 "puds/demo/demo02"
extract_pud 250 "puds/demo/demo03"
extract_pud 251 "puds/demo/demo04"
# --------------------------------------------------
extract_gfx 252 "land construction site (summer,wasteland)"
extract_gfx 253 "human shipyard construction site (summer,wasteland)"
extract_gfx 254 "orc shipyard construction site (summer,wasteland)"
extract_gfx 255 "human oil well construction site (summer)"
extract_gfx 256 "orc oil well construction site (summer)"
extract_gfx 257 "human refinery construction site (summer,wasteland)"
extract_gfx 258 "orc refinery construction site (summer,wasteland)"
extract_gfx 259 "human foundry construction site (summer,wasteland)"
extract_gfx 260 "orc foundry construction site (summer,wasteland)"
extract_gfx 261 "wall construction site (summer)"
PALETTE=winter.rgb
extract_gfx 262 "land construction site (winter)"
extract_gfx 263 "human shipyard construction site (winter)"
extract_gfx 264 "orc shipyard construction site (winter)"
extract_gfx 265 "human oil well construction site (winter)"
extract_gfx 266 "orc oil well construction site (winter)"
extract_gfx 267 "human refinery construction site (winter)"
extract_gfx 268 "orc refinery construction site (winter)"
extract_gfx 269 "human foundry construction site (winter)"
extract_gfx 270 "orc foundry construction site (winter)"
PALETTE=wasteland.rgb
extract_gfx 271 "human oil well construction site (wasteland)"
extract_gfx 272 "orc oil well construction site (wasteland)"
PALETTE=winter.rgb
extract_gfx 273 "wall (winter)"
PALETTE=wasteland.rgb
extract_gfx 274 "wall (wasteland)"
PALETTE=winter.rgb
extract_gfx 275 "wall construction site (winter)"
PALETTE=wasteland.rgb
extract_gfx 276 "wall construction site (wasteland)"

extract_fnt 279 "large font episode titles"
extract_fnt 280 "small font episode titles"
extract_fnt 281 "large font"
extract_fnt 282 "small font"
extract_fnt 283 "game font"

PALETTE=summer.rgb
extract_img 287 "Map border (Top,Humans)"
extract_img 288 "Map border (Top,Orcs)"
extract_img 289 "Map border (Right,Humans)"
extract_img 290 "Map border (Right,Orcs)"
extract_img 291 "Map border (Bottom,Humans)"
extract_img 292 "Map border (Bottom,Orcs)"
extract_img 293 "Minimap border (Top,Humans)"
extract_img 294 "Minimap border (Top,Orcs)"
extract_img 295 "Minimap (Humans)"
extract_img 296 "Minimap (Orcs)"
extract_img 297 "Panel (Bottom,Humans)"
extract_img 298 "Panel (Bottom,Orcs)"
extract_palette 300 title.rgb
PALETTE=title.rgb
extract_img 299 "title"
PALETTE=summer.rgb
extract_cur 301 "human gauntlet"
extract_cur 302 "orcish claw"
extract_cur 303 "human don't click here"
extract_cur 304 "orcish don't click here"
extract_cur 305 "yellow eagle"
extract_cur 306 "yellow crosshairs"
extract_cur 307 "red eagle"
extract_cur 308 "red crosshairs"
extract_cur 309 "green eagle"
extract_cur 310 "green crosshairs"
extract_cur 311 "magnifying glass"
extract_cur 312 "small green cross"
extract_cur 313 "hourglass"
extract_cur 314 "blue arrow NW"
extract_cur 315 "arrow N"
extract_cur 316 "arrow NE"
extract_cur 317 "arrow E"
extract_cur 318 "arrow SE"
extract_cur 319 "arrow S"
extract_cur 320 "arrow SW"
extract_cur 321 "arrow W"
extract_cur 322 "arrow NW"
extract_gfu 323 "bloodlust,haste,slow,invis.,shield"
# --------------------------------------------------------
extract_gfx 324 "lightning"
extract_gfx 325 "gryphon hammer"
extract_gfx 326 "dragon breath"		# "fireball (also dragon breath)"
extract_gfx 327 "fireball"		#  (when casting flameshield)"
extract_gfx 328 "flame shield"
extract_gfx 329 "blizzard"
extract_gfx 330 "death and decay"
extract_gfx 331 "big cannon"
extract_gfx 332 "exorcism"
extract_gfx 333 "heal effect"
extract_gfx 334 "touch of death"
extract_gfx 335 "rune"
extract_gfx 336 "tornado"
extract_gfx 337 "catapult rock"
extract_gfx 338 "ballista bolt"
extract_gfx 339 "arrow"
extract_gfx 340 "axe"
extract_gfx 341 "submarine missile"
extract_gfx 342 "turtle missile"
extract_gfx 343 "small fire"
extract_gfx 344 "big fire"
extract_gfx 345 "ballista-catapult impact"
extract_gfx 346 "normal spell"
extract_gfx 347 "explosion"
extract_gfx 348 "cannon"
extract_gfx 349 "cannon explosion"
extract_gfx 350 "cannon-tower explosion"
extract_gfx 351 "daemon fire"
extract_gfx 352 "green cross"
extract_gfx 353 "unit shadow"
extract_gfu 354 "human panel"
extract_gfu 355 "orcish panel"
extract_gfx 356 "icons (summer)"
PALETTE=winter.rgb
extract_gfx 357 "icons (winter)"
PALETTE=wasteland.rgb
extract_gfx 358 "icons (wasteland)"

extract_wav1 432 "click"
extract_wav1 435 "highclick"

PALETTE=swamp.rgb
extract_tile 439 440 441 "swamp"
# --------------------------------------------------
extract_pud 446 "campaigns/human-exp/levelx01h"
extract_pud 447 "campaigns/orc-exp/levelx01o"
extract_pud 448 "campaigns/human-exp/levelx02h"
extract_pud 449 "campaigns/orc-exp/levelx02o"
extract_pud 450 "campaigns/human-exp/levelx03h"
extract_pud 451 "campaigns/orc-exp/levelx03o"
extract_pud 452 "campaigns/human-exp/levelx04h"
extract_pud 453 "campaigns/orc-exp/levelx04o"
extract_pud 454 "campaigns/human-exp/levelx05h"
extract_pud 455 "campaigns/orc-exp/levelx05o"
extract_pud 456 "campaigns/human-exp/levelx06h"
extract_pud 457 "campaigns/orc-exp/levelx06o"
extract_pud 458 "campaigns/human-exp/levelx07h"
extract_pud 459 "campaigns/orc-exp/levelx07o"
extract_pud 460 "campaigns/human-exp/levelx08h"
extract_pud 461 "campaigns/orc-exp/levelx08o"
extract_pud 462 "campaigns/human-exp/levelx09h"
extract_pud 463 "campaigns/orc-exp/levelx09o"
extract_pud 464 "campaigns/human-exp/levelx10h"
extract_pud 465 "campaigns/orc-exp/levelx10o"
extract_pud 466 "campaigns/human-exp/levelx11h"
extract_pud 467 "campaigns/orc-exp/levelx11o"
extract_pud 468 "campaigns/human-exp/levelx12h"
extract_pud 469 "campaigns/orc-exp/levelx12o"
# ------------------------------------------
extract_gfx 470 "critter (swamp)"
extract_gfx 471 "icons (swamp)"
# 472: default UDTA for expansion PUDs
extract_gfx 473 "keep (swamp)"
extract_gfx 474 "stronghold (swamp)"
extract_gfx 475 "gryphon aviary (swamp)"
extract_gfx 476 "dragon roost (swamp)"
extract_gfx 477 "gnomish inventor (swamp)"
extract_gfx 478 "goblin alchemist (swamp)"
extract_gfx 479 "farm (swamp)"
extract_gfx 480 "pig farm (swamp)"
extract_gfx 481 "human barracks (swamp)"
extract_gfx 482 "orc barracks (swamp)"
extract_gfx 483 "church (swamp)"
extract_gfx 484 "altar of storms (swamp)"
extract_gfx 485 "human scout tower (swamp)"
extract_gfx 486 "orc scout tower (swamp)"
extract_gfx 487 "town hall (swamp)"
extract_gfx 488 "great hall (swamp)"
extract_gfx 489 "elven lumber mill (swamp)"
extract_gfx 490 "troll lumber mill (swamp)"
extract_gfx 491 "stables (swamp)"
extract_gfx 492 "ogre mound (swamp)"
extract_gfx 493 "human blacksmith (swamp)"
extract_gfx 494 "orc blacksmith (swamp)"
extract_gfx 495 "human shipyard (swamp)"
extract_gfx 496 "orc shipyard (swamp)"
extract_gfx 497 "human foundry (swamp)"
extract_gfx 498 "orc foundry (swamp)"
extract_gfx 499 "human refinery (swamp)"
extract_gfx 500 "orc refinery (swamp)"
extract_gfx 501 "human oil well (swamp)"
extract_gfx 502 "orc oil well (swamp)"
extract_gfx 503 "castle (swamp)"
extract_gfx 504 "fortress (swamp)"
extract_gfx 505 "mage tower (swamp)"
extract_gfx 506 "temple of the damned (swamp)"
extract_gfx 507 "human guard tower (swamp)"
extract_gfx 508 "orc guard tower (swamp)"
extract_gfx 509 "human cannon tower (swamp)"
extract_gfx 510 "orc cannon tower (swamp)"
extract_gfx 511 "gold mine (swamp)"
extract_gfx 512 "destroyed site (swamp)"
extract_gfx 513 "dark portal (swamp)"
extract_gfx 514 "runestone (swamp)"
extract_gfx 515 "oil patch (swamp)"
extract_gfx 516 "human shipyard construction site (swamp)"
extract_gfx 517 "orc shipyard construction site (swamp)"
extract_gfx 518 "human oil well construction site (swamp)"
extract_gfx 519 "orc oil well construction site (swamp)"
extract_gfx 520 "human refinery construction site (swamp)"
extract_gfx 521 "orc refinery construction site (swamp)"
extract_gfx 522 "human foundry construction site (swamp)"
extract_gfx 523 "orc foundry construction site (swamp)"
extract_gfx 524 "small destroyed site (swamp)"
extract_gfx 525 "circle of power (swamp)"
extract_gfx 526 "gnomish submarine (swamp)"
extract_gfx 527 "giant turtle (swamp)"

# own supplied files
cp $CONTRIB/cross.png $DIR/graphic
cp $CONTRIB/mana.png $DIR
cp $CONTRIB/health.png $DIR
cp $CONTRIB/ale-title.png $DIR

###############################################################################
##	SOUNDS
###############################################################################

DATAFILE=$SFXDAT

extract_wav 2 "placement error"
extract_wav 3 "placement sucess"
extract_wav 4 "building construction"
extract_wav 5 "basic human voices selected 1"
extract_wav 6 "basic orc voices selected 1"
extract_wav 7 "basic human voices selected 2"
extract_wav 8 "basic orc voices selected 2"
extract_wav 9 "basic human voices selected 3"
extract_wav 10 "basic orc voices selected 3"
extract_wav 11 "basic human voices selected 4"
extract_wav 12 "basic orc voices selected 4"
extract_wav 13 "basic human voices selected 5"
extract_wav 14 "basic orc voices selected 5"
extract_wav 15 "basic human voices selected 6"
extract_wav 16 "basic orc voices selected 6"
extract_wav 17 "basic human voices annoyed 1"
extract_wav 18 "basic orc voices annoyed 1"
extract_wav 19 "basic human voices annoyed 2"
extract_wav 20 "basic orc voices annoyed 2"
extract_wav 21 "basic human voices annoyed 3"
extract_wav 22 "basic orc voices annoyed 3"
extract_wav 23 "basic human voices annoyed 4"
extract_wav 24 "basic orc voices annoyed 4"
extract_wav 25 "basic human voices annoyed 5"
extract_wav 26 "basic orc voices annoyed 5"
extract_wav 27 "basic human voices annoyed 6"
extract_wav 28 "basic orc voices annoyed 6"
extract_wav 29 "basic human voices annoyed 7"
extract_wav 30 "basic orc voices annoyed 7"
extract_wav 31 "explosion"
extract_wav 32 "basic human voices acknowledgement 1"
extract_wav 33 "basic orc voices acknowledgement 1"
extract_wav 34 "basic human voices acknowledgement 2"
extract_wav 35 "basic orc voices acknowledgement 2"
extract_wav 36 "basic human voices acknowledgement 3"
extract_wav 37 "basic orc voices acknowledgement 3"
extract_wav 38 "basic human voices acknowledgement 4"
extract_wav 39 "basic orc voices acknowledgement 4"
extract_wav 40 "basic human voices work complete"
extract_wav 41 "basic orc voices work complete"
extract_wav 42 "peasant work complete"
extract_wav 43 "basic human voices ready"
extract_wav 44 "basic orc voices ready"
extract_wav 45 "basic human voices help 1"
extract_wav 46 "basic orc voices help 1"
extract_wav 47 "basic human voices help 2"
extract_wav 48 "basic orc voices help 2"
extract_wav 49 "basic human voices dead"
extract_wav 50 "basic orc voices dead"
extract_wav 51 "ship sinking"
extract_wav 52 "explosion 1"
extract_wav 53 "explosion 2"
extract_wav 54 "explosion 3"
extract_wav 55 "catapult-ballista attack"
extract_wav 56 "tree chopping 1"
extract_wav 57 "tree chopping 2"
extract_wav 58 "tree chopping 3"
extract_wav 59 "tree chopping 4"
extract_wav 60 "sword attack 1"
extract_wav 61 "sword attack 2"
extract_wav 62 "sword attack 3"
extract_wav 63 "punch"
extract_wav 64 "fireball hit"
extract_wav 65 "fireball throw"
extract_wav 66 "bow throw"
extract_wav 67 "bow hit"
extract_wav 68 "spells basic spell sound"
extract_wav 69 "blacksmith"
extract_wav 70 "church"
extract_wav 71 "altar of storms"
extract_wav 72 "stables"
extract_wav 73 "ogre mound"
extract_wav 74 "farm"
extract_wav 75 "pig farm"
extract_wav 76 "gold mine"
extract_wav 77 "axe throw"
extract_wav 78 "tanker acknowledgement"
extract_wav 79 "fist"
extract_wav 80 "shipyard"
extract_wav 81 "peasant attack"
extract_wav 82 "oil platform"
extract_wav 83 "oil refinery"
extract_wav 84 "lumbermill"
extract_wav 85 "transport docking"
extract_wav 86 "burning"
extract_wav 87 "gryphon aviary"
extract_wav 88 "dragon roost"
extract_wav 89 "foundry"
extract_wav 90 "gnomish inventor"
extract_wav 91 "goblin alchemist"
extract_wav 92 "mage tower"
extract_wav 93 "temple of the damned"
extract_wav 94 'capture (human)'
extract_wav 95 'capture (orc)'
extract_wav 96 'rescue (human)'
extract_wav 97 'rescue (orc)'
extract_wav 98 "bloodlust"
extract_wav 99 "death and decay"
extract_wav 100 "death coil"
extract_wav 101 "exorcism"
extract_wav 102 "flame shield"
extract_wav 103 "haste"
extract_wav 104 "healing"
extract_wav 105 "holy vision"
extract_wav 106 "blizzard"
extract_wav 107 "invisibility"
extract_wav 108 "eye of kilrogg"
extract_wav 109 "polymorph"
extract_wav 110 "slow"
extract_wav 111 "lightning"
extract_wav 112 "touch of darkness"
extract_wav 113 "unholy armour"
extract_wav 114 "whirlwind"
extract_wav 115 "peon ready"
extract_wav 116 "death knight annoyed 1"
extract_wav 117 "death knight annoyed 2"
extract_wav 118 "death knight annoyed 3"
extract_wav 119 "death knight ready"
extract_wav 120 "death knight selected 1"
extract_wav 121 "death knight selected 2"
extract_wav 122 "death knight acknowledgement 1"
extract_wav 123 "death knight acknowledgement 2"
extract_wav 124 "death knight acknowledgement 3"
extract_wav 125 "dwarven demolition squad annoyed 1"
extract_wav 126 "dwarven demolition squad annoyed 2"
extract_wav 127 "dwarven demolition squad annoyed 3"
extract_wav 128 "dwarven demolition squad ready"
extract_wav 129 "dwarven demolition squad selected 1"
extract_wav 130 "dwarven demolition squad selected 2"
extract_wav 131 "dwarven demolition squad acknowledgement 1"
extract_wav 132 "dwarven demolition squad acknowledgement 2"
extract_wav 133 "dwarven demolition squad acknowledgement 3"
extract_wav 134 "dwarven demolition squad acknowledgement 4"
extract_wav 135 "dwarven demolition squad acknowledgement 5"
extract_wav 136 "elven archer-ranger annoyed 1"
extract_wav 137 "elven archer-ranger annoyed 2"
extract_wav 138 "elven archer-ranger annoyed 3"
extract_wav 139 "elven archer-ranger ready"
extract_wav 140 "elven archer-ranger selected 1"
extract_wav 141 "elven archer-ranger selected 2"
extract_wav 142 "elven archer-ranger selected 3"
extract_wav 143 "elven archer-ranger selected 4"
extract_wav 144 "elven archer-ranger acknowledgement 1"
extract_wav 145 "elven archer-ranger acknowledgement 2"
extract_wav 146 "elven archer-ranger acknowledgement 3"
extract_wav 147 "elven archer-ranger acknowledgement 4"
extract_wav 148 "gnomish flying machine annoyed 1"
extract_wav 149 "gnomish flying machine annoyed 2"
extract_wav 150 "gnomish flying machine annoyed 3"
extract_wav 151 "gnomish flying machine annoyed 4"
extract_wav 152 "gnomish flying machine annoyed 5"
extract_wav 153 "gnomish flying machine ready"
extract_wav 154 "gnomish flying machine acknowledgement 1"
extract_wav 155 "goblin sappers annoyed 1"
extract_wav 156 "goblin sappers annoyed 2"
extract_wav 157 "goblin sappers annoyed 3"
extract_wav 158 "goblin sappers ready"
extract_wav 159 "goblin sappers selected 1"
extract_wav 160 "goblin sappers selected 2"
extract_wav 161 "goblin sappers selected 3"
extract_wav 162 "goblin sappers selected 4"
extract_wav 163 "goblin sappers acknowledgement 1"
extract_wav 164 "goblin sappers acknowledgement 2"
extract_wav 165 "goblin sappers acknowledgement 3"
extract_wav 166 "goblin sappers acknowledgement 4"
extract_wav 167 "goblin zeppelin annoyed 1"
extract_wav 168 "goblin zeppelin annoyed 2"
extract_wav 169 "goblin zeppelin ready"
extract_wav 170 "goblin zeppelin acknowledgement 1"
extract_wav 171 "knight annoyed 1"
extract_wav 172 "knight annoyed 2"
extract_wav 173 "knight annoyed 3"
extract_wav 174 "knight ready"
extract_wav 175 "knight selected 1"
extract_wav 176 "knight selected 2"
extract_wav 177 "knight selected 3"
extract_wav 178 "knight selected 4"
extract_wav 179 "knight acknowledgement 1"
extract_wav 180 "knight acknowledgement 2"
extract_wav 181 "knight acknowledgement 3"
extract_wav 182 "knight acknowledgement 4"
extract_wav 183 "paladin annoyed 1"
extract_wav 184 "paladin annoyed 2"
extract_wav 185 "paladin annoyed 3"
extract_wav 186 "paladin ready"
extract_wav 187 "paladin selected 1"
extract_wav 188 "paladin selected 2"
extract_wav 189 "paladin selected 3"
extract_wav 190 "paladin selected 4"
extract_wav 191 "paladin acknowledgement 1"
extract_wav 192 "paladin acknowledgement 2"
extract_wav 193 "paladin acknowledgement 3"
extract_wav 194 "paladin acknowledgement 4"
extract_wav 195 "ogre annoyed 1"
extract_wav 196 "ogre annoyed 2"
extract_wav 197 "ogre annoyed 3"
extract_wav 198 "ogre annoyed 4"
extract_wav 199 "ogre annoyed 5"
extract_wav 200 "ogre ready"
extract_wav 201 "ogre selected 1"
extract_wav 202 "ogre selected 2"
extract_wav 203 "ogre selected 3"
extract_wav 204 "ogre selected 4"
extract_wav 205 "ogre acknowledgement 1"
extract_wav 206 "ogre acknowledgement 2"
extract_wav 207 "ogre acknowledgement 3"
extract_wav 208 "ogre-mage annoyed 1"
extract_wav 209 "ogre-mage annoyed 2"
extract_wav 210 "ogre-mage annoyed 3"
extract_wav 211 "ogre-mage ready"
extract_wav 212 "ogre-mage selected 1"
extract_wav 213 "ogre-mage selected 2"
extract_wav 214 "ogre-mage selected 3"
extract_wav 215 "ogre-mage selected 4"
extract_wav 216 "ogre-mage acknowledgement 1"
extract_wav 217 "ogre-mage acknowledgement 2"
extract_wav 218 "ogre-mage acknowledgement 3"
extract_wav 219 "ships human annoyed 1"
extract_wav 220 "ships orc annoyed 1"
extract_wav 221 "ships human annoyed 2"
extract_wav 222 "ships orc annoyed 2"
extract_wav 223 "ships human annoyed 3"
extract_wav 224 "ships orc annoyed 3"
extract_wav 225 "ships human ready"
extract_wav 226 "ships orc ready"
extract_wav 227 "ships human selected 1"
extract_wav 228 "ships orc selected 1"
extract_wav 229 "ships human selected 2"
extract_wav 230 "ships orc selected 2"
extract_wav 231 "ships human selected 3"
extract_wav 232 "ships orc selected 3"
extract_wav 233 "ships human acknowledgement 1"
extract_wav 234 "ships orc acknowledgement 1"
extract_wav 235 "ships human acknowledgement 2"
extract_wav 236 "ships orc acknowledgement 2"
extract_wav 237 "ships human acknowledgement 3"
extract_wav 238 "ships orc acknowledgement 3"
extract_wav 239 "ships submarine annoyed 1"
extract_wav 240 "ships submarine annoyed 2"
extract_wav 241 "ships submarine annoyed 3"
extract_wav 242 "ships submarine annoyed 4"
extract_wav 243 "troll axethrower-berserker annoyed 1"
extract_wav 244 "troll axethrower-berserker annoyed 2"
extract_wav 245 "troll axethrower-berserker annoyed 3"
extract_wav 246 "troll axethrower-berserker ready"
extract_wav 247 "troll axethrower-berserker selected 1"
extract_wav 248 "troll axethrower-berserker selected 2"
extract_wav 249 "troll axethrower-berserker selected 3"
extract_wav 250 "troll axethrower-berserker acknowledgement 1"
extract_wav 251 "troll axethrower-berserker acknowledgement 2"
extract_wav 252 "troll axethrower-berserker acknowledgement 3"
extract_wav 253 "mage annoyed 1"
extract_wav 254 "mage annoyed 2"
extract_wav 255 "mage annoyed 3"
extract_wav 256 "mage ready"
extract_wav 257 "mage selected 1"
extract_wav 258 "mage selected 2"
extract_wav 259 "mage selected 3"
extract_wav 260 "mage acknowledgement 1"
extract_wav 261 "mage acknowledgement 2"
extract_wav 262 "mage acknowledgement 3"
extract_wav 263 "peasant ready"
extract_wav 264 "peasant annoyed 1"
extract_wav 265 "peasant annoyed 2"
extract_wav 266 "peasant annoyed 3"
extract_wav 267 "peasant annoyed 4"
extract_wav 268 "peasant annoyed 5"
extract_wav 269 "peasant annoyed 6"
extract_wav 270 "peasant annoyed 7"
extract_wav 271 "peasant selected 1"
extract_wav 272 "peasant selected 2"
extract_wav 273 "peasant selected 3"
extract_wav 274 "peasant selected 4"
extract_wav 275 "peasant acknowledgement 1"
extract_wav 276 "peasant acknowledgement 2"
extract_wav 277 "peasant acknowledgement 3"
extract_wav 278 "peasant acknowledgement 4"
extract_wav 279 "dragon ready 2"
extract_wav 280 "dragon selected"
extract_wav 281 "dragon acknowledgement 1"
extract_wav 282 "dragon acknowledgement 2"
extract_wav 283 "gryphon rider selected"
extract_wav 284 "gryphon rider griffon1"
extract_wav 285 "gryphon rider griffon2"
extract_wav 286 "sheep selected"
extract_wav 287 "sheep annoyed"
extract_wav 288 "seal selected"
extract_wav 289 "seal annoyed"
extract_wav 290 "pig selected"
extract_wav 291 "pig annoyed"
extract_wav 292 "catapult-ballista movement"
extract_wav 293 "alleria annoyed 1"
extract_wav 294 "alleria annoyed 2"
extract_wav 295 "alleria annoyed 3"
extract_wav 296 "alleria selected 1"
extract_wav 297 "alleria selected 2"
extract_wav 298 "alleria selected 3"
extract_wav 299 "alleria acknowledgement 1"
extract_wav 300 "alleria acknowledgement 2"
extract_wav 301 "alleria acknowledgement 3"
extract_wav 302 "danath annoyed 1"
extract_wav 303 "danath annoyed 2"
extract_wav 304 "danath annoyed 3"
extract_wav 305 "danath selected 1"
extract_wav 306 "danath selected 2"
extract_wav 307 "danath selected 3"
extract_wav 308 "danath acknowledgement 1"
extract_wav 309 "danath acknowledgement 2"
extract_wav 310 "danath acknowledgement 3"
extract_wav 311 "khadgar annoyed 1"
extract_wav 312 "khadgar annoyed 2"
extract_wav 313 "khadgar annoyed 3"
extract_wav 314 "khadgar selected 1"
extract_wav 315 "khadgar selected 2"
extract_wav 316 "khadgar selected 3"
extract_wav 317 "khadgar acknowledgement 1"
extract_wav 318 "khadgar acknowledgement 2"
extract_wav 319 "khadgar acknowledgement 3"
extract_wav 320 "kurdran annoyed 1"
extract_wav 321 "kurdran annoyed 2"
extract_wav 322 "kurdran annoyed 3"
extract_wav 323 "kurdran selected 1"
extract_wav 324 "kurdran selected 2"
extract_wav 325 "kurdran selected 3"
extract_wav 326 "kurdran acknowledgement 1"
extract_wav 327 "kurdran acknowledgement 2"
extract_wav 328 "kurdran acknowledgement 3"
extract_wav 329 "turalyon annoyed 1"
extract_wav 330 "turalyon annoyed 2"
extract_wav 331 "turalyon annoyed 3"
extract_wav 332 "turalyon selected 1"
extract_wav 333 "turalyon selected 2"
extract_wav 334 "turalyon selected 3"
extract_wav 335 "turalyon acknowledgement 1"
extract_wav 336 "turalyon acknowledgement 2"
extract_wav 337 "turalyon acknowledgement 3"
extract_wav 338 "deathwing annoyed 1"
extract_wav 339 "deathwing annoyed 2"
extract_wav 340 "deathwing annoyed 3"
extract_wav 341 "deathwing selected 1"
extract_wav 342 "deathwing selected 2"
extract_wav 343 "deathwing selected 3"
extract_wav 344 "deathwing acknowledgement 1"
extract_wav 345 "deathwing acknowledgement 2"
extract_wav 346 "deathwing acknowledgement 3"
extract_wav 347 "dentarg annoyed 1"
extract_wav 348 "dentarg annoyed 2"
extract_wav 349 "dentarg annoyed 3"
extract_wav 350 "dentarg selected 1"
extract_wav 351 "dentarg selected 2"
extract_wav 352 "dentarg selected 3"
extract_wav 353 "dentarg acknowledgement 1"
extract_wav 354 "dentarg acknowledgement 2"
extract_wav 355 "dentarg acknowledgement 3"
extract_wav 356 "grom hellscream annoyed 1"
extract_wav 357 "grom hellscream annoyed 2"
extract_wav 358 "grom hellscream annoyed 3"
extract_wav 359 "grom hellscream selected 1"
extract_wav 360 "grom hellscream selected 2"
extract_wav 361 "grom hellscream selected 3"
extract_wav 362 "grom hellscream acknowledgement 1"
extract_wav 363 "grom hellscream acknowledgement 2"
extract_wav 364 "grom hellscream acknowledgement 3"
extract_wav 365 "korgath bladefist annoyed 1"
extract_wav 366 "korgath bladefist annoyed 2"
extract_wav 367 "korgath bladefist annoyed 3"
extract_wav 368 "korgath bladefist selected 1"
extract_wav 369 "korgath bladefist selected 2"
extract_wav 370 "korgath bladefist selected 3"
extract_wav 371 "korgath bladefist acknowledgement 1"
extract_wav 372 "korgath bladefist acknowledgement 2"
extract_wav 373 "korgath bladefist acknowledgement 3"
extract_wav 374 "teron gorefiend annoyed 1"
extract_wav 375 "teron gorefiend annoyed 2"
extract_wav 376 "teron gorefiend annoyed 3"
extract_wav 377 "teron gorefiend selected 1"
extract_wav 378 "teron gorefiend selected 2"
extract_wav 379 "teron gorefiend selected 3"
extract_wav 380 "teron gorefiend acknowledgement 1"
extract_wav 381 "teron gorefiend acknowledgement 2"
extract_wav 382 "teron gorefiend acknowledgement 3"
extract_wav 383 "warthog selected"
extract_wav 384 "warthog annoyed"

###############################################################################
##	INTERFACE
###############################################################################

DATAFILE=$REZDAT

extract_gfu 0 "interface/buttons 1"
extract_gfu 1 "interface/buttons 2"
extract_img 3 "interface/panel 1 (humans)"
extract_img 4 "interface/panel 1 (orcs)"
extract_img 5 "interface/panel 2 (humans)"
extract_img 6 "interface/panel 2 (orcs)"
extract_img 7 "interface/panel 3 (humans)"
extract_img 8 "interface/panel 3 (orcs)"
extract_img 9 "interface/panel 4 (humans)"
extract_img 10 "interface/panel 4 (orcs)"
extract_img 11 "interface/panel 5 (humans)"
extract_img 12 "interface/panel 5 (orcs)"

###############################################################################
##	TEXT
###############################################################################

extract_txt 65 "level01h"
extract_txt 66 "level01o"
extract_txt 67 "level02h"
extract_txt 68 "level02o"
extract_txt 69 "level03h"
extract_txt 70 "level03o"
extract_txt 71 "level04h"
extract_txt 72 "level04o"
extract_txt 73 "level05h"
extract_txt 74 "level05o"
extract_txt 75 "level06h"
extract_txt 76 "level06o"
extract_txt 77 "level07h"
extract_txt 78 "level07o"
extract_txt 79 "level08h"
extract_txt 80 "level08o"
extract_txt 81 "level09h"
extract_txt 82 "level09o"
extract_txt 83 "level10h"
extract_txt 84 "level10o"
extract_txt 85 "level11h"
extract_txt 86 "level11o"
extract_txt 87 "level12h"
extract_txt 88 "level12o"
extract_txt 89 "level13h"
extract_txt 90 "level13o"
extract_txt 91 "level14h"
extract_txt 92 "level14o"

extract_txt 99 "levelx01h"
extract_txt 100 "levelx01o"
extract_txt 101 "levelx02h"
extract_txt 102 "levelx02o"
extract_txt 103 "levelx03h"
extract_txt 104 "levelx03o"
extract_txt 105 "levelx04h"
extract_txt 106 "levelx04o"
extract_txt 107 "levelx05h"
extract_txt 108 "levelx05o"
extract_txt 109 "levelx06h"
extract_txt 110 "levelx06o"
extract_txt 111 "levelx07h"
extract_txt 112 "levelx07o"
extract_txt 113 "levelx08h"
extract_txt 114 "levelx08o"
extract_txt 115 "levelx09h"
extract_txt 116 "levelx09o"
extract_txt 117 "levelx10h"
extract_txt 118 "levelx10o"
extract_txt 119 "levelx11h"
extract_txt 120 "levelx11o"
extract_txt 121 "levelx12h"
extract_txt 122 "levelx12o"

###############################################################################
##	MISC
###############################################################################

#
#	Copy original puds into data directory
#
echo "Copy puds and compressing"
cp $DATADIR/../*.pud $DIR/puds
cp -r $PUDS $DIR
chmod -R +w $DIR/puds
gzip --force --best `find $DIR/puds -type f -print`

#
##	The default pud.
#
ln -s puds/internal/internal12.pud.gz $DIR/default.pud.gz
