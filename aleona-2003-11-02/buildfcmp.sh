#!/bin/sh

if [ -z "$1" ]; then
    echo "Usage: $0 WARGUSDIR"
    exit 0
else
    WGDIR="$1"
fi

I=.			#input dir
O=./data.aleona		#output dir
WC2=$WGDIR		#wc2 data dir
CONTRIB=$WGDIR/contrib	#contrib dir

#uncomment for verbosity
# V="--verbose" 

#SOX="sox -r22050 -c1"
SOX="cp"

CP="cp --no-dereference"
MD="mkdir -p $V"
COMPRESS="gzip"
#COMPRESS="bzip2"

#DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU ARE DOING
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
for i in $WGDIR $I $WC2 $CONTRIB; do
    if [ ! -d $i ]; then
	echo
	echo "ERROR: $i not found, please edit $0 and correct location"
	echo
	exit
    fi
done

if [ "$1" = "--help" ]; then
echo "usage: $0 [ -m ]"
echo "	-m	make a zip and tar.gz package"
echo
exit
fi

DATE=`date +"%y%m%d"`
BUILDPACKAGE=0
if [ "$1" = "-m" ]; then
    BUILDPACKAGE=1
fi

echo "Creating dirs and copying files from CVS"

rm -rf $O
$MD "$O"

#campaigns
$MD "$O/campaigns"
$MD "$O/campaigns/alliance"
$CP "$I/editions/wc2-edition/campaigns/cms/level-01.cm" "$O/campaigns/alliance/level01a.cm"
$CP "$I/editions/wc2-edition/campaigns/puds/level-01.pud" "$O/campaigns/alliance/level01a.pud"
$CP "$I/editions/wc2-edition/campaigns/texts/level-01.txt" "$O/campaigns/alliance/level01a.txt"
$CP "$I/editions/wc2-edition/campaigns/cms/level-02.cm" "$O/campaigns/alliance/level02a.cm"
$CP "$I/editions/wc2-edition/campaigns/puds/level-02.pud" "$O/campaigns/alliance/level02a.pud"
$CP "$I/editions/wc2-edition/campaigns/texts/level-02.txt" "$O/campaigns/alliance/level02a.txt"
$CP "$I/editions/wc2-edition/campaigns/cms/level-03.cm" "$O/campaigns/alliance/level03a.cm"
$CP "$I/editions/wc2-edition/campaigns/puds/level-03.pud" "$O/campaigns/alliance/level03a.pud"
$CP "$I/editions/wc2-edition/campaigns/texts/level-03.txt" "$O/campaigns/alliance/level03a.txt"
$CP "$I/editions/wc2-edition/campaigns/cms/level-04.cm" "$O/campaigns/alliance/level04a.cm"
$CP "$I/editions/wc2-edition/campaigns/puds/level-04.pud" "$O/campaigns/alliance/level04a.pud"
$CP "$I/editions/wc2-edition/campaigns/texts/level-04.txt" "$O/campaigns/alliance/level04a.txt"
$MD "$O/campaigns/alliance/interface"
$CP "$I/editions/wc2-edition/campaigns/interface/actscreen-01.png" "$O/campaigns/alliance/interface/Act_I_-_Coast_of_Blardneg.png"
$CP "$I/editions/wc2-edition/campaigns/interface/introscreen-01.png" "$O/campaigns/alliance/interface/introscreen1.png"

#ccl
$MD "$O/ccl"
$MD "$O/ccl/alliance"
$MD "$O/ccl/mythical"
$MD "$O/ccl/tilesets"

#graphics
$MD "$O/graphics"

$MD "$O/graphics/alliance"
$MD "$O/graphics/alliance/buildings"

#BEGIN ALLIANCE BUILDINGS

$CP "$I/buildings/foundry/foundry_construction_site-summer-01.png" "$O/graphics/alliance/buildings/foundry_construction_site.png"
$CP "$I/buildings/shipyard/shipyard_construction_site-summer-01.png" "$O/graphics/alliance/buildings/shipyard_construction_site.png"
$CP "$I/buildings/refinery/refinery_construction_site-summer-01.png" "$O/graphics/alliance/buildings/refinery_construction_site.png"

#END ALLIANCE BUILDINGS

$MD "$O/graphics/alliance/units"

#BEGIN ALLIANCE UNITS

$CP "$I/units/submarine/submarine-01.png" "$O/graphics/alliance/units/submarine.png"
$CP "$I/units/knight/knight-01.png" "$O/graphics/alliance/units/knight.png"
$CP "$I/units/peasant/peasant-01.png" "$O/graphics/alliance/units/peasant.png"
$CP "$I/units/destroyer/destroyer-01.png" "$O/graphics/alliance/units/alliance_destroyer.png"
$CP "$I/units/transport/transport-01.png" "$O/graphics/alliance/units/transport.png"
$CP "$I/units/demolition_squad/demolition_squad-01.png" "$O/graphics/alliance/units/demolition_squad.png"
$CP "$I/units/battleship/battleship-01.png" "$O/graphics/alliance/units/battleship.png"
$CP "$I/units/gryphon_rider/gryphon_rider-01.png" "$O/graphics/alliance/units/gryphon_rider.png"
$CP "$I/units/mage/mage-01.png" "$O/graphics/alliance/units/mage.png"
$CP "$I/units/footman/footman-01.png" "$O/graphics/alliance/units/footman.png"
$CP "$I/units/balloon/balloon-01.png" "$O/graphics/alliance/units/balloon.png"
$CP "$I/units/ballista/ballista-01.png" "$O/graphics/alliance/units/ballista.png"
$CP "$I/units/oil_tanker/oil_tanker_empty-01.png" "$O/graphics/alliance/units/oil_tanker_empty.png"
$CP "$I/units/oil_tanker/oil_tanker_full-01.png" "$O/graphics/alliance/units/oil_tanker_full.png"
$CP "$I/units/peasant/peasant_with_wood-01.png" "$O/graphics/alliance/units/peasant_with_wood.png"
$CP "$I/units/archer/archer-01.png" "$O/graphics/alliance/units/archer.png"
$CP "$I/units/peasant/peasant_with_gold-01.png" "$O/graphics/alliance/units/peasant_with_gold.png"
$CP "$I/misc/x_startpoint-01.png" "$O/graphics/alliance/x_startpoint.png"

#END ALLIANCE UNITS

$MD "$O/graphics/missiles"

#BEGIN MISSILES

$CP "$I/missiles/cannon_explosion-01.png" "$O/graphics/missiles/cannon_explosion.png"
$CP "$I/misc/unit_shadow-01.png" "$O/graphics/missiles/unit_shadow.png"
$CP "$I/missiles/small_fire-01.png" "$O/graphics/missiles/small_fire.png"
$CP "$I/missiles/dragon_breath-01.png" "$O/graphics/missiles/dragon_breath.png"
$CP "$I/spells/lightning-01.png" "$O/graphics/missiles/lightning.png"
$CP "$I/spells/flame_shield-01.png" "$O/graphics/missiles/flame_shield.png"
$CP "$I/missiles/explosion-01.png" "$O/graphics/missiles/explosion.png"
#$CP $O/graphics/missiles/red_cross.png
$CP "$I/missiles/cannon_tower_explosion-01.png" "$O/graphics/missiles/cannon-tower_explosion.png"
$CP "$I/missiles/ballista_bolt-01.png" "$O/graphics/missiles/ballista_bolt.png"
$CP "$I/missiles/big_fire-01.png" "$O/graphics/missiles/big_fire.png"
$CP "$I/missiles/submarine_missile-01.png" "$O/graphics/missiles/submarine_missile.png"
$CP "$I/missiles/big_cannon-01.png" "$O/graphics/missiles/big_cannon.png"
$CP "$I/spells/rune-01.png" "$O/graphics/missiles/rune.png"
$CP "$I/spells/heal_effect-01.png" "$O/graphics/missiles/heal_effect.png"
$CP "$I/missiles/turtle_missile-01.png" "$O/graphics/missiles/turtle_missile.png"
$CP "$I/missiles/daemon_fire-01.png" "$O/graphics/missiles/daemon_fire.png"
$CP "$I/spells/blizzard-01.png" "$O/graphics/missiles/blizzard.png"
$CP "$I/spells/fireball-01.png" "$O/graphics/missiles/fireball.png"
$CP "$I/missiles/gryphon_hammer-01.png" "$O/graphics/missiles/gryphon_hammer.png"
$CP "$I/missiles/green_cross-01.png" "$O/graphics/missiles/green_cross.png"
$CP "$I/spells/normal_spell-01.png" "$O/graphics/missiles/normal_spell.png"
$CP "$I/missiles/arrow-01.png" "$O/graphics/missiles/arrow.png"
$CP "$I/missiles/cannon-01.png" "$O/graphics/missiles/cannon.png"
$CP "$I/spells/touch_of_death-01.png" "$O/graphics/missiles/touch_of_death.png"
$CP "$I/missiles/ballista_catapult_impact-01.png" "$O/graphics/missiles/ballista-catapult_impact.png"
$CP "$I/spells/death_and_decay-01.png" "$O/graphics/missiles/death_and_decay.png"
$CP "$I/missiles/axe-01.png" "$O/graphics/missiles/axe.png"
$CP "$I/spells/exorcism-01.png" "$O/graphics/missiles/exorcism.png"
$CP "$I/missiles/catapult_rock-01.png" "$O/graphics/missiles/catapult_rock.png"
$CP "$I/spells/tornado-01.png" "$O/graphics/missiles/tornado.png"

#END MISSILES

$MD "$O/graphics/mythical"
$MD "$O/graphics/mythical/buildings"

#BEGIN MYTHICAL BUILDINGS

$CP "$I/buildings/foundry/foundry_construction_site-summer-02.png" "$O/graphics/mythical/buildings/foundry_construction_site.png"
$CP "$I/buildings/shipyard/shipyard_construction_site-summer-02.png" "$O/graphics/mythical/buildings/shipyard_construction_site.png"
$CP "$I/buildings/refinery/refinery_construction_site-summer-02.png" "$O/graphics/mythical/buildings/refinery_construction_site.png"

#END MYTHICAL BUILDINGS

$MD "$O/graphics/mythical/units"

#BEGIN MYTHICAL UNITS

$CP "$I/misc/o_startpoint-01.png" "$O/graphics/mythical/o_startpoint.png"
$CP "$I/units/death_knight/death_knight-01.png" "$O/graphics/mythical/units/death_knight.png"
$CP "$I/units/transport/transport-02.png" "$O/graphics/mythical/units/transport.png"
$CP "$I/units/submarine/submarine-02.png" "$O/graphics/mythical/units/submarine.png"
$CP "$I/units/juggernaught/juggernaught-01.png" "$O/graphics/mythical/units/juggernaught.png"
$CP "$I/units/axethrower/axethrower-01.png" "$O/graphics/mythical/units/axethrower.png"
$CP "$I/units/faun/faun_with_gold-01.png" "$O/graphics/mythical/units/faun_with_gold.png"
$CP "$I/units/black_knight/black_knight-01.png" "$O/graphics/mythical/units/black_knight.png"
$CP "$I/units/ogre/ogre-01.png" "$O/graphics/mythical/units/ogre.png"
$CP "$I/units/oil_tanker/oil_tanker_empty-02.png" "$O/graphics/mythical/units/oil_tanker_empty.png"
$CP "$I/units/oil_tanker/oil_tanker_full-02.png" "$O/graphics/mythical/units/oil_tanker_full.png"
$CP "$I/units/destroyer/destroyer-02.png" "$O/graphics/mythical/units/mythical_destroyer.png"
$CP "$I/units/goblin_sappers/goblin_sappers-01.png" "$O/graphics/mythical/units/goblin_sappers.png"
$CP "$I/units/zeppelin/zeppelin-01.png" "$O/graphics/mythical/units/zeppelin.png"
$CP "$I/units/eye_of_vision/eye_of_vision-01.png" "$O/graphics/mythical/units/eye_of_vision.png"
$CP "$I/units/faun/faun-01.png" "$O/graphics/mythical/units/faun.png"
$CP "$I/units/catapult/catapult-01.png" "$O/graphics/mythical/units/catapult.png"
$CP "$I/units/faun/faun_with_wood-01.png" "$O/graphics/mythical/units/faun_with_wood.png"
$CP "$I/units/dragon/dragon-01.png" "$O/graphics/mythical/units/dragon.png"

#END MYTHICAL UNITS

$MD "$O/graphics/neutral"
$MD "$O/graphics/neutral/buildings"

#BEGIN NEUTRAL BUILDINGS

$CP "$I/buildings/circle_of_power/circle_of_power-summer-01.png" "$O/graphics/neutral/buildings/circle_of_power.png"
$CP "$I/buildings/runestone/runestone-summer-01.png" "$O/graphics/neutral/buildings/runestone.png"
$CP "$I/buildings/land_construction_site/land_construction_site-summer-01.png" "$O/graphics/neutral/buildings/land_construction_site.png"

#END NEUTRAL BUILDINGS

$MD "$O/graphics/neutral/units"

#BEGIN NEUTRAL UNITS

$CP "$I/units/skeleton/skeleton-01.png" "$O/graphics/neutral/units/skeleton.png"
$CP "$I/units/daemon/daemon-01.png" "$O/graphics/neutral/units/daemon.png"
$CP "$I/units/corpse/corpse-01.png" "$O/graphics/neutral/units/corpses.png"

#END NEUTRAL UNITS

$MD "$O/graphics/tilesets"
$MD "$O/graphics/tilesets/summer"
$MD "$O/graphics/tilesets/summer/alliance"
$MD "$O/graphics/tilesets/summer/alliance/buildings"

#BEGIN ALLIANCE SUMMER BUILDINGS

$CP "$I/buildings/cannon_tower/cannon_tower-summer-06.png" "$O/graphics/tilesets/summer/alliance/buildings/cannon_tower.png"
$CP "$I/buildings/oil_platform/oil_platform-summer-01.png" "$O/graphics/tilesets/summer/alliance/buildings/oil_platform.png"
$CP "$I/buildings/foundry/foundry-summer-01.png" "$O/graphics/tilesets/summer/alliance/buildings/foundry.png"
$CP "$I/buildings/guard_tower/guard_tower-summer-07.png" "$O/graphics/tilesets/summer/alliance/buildings/guard_tower.png"
$CP "$I/buildings/scout_tower/scout_tower-summer-06.png" "$O/graphics/tilesets/summer/alliance/buildings/scout_tower.png"
$CP "$I/buildings/stables/stables-summer-03.png" "$O/graphics/tilesets/summer/alliance/buildings/stables.png"
$CP "$I/buildings/farm/farm-summer-06.png" "$O/graphics/tilesets/summer/alliance/buildings/farm.png"
$CP "$I/buildings/blacksmith/blacksmith-summer-05.png" "$O/graphics/tilesets/summer/alliance/buildings/blacksmith.png"
$CP "$I/buildings/barracks/barracks-summer-06.png" "$O/graphics/tilesets/summer/alliance/buildings/barracks.png"
$CP "$I/buildings/church/church-summer-05.png" "$O/graphics/tilesets/summer/alliance/buildings/church.png"
$CP "$I/buildings/lumber_mill/lumber_mill-summer-04.png" "$O/graphics/tilesets/summer/alliance/buildings/alliance_lumber_mill.png"
$CP "$I/buildings/refinery/refinery-summer-01.png" "$O/graphics/tilesets/summer/alliance/buildings/refinery.png"
$CP "$I/buildings/inventor/inventor-summer-03.png" "$O/graphics/tilesets/summer/alliance/buildings/inventor.png"
$CP "$I/buildings/mage_tower/mage_tower-summer-03.png" "$O/graphics/tilesets/summer/alliance/buildings/mage_tower.png"
$CP "$I/buildings/castle/castle-summer-04.png" "$O/graphics/tilesets/summer/alliance/buildings/castle.png"
$CP "$I/buildings/shipyard/shipyard-summer-01.png" "$O/graphics/tilesets/summer/alliance/buildings/shipyard.png"
$CP "$I/buildings/keep/keep-summer-04.png" "$O/graphics/tilesets/summer/alliance/buildings/keep.png"
$CP "$I/buildings/town_hall/town_hall-summer-04.png" "$O/graphics/tilesets/summer/alliance/buildings/town_hall.png"
$CP "$I/buildings/gryphon_aviary/gryphon_aviary-summer-02.png" "$O/graphics/tilesets/summer/alliance/buildings/gryphon_aviary.png"
$CP "$I/buildings/oil_platform/oil_platform_construction_site-summer-01.png" "$O/graphics/tilesets/summer/alliance/buildings/oil_well_construction_site.png"

#END ALLIANCE SUMMER BUILDINGS

$MD "$O/graphics/tilesets/summer/mythical"
$MD "$O/graphics/tilesets/summer/mythical/buildings"

#BEGIN MYTHICAL SUMMER BUILDINGS

$CP "$I/buildings/cannon_tower/cannon_tower-summer-02.png" "$O/graphics/tilesets/summer/mythical/buildings/cannon_tower.png"
$CP "$I/buildings/oil_platform/oil_platform-summer-02.png" "$O/graphics/tilesets/summer/mythical/buildings/oil_platform.png"
$CP "$I/buildings/foundry/foundry-summer-02.png" "$O/graphics/tilesets/summer/mythical/buildings/foundry.png"
$CP "$I/buildings/farm/farm-summer-07.png" "$O/graphics/tilesets/summer/mythical/buildings/farm.png"
$CP "$I/buildings/guard_tower/guard_tower-summer-02.png" "$O/graphics/tilesets/summer/mythical/buildings/guard_tower.png"
$CP "$I/buildings/altar_of_storms/altar_of_storms-summer-01.png" "$O/graphics/tilesets/summer/mythical/buildings/altar_of_storms.png"
$CP "$I/buildings/blacksmith/blacksmith-summer-06.png" "$O/graphics/tilesets/summer/mythical/buildings/blacksmith.png"
$CP "$I/buildings/scout_tower/scout_tower-summer-05.png" "$O/graphics/tilesets/summer/mythical/buildings/watch_tower.png"
$CP "$I/buildings/barracks/barracks-summer-08.png" "$O/graphics/tilesets/summer/mythical/buildings/barracks.png"
$CP "$I/buildings/refinery/refinery-summer-02.png" "$O/graphics/tilesets/summer/mythical/buildings/refinery.png"
$CP "$I/buildings/stronghold/stronghold-summer-01.png" "$O/graphics/tilesets/summer/mythical/buildings/stronghold.png"
$CP "$I/buildings/alchemist/alchemist-summer-01.png" "$O/graphics/tilesets/summer/mythical/buildings/alchemist.png"
$CP "$I/buildings/lumber_mill/lumber_mill-summer-05.png" "$O/graphics/tilesets/summer/mythical/buildings/mythical_lumber_mill.png"
$CP "$I/buildings/temple_of_the_damned/temple_of_the_damned-summer-03.png" "$O/graphics/tilesets/summer/mythical/buildings/temple_of_the_damned.png"
$CP "$I/buildings/shipyard/shipyard-summer-02.png" "$O/graphics/tilesets/summer/mythical/buildings/shipyard.png"
$CP "$I/buildings/dragon_roost/dragon_roost-summer-01.png" "$O/graphics/tilesets/summer/mythical/buildings/dragon_roost.png"
$CP "$I/buildings/great_hall/great_hall-summer-04.png" "$O/graphics/tilesets/summer/mythical/buildings/great_hall.png"
$CP "$I/buildings/fortress/fortress-summer-01.png" "$O/graphics/tilesets/summer/mythical/buildings/fortress.png"
$CP "$I/buildings/oil_platform/oil_platform_construction_site-summer-02.png" "$O/graphics/tilesets/summer/mythical/buildings/oil_well_construction_site.png"
$CP "$I/buildings/ogre_mound/ogre_mound-summer-02.png" "$O/graphics/tilesets/summer/mythical/buildings/ogre_mound.png"

#END MYTHICAL SUMMER BUILDINGS

$MD "$O/graphics/tilesets/summer/neutral"
$MD "$O/graphics/tilesets/summer/neutral/buildings"

#BEGIN NEUTRAL SUMMER BUILDINGS

$CP "$I/buildings/dark_portal/dark_portal-summer-01.png" "$O/graphics/tilesets/summer/neutral/buildings/dark_portal.png"
$CP "$I/buildings/gold_mine/gold_mine-summer-01.png" "$O/graphics/tilesets/summer/neutral/buildings/gold_mine.png"
$CP "$I/buildings/wall/wall_construction_site-summer-01.png" "$O/graphics/tilesets/summer/neutral/buildings/wall_construction_site.png"
$CP "$I/buildings/wall/wall-summer-01.png" "$O/graphics/tilesets/summer/neutral/buildings/wall.png"
$CP "$I/buildings/destroyed_site/small_destroyed_site-summer-01.png" "$O/graphics/tilesets/summer/neutral/buildings/small_destroyed_site.png"
$CP "$I/buildings/destroyed_site/big_destroyed_site-summer-01.png" "$O/graphics/tilesets/summer/neutral/buildings/destroyed_site.png"
$CP "$I/buildings/oil_patch/oil_patch-summer-02.png" "$O/graphics/tilesets/summer/neutral/buildings/oil_patch.png"

#END NEUTRAL SUMMER BUILDINGS

$MD "$O/graphics/tilesets/summer/neutral/units"

#BEGIN NEUTRAL SUMMER UNITS

$CP "$I/units/critter/sheep-02.png" "$O/graphics/tilesets/summer/neutral/units/critter.png"

#END NEUTRAL SUMMER UNITS

$MD "$O/graphics/tilesets/summer/terrain"

#BEGIN SUMMER TILESET

$CP "$I/tileset/summer/terrain/terrain-summer-01.png" "$O/graphics/tilesets/summer/terrain/summer.png"
$CP "$I/tileset/summer/icons/icons-summer-01.png" "$O/graphics/tilesets/summer/icons.png"
#$CP $O/graphics/tilesets/summer/summer.gimp
$CP "$I/tileset/summer/palette/palette-summer-01.rgb" "$O/graphics/tilesets/summer/summer.rgb"

#END SUMMER TILESET

$MD "$O/graphics/tilesets/swamp"
$MD "$O/graphics/tilesets/swamp/alliance"
$MD "$O/graphics/tilesets/swamp/alliance/buildings"

#BEGIN ALLIANCE SWAMP BUILDINGS

$CP "$I/buildings/cannon_tower/cannon_tower-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/cannon_tower.png"
$CP "$I/buildings/oil_platform/oil_platform-summer-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/oil_platform.png"
$CP "$I/buildings/foundry/foundry-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/foundry.png"
$CP "$I/buildings/foundry/foundry_construction_site-summer-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/foundry_construction_site.png"
$CP "$I/buildings/guard_tower/guard_tower-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/guard_tower.png"
$CP "$I/buildings/scout_tower/scout_tower-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/scout_tower.png"
$CP "$I/buildings/stables/stables-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/stables.png"
$CP "$I/buildings/farm/farm-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/farm.png"
$CP "$I/buildings/blacksmith/blacksmith-summer-04.png" "$O/graphics/tilesets/swamp/alliance/buildings/blacksmith.png"
$CP "$I/buildings/barracks/barracks-summer-05.png" "$O/graphics/tilesets/swamp/alliance/buildings/barracks.png"
$CP "$I/buildings/church/church-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/church.png"
$CP "$I/buildings/lumber_mill/lumber_mill-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/alliance_lumber_mill.png"
$CP "$I/buildings/refinery/refinery-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/refinery.png"
$CP "$I/buildings/inventor/inventor-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/inventor.png"
$CP "$I/buildings/mage_tower/mage_tower-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/mage_tower.png"
$CP "$I/buildings/shipyard/shipyard_construction_site-summer-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/shipyard_construction_site.png"
$CP "$I/buildings/castle/castle-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/castle.png"
$CP "$I/buildings/shipyard/shipyard-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/shipyard.png"
$CP "$I/buildings/keep/keep-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/keep.png"
$CP "$I/buildings/town_hall/town_hall-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/town_hall.png"
$CP "$I/buildings/refinery/refinery_construction_site-summer-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/refinery_construction_site.png"
$CP "$I/buildings/oil_platform/oil_platform_construction_site-summer-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/oil_platform_construction_site.png"
$CP "$I/buildings/gryphon_aviary/gryphon_aviary-swamp-01.png" "$O/graphics/tilesets/swamp/alliance/buildings/gryphon_aviary.png"

#END ALLIANCE SWAMP BUILDINGS

$MD "$O/graphics/tilesets/swamp/alliance/units"

#BEGIN ALLIANCE SWAMP UNITS

$CP "$I/units/submarine/submarine-01.png" "$O/graphics/tilesets/swamp/alliance/units/submarine.png"

#END ALLIANCE SWAMP UNITS

$MD "$O/graphics/tilesets/swamp/mythical"
$MD "$O/graphics/tilesets/swamp/mythical/buildings"

#BEGIN MYTHICAL SWAMP BUILDINGS

$CP "$I/buildings/cannon_tower/cannon_tower-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/cannon_tower.png"
$CP "$I/buildings/oil_platform/oil_platform-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/oil_platform.png"
$CP "$I/buildings/foundry/foundry-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/foundry.png"
$CP "$I/buildings/foundry/foundry_construction_site-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/foundry_construction_site.png"
$CP "$I/buildings/farm/farm-summer-07.png" "$O/graphics/tilesets/swamp/mythical/buildings/farm.png"
$CP "$I/buildings/guard_tower/guard_tower-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/guard_tower.png"
$CP "$I/buildings/altar_of_storms/altar_of_storms-summer-01.png" "$O/graphics/tilesets/swamp/mythical/buildings/altar_of_storms.png"
$CP "$I/buildings/blacksmith/blacksmith-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/blacksmith.png"
$CP "$I/buildings/scout_tower/scout_tower-summer-05.png" "$O/graphics/tilesets/swamp/mythical/buildings/watch_tower.png"
$CP "$I/buildings/barracks/barracks-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/barracks.png"
$CP "$I/buildings/refinery/refinery-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/refinery.png"
$CP "$I/buildings/stronghold/stronghold-summer-01.png" "$O/graphics/tilesets/swamp/mythical/buildings/stronghold.png"
$CP "$I/buildings/alchemist/alchemist-summer-01.png" "$O/graphics/tilesets/swamp/mythical/buildings/alchemist.png"
$CP "$I/buildings/lumber_mill/lumber_mill-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/mythical_lumber_mill.png"
$CP "$I/buildings/shipyard/shipyard_construction_site-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/shipyard_construction_site.png"
$CP "$I/buildings/temple_of_the_damned/temple_of_the_damned-summer-01.png" "$O/graphics/tilesets/swamp/mythical/buildings/temple_of_the_damned.png"
$CP "$I/buildings/shipyard/shipyard-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/shipyard.png"
$CP "$I/buildings/dragon_roost/dragon_roost-summer-01.png" "$O/graphics/tilesets/swamp/mythical/buildings/dragon_roost.png"
$CP "$I/buildings/great_hall/great_hall-summer-01.png" "$O/graphics/tilesets/swamp/mythical/buildings/great_hall.png"
$CP "$I/buildings/fortress/fortress-summer-01.png" "$O/graphics/tilesets/swamp/mythical/buildings/fortress.png"
$CP "$I/buildings/refinery/refinery_construction_site-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/refinery_construction_site.png"
$CP "$I/buildings/oil_platform/oil_platform_construction_site-summer-02.png" "$O/graphics/tilesets/swamp/mythical/buildings/oil_platform_construction_site.png"
$CP "$I/buildings/ogre_mound/ogre_mound-summer-01.png" "$O/graphics/tilesets/swamp/mythical/buildings/ogre_mound.png"

#END MYTHICAL SWAMP BUILDINGS

$MD "$O/graphics/tilesets/swamp/mythical/units"

#BEGIN MYTHICAL SWAMP UNITS

$CP "$I/units/submarine/submarine-02.png" "$O/graphics/tilesets/swamp/mythical/units/submarine.png"

#END MYTHICAL SWAMP UNITS

$MD "$O/graphics/tilesets/swamp/neutral"
$MD "$O/graphics/tilesets/swamp/neutral/buildings"

#BEGIN NEUTRAL SWAMP BUILDINGS

$CP "$I/buildings/dark_portal/dark_portal-summer-01.png" "$O/graphics/tilesets/swamp/neutral/buildings/dark_portal.png"
$CP "$I/buildings/circle_of_power/circle_of_power-summer-01.png" "$O/graphics/tilesets/swamp/neutral/buildings/circle_of_power.png"
$CP "$I/buildings/runestone/runestone-summer-01.png" "$O/graphics/tilesets/swamp/neutral/buildings/runestone.png"
$CP "$I/buildings/gold_mine/gold_mine-swamp-01.png" "$O/graphics/tilesets/swamp/neutral/buildings/gold_mine.png"
$CP "$I/buildings/destroyed_site/small_destroyed_site-summer-01.png" "$O/graphics/tilesets/swamp/neutral/buildings/small_destroyed_site.png"
$CP "$I/buildings/destroyed_site/big_destroyed_site-summer-01.png" "$O/graphics/tilesets/swamp/neutral/buildings/destroyed_site.png"
$CP "$I/buildings/oil_patch/oil_patch-summer-02.png" "$O/graphics/tilesets/swamp/neutral/buildings/oil_patch.png"

#END NEUTRAL SWAMP BUILDINGS

$MD "$O/graphics/tilesets/swamp/neutral/units"

#BEGIN NEUTRAL SWAMP UNITS

$CP "$I/units/critter/sheep-02.png" "$O/graphics/tilesets/swamp/neutral/units/critter.png"

#END NEUTRAL SWAMP UNITS

$MD "$O/graphics/tilesets/swamp/terrain"

#BEGIN SWAMP TILESET

$CP "$I/tileset/swamp/terrain/terrain-swamp-01.png" "$O/graphics/tilesets/swamp/terrain/swamp.png"
$CP "$I/tileset/swamp/icons/icons-swamp-01.png" "$O/graphics/tilesets/swamp/icons.png"
#$CP $O/graphics/tilesets/swamp/swamp.gimp
$CP "$I/tileset/swamp/palette/palette-swamp-01.rgb" "$O/graphics/tilesets/swamp/swamp.rgb"

#END SWAMP TILESET

$MD "$O/graphics/tilesets/wasteland"
$MD "$O/graphics/tilesets/wasteland/alliance"
$MD "$O/graphics/tilesets/wasteland/alliance/buildings"

#BEGIN ALLIANCE WASTELAND BUILDINGS

$CP "$I/buildings/oil_platform/oil_platform-summer-01.png" "$O/graphics/tilesets/wasteland/alliance/buildings/oil_platform.png"
$CP "$I/buildings/farm/farm-summer-01.png" "$O/graphics/tilesets/wasteland/alliance/buildings/farm.png"
$CP "$I/buildings/lumber_mill/lumber_mill-summer-01.png" "$O/graphics/tilesets/wasteland/alliance/buildings/alliance_lumber_mill.png"
$CP "$I/buildings/oil_platform/oil_platform_construction_site-summer-01.png" "$O/graphics/tilesets/wasteland/alliance/buildings/oil_well_construction_site.png"

#END ALLIANCE WASTELAND BUILDINGS

$MD "$O/graphics/tilesets/wasteland/alliance/units"

#BEGIN ALLIANCE WASTELAND UNITS

$CP "$I/units/submarine/submarine-01.png" "$O/graphics/tilesets/wasteland/alliance/units/submarine.png"

#END ALLIANCE WASTELAND UNITS

$MD "$O/graphics/tilesets/wasteland/mythical"
$MD "$O/graphics/tilesets/wasteland/mythical/buildings"

#BEGIN MYTHICAL WASTELAND BUILDINGS

$CP "$I/buildings/oil_platform/oil_platform-summer-02.png" "$O/graphics/tilesets/wasteland/mythical/buildings/oil_platform.png"
$CP "$I/buildings/farm/farm-summer-03.png" "$O/graphics/tilesets/wasteland/mythical/buildings/farm.png"
$CP "$I/buildings/lumber_mill/lumber_mill-summer-02.png" "$O/graphics/tilesets/wasteland/mythical/buildings/mythical_lumber_mill.png"
$CP "$I/buildings/oil_platform/oil_platform_construction_site-summer-02.png" "$O/graphics/tilesets/wasteland/mythical/buildings/oil_well_construction_site.png"

#END MYTHICAL WASTELAND BUILDINGS

$MD "$O/graphics/tilesets/wasteland/mythical/units"

#BEGIN MYTHICAL WASTELAND UNITS

$CP "$I/units/submarine/submarine-02.png" "$O/graphics/tilesets/wasteland/mythical/units/submarine.png"

#END MYTHICAL WASTELAND UNITS

$MD "$O/graphics/tilesets/wasteland/neutral"
$MD "$O/graphics/tilesets/wasteland/neutral/buildings"

#BEGIN NEUTRAL WASTELAND BUILDINGS

$CP "$I/buildings/dark_portal/dark_portal-summer-01.png" "$O/graphics/tilesets/wasteland/neutral/buildings/dark_portal.png"
$CP "$I/buildings/gold_mine/gold_mine-summer-01.png" "$O/graphics/tilesets/wasteland/neutral/buildings/gold_mine.png"
$CP "$I/buildings/wall/wall_construction_site-summer-01.png" "$O/graphics/tilesets/wasteland/neutral/buildings/wall_construction_site.png"
$CP "$I/buildings/wall/wall-summer-01.png" "$O/graphics/tilesets/wasteland/neutral/buildings/wall.png"
$CP "$I/buildings/destroyed_site/small_destroyed_site-summer-01.png" "$O/graphics/tilesets/wasteland/neutral/buildings/small_destroyed_site.png"
$CP "$I/buildings/destroyed_site/big_destroyed_site-summer-01.png" "$O/graphics/tilesets/wasteland/neutral/buildings/destroyed_site.png"
$CP "$I/buildings/oil_patch/oil_patch-summer-02.png" "$O/graphics/tilesets/wasteland/neutral/buildings/oil_patch.png"

#END NEUTRAL WASTELAND BUILDINGS

$MD "$O/graphics/tilesets/wasteland/neutral/units"

#BEGIN NEUTRAL WASTELAND UNITS

$CP "$I/units/critter/sheep-02.png" "$O/graphics/tilesets/wasteland/neutral/units/critter.png"

#END NEUTRAL WASTELAND UNITS

$MD "$O/graphics/tilesets/wasteland/terrain"

#BEGIN WASTELAND TILESET

$CP "$I/tileset/wasteland/terrain/terrain-wasteland-01.png" "$O/graphics/tilesets/wasteland/terrain/wasteland.png"
#$CP $O/graphics/tilesets/wasteland/wasteland.gimp
$CP "$I/tileset/wasteland/palette/palette-wasteland-01.rgb" "$O/graphics/tilesets/wasteland/wasteland.rgb"
$CP "$I/tileset/wasteland/icons/icons-wasteland-01.png" "$O/graphics/tilesets/wasteland/icons.png"

#END WASTELAND TILESET

$MD "$O/graphics/tilesets/winter"
$MD "$O/graphics/tilesets/winter/alliance"
$MD "$O/graphics/tilesets/winter/alliance/buildings"

#BEGIN ALLIANCE WINTER BUILDINGS

$CP "$I/buildings/cannon_tower/cannon_tower-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/cannon_tower.png"
$CP "$I/buildings/oil_platform/oil_platform-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/oil_platform.png"
$CP "$I/buildings/foundry/foundry-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/foundry.png"
$CP "$I/buildings/foundry/foundry_construction_site-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/foundry_construction_site.png"
$CP "$I/buildings/guard_tower/guard_tower-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/guard_tower.png"
$CP "$I/buildings/scout_tower/scout_tower-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/scout_tower.png"
$CP "$I/buildings/stables/stables-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/stables.png"
$CP "$I/buildings/farm/farm-winter-01.png" "$O/graphics/tilesets/winter/alliance/buildings/farm.png"
$CP "$I/buildings/blacksmith/blacksmith-summer-04.png" "$O/graphics/tilesets/winter/alliance/buildings/blacksmith.png"
$CP "$I/buildings/barracks/barracks-summer-05.png" "$O/graphics/tilesets/winter/alliance/buildings/barracks.png"
$CP "$I/buildings/church/church-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/church.png"
$CP "$I/buildings/lumber_mill/lumber_mill-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/alliance_lumber_mill.png"
$CP "$I/buildings/refinery/refinery-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/refinery.png"
$CP "$I/buildings/inventor/inventor-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/inventor.png"
$CP "$I/buildings/mage_tower/mage_tower-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/mage_tower.png"
$CP "$I/buildings/shipyard/shipyard_construction_site-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/shipyard_construction_site.png"
$CP "$I/buildings/castle/castle-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/castle.png"
$CP "$I/buildings/shipyard/shipyard-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/shipyard.png"
$CP "$I/buildings/keep/keep-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/keep.png"
$CP "$I/buildings/town_hall/town_hall-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/town_hall.png"
$CP "$I/buildings/refinery/refinery_construction_site-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/refinery_construction_site.png"
$CP "$I/buildings/gryphon_aviary/gryphon_aviary-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/gryphon_aviary.png"
$CP "$I/buildings/oil_platform/oil_platform_construction_site-summer-01.png" "$O/graphics/tilesets/winter/alliance/buildings/oil_well_construction_site.png"

#END ALLIANCE WINTER BUILDINGS

$MD "$O/graphics/tilesets/winter/mythical"
$MD "$O/graphics/tilesets/winter/mythical/buildings"

#BEGIN MYTHICAL WINTER BUILDINGS

$CP "$I/buildings/cannon_tower/cannon_tower-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/cannon_tower.png"
$CP "$I/buildings/oil_platform/oil_platform-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/oil_platform.png"
$CP "$I/buildings/foundry/foundry-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/foundry.png"
$CP "$I/buildings/foundry/foundry_construction_site-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/foundry_construction_site.png"
$CP "$I/buildings/farm/farm-summer-03.png" "$O/graphics/tilesets/winter/mythical/buildings/farm.png"
$CP "$I/buildings/guard_tower/guard_tower-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/guard_tower.png"
$CP "$I/buildings/altar_of_storms/altar_of_storms-summer-01.png" "$O/graphics/tilesets/winter/mythical/buildings/altar_of_storms.png"
$CP "$I/buildings/blacksmith/blacksmith-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/blacksmith.png"
$CP "$I/buildings/scout_tower/scout_tower-summer-05.png" "$O/graphics/tilesets/winter/mythical/buildings/watch_tower.png"
$CP "$I/buildings/barracks/barracks-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/barracks.png"
$CP "$I/buildings/refinery/refinery-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/refinery.png"
$CP "$I/buildings/stronghold/stronghold-summer-01.png" "$O/graphics/tilesets/winter/mythical/buildings/stronghold.png"
$CP "$I/buildings/alchemist/alchemist-summer-01.png" "$O/graphics/tilesets/winter/mythical/buildings/alchemist.png"
$CP "$I/buildings/lumber_mill/lumber_mill-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/mythical_lumber_mill.png"
$CP "$I/buildings/shipyard/shipyard_construction_site-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/shipyard_construction_site.png"
$CP "$I/buildings/temple_of_the_damned/temple_of_the_damned-summer-01.png" "$O/graphics/tilesets/winter/mythical/buildings/temple_of_the_damned.png"
$CP "$I/buildings/shipyard/shipyard-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/shipyard.png"
$CP "$I/buildings/dragon_roost/dragon_roost-summer-01.png" "$O/graphics/tilesets/winter/mythical/buildings/dragon_roost.png"
$CP "$I/buildings/great_hall/great_hall-summer-01.png" "$O/graphics/tilesets/winter/mythical/buildings/great_hall.png"
$CP "$I/buildings/refinery/refinery_construction_site-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/refinery_construction_site.png"
$CP "$I/buildings/fortress/fortress-summer-01.png" "$O/graphics/tilesets/winter/mythical/buildings/fortress.png"
$CP "$I/buildings/oil_platform/oil_platform_construction_site-summer-02.png" "$O/graphics/tilesets/winter/mythical/buildings/oil_well_construction_site.png"
$CP "$I/buildings/ogre_mound/ogre_mound-summer-01.png" "$O/graphics/tilesets/winter/mythical/buildings/ogre_mound.png"

#END MYTHICAL WINTER BUILDINGS

$MD "$O/graphics/tilesets/winter/neutral"
$MD "$O/graphics/tilesets/winter/neutral/buildings"

#BEGIN NEUTRAL WINTER BUILDINGS

$CP "$I/buildings/dark_portal/dark_portal-summer-01.png" "$O/graphics/tilesets/winter/neutral/buildings/dark_portal.png"
$CP "$I/buildings/gold_mine/gold_mine-winter-01.png" "$O/graphics/tilesets/winter/neutral/buildings/gold_mine.png"
$CP "$I/buildings/wall/wall_construction_site-summer-01.png" "$O/graphics/tilesets/winter/neutral/buildings/wall_construction_site.png"
$CP "$I/buildings/wall/wall-summer-01.png" "$O/graphics/tilesets/winter/neutral/buildings/wall.png"
$CP "$I/buildings/destroyed_site/small_destroyed_site-summer-01.png" "$O/graphics/tilesets/winter/neutral/buildings/small_destroyed_site.png"
$CP "$I/buildings/runestone/runestone-summer-01.png" "$O/graphics/tilesets/winter/neutral/buildings/runestone.png"
$CP "$I/buildings/destroyed_site/big_destroyed_site-summer-01.png" "$O/graphics/tilesets/winter/neutral/buildings/destroyed_site.png"
$CP "$I/buildings/land_construction_site/land_construction_site-summer-01.png" "$O/graphics/tilesets/winter/neutral/buildings/land_construction_site.png"

#END NEUTRAL WINTER BUILDINGS

$MD "$O/graphics/tilesets/winter/neutral/units"

#BEGIN NEUTRAL WINTER UNITS

$CP "$I/units/critter/tux-01.png" "$O/graphics/tilesets/winter/neutral/units/critter.png"

#END NEUTRAL WINTER UNITS

$MD "$O/graphics/tilesets/winter/terrain"

#BEGIN WINTER TILESET

$CP "$I/tileset/winter/icons/icons-winter-01.png" "$O/graphics/tilesets/winter/icons.png"
$CP "$I/tileset/winter/terrain/terrain-winter-01.png" "$O/graphics/tilesets/winter/terrain/winter.png"
#$CP $O/graphics/tilesets/winter/winter.gimp
$CP "$I/tileset/winter/palette/palette-winter-01.rgb" "$O/graphics/tilesets/winter/winter.rgb"

#END WINTER TILESET

$MD "$O/graphics/ui"
$MD "$O/graphics/ui/alliance"

#BEGIN ALLIANCE UI

$CP "$I/ui/backgrounds/menu_background_with_title-01.png" "$O/graphics/ui/alliance/defeat.png"
$CP "$I/ui/backgrounds/menu_background_with_title-01.png" "$O/graphics/ui/alliance/victory.png"
$CP "$I/ui/panels/infopanel-03.png" "$O/graphics/ui/alliance/infopanel.png"
$CP "$I/ui/panels/menubutton-03.png" "$O/graphics/ui/alliance/menubutton.png"
$CP "$I/ui/panels/minimap-03.png" "$O/graphics/ui/alliance/minimap.png"
$CP "$I/ui/panels/panel_1-01.png" "$O/graphics/ui/alliance/panel_1.png"
$CP "$I/ui/panels/panel_2-01.png" "$O/graphics/ui/alliance/panel_2.png"
$CP "$I/ui/panels/panel_3-01.png" "$O/graphics/ui/alliance/panel_3.png"
$CP "$I/ui/panels/panel_4-01.png" "$O/graphics/ui/alliance/panel_4.png"
$CP "$I/ui/panels/panel_5-01.png" "$O/graphics/ui/alliance/panel_5.png"
#$CP "$I/ui" "$O/graphics/ui/alliance/Smashing_of_Lordaeron_scroll.png"

$MD "$O/graphics/ui/alliance/1024x768"
$CP "$I/ui/1024x768/resource-01.png" "$O/graphics/ui/alliance/1024x768/resource.png"
$CP "$I/ui/1024x768/filler-01.png" "$O/graphics/ui/alliance/1024x768/filler1.png"
$CP "$I/ui/1024x768/statusline-01.png" "$O/graphics/ui/alliance/1024x768/statusline.png"
$CP "$I/ui/panels/buttonpanel-01.png" "$O/graphics/ui/alliance/1024x768/buttonpanel.png"

$MD "$O/graphics/ui/alliance/1280x960"
$CP "$I/ui/1280x960/resource-01.png" "$O/graphics/ui/alliance/1280x960/resource.png"
$CP "$I/ui/1280x960/filler-01.png" "$O/graphics/ui/alliance/1280x960/filler1.png"
$CP "$I/ui/1280x960/statusline-01.png" "$O/graphics/ui/alliance/1280x960/statusline.png"
$CP "$I/ui/panels/buttonpanel-01.png" "$O/graphics/ui/alliance/1280x960/buttonpanel.png"

$MD "$O/graphics/ui/alliance/1600x1200"
$CP "$I/ui/1600x1200/resource-01.png" "$O/graphics/ui/alliance/1600x1200/resource.png"
$CP "$I/ui/1600x1200/filler-01.png" "$O/graphics/ui/alliance/1600x1200/filler1.png"
$CP "$I/ui/1600x1200/statusline-01.png" "$O/graphics/ui/alliance/1600x1200/statusline.png"
$CP "$I/ui/panels/buttonpanel-01.png" "$O/graphics/ui/alliance/1600x1200/buttonpanel.png"

$MD "$O/graphics/ui/alliance/800x600"
$CP "$I/ui/800x600/resource-01.png" "$O/graphics/ui/alliance/800x600/resource.png"
$CP "$I/ui/800x600/filler-01.png" "$O/graphics/ui/alliance/800x600/filler1.png"
$CP "$I/ui/800x600/statusline-01.png" "$O/graphics/ui/alliance/800x600/statusline.png"
$CP "$I/ui/panels/buttonpanel-01.png" "$O/graphics/ui/alliance/800x600/buttonpanel.png"

$MD "$O/graphics/ui/alliance/640x480"
$CP "$I/ui/640x480/resource-03.png" "$O/graphics/ui/alliance/640x480/resource.png"
$CP "$I/ui/640x480/filler_right-03.png" "$O/graphics/ui/alliance/640x480/filler1.png"
$CP "$I/ui/640x480/statusline-03.png" "$O/graphics/ui/alliance/640x480/statusline.png"
$CP "$I/ui/panels/buttonpanel-03.png" "$O/graphics/ui/alliance/640x480/buttonpanel.png"

$MD "$O/graphics/ui/alliance/cursors"
$CP "$I/ui/cursors/green_eagle-01.png" "$O/graphics/ui/alliance/cursors/green_eagle.png"
$CP "$I/ui/cursors/gauntlet-01.png" "$O/graphics/ui/alliance/cursors/alliance_claw.png"
$CP "$I/ui/cursors/red_eagle-01.png" "$O/graphics/ui/alliance/cursors/red_eagle.png"
$CP "$I/ui/cursors/yellow_eagle-01.png" "$O/graphics/ui/alliance/cursors/yellow_eagle.png"
$CP "$I/ui/cursors/don't_click_here-01.png" "$O/graphics/ui/alliance/cursors/alliance_don't_click_here.png"

#END ALLIANCE UI

$MD "$O/graphics/ui/cursors"

#BEGIN CURSORS

$CP "$I/ui/cursors/gauntlet-01.png" "$O/graphics/ui/cursors/credits_arrow.png"
#$CP "$I/ui/cursors/cross-01.png" "$O/graphics/ui/cursors/cross.png"
$CP "$I/ui/cursors/magnifying_glass-01.png" "$O/graphics/ui/cursors/magnifying_glass.png"
$CP "$I/ui/cursors/hourglass-01.png" "$O/graphics/ui/cursors/hourglass.png"
$CP "$I/ui/cursors/arrow_E-01.png" "$O/graphics/ui/cursors/arrow_E.png"
$CP "$I/ui/cursors/arrow_N-01.png" "$O/graphics/ui/cursors/arrow_N.png"
$CP "$I/ui/cursors/arrow_S-01.png" "$O/graphics/ui/cursors/arrow_S.png"
$CP "$I/ui/cursors/arrow_W-01.png" "$O/graphics/ui/cursors/arrow_W.png"
$CP "$I/ui/cursors/arrow_NE-01.png" "$O/graphics/ui/cursors/arrow_NE.png"
$CP "$I/ui/cursors/arrow_NW-01.png" "$O/graphics/ui/cursors/arrow_NW.png"
$CP "$I/ui/cursors/arrow_SE-01.png" "$O/graphics/ui/cursors/arrow_SE.png"
$CP "$I/ui/cursors/small_green_cross-01.png" "$O/graphics/ui/cursors/small_green_cross.png"
$CP "$I/ui/cursors/arrow_SW-01.png" "$O/graphics/ui/cursors/arrow_SW.png"

#END CURSORS

$MD "$O/graphics/ui/fonts"

#BEGIN FONTS

#$CP "$I/ui" "$O/graphics/ui/fonts/small_episode_titles.png"
#$CP "$I/ui" "$O/graphics/ui/fonts/large_episode_titles.png"
$CP "$I/ui/fonts/small-01.png" "$O/graphics/ui/fonts/small.png"
$CP "$I/ui/fonts/large-01.png" "$O/graphics/ui/fonts/large.png"
$CP "$I/ui/fonts/game-03.png" "$O/graphics/ui/fonts/game.png"

#END FONTS

$MD "$O/graphics/ui/mythical"

#BEGIN MYTHICAL UI

$CP "$I/ui/backgrounds/menu_background_with_title-01.png" "$O/graphics/ui/mythical/defeat.png"
$CP "$I/ui/backgrounds/menu_background_with_title-01.png" "$O/graphics/ui/mythical/victory.png"
$CP "$I/ui/panels/infopanel-04.png" "$O/graphics/ui/mythical/infopanel.png"
$CP "$I/ui/panels/menubutton-04.png" "$O/graphics/ui/mythical/menubutton.png"
$CP "$I/ui/panels/minimap-04.png" "$O/graphics/ui/mythical/minimap.png"
$CP "$I/ui/panels/panel_1-02.png" "$O/graphics/ui/mythical/panel_1.png"
$CP "$I/ui/panels/panel_2-02.png" "$O/graphics/ui/mythical/panel_2.png"
$CP "$I/ui/panels/panel_3-02.png" "$O/graphics/ui/mythical/panel_3.png"
$CP "$I/ui/panels/panel_4-02.png" "$O/graphics/ui/mythical/panel_4.png"
$CP "$I/ui/panels/panel_5-02.png" "$O/graphics/ui/mythical/panel_5.png"
#$CP "ui" "$O/graphics/ui/mythical/Smashing_of_Lordaeron_scroll.png"

$MD "$O/graphics/ui/mythical/1024x768"
$CP "$I/ui/1024x768/resource-02.png" "$O/graphics/ui/mythical/1024x768/resource.png"
$CP "$I/ui/1024x768/filler-02.png" "$O/graphics/ui/mythical/1024x768/filler1.png"
$CP "$I/ui/1024x768/statusline-02.png" "$O/graphics/ui/mythical/1024x768/statusline.png"
$CP "$I/ui/panels/buttonpanel-02.png" "$O/graphics/ui/mythical/1024x768/buttonpanel.png"

$MD "$O/graphics/ui/mythical/1280x960"
$CP "$I/ui/1280x960/resource-02.png" "$O/graphics/ui/mythical/1280x960/resource.png"
$CP "$I/ui/1280x960/filler-02.png" "$O/graphics/ui/mythical/1280x960/filler1.png"
$CP "$I/ui/1280x960/statusline-02.png" "$O/graphics/ui/mythical/1280x960/statusline.png"
$CP "$I/ui/panels/buttonpanel-02.png" "$O/graphics/ui/mythical/1280x960/buttonpanel.png"

$MD "$O/graphics/ui/mythical/1600x1200"
$CP "$I/ui/1600x1200/resource-02.png" "$O/graphics/ui/mythical/1600x1200/resource.png"
$CP "$I/ui/1600x1200/filler-02.png" "$O/graphics/ui/mythical/1600x1200/filler1.png"
$CP "$I/ui/1600x1200/statusline-02.png" "$O/graphics/ui/mythical/1600x1200/statusline.png"
$CP "$I/ui/panels/buttonpanel-02.png" "$O/graphics/ui/mythical/1600x1200/buttonpanel.png"

$MD "$O/graphics/ui/mythical/800x600"
$CP "$I/ui/800x600/resource-02.png" "$O/graphics/ui/mythical/800x600/resource.png"
$CP "$I/ui/800x600/filler-02.png" "$O/graphics/ui/mythical/800x600/filler1.png"
$CP "$I/ui/800x600/statusline-02.png" "$O/graphics/ui/mythical/800x600/statusline.png"
$CP "$I/ui/panels/buttonpanel-02.png" "$O/graphics/ui/mythical/800x600/buttonpanel.png"

$MD "$O/graphics/ui/mythical/640x480"
$CP "$I/ui/640x480/resource-04.png" "$O/graphics/ui/mythical/640x480/resource.png"
$CP "$I/ui/640x480/filler_right-04.png" "$O/graphics/ui/mythical/640x480/filler1.png"
$CP "$I/ui/640x480/statusline-04.png" "$O/graphics/ui/mythical/640x480/statusline.png"
$CP "$I/ui/panels/buttonpanel-04.png" "$O/graphics/ui/mythical/640x480/buttonpanel.png"

$MD "$O/graphics/ui/mythical/cursors"
$CP "$I/ui/cursors/green_crosshairs-01.png" "$O/graphics/ui/mythical/cursors/green_crosshairs.png"
$CP "$I/ui/cursors/claw-01.png" "$O/graphics/ui/mythical/cursors/mythical_claw.png"
$CP "$I/ui/cursors/red_crosshairs-01.png" "$O/graphics/ui/mythical/cursors/red_crosshairs.png"
$CP "$I/ui/cursors/yellow_crosshairs-01.png" "$O/graphics/ui/mythical/cursors/yellow_crosshairs.png"
$CP "$I/ui/cursors/don't_click_here-01.png" "$O/graphics/ui/mythical/cursors/mythical_don't_click_here.png"

#END MYTHICAL UI

#BEGIN MISC UI

#$CP "$I/ui/misc/health-01.png" "$O/graphics/ui/health.png"
#$CP "$I/ui/misc/health-02.png" "$O/graphics/ui/health2.png"
#$CP "$I/ui/misc/food-01.png" "$O/graphics/ui/food.png"
$CP "$I/ui/misc/gold_wood_oil_mana-01.png" "$O/graphics/ui/gold,wood,oil,mana.png"
$CP "$I/ui/backgrounds/stratagus-01.png" "$O/graphics/ui/stratagus.png"
$CP "$I/ui/misc/buttons-01.png" "$O/graphics/ui/buttons_1.png"
$CP "$I/ui/misc/buttons-02.png" "$O/graphics/ui/buttons_2.png"
$CP "$I/ui/misc/titlescreen-01.png" "$O/graphics/ui/title.png"
#$CP "$I/ui/misc/mana-01.png" "$O/graphics/ui/mana.png"
#$CP "$I/ui/misc/mana-02.png" "$O/graphics/ui/mana2.png"
#$CP "$I/ui/misc/score-01.png" "$O/graphics/ui/score.png"
$CP "$I/ui/backgrounds/menu_background_with_title-01.png" "$O/graphics/ui/Menu_background_with_title.png"
$CP "$I/ui/misc/cd_icon-01.png" "$O/graphics/ui/cd-icon.png"
$CP "$I/ui/backgrounds/credits_background-01.png" "$O/graphics/ui/Credits_background.png"
$CP "$I/ui/backgrounds/menu_background_without_title-01.png" "$O/graphics/ui/Menu_background_without_title.png"
#$CP "$I/ui" "$O/graphics/ui/Patch.png"
$CP "$I/ui/backgrounds/credits_background-02.png" "$O/graphics/ui/Credits_for_extension_background.png"
#$CP "$I/ui/misc/ore_stone_coal-01.png" "$O/graphics/ui/ore,stone,coal.png"
$CP "$I/ui/misc/bloodlust_haste_slow_invisible_shield-01.png" "$O/graphics/ui/bloodlust,haste,slow,invisible,shield.png"

#END MISC UI

#music
$MD "$O/music"

#maps
$MD "$O/maps"
$MD "$O/maps/single"
$MD "$O/maps/multiple"
$MD "$O/maps/my_puds"

#sounds
$MD "$O/sounds"

$MD "$O/sounds/alliance"

#BEGIN MISC ALLIANCE SOUNDS

$CP "$I/misc/capture-01.wav" "$O/sounds/alliance/capture.wav"
$CP "$I/misc/rescue-01.wav" "$O/sounds/alliance/rescue.wav"

#END MISC ALLIANCE SOUNDS

$MD "$O/sounds/alliance/basic_voices"

#BEGIN ALLIANCE BASIC VOICES

$MD "$O/sounds/alliance/basic_voices/acknowledgement"
$CP "$I/units/footman/acknowledgement-01.wav" "$O/sounds/alliance/basic_voices/acknowledgement/1.wav"
$CP "$I/units/footman/acknowledgement-02.wav" "$O/sounds/alliance/basic_voices/acknowledgement/2.wav"
$CP "$I/units/footman/acknowledgement-03.wav" "$O/sounds/alliance/basic_voices/acknowledgement/3.wav"
$CP "$I/units/footman/acknowledgement-04.wav" "$O/sounds/alliance/basic_voices/acknowledgement/4.wav"
$MD "$O/sounds/alliance/basic_voices/annoyed"
$CP "$I/units/footman/annoyed-01.wav" "$O/sounds/alliance/basic_voices/annoyed/1.wav"
$CP "$I/units/footman/annoyed-02.wav" "$O/sounds/alliance/basic_voices/annoyed/2.wav"
$CP "$I/units/footman/annoyed-03.wav" "$O/sounds/alliance/basic_voices/annoyed/3.wav"
$CP "$I/units/footman/annoyed-04.wav" "$O/sounds/alliance/basic_voices/annoyed/4.wav"
$CP "$I/units/footman/annoyed-05.wav" "$O/sounds/alliance/basic_voices/annoyed/5.wav"
$CP "$I/units/footman/annoyed-06.wav" "$O/sounds/alliance/basic_voices/annoyed/6.wav"
$CP "$I/units/footman/annoyed-07.wav" "$O/sounds/alliance/basic_voices/annoyed/7.wav"
$MD "$O/sounds/alliance/basic_voices/help"
$CP "$I/units/common/help_1-01.wav" "$O/sounds/alliance/basic_voices/help/1.wav"
$CP "$I/units/common/help_2-01.wav" "$O/sounds/alliance/basic_voices/help/2.wav"
$MD "$O/sounds/alliance/basic_voices/selected"
$CP "$I/units/footman/selected-01.wav" "$O/sounds/alliance/basic_voices/selected/1.wav"
$CP "$I/units/footman/selected-02.wav" "$O/sounds/alliance/basic_voices/selected/2.wav"
$CP "$I/units/footman/selected-03.wav" "$O/sounds/alliance/basic_voices/selected/3.wav"
$CP "$I/units/footman/selected-04.wav" "$O/sounds/alliance/basic_voices/selected/4.wav"
$CP "$I/units/footman/selected-05.wav" "$O/sounds/alliance/basic_voices/selected/5.wav"
$CP "$I/units/footman/selected-06.wav" "$O/sounds/alliance/basic_voices/selected/6.wav"
$CP "$I/units/common/dead-01.wav" "$O/sounds/alliance/basic_voices/dead.wav"
$CP "$I/units/common/ready-01.wav" "$O/sounds/alliance/basic_voices/ready.wav"
$CP "$I/units/common/work_complete-01.wav" "$O/sounds/alliance/basic_voices/work_complete.wav"

#END ALLIANCE BASIC VOICES

$MD "$O/sounds/alliance/buildings"

#BEGIN ALLIANCE BUILDING SOUNDS

$CP "$I/buildings/church/church-01.wav" "$O/sounds/alliance/buildings/church.wav"
$CP "$I/buildings/farm/farm-01.wav" "$O/sounds/alliance/buildings/farm.wav"
$CP "$I/buildings/gryphon_aviary/gryphon_aviary-01.wav" "$O/sounds/alliance/buildings/gryphon_aviary.wav"
$CP "$I/buildings/inventor/inventor-01.wav" "$O/sounds/alliance/buildings/inventor.wav"
$CP "$I/buildings/mage_tower/mage_tower-01.wav" "$O/sounds/alliance/buildings/mage_tower.wav"
$CP "$I/buildings/stables/stable-01.wav" "$O/sounds/alliance/buildings/stables.wav"

#END ALLIANCE BUILDING SOUNDS

$MD "$O/sounds/alliance/ships"

#BEGIN ALLIANCE SHIPS SOUNDS

$CP "$I/units/ships/ready-01.wav" "$O/sounds/alliance/ships/ready.wav"
$MD "$O/sounds/alliance/ships/acknowledgement"
$CP "$I/units/ships/acknowledgement-01.wav" "$O/sounds/alliance/ships/acknowledgement/1.wav"
$CP "$I/units/ships/acknowledgement-02.wav" "$O/sounds/alliance/ships/acknowledgement/2.wav"
$CP "$I/units/ships/acknowledgement-03.wav" "$O/sounds/alliance/ships/acknowledgement/3.wav"
$MD "$O/sounds/alliance/ships/annoyed"
$CP "$I/units/ships/annoyed-01.wav" "$O/sounds/alliance/ships/annoyed/1.wav"
$CP "$I/units/ships/annoyed-02.wav" "$O/sounds/alliance/ships/annoyed/2.wav"
$CP "$I/units/ships/annoyed-03.wav" "$O/sounds/alliance/ships/annoyed/3.wav"
$MD "$O/sounds/alliance/ships/selected"
$CP "$I/units/ships/selected-01.wav" "$O/sounds/alliance/ships/selected/1.wav"
$CP "$I/units/ships/selected-02.wav" "$O/sounds/alliance/ships/selected/2.wav"
$CP "$I/units/ships/selected-03.wav" "$O/sounds/alliance/ships/selected/3.wav"
$MD "$O/sounds/alliance/ships/submarine"
$MD "$O/sounds/alliance/ships/submarine/annoyed"
$CP "$I/units/submarine/annoyed-01.wav" "$O/sounds/alliance/ships/submarine/annoyed/1.wav"
$CP "$I/units/submarine/annoyed-02.wav" "$O/sounds/alliance/ships/submarine/annoyed/2.wav"
$CP "$I/units/submarine/annoyed-03.wav" "$O/sounds/alliance/ships/submarine/annoyed/3.wav"
$CP "$I/units/submarine/annoyed-04.wav" "$O/sounds/alliance/ships/submarine/annoyed/4.wav"

#END ALLIANCE SHIPS SOUNDS

$MD "$O/sounds/alliance/units"
$MD "$O/sounds/alliance/units/archer-ranger"

#BEGIN ARCHER/RANGER SOUNDS

$CP "$I/units/archer/ready-01.wav" "$O/sounds/alliance/units/archer-ranger/ready.wav"
$MD "$O/sounds/alliance/units/archer-ranger/acknowledgement"
$CP "$I/units/archer/acknowledgement-01.wav" "$O/sounds/alliance/units/archer-ranger/acknowledgement/1.wav"
$CP "$I/units/archer/acknowledgement-02.wav" "$O/sounds/alliance/units/archer-ranger/acknowledgement/2.wav"
$CP "$I/units/archer/acknowledgement-03.wav" "$O/sounds/alliance/units/archer-ranger/acknowledgement/3.wav"
$CP "$I/units/archer/acknowledgement-04.wav" "$O/sounds/alliance/units/archer-ranger/acknowledgement/4.wav"
$MD "$O/sounds/alliance/units/archer-ranger/annoyed"
$CP "$I/units/archer/annoyed-01.wav" "$O/sounds/alliance/units/archer-ranger/annoyed/1.wav"
$CP "$I/units/archer/annoyed-02.wav" "$O/sounds/alliance/units/archer-ranger/annoyed/2.wav"
$CP "$I/units/archer/annoyed-03.wav" "$O/sounds/alliance/units/archer-ranger/annoyed/3.wav"
$MD "$O/sounds/alliance/units/archer-ranger/selected"
$CP "$I/units/archer/selected-01.wav" "$O/sounds/alliance/units/archer-ranger/selected/1.wav"
$CP "$I/units/archer/selected-02.wav" "$O/sounds/alliance/units/archer-ranger/selected/2.wav"
$CP "$I/units/archer/selected-03.wav" "$O/sounds/alliance/units/archer-ranger/selected/3.wav"
$CP "$I/units/archer/selected-04.wav" "$O/sounds/alliance/units/archer-ranger/selected/4.wav"

#END ARCHER/RANGER SOUNDS

$MD "$O/sounds/alliance/units/balloon"

#BEGIN BALLOON SOUNDS

$CP "$I/units/balloon/ready-01.wav" "$O/sounds/alliance/units/balloon/ready.wav"
$MD "$O/sounds/alliance/units/balloon/acknowledgement"
$CP "$I/units/balloon/acknowledgement-01.wav" "$O/sounds/alliance/units/balloon/acknowledgement/1.wav"
$MD "$O/sounds/alliance/units/balloon/annoyed"
$CP "$I/units/balloon/annoyed-01.wav" "$O/sounds/alliance/units/balloon/annoyed/1.wav"
$CP "$I/units/balloon/annoyed-02.wav" "$O/sounds/alliance/units/balloon/annoyed/2.wav"
$CP "$I/units/balloon/annoyed-03.wav" "$O/sounds/alliance/units/balloon/annoyed/3.wav"
$CP "$I/units/balloon/annoyed-04.wav" "$O/sounds/alliance/units/balloon/annoyed/4.wav"
$CP "$I/units/balloon/annoyed-05.wav" "$O/sounds/alliance/units/balloon/annoyed/5.wav"

#END BALLOON SOUNDS

$MD "$O/sounds/alliance/units/demolition_squad"

#BEGIN DEMOLITION SQUAD SOUNDS

$CP "$I/units/demolition_squad/ready-01.wav" "$O/sounds/alliance/units/demolition_squad/ready.wav"
$MD "$O/sounds/alliance/units/demolition_squad/acknowledgement"
$CP "$I/units/demolition_squad/acknowledgement-01.wav" "$O/sounds/alliance/units/demolition_squad/acknowledgement/1.wav"
$CP "$I/units/demolition_squad/acknowledgement-02.wav" "$O/sounds/alliance/units/demolition_squad/acknowledgement/2.wav"
$CP "$I/units/demolition_squad/acknowledgement-03.wav" "$O/sounds/alliance/units/demolition_squad/acknowledgement/3.wav"
$CP "$I/units/demolition_squad/acknowledgement-04.wav" "$O/sounds/alliance/units/demolition_squad/acknowledgement/4.wav"
$CP "$I/units/demolition_squad/acknowledgement-05.wav" "$O/sounds/alliance/units/demolition_squad/acknowledgement/5.wav"
$MD "$O/sounds/alliance/units/demolition_squad/annoyed"
$CP "$I/units/demolition_squad/annoyed-01.wav" "$O/sounds/alliance/units/demolition_squad/annoyed/1.wav"
$CP "$I/units/demolition_squad/annoyed-02.wav" "$O/sounds/alliance/units/demolition_squad/annoyed/2.wav"
$CP "$I/units/demolition_squad/annoyed-03.wav" "$O/sounds/alliance/units/demolition_squad/annoyed/3.wav"
$MD "$O/sounds/alliance/units/demolition_squad/selected"
$CP "$I/units/demolition_squad/selected-01.wav" "$O/sounds/alliance/units/demolition_squad/selected/1.wav"
$CP "$I/units/demolition_squad/selected-02.wav" "$O/sounds/alliance/units/demolition_squad/selected/2.wav"

#END DEMOLITION SQUAD SOUNDS

$MD "$O/sounds/alliance/units/female_hero"

#BEGIN FEMALE HERO SOUNDS

$MD "$O/sounds/alliance/units/female_hero/acknowledgement"
$CP "$I/units/heroes/joan_of_arc/acknowledgement_1-01.wav" "$O/sounds/alliance/units/female_hero/acknowledgement/1.wav"
$CP "$I/units/heroes/joan_of_arc/acknowledgement_2-01.wav" "$O/sounds/alliance/units/female_hero/acknowledgement/2.wav"
$CP "$I/units/heroes/joan_of_arc/acknowledgement_3-01.wav" "$O/sounds/alliance/units/female_hero/acknowledgement/3.wav"
$MD "$O/sounds/alliance/units/female_hero/annoyed"
$CP "$I/units/heroes/joan_of_arc/annoyed_1-01.wav" "$O/sounds/alliance/units/female_hero/annoyed/1.wav"
$CP "$I/units/heroes/joan_of_arc/annoyed_2-01.wav" "$O/sounds/alliance/units/female_hero/annoyed/2.wav"
$CP "$I/units/heroes/joan_of_arc/annoyed_3-01.wav" "$O/sounds/alliance/units/female_hero/annoyed/3.wav"
$MD "$O/sounds/alliance/units/female_hero/selected"
$CP "$I/units/heroes/joan_of_arc/selected_1-01.wav" "$O/sounds/alliance/units/female_hero/selected/1.wav"
$CP "$I/units/heroes/joan_of_arc/selected_2-01.wav" "$O/sounds/alliance/units/female_hero/selected/2.wav"
$CP "$I/units/heroes/joan_of_arc/selected_3-01.wav" "$O/sounds/alliance/units/female_hero/selected/3.wav"

#END FEMALE HERO SOUNDS

$MD "$O/sounds/alliance/units/flying_angle"

#BEGIN FLYING ANGLE SOUNDS

$MD "$O/sounds/alliance/units/flying_angle/acknowledgement"
$CP "$I/units/heroes/perseus/acknowledgement_1-01.wav" "$O/sounds/alliance/units/flying_angle/acknowledgement/1.wav"
$CP "$I/units/heroes/perseus/acknowledgement_2-01.wav" "$O/sounds/alliance/units/flying_angle/acknowledgement/2.wav"
$CP "$I/units/heroes/perseus/acknowledgement_3-01.wav" "$O/sounds/alliance/units/flying_angle/acknowledgement/3.wav"
$MD "$O/sounds/alliance/units/flying_angle/annoyed"
$CP "$I/units/heroes/perseus/annoyed_1-01.wav" "$O/sounds/alliance/units/flying_angle/annoyed/1.wav"
$CP "$I/units/heroes/perseus/annoyed_2-01.wav" "$O/sounds/alliance/units/flying_angle/annoyed/2.wav"
$CP "$I/units/heroes/perseus/annoyed_3-01.wav" "$O/sounds/alliance/units/flying_angle/annoyed/3.wav"
$MD "$O/sounds/alliance/units/flying_angle/selected"
$CP "$I/units/heroes/perseus/selected_1-01.wav" "$O/sounds/alliance/units/flying_angle/selected/1.wav"
$CP "$I/units/heroes/perseus/selected_2-01.wav" "$O/sounds/alliance/units/flying_angle/selected/2.wav"
$CP "$I/units/heroes/perseus/selected_3-01.wav" "$O/sounds/alliance/units/flying_angle/selected/3.wav"

#END FLYING ANGLE SOUNDS

$MD "$O/sounds/alliance/units/gryphon_rider"

#BEGIN GRYPHON RIDER SOUNDS

$CP "$I/units/gryphon_rider/acknowledgement-02.wav" "$O/sounds/alliance/units/gryphon_rider/ready.wav"
$MD "$O/sounds/alliance/units/gryphon_rider/acknowledgement"
$CP "$I/units/gryphon_rider/acknowledgement-01.wav" "$O/sounds/alliance/units/gryphon_rider/acknowledgement/2.wav"
$MD "$O/sounds/alliance/units/gryphon_rider/selected"
$CP "$I/units/gryphon_rider/selected-01.wav" "$O/sounds/alliance/units/gryphon_rider/selected/1.wav"

#END GRYPHON RIDER SOUNDS

$MD "$O/sounds/alliance/units/knight"

#BEGIN KNIGHT SOUNDS

$CP "$I/units/knight/ready-01.wav" "$O/sounds/alliance/units/knight/ready.wav"
$MD "$O/sounds/alliance/units/knight/acknowledgement"
$CP "$I/units/knight/acknowledgement-01.wav" "$O/sounds/alliance/units/knight/acknowledgement/1.wav"
$CP "$I/units/knight/acknowledgement-01.wav" "$O/sounds/alliance/units/knight/acknowledgement/2.wav"
$CP "$I/units/knight/acknowledgement-01.wav" "$O/sounds/alliance/units/knight/acknowledgement/3.wav"
$CP "$I/units/knight/acknowledgement-01.wav" "$O/sounds/alliance/units/knight/acknowledgement/4.wav"
$MD "$O/sounds/alliance/units/knight/annoyed"
$CP "$I/units/knight/annoyed-01.wav" "$O/sounds/alliance/units/knight/annoyed/1.wav"
$CP "$I/units/knight/annoyed-02.wav" "$O/sounds/alliance/units/knight/annoyed/2.wav"
$CP "$I/units/knight/annoyed-03.wav" "$O/sounds/alliance/units/knight/annoyed/3.wav"
$MD "$O/sounds/alliance/units/knight/selected"
$CP "$I/units/knight/selected-01.wav" "$O/sounds/alliance/units/knight/selected/1.wav"
$CP "$I/units/knight/selected-02.wav" "$O/sounds/alliance/units/knight/selected/2.wav"
$CP "$I/units/knight/selected-03.wav" "$O/sounds/alliance/units/knight/selected/3.wav"
$CP "$I/units/knight/selected-04.wav" "$O/sounds/alliance/units/knight/selected/4.wav"

#END KNIGHT SOUNDS

$MD "$O/sounds/alliance/units/knight_rider"

#BEGIN KNIGHT RIDER SOUNDS

$MD "$O/sounds/alliance/units/knight_rider/acknowledgement"
$CP "$I/units/heroes/lancelot/acknowledgement_1-01.wav" "$O/sounds/alliance/units/knight_rider/acknowledgement/1.wav"
$CP "$I/units/heroes/lancelot/acknowledgement_2-01.wav" "$O/sounds/alliance/units/knight_rider/acknowledgement/2.wav"
$CP "$I/units/heroes/lancelot/acknowledgement_3-01.wav" "$O/sounds/alliance/units/knight_rider/acknowledgement/3.wav"
$MD "$O/sounds/alliance/units/knight_rider/annoyed"
$CP "$I/units/heroes/lancelot/annoyed_1-01.wav" "$O/sounds/alliance/units/knight_rider/annoyed/1.wav"
$CP "$I/units/heroes/lancelot/annoyed_2-01.wav" "$O/sounds/alliance/units/knight_rider/annoyed/2.wav"
$CP "$I/units/heroes/lancelot/annoyed_3-01.wav" "$O/sounds/alliance/units/knight_rider/annoyed/3.wav"
$MD "$O/sounds/alliance/units/knight_rider/selected"
$CP "$I/units/heroes/lancelot/selected_1-01.wav" "$O/sounds/alliance/units/knight_rider/selected/1.wav"
$CP "$I/units/heroes/lancelot/selected_2-01.wav" "$O/sounds/alliance/units/knight_rider/selected/2.wav"
$CP "$I/units/heroes/lancelot/selected_3-01.wav" "$O/sounds/alliance/units/knight_rider/selected/3.wav"

#END KNIGHT RIDER SOUNDS

$MD "$O/sounds/alliance/units/mage"

#BEGIN MAGE SOUNDS

$CP "$I/units/mage/ready-01.wav" "$O/sounds/alliance/units/mage/ready.wav"
$MD "$O/sounds/alliance/units/mage/acknowledgement"
$CP "$I/units/mage/acknowledgement-01.wav" "$O/sounds/alliance/units/mage/acknowledgement/1.wav"
$CP "$I/units/mage/acknowledgement-02.wav" "$O/sounds/alliance/units/mage/acknowledgement/2.wav"
$CP "$I/units/mage/acknowledgement-03.wav" "$O/sounds/alliance/units/mage/acknowledgement/3.wav"
$MD "$O/sounds/alliance/units/mage/annoyed"
$CP "$I/units/mage/annoyed-01.wav" "$O/sounds/alliance/units/mage/annoyed/1.wav"
$CP "$I/units/mage/annoyed-02.wav" "$O/sounds/alliance/units/mage/annoyed/2.wav"
$CP "$I/units/mage/annoyed-03.wav" "$O/sounds/alliance/units/mage/annoyed/3.wav"
$MD "$O/sounds/alliance/units/mage/selected"
$CP "$I/units/mage/selected-01.wav" "$O/sounds/alliance/units/mage/selected/1.wav"
$CP "$I/units/mage/selected-02.wav" "$O/sounds/alliance/units/mage/selected/2.wav"
$CP "$I/units/mage/selected-03.wav" "$O/sounds/alliance/units/mage/selected/3.wav"

#END MAGE SOUNDS

$MD "$O/sounds/alliance/units/paladin"

#BEGIN PALADIN SOUNDS

$CP "$I/units/paladin/ready-01.wav" "$O/sounds/alliance/units/paladin/ready.wav"
$MD "$O/sounds/alliance/units/paladin/acknowledgement"
$CP "$I/units/paladin/acknowledgement-01.wav" "$O/sounds/alliance/units/paladin/acknowledgement/1.wav"
$CP "$I/units/paladin/acknowledgement-02.wav" "$O/sounds/alliance/units/paladin/acknowledgement/2.wav"
$CP "$I/units/paladin/acknowledgement-03.wav" "$O/sounds/alliance/units/paladin/acknowledgement/3.wav"
$CP "$I/units/paladin/acknowledgement-04.wav" "$O/sounds/alliance/units/paladin/acknowledgement/4.wav"
$MD "$O/sounds/alliance/units/paladin/annoyed"
$CP "$I/units/paladin/annoyed-01.wav" "$O/sounds/alliance/units/paladin/annoyed/1.wav"
$CP "$I/units/paladin/annoyed-02.wav" "$O/sounds/alliance/units/paladin/annoyed/2.wav"
$CP "$I/units/paladin/annoyed-03.wav" "$O/sounds/alliance/units/paladin/annoyed/3.wav"
$MD "$O/sounds/alliance/units/paladin/selected"
$CP "$I/units/paladin/selected-01.wav" "$O/sounds/alliance/units/paladin/selected/1.wav"
$CP "$I/units/paladin/selected-02.wav" "$O/sounds/alliance/units/paladin/selected/2.wav"
$CP "$I/units/paladin/selected-03.wav" "$O/sounds/alliance/units/paladin/selected/3.wav"
$CP "$I/units/paladin/selected-04.wav" "$O/sounds/alliance/units/paladin/selected/4.wav"

#END PALADIN SOUNDS

$MD "$O/sounds/alliance/units/peasant"

#BEGIN PEASANT SOUNDS

$CP "$I/units/peasant/attack-01.wav" "$O/sounds/alliance/units/peasant/attack.wav"
$CP "$I/units/peasant/ready-01.wav" "$O/sounds/alliance/units/peasant/ready.wav"
$CP "$I/units/peasant/work_complete-01.wav" "$O/sounds/alliance/units/peasant/work_complete.wav"
$MD "$O/sounds/alliance/units/peasant/acknowledgement"
$CP "$I/units/peasant/acknowledgement-01.wav" "$O/sounds/alliance/units/peasant/acknowledgement/1.wav"
$CP "$I/units/peasant/acknowledgement-02.wav" "$O/sounds/alliance/units/peasant/acknowledgement/2.wav"
$CP "$I/units/peasant/acknowledgement-03.wav" "$O/sounds/alliance/units/peasant/acknowledgement/3.wav"
$CP "$I/units/peasant/acknowledgement-04.wav" "$O/sounds/alliance/units/peasant/acknowledgement/4.wav"
$MD "$O/sounds/alliance/units/peasant/annoyed"
$CP "$I/units/peasant/annoyed-01.wav" "$O/sounds/alliance/units/peasant/annoyed/1.wav"
$CP "$I/units/peasant/annoyed-02.wav" "$O/sounds/alliance/units/peasant/annoyed/2.wav"
$CP "$I/units/peasant/annoyed-03.wav" "$O/sounds/alliance/units/peasant/annoyed/3.wav"
$CP "$I/units/peasant/annoyed-04.wav" "$O/sounds/alliance/units/peasant/annoyed/4.wav"
$CP "$I/units/peasant/annoyed-05.wav" "$O/sounds/alliance/units/peasant/annoyed/5.wav"
$CP "$I/units/peasant/annoyed-06.wav" "$O/sounds/alliance/units/peasant/annoyed/6.wav"
$CP "$I/units/peasant/annoyed-07.wav" "$O/sounds/alliance/units/peasant/annoyed/7.wav"
$MD "$O/sounds/alliance/units/peasant/selected"
$CP "$I/units/peasant/selected-01.wav" "$O/sounds/alliance/units/peasant/selected/1.wav"
$CP "$I/units/peasant/selected-02.wav" "$O/sounds/alliance/units/peasant/selected/2.wav"
$CP "$I/units/peasant/selected-03.wav" "$O/sounds/alliance/units/peasant/selected/3.wav"
$CP "$I/units/peasant/selected-04.wav" "$O/sounds/alliance/units/peasant/selected/4.wav"

#END PEASANT SOUNDS

$MD "$O/sounds/alliance/units/ugly_guy"

#BEGIN UGLY GUY SOUNDS

$MD "$O/sounds/alliance/units/ugly_guy/acknowledgement"
$CP "$I/units/heroes/hercules/acknowledgement_1-01.wav" "$O/sounds/alliance/units/ugly_guy/acknowledgement/1.wav"
$CP "$I/units/heroes/hercules/acknowledgement_2-01.wav" "$O/sounds/alliance/units/ugly_guy/acknowledgement/2.wav"
$CP "$I/units/heroes/hercules/acknowledgement_3-01.wav" "$O/sounds/alliance/units/ugly_guy/acknowledgement/3.wav"
$MD "$O/sounds/alliance/units/ugly_guy/annoyed"
$CP "$I/units/heroes/hercules/annoyed_1-01.wav" "$O/sounds/alliance/units/ugly_guy/annoyed/1.wav"
$CP "$I/units/heroes/hercules/annoyed_2-01.wav" "$O/sounds/alliance/units/ugly_guy/annoyed/2.wav"
$CP "$I/units/heroes/hercules/annoyed_3-01.wav" "$O/sounds/alliance/units/ugly_guy/annoyed/3.wav"
$MD "$O/sounds/alliance/units/ugly_guy/selected"
$CP "$I/units/heroes/hercules/selected_1-01.wav" "$O/sounds/alliance/units/ugly_guy/selected/1.wav"
$CP "$I/units/heroes/hercules/selected_2-01.wav" "$O/sounds/alliance/units/ugly_guy/selected/2.wav"
$CP "$I/units/heroes/hercules/selected_3-01.wav" "$O/sounds/alliance/units/ugly_guy/selected/3.wav"

#END UGLY GUY SOUNDS

$MD "$O/sounds/alliance/units/white_mage"

#BEGIN WHITE MAGE SOUNDS

$MD "$O/sounds/alliance/units/white_mage/acknowledgement"
$CP "$I/units/heroes/merlin/acknowledgement_1-01.wav" "$O/sounds/alliance/units/white_mage/acknowledgement/1.wav"
$CP "$I/units/heroes/merlin/acknowledgement_2-01.wav" "$O/sounds/alliance/units/white_mage/acknowledgement/2.wav"
$CP "$I/units/heroes/merlin/acknowledgement_3-01.wav" "$O/sounds/alliance/units/white_mage/acknowledgement/3.wav"
$MD "$O/sounds/alliance/units/white_mage/annoyed"
$CP "$I/units/heroes/merlin/annoyed_1-01.wav" "$O/sounds/alliance/units/white_mage/annoyed/1.wav"
$CP "$I/units/heroes/merlin/annoyed_2-01.wav" "$O/sounds/alliance/units/white_mage/annoyed/2.wav"
$CP "$I/units/heroes/merlin/annoyed_3-01.wav" "$O/sounds/alliance/units/white_mage/annoyed/3.wav"
$MD "$O/sounds/alliance/units/white_mage/selected"
$CP "$I/units/heroes/merlin/selected_1-01.wav" "$O/sounds/alliance/units/white_mage/selected/1.wav"
$CP "$I/units/heroes/merlin/selected_2-01.wav" "$O/sounds/alliance/units/white_mage/selected/2.wav"
$CP "$I/units/heroes/merlin/selected_3-01.wav" "$O/sounds/alliance/units/white_mage/selected/3.wav"

#END WHITE MAGE SOUNDS

$MD "$O/sounds/buildings"

#BEGIN BUILDING SOUNDS

$SOX "$I/buildings/blacksmith/blacksmith-01.wav" "$O/sounds/buildings/blacksmith.wav"
$SOX "$I/buildings/oil_platform/oil_platform-01.wav" "$O/sounds/buildings/oil_platform.wav"
$SOX "$I/buildings/foundry/foundry-01.wav" "$O/sounds/buildings/foundry.wav"
$SOX "$I/buildings/refinery/refinery-01.wav" "$O/sounds/buildings/oil_refinery.wav"
$CP "$I/buildings/lumber_mill/lumber_mill-01.wav" "$O/sounds/buildings/lumbermill.wav"
$CP "$I/buildings/shipyard/shipyard-02.wav" "$O/sounds/buildings/shipyard.wav"

#END BUILDING SOUNDS

$MD "$O/sounds/misc"

#BEGIN MISC SOUNDS

$MD "$O/sounds/misc/building_explosion"
$SOX "$I/misc/building_explosion-04.wav" "$O/sounds/misc/building_explosion/1.wav"
$CP "$I/misc/building_explosion-03.wav" "$O/sounds/misc/building_explosion/2.wav"
$CP "$I/misc/building_explosion-02.wav" "$O/sounds/misc/building_explosion/3.wav"
$MD "$O/sounds/misc/tree_chopping"
$SOX "$I/misc/tree_chopping-01.wav" "$O/sounds/misc/tree_chopping/1.wav"
$SOX "$I/misc/tree_chopping-02.wav" "$O/sounds/misc/tree_chopping/2.wav"
$SOX "$I/misc/tree_chopping-03.wav" "$O/sounds/misc/tree_chopping/3.wav"
$SOX "$I/misc/tree_chopping-04.wav" "$O/sounds/misc/tree_chopping/4.wav"
$CP "$I/misc/burning-01.wav" "$O/sounds/misc/burning.wav"
$CP "$I/misc/building_explosion-01.wav" "$O/sounds/misc/building_construction.wav"
$CP "$I/misc/explosion-01.wav" "$O/sounds/misc/explosion.wav"
$CP "$I/misc/transport_docking-01.wav" "$O/sounds/misc/transport_docking.wav"

#END MISC SOUNDS

$MD "$O/sounds/missiles"

#BEGIN MISSILES SOUNDS

$SOX "$I/missiles/axe_throw-01.wav" "$O/sounds/missiles/axe_throw.wav"
$SOX "$I/missiles/bow_hit-01.wav" "$O/sounds/missiles/bow_hit.wav"
$SOX "$I/missiles/bow_throw-01.wav" "$O/sounds/missiles/bow_throw.wav"
$SOX "$I/missiles/catapult_ballista_attack-01.wav" "$O/sounds/missiles/catapult-ballista_attack.wav"
$CP "$I/spells/fireball_hit-01.wav" "$O/sounds/missiles/fireball_hit.wav"
$CP "$I/spells/fireball_throw-01.wav" "$O/sounds/missiles/fireball_throw.wav"
$SOX "$I/missiles/fist-01.wav" "$O/sounds/missiles/fist.wav"
$SOX "$I/missiles/punch-01.wav" "$O/sounds/missiles/punch.wav"
$MD "$O/sounds/missiles/sword_attack"
$SOX "$I/missiles/sword_attack-01.wav" "$O/sounds/missiles/sword_attack/1.wav"
$SOX "$I/missiles/sword_attack-02.wav" "$O/sounds/missiles/sword_attack/2.wav"
$SOX "$I/missiles/sword_attack-03.wav" "$O/sounds/missiles/sword_attack/3.wav"

#END MISSILES SOUNDS

$MD "$O/sounds/mythical"

#BEGIN MISC MYTHICAL SOUNDS

$CP "$I/misc/capture-02.wav" "$O/sounds/mythical/capture.wav"
$CP "$I/misc/rescue-02.wav" "$O/sounds/mythical/rescue.wav"

#END MISC MYTHICAL SOUNDS

$MD "$O/sounds/mythical/basic_voices"

#BEGIN MYTHICAL BASIC VOICES

$CP "$I/units/common/dead-02.wav" "$O/sounds/mythical/basic_voices/dead.wav"
$CP "$I/units/common/ready-02.wav" "$O/sounds/mythical/basic_voices/ready.wav"
$CP "$I/units/common/work_complete-02.wav" "$O/sounds/mythical/basic_voices/work_complete.wav"
$MD "$O/sounds/mythical/basic_voices/acknowledgement"
$CP "$I/units/grunt/acknowledgement-01.wav" "$O/sounds/mythical/basic_voices/acknowledgement/1.wav"
$CP "$I/units/grunt/acknowledgement-02.wav" "$O/sounds/mythical/basic_voices/acknowledgement/2.wav"
$CP "$I/units/grunt/acknowledgement-03.wav" "$O/sounds/mythical/basic_voices/acknowledgement/3.wav"
$CP "$I/units/grunt/acknowledgement-04.wav" "$O/sounds/mythical/basic_voices/acknowledgement/4.wav"
$MD "$O/sounds/mythical/basic_voices/annoyed"
$CP "$I/units/grunt/annoyed-01.wav" "$O/sounds/mythical/basic_voices/annoyed/1.wav"
$CP "$I/units/grunt/annoyed-02.wav" "$O/sounds/mythical/basic_voices/annoyed/2.wav"
$CP "$I/units/grunt/annoyed-03.wav" "$O/sounds/mythical/basic_voices/annoyed/3.wav"
$CP "$I/units/grunt/annoyed-04.wav" "$O/sounds/mythical/basic_voices/annoyed/4.wav"
$CP "$I/units/grunt/annoyed-05.wav" "$O/sounds/mythical/basic_voices/annoyed/5.wav"
$CP "$I/units/grunt/annoyed-06.wav" "$O/sounds/mythical/basic_voices/annoyed/6.wav"
$CP "$I/units/grunt/annoyed-07.wav" "$O/sounds/mythical/basic_voices/annoyed/7.wav"
$MD "$O/sounds/mythical/basic_voices/help"
$CP "$I/units/common/help_1-02.wav" "$O/sounds/mythical/basic_voices/help/1.wav"
$CP "$I/units/common/help_2-02.wav" "$O/sounds/mythical/basic_voices/help/2.wav"
$MD "$O/sounds/mythical/basic_voices/selected"
$CP "$I/units/grunt/selected-01.wav" "$O/sounds/mythical/basic_voices/selected/1.wav"
$CP "$I/units/grunt/selected-02.wav" "$O/sounds/mythical/basic_voices/selected/2.wav"
$CP "$I/units/grunt/selected-03.wav" "$O/sounds/mythical/basic_voices/selected/3.wav"
$CP "$I/units/grunt/selected-04.wav" "$O/sounds/mythical/basic_voices/selected/4.wav"
$CP "$I/units/grunt/selected-05.wav" "$O/sounds/mythical/basic_voices/selected/5.wav"
$CP "$I/units/grunt/selected-06.wav" "$O/sounds/mythical/basic_voices/selected/6.wav"

#END MYTHICAL BASIC VOICES

$MD "$O/sounds/mythical/buildings"

#BEGIN MYTHICAL BUILDING SOUNDS

$CP "$I/buildings/alchemist/alchemist-01.wav" "$O/sounds/mythical/buildings/alchemist.wav"
$CP "$I/buildings/altar_of_storms/altar_of_storms-01.wav" "$O/sounds/mythical/buildings/altar_of_storms.wav"
$CP "$I/buildings/dragon_roost/dragon_roost-01.wav" "$O/sounds/mythical/buildings/dragon_roost.wav"
$CP "$I/buildings/farm/farm-02.wav" "$O/sounds/mythical/buildings/farm.wav"
$CP "$I/buildings/ogre_mound/ogre_mound-01.wav" "$O/sounds/mythical/buildings/ogre_mound.wav"
$CP "$I/buildings/temple_of_the_damned/temple_of_the_damned-01.wav" "$O/sounds/mythical/buildings/temple_of_the_damned.wav"

#END MYTHICAL BUILDING SOUNDS

$MD "$O/sounds/mythical/peon"

#BEGIN PEON SOUNDS

$CP "$I/units/peon/ready-01.wav" "$O/sounds/mythical/peon/ready.wav"

#END PEON SOUNDS

$MD "$O/sounds/mythical/ships"

#BEGIN MYTHICAL SHIPS SOUNDS

$CP "$I/units/ships/ready-02.wav" "$O/sounds/mythical/ships/ready.wav"
$MD "$O/sounds/mythical/ships/acknowledgement"
$CP "$I/units/ships/acknowledgement-04.wav" "$O/sounds/mythical/ships/acknowledgement/1.wav"
$CP "$I/units/ships/acknowledgement-05.wav" "$O/sounds/mythical/ships/acknowledgement/2.wav"
$CP "$I/units/ships/acknowledgement-06.wav" "$O/sounds/mythical/ships/acknowledgement/3.wav"
$MD "$O/sounds/mythical/ships/annoyed"
$CP "$I/units/ships/annoyed-04.wav" "$O/sounds/mythical/ships/annoyed/1.wav"
$CP "$I/units/ships/annoyed-05.wav" "$O/sounds/mythical/ships/annoyed/2.wav"
$CP "$I/units/ships/annoyed-06.wav" "$O/sounds/mythical/ships/annoyed/3.wav"
$MD "$O/sounds/mythical/ships/selected"
$CP "$I/units/ships/selected-04.wav" "$O/sounds/mythical/ships/selected/1.wav"
$CP "$I/units/ships/selected-05.wav" "$O/sounds/mythical/ships/selected/2.wav"
$CP "$I/units/ships/selected-06.wav" "$O/sounds/mythical/ships/selected/3.wav"

#END MYTHICAL SHIPS SOUNDS

$MD "$O/sounds/mythical/units"
$MD "$O/sounds/mythical/units/axethrower-berserker"

#BEGIN AXETHROWER SOUNDS

$CP "$I/units/axethrower/ready-01.wav" "$O/sounds/mythical/units/axethrower-berserker/ready.wav"
$MD "$O/sounds/mythical/units/axethrower-berserker/acknowledgement"
$CP "$I/units/axethrower/acknowledgement-01.wav" "$O/sounds/mythical/units/axethrower-berserker/acknowledgement/1.wav"
$CP "$I/units/axethrower/acknowledgement-02.wav" "$O/sounds/mythical/units/axethrower-berserker/acknowledgement/2.wav"
$CP "$I/units/axethrower/acknowledgement-03.wav" "$O/sounds/mythical/units/axethrower-berserker/acknowledgement/3.wav"
$MD "$O/sounds/mythical/units/axethrower-berserker/annoyed"
$CP "$I/units/axethrower/annoyed-01.wav" "$O/sounds/mythical/units/axethrower-berserker/annoyed/1.wav"
$CP "$I/units/axethrower/annoyed-02.wav" "$O/sounds/mythical/units/axethrower-berserker/annoyed/2.wav"
$CP "$I/units/axethrower/annoyed-03.wav" "$O/sounds/mythical/units/axethrower-berserker/annoyed/3.wav"
$MD "$O/sounds/mythical/units/axethrower-berserker/selected"
$CP "$I/units/axethrower/selected-01.wav" "$O/sounds/mythical/units/axethrower-berserker/selected/1.wav"
$CP "$I/units/axethrower/selected-02.wav" "$O/sounds/mythical/units/axethrower-berserker/selected/2.wav"
$CP "$I/units/axethrower/selected-03.wav" "$O/sounds/mythical/units/axethrower-berserker/selected/3.wav"

#END AXETHROWER SOUNDS

$MD "$O/sounds/mythical/units/beast_cry"

#BEGIN BEAST CRY SOUNDS

$MD "$O/sounds/mythical/units/beast_cry/acknowledgement"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/beast_cry/acknowledgement/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/beast_cry/acknowledgement/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/beast_cry/acknowledgement/3.wav"
$MD "$O/sounds/mythical/units/beast_cry/annoyed"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/beast_cry/annoyed/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/beast_cry/annoyed/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/beast_cry/annoyed/3.wav"
$MD "$O/sounds/mythical/units/beast_cry/selected"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/beast_cry/selected/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/beast_cry/selected/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/beast_cry/selected/3.wav"

#END BEAST CRY SOUNDS

$MD "$O/sounds/mythical/units/death_knight"

#BEGIN DEATH KNIGHT SOUNDS

$CP "$I/units/death_knight/ready-03.wav" "$O/sounds/mythical/units/death_knight/ready.wav"
$MD "$O/sounds/mythical/units/death_knight/acknowledgement"
$CP "$I/units/death_knight/acknowledgement-08.wav" "$O/sounds/mythical/units/death_knight/acknowledgement/1.wav"
$CP "$I/units/death_knight/acknowledgement-09.wav" "$O/sounds/mythical/units/death_knight/acknowledgement/2.wav"
$CP "$I/units/death_knight/acknowledgement-10.wav" "$O/sounds/mythical/units/death_knight/acknowledgement/3.wav"
$MD "$O/sounds/mythical/units/death_knight/annoyed"
$CP "$I/units/death_knight/annoyed-07.wav" "$O/sounds/mythical/units/death_knight/annoyed/1.wav"
$CP "$I/units/death_knight/annoyed-08.wav" "$O/sounds/mythical/units/death_knight/annoyed/2.wav"
$CP "$I/units/death_knight/annoyed-09.wav" "$O/sounds/mythical/units/death_knight/annoyed/3.wav"
$MD "$O/sounds/mythical/units/death_knight/selected"
$CP "$I/units/death_knight/selected-05.wav" "$O/sounds/mythical/units/death_knight/selected/1.wav"
$CP "$I/units/death_knight/selected-06.wav" "$O/sounds/mythical/units/death_knight/selected/2.wav"

#END DEATH KNIGHT SOUNDS

$MD "$O/sounds/mythical/units/dragon"

#BEGIN DRAGON SOUNDS

$CP "$I/units/dragon/ready-01.wav" "$O/sounds/mythical/units/dragon/ready.wav"
$MD "$O/sounds/mythical/units/dragon/acknowledgement"
$CP "$I/units/dragon/acknowledgement-01.wav" "$O/sounds/mythical/units/dragon/acknowledgement/1.wav"
$CP "$I/units/dragon/acknowledgement-02.wav" "$O/sounds/mythical/units/dragon/acknowledgement/2.wav"
$MD "$O/sounds/mythical/units/dragon/selected"
$CP "$I/units/dragon/selected-01.wav" "$O/sounds/mythical/units/dragon/selected/1.wav"

#END DRAGON SOUNDS

$MD "$O/sounds/mythical/units/evil_knight"

#BEGIN EVIL KNIGHT SOUNDS

$MD "$O/sounds/mythical/units/evil_knight/acknowledgement"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/evil_knight/acknowledgement/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/evil_knight/acknowledgement/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/evil_knight/acknowledgement/3.wav"
$MD "$O/sounds/mythical/units/evil_knight/annoyed"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/evil_knight/annoyed/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/evil_knight/annoyed/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/evil_knight/annoyed/3.wav"
$MD "$O/sounds/mythical/units/evil_knight/selected"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/evil_knight/selected/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/evil_knight/selected/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/evil_knight/selected/3.wav"

#END EVIL KNIGHT SOUNDS

$MD "$O/sounds/mythical/units/fad_man"

#BEGIN FAD MAN SOUNDS

$MD "$O/sounds/mythical/units/fad_man/acknowledgement"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fad_man/acknowledgement/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fad_man/acknowledgement/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fad_man/acknowledgement/3.wav"
$MD "$O/sounds/mythical/units/fad_man/annoyed"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fad_man/annoyed/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fad_man/annoyed/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fad_man/annoyed/3.wav"
$MD "$O/sounds/mythical/units/fad_man/selected"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fad_man/selected/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fad_man/selected/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fad_man/selected/3.wav"

#END FAD MAN SOUNDS

$MD "$O/sounds/mythical/units/fire_breeze"

#BEGIN FIRE BREEZE SOUNDS

$MD "$O/sounds/mythical/units/fire_breeze/acknowledgement"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fire_breeze/acknowledgement/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fire_breeze/acknowledgement/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fire_breeze/acknowledgement/3.wav"
$MD "$O/sounds/mythical/units/fire_breeze/annoyed"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fire_breeze/annoyed/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fire_breeze/annoyed/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fire_breeze/annoyed/3.wav"
$MD "$O/sounds/mythical/units/fire_breeze/selected"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fire_breeze/selected/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fire_breeze/selected/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/fire_breeze/selected/3.wav"

#END FIRE BREEZE SOUNDS

$MD "$O/sounds/mythical/units/goblin_sappers"

#BEGIN GOBLIN SAPPERS SOUNDS

$CP "$I/units/goblin_sappers/ready-01.wav" "$O/sounds/mythical/units/goblin_sappers/ready.wav"
$MD "$O/sounds/mythical/units/goblin_sappers/acknowledgement"
$CP "$I/units/goblin_sappers/acknowledgement-01.wav" "$O/sounds/mythical/units/goblin_sappers/acknowledgement/1.wav"
$CP "$I/units/goblin_sappers/acknowledgement-02.wav" "$O/sounds/mythical/units/goblin_sappers/acknowledgement/2.wav"
$CP "$I/units/goblin_sappers/acknowledgement-03.wav" "$O/sounds/mythical/units/goblin_sappers/acknowledgement/3.wav"
$CP "$I/units/goblin_sappers/acknowledgement-04.wav" "$O/sounds/mythical/units/goblin_sappers/acknowledgement/4.wav"
$MD "$O/sounds/mythical/units/goblin_sappers/annoyed"
$CP "$I/units/goblin_sappers/annoyed-01.wav" "$O/sounds/mythical/units/goblin_sappers/annoyed/1.wav"
$CP "$I/units/goblin_sappers/annoyed-02.wav" "$O/sounds/mythical/units/goblin_sappers/annoyed/2.wav"
$CP "$I/units/goblin_sappers/annoyed-03.wav" "$O/sounds/mythical/units/goblin_sappers/annoyed/3.wav"
$MD "$O/sounds/mythical/units/goblin_sappers/selected"
$CP "$I/units/goblin_sappers/selected-01.wav" "$O/sounds/mythical/units/goblin_sappers/selected/1.wav"
$CP "$I/units/goblin_sappers/selected-02.wav" "$O/sounds/mythical/units/goblin_sappers/selected/2.wav"
$CP "$I/units/goblin_sappers/selected-03.wav" "$O/sounds/mythical/units/goblin_sappers/selected/3.wav"
$CP "$I/units/goblin_sappers/selected-04.wav" "$O/sounds/mythical/units/goblin_sappers/selected/4.wav"

#END GOBLIN SAPPERS SOUNDS

$MD "$O/sounds/mythical/units/ogre"

#BEGIN OGRE SOUNDS

$CP "$I/units/ogre/ready-01.wav" "$O/sounds/mythical/units/ogre/ready.wav"
$MD "$O/sounds/mythical/units/ogre/acknowledgement"
$CP "$I/units/ogre/acknowledgement-01.wav" "$O/sounds/mythical/units/ogre/acknowledgement/1.wav"
$CP "$I/units/ogre/acknowledgement-02.wav" "$O/sounds/mythical/units/ogre/acknowledgement/2.wav"
$CP "$I/units/ogre/acknowledgement-03.wav" "$O/sounds/mythical/units/ogre/acknowledgement/3.wav"
$MD "$O/sounds/mythical/units/ogre/annoyed"
$CP "$I/units/ogre/annoyed-01.wav" "$O/sounds/mythical/units/ogre/annoyed/1.wav"
$CP "$I/units/ogre/annoyed-02.wav" "$O/sounds/mythical/units/ogre/annoyed/2.wav"
$CP "$I/units/ogre/annoyed-03.wav" "$O/sounds/mythical/units/ogre/annoyed/3.wav"
$CP "$I/units/ogre/annoyed-04.wav" "$O/sounds/mythical/units/ogre/annoyed/4.wav"
$CP "$I/units/ogre/annoyed-05.wav" "$O/sounds/mythical/units/ogre/annoyed/5.wav"
$MD "$O/sounds/mythical/units/ogre/selected"
$CP "$I/units/ogre/selected-01.wav" "$O/sounds/mythical/units/ogre/selected/1.wav"
$CP "$I/units/ogre/selected-02.wav" "$O/sounds/mythical/units/ogre/selected/2.wav"
$CP "$I/units/ogre/selected-03.wav" "$O/sounds/mythical/units/ogre/selected/3.wav"
$CP "$I/units/ogre/selected-04.wav" "$O/sounds/mythical/units/ogre/selected/4.wav"

#END OGRE SOUNDS

$MD "$O/sounds/mythical/units/ogre-mage"

#BEGIN OGRE MAGE SOUNDS

$CP "$I/units/ogre/ogre_mage_ready-01.wav" "$O/sounds/mythical/units/ogre-mage/ready.wav"
$MD "$O/sounds/mythical/units/ogre-mage/acknowledgement"
$CP "$I/units/ogre/ogre_mage_acknowledgement-01.wav" "$O/sounds/mythical/units/ogre-mage/acknowledgement/1.wav"
$CP "$I/units/ogre/ogre_mage_acknowledgement-02.wav" "$O/sounds/mythical/units/ogre-mage/acknowledgement/2.wav"
$CP "$I/units/ogre/ogre_mage_acknowledgement-03.wav" "$O/sounds/mythical/units/ogre-mage/acknowledgement/3.wav"
$MD "$O/sounds/mythical/units/ogre-mage/annoyed"
$CP "$I/units/ogre/ogre_mage_annoyed-01.wav" "$O/sounds/mythical/units/ogre-mage/annoyed/1.wav"
$CP "$I/units/ogre/ogre_mage_annoyed-02.wav" "$O/sounds/mythical/units/ogre-mage/annoyed/2.wav"
$CP "$I/units/ogre/ogre_mage_annoyed-03.wav" "$O/sounds/mythical/units/ogre-mage/annoyed/3.wav"
$MD "$O/sounds/mythical/units/ogre-mage/selected"
$CP "$I/units/ogre/ogre_mage_selected-01.wav" "$O/sounds/mythical/units/ogre-mage/selected/1.wav"
$CP "$I/units/ogre/ogre_mage_selected-02.wav" "$O/sounds/mythical/units/ogre-mage/selected/2.wav"
$CP "$I/units/ogre/ogre_mage_selected-03.wav" "$O/sounds/mythical/units/ogre-mage/selected/3.wav"
$CP "$I/units/ogre/ogre_mage_selected-04.wav" "$O/sounds/mythical/units/ogre-mage/selected/4.wav"

#END OGRE MAGE SOUNDS

$MD "$O/sounds/mythical/units/quick_blade"

#BEGIN QUICK BLADE SOUNDS

$MD "$O/sounds/mythical/units/quick_blade/acknowledgement"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/quick_blade/acknowledgement/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/quick_blade/acknowledgement/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/quick_blade/acknowledgement/3.wav"
$MD "$O/sounds/mythical/units/quick_blade/annoyed"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/quick_blade/annoyed/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/quick_blade/annoyed/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/quick_blade/annoyed/3.wav"
$MD "$O/sounds/mythical/units/quick_blade/selected"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/quick_blade/selected/1.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/quick_blade/selected/2.wav"
$CP "$I/misc/missing-01.wav" "$O/sounds/mythical/units/quick_blade/selected/3.wav"

#END QUICK BLADE SOUNDS

$MD "$O/sounds/mythical/units/zeppelin"

#BEGIN ZEPPELIN SOUNDS

$CP "$I/units/zeppelin/ready-02.wav" "$O/sounds/mythical/units/zeppelin/ready.wav"
$MD "$O/sounds/mythical/units/zeppelin/acknowledgement"
$CP "$I/units/zeppelin/acknowledgement-01.wav" "$O/sounds/mythical/units/zeppelin/acknowledgement/1.wav"
$MD "$O/sounds/mythical/units/zeppelin/annoyed"
$CP "$I/units/zeppelin/annoyed-01.wav" "$O/sounds/mythical/units/zeppelin/annoyed/1.wav"
$CP "$I/units/zeppelin/annoyed-02.wav" "$O/sounds/mythical/units/zeppelin/annoyed/2.wav"

#END ZEPPELIN SOUNDS

$MD "$O/sounds/neutral"
$MD "$O/sounds/neutral/buildings"

#BEGIN NEUTRAL BUILDING SOUNDS

$SOX "$I/buildings/gold_mine/gold_mine-01.wav" "$O/sounds/neutral/buildings/gold_mine.wav"

#END NEUTRAL BUILDING SOUNDS

$MD "$O/sounds/neutral/units"

#BEGIN CRITTER SOUNDS

$MD "$O/sounds/neutral/units/pig"
$MD "$O/sounds/neutral/units/pig/annoyed"
$SOX "$I/units/critter/pig-annoyed-01.wav" "$O/sounds/neutral/units/pig/annoyed/1.wav"
$MD "$O/sounds/neutral/units/pig/selected"
$SOX "$I/units/critter/pig-selected-01.wav" "$O/sounds/neutral/units/pig/selected/1.wav"
$MD "$O/sounds/neutral/units/seal"
$MD "$O/sounds/neutral/units/seal/annoyed"
$SOX "$I/units/critter/seal-annoyed-01.wav" "$O/sounds/neutral/units/seal/annoyed/1.wav"
$MD "$O/sounds/neutral/units/seal/selected"
$SOX "$I/units/critter/seal-selected-01.wav" "$O/sounds/neutral/units/seal/selected/1.wav"
$MD "$O/sounds/neutral/units/sheep"
$MD "$O/sounds/neutral/units/sheep/annoyed"
$SOX "$I/units/critter/sheep-annoyed-01.wav" "$O/sounds/neutral/units/sheep/annoyed/1.wav"
$MD "$O/sounds/neutral/units/sheep/selected"
$SOX "$I/units/critter/sheep-selected-01.wav" "$O/sounds/neutral/units/sheep/selected/1.wav"
$MD "$O/sounds/neutral/units/warthog"
$MD "$O/sounds/neutral/units/warthog/annoyed"
$SOX "$I/units/critter/warthog-annoyed-01.wav" "$O/sounds/neutral/units/warthog/annoyed/1.wav"
$MD "$O/sounds/neutral/units/warthog/selected"
$SOX "$I/units/critter/warthog-selected-01.wav" "$O/sounds/neutral/units/warthog/selected/1.wav"

#END CRITTER SOUNDS

$MD "$O/sounds/ships"

#BEGIN SHIP SOUNDS

$CP "$I/units/ships/sinking.wav" "$O/sounds/ships/sinking.wav"
$MD "$O/sounds/ships/tanker"
$MD "$O/sounds/ships/tanker/acknowledgement"
$CP "$I/units/oil_tanker/acknowledgement-01.wav" "$O/sounds/ships/tanker/acknowledgement/1.wav"

#END SHIP SOUNDS

$MD "$O/sounds/spells"

#BEGIN SPELL SOUNDS

$CP "$I/spells/basic_spell_sound-01.wav" "$O/sounds/spells/basic_spell_sound.wav"
$CP "$I/spells/blizzard-01.wav" "$O/sounds/spells/blizzard.wav"
$CP "$I/spells/bloodlust-01.wav" "$O/sounds/spells/bloodlust.wav"
$CP "$I/spells/death_and_decay-01.wav" "$O/sounds/spells/death_and_decay.wav"
$CP "$I/spells/death_coil-01.wav" "$O/sounds/spells/death_coil.wav"
$CP "$I/spells/exorcism-01.wav" "$O/sounds/spells/exorcism.wav"
$CP "$I/spells/eye_of_vision-01.wav" "$O/sounds/spells/eye_of_vision.wav"
$CP "$I/spells/flame_shield-01.wav" "$O/sounds/spells/flame_shield.wav"
$CP "$I/spells/haste-01.wav" "$O/sounds/spells/haste.wav"
$CP "$I/spells/healing-01.wav" "$O/sounds/spells/healing.wav"
$CP "$I/spells/holy_vision-01.wav" "$O/sounds/spells/holy_vision.wav"
$CP "$I/spells/invisibility-01.wav" "$O/sounds/spells/invisibility.wav"
$CP "$I/spells/lightning-01.wav" "$O/sounds/spells/lightning.wav"
$CP "$I/spells/polymorph-01.wav" "$O/sounds/spells/polymorph.wav"
$CP "$I/spells/slow-01.wav" "$O/sounds/spells/slow.wav"
$CP "$I/spells/touch_of_darkness-01.wav" "$O/sounds/spells/touch_of_darkness.wav"
$CP "$I/spells/unholy_armor-01.wav" "$O/sounds/spells/unholy_armor.wav"
$CP "$I/spells/whirlwind-01.wav" "$O/sounds/spells/whirlwind.wav"

#END SPELL SOUNDS

$MD "$O/sounds/ui"

#BEGIN UI SOUNDS

$CP "$I/ui/misc/click-01.wav" "$O/sounds/ui/click.wav"
$CP "$I/ui/misc/placement_success-01.wav" "$O/sounds/ui/placement_success.wav"
$CP "$I/ui/misc/highclick-01.wav" "$O/sounds/ui/highclick.wav"
$CP "$I/ui/misc/placement_error-01.wav" "$O/sounds/ui/placement_error.wav"

#END UI SOUNDS

$MD "$O/sounds/units"
$MD "$O/sounds/units/catapult-ballista"

#BEGIN CATAPULT-BALLISTA SOUNDS

$MD "$O/sounds/units/catapult-ballista/acknowledgement"
$CP "$I/units/ballista/acknowledgement-01.wav" "$O/sounds/units/catapult-ballista/acknowledgement/1.wav"

#END CATAPULT-BALLISTA SOUNDS

# $CP "$I/ChangeLog.html" "$O/ChangeLog.html"

echo "Copying and modifying files from wc2 datadir"
$CP "$WC2/default.cm" "$O/default.cm"

echo "Copying files from contrib"
$CP "$CONTRIB/cross.png" "$O/graphics/ui/cursors/cross.png"
$CP "$CONTRIB/red_cross.png" "$O/graphics/missiles/red_cross.png"
$CP "$CONTRIB/mana.png" "$O/graphics/ui/mana.png"
$CP "$CONTRIB/mana2.png" "$O/graphics/ui/mana2.png"
$CP "$CONTRIB/health.png" "$O/graphics/ui/health.png"
$CP "$CONTRIB/health2.png" "$O/graphics/ui/health2.png"
$CP "$CONTRIB/food.png" "$O/graphics/ui/food.png"
$CP "$CONTRIB/score.png" "$O/graphics/ui/score.png"
$CP "$CONTRIB/ore,stone,coal.png" "$O/graphics/ui/ore,stone,coal.png"
#$CP "$CONTRIB/stratagus.png" "$O/graphics/ui/stratagus.png"
$CP "$CONTRIB/toccata.mod.gz" "$O/music/default.mod.gz"
cp $CONTRIB/puds/single/* $O/puds/single >/dev/null 2>&1
cp $CONTRIB/puds/multi/* $O/puds/multiple >/dev/null 2>&1

echo "Compressing files"
find $O -type f -name "*.mod" -exec $COMPRESS {} \;
find $O -type f -name "*.pud" -exec $COMPRESS {} \;
find $O -type f -name "*.txt" -exec $COMPRESS {} \;
find $O -type f -name "*.wav" -exec $COMPRESS {} \;

echo "Creating ccl's"
./createccl.sh $WC2/scripts $O/scripts

if [ "$BUILDPACKAGE" = 1 ]; then
    rm data
    mv $O data
    zip -r aleona-cvs-$DATE.zip data
    tar -zcvf aleona-cvs-$DATE.tar.gz data
    mv data $O
fi
