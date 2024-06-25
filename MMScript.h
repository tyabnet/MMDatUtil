// CompressScript.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifndef _MMSCRIPT_H
#define _MMSCRIPT_H

#include <stdio.h>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cassert>

#include "MMDatUtil.h"

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max


// script engine.
// script format

// varvalue:  integer | float | retmacro | variable
// variable: 


class ScriptEngine
{
public:
    ScriptEngine() { doInit(); }
    ~ScriptEngine() = default;

protected:
    bool isReservedWord(const std::string& key)
    {
        return isReservedVar( key );
    }

    // script reserved words in mixed case Friendly format
    const std::string kStr_addrandomspawn               = "addrandomspawn";                  // event
    const std::string kStr_air                          = "air";                             // macro int, amount of air
    const std::string kStr_arrow                        = "arrow";                           // variable type
    const std::string kStr_Barrier_C                    = "Barrier_C";                       // collection, building
    const std::string kStr_Bat                          = "Bat";                             // collection, creature
    const std::string kStr_black                        = "black";                           // arrow color
    const std::string kStr_blue                         = "blue";                            // arrow color
    const std::string kStr_A                            = "A";                               // emerge direction
    const std::string kStr_bool                         = "bool";                            // variable type
    const std::string kStr_building                     = "building";                        // variable type or collection, building
    const std::string kStr_building_path                = "building_path";                   // macro int, number of objects
    const std::string kStr_BuildingCanteen_C            = "BuildingCanteen_C";               // collection, building
    const std::string kStr_BuildingDocks_C              = "BuildingDocks_C";                 // collection, building
    const std::string kStr_BuildingElectricFence_C      = "BuildingElectricFence_C";         // collection, building
    const std::string kStr_BuildingGeologicalCenter_C   = "BuildingGeologicalCenter_C";      // collection, building
    const std::string kStr_BuildingMiningLaser_C        = "BuildingMiningLaser_C";           // collection, building
    const std::string kStr_BuildingOreRefinery_C        = "BuildingOreRefinery_C";           // collection, building
    const std::string kStr_BuildingPowerPath_C          = "BuildingPowerPath_C";             // collection, building
    const std::string kStr_BuildingPowerStation_C       = "BuildingPowerStation_C";          // collection, building
    const std::string kStr_buildings                    = "buildings";                       // macro int, number of buildings.
    const std::string kStr_BuildingSuperTeleport_C      = "BuildingSuperTeleport_C";         // collection, building
    const std::string kStr_BuildingSupportStation_C     = "BuildingSupportStation_C";        // collection, building
    const std::string kStr_BuildingTeleportPad_C        = "BuildingTeleportPad_C";           // collection, building
    const std::string kStr_BuildingToolStore_C          = "BuildingToolStore_C";             // collection, building
    const std::string kStr_BuildingUpgradeStation_C     = "BuildingUpgradeStation_C";        // collection, building
    const std::string kStr_built                        = "built";                           // data field trigger
    const std::string kStr_canteen                      = "canteen";                         // macro, int, number of Canteens.
    const std::string kStr_cargocarrier                 = "cargocarrier";                    // macro, int number of Cargo Carriers.
    const std::string kStr_change                       = "change";                          // trigger
    const std::string kStr_chromecrusher                = "chromecrusher";                   // macro int, number of Chrome Crushers
    const std::string kStr_click                        = "click";                           // Data Field Trigger
    const std::string kStr_clock                        = "clock";                           // macro, float, return game time
    const std::string kStr_col                          = "col";                             // Data Field int, object column
    const std::string kStr_column                       = "column";                          // data field int, object column
    const std::string kStr_ConstructedBuilding          = "ConstructedBuilding";             // macro, bu1                                                                                                                           ilding, Last constructed building
    const std::string kStr_creature                     = "creature";                        // variable type
    const std::string kStr_CreatureBat_C                = "CreatureBat_C";                   // collection, creature
    const std::string kStr_CreatureIceMonster_C         = "CreatureIceMonster_C";            // collection, creature
    const std::string kStr_CreatureLavaMonster_C        = "CreatureLavaMonster_C";           // collection, creature
    const std::string kStr_CreatureRockMonster_C        = "CreatureRockMonster_C";           // collection, creature
    const std::string kStr_CreatureSmallSpider_C        = "CreatureSmallSpider_C";           // collection, creature
    const std::string kStr_CreatureSlimySlug_C          = "CreatureSlimySlug_C";             // collection, creature
    const std::string kStr_creatures                    = "creatures";                       // macro, int, Number of creatures
    const std::string kStr_Crystal_C                    = "Crystal_C";                       // collection, unstored crystals|
    const std::string kStr_crystal_seam                 = "crystal_seam";                    // macro, int, id of crystal seam always 42
    const std::string kStr_crystals                     = "crystals";                        // macro, int, number of stored crystals
    const std::string kStr_darkgreen                    = "darkgreen";                       // arrow color
    const std::string kStr_dead                         = "dead";                            // Data Field Trigger, Trigger when object is dead.
    const std::string kStr_dirt                         = "dirt";                            // Macro, Tile ID of dirt (26).
    const std::string kStr_disable                      = "disable";                         // Event, Disable object.
    const std::string kStr_docks                        = "docs";                            // Macro, Number of Docks.
    const std::string kStr_drill                        = "drill";                           // Event / Trigger, Drill tile.
    const std::string kStr_drive                        = "drive";                           // Trigger, Trigger when vehicle over tile.
    const std::string kStr_driven                       = "driven";                          // Data Field Trigger|Trigger when a miner enters a vehicle.
    const std::string kStr_driver                       = "driver";                          // Data Field, miner id of the driver.Same as driverid.
    const std::string kStr_driverid                     = "driverid";                        // Data Field, miner id of the driver.Same as driver.
    const std::string kStr_Dynamite_C                   = "Dynamite_C";                      // Collection, All dynamite outside of toolstore.
    const std::string kStr_E                            = "E";                               // Emerge Direction, East.
    const std::string kStr_eaten                        = "eaten";                           // Data Field, The number of crystals eaten / absorbed.
    const std::string kStr_electricfence                = "electricfence";                   // Macro, Number of Fences.
    const std::string kStr_ElectricFence_C              = "ElectricFence_C";                 // Macro | Number of Fence objects.Not a collection. |
    const std::string kStr_emerge                       = "emerge";                          // | Event | Spawn creature. |
    const std::string kStr_enable                       = "enable";                          // | Event | Enable object |
    const std::string kStr_enter                        = "enter";                           // | Trigger | Trigger when object enters tile. |
    const std::string kStr_EventErosion_C               = "EventErosion_C";                  // | Collection | Active erosions. |
    const std::string kStr_EventLandslide_C             = "EventLandslide_C";                // | Collection | Active active landslides. |
    const std::string kStr_erosionscale                 = "erosionscale";                    // | Macro | Global erosion scale factor |
    const std::string kStr_float                        = "float";                           // | Variable | Floating point number. |
    const std::string kStr_geologicalcenter             = "geologicalcenter";                // | Macro | Number of Geological Centers. |
    const std::string kStr_get                          = "get";                             // | Macro | Get tile ID., get(row)(col) |
    const std::string kStr_granitegrinder               = "granitegrinder";                  // | Macro | Number of Granite Grinders. |
    const std::string kStr_green                        = "green";                           // | Color | Arrow colors. |
    const std::string kStr_hard_rock                    = "hard_rock";                       // | Macro | Tile ID of hard rock(34). |
    const std::string kStr_heal                         = "heal";                            // | Event | Heal object. |
    const std::string kStr_health                       = "health";                          // | Data Field | Object hitpoints.Same has hp. |
    const std::string kStr_hidearrow                    = "hidearrow";                       // | Event | Hide arrow object. |
    const std::string kStr_highlight                    = "highlight";                       // | Event | Highlight tile. |
    const std::string kStr_highlightarrow               = "highlightarrow";                  // | [Event] | showarrow and highlight at once. |
    const std::string kStr_hostiles                     = "hostiles";                        // | Macro | Number of hostile creatures. |
    const std::string kStr_hover                        = "hover";                           // | Trigger | Trigger when mouse is over a tile. |
    const std::string kStr_hoverscout                   = "hoverscout";                      // | Macro | Number of Hover Scouts. |
    const std::string kStr_hp                           = "hp";                              // | Data Field | Object hitpoints.Same has health. |
    const std::string kStr_hurt                         = "hurt";                            // | Data Field Trigger | Trigger when object takes damage. |
    const std::string kStr_IceMonster                   = "IceMonster";                      // | Collection | Ice Monsters. |
    const std::string kStr_id                           = "id";                              // | Data Field | Object ID. |
    const std::string kStr_if                           = "if";                              // | Occurance | single time trigger. |
    const std::string kStr_init                         = "init";                            // | Event Chain | First event called after map load.
    const std::string kStr_int                          = "int";                             // | Variable | Integer number. |
    const std::string kStr_intarray                     = "intarray";                        // | Variable | Integer array. |
    const std::string kStr_ispowered                    = "ispowered";                       // | Data Field | Same as power. |
    const std::string kStr_kill                         = "kill";                            // | Event | Kill object. |
    const std::string kStr_landslide                    = "landslide";                       // | Event | Unknown. |
    const std::string kStr_laser                        = "laser";                           // | Trigger | Trigger when wall destroyed by laser. |
    const std::string kStr_laserhit                     = "laserhit";                        // | Trigger | Trigger when wall hit by laser. |
    const std::string kStr_lastbuilding                 = "lastbuilding";                    // | Macro | Last building that activated a trigger. |
    const std::string kStr_lastcreature                 = "lastcreature";                    // | Macro | Last creature that activated a trigger. |
    const std::string kStr_lastminer                    = "lastminer";                       // | Macro | Last miner that activated a trigger. |
    const std::string kStr_lastvehicle                  = "lastvehicle";                     // | Macro | Last vehicle that activated a trigger. |
    const std::string kStr_lava                         = "lava";                            // | Macro | Tile ID of lava(6). |
    const std::string kStr_LavaMonster                  = "LavaMonster";                     // | Collection | Lava Monsters. |
    const std::string kStr_level                        = "level";                           // | Data Field | Returns upgrade level of the building. |
    const std::string kStr_light                        = "light";                           // | Parameter | enable / disable parameter. |
    const std::string kStr_lights                       = "lights";                          // | Parameter | same as light. |
    const std::string kStr_LMLC                         = "LMLC";                            // | Macro | Number of Large Mobile Laser Cutters. |
    const std::string kStr_loaderdozer                  = "loaderdozer";                     // | Macro | Number of Loader Dozers. |
    const std::string kStr_loose_rock                   = "loose_rock";                      // | Macro | Tile ID of loose rock(30). |
    const std::string kStr_lose                         = "lose";                            // | Event | Lose the map. |
    const std::string kStr_miner                        = "miner";                           // | Variable / Class | Miner object or class. |
    const std::string kStr_miners                       = "miners";                          // | Macro | Miners discovered or active. |
    const std::string kStr_mininglaser                  = "mininglaser";                     // | Macro | Number of Geological Centers. |
    const std::string kStr_monsters                     = "monsters";                        // | Macro | Number of monsters. |
    const std::string kStr_msg                          = "msg";                             // | Event | Display a message to user. |
    const std::string kStr_N                            = "N";                               // | Emerge Direction | North. |
    const std::string kStr_NavModifierLava_C            = "NavModifierLava_C";               // | Collection | Amount of lava tiles. | NavModifierLava_C | Collection | Amount of lava tiles. |
    const std::string kStr_NavModifierPowerPath_C       = "NavModifierPowerPath_C";          // | Collection | Amount of Power Path tiles, any type.NavModifierPowerPath_C | Collection | Amount of Power Path tiles, any type.Only finished patOnly finished paths. |
    const std::string kStr_NavModifierRubble_C          = "NavModifierRubble_C";             // | Collection | Amount of Rubble tiles, any stage. |
    const std::string kStr_NavModifierWater_C           = "NavModifierWater_C";              // | Collection | Amount of water tiles. |
    const std::string kStr_new                          = "new";                             // | Data Field Trigger | Trigger when object is created. |
    const std::string kStr_ore                          = "ore";                             // | Macro | Ore count. |
    const std::string kStr_Ore_C                        = "Ore_C";                           // | Collection | All ore. |
    const std::string kStr_orerefinery                  = "orerefinery";                     // | Macro | Number of Ore Refineries. |
    const std::string kStr_ore_seam                     = "ore_seam";                        // | Macro | Tile ID of an ore seam(46). |
    const std::string kStr_pan                          = "pan";                             // | Event | Pan camera. |
    const std::string kStr_pause                        = "pause";                           // | Event | Pauses the game. |
    const std::string kStr_place                        = "place";                           // | Event | Change tile. |
    const std::string kStr_power                        = "power";                           // | Data Field | Returns 1 if the building has power, 0 if it doesn't. |
    const std::string kStr_powered                      = "powered";                         // | Data Field | Same as power. |
    const std::string kStr_poweroff                     = "poweroff";                        // | Data Field Trigger | Trigger when power is deactivated for a building.
    const std::string kStr_poweron                      = "poweron";                         // | Data Field Trigger | Trigger when power is activated for a building.
    const std::string kStr_powerstation                 = "powerstation";                    // | Macro | Number of Power Stations. |
    const std::string kStr_progress_path                = "progress_path";                   // | Macro | Tile id of a progress path(13). |
    const std::string kStr_qmsg                         = "qmsg";                            // | Event | Display message to user. |
    const std::string kStr_rapidrider                   = "rapidrider";                      // | Macro | Number of Rapid Riders. |
    const std::string kStr_random                       = "random";                          // | Macro | return random number, random(low)(high) |
    const std::string kStr_RechargeSeamGoal_C           = "RechargeSeamGoal_C";              // | Collection | Visible recharge seams. |
    const std::string kStr_red                          = "red";                             // | Color | Arrow colors. |
    const std::string kStr_reinforce                    = "reinforce";                       // | Trigger | Trigger when wall is reinforced. |
    const std::string kStr_reset                        = "reset";                           // | Event | Resets the player's selection |
    const std::string kStr_resetspeed                   = "resetspeed";                      // | Event | Loads the game speed from settings again. |
    const std::string kStr_resume                       = "resume";                          // | Event | Same as unpause. |
    const std::string kStr_RockMonster                  = "RockMonster";                     // | Collection | Rock Monsters. |
    const std::string kStr_row                          = "row";                             // | Data Field | Object row. |
    const std::string kStr_S                            = "S";                               // | Emerge Direction | South. |
    const std::string kStr_save                         = "save";                            // | Event | Save last miner that activated a trigger into a variable. |
    const std::string kStr_savebuilding                 = "savebuilding";                    // | Event | Save last building that activated a trigger into a variable.
    const std::string kStr_savecreature                 = "savecreature";                    // | Event | Save last creature that activated a trigger into a variable.
    const std::string kStr_savevehicle                  = "savevehicle";                     // | Event | Save last vehicle that activated a triggger into a variable.
    const std::string kStr_shake                        = "shake";                           // | Event | Shake camera. |
    const std::string kStr_showarrow                    = "showarrow";                       // | Event | Show arrow object. |
    const std::string kStr_SlimySlug                    = "SlimySlug";                       // | Collection | Slimy Slugs. |
    const std::string kStr_slug_hole                    = "slug_hole";                       // | Macro | Tile id of slimy slug hole(12). |
    const std::string kStr_slugs                        = "slugs";                           // | Macro | Number of slimy slugs. |
    const std::string kStr_smalldigger                  = "smalldigger";                     // | Macro | Number of Small Diggers. |
    const std::string kStr_SmallSpider                  = "SmallSpider";                     // | Collection | Small Spiders. |
    const std::string kStr_smalltransporttruck          = "smalltransporttruck";             // | Macro | Number of Small Transport Trucks. |
    const std::string kStr_SMLC                         = "SMLC";                            // | Macro | Number of Small Mobile Laser Cutters |
    const std::string kStr_solid_rock                   = "solid_rock";                      // | Macro | Tile ID of solid rock(38). |
    const std::string kStr_sound                        = "sound";                           // | Event | Play.ogg file. |
    const std::string kStr_spawncap                     = "spawncap";                        // | Event | Config random spawn. |
    const std::string kStr_spawnwave                    = "spawnwave";                       // | Event | Config random spawn. |
    const std::string kStr_speed                        = "speed";                           // | Event | Sets the game speed. |
    const std::string kStr_stamina                      = "stamina";                         // | Data Field | same as hp. |
    const std::string kStr_startrandomspawn             = "startrandomspawn";                // | Event | Start configured random spawn. |
    const std::string kStr_starttimer                   = "starttimer";                      // | Event | Start a timer. |
    const std::string kStr_stoprandomspawn              = "stoprandomspawn";                 // | Event | Stop random spawn. |
    const std::string kStr_stoptimer                    = "stoptimer";                       // | Event | Stop a timer. |
    const std::string kStr_Stud_C                       = "Stud_C";                          // | Collection | All building studs. |
    const std::string kStr_studs                        = "studs";                           // | Macro | Building Stud count. |
    const std::string kStr_string                       = "string";                          // | Variable | Text. |
    const std::string kStr_superteleport                = "superteleport";                   // | Macro | Number of Super Teleports. |
    const std::string kStr_supportstation               = "supportstation";                  // | Macro | Number of Support Stations. |
    const std::string kStr_teleportpad                  = "teleportpad";                     // | Macro | Numbewr of Teleport Pads. |
    const std::string kStr_tick                         = "tick";                            // | Event Chain | Called on every engine tick.Not recommended. |
    const std::string kStr_tile                         = "tile";                            // | Data Field | TileID for objectt. |
    const std::string kStr_tileid                       = "tileid";                          // | Data Field | same as tile. |
    const std::string kStr_time                         = "time";                            // | Macro / Trigger | Game time or trigger. |
    const std::string kStr_timer                        = "timer";                           // | Variable | Timer object. |
    const std::string kStr_toolstore                    = "toolstore";                       // | Macro | Returns number of toolstores. |
    const std::string kStr_truewait                     = "truewait";                        // | Event | Suspend event chain for real user time period. |
    const std::string kStr_tunnelscout                  = "tunnelscout";                     // | Macro | Number of Tunnel Scouts. |
    const std::string kStr_tunneltransport              = "tunneltransport";                 // | Macro | Number of Tunnel Transports. |
    const std::string kStr_upgrade                      = "upgrade";                         // | Trigger when vehicle is upgraded. |
    const std::string kStr_upgraded                     = "upgraded";                        // | Trigger | Not working, don't use. |`
    const std::string kStr_upgradestation               = "upgradestation";                  // | Macro | Number of Upgrade Stations. |
    const std::string kStr_unpause                      = "unpause";                         // | Event | Resumes the game if paused. |
    const std::string kStr_vehicle                      = "vehicle";                         // | Variable / Class | Vehicle object or trigger class. |
    const std::string kStr_vehicles                     = "vehicles";                        // | Macro | Number of vehicles. |
    const std::string kStr_VehicleCargoCarrier_C        = "VehicleCargoCarrier_C";           // | Collection | Cargo Carriers |
    const std::string kStr_VehicleChromeCrusher_C       = "VehicleChromeCrusher_C";          // | Collection | Chrome Crushers |
    const std::string kStr_VehicleGraniteGrinder_C      = "VehicleGraniteGrinder_C";         // | Collection | Granite Grinders |
    const std::string kStr_VehicleHoverScout_C          = "VehicleHoverScout_C";             // | Collection | Hover Scouts |
    const std::string kStr_VehicleLMLC_C                = "VehicleLMLC_C";                   // | Collection | L   arge Mobile Laser Cutters |
    const std::string kStr_VehicleLoaderDozer_C         = "VehicleLoaderDozer_C";            // | Collection | Loader Dozers |
    const std::string kStr_VehicleRapidRider_C          = "VehicleRapidRider_C";             // | Collection | Rapid Riders |
    const std::string kStr_VehicleSmallDigger_C         = "VehicleSmallDigger_C";            // | Collection | Small Diggers |
    const std::string kStr_VehicleSmallTransportTruck_C = "VehicleSmallTransportTruck_C";    // | Collection | Small Transport Trucks |
    const std::string kStr_VehicleSMLC_C                = "VehicleSMLC_C";                   // | Collection | Small Mobile Laser Cutter |
    const std::string kStr_VehicleTunnelScout_C         = "VehicleTunnelScout_C";            // | Collection | Tunnel Scouts |
    const std::string kStr_VehicleTunnelTransport_C     = "VehicleTunnelTransport_C";        // | Collection | Tunnel Transports |
    const std::string kStr_wait                         = "wait";                            // | Event | Suspend event chain for a given period of time modified by game speed. | me speed. |
    const std::string kStr_walk                         = "walk";                            // | Trigger | Trigger when miner walks on a tile. |
    const std::string kStr_water                        = "water";                           // | Macro | Tile ID of water(11). |
    const std::string kStr_W                            = "W";                               // | Emerge Direction | West. |
    const std::string kStr_when                         = "when";                            // | Occurance | multiple time trigger. |
    const std::string kStr_white                        = "white";                           // | Color | Arrow colors. |
    const std::string kStr_win                          = "win";                             // | Event | Win the map. |
    const std::string kStr_X                            = "X";                               // | Data Field | Column, 300 values per cell |
    const std::string kStr_Y                            = "Y";                               // | Data Field | Row, 300 values per cell |
    const std::string kStr_yellow                       = "yellow";                          // | Colors | Arrow colors. |
    const std::string kStr_Z                            = "Z";                               // | Data Field | Height, 300 values per cell |

    // script reserved words converted to lower case
    const std::string kS_A                            = MMUtil::toLower( kStr_A                            );   // emerge direction
    const std::string kS_addrandomspawn               = MMUtil::toLower( kStr_addrandomspawn               );   // event
    const std::string kS_air                          = MMUtil::toLower( kStr_air                          );   // macro int, amount of air
    const std::string kS_arrow                        = MMUtil::toLower( kStr_arrow                        );   // variable type
    const std::string kS_Barrier_C                    = MMUtil::toLower( kStr_Barrier_C                    );   // collection, building
    const std::string kS_Bat                          = MMUtil::toLower( kStr_Bat                          );   // collection, creature
    const std::string kS_black                        = MMUtil::toLower( kStr_black                        );   // arrow color
    const std::string kS_blue                         = MMUtil::toLower( kStr_blue                         );   // arrow color
    const std::string kS_bool                         = MMUtil::toLower( kStr_bool                         );   // variable type
    const std::string kS_building                     = MMUtil::toLower( kStr_building                     );   // variable type or collection, building
    const std::string kS_building_path                = MMUtil::toLower( kStr_building_path                );   // macro int, number of objects
    const std::string kS_BuildingCanteen_C            = MMUtil::toLower( kStr_BuildingCanteen_C            );   // collection, building
    const std::string kS_BuildingDocks_C              = MMUtil::toLower( kStr_BuildingDocks_C              );   // collection, building
    const std::string kS_BuildingElectricFence_C      = MMUtil::toLower( kStr_BuildingElectricFence_C      );   // collection, building
    const std::string kS_BuildingGeologicalCenter_C   = MMUtil::toLower( kStr_BuildingGeologicalCenter_C   );   // collection, building
    const std::string kS_BuildingMiningLaser_C        = MMUtil::toLower( kStr_BuildingMiningLaser_C        );   // collection, building
    const std::string kS_BuildingOreRefinery_C        = MMUtil::toLower( kStr_BuildingOreRefinery_C        );   // collection, building
    const std::string kS_BuildingPowerPath_C          = MMUtil::toLower( kStr_BuildingPowerPath_C          );   // collection, building
    const std::string kS_BuildingPowerStation_C       = MMUtil::toLower( kStr_BuildingPowerStation_C       );   // collection, building
    const std::string kS_buildings                    = MMUtil::toLower( kStr_buildings                    );   // macro int, number of buildings.
    const std::string kS_BuildingSuperTeleport_C      = MMUtil::toLower( kStr_BuildingSuperTeleport_C      );   // collection, building
    const std::string kS_BuildingSupportStation_C     = MMUtil::toLower( kStr_BuildingSupportStation_C     );   // collection, building
    const std::string kS_BuildingTeleportPad_C        = MMUtil::toLower( kStr_BuildingTeleportPad_C        );   // collection, building
    const std::string kS_BuildingToolStore_C          = MMUtil::toLower( kStr_BuildingToolStore_C          );   // collection, building
    const std::string kS_BuildingUpgradeStation_C     = MMUtil::toLower( kStr_BuildingUpgradeStation_C     );   // collection, building
    const std::string kS_built                        = MMUtil::toLower( kStr_built                        );   // data field trigger
    const std::string kS_canteen                      = MMUtil::toLower( kStr_canteen                      );   // macro, int, number of Canteens.
    const std::string kS_cargocarrier                 = MMUtil::toLower( kStr_cargocarrier                 );   // macro, int number of Cargo Carriers.
    const std::string kS_change                       = MMUtil::toLower( kStr_change                       );   // trigger
    const std::string kS_chromecrusher                = MMUtil::toLower( kStr_chromecrusher                );   // macro int, number of Chrome Crushers
    const std::string kS_click                        = MMUtil::toLower( kStr_click                        );   // Data Field Trigger
    const std::string kS_clock                        = MMUtil::toLower( kStr_clock                        );   // macro, float, return game time
    const std::string kS_col                          = MMUtil::toLower( kStr_col                          );   // Data Field int, object column
    const std::string kS_column                       = MMUtil::toLower( kStr_column                       );   // data field int, object column
    const std::string kS_ConstructedBuilding          = MMUtil::toLower( kStr_ConstructedBuilding          );   // macro, bu1                                                                                                                           ilding, Last constructed building
    const std::string kS_creature                     = MMUtil::toLower( kStr_creature                     );   // variable type
    const std::string kS_CreatureBat_C                = MMUtil::toLower( kStr_CreatureBat_C                );   // collection, creature
    const std::string kS_CreatureIceMonster_C         = MMUtil::toLower( kStr_CreatureIceMonster_C         );   // collection, creature
    const std::string kS_CreatureLavaMonster_C        = MMUtil::toLower( kStr_CreatureLavaMonster_C        );   // collection, creature
    const std::string kS_CreatureRockMonster_C        = MMUtil::toLower( kStr_CreatureRockMonster_C        );   // collection, creature
    const std::string kS_CreatureSmallSpider_C        = MMUtil::toLower( kStr_CreatureSmallSpider_C        );   // collection, creature
    const std::string kS_CreatureSlimySlug_C          = MMUtil::toLower( kStr_CreatureSlimySlug_C          );   // collection, creature
    const std::string kS_creatures                    = MMUtil::toLower( kStr_creatures                    );   // macro, int, Number of creatures
    const std::string kS_Crystal_C                    = MMUtil::toLower( kStr_Crystal_C                    );   // collection, unstored crystals|
    const std::string kS_crystal_seam                 = MMUtil::toLower( kStr_crystal_seam                 );   // macro, int, id of crystal seam always 42
    const std::string kS_crystals                     = MMUtil::toLower( kStr_crystals                     );   // macro, int, number of stored crystals
    const std::string kS_darkgreen                    = MMUtil::toLower( kStr_darkgreen                    );   // arrow color
    const std::string kS_dead                         = MMUtil::toLower( kStr_dead                         );   // Data Field Trigger, Trigger when object is dead.
    const std::string kS_dirt                         = MMUtil::toLower( kStr_dirt                         );   // Macro, Tile ID of dirt (26).
    const std::string kS_disable                      = MMUtil::toLower( kStr_disable                      );   // Event, Disable object.
    const std::string kS_docks                        = MMUtil::toLower( kStr_docks                        );   // Macro, Number of Docks.
    const std::string kS_drill                        = MMUtil::toLower( kStr_drill                        );   // Event / Trigger, Drill tile.
    const std::string kS_drive                        = MMUtil::toLower( kStr_drive                        );   // Trigger, Trigger when vehicle over tile.
    const std::string kS_driven                       = MMUtil::toLower( kStr_driven                       );   // Data Field Trigger|Trigger when a miner enters a vehicle.
    const std::string kS_driver                       = MMUtil::toLower( kStr_driver                       );   // Data Field, miner id of the driver.Same as driverid.
    const std::string kS_driverid                     = MMUtil::toLower( kStr_driverid                     );   // Data Field, miner id of the driver.Same as driver.
    const std::string kS_Dynamite_C                   = MMUtil::toLower( kStr_Dynamite_C                   );   // Collection, All dynamite outside of toolstore.
    const std::string kS_E                            = MMUtil::toLower( kStr_E                            );   // Emerge Direction, East.
    const std::string kS_eaten                        = MMUtil::toLower( kStr_eaten                        );   // Data Field, The number of crystals eaten / absorbed.
    const std::string kS_electricfence                = MMUtil::toLower( kStr_electricfence                );   // Macro, Number of Fences.
    const std::string kS_ElectricFence_C              = MMUtil::toLower( kStr_ElectricFence_C              );   // Macro | Number of Fence objects.Not a collection. |
    const std::string kS_emerge                       = MMUtil::toLower( kStr_emerge                       );   // | Event | Spawn creature. |
    const std::string kS_enable                       = MMUtil::toLower( kStr_enable                       );   // | Event | Enable object |
    const std::string kS_enter                        = MMUtil::toLower( kStr_enter                        );   // | Trigger | Trigger when object enters tile. |
    const std::string kS_EventErosion_C               = MMUtil::toLower( kStr_EventErosion_C               );   // | Collection | Active erosions. |
    const std::string kS_EventLandslide_C             = MMUtil::toLower( kStr_EventLandslide_C             );   // | Collection | Active active landslides. |
    const std::string kS_erosionscale                 = MMUtil::toLower( kStr_erosionscale                 );   // | Macro | Global erosion scale factor |
    const std::string kS_float                        = MMUtil::toLower( kStr_float                        );   // | Variable | Floating point number. |
    const std::string kS_geologicalcenter             = MMUtil::toLower( kStr_geologicalcenter             );   // | Macro | Number of Geological Centers. |
    const std::string kS_get                          = MMUtil::toLower( kStr_get                          );   // | Macro | Get tile ID. |
    const std::string kS_granitegrinder               = MMUtil::toLower( kStr_granitegrinder               );   // | Macro | Number of Granite Grinders. |
    const std::string kS_green                        = MMUtil::toLower( kStr_green                        );   // | Color | Arrow colors. |
    const std::string kS_hard_rock                    = MMUtil::toLower( kStr_hard_rock                    );   // | Macro | Tile ID of hard rock(34). |
    const std::string kS_heal                         = MMUtil::toLower( kStr_heal                         );   // | Event | Heal object. |
    const std::string kS_health                       = MMUtil::toLower( kStr_health                       );   // | Data Field | Object hitpoints.Same has hp. |
    const std::string kS_hidearrow                    = MMUtil::toLower( kStr_hidearrow                    );   // | Event | Hide arrow object. |
    const std::string kS_highlight                    = MMUtil::toLower( kStr_highlight                    );   // | Event | Highlight tile. |
    const std::string kS_highlightarrow               = MMUtil::toLower( kStr_highlightarrow               );   // | [Event] | showarrow and highlight at once. |
    const std::string kS_hostiles                     = MMUtil::toLower( kStr_hostiles                     );   // | Macro | Number of hostile creatures. |
    const std::string kS_hover                        = MMUtil::toLower( kStr_hover                        );   // | Trigger | Trigger when mouse is over a tile. |
    const std::string kS_hoverscout                   = MMUtil::toLower( kStr_hoverscout                   );   // | Macro | Number of Hover Scouts. |
    const std::string kS_hp                           = MMUtil::toLower( kStr_hp                           );   // | Data Field | Object hitpoints.Same has health. |
    const std::string kS_hurt                         = MMUtil::toLower( kStr_hurt                         );   // | Data Field Trigger | Trigger when object takes damage. |
    const std::string kS_IceMonster                   = MMUtil::toLower( kStr_IceMonster                   );   // | Collection | Ice Monsters. |
    const std::string kS_id                           = MMUtil::toLower( kStr_id                           );   // | Data Field | Object ID. |
    const std::string kS_if                           = MMUtil::toLower( kStr_if                           );   // | Occurance | single time trigger. |
    const std::string kS_init                         = MMUtil::toLower( kStr_init                         );   // | Event Chain | First event called after map load.
    const std::string kS_int                          = MMUtil::toLower( kStr_int                          );   // | Variable | Integer number. |
    const std::string kS_intarray                     = MMUtil::toLower( kStr_intarray                     );   // | Variable | Integer array. |
    const std::string kS_ispowered                    = MMUtil::toLower( kStr_ispowered                    );   // | Data Field | Same as power. |
    const std::string kS_kill                         = MMUtil::toLower( kStr_kill                         );   // | Event | Kill object. |
    const std::string kS_landslide                    = MMUtil::toLower( kStr_landslide                    );   // | Event | Unknown. |
    const std::string kS_laser                        = MMUtil::toLower( kStr_laser                        );   // | Trigger | Trigger when wall destroyed by laser. |
    const std::string kS_laserhit                     = MMUtil::toLower( kStr_laserhit                     );   // | Trigger | Trigger when wall hit by laser. |
    const std::string kS_lastbuilding                 = MMUtil::toLower( kStr_lastbuilding                 );   // | Macro | Last building that activated a trigger. |
    const std::string kS_lastcreature                 = MMUtil::toLower( kStr_lastcreature                 );   // | Macro | Last creature that activated a trigger. |
    const std::string kS_lastminer                    = MMUtil::toLower( kStr_lastminer                    );   // | Macro | Last miner that activated a trigger. |
    const std::string kS_lastvehicle                  = MMUtil::toLower( kStr_lastvehicle                  );   // | Macro | Last vehicle that activated a trigger. |
    const std::string kS_lava                         = MMUtil::toLower( kStr_lava                         );   // | Macro | Tile ID of lava(6). |
    const std::string kS_LavaMonster                  = MMUtil::toLower( kStr_LavaMonster                  );   // | Collection | Lava Monsters. |
    const std::string kS_level                        = MMUtil::toLower( kStr_level                        );   // | Data Field | Returns upgrade level of the building. |
    const std::string kS_light                        = MMUtil::toLower( kStr_light                        );   // | Parameter | enable / disable parameter. |
    const std::string kS_lights                       = MMUtil::toLower( kStr_lights                       );   // | Parameter | same as light. |
    const std::string kS_LMLC                         = MMUtil::toLower( kStr_LMLC                         );   // | Macro | Number of Large Mobile Laser Cutters. |
    const std::string kS_loaderdozer                  = MMUtil::toLower( kStr_loaderdozer                  );   // | Macro | Number of Loader Dozers. |
    const std::string kS_loose_rock                   = MMUtil::toLower( kStr_loose_rock                   );   // | Macro | Tile ID of loose rock(30). |
    const std::string kS_lose                         = MMUtil::toLower( kStr_lose                         );   // | Event | Lose the map. |
    const std::string kS_miner                        = MMUtil::toLower( kStr_miner                        );   // | Variable / Class | Miner object or class. |
    const std::string kS_miners                       = MMUtil::toLower( kStr_miners                       );   // | Macro | Miners discovered or active. |
    const std::string kS_mininglaser                  = MMUtil::toLower( kStr_mininglaser                  );   // | Macro | Number of Mining Lasers. |
    const std::string kS_monsters                     = MMUtil::toLower( kStr_monsters                     );   // | Macro | Number of monsters. |
    const std::string kS_msg                          = MMUtil::toLower( kStr_msg                          );   // | Event | Display a message to user. |
    const std::string kS_N                            = MMUtil::toLower( kStr_N                            );   // | Emerge Direction | North. |
    const std::string kS_NavModifierLava_C            = MMUtil::toLower( kStr_NavModifierLava_C            );   // | Collection | Amount of lava tiles. | NavModifierLava_C | Collection | Amount of lava tiles. |
    const std::string kS_NavModifierPowerPath_C       = MMUtil::toLower( kStr_NavModifierPowerPath_C       );   // | Collection | Amount of Power Path tiles, any type.NavModifierPowerPath_C | Collection | Amount of Power Path tiles, any type.Only finished patOnly finished paths. |
    const std::string kS_NavModifierRubble_C          = MMUtil::toLower( kStr_NavModifierRubble_C          );   // | Collection | Amount of Rubble tiles, any stage. |
    const std::string kS_NavModifierWater_C           = MMUtil::toLower( kStr_NavModifierWater_C           );   // | Collection | Amount of water tiles. |
    const std::string kS_new                          = MMUtil::toLower( kStr_new                          );   // | Data Field Trigger | Trigger when object is created. |
    const std::string kS_ore                          = MMUtil::toLower( kStr_ore                          );   // | Macro | Ore count. |
    const std::string kS_Ore_C                        = MMUtil::toLower( kStr_Ore_C                        );   // | Collection | All ore. |
    const std::string kS_orerefinery                  = MMUtil::toLower( kStr_orerefinery                  );   // | Macro | Number of Ore Refineries. |
    const std::string kS_ore_seam                     = MMUtil::toLower( kStr_ore_seam                     );   // | Macro | Tile ID of an ore seam(46). |
    const std::string kS_pan                          = MMUtil::toLower( kStr_pan                          );   // | Event | Pan camera. |
    const std::string kS_pause                        = MMUtil::toLower( kStr_pause                        );   // | Event | Pauses the game. |
    const std::string kS_place                        = MMUtil::toLower( kStr_place                        );   // | Event | Change tile. |
    const std::string kS_power                        = MMUtil::toLower( kStr_power                        );   // | Data Field | Returns 1 if the building has power, 0 if it doesn't. |
    const std::string kS_powered                      = MMUtil::toLower( kStr_powered                      );   // | Data Field | Same as power. |
    const std::string kS_poweroff                     = MMUtil::toLower( kStr_poweroff                     );   // | Data Field Trigger | Trigger when power is deactivated for a building.
    const std::string kS_poweron                      = MMUtil::toLower( kStr_poweron                      );   // | Data Field Trigger | Trigger when power is activated for a building.
    const std::string kS_powerstation                 = MMUtil::toLower( kStr_powerstation                 );   // | Macro | Number of Power Stations. |
    const std::string kS_progress_path                = MMUtil::toLower( kStr_progress_path                );   // | Macro | Tile id of a progress path(13). |
    const std::string kS_qmsg                         = MMUtil::toLower( kStr_qmsg                         );   // | Event | Display message to user. |
    const std::string kS_random                       = MMUtil::toLower( kStr_random                       );   // | Macro | return random number, random(low)(high) |
    const std::string kS_rapidrider                   = MMUtil::toLower( kStr_rapidrider                   );   // | Macro | Number of Rapid Riders. |
    const std::string kS_RechargeSeamGoal_C           = MMUtil::toLower( kStr_RechargeSeamGoal_C           );   // | Collection | Visible recharge seams. |
    const std::string kS_red                          = MMUtil::toLower( kStr_red                          );   // | Color | Arrow colors. |
    const std::string kS_reinforce                    = MMUtil::toLower( kStr_reinforce                    );   // | Trigger | Trigger when wall is reinforced. |
    const std::string kS_reset                        = MMUtil::toLower( kStr_reset                        );   // | Event | Resets the player's selection |
    const std::string kS_resetspeed                   = MMUtil::toLower( kStr_resetspeed                   );   // | Event | Loads the game speed from settings again. |
    const std::string kS_resume                       = MMUtil::toLower( kStr_resume                       );   // | Event | Same as unpause. |
    const std::string kS_RockMonster                  = MMUtil::toLower( kStr_RockMonster                  );   // | Collection | Rock Monsters. |
    const std::string kS_row                          = MMUtil::toLower( kStr_row                          );   // | Data Field | Object row. |
    const std::string kS_S                            = MMUtil::toLower( kStr_S                            );   // | Emerge Direction | South. |
    const std::string kS_save                         = MMUtil::toLower( kStr_save                         );   // | Event | Save last miner that activated a trigger into a variable. |
    const std::string kS_savebuilding                 = MMUtil::toLower( kStr_savebuilding                 );   // | Event | Save last building that activated a trigger into a variable.
    const std::string kS_savecreature                 = MMUtil::toLower( kStr_savecreature                 );   // | Event | Save last creature that activated a trigger into a variable.
    const std::string kS_savevehicle                  = MMUtil::toLower( kStr_savevehicle                  );   // | Event | Save last vehicle that activated a triggger into a variable.
    const std::string kS_shake                        = MMUtil::toLower( kStr_shake                        );   // | Event | Shake camera. |
    const std::string kS_showarrow                    = MMUtil::toLower( kStr_showarrow                    );   // | Event | Show arrow object. |
    const std::string kS_SlimySlug                    = MMUtil::toLower( kStr_SlimySlug                    );   // | Collection | Slimy Slugs. |
    const std::string kS_slug_hole                    = MMUtil::toLower( kStr_slug_hole                    );   // | Macro | Tile id of slimy slug hole(12). |
    const std::string kS_slugs                        = MMUtil::toLower( kStr_slugs                        );   // | Macro | Number of slimy slugs. |
    const std::string kS_smalldigger                  = MMUtil::toLower( kStr_smalldigger                  );   // | Macro | Number of Small Diggers. |
    const std::string kS_SmallSpider                  = MMUtil::toLower( kStr_SmallSpider                  );   // | Collection | Small Spiders. |
    const std::string kS_smalltransporttruck          = MMUtil::toLower( kStr_smalltransporttruck          );   // | Macro | Number of Small Transport Trucks. |
    const std::string kS_SMLC                         = MMUtil::toLower( kStr_SMLC                         );   // | Macro | Number of Small Mobile Laser Cutters |
    const std::string kS_solid_rock                   = MMUtil::toLower( kStr_solid_rock                   );   // | Macro | Tile ID of solid rock(38). |
    const std::string kS_sound                        = MMUtil::toLower( kStr_sound                        );   // | Event | Play.ogg file. |
    const std::string kS_spawncap                     = MMUtil::toLower( kStr_spawncap                     );   // | Event | Config random spawn. |
    const std::string kS_spawnwave                    = MMUtil::toLower( kStr_spawnwave                    );   // | Event | Config random spawn. |
    const std::string kS_speed                        = MMUtil::toLower( kStr_speed                        );   // | Event | Sets the game speed. |
    const std::string kS_stamina                      = MMUtil::toLower( kStr_stamina                      );   // | Data Field | same as hp. |
    const std::string kS_startrandomspawn             = MMUtil::toLower( kStr_startrandomspawn             );   // | Event | Start configured random spawn. |
    const std::string kS_starttimer                   = MMUtil::toLower( kStr_starttimer                   );   // | Event | Start a timer. |
    const std::string kS_stoprandomspawn              = MMUtil::toLower( kStr_stoprandomspawn              );   // | Event | Stop random spawn. |
    const std::string kS_stoptimer                    = MMUtil::toLower( kStr_stoptimer                    );   // | Event | Stop a timer. |
    const std::string kS_Stud_C                       = MMUtil::toLower( kStr_Stud_C                       );   // | Collection | All building studs. |
    const std::string kS_studs                        = MMUtil::toLower( kStr_studs                        );   // | Macro | Building Stud count. |
    const std::string kS_string                       = MMUtil::toLower( kStr_string                       );   // | Variable | Text. |
    const std::string kS_superteleport                = MMUtil::toLower( kStr_superteleport                );   // | Macro | Number of Super Teleports. |
    const std::string kS_supportstation               = MMUtil::toLower( kStr_supportstation               );   // | Macro | Number of Support Stations. |
    const std::string kS_teleportpad                  = MMUtil::toLower( kStr_teleportpad                  );   // | Macro | Numbewr of Teleport Pads. |
    const std::string kS_tick                         = MMUtil::toLower( kStr_tick                         );   // | Event Chain | Called on every engine tick.Not recommended. |
    const std::string kS_tile                         = MMUtil::toLower( kStr_tile                         );   // | Data Field | TileID for objectt. |
    const std::string kS_tileid                       = MMUtil::toLower( kStr_tileid                       );   // | Data Field | same as tile. |
    const std::string kS_time                         = MMUtil::toLower( kStr_time                         );   // | Macro / Trigger | Game time or trigger. |
    const std::string kS_timer                        = MMUtil::toLower( kStr_timer                        );   // | Variable | Timer object. |
    const std::string kS_toolstore                    = MMUtil::toLower( kStr_toolstore                    );   // | Macro | Returns number of toolstores. |
    const std::string kS_truewait                     = MMUtil::toLower( kStr_truewait                     );   // | Event | Suspend event chain for real user time period. |
    const std::string kS_tunnelscout                  = MMUtil::toLower( kStr_tunnelscout                  );   // | Macro | Number of Tunnel Scouts. |
    const std::string kS_tunneltransport              = MMUtil::toLower( kStr_tunneltransport              );   // | Macro | Number of Tunnel Transports. |
    const std::string kS_upgrade                      = MMUtil::toLower( kStr_upgrade                      );   // | Trigger when vehicle is upgraded. |
    const std::string kS_upgraded                     = MMUtil::toLower( kStr_upgraded                     );   // | Trigger | Not working, don't use. |
    const std::string kS_upgradestation               = MMUtil::toLower( kStr_upgradestation               );   // | Macro | Number of Upgrade Stations. |
    const std::string kS_unpause                      = MMUtil::toLower( kStr_unpause                      );   // | Event | Resumes the game if paused. |
    const std::string kS_vehicle                      = MMUtil::toLower( kStr_vehicle                      );   // | Variable / Class | Vehicle object or trigger class. |
    const std::string kS_vehicles                     = MMUtil::toLower( kStr_vehicles                     );   // | Macro | Number of vehicles. |
    const std::string kS_VehicleCargoCarrier_C        = MMUtil::toLower( kStr_VehicleCargoCarrier_C        );   // | Collection | Cargo Carriers |
    const std::string kS_VehicleChromeCrusher_C       = MMUtil::toLower( kStr_VehicleChromeCrusher_C       );   // | Collection | Chrome Crushers |
    const std::string kS_VehicleGraniteGrinder_C      = MMUtil::toLower( kStr_VehicleGraniteGrinder_C      );   // | Collection | Granite Grinders |
    const std::string kS_VehicleHoverScout_C          = MMUtil::toLower( kStr_VehicleHoverScout_C          );   // | Collection | Hover Scouts |
    const std::string kS_VehicleLMLC_C                = MMUtil::toLower( kStr_VehicleLMLC_C                );   // | Collection | L   arge Mobile Laser Cutters |
    const std::string kS_VehicleLoaderDozer_C         = MMUtil::toLower( kStr_VehicleLoaderDozer_C         );   // | Collection | Loader Dozers |
    const std::string kS_VehicleRapidRider_C          = MMUtil::toLower( kStr_VehicleRapidRider_C          );   // | Collection | Rapid Riders |
    const std::string kS_VehicleSmallDigger_C         = MMUtil::toLower( kStr_VehicleSmallDigger_C         );   // | Collection | Small Diggers |
    const std::string kS_VehicleSmallTransportTruck_C = MMUtil::toLower( kStr_VehicleSmallTransportTruck_C );   // | Collection | Small Transport Trucks |
    const std::string kS_VehicleSMLC_C                = MMUtil::toLower( kStr_VehicleSMLC_C                );   // | Collection | Small Mobile Laser Cutter |
    const std::string kS_VehicleTunnelScout_C         = MMUtil::toLower( kStr_VehicleTunnelScout_C         );   // | Collection | Tunnel Scouts |
    const std::string kS_VehicleTunnelTransport_C     = MMUtil::toLower( kStr_VehicleTunnelTransport_C     );   // | Collection | Tunnel Transports |
    const std::string kS_wait                         = MMUtil::toLower( kStr_wait                         );   // | Event | Suspend event chain for a given period of time modified by game speed. | me speed. |
    const std::string kS_walk                         = MMUtil::toLower( kStr_walk                         );   // | Trigger | Trigger when miner walks on a tile. |
    const std::string kS_water                        = MMUtil::toLower( kStr_water                        );   // | Macro | Tile ID of water(11). |
    const std::string kS_W                            = MMUtil::toLower( kStr_W                            );   // | Emerge Direction | West. |
    const std::string kS_when                         = MMUtil::toLower( kStr_when                         );   // | Occurance | multiple time trigger. |
    const std::string kS_white                        = MMUtil::toLower( kStr_white                        );   // | Color | Arrow colors. |
    const std::string kS_win                          = MMUtil::toLower( kStr_win                          );   // | Event | Win the map. |
    const std::string kS_X                            = MMUtil::toLower( kStr_X                            );   // | Data Field | Column, 300 values per cell |
    const std::string kS_Y                            = MMUtil::toLower( kStr_Y                            );   // | Data Field | Row, 300 values per cell |
    const std::string kS_yellow                       = MMUtil::toLower( kStr_yellow                       );   // | Colors | Arrow colors. |
    const std::string kS_Z                            = MMUtil::toLower( kStr_Z                            );   // | Data Field | Height, 300 values per cell |

    // map of reserved words mapping lower case to friendly name.
    // variable names and event chains may not have any of these.
    // init and tick are not added since the script developer provides those optional event chains.
    // thus variable names are also checked for init and tick since they are event chain only.
    typedef std::unordered_map<std::string, std::string> ReservedWordsType;


    // map contains all reserved words, key is lower case, value is friendly name
    const ReservedWordsType ReservedWordsMap =
    {
        { kS_A                            , kStr_A                            },   // emerge direction
        { kS_addrandomspawn               , kStr_addrandomspawn               },   // event
        { kS_air                          , kStr_air                          },   // macro int, amount of air
        { kS_arrow                        , kStr_arrow                        },   // variable type
        { kS_Barrier_C                    , kStr_Barrier_C                    },   // collection, building
        { kS_Bat                          , kStr_Bat                          },   // collection, creature
        { kS_black                        , kStr_black                        },   // arrow color
        { kS_blue                         , kStr_blue                         },   // arrow color
        { kS_bool                         , kStr_bool                         },   // variable type
        { kS_building                     , kStr_building                     },   // variable type or collection, building
        { kS_building_path                , kStr_building_path                },   // macro int, number of objects
        { kS_BuildingCanteen_C            , kStr_BuildingCanteen_C            },   // collection, building
        { kS_BuildingDocks_C              , kStr_BuildingDocks_C              },   // collection, building
        { kS_BuildingElectricFence_C      , kStr_BuildingElectricFence_C      },   // collection, building
        { kS_BuildingGeologicalCenter_C   , kStr_BuildingGeologicalCenter_C   },   // collection, building
        { kS_BuildingMiningLaser_C        , kStr_BuildingMiningLaser_C        },   // collection, building
        { kS_BuildingOreRefinery_C        , kStr_BuildingOreRefinery_C        },   // collection, building
        { kS_BuildingPowerPath_C          , kStr_BuildingPowerPath_C          },   // collection, building
        { kS_BuildingPowerStation_C       , kStr_BuildingPowerStation_C       },   // collection, building
        { kS_buildings                    , kStr_buildings                    },   // macro int, number of buildings.
        { kS_BuildingSuperTeleport_C      , kStr_BuildingSuperTeleport_C      },   // collection, building
        { kS_BuildingSupportStation_C     , kStr_BuildingSupportStation_C     },   // collection, building
        { kS_BuildingTeleportPad_C        , kStr_BuildingTeleportPad_C        },   // collection, building
        { kS_BuildingToolStore_C          , kStr_BuildingToolStore_C          },   // collection, building
        { kS_BuildingUpgradeStation_C     , kStr_BuildingUpgradeStation_C     },   // collection, building
        { kS_built                        , kStr_built                        },   // data field trigger
        { kS_canteen                      , kStr_canteen                      },   // macro, int, number of Canteens.
        { kS_cargocarrier                 , kStr_cargocarrier                 },   // macro, int number of Cargo Carriers.
        { kS_change                       , kStr_change                       },   // trigger
        { kS_chromecrusher                , kStr_chromecrusher                },   // macro int, number of Chrome Crushers
        { kS_click                        , kStr_click                        },   // Data Field Trigger
        { kS_clock                        , kStr_clock                        },   // macro, float, return game time
        { kS_col                          , kStr_col                          },   // Data Field int, object column
        { kS_column                       , kStr_column                       },   // data field int, object column
        { kS_ConstructedBuilding          , kStr_ConstructedBuilding          },   // macro, bu1                                                                                                                           ilding, Last constructed building
        { kS_creature                     , kStr_creature                     },   // variable type
        { kS_CreatureBat_C                , kStr_CreatureBat_C                },   // collection, creature
        { kS_CreatureIceMonster_C         , kStr_CreatureIceMonster_C         },   // collection, creature
        { kS_CreatureLavaMonster_C        , kStr_CreatureLavaMonster_C        },   // collection, creature
        { kS_CreatureRockMonster_C        , kStr_CreatureRockMonster_C        },   // collection, creature
        { kS_CreatureSmallSpider_C        , kStr_CreatureSmallSpider_C        },   // collection, creature
        { kS_CreatureSlimySlug_C          , kStr_CreatureSlimySlug_C          },   // collection, creature
        { kS_creatures                    , kStr_creatures                    },   // macro, int, Number of creatures
        { kS_Crystal_C                    , kStr_Crystal_C                    },   // collection, unstored crystals|
        { kS_crystal_seam                 , kStr_crystal_seam                 },   // macro, int, id of crystal seam always 42
        { kS_crystals                     , kStr_crystals                     },   // macro, int, number of stored crystals
        { kS_darkgreen                    , kStr_darkgreen                    },   // arrow color
        { kS_dead                         , kStr_dead                         },   // Data Field Trigger, Trigger when object is dead.
        { kS_dirt                         , kStr_dirt                         },   // Macro, Tile ID of dirt (26).
        { kS_disable                      , kStr_disable                      },   // Event, Disable object.
        { kS_docks                        , kStr_docks                        },   // Macro, Number of Docks.
        { kS_drill                        , kStr_drill                        },   // Event / Trigger, Drill tile.
        { kS_drive                        , kStr_drive                        },   // Trigger, Trigger when vehicle over tile.
        { kS_driven                       , kStr_driven                       },   // Data Field Trigger|Trigger when a miner enters a vehicle.
        { kS_driver                       , kStr_driver                       },   // Data Field, miner id of the driver.Same as driverid.
        { kS_driverid                     , kStr_driverid                     },   // Data Field, miner id of the driver.Same as driver.
        { kS_Dynamite_C                   , kStr_Dynamite_C                   },   // Collection, All dynamite outside of toolstore.
        { kS_E                            , kStr_E                            },   // Emerge Direction, East.
        { kS_eaten                        , kStr_eaten                        },   // Data Field, The number of crystals eaten / absorbed.
        { kS_electricfence                , kStr_electricfence                },   // Macro, Number of Fences.
        { kS_ElectricFence_C              , kStr_ElectricFence_C              },   // Macro | Number of Fence objects.Not a collection. |
        { kS_emerge                       , kStr_emerge                       },   // | Event | Spawn creature. |
        { kS_enable                       , kStr_enable                       },   // | Event | Enable object |
        { kS_enter                        , kStr_enter                        },   // | Trigger | Trigger when object enters tile. |
        { kS_EventErosion_C               , kStr_EventErosion_C               },   // | Collection | Active erosions. |
        { kS_EventLandslide_C             , kStr_EventLandslide_C             },   // | Collection | Active active landslides. |
        { kS_erosionscale                 , kStr_erosionscale                 },   // | Macro | Global erosion scale factor |
        { kS_float                        , kStr_float                        },   // | Variable | Floating point number. |
        { kS_geologicalcenter             , kStr_geologicalcenter             },   // | Macro | Number of Geological Centers. |
        { kS_get                          , kStr_get                          },   // | Macro | Get tile ID. |
        { kS_granitegrinder               , kStr_granitegrinder               },   // | Macro | Number of Granite Grinders. |
        { kS_green                        , kStr_green                        },   // | Color | Arrow colors. |
        { kS_hard_rock                    , kStr_hard_rock                    },   // | Macro | Tile ID of hard rock(34). |
        { kS_heal                         , kStr_heal                         },   // | Event | Heal object. |
        { kS_health                       , kStr_health                       },   // | Data Field | Object hitpoints.Same has hp. |
        { kS_hidearrow                    , kStr_hidearrow                    },   // | Event | Hide arrow object. |
        { kS_highlight                    , kStr_highlight                    },   // | Event | Highlight tile. |
        { kS_highlightarrow               , kStr_highlightarrow               },   // | [Event] | showarrow and highlight at once. |
        { kS_hostiles                     , kStr_hostiles                     },   // | Macro | Number of hostile creatures. |
        { kS_hover                        , kStr_hover                        },   // | Trigger | Trigger when mouse is over a tile. |
        { kS_hoverscout                   , kStr_hoverscout                   },   // | Macro | Number of Hover Scouts. |
        { kS_hp                           , kStr_hp                           },   // | Data Field | Object hitpoints.Same has health. |
        { kS_hurt                         , kStr_hurt                         },   // | Data Field Trigger | Trigger when object takes damage. |
        { kS_IceMonster                   , kStr_IceMonster                   },   // | Collection | Ice Monsters. |
        { kS_id                           , kStr_id                           },   // | Data Field | Object ID. |
        { kS_if                           , kStr_if                           },   // | Occurance | single time trigger. |
        { kS_init                         , kStr_init                         },   // | Event Chain | First event called after map load.
        { kS_int                          , kStr_int                          },   // | Variable | Integer number. |
        { kS_intarray                     , kStr_intarray                     },   // | Variable | Integer array. |
        { kS_ispowered                    , kStr_ispowered                    },   // | Data Field | Same as power. |
        { kS_kill                         , kStr_kill                         },   // | Event | Kill object. |
        { kS_landslide                    , kStr_landslide                    },   // | Event | Unknown. |
        { kS_laser                        , kStr_laser                        },   // | Trigger | Trigger when wall destroyed by laser. |
        { kS_laserhit                     , kStr_laserhit                     },   // | Trigger | Trigger when wall hit by laser. |
        { kS_lastbuilding                 , kStr_lastbuilding                 },   // | Macro | Last building that activated a trigger. |
        { kS_lastcreature                 , kStr_lastcreature                 },   // | Macro | Last creature that activated a trigger. |
        { kS_lastminer                    , kStr_lastminer                    },   // | Macro | Last miner that activated a trigger. |
        { kS_lastvehicle                  , kStr_lastvehicle                  },   // | Macro | Last vehicle that activated a trigger. |
        { kS_lava                         , kStr_lava                         },   // | Macro | Tile ID of lava(6). |
        { kS_LavaMonster                  , kStr_LavaMonster                  },   // | Collection | Lava Monsters. |
        { kS_level                        , kStr_level                        },   // | Data Field | Returns upgrade level of the building. |
        { kS_light                        , kStr_light                        },   // | Parameter | enable / disable parameter. |
        { kS_lights                       , kStr_lights                       },   // | Parameter | same as light. |
        { kS_LMLC                         , kStr_LMLC                         },   // | Macro | Number of Large Mobile Laser Cutters. |
        { kS_loaderdozer                  , kStr_loaderdozer                  },   // | Macro | Number of Loader Dozers. |
        { kS_loose_rock                   , kStr_loose_rock                   },   // | Macro | Tile ID of loose rock(30). |
        { kS_lose                         , kStr_lose                         },   // | Event | Lose the map. |
        { kS_miner                        , kStr_miner                        },   // | Variable / Class | Miner object or class. |
        { kS_miners                       , kStr_miners                       },   // | Macro | Miners discovered or active. |
        { kS_mininglaser                  , kStr_mininglaser                  },   // | Macro | Number of Mining Lasers. |
        { kS_monsters                     , kStr_monsters                     },   // | Macro | Number of monsters. |
        { kS_msg                          , kStr_msg                          },   // | Event | Display a message to user. |
        { kS_N                            , kStr_N                            },   // | Emerge Direction | North. |
        { kS_NavModifierLava_C            , kStr_NavModifierLava_C            },   // | Collection | Amount of lava tiles. | NavModifierLava_C | Collection | Amount of lava tiles. |
        { kS_NavModifierPowerPath_C       , kStr_NavModifierPowerPath_C       },   // | Collection | Amount of Power Path tiles, any type.NavModifierPowerPath_C | Collection | Amount of Power Path tiles, any type.Only finished patOnly finished paths. |
        { kS_NavModifierRubble_C          , kStr_NavModifierRubble_C          },   // | Collection | Amount of Rubble tiles, any stage. |
        { kS_NavModifierWater_C           , kStr_NavModifierWater_C           },   // | Collection | Amount of water tiles. |
        { kS_new                          , kStr_new                          },   // | Data Field Trigger | Trigger when object is created. |
        { kS_ore                          , kStr_ore                          },   // | Macro | Ore count. |
        { kS_Ore_C                        , kStr_Ore_C                        },   // | Collection | All ore. |
        { kS_orerefinery                  , kStr_orerefinery                  },   // | Macro | Number of Ore Refineries. |
        { kS_ore_seam                     , kStr_ore_seam                     },   // | Macro | Tile ID of an ore seam(46). |
        { kS_pan                          , kStr_pan                          },   // | Event | Pan camera. |
        { kS_pause                        , kStr_pause                        },   // | Event | Pauses the game. |
        { kS_place                        , kStr_place                        },   // | Event | Change tile. |
        { kS_power                        , kStr_power                        },   // | Data Field | Returns 1 if the building has power, 0 if it doesn't. |
        { kS_powered                      , kStr_powered                      },   // | Data Field | Same as power. |
        { kS_poweroff                     , kStr_poweroff                     },   // | Data Field Trigger | Trigger when power is deactivated for a building.
        { kS_poweron                      , kStr_poweron                      },   // | Data Field Trigger | Trigger when power is activated for a building.
        { kS_powerstation                 , kStr_powerstation                 },   // | Macro | Number of Power Stations. |
        { kS_progress_path                , kStr_progress_path                },   // | Macro | Tile id of a progress path(13). |
        { kS_qmsg                         , kStr_qmsg                         },   // | Event | Display message to user. |
        { kS_random                       , kStr_random                       },   // | Macro | return random number, random(low)(high) |
        { kS_rapidrider                   , kStr_rapidrider                   },   // | Macro | Number of Rapid Riders. |
        { kS_RechargeSeamGoal_C           , kStr_RechargeSeamGoal_C           },   // | Collection | Visible recharge seams. |
        { kS_red                          , kStr_red                          },   // | Color | Arrow colors. |
        { kS_reinforce                    , kStr_reinforce                    },   // | Trigger | Trigger when wall is reinforced. |
        { kS_reset                        , kStr_reset                        },   // | Event | Resets the player's selection |
        { kS_resetspeed                   , kStr_resetspeed                   },   // | Event | Loads the game speed from settings again. |
        { kS_resume                       , kStr_resume                       },   // | Event | Same as unpause. |
        { kS_RockMonster                  , kStr_RockMonster                  },   // | Collection | Rock Monsters. |
        { kS_row                          , kStr_row                          },   // | Data Field | Object row. |
        { kS_S                            , kStr_S                            },   // | Emerge Direction | South. |
        { kS_save                         , kStr_save                         },   // | Event | Save last miner that activated a trigger into a variable. |
        { kS_savebuilding                 , kStr_savebuilding                 },   // | Event | Save last building that activated a trigger into a variable.
        { kS_savecreature                 , kStr_savecreature                 },   // | Event | Save last creature that activated a trigger into a variable.
        { kS_savevehicle                  , kStr_savevehicle                  },   // | Event | Save last vehicle that activated a triggger into a variable.
        { kS_shake                        , kStr_shake                        },   // | Event | Shake camera. |
        { kS_showarrow                    , kStr_showarrow                    },   // | Event | Show arrow object. |
        { kS_SlimySlug                    , kStr_SlimySlug                    },   // | Collection | Slimy Slugs. |
        { kS_slug_hole                    , kStr_slug_hole                    },   // | Macro | Tile id of slimy slug hole(12). |
        { kS_slugs                        , kStr_slugs                        },   // | Macro | Number of slimy slugs. |
        { kS_smalldigger                  , kStr_smalldigger                  },   // | Macro | Number of Small Diggers. |
        { kS_SmallSpider                  , kStr_SmallSpider                  },   // | Collection | Small Spiders. |
        { kS_smalltransporttruck          , kStr_smalltransporttruck          },   // | Macro | Number of Small Transport Trucks. |
        { kS_SMLC                         , kStr_SMLC                         },   // | Macro | Number of Small Mobile Laser Cutters |
        { kS_solid_rock                   , kStr_solid_rock                   },   // | Macro | Tile ID of solid rock(38). |
        { kS_sound                        , kStr_sound                        },   // | Event | Play.ogg file. |
        { kS_spawncap                     , kStr_spawncap                     },   // | Event | Config random spawn. |
        { kS_spawnwave                    , kStr_spawnwave                    },   // | Event | Config random spawn. |
        { kS_speed                        , kStr_speed                        },   // | Event | Sets the game speed. |
        { kS_stamina                      , kStr_stamina                      },   // | Data Field | same as hp. |
        { kS_startrandomspawn             , kStr_startrandomspawn             },   // | Event | Start configured random spawn. |
        { kS_starttimer                   , kStr_starttimer                   },   // | Event | Start a timer. |
        { kS_stoprandomspawn              , kStr_stoprandomspawn              },   // | Event | Stop random spawn. |
        { kS_stoptimer                    , kStr_stoptimer                    },   // | Event | Stop a timer. |
        { kS_Stud_C                       , kStr_Stud_C                       },   // | Collection | All building studs. |
        { kS_studs                        , kStr_studs                        },   // | Macro | Building Stud count. |
        { kS_string                       , kStr_string                       },   // | Variable | Text. |
        { kS_superteleport                , kStr_superteleport                },   // | Macro | Number of Super Teleports. |
        { kS_supportstation               , kStr_supportstation               },   // | Macro | Number of Support Stations. |
        { kS_teleportpad                  , kStr_teleportpad                  },   // | Macro | Numbewr of Teleport Pads. |
        { kS_tick                         , kStr_tick                         },   // | Event Chain | Called on every engine tick.Not recommended. |
        { kS_tile                         , kStr_tile                         },   // | Data Field | TileID for objectt. |
        { kS_tileid                       , kStr_tileid                       },   // | Data Field | same as tile. |
        { kS_time                         , kStr_time                         },   // | Macro / Trigger | Game time or trigger. |
        { kS_timer                        , kStr_timer                        },   // | Variable | Timer object. |
        { kS_toolstore                    , kStr_toolstore                    },   // | Macro | Returns number of toolstores. |
        { kS_truewait                     , kStr_truewait                     },   // | Event | Suspend event chain for real user time period. |
        { kS_tunnelscout                  , kStr_tunnelscout                  },   // | Macro | Number of Tunnel Scouts. |
        { kS_tunneltransport              , kStr_tunneltransport              },   // | Macro | Number of Tunnel Transports. |
        { kS_upgrade                      , kStr_upgrade                      },   // | Trigger when vehicle is upgraded. |
        { kS_upgraded                     , kStr_upgraded                     },   // | Trigger | Not working, don't use. |
        { kS_upgradestation               , kStr_upgradestation               },   // | Macro | Number of Upgrade Stations. |
        { kS_unpause                      , kStr_unpause                      },   // | Event | Resumes the game if paused. |
        { kS_vehicle                      , kStr_vehicle                      },   // | Variable / Class | Vehicle object or trigger class. |
        { kS_vehicles                     , kStr_vehicles                     },   // | Macro | Number of vehicles. |
        { kS_VehicleCargoCarrier_C        , kStr_VehicleCargoCarrier_C        },   // | Collection | Cargo Carriers |
        { kS_VehicleChromeCrusher_C       , kStr_VehicleChromeCrusher_C       },   // | Collection | Chrome Crushers |
        { kS_VehicleGraniteGrinder_C      , kStr_VehicleGraniteGrinder_C      },   // | Collection | Granite Grinders |
        { kS_VehicleHoverScout_C          , kStr_VehicleHoverScout_C          },   // | Collection | Hover Scouts |
        { kS_VehicleLMLC_C                , kStr_VehicleLMLC_C                },   // | Collection | L   arge Mobile Laser Cutters |
        { kS_VehicleLoaderDozer_C         , kStr_VehicleLoaderDozer_C         },   // | Collection | Loader Dozers |
        { kS_VehicleRapidRider_C          , kStr_VehicleRapidRider_C          },   // | Collection | Rapid Riders |
        { kS_VehicleSmallDigger_C         , kStr_VehicleSmallDigger_C         },   // | Collection | Small Diggers |
        { kS_VehicleSmallTransportTruck_C , kStr_VehicleSmallTransportTruck_C },   // | Collection | Small Transport Trucks |
        { kS_VehicleSMLC_C                , kStr_VehicleSMLC_C                },   // | Collection | Small Mobile Laser Cutter |
        { kS_VehicleTunnelScout_C         , kStr_VehicleTunnelScout_C         },   // | Collection | Tunnel Scouts |
        { kS_VehicleTunnelTransport_C     , kStr_VehicleTunnelTransport_C     },   // | Collection | Tunnel Transports |
        { kS_wait                         , kStr_wait                         },   // | Event | Suspend event chain for a given period of time modified by game speed. | me speed. |
        { kS_walk                         , kStr_walk                         },   // | Trigger | Trigger when miner walks on a tile. |
        { kS_water                        , kStr_water                        },   // | Macro | Tile ID of water(11). |
        { kS_W                            , kStr_W                            },   // | Emerge Direction | West. |
        { kS_when                         , kStr_when                         },   // | Occurance | multiple time trigger. |
        { kS_white                        , kStr_white                        },   // | Color | Arrow colors. |
        { kS_win                          , kStr_win                          },   // | Event | Win the map. |
        { kS_X                            , kStr_X                            },   // | Data Field | Column, 300 values per cell |
        { kS_Y                            , kStr_Y                            },   // | Data Field | Row, 300 values per cell |
        { kS_yellow                       , kStr_yellow                       },   // | Colors | Arrow colors. |
        { kS_Z                            , kStr_Z                            },   // | Data Field | Height, 300 values per cell |
    };


    static constexpr uint64_t eMacroFlagReturnNumeric = 0x0000000000000001;  // int/floats may be freely exchanged between each other
    static constexpr uint64_t eMacroFlag2Param        = 0x0000000000000002;  // macro has 2 parameters

    // macros that return a value - can be used as variable
    const std::unordered_map<std::string, uint64_t> macrosReturnValue =
    {
        { kS_air                          , eMacroFlagReturnNumeric },   // macro int, amount of air
        { kS_Barrier_C                    , eMacroFlagReturnNumeric },   // collection, building
        { kS_Bat                          , eMacroFlagReturnNumeric },   // collection, creature
        { kS_building_path                , eMacroFlagReturnNumeric },   // macro int, number of objects
        { kS_BuildingCanteen_C            , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingDocks_C              , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingElectricFence_C      , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingGeologicalCenter_C   , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingMiningLaser_C        , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingOreRefinery_C        , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingPowerPath_C          , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingPowerStation_C       , eMacroFlagReturnNumeric },   // collection, building
        { kS_buildings                    , eMacroFlagReturnNumeric },   // macro int, number of buildings.
        { kS_BuildingSuperTeleport_C      , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingSupportStation_C     , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingTeleportPad_C        , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingToolStore_C          , eMacroFlagReturnNumeric },   // collection, building
        { kS_BuildingUpgradeStation_C     , eMacroFlagReturnNumeric },   // collection, building
        { kS_canteen                      , eMacroFlagReturnNumeric },   // macro, int, number of Canteens.
        { kS_cargocarrier                 , eMacroFlagReturnNumeric },   // macro, int number of Cargo Carriers.
        { kS_chromecrusher                , eMacroFlagReturnNumeric },   // macro int, number of Chrome Crushers
        { kS_clock                        , eMacroFlagReturnNumeric },   // macro, float, return game time
        { kS_CreatureBat_C                , eMacroFlagReturnNumeric },   // collection, creature
        { kS_CreatureIceMonster_C         , eMacroFlagReturnNumeric },   // collection, creature
        { kS_CreatureLavaMonster_C        , eMacroFlagReturnNumeric },   // collection, creature
        { kS_CreatureRockMonster_C        , eMacroFlagReturnNumeric },   // collection, creature
        { kS_CreatureSmallSpider_C        , eMacroFlagReturnNumeric },   // collection, creature
        { kS_CreatureSlimySlug_C          , eMacroFlagReturnNumeric },   // collection, creature
        { kS_creatures                    , eMacroFlagReturnNumeric },   // macro, int, Number of creatures
        { kS_Crystal_C                    , eMacroFlagReturnNumeric },   // collection, unstored crystals|
        { kS_crystal_seam                 , eMacroFlagReturnNumeric },   // macro, int, id of crystal seam always 42
        { kS_crystals                     , eMacroFlagReturnNumeric },   // macro, int, number of stored crystals
        { kS_docks                        , eMacroFlagReturnNumeric },   // Macro, Number of Docks.
        { kS_Dynamite_C                   , eMacroFlagReturnNumeric },   // Collection, All dynamite outside of toolstore.
        { kS_electricfence                , eMacroFlagReturnNumeric },   // Macro, Number of Fences.
        { kS_ElectricFence_C              , eMacroFlagReturnNumeric },   // Macro | Number of Fence objects.Not a collection. |
        { kS_EventErosion_C               , eMacroFlagReturnNumeric },   // | Collection | Active erosions. |
        { kS_EventLandslide_C             , eMacroFlagReturnNumeric },   // | Collection | Active active landslides. |
        { kS_erosionscale                 , eMacroFlagReturnNumeric },   // | Macro | Global erosion scale factor |
        { kS_geologicalcenter             , eMacroFlagReturnNumeric },   // | Macro | Number of Geological Centers. |
        { kS_get                          , eMacroFlagReturnNumeric | eMacroFlag2Param },   // | Macro | Get tile ID. |
        { kS_granitegrinder               , eMacroFlagReturnNumeric },   // | Macro | Number of Granite Grinders. |
        { kS_hard_rock                    , eMacroFlagReturnNumeric },   // | Macro | Tile ID of hard rock(34). |
        { kS_hostiles                     , eMacroFlagReturnNumeric },   // | Macro | Number of hostile creatures. |
        { kS_hoverscout                   , eMacroFlagReturnNumeric },   // | Macro | Number of Hover Scouts. |
        { kS_IceMonster                   , eMacroFlagReturnNumeric },   // | Collection | Ice Monsters. |
        { kS_lava                         , eMacroFlagReturnNumeric },   // | Macro | Tile ID of lava(6). |
        { kS_LavaMonster                  , eMacroFlagReturnNumeric },   // | Collection | Lava Monsters. |
        { kS_LMLC                         , eMacroFlagReturnNumeric },   // | Macro | Number of Large Mobile Laser Cutters. |
        { kS_loaderdozer                  , eMacroFlagReturnNumeric },   // | Macro | Number of Loader Dozers. |
        { kS_loose_rock                   , eMacroFlagReturnNumeric },   // | Macro | Tile ID of loose rock(30). |
        { kS_miners                       , eMacroFlagReturnNumeric },   // | Macro | Miners discovered or active. |
        { kS_mininglaser                  , eMacroFlagReturnNumeric },   // | Macro | Number of Geological Centers. |
        { kS_monsters                     , eMacroFlagReturnNumeric },   // | Macro | Number of monsters. |
        { kS_NavModifierLava_C            , eMacroFlagReturnNumeric },   // | Collection | Amount of lava tiles. | NavModifierLava_C | Collection | Amount of lava tiles. |
        { kS_NavModifierPowerPath_C       , eMacroFlagReturnNumeric },   // | Collection | Amount of Power Path tiles, any type.NavModifierPowerPath_C | Collection | Amount of Power Path tiles, any type.Only finished patOnly finished paths. |
        { kS_NavModifierRubble_C          , eMacroFlagReturnNumeric },   // | Collection | Amount of Rubble tiles, any stage. |
        { kS_NavModifierWater_C           , eMacroFlagReturnNumeric },   // | Collection | Amount of water tiles. |
        { kS_ore                          , eMacroFlagReturnNumeric },   // | Macro | Ore count. |
        { kS_Ore_C                        , eMacroFlagReturnNumeric },   // | Collection | All ore. |
        { kS_orerefinery                  , eMacroFlagReturnNumeric },   // | Macro | Number of Ore Refineries. |
        { kS_ore_seam                     , eMacroFlagReturnNumeric },   // | Macro | Tile ID of an ore seam(46). |
        { kS_powerstation                 , eMacroFlagReturnNumeric },   // | Macro | Number of Power Stations. |
        { kS_progress_path                , eMacroFlagReturnNumeric },   // | Macro | Tile id of a progress path(13). |
        { kS_random                       , eMacroFlagReturnNumeric | eMacroFlag2Param },   // | Macro | return random number, random(low)(high) |
        { kS_rapidrider                   , eMacroFlagReturnNumeric },   // | Macro | Number of Rapid Riders. |
        { kS_RechargeSeamGoal_C           , eMacroFlagReturnNumeric },   // | Collection | Visible recharge seams. |
        { kS_RockMonster                  , eMacroFlagReturnNumeric },   // | Collection | Rock Monsters. |
        { kS_SlimySlug                    , eMacroFlagReturnNumeric },   // | Collection | Slimy Slugs. |
        { kS_slug_hole                    , eMacroFlagReturnNumeric },   // | Macro | Tile id of slimy slug hole(12). |
        { kS_slugs                        , eMacroFlagReturnNumeric },   // | Macro | Number of slimy slugs. |
        { kS_smalldigger                  , eMacroFlagReturnNumeric },   // | Macro | Number of Small Diggers. |
        { kS_SmallSpider                  , eMacroFlagReturnNumeric },   // | Collection | Small Spiders. |
        { kS_smalltransporttruck          , eMacroFlagReturnNumeric },   // | Macro | Number of Small Transport Trucks. |
        { kS_SMLC                         , eMacroFlagReturnNumeric },   // | Macro | Number of Small Mobile Laser Cutters |
        { kS_solid_rock                   , eMacroFlagReturnNumeric },   // | Macro | Tile ID of solid rock(38). |
        { kS_Stud_C                       , eMacroFlagReturnNumeric },   // | Collection | All building studs. |
        { kS_studs                        , eMacroFlagReturnNumeric },   // | Macro | Building Stud count. |
        { kS_superteleport                , eMacroFlagReturnNumeric },   // | Macro | Number of Super Teleports. |
        { kS_supportstation               , eMacroFlagReturnNumeric },   // | Macro | Number of Support Stations. |
        { kS_teleportpad                  , eMacroFlagReturnNumeric },   // | Macro | Numbewr of Teleport Pads. |
        { kS_time                         , eMacroFlagReturnNumeric },   // | Macro / Trigger | Game time or trigger. |
        { kS_toolstore                    , eMacroFlagReturnNumeric },   // | Macro | Returns number of toolstores. |
        { kS_tunnelscout                  , eMacroFlagReturnNumeric },   // | Macro | Number of Tunnel Scouts. |
        { kS_tunneltransport              , eMacroFlagReturnNumeric },   // | Macro | Number of Tunnel Transports. |
        { kS_upgradestation               , eMacroFlagReturnNumeric },   // | Macro | Number of Upgrade Stations. |
        { kS_vehicles                     , eMacroFlagReturnNumeric },   // | Macro | Number of vehicles. |
        { kS_VehicleCargoCarrier_C        , eMacroFlagReturnNumeric },   // | Collection | Cargo Carriers |
        { kS_VehicleChromeCrusher_C       , eMacroFlagReturnNumeric },   // | Collection | Chrome Crushers |
        { kS_VehicleGraniteGrinder_C      , eMacroFlagReturnNumeric },   // | Collection | Granite Grinders |
        { kS_VehicleHoverScout_C          , eMacroFlagReturnNumeric },   // | Collection | Hover Scouts |
        { kS_VehicleLMLC_C                , eMacroFlagReturnNumeric },   // | Collection | L   arge Mobile Laser Cutters |
        { kS_VehicleLoaderDozer_C         , eMacroFlagReturnNumeric },   // | Collection | Loader Dozers |
        { kS_VehicleRapidRider_C          , eMacroFlagReturnNumeric },   // | Collection | Rapid Riders |
        { kS_VehicleSmallDigger_C         , eMacroFlagReturnNumeric },   // | Collection | Small Diggers |
        { kS_VehicleSmallTransportTruck_C , eMacroFlagReturnNumeric },   // | Collection | Small Transport Trucks |
        { kS_VehicleSMLC_C                , eMacroFlagReturnNumeric },   // | Collection | Small Mobile Laser Cutter |
        { kS_VehicleTunnelScout_C         , eMacroFlagReturnNumeric },   // | Collection | Tunnel Scouts |
        { kS_VehicleTunnelTransport_C     , eMacroFlagReturnNumeric },   // | Collection | Tunnel Transports |
        { kS_water                        , eMacroFlagReturnNumeric },   // | Macro | Tile ID of water(11). |
    };

    const std::unordered_set<std::string> colors =
    {
        kS_black,
        kS_blue,
        kS_darkgreen,
        kS_green,
        kS_red,
        kS_white,
        kS_yellow,
    };

    bool isColor(const std::string& color) const
    {
        return colors.contains(MMUtil::toLower(color));
    }


    typedef std::unordered_set<std::string> ReserveWordsEventsVars;

    bool isReservedVar( const std::string & key ) const
    {
        return m_ReservedWordsVars.contains( MMUtil::toLower( key ) );
    }

    bool isReservedEvent( const std::string & key ) const
    {
        return m_ReservedWordsEvents.contains( MMUtil::toLower( key ) );
    }

    void doInit()
    {
        BuildReservedWords();
    }

    void BuildReservedWords()
    {
        m_ReservedWordsVars.reserve( ReservedWordsMap.size() );
        m_ReservedWordsEvents.reserve( ReservedWordsMap.size() );
        for (auto it : ReservedWordsMap)
        {
            m_ReservedWordsVars.insert( it.first );
        }
        m_ReservedWordsEvents = m_ReservedWordsVars;
        m_ReservedWordsEvents.erase( kS_init );
        m_ReservedWordsEvents.erase( kS_tick );
    }

    ReserveWordsEventsVars m_ReservedWordsVars;
    ReserveWordsEventsVars m_ReservedWordsEvents;

protected:
    class Defines   // we support #pragma define key=value, there are also predefined ones
    {
    public:
        class DefineKeyValue
        {
        public:
            DefineKeyValue() = default;
            DefineKeyValue(const std::string& key, const std::string& value) { setKeyValue( key, value); }
            ~DefineKeyValue() = default;

            void setKeyValue(const std::string &key, const std::string& value)
            {
                m_key = key;
                m_keylc = MMUtil::toLower(key);
                m_value = value;
            }

            bool operator == (const DefineKeyValue& rhs) const  // for collections
            {
                return m_keylc == rhs.m_keylc;
            }
            bool operator < (const DefineKeyValue& rhs) const   // for collections
            {
                return m_keylc < rhs.m_keylc;
            }

            void changeValue(const std::string& value)
            {
                m_value = value;
            }

            const std::string & getKey() const   { return m_key; }
            const std::string & getValue() const { return m_value;}
            const std::string & getkeylc() const { return m_keylc; }

        protected:
            std::string   m_key;    // user key
            std::string   m_keylc;  // key to compare against
            std::string   m_value;  // value of key
        };

        Defines()
        {
            m_defines.reserve(32);
        }
        ~Defines() = default;

        bool contains(const std::string & key) const
        {
            std::string keylc = MMUtil::toLower(key);
            return m_defines.find(keylc) != m_defines.cend();
        }

        std::string getValue(const std::string & key) const
        {
            std::string keylc = MMUtil::toLower(key);
            auto it = m_defines.find(keylc);
            if (it != m_defines.cend())
                return (*it).second.getValue();
            return std::string();
        }

        void addKeyValue(const std::string & key, const std::string & value)
        {
            std::string keylc = MMUtil::toLower(key);
            auto it = m_defines.find(keylc);
            if (it != m_defines.end())      // already exists
            {
                (*it).second.changeValue(value);       // change value
            }
            else
            {
                m_defines.emplace(keylc, DefineKeyValue(key, value) );
            }
        }

    protected:
        std::unordered_map<std::string, DefineKeyValue> m_defines;
    };

    // when we tokenize a line, each token will be one of these.
    static constexpr uint64_t eUnknown             = 0x0000000000000000ull;  // unknown
    static constexpr uint64_t eTokenFloat          = 0x0000000000000001ull;  // some sort of floating point number
    static constexpr uint64_t eTokenInt            = 0x0000000000000002ull;  // number 
    static constexpr uint64_t eTokenName           = 0x0000000000000004ull;  // starts with alpha, has nothing but more alpha or digits. If it starts with a numeric, it must have an alpha to identify it as a variable
    static constexpr uint64_t eTokenSpace          = 0x0000000000000008ull;  // single space
    static constexpr uint64_t eTokenSpaces         = 0x0000000000000010ull;  // more than 1 space
    static constexpr uint64_t eTokenDot            = 0x0000000000000020ull;  // .
    static constexpr uint64_t eTokenSemi           = 0x0000000000000040ull;  // ;
    static constexpr uint64_t eTokenColon          = 0x0000000000000080ull;  // :
    static constexpr uint64_t eTokenDColon         = 0x0000000000000100ull;  // ::
    static constexpr uint64_t eTokenOBrace         = 0x0000000000000200ull;  // (
    static constexpr uint64_t eTokenDOBrace        = 0x0000000000000400ull;  // ((
    static constexpr uint64_t eTokenCBrace         = 0x0000000000000800ull;  // )
    static constexpr uint64_t eTokenDCBrace        = 0x0000000000001000ull;  // ))
    static constexpr uint64_t eTokenOBracket       = 0x0000000000002000ull;  // [
    static constexpr uint64_t eTokenCBracket       = 0x0000000000004000ull;  // ]
    static constexpr uint64_t eTokenPlus           = 0x0000000000008000ull;  // +
    static constexpr uint64_t eTokenPlusAssign     = 0x0000000000010000ull;  // +=
    static constexpr uint64_t eTokenMinus          = 0x0000000000020000ull;  // -
    static constexpr uint64_t eTokenMinusAssign    = 0x0000000000040000ull;  // -=
    static constexpr uint64_t eTokenMultiply       = 0x0000000000080000ull;  // *
    static constexpr uint64_t eTokenMultiplyAssign = 0x0000000000100000ull;  // *=
    static constexpr uint64_t eTokenFslash         = 0x0000000000200000ull;  //  /  special case, follows is an objective treat as a string
    static constexpr uint64_t eTokenDivide         = 0x0000000000400000ull;  // //  divide   not supported on older versions of the engine
    static constexpr uint64_t eTokenDivideAssign   = 0x0000000000800000ull;  // /=
    static constexpr uint64_t eTokenAssignment     = 0x0000000001000000ull;  // =
    static constexpr uint64_t eTokenEqual          = 0x0000000002000000ull;  // ==
    static constexpr uint64_t eTokenObjStr         = 0x0000000004000000ull;  //  string after / is objective, until end of line
    static constexpr uint64_t eTokenLess           = 0x0000000008000000ull;  // <
    static constexpr uint64_t eTokenLessEqual      = 0x0000000010000000ull;  // <=
    static constexpr uint64_t eTokenGreater        = 0x0000000020000000ull;  // >
    static constexpr uint64_t eTokenGreaterEqual   = 0x0000000040000000ull;  // >=
    static constexpr uint64_t eTokenNot            = 0x0000000080000000ull;  // !  mm engine does not support this currently, we will generate an error
    static constexpr uint64_t eTokenNotEqual       = 0x0000000100000000ull;  // !=
    static constexpr uint64_t eTokenComma          = 0x0000000200000000ull;  // ,
    static constexpr uint64_t eTokenString         = 0x0000000400000000ull;  // string literal (was defined within quotes), quotes not part of string
    static constexpr uint64_t eTokenComment        = 0x0000000800000000ull;  // contains comment and optional leading space prior to comment
    static constexpr uint64_t eTokenCommentLine    = 0x0000001000000000ull;  // entire line is comment, may have space in front
    static constexpr uint64_t eTokenBlankLine      = 0x0000002000000000ull;  // entire line is empty and not a comment. Will end event chain if within one
    static constexpr uint64_t eTokenVariable       = 0x0000002000000000ull;  // also has eTokenName set, this is a user variable
    static constexpr uint64_t eTokenEventChain     = 0x0000002000000000ull;  // also has eTokenName set, this is a user event chain name

    static constexpr uint64_t eTokenIgnore         = 0x8000000000000000ull;  // if set ignore the token completely. When when ignorning invalid spaces

    class variableType
    {
    public:

        enum varType : int {
            eVarTypeBool = 0,   // variable holds bool4
            eVarTypeInt,        // variable holds int
            eVarTypeFloat,      // variable holds float
            eVarTypeString,     // variable holds string
            eVarTypeIntArray,   // variable holds int array
            eVarTypeMiner,      // variable holds Miner
            eVarTypeBuilding,   // variable holds Building
            eVarTypeVehicle,    // variable holds Vehicle
            eVarTypeCreature,   // variable holds creature
            eVarTypeArrow,      // variable holds arrow
            eVarTypeTimer,      // variable holds timers
            eVarTypeLAST = eVarTypeTimer
        };

        variableType() = default;
        variableType(const std::string& name, varType type) : m_name(name), m_namelc(MMUtil::toLower(m_name)), m_type(type), m_count(1) {}
        ~variableType() = default;

        void setValueString( const std::string &val ) { m_string = val; }
        void setValueTimer(float delay, float min, float max, const std::string& event)
        {
            m_timerdelay = delay;
            m_timermin = min;
            m_timermax = max;
            m_timerevent = event;
        }
        void setValueBool( bool val ) { m_bool = val; }
        void setValueInt( int val ) { m_int = val; }
        void setValueFloat( float val ) { m_float = val; }
        void setValueMiner( int id ) { m_id = id; }
        void setValueBuilding( int row, int col ) { m_row = row; m_col = col; }

        int getType() const { return (int)m_type; }
        const std::string & getName() const { return m_name; }
        const std::string & getNamelc() const { return m_namelc; }

        bool operator == (const variableType& rhs) const { return m_namelc == rhs.m_namelc; } // collections
        bool operator <  (const variableType& rhs) const { return m_namelc <  rhs.m_namelc; } // collections

        // return true/false if this varible is a duplicate value - only for miner, building, vehicle,creature,arrow
        bool isDuplicateValue(const variableType& rhs) const
        {
            switch (rhs.m_type)
            {
            default: break;
 
            case eVarTypeMiner:
            case eVarTypeVehicle:
            case eVarTypeCreature:   // variable holds creature
                return (rhs.m_type == m_type) && (rhs.m_id == m_id);

            case eVarTypeBuilding:
                return (rhs.m_type == m_type) && (rhs.m_row == m_row) && (rhs.m_col == m_col);
            }
            return false;
        }

        // true if name has been optimized
        bool isOptName() const
        {
            return m_optname.empty();
        }

        // return ref to either original name or optimized name if it has one
        const std::string & getOptName() const
        {
            return m_optname.empty() ? m_name : m_optname;
        }

        void intcount() { m_count++; }
        int getCount() const { return m_count; }


    protected:
        std::string m_namelc;     // lower case name
        std::string m_name;       // original name may be mixed case
        std::string m_optname;    // optimized name to write with
        int         m_count = 0;  // number of references
        varType     m_type = eVarTypeBool;  // defines type of variable

        // these are the types of = parameters
        std::string m_string;         // string value or arrow color
        std::string m_timerevent;
        float       m_float = 0;      // float value
        float       m_timerdelay = 0;
        float       m_timermin = 0;
        float       m_timermax = 0;
        bool        m_int = 0;        // integer value
        int         m_id  = 0;        // id of miner/vehicle/creature
        int         m_row = 0;        // building row
        int         m_col = 0;        // building col
        bool        m_bool = false;   // true/false
        bool        m_hasValue = false;
    };

    typedef std::shared_ptr<variableType> variableTypeSP;
    
    // holds all user variables
    // the class variableType is the information for a single user variable. The collections all store
    // a shared_ptr to it, so all the collections can work on the same object and we don't worry about ownership.
    class allUserVariables
    {
    public:

        // In addition the the entire collection, we duplicate into variable type collections.
        // This allows faster checks to identify duplicate variables to same object (which is an error)
        class userVariables
        {
        public:
            userVariables() = default;
            ~userVariables() = default;

            void add(const std::string& key, variableTypeSP& p)
            {
                m_variables[p.get()->getType()].emplace( key, p );
            }

            bool contains(const std::string& key, variableType::varType type)
            {
                return m_variables[type].contains(MMUtil::toLower(key));
            }

            variableType * find(const std::string& key, variableType::varType type)
            {
                auto it = m_variables[type].find(MMUtil::toLower(key));
                return it->second.get();
            }

            const variableType * find(const std::string& key, variableType::varType type) const
            {
                auto it = m_variables[type].find(MMUtil::toLower(key));
                return it->second.get();
            }
        protected:

            std::unordered_map<std::string, variableTypeSP> m_variables[variableType::eVarTypeLAST+1];  
        };


        allUserVariables() = default;
        ~allUserVariables() = default;

        void add(variableTypeSP& variable)
        {
            const std::string & key = variable.get()->getNamelc();
            if (!contains(key))
            {
                m_allvariables.emplace(key,variable);
                m_typevariables.add(key, variable);
            }
            assert(0 && "adding existing variable");
        }

        bool contains(const std::string& key)
        {
            return m_allvariables.contains( MMUtil::toLower(key) );
        }

        variableType * find(const std::string& key)
        {
            auto it = m_allvariables.find( MMUtil::toLower(key));
            return it->second.get();
        }

        const variableType * find(const std::string& key) const
        {
            auto it = m_allvariables.find( MMUtil::toLower(key));
            return it->second.get();
        }

    protected:
        std::unordered_map<std::string, variableTypeSP> m_allvariables;
        userVariables                                   m_typevariables;
    };


    class eventChainName
    {
    public:
        eventChainName() = default;
        eventChainName(const std::string& name) : m_name(name), m_namelc(MMUtil::toLower(name)), m_count(1) {}
        ~eventChainName() = default;

        void incCount() { m_count++; }
        int  getCount() const { return m_count; }
        void setOptName( const std::string &optname ) { m_optname = optname; }

        // true if name has been optimized
        bool isOptName() const
        {
            return m_optname.empty();
        }

        const std::string& getOptName() const { return m_optname.empty() ? m_name : m_optname; }

    protected:
        std::string m_name;      // original name
        std::string m_namelc;    // lower case version of name
        std::string m_optname;   // optimized name
        int         m_count = 0; // number of times name is used
    };

    class allEventChainNames
    {
    public:
        allEventChainNames() = default;
        ~allEventChainNames() = default;

        bool contains(const std::string& key) const
        {
            return m_eventchainnames.contains(MMUtil::toLower(key));
        }

        // increment the count if it already exists, or add a new one
        void add(const std::string& name)
        {
            std::string namelc = MMUtil::toLower(name);
            auto it = m_eventchainnames.find( MMUtil::toLower(name));
            if (it != m_eventchainnames.end())
            {
                (*it).second.incCount();
            }
            else
            {
                m_eventchainnames.emplace(namelc,eventChainName(name));
            }
        }

        // true if name has been optimized
        bool isOptName(const std::string& name) const
        {
            auto it = m_eventchainnames.find( MMUtil::toLower(name));
            if (it != m_eventchainnames.end())
            {
                return (*it).second.isOptName();
            }
            return false;
        }


        void setOptName(const std::string& name, const std::string& optName)
        {
            auto it = m_eventchainnames.find( MMUtil::toLower(name));
            if (it != m_eventchainnames.end())
            {
                (*it).second.setOptName( optName );
            }
        }

        std::string getOptName(const std::string& name) const
        {
            std::string namelc = MMUtil::toLower(name);
            auto it = m_eventchainnames.find( MMUtil::toLower(name));
            if (it != m_eventchainnames.end())
            {
                return (*it).second.getOptName();
            }
            return std::string();
        }



    protected:
        std::unordered_map<std::string, eventChainName> m_eventchainnames;
    };

    class ScriptToken   // TODO needs to hold more info - not sure what that will be
    {
    public:
        ScriptToken() = default;
        ~ScriptToken() = default;

        ScriptToken(const std::string& str, uint64_t id) : m_token(str), m_tokenlc(MMUtil::toLower(m_token)), m_id(id) {}
        ScriptToken(std::string_view str, uint64_t id)   : m_token(str), m_tokenlc(MMUtil::toLower(m_token)), m_id(id) {}

        void setToken( const std::string &str) { m_token = str; m_tokenlc = MMUtil::toLower(str); }     // used to convert multiple spaces to single space

        bool operator == (const ScriptToken &rhs ) { return m_tokenlc == rhs.m_tokenlc; }   // for collections
        bool operator <  (const ScriptToken &rhs ) { return m_tokenlc < rhs.m_tokenlc; }    // for collections

        const std::string & getToken() const   { return m_token; }
        const std::string & getTokenlc() const { return m_tokenlc; }
        uint64_t getID() const { return m_id; }
        void orID( uint64_t value ) { m_id |= value; }      // used to refine the token type during processing.

    protected:
        std::string m_token;
        std::string m_tokenlc;  // converted to lower case
        uint64_t    m_id;       // need to figure out what we will do with these
    };

    // holds a line and all tokens for that line
    // caller takes care of pragma processing
    class ScriptLine
    {
    public:
        ScriptLine() = default;
        ScriptLine(const InputLine & line) : m_line(line) {}
        ~ScriptLine() = default;

        // no syntax analysis, simply break up line into tokens.
        // all macro subsitution is part of rawParse - there is no macro token
        // when a macro is found, its value replaces $(name) into the temp line, becomes the new input line and the line starts all over again.
        // macros inside of strings will expand with all quotes removed into the current string, no need to reprocess the line.
        void rawParse( ErrorWarning & errors, const Defines & defines )
        {

            bool bMacro = false;        // if a macro is found, reparse the line after macro expansion.
            do   // stay in loop retokenizing entire line until no more macros
            {
                bMacro = false;                             // start off no macro in this line so far
                m_tokens.clear();                           // start off with no tokens
                std::string localline = m_line.getLine();   // make a copy of the line in case of macros
                std::string_view input = localline;         // view into entire line
                std::size_t inputLen = input.length();

                // check for comment
                std::size_t commentpos = input.find('#');
                if (commentpos == 0)                          // line starts with comment, macros are not processed inside of comments.
                {
                    std::string comment(input);
                    if (comment.size() > 1 && comment[1] == '.')    // comments starting with #. we will expaande macros into
                    {
                        if (!embededMacro( comment, errors, defines))
                            return;
                    }

                    m_tokens.emplace_back(comment, eTokenCommentLine);     // entire line is comment
                    return;   // full length comment
                }
                if ((commentpos > 0) && (commentpos < inputLen))  // see if all blank before comment
                {
                    bool bWhite = true;
                    for (std::size_t i = 0; i < commentpos; i++)
                    {
                        if (std::isblank((unsigned char)input[i]))
                            continue;
                        else
                        {
                            bWhite = false;
                            break;
                        }
                    }
                    if (bWhite)   // all spaces prior to comment - this is allowed
                    {
                        std::string comment(input);
                        if (comment.size() > commentpos && comment[commentpos+1] == '.')    // comments starting with #. we will expaand macros
                        {
                            if (!embededMacro( comment, errors, defines))
                                return;
                        }
                        m_tokens.emplace_back(comment, eTokenCommentLine);     // entire line is comment
                        return;
                    }

                    // now start backwards from comment, find first non-space.
                    // will remember there is a comment, it is added at end.
                    std::size_t i = commentpos - 1;
                    for (; i > 0; i--)
                    {
                        if (!std::isblank((unsigned char)input[i]))
                        {
                            break;
                        }
                    }
                    commentpos = i + 1; // either # char or beginning of white space prior to comment
                }
                else
                {
                    // see if entire line is empty
                    if (MMUtil::isEmptyStr(input))
                    {
                        m_tokens.emplace_back(input, eTokenBlankLine);   // entire line is empty - it will terminate an event chain
                        return;
                    }
                    commentpos = inputLen;    // pretend we have a comment after end of line - this allows us to use this to stop parsing
                }

                // now sit in loop parsing tokens. Stop when we hit the comment start location (or end of string)
                std::string_view token;
                for (std::size_t npos = 0; (npos < commentpos) && !bMacro;)
                {
                    std::size_t epos = npos + 1;
                    // if we have an alpha, treat as some sort of name for the remaining alpha and digits
                    uint8_t ch = input[npos];
                    if (std::isalpha((unsigned char)ch))   // have an alpha char. Continue while more alpha or digits
                    {
                        for (; epos < commentpos; epos++)
                        {
                            if (!std::isdigit(input[epos]) && !std::isalpha((unsigned char)input[epos])) // this char ends the token
                                break;
                        }
                        token = input.substr(npos, epos - npos);
                        m_tokens.emplace_back(token, eTokenName);
                    }

                    else
                    {
                        switch (ch)
                        {
                        case ' ':  // space   see if single space or sequence of spaces
                        case 9:    //tab
                        {
                            for (; (epos < commentpos) && std::isblank((unsigned char)input[epos]); epos++)
                                ;
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, token.length() > 1 ? eTokenSpaces : eTokenSpace);
                            break;
                        }

                        case '.':   // this may be a dot not a number
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':  // starting number, float, or variable
                        {
                            // because variables can start with a number, we need to scan to see if we only have numbers and at least one alpha.
                            // anything that is non-alpha before the alpha means a number. Once you see an alpha, continue with only
                            // alpha and numbers.
                            if (ch != '.')
                            {
                                for (; (epos < commentpos) && std::isdigit((unsigned char)input[epos]); epos++)
                                    ;  // scan until first non-number
                                if ((epos < commentpos) && isalpha(input[epos]))  // we have an alpha, so treat as an name
                                {
                                    for (epos++; epos < commentpos && (std::isalpha((unsigned char)input[epos]) || std::isdigit((unsigned char)input[epos])); epos++)
                                        ;
                                    token = input.substr(npos, epos - npos);
                                    m_tokens.emplace_back(token, eTokenName);
                                    break;
                                }
                                epos = npos + 1; // back so we can build number or float
                            }

                            bool bFloat = false;
                            if (ch == '.')                  // floats can begin with a .
                            {
                                if ((epos < commentpos) && std::isdigit((unsigned char)input[epos]))  // next is a number so treat as a float
                                    bFloat = true;
                                else                       // just a dot char, so add to tokens as a dot
                                {
                                    token = input.substr(npos, 1);
                                    m_tokens.emplace_back(token, eTokenDot);
                                    break;
                                }
                            }
                            for (; epos < commentpos; epos++)  // now we treat it as either an int or float
                            {
                                if (!std::isdigit((unsigned char)input[epos]))
                                {
                                    if (input[epos] == '.')
                                    {
                                        if (!bFloat)
                                            bFloat = true;
                                        else
                                            break;
                                    }
                                    else
                                        break;
                                }
                            }
                            // token is from [spos,epos)
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, bFloat ? eTokenFloat : eTokenInt);
                            break;
                        }

                        case ';':
                        {
                            token = input.substr(npos, 1);
                            m_tokens.emplace_back(token, eTokenSemi);
                            break;
                        }

                        case ':':  // single or double
                        {
                            uint64_t tag = eTokenColon;
                            if ((epos < commentpos) && (input[epos] == ':'))
                            {
                                tag = eTokenDColon;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case '(':  // single or double brace
                        {
                            uint64_t tag = eTokenOBrace;
                            if ((epos < commentpos) && (input[epos] == '('))
                            {
                                tag = eTokenDOBrace;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case ')':  // single or double brace
                        {
                            uint64_t tag = eTokenCBrace;
                            if ((epos < commentpos) && (input[epos] == ')'))
                            {
                                tag = eTokenDCBrace;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case '[':
                        {
                            token = input.substr(npos, 1);
                            m_tokens.emplace_back(token, eTokenOBracket);
                            break;
                        }

                        case ']':
                        {
                            token = input.substr(npos, 1);
                            m_tokens.emplace_back(token, eTokenCBracket);
                            break;
                        }

                        case '+':  // addition or asignment
                        {
                            uint64_t tag = eTokenPlus;
                            if ((epos < commentpos) && (input[epos] == '='))
                            {
                                tag = eTokenPlusAssign;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case '-':  // addition or asignment
                        {
                            uint64_t tag = eTokenMinus;
                            if ((epos < commentpos) && (input[epos] == '='))
                            {
                                tag = eTokenMinusAssign;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case '*':  // addition or asignment
                        {
                            uint64_t tag = eTokenMultiply;
                            if ((epos < commentpos) && (input[epos] == '='))
                            {
                                tag = eTokenMultiplyAssign;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case '/':  // can be divide, objective, assignment
                        {
                            uint64_t tag = eTokenFslash;
                            if (epos < commentpos)   // possible to have 2nd char
                            {
                                if (input[epos] == '/')
                                {
                                    epos++;
                                    tag = eTokenDivide;
                                }
                                else if (input[epos] == '=')
                                {
                                    epos++;
                                    tag = eTokenDivideAssign;
                                }
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            if (tag == eTokenFslash)  // if objective, everything after the / is object and treat as string
                            {
                                token = input.substr(epos, commentpos - epos);
                                m_tokens.emplace_back(token, eTokenObjStr);
                                epos = commentpos;  // this forces the end of processing
                            }
                            break;
                        }

                        case '<':  // < or <=
                        {
                            uint64_t tag = eTokenLess;
                            if ((epos < commentpos) && (input[epos] == '='))
                            {
                                tag = eTokenLessEqual;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case '>':  // > or >=
                        {
                            uint64_t tag = eTokenGreater;
                            if ((epos < commentpos) && (input[epos] == '='))
                            {
                                tag = eTokenGreaterEqual;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case '!':  // ! or !=    ! is current an error
                        {
                            uint64_t tag = eTokenNot;
                            if ((epos < commentpos) && (input[epos] == '='))
                            {
                                tag = eTokenNotEqual;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case '=':  // = or ==   
                        {
                            uint64_t tag = eTokenAssignment;
                            if ((epos < commentpos) && (input[epos] == '='))
                            {
                                tag = eTokenEqual;
                                epos++;
                            }
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, tag);
                            break;
                        }

                        case ',':
                        {
                            token = input.substr(npos, 1);
                            m_tokens.emplace_back(token, eTokenComma);
                            break;
                        }

                        case '"':  // start quoted string. We want to keep the quotes
                        {
                            for (; epos < commentpos && input[epos] != '"'; epos++)
                                ;
                            if ((epos >= commentpos) || (input[epos] != '"'))   // missing ending quote
                            {
                                ;   // we need to do some sort of error
                            }
                            if (epos == npos)
                                token = "\"\"";
                            else
                                token = input.substr(npos, epos++ - npos + 1);

                            std::string quotetoken(token);  // saved quoted string into string so we can deal with macro expansion
                            if (!embededMacro(quotetoken, errors, defines))
                                return;     // error

                            m_tokens.emplace_back(quotetoken, eTokenString);

                            break;
                        }

                        case '$':   // possible start of macro. Format is $(macro)$. macro is only alpha/digits, must start with alpha
                        {
                            std::size_t endpos;
                            std::string macro;
                            if (isMacro(m_line, errors, input, npos, endpos, macro))
                            {
                                if (!defines.contains(macro))
                                {
                                    errors.setError(m_line,std::string("Unknown macro name: ")+macro);
                                    return;
                                }
                                std::string macrovalue = defines.getValue( macro );
                                if (macrovalue.empty())
                                {
                                    errors.setError(m_line,"Empty macro value");
                                    return;
                                }
                                // have macro, replace data in line, expanded line is now the input line (allows errors to capture it), and start reprocessing entire line again.
                                // this allows macros to be almost every token
                                localline.replace( npos,endpos - npos+1,macrovalue);
                                m_line.setLine(localline);          // change InputLine to have modified expanded line.
                                bMacro = true;                      // will cause token processing to start all over for this line.
                                break;

                            }
                            // invalid starting $ sequence treat same as default case
                            // for now take all the unknown chars and make a token out of it (may have to also allow alpha and digits if mm engine supports unicode var names)
                            for (; (epos < commentpos) && !isKnown(input[epos]); epos++)
                                ;
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, eUnknown);
                            break;
                        }

                        default:  // no idea. Posssible a unicode variable name (TODO - does MM engine deal with this case?)
                        {
                            // for now take all the unknown chars and make a token out of it (may have to also allow alpha and digits if mm engine supports unicode var names)
                            for (; (epos < commentpos) && !isKnown(input[epos]); epos++)
                                ;
                            token = input.substr(npos, epos - npos);
                            m_tokens.emplace_back(token, eUnknown);
                            break;
                        }

                        } // switch
                    } // else
                    npos = epos;
                } // for
                  // we have parsed everything not a comment. If we have a comment, now add it
                if (!bMacro && (commentpos < inputLen))
                {
                    std::string_view commentview = input.substr(commentpos);   // may have spaces before comment.
                    std::string commentstr(commentview);                      // string that may be modified
                    if (MMUtil::removeLeadingWhite(commentview).find("#.") == 0)
                    {
                        if (!embededMacro( commentstr, errors, defines))
                            return;
                    }

                    m_tokens.emplace_back(commentstr, eTokenComment);
                }
            }
            while (bMacro == true);  // until no more macros

        }

        // build line from the parsed tokens
        std::string serialize_out( const ScriptEngine & se ) const
        {
            std::string str;
            std::size_t len = m_line.getLine().length();
            str.reserve( len ? len : 256 );
            for (auto it : m_tokens)
            {
                if (it.getID() & eTokenIgnore)  // if token is flag for ignore
                    continue;                   // don't add it to output. Used to remove invalid space
                if (it.getID() & eTokenVariable) // token is some sort of user variable, it may be renamed
                {
                    const allUserVariables & var = se.getUserVariables();
                    const variableType * vt = var.find(it.getToken());
                    if (vt->isOptName())
                        str += vt->getOptName();       // replace with generated optimized name
                    else
                        str += it.getToken();          // keep case used in this instance
                }
                else if (it.getID() & eTokenEventChain) // token is an event chain name
                {
                    const allEventChainNames & var = se.getEventChainNames();
                    if (var.isOptName(it.getToken()))
                        str += var.getOptName(it.getToken());
                    else
                        str += it.getToken();          // keep case used in this instance
                }
                else
                    str += it.getToken();
            }

            str.shrink_to_fit();
            return str;
        }

        // process the tokens, see if this is a variable declaration. if so return true, if not return false
        // errors can be filled in if invalid, caller checks for any errors added.
        // spaces can be removed if bFixSpace
        // vars is updated as complete declarations are detected.
        bool processVariableDecleration(const ScriptEngine& se, allUserVariables& vars, bool bFixSpace, ErrorWarning& errors)
        {
            if (m_processed)    // line has been processed, don't process again.
                return false;

            bool retval = false;
            std::size_t index = 0;

            if (index >= m_tokens.size())
                return false;

            ScriptToken& it = m_tokens[index];

            if (it.getID() & eTokenIgnore)
            {
                index++;
                if (index >= m_tokens.size())
                    return false;
            }

            if (processSpaces(it, bFixSpace, 0, errors))    // return true if a space or spaces and may change to ignorable or convert multiple to single
            {
                index++;
                if (index >= m_tokens.size())
                    return false;
            }

            it = m_tokens[index];
            if ((index <= 1) && (it.getID() & eTokenName))  // have token, see if it is one of the variable types (bool, int, float, string, intarrray, miner, building, vehicle, creature, arrow, timer)
            {
                if (it.getTokenlc() == se.kS_bool)
                    return processVariableDecl(variableType::eVarTypeBool, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_int)
                    return processVariableDecl(variableType::eVarTypeInt, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_float)
                    return processVariableDecl(variableType::eVarTypeFloat, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_string)
                    return processVariableDecl(variableType::eVarTypeString, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_intarray)
                    return processVariableDecl(variableType::eVarTypeIntArray, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_miner)
                    return processVariableDecl(variableType::eVarTypeMiner, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_building)
                    return processVariableDecl(variableType::eVarTypeBuilding, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_vehicle)
                    return processVariableDecl(variableType::eVarTypeVehicle, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_creature)
                    return processVariableDecl(variableType::eVarTypeCreature, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_arrow)
                    return processVariableDecl(variableType::eVarTypeArrow, index, se, vars, bFixSpace, errors);

                if (it.getTokenlc() == se.kS_timer)
                    return processVariableDecl(variableType::eVarTypeTimer, index, se, vars, bFixSpace, errors);

            }
            return retval;
        }

    protected:
        // return true if a space or spaces and may change to ignorable or convert multiple to single
        bool processSpaces(ScriptToken& it, bool bFixSpace, uint64_t mask, ErrorWarning& errors)
        {
            if (it.getID() & eTokenIgnore)          // already told to ignore this token
                return false;                       // no a space

            if (it.getID() & eTokenSpace)           // single space
            {
                if (!(mask & eTokenSpace))          // space not allowed
                {
                    if (bFixSpace)                  // allowed to fix
                    {
                        it.orID( eTokenIgnore );    // set ignore to comp
                    }
                    else
                    {
                        errors.setError(m_line,"Invalid space");
                    }
                }
                return true;
            }

            if (it.getID() & eTokenSpaces)      // sequence of spaces (more than one)
            {
                if (!(mask & eTokenSpace))      // no spaces allowed
                {
                    if (bFixSpace)                  // allowed to fix
                    {
                        it.orID( eTokenIgnore );    // set ignore to comp
                    }
                    else
                    {
                        errors.setError(m_line,"Invalid space - no spaces allowed");
                    }

                }
                else    // single space is allowed
                {
                    if (bFixSpace)                  // allowed to fix
                    {
                        it.setToken(" ");       // change to single space
                    }
                    else
                    {
                        errors.setError(m_line,"Invalid spaces - only single space allowed");
                    }
                }
                return true;
            }
        }


        // index is the token for type. format is space name optional =value(s) depending on type of variable.
        bool processVariableDecl(variableType::varType type, std::size_t index, const ScriptEngine& se, allUserVariables& vars, bool bFixSpace, ErrorWarning& errors)
        {
            m_processed = true;     // no matter what, line has now been processed

            index++;                //next token
            if (index >= m_tokens.size())
            {
                errors.setError(m_line, "missing space and variable name");
                return true;
            }

            // must have a space token
            if (!processSpaces(m_tokens[index], bFixSpace, eTokenSpace, errors))   // must have a space
            {
                errors.setError(m_line, "missing space after variable decleration");
                return true;
            }

            index++;
            if (index >= m_tokens.size())
            {
                errors.setError(m_line, "missing variable name");
                return true;
            }

            ScriptToken & it = m_tokens[index];
            if (!(it.getID() & eTokenName))
            {
                errors.setError(m_line, std::string("invalid variable name: ") + it.getToken());
                return true;
            }

            if (se.isReservedVar(it.getTokenlc()))
            {
                errors.setError(m_line, std::string("variable name is reserved keyword: ") + it.getTokenlc());
                return true;
            }

            if (vars.contains(it.getTokenlc()))
            {
                errors.setError(m_line,std::string("Duplicate variable name: ") + it.getToken());
            }

            if (se.getEventChainNames().contains(it.getTokenlc()))
            {
                errors.setError(m_line,std::string("Variable name duplicates EventChain name")+ it.getTokenlc());
            }

            // variable name is valid, and not already in use. start building the variable data
            variableTypeSP vtsp(std::make_shared<variableType>(type, it.getToken())); // the shared pointer is what is added to collections
            variableType * vt = vtsp.get();     // actual data

            index++;
            if (index < m_tokens.size())
            {
               // bool bOk = processVarDeclParms( type, index, se, vars, bFixSpace, errors, vt );
                //if(!bOk)
                //    return true;
            }

            vars.add( vtsp );  // add the variable
            return true;
        }

        // look at line starting at pos (it is the first $ char). Return true if we have valid $(name) format.
        // error will be filled in if $( found but name is invalid or no closing ) and false returned
        // if true returned, the  macronamelc is filled in with lower case name of the macro
        // if true returned, epos is next char past the closing )
        bool isMacro(const InputLine &iline, ErrorWarning & errors, std::string_view line, std::size_t spos, std::size_t & epos, std::string& macroname )
        {
            bool retval = false;
            std::size_t pos = spos;           // start
            epos = line.size();               // end (temp for now)
            if ((pos < epos) && (line[pos] == '$'))  // should always be true by caller
            {
                pos++;
                if ((pos < epos) && (line[pos] == '('))  // now have $( sequence.
                {
                    pos++;
                    for (epos = pos; (epos < line.size()) && (line[epos] != ')'); epos++)  // find closing )
                        ;
                    if (epos >= line.size())
                    {
                        errors.setError(iline,"Missing closing ) in macro");
                        return false;
                    }
                    if (epos - spos - 2 <= 0)
                    {
                        errors.setError(iline,"Missing macro name inside $()");
                        return false;
                    }
                    macroname = MMUtil::removeLeadingAndTrailingWhite(line.substr(spos+2,epos-spos-2));
                    if (MMUtil::isAlphaNumericName(macroname))
                        retval = true;
                    else
                        errors.setError(iline,std::string("Invalid macro name: ")+macroname);
                }
            }
            return retval;
        }

        bool embededMacro(std::string& token, ErrorWarning& errors, const Defines & defines)
        {
            bool bqMacro = false;
            do    // loop until no more interior macros
            {
                bqMacro = false;         // start off assuming no macro
                std::size_t stpos = token.find("$(");
                if (stpos != std::string::npos)     // found $(
                {
                    std::string macroname;
                    std::size_t endpos;
                    if (isMacro(m_line, errors, token, stpos, endpos, macroname))  // have macro
                    {
                        if (!defines.contains(macroname))
                        {
                            errors.setError(m_line, std::string("Unknown macro name: ") + macroname);
                            return false;
                        }
                        std::string macrovalue = defines.getValue(macroname);
                        if (macrovalue.empty())
                        {
                            errors.setError(m_line, "Empty macro value");
                            return false;
                        }
                        token.replace(stpos, endpos - stpos + 1, MMUtil::removeAllDoubleQuotes(macrovalue));
                        bqMacro = true;
                    }
                }
            } while (bqMacro);
            return true;
        }

        // this defines the allowed first char of a token. Note it does not allow any non-ascii variable names
        bool isKnown(char ch)
        {
            if (std::isdigit((unsigned char)ch) || std::isalpha((unsigned char)ch) || std::isblank((unsigned char)ch))
                return true;

            std::size_t npos =
                std::string_view("\",!=<>/*\\-+()[]:;.").find(ch);  
            return npos != std::string_view::npos;
        }


        InputLine               m_line;                // holds the line, linenumber, filename
        std::deque<ScriptToken> m_tokens;              // parsed tokens for this line
        bool                    m_processed = false;   // set to true when line has been completely processed
    };

public:
    // called if ignoring map script and loading from a script file.
    void setFileName(const std::filesystem::path& filename)
    {
        m_filename = filename;
    }

    const ErrorWarning &getErrors() const { return m_errors; }

    // passed in the lines loaded from the map script section.
    // if there is a filename setup, then this input is ignored
    // the file is processed
    bool loadScript(const std::deque<InputLine>& scriptSectionLines, const std::deque<std::filesystem::path> &incDirs, bool bAnsi )
    {
        bool retval = true;     // assume ok
        if (m_filename.empty())
            m_inputlines = scriptSectionLines;
        else
        {
            std::deque<std::filesystem::path> processed;    // how we check for circular reference and more directories to search
            retval = loadScriptFile(m_filename,incDirs, processed, InputLine(), bAnsi, "  ");
        }
        return retval;
    }

    // setup script macros with predefined ones. We have builtin ones plus any from the command line
    int addCmdDefines(const CommandLineOptions& cmdline, int rows, int cols, const std::string &mapname)
    {
        // first add the predefined ones
        m_defines.addKeyValue("TyabMapRows", std::to_string(rows));
        m_defines.addKeyValue("TyabMapCols", std::to_string(cols));
        m_defines.addKeyValue("TyabMapName", mapname);

        // compute date for main script
        std::string sdate = FileIO::getDateStr(m_filename, cmdline.m_datestr);
        if (sdate.empty() && !m_inputlines.empty())
        {
            sdate = FileIO::getDateStr(std::filesystem::path(Unicode::utf8_to_wstring(m_inputlines[0].getFileName())), cmdline.m_datestr);
        }

        m_defines.addKeyValue("TyabScriptDate", sdate);        // main script we know its date
        m_defines.addKeyValue("TyabMMUtilDate", __DATE__ );    // compile date for this utility

        for (auto it : cmdline.m_defines)
        {
            if (m_defines.contains(it.key()))
            {
                m_errors.setError(InputLine(), "Duplicate name=value from command line: " + std::string(it.key()));
                return 1;
            }
            if (isReservedWord(it.key()))
            {
                m_errors.setError(InputLine(), "name=value from command line is reserved name: " + std::string(it.key()));
                return 1;
            }

            m_defines.addKeyValue(it.key(), it.value());

        }
        return 0;
    }

    std::deque<InputLine> processInputLines( bool bFixSpace)
    {
        std::deque<InputLine> output;

        pass0Processing();

        if (!m_errors.emptyErrors())    // have errors, return empty output
            return output;

        // do all the processing. TODO
        pass1Processing( bFixSpace );
        // 
        // 
        //
        // end processing.

        // now generate the output. have each line serialize itself out, make into a new InputLine for output
        int linenum = 1;
        std::string filename;       // blank filename for output

        for (auto it : m_scriptlines)
        {
            output.push_back(InputLine(it.serialize_out(*this),linenum++,filename));
        }
        return output;
    }

protected:
    // pass 0 is parsing all input lines into tokens.
    void pass0Processing()
    {
        for (auto it : m_inputlines)
        {
            ScriptLine sl(it);
            sl.rawParse(m_errors, m_defines);
            m_scriptlines.push_back(sl);
        }
    }

    // pass 1 is collecting up all variables and event chain names.
    void pass1Processing( bool bFixSpace )
    {
        for (auto it : m_scriptlines)
        {

        }
    }




    // this is recursive. The passed in filename is to be loaded and lines added to input lines.
    // return false if unable to load file. Return true if file is loaded
    bool loadScriptFile(const std::filesystem::path & filename, const std::deque<std::filesystem::path> & incdirs, std::deque<std::filesystem::path> & processed, const InputLine &inputline, bool bAnsi, const std::string & indent )
    {
        if (filename.empty())
        {
            m_errors.setError(inputline,indent+"Missing filename");
            return false;   // unable to load empty file
        }

        // first thing is we look at the passed in file. Get only the filename, and compare it to all of the files processed
        // to detect circular reference
        std::filesystem::path filenameonly = filename.filename();
        std::filesystem::path pathonly = filename;
        pathonly.remove_filename();
        for (auto it : processed)
        {
            if (it.empty())     // should never happen we don't store empty paths
                continue;
            std::filesystem::path fnameonly = it.filename();
            if (filenameonly == fnameonly)
            {
                m_inputlines.push_back(InputLine(std::string("## WARNING: Include IGNORED: ")+Unicode::wstring_to_utf8(filename.wstring())+" - file already included",inputline.getLineNum(),inputline.getFileName()));
                m_errors.setWarning(inputline,indent+"Ignoring circular included file: " + Unicode::wstring_to_utf8(filename.wstring()));
                return true;    // continue processing
            }
        }

        // have not included this file before. We only add it if successful
        std::filesystem::path foundfullpath = FileIO::searchPaths(filename, incdirs, processed);
        if (foundfullpath.empty())
        {
            m_errors.setError(inputline,indent+"Unable to find: " + Unicode::wstring_to_utf8(filename.wstring()));
            return false;
        }

        FileIO                 fileio;      // how we read files.
        std::deque<InputLine>  lines;       // lines read from file.
        int linenum = 0;

        fileio.setFileName( foundfullpath );
        fileio.setAnsi( bAnsi );
        lines = fileio.readFile(m_errors);
        if (!m_errors.emptyErrors())        // if there are errors, fail and return them
            return false;
        if (lines.empty())
        {
            m_errors.setWarning(inputline,indent+"Empty file: " + Unicode::wstring_to_utf8(foundfullpath.wstring()));
            return true;    // contiunue processing
        }
        wprintf((Unicode::utf8_to_wstring(indent+"processing script: ")+foundfullpath.wstring()+L"\n").c_str());

        processed.push_back(foundfullpath);  // add this file as one processed

        constexpr char kPragma2[]           = "##pragma ";          // already processed pragma, ignore
        constexpr char kPragma1[]           = "#pragma ";           // beginning of pragma. Must be in pos 0.
        constexpr char kdefine []           = "define ";            // pragma define. Define key=value macro. key must be unique
        constexpr char ktyabscriptdate[]    = "tyabscriptdate ";    // following is date format, Add to line the processed date for main script file
        constexpr char ktyabscriptincdate[] = "tyabscriptincdate "; // following is date format. Add to line the processed data for current script file.
        constexpr char kinclude[]           = "include ";           // include the following script. Already included includes ignored.
        constexpr char ktyabMMDatUtil[]     = "tyabmmdatutil";      // add in this utility info.

        constexpr char kKeepComment[]       = "#.";                 // keep comment, ignore it

        // process every line, dealing with the pragmas
        for (auto lineit : lines)
        {
            linenum++;
            const InputLine &iline = lineit;                // line we are working on.
            std::string_view lineview = iline.getLine();    // string data.

            std::string linelc = MMUtil::toLower(lineview);
            std::string_view linelcview = linelc;
            if ((linelcview.find(kKeepComment) == 0) || (linelcview.find(kPragma2) == 0))    // skip already processsed lines or #. comments
            {
                m_inputlines.push_back(iline);
                continue;
            }
            std::size_t pos = linelcview.find(kPragma1);
            if (pos == std::string_view::npos)  // pragma not found, so normal line
            {
                m_inputlines.push_back(iline);
                continue;
            }
            if (pos != 0 || (linelcview.size() <= std::size(kPragma1))) // don't have pragma or too short
            {
                m_errors.setWarning(lineit,indent+"#pragma ignored if not in col 0 or missing option");
                m_inputlines.push_back(iline);
                continue;
            }

            // have "#pragma " in pos 0.
            linelcview = MMUtil::removeLeadingWhite(linelcview.substr(pos+std::size(kPragma1)-1));

            if (linelcview.find("define ") == 0)    // define new macro key=value
            {
                if (!processPragmaDefine(lineit, linelcview, sizeof(kdefine),indent))
                    return false;
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( InputLine (std::string("#")+iline.getLine(),iline.getLineNum(), iline.getFileName()) );
            }
            else if (linelcview.find(ktyabscriptdate) == 0) // add in file time info from main script file
            {
                std::string value;
                if (!processPragmatyabscriptdate(lineit, linelcview, sizeof(ktyabscriptdate), value, indent))
                    return false;
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( InputLine (std::string("#")+iline.getLine()+" "+value, iline.getLineNum(), iline.getFileName()));
            }
            else if (linelcview.find(ktyabscriptincdate) == 0) // add in file time info from current included script file
            {
                std::string value;
                if (!processPragmatyabscriptincdate(lineit, linelcview, sizeof(ktyabscriptincdate), foundfullpath, value, indent))
                    return false;
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( InputLine (std::string("#")+iline.getLine()+" "+value, iline.getLineNum(), iline.getFileName()));
            }
            else if (linelcview.find(kinclude) == 0)    // include another script file
            {
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( InputLine (std::string("#")+iline.getLine(),iline.getLineNum(), iline.getFileName()) );
                if (!processPragmaInclude(linelcview,sizeof(kinclude),incdirs, processed, lineit, bAnsi, indent))
                    return false;
            }
            else if (linelcview.find(ktyabMMDatUtil) == 0)  // info on this utility
            {
                std::string value;
                processPragmatyabMMDatUtil(value);
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( InputLine (std::string("#")+iline.getLine()+" "+value, iline.getLineNum(), iline.getFileName()));
            }
            else
            {
                m_errors.setWarning(lineit, indent+"unknown pragma, ignored");
                m_inputlines.push_back(iline);
            }
        }
        wprintf((Unicode::utf8_to_wstring(indent+"lines read: "+std::to_string(linenum)+" script: ")+filename.wstring() + L"\n").c_str());

        return true;
    }

    void processPragmatyabMMDatUtil(std::string &value)
    {
        value = std::string("MMDatUtil ") + DATE_ISO + " https://github.com/tyabnet/MMDatUtil";
    }

    bool processPragmaInclude(std::string_view line, std::size_t len, const std::deque<std::filesystem::path> & incdirs, std::deque<std::filesystem::path> & processed, const InputLine &lineit, bool bAnsi, const std::string & indent)
    {
        line = line.substr(len-1);    // skip past the token
        line = MMUtil::removeLeadingAndTrailingWhite(line);
        std::string_view fname;
        if (line.size() > 0)
        {
            if (line[0] == '\"')
            {
                std::size_t epos;
                for (epos=1;epos < line.size() && line[epos]!='\"'; epos++)
                    ;
                fname = line.substr(1,epos-1);
            }
            else
            {
                std::size_t epos;
                for (epos=1;epos < line.size() && !std::isblank((unsigned char)line[epos]); epos++)
                    ;
                fname = line.substr(0,epos);
            }
        }
        return loadScriptFile(std::filesystem::path(Unicode::utf8_to_wstring(fname)),incdirs, processed, lineit, bAnsi, indent + "  ");
    }



    bool processPragmatyabscriptincdate(const InputLine & /*lineit*/, std::string_view line, std::size_t len, const std::filesystem::path& fname, std::string& value, const std::string& /*indent*/)
    {
        line = line.substr(len-1);    // skip past the token
        line = MMUtil::removeLeadingAndTrailingWhite(line);
        // if quote, find matching end quote.
        std::string_view format = line;
        if (line.size() > 0)
        {
            if (line[0] == '\"')
            {
                std::size_t epos;
                for (epos=1;epos < line.size() && line[epos]!='\"'; epos++)
                    ;
                format = line.substr(0,epos);
            }
            else
            {
                std::size_t epos;
                for (epos=1;epos < line.size() && !std::isblank((unsigned char)line[epos]); epos++)
                    ;
                format = line.substr(0,epos);
            }
        }
        value = FileIO::getDateStr(fname, format.empty() ? "m.d.y" : format);
        return true;
    }


    bool processPragmatyabscriptdate(const InputLine& /*lineit*/, std::string_view line, std::size_t len, std::string& value, const std::string& /*indent*/)
    {
        line = line.substr(len-1);    // skip past the token
        line = MMUtil::removeLeadingAndTrailingWhite(line);
        // if quote, find matching end quote.
        if (line.size() > 0)
        {
            std::string_view format;
            if (line[0] == '\"')
            {
                std::size_t epos;
                for (epos=1;epos < line.size() && line[epos]!='\"'; epos++)
                    ;
                format = line.substr(0,epos);
            }
            else
            {
                std::size_t epos;
                for (epos=1;epos < line.size() && !std::isblank((unsigned char)line[epos]); epos++)
                    ;
                format = line.substr(0,epos);
            }
            if (!format.empty())
            {
                value = FileIO::getDateStr(m_filename.empty() ? std::filesystem::path(Unicode::utf8_to_wstring(m_inputlines[0].getFileName())) : m_filename, format);
            }
            else
                value = m_defines.getValue("TyabScriptDate");
        }
        else // no format string, so take the default
        {
            value = m_defines.getValue("TyabScriptDate");
        }
        return true;
    }


    // return false if invalid pragma define, true if pragma added
    bool processPragmaDefine(const InputLine &iline, std::string_view line, std::size_t len, const std::string & indent)
    {
        // find start of key
        line = line.substr(len-1);    // skip past the token
        std::size_t pos = line.find('=');
        if (pos != std::string_view::npos)
        {
            std::string_view key;
            std::string_view value;
            key = line.substr(0,pos);
            value = line.substr(pos+1);
            key = MMUtil::removeLeadingAndTrailingWhite(key);
            value = MMUtil::removeLeadingAndTrailingWhite(value);
            if (MMUtil::isAlphaNumericName(key))
            {
                if (!value.empty())
                {
                    if (value[0] == '\"')   // starts with quote, end at matching quote
                    {
                        pos = value.find('\"',1);
                        if (pos != std::string_view::npos)
                        {
                            value = value.substr(0,pos);
                        }
                        else
                        {
                            m_errors.setError(iline,indent+"missing closing \" in value");
                            return false;
                        }
                    }
                    else    // scan until blank or # char
                    {
                        for(pos = 1; pos < value.size() && !std::isblank((unsigned char)value[pos]) && value[pos] != '#';pos++)
                            ;
                        value = value.substr(0,pos);
                    }
                    if (!value.empty())
                    {
                        // key and value appear to be good. Check for key reserved word
                        std::string keystr(key);
                        if (isReservedWord(keystr))
                        {
                            m_errors.setError(iline, indent + "Macro name is reserved keyword");
                            return false;
                        }
                        // make sure it is not already defined
                        if (m_defines.contains(keystr))
                        {
                            m_errors.setError(iline, indent + "Macro name is already defined");
                            return false;
                        }
                        m_defines.addKeyValue(keystr,std::string(value));
                        return true;
                    }
                    else
                        m_errors.setError(iline,indent+"empty macro value");
                }
                else
                    m_errors.setError(iline,indent+"empty macro value");
            }
            else
                m_errors.setError(iline,indent+"invalid macro name");
        }
        else
            m_errors.setError(iline,indent+"#pragma define missing =, expecting macro=value");

        return false;
    }

    const allUserVariables   & getUserVariables()   const { return m_variableNames; }
    const allEventChainNames & getEventChainNames() const { return m_eventChainNames; }


protected:
    // walk the event chain and variable names, ignore init and tick, sort in order of reference,
    // and generate new names that are as short as possible
    void optimizeNames()
    {

    }


    // given the last optimized name returned give the next one. Start with nullstr to start from beginning
    // names are a-z if single length. z wraps to a0
    // 2 or more length are alpha followed by 0-9 or a-z. Thus az wraps to b0, zz wraps to a00
    // internal recrusive generate next name.
    std::string iNextName(const std::string& name ) const
    {
        std::size_t len = name.length();
        if (!len)
            return std::string("a");        // handle first time
        if (len == 1)  // single length is only a-z and wraps after z to a0
        {
            char current = name[0];
            assert(current >= 'a' && current <= 'z');
            if (current < 'z')
            {
                char str[2];
                str[0] = current + 1;
                str[1] = 0;
                return std::string(str);
            }
            return std::string("a0");   // next one after z
        }
        std::string prev = name.substr(0,len-1);
        char current = name[len-1];
        if (current == '9')  // 0-9 continue into a-z
        {
            current = 'a';
            return prev + current;
        }
        if (current < 'z')
        {
            current++;      // next numeric or next alpha
            return prev + current;
        }
        // wrap to 0, so increment the prev substring allowing az -> b0
        current = '0';      // wrap to 0.
        return iNextName(prev) + current;  // recursive
    }

    // uses iNextName to generate next name. Skips reserved words.
    std::string nextOptimizedName(const std::string& last) const
    {
        std::string next = last;
        do
        {
            next = iNextName(next);
        }
        while (isReservedVar(next));
        return next;
    }





    std::filesystem::path          m_filename;     // empty unless script file is replacing map script section. This is the main file.
    std::deque<InputLine>          m_inputlines;   // lines to process after all include and other pragmas 
    std::deque<InputLine>          m_outputlines;  // lines all processed ready to replace the script section

    std::deque<ScriptLine>         m_scriptlines;   // every line of script processed into tokens.

    allEventChainNames             m_eventChainNames;  // all event chain names
    allUserVariables               m_variableNames;    // all variable names

    ErrorWarning                   m_errors;
    Defines                        m_defines;      // user macros


};


#pragma pop_macro("min")
#pragma pop_macro("max")

#endif // _MMSCRIPT_H
