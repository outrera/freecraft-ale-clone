#!/bin/sh

#
# $Id: createccl.sh,v 1.10 2006/06/16 18:13:24 nehalmistry Exp $
#

# Path to the wc2 ccl directory
WC2CCL="../wargus/scripts"

# Path to the aleona ccl directory
ALEONACCL="./data.aleona/scripts"

if [ "$1" ]; then
    WC2CCL=$1
fi
if [ "$2" ]; then
    ALEONACCL=$2
fi

# Make the aleona dirs if they don't exist already
if [ ! -d $ALEONACCL ]; then mkdir $ALEONACCL; fi
if [ ! -d $ALEONACCL/ai ]; then mkdir $ALEONACCL/ai; fi
if [ ! -d $ALEONACCL/alliance ]; then mkdir $ALEONACCL/alliance; fi
if [ ! -d $ALEONACCL/mythical ]; then mkdir $ALEONACCL/mythical; fi
if [ ! -d $ALEONACCL/tilesets ]; then mkdir $ALEONACCL/tilesets; fi


cat $WC2CCL/../default.cm \
 > $ALEONACCL/../default.cm

cat $WC2CCL/ai.lua | sed \
 -e "s/human/alliance/g" \
 -e "s/unit-alliance-wall/unit-human-wall/" \
 -e "s/unit-alliance-refinery/unit-human-refinery/" \
 -e "s/unit-alliance-oil-tanker/unit-human-oil-tanker/g" \
 -e "s/elven-destroyer/alliance-destroyer/" \
 -e "s/gnomish-inventor/inventor/" \
 -e "s/gnomish-flying-machine/balloon/" \
 -e "s/gnomish-submarine/alliance-submarine/" \
 -e "s/orc/mythical/g" \
 -e "s/fmythicale/force/g" \
 -e "s/unit-mythical-wall/unit-orc-wall/g" \
 -e "s/unit-mythical-refinery/unit-orc-refinery/g" \
 -e "s/unit-mythical-oil-tanker/unit-orc-oil-tanker/g" \
 -e "s/troll-destroyer/mythical-destroyer/" \
 -e "s/exmythicalism/exorcism/" \
 -e "s/giant-turtle/mythical-submarine/" \
 -e "s/goblin-zeppelin/zeppelin/" \
 -e "s/goblin-alchemist/alchemist/" \
 > $ALEONACCL/ai.lua

cat $WC2CCL/ai/air_attack.lua \
 > $ALEONACCL/ai/air_attack.lua

cat $WC2CCL/ai/land_attack.lua \
 > $ALEONACCL/ai/land_attack.lua

cat $WC2CCL/ai/passive.lua \
 > $ALEONACCL/ai/passive.lua

cat $WC2CCL/ai/sea_attack.lua \
 > $ALEONACCL/ai/sea_attack.lua

cat $WC2CCL/anim.lua | sed \
 -e "s/human/alliance/" \
 -e "s/Gnomish_Submarine/Submarine/" \
 -e "s/gnomish-submarine/submarine/" \
 -e "s/Elven_Destroyer/Alliance_Destroyer/" \
 -e "s/elven-destroyer/alliance-destroyer/" \
 -e "s/gnomish-flying-machine/balloon/" \
 -e "s/orc/mythical/" \
 -e "s/Troll_Destroyer/Mythical_Destroyer/" \
 -e "s/goblin-zeppelin/zeppelin/" \
 -e "s/Grom_Hellscream/Beast_Cry/" \
 -e "s/Danath/Arthor_Literios/" \
 -e "s/Korgath_Bladefist/Quick_Blade/" \
 -e "s/Turalyon/Knight_Rider/" \
 -e "s/Lothar/Wise_Man/" \
 -e "s/Uther_Lightbringer/Man_of_Light/" \
 -e "s/Dentarg/Fad_Man/" \
 -e "s/Cho'gall/Double_Head/" \
 -e "s/Alleria/Female_Hero/" \
 -e "s/Zuljin/Sharp_Axe/" \
 -e "s/Death_Knight_Rider/Death_Knight/" \
 -e "s/Teron_Gorefiend/Evil_Knight/" \
 -e "s/Gul'dan/Ice_Bringer/" \
 -e "s/Khadgar/White_Mage/" \
 -e "s/Kurdan_and_Sky'ree/Flying_angle/" \
 -e "s/Deathwing/Fire_Breeze/" \
 -e "s/Eye_of_Kilrogg/Eye_of_Vision/" \
 -e "s/kurdan-and-sky-ree/flying-angle/" \
 -e "s/deathwing/fire-breeze/" \
 > $ALEONACCL/anim.lua


cat $WC2CCL/buttons.lua | sed \
 -e "s/human/alliance/" \
 -e "s/orc/mythical/" \
 > $ALEONACCL/buttons.lua


#cat $WC2CCL/campaigns.lua | sed \
# -e "s///" \
# > $ALEONACCL/campaigns.lua

cat $WC2CCL/cheats.lua \
 > $ALEONACCL/cheats.lua

cat $WC2CCL/constructions.lua | sed \
 -e "s/human/alliance/" \
 -e "s/orc/mythical/" \
 > $ALEONACCL/constructions.lua


cat $WC2CCL/credits.lua \
 > $ALEONACCL/credits.lua


cat $WC2CCL/editor.lua | sed \
 -e "s/human-blacksmith/alliance-blacksmith/g" \
 -e "s/human/alliance/" \
 -e "s/unit-alliance-wall/unit-human-wall/" \
 -e "s/unit-alliance-refinery/unit-human-refinery/" \
 -e "s/unit-alliance-oil-tanker/unit-human-oil-tanker/g" \
 -e "s/elven-destroyer/alliance-destroyer/" \
 -e "s/gnomish-inventor/inventor/" \
 -e "s/gnomish-flying-machine/balloon/" \
 -e "s/gnomish-submarine/alliance-submarine/" \
 -e "s/orc/mythical/" \
 -e "s/unit-mythical-wall/unit-orc-wall/g" \
 -e "s/unit-mythical-refinery/unit-orc-refinery/g" \
 -e "s/unit-mythical-oil-tanker/unit-orc-oil-tanker/g" \
 -e "s/troll-destroyer/mythical-destroyer/" \
 -e "s/exmythicalism/exorcism/" \
 -e "s/giant-turtle/mythical-submarine/" \
 -e "s/goblin-zeppelin/zeppelin/" \
 -e "s/goblin-alchemist/alchemist/" \
 -e "s/alleria/female-hero/" \
 -e "s/kurdan-and-sky-ree/flying-angle/" \
 -e "s/khadgar/white-mage/" \
 -e "s/turalyon/knight-rider/" \
 -e "s/danath/arthor-literios/" \
 -e "s/lothar/wise-man/" \
 -e "s/uther-lightbringer/man-of-light/" \
 -e "s/orc-start/mythical-start/" \
 -e "s/temple-of-the-damned/mage-tower/" \
 -e "s/teron-gorefiend/evil-knight/" \
 -e "s/dentarg/fad-man/" \
 -e "s/grom-hellscream/beast-cry/" \
 -e "s/deathwing/fire-breeze/" \
 -e "s/korgath-bladefist/quick-blade/" \
 -e "s/gul-dan/ice-bringer/" \
 -e "s/cho-gall/double-head/" \
 -e "s/zuljin/sharp-axe/" \
 > $ALEONACCL/editor.lua


cat $WC2CCL/fonts.lua | sed \
 -e "s/small_episode_titles/large/" \
 -e "s/32, 35/17, 17/" \
 -e "s/large_episode_titles/large/" \
 -e "s/52, 50/17, 17/" \
 > $ALEONACCL/fonts.lua


cat $WC2CCL/stratagus.lua | sed \
 -e "s/list_'wc2/list_'aleona-wc2/" \
 > $ALEONACCL/stratagus.lua


cat $WC2CCL/icons.lua | sed \
 -e "s/human/alliance/" \
 -e "s/elven-destroyer/alliance-destroyer/" \
 -e "s/gnomish-inventor/inventor/" \
 -e "s/gnomish-submarine/alliance-submarine/" \
 -e "s/gnomish-flying-machine/balloon/" \
 -e "s/orc/mythical/" \
 -e "s/exmythicalism/exorcism/" \
 -e "s/troll-destroyer/mythical-destroyer/" \
 -e "s/giant-turtle/mythical-submarine/" \
 -e "s/goblin-alchemist/alchemist/" \
 -e "s/goblin-zeppelin/zeppelin/" \
 -e "s/lothar/wise-man/" \
 -e "s/gul-dan/ice-bringer/" \
 -e "s/uther-lightbringer/man-of-light/" \
 -e "s/zuljin/sharp-axe/" \
 -e "s/cho-gall/double-head/" \
 -e "s/korgath-bladefist/quick-blade/" \
 -e "s/alleria/female-hero/" \
 -e "s/danath/arthor-literios/" \
 -e "s/teron-gorefiend/evil-knight/" \
 -e "s/grom-hellscream/beast-cry/" \
 -e "s/kurdan-and-sky-ree/flying-angle/" \
 -e "s/deathwing/fire-breeze/" \
 -e "s/khadgar/white-mage/" \
 -e "s/dentarg/fad-man/" \
 -e "s/turalyon/knight-rider/" \
 > $ALEONACCL/icons.lua


cat $WC2CCL/keystrokes.lua \
 > $ALEONACCL/keystrokes.lua


cat $WC2CCL/menus.lua | sed \
 -e "s/Forest/Summer/" \
 -e "s/Orc_Swamp/Swamp/" \
 -e "s/Human/Alliance/" \
 -e "s/Orc/Mythical/" \
 > $ALEONACCL/menus.lua


cat $WC2CCL/missiles.lua \
 > $ALEONACCL/missiles.lua


cat $WC2CCL/ranks.lua | sed \
 -e "s/human/alliance/" \
 -e "s/orc/mythical/" \
 -e "s/Servant/Rank0/" \
 -e "s/Peasant/Rank1/" \
 -e "s/Squire/Rank2/" \
 -e "s/Footman/Rank3/" \
 -e "s/Corporal/Rank4/" \
 -e "s/Sergeant/Rank5/" \
 -e "s/Lieutenant/Rank6/" \
 -e "s/Captain/Rank7/" \
 -e "s/Major/Rank8/" \
 -e "s/Knight/Rank9/" \
 -e "s/General/Rank10/" \
 -e "s/Grand Admiral/Rank14/" \
 -e "s/Admiral/Rank11/" \
 -e "s/Marshall/Rank12/" \
 -e "s/Lord/Rank13/" \
 -e "s/Highlord/Rank15/" \
 -e "s/Thundergod/Rank16/" \
 -e "s/God/Rank17/" \
 -e "s/Designer/Rank18/" \
 -e "s/Slave/Rank0/" \
 -e "s/Peon/Rank1/" \
 -e "s/Rogue/Rank2/" \
 -e "s/Grunt/Rank3/" \
 -e "s/Slasher/Rank4/" \
 -e "s/Marauder/Rank5/" \
 -e "s/Commander/Rank6/" \
 -e "s/Master/Rank11/" \
 -e "s/Marshall/Rank12/" \
 -e "s/Chieftain/Rank13/" \
 -e "s/Overlord/Rank14/" \
 -e "s/War Chief/Rank15/" \
 -e "s/Demigod/Rank16/" \
 > $ALEONACCL/ranks.lua

cat $WC2CCL/scripts.lua | sed \
 -e "s/human/alliance/g" \
 -e "s/orc/mythical/g" \
 > $ALEONACCL/scripts.lua

# FIXME:
touch $ALEONACCL/sound.lua
cat $WC2CCL/sound.lua | sed \
 -e "s/human/alliance/g" \
 -e "s/orc/mythical/g" \
 -e "s/goblin-zeppelin/zeppelin/" \
 -e "s/goblin_zeppelin/zeppelin/" \
> $ALEONACCL/sound.lua

echo "expansion = false" > $ALEONACCL/wc2-config.lua
touch $ALEONACCL/campaigns.lua

cat $WC2CCL/tilesets.lua \
 > $ALEONACCL/tilesets.lua

cat $WC2CCL/tilesets/summer.lua \
 > $ALEONACCL/tilesets/summer.lua

cat $WC2CCL/tilesets/winter.lua \
 > $ALEONACCL/tilesets/winter.lua

cat $WC2CCL/tilesets/swamp.lua \
 > $ALEONACCL/tilesets/swamp.lua

cat $WC2CCL/tips.lua \
 > $ALEONACCL/tips.lua

cat $WC2CCL/ui.lua | sed \
 -e "s/human/alliance/" \
 -e "s/orc/mythical/" \
 > $ALEONACCL/ui.lua

cat $WC2CCL/human/ui.lua | sed \
 -e "s/human/alliance/" \
 -e "s/orc/mythical/" \
 -e "s/human_gauntlet/alliance_claw/" \
 -e "s/filler-right/filler1/" \
 > $ALEONACCL/alliance/ui.lua

cat $WC2CCL/orc/ui.lua | sed \
 -e "s/human/alliance/" \
 -e "s/orcish/mythical/" \
 -e "s/orc/mythical/" \
 -e "s/filler-right/filler1/" \
 > $ALEONACCL/mythical/ui.lua

cat $WC2CCL/units.lua | sed \
 -e "s/human/alliance/" \
 -e "s/orc/mythical/" \
 > $ALEONACCL/units.lua

cat $WC2CCL/upgrade.lua | sed \
 -e "s/human/alliance/" \
 -e "s/orc/mythical/" \
 > $ALEONACCL/upgrade.lua

cat $WC2CCL/wc2.lua | sed \
 -e "s/human/alliance/" \
 -e "s/orc/mythical/" \
 > $ALEONACCL/wc2.lua


cat $WC2CCL/human/anim.lua | sed \
 -e "s/human/alliance/" \
 -e "s/Gnomish_Submarine/Submarine/" \
 -e "s/gnomish-submarine/submarine/" \
 -e "s/Elven_Destroyer/Alliance_Destroyer/" \
 -e "s/elven-destroyer/alliance-destroyer/" \
 -e "s/gnomish-flying-machine/balloon/" \
 -e "s/orc/mythical/" \
 -e "s/Troll_Destroyer/Mythical_Destroyer/" \
 -e "s/goblin-zeppelin/zeppelin/" \
 -e "s/Grom_Hellscream/Beast_Cry/" \
 -e "s/Danath/Arthor_Literios/" \
 -e "s/Korgath_Bladefist/Quick_Blade/" \
 -e "s/Turalyon/Knight_Rider/" \
 -e "s/Lothar/Wise_Man/" \
 -e "s/Uther_Lightbringer/Man_of_Light/" \
 -e "s/Dentarg/Fad_Man/" \
 -e "s/Cho'gall/Double_Head/" \
 -e "s/Alleria/Female_Hero/" \
 -e "s/Zuljin/Sharp_Axe/" \
 -e "s/Death_Knight_Rider/Death_Knight/" \
 -e "s/Teron_Gorefiend/Evil_Knight/" \
 -e "s/Gul'dan/Ice_Bringer/" \
 -e "s/Khadgar/White_Mage/" \
 -e "s/Kurdan_and_Sky'ree/Flying_angle/" \
 -e "s/Deathwing/Fire_Breeze/" \
 -e "s/Eye_of_Kilrogg/Eye_of_Vision/" \
 -e "s/kurdan-and-sky-ree/flying-angle/" \
 -e "s/deathwing/fire-breeze/" \
 > $ALEONACCL/alliance/anim.lua
 
 
cat $WC2CCL/human/buttons.lua | sed \
 -e "s/human/alliance/g" \
 -e "s/unit-alliance-wall/unit-human-wall/" \
 -e "s/unit-alliance-refinery/unit-human-refinery/" \
 -e "s/unit-alliance-oil-tanker/unit-human-oil-tanker/g" \
 -e "s/elven-destroyer/alliance-destroyer/" \
 -e "s/gnomish-inventor/inventor/" \
 -e "s/gnomish-flying-machine/balloon/" \
 -e "s/gnomish-submarine/alliance-submarine/" \
 -e "s/alleria/female-hero/" \
 -e "s/kurdan-and-sky-ree/flying-angle/" \
 -e "s/danath/arthor-literios/" \
 -e "s/turalyon/knight-rider/" \
 -e "s/lothar/wise-man/" \
 -e "s/uther-lightbringer/man-of-light/" \
 -e "s/khadgar/white-mage/" \
 -e "s/~!DESTROYER/ALLIANCE_~!DESTROYER/" \
 -e "s/GNOMISH_~!SUBMARINE/ALLIANCE_~!SUBMARINE/" \
 -e "s/'key_\"f\" 'hint \"BUILD GNOMISH ~!FLYING MACHINE\"/'key \"b\" 'hint \"BUILD ~!BALLOON\"/" \
 -e "s/GNOMISH_~!INVENTOR/~!INVENTOR/" \
 -e "s/ELVEN_~!LUMBER/ALLIANCE_~!LUMBER/" \
 > $ALEONACCL/alliance/buttons.lua


# cat $WC2CCL/human/campaign1.lua | sed \
#  > $ALEONACCL/alliance/campaign1.lua
# cat $WC2CCL/human/campaign2.lua | sed \
#  > $ALEONACCL/alliance/campaign2.lua


cat $WC2CCL/human/constructions.lua | sed \
 -e "s/human/alliance/g" \
 > $ALEONACCL/alliance/constructions.lua
# -e "s/tilesets\/winter\///g" \
# -e "s/tilesets\/swamp\///g" \



cat $WC2CCL/human/units.lua | sed \
 -e "s/human/alliance/g" \
 -e "s/unit-alliance-refinery/unit-human-refinery/" \
 -e "s/unit-alliance-oil-tanker/unit-human-oil-tanker/g" \
 -e "s/unit-alliance-wall/unit-human-wall/g" \
 -e "s/elven_archer/archer/" \
 -e "s/dwarven_demolition_squad/demolition_squad/" \
 -e "s/alleria/female-hero/" \
 -e "s/Alleria/Female_Hero/" \
 -e "s/kurdan-and-sky-ree/flying-angle/g" \
 -e "s/Kurdan_and_Sky'ree/Flying_Angle/" \
 -e "s/khadgar/white-mage/" \
 -e "s/Khadgar/White_Mage/" \
 -e "s/elven_destroyer/alliance_destroyer/g" \
 -e "s/elven-destroyer/alliance-destroyer/g" \
 -e "s/Elven_Destroyer/Alliance_Destroyer/" \
 -e "s/gnomish-submarine/alliance-submarine/g" \
 -e "s/animations-alliance-submarine/animations-submarine/" \
 -e "s/gnomish_submarine/submarine/g" \
 -e "s/Gnomish_Submarine/Alliance_Submarine/" \
 -e "s/gnomish.flying.machine/balloon/g" \
 -e "s/Gnomish_Flying_Machine/Balloon/" \
 -e "s/turalyon/knight-rider/" \
 -e "s/Turalyon/Knight_Rider/" \
 -e "s/danath/arthor-literios/" \
 -e "s/Danath/Arthor_Literios/" \
 -e "s/lothar/wise-man/" \
 -e "s/Lothar/Wise_Man/" \
 -e "s/Uther_Lightbringer/Man_of_Light/" \
 -e "s/uther-lightbringer/man-of-light/" \
 -e "s/gnomish.inventor/inventor/" \
 -e "s/Gnomish_Inventor/Inventor/" \
 -e "s/elven_lumber/alliance_lumber/" \
 -e "s/Elven_Lumber/Alliance_Lumber/" \
 > $ALEONACCL/alliance/units.lua


cat $WC2CCL/human/upgrade.lua | sed \
 -e "s/human/alliance/g" \
 -e "s/unit-alliance-wall/unit-human-wall/" \
 -e "s/unit-alliance-refinery/unit-human-refinery/" \
 -e "s/unit-alliance-oil-tanker/unit-human-oil-tanker/g" \
 -e "s/elven-destroyer/alliance-destroyer/" \
 -e "s/gnomish-inventor/inventor/" \
 -e "s/gnomish-flying-machine/balloon/" \
 -e "s/gnomish-submarine/alliance-submarine/" \
 -e "s/danath/arthor-literios/" \
 -e "s/lothar/wise-man/" \
 -e "s/uther-lightbringer/man-of-light/" \
 -e "s/turalyon/knight-rider/" \
 -e "s/alleria/female-hero/" \
 -e "s/khadgar/white-mage/" \
 -e "s/kurdan-and-sky-ree/flying-angle/g" \
 > $ALEONACCL/alliance/upgrade.lua


cat $WC2CCL/orc/anim.lua | sed \
 -e "s/human/alliance/" \
 -e "s/Gnomish_Submarine/Submarine/" \
 -e "s/gnomish-submarine/submarine/" \
 -e "s/Elven_Destroyer/Alliance_Destroyer/" \
 -e "s/elven-destroyer/alliance-destroyer/" \
 -e "s/gnomish-flying-machine/balloon/" \
 -e "s/orc/mythical/" \
 -e "s/Troll_Destroyer/Mythical_Destroyer/" \
 -e "s/troll-destroyer/mythical-destroyer/" \
 -e "s/goblin-zeppelin/zeppelin/" \
 -e "s/Grom_Hellscream/Beast_Cry/" \
 -e "s/Danath/Arthor_Literios/" \
 -e "s/Korgath_Bladefist/Quick_Blade/" \
 -e "s/Turalyon/Knight_Rider/" \
 -e "s/Lothar/Wise_Man/" \
 -e "s/Uther_Lightbringer/Man_of_Light/" \
 -e "s/Dentarg/Fad_Man/" \
 -e "s/Cho'gall/Double_Head/" \
 -e "s/Alleria/Female_Hero/" \
 -e "s/Zuljin/Sharp_Axe/" \
 -e "s/Death_Knight_Rider/Death_Knight/" \
 -e "s/Teron_Gorefiend/Evil_Knight/" \
 -e "s/Gul'dan/Ice_Bringer/" \
 -e "s/Khadgar/White_Mage/" \
 -e "s/Kurdan_and_Sky'ree/Flying_angle/" \
 -e "s/Deathwing/Fire_Breeze/" \
 -e "s/Eye_of_Kilrogg/Eye_of_Vision/" \
 -e "s/kurdan-and-sky-ree/flying-angle/" \
 -e "s/deathwing/fire-breeze/" \
 > $ALEONACCL/mythical/anim.lua


cat $WC2CCL/orc/buttons.lua | sed \
 -e "s/orc/mythical/g" \
 -e "s/unit-mythical-wall/unit-orc-wall/" \
 -e "s/unit-mythical-refinery/unit-orc-refinery/" \
 -e "s/unit-mythical-oil-tanker/unit-orc-oil-tanker/g" \
 -e "s/giant-turtle/mythical-submarine/" \
 -e "s/troll-destroyer/mythical-destroyer/" \
 -e "s/goblin-zeppelin/zeppelin/" \
 -e "s/goblin-alchemist/alchemist/" \
 -e "s/dentarg/fad-man/" \
 -e "s/cho-gall/double-head/" \
 -e "s/zuljin/sharp-axe/" \
 -e "s/grom-hellscream/beast-cry/" \
 -e "s/korgath-bladefist/quick-blade/" \
 -e "s/gul-dan/ice-bringer/" \
 -e "s/teron-gorefiend/evil-knight/" \
 -e "s/deathwing/fire-breeze/" \
 > $ALEONACCL/mythical/buttons.lua


cat $WC2CCL/orc/constructions.lua | sed \
 -e "s/orc/mythical/g" \
 > $ALEONACCL/mythical/constructions.lua


cat $WC2CCL/orc/units.lua | sed \
 -e "s/human/alliance/" \
 -e "s/unit-alliance-wall/unit-human-wall/" \
 -e "s/elven-destroyer/alliance-destroyer/" \
 -e "s/orc/mythical/g" \
 -e "s/unit-mythical-refinery/unit-orc-refinery/" \
 -e "s/unit-mythical-oil-tanker/unit-orc-oil-tanker/g" \
 -e "s/unit-mythical-wall/unit-orc-wall/g" \
 -e "s/Peon/Faun/" \
 -e "s/\/peon/\/faun/g" \
 -e "s/pig_farm/farm/" \
 -e "s/troll_axethrower/axethrower/" \
 -e "s/Troll_Axethrower/Axethrower/" \
 -e "s/teron-gorefiend/evil-knight/" \
 -e "s/Teron_Gorefiend/Evil_Knight/" \
 -e "s/dentarg/fad-man/" \
 -e "s/Dentarg/Fad_Man/" \
 -e "s/grom-hellscream/beast-cry/" \
 -e "s/Grom_Hellscream/Beast_Cry/" \
 -e "s/troll_destroyer/mythical_destroyer/" \
 -e "s/troll-destroyer/mythical-destroyer/" \
 -e "s/Troll_Destroyer/Mythical_Destroyer/" \
 -e "s/troll_lumber_mill/mythical_lumber_mill/" \
 -e "s/Troll_Lumber_Mill/Mythical_Lumber_Mill/" \
 -e "s/ogre_juggernaught/juggernaught/" \
 -e "s/Ogre_Juggernaught/Juggernaught/" \
 -e "s/deathwing/fire-breeze/g" \
 -e "s/Deathwing/Fire_Breeze/" \
 -e "s/giant-turtle/mythical-submarine/" \
 -e "s/giant_turtle/submarine/" \
 -e "s/animations-gnomish-submarine/animations-submarine/" \
 -e "s/Giant_Turtle/Mythical_Submarine/" \
 -e "s/goblin-zeppelin/zeppelin/g" \
 -e "s/goblin_zeppelin/zeppelin/" \
 -e "s/Goblin_Zeppelin/Zeppelin/" \
 -e "s/goblin-alchemist/alchemist/" \
 -e "s/goblin_alchemist/alchemist/" \
 -e "s/Goblin_Alchemist/Alchemist/" \
 -e "s/eye-of-kilrogg/eye-of-vision/" \
 -e "s/eye_of_kilrogg/eye_of_vision/" \
 -e "s/Eye_of_Kilrogg/Eye_of_Vision/" \
 -e "s/icon-eye-of-vision/icon-eye-of-kilrogg/" \
 -e "s/korgath-bladefist/quick-blade/" \
 -e "s/Korgath_Bladefist/Quick_Blade/" \
 -e "s/cho-gall/double-head/" \
 -e "s/Cho'gall/Double_Head/" \
 -e "s/zuljin/sharp-axe/" \
 -e "s/Zuljin/Sharp_Axe/" \
 -e "s/gul-dan/ice-bringer/" \
 -e "s/Gul'dan/Ice_Bringer/" \
 -e "s/Grunt/Black_Knight/" \
 -e "s/grunt.png/black_knight.png/" \
 > $ALEONACCL/mythical/units.lua


cat $WC2CCL/orc/upgrade.lua | sed \
 -e "s/orc/mythical/g" \
 -e "s/unit-mythical-wall/unit-orc-wall/" \
 -e "s/unit-mythical-refinery/unit-orc-refinery/" \
 -e "s/unit-mythical-oil-tanker/unit-orc-oil-tanker/g" \
 -e "s/troll-destroyer/mythical-destroyer/" \
 -e "s/giant-turtle/mythical-submarine/" \
 -e "s/goblin-zeppelin/zeppelin/" \
 -e "s/goblin-alchemist/alchemist/" \
 -e "s/dentarg/fad-man/" \
 -e "s/cho-gall/double-head/" \
 -e "s/zuljin/sharp-axe/" \
 -e "s/grom-hellscream/beast-cry/" \
 -e "s/korgath-bladefist/quick-blade/" \
 -e "s/gul-dan/ice-bringer/" \
 -e "s/teron-gorefiend/evil-knight/" \
 -e "s/deathwing/fire-breeze/" \
  > $ALEONACCL/mythical/upgrade.lua

cp $WC2CCL/spells.lua $ALEONACCL/spells.lua
cp $WC2CCL/widgets.lua $ALEONACCL/widgets.lua

# EOF #
