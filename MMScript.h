// CompressScript.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifndef _MMSCRIPT_H
#define _MMSCRIPT_H

#include <stdio.h>
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <format>

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

    void setSize( int rows, int cols ) { m_rows = rows; m_cols = cols; }
    int getRows() const { return m_rows; }
    int getCols() const { return m_cols; }

protected:
    // generic storage for a name that must not be optimized. Used by objectives and blocks sections
    class noOptimizeName
    {
    public:
        noOptimizeName() = default;
        noOptimizeName(InputLinePtr const& iline, std::string const& name)
            : m_iline(iline), m_name(name), m_namelc(MMUtil::toLower(name))
        {}

        InputLinePtr const &iline()  const { return m_iline; }
        std::string  const &name()   const { return m_name; }
        std::string  const &namelc() const { return m_namelc; }

    protected:
        InputLinePtr m_iline;      // line referenced in .dat file
        std::string  m_name;       // name used in line
        std::string  m_namelc;     // lower case version of name, use this for key
    };


    bool isReservedWord(const std::string& key)
    {
        return isReservedVar( key );
    }

    // script reserved words in mixed case Friendly format
    const std::string kStr_actionstations               = "actionstations";                  // enable/disable value
    const std::string kStr_addrandomspawn               = "addrandomspawn";                  // event
    const std::string kStr_air                          = "air";                             // macro int, amount of air
    const std::string kStr_arrow                        = "arrow";                           // variable type
    const std::string kStr_Barrier_C                    = "Barrier_C";                       // collection, building
    const std::string kStr_Bat                          = "Bat";                             // collection, creature
    const std::string kStr_Bat_C                        = "Bat_C";                           // collection, creature
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
    const std::string kStr_Canteen_C                    = "Canteen_C";                       // macro, int, number of Canteens.
    const std::string kStr_cargocarrier                 = "cargocarrier";                    // macro, int number of Cargo Carriers.
    const std::string kStr_CargoCarrier_C               = "CargoCarrier_C";                  // macro, int number of Cargo Carriers.
    const std::string kStr_change                       = "change";                          // trigger
    const std::string kStr_chromecrusher                = "chromecrusher";                   // macro int, number of Chrome Crushers
    const std::string kStr_ChromeCrusher_C              = "ChromeCrusher_C";                 // macro int, number of Chrome Crushers
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
    const std::string kStr_docks                        = "docks";                           // Macro, int, number of docks
    const std::string kStr_Docks_C                      = "Docks_C";                         // Macro, int, number of docks
    const std::string kStr_drain                        = "drain";                           // event, int, number of crystals to drain
    const std::string kStr_drill                        = "drill";                           // Event / Trigger, Drill tile.
    const std::string kStr_drive                        = "drive";                           // Trigger, Trigger when vehicle over tile.
    const std::string kStr_driven                       = "driven";                          // Data Field Trigger|Trigger when a miner enters a vehicle.
    const std::string kStr_driver                       = "driver";                          // Data Field, miner id of the driver.Same as driverid.
    const std::string kStr_driverid                     = "driverid";                        // Data Field, miner id of the driver.Same as driver.
    const std::string kStr_dynamite                     = "dynamite";                        // Parameter, used with enable/disable.
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
    const std::string kStr_false                        = "false";                           // | bool value or 0.
    const std::string kStr_flee                         = "flee";                            // | Event | Monster flees |
    const std::string kStr_float                        = "float";                           // | Variable | Floating point number. |
    const std::string kStr_geologicalcenter             = "geologicalcenter";                // | Macro | Number of Geological Centers. |
    const std::string kStr_GeologicalCenter_C           = "GeologicalCenter_C";              // | Macro | Number of Geological Centers. |
    const std::string kStr_get                          = "get";                             // | Macro | Get tile ID., get(row)(col) |
    const std::string kStr_granitegrinder               = "granitegrinder";                  // | Macro | Number of Granite Grinders. |
    const std::string kStr_GraniteGrinder_C             = "GraniteGrinder_C";                // | Macro | Number of Granite Grinders. |
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
    const std::string kStr_HoverScout_C                 = "HoverScout_C";                    // | Macro | Number of Hover Scouts. |
    const std::string kStr_hp                           = "hp";                              // | Data Field | Object hitpoints.Same has health. |
    const std::string kStr_hurt                         = "hurt";                            // | Data Field Trigger | Trigger when object takes damage. |
    const std::string kStr_IceMonster                   = "IceMonster";                      // | Collection | Ice Monsters. |
    const std::string kStr_IceMonster_C                 = "IceMonster_C";                    // | Collection | Ice Monsters. |
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
    const std::string kStr_LavaMonster_C                = "LavaMonster_C";                   // | Collection | Lava Monsters. |
    const std::string kStr_level                        = "level";                           // | Data Field | Returns upgrade level of the building. |
    const std::string kStr_levelup                      = "levelup";                         // | Data Field Trigger | Building leveled up. |
    const std::string kStr_light                        = "light";                           // | Parameter | enable / disable parameter. |
    const std::string kStr_lights                       = "lights";                          // | Parameter | same as light. |
    const std::string kStr_LMLC                         = "LMLC";                            // | Macro | Number of Large Mobile Laser Cutters. |
    const std::string kStr_LMLC_C                       = "LMLC_C";                          // | Macro | Number of Large Mobile Laser Cutters. |
    const std::string kStr_loaderdozer                  = "loaderdozer";                     // | Macro | Number of Loader Dozers. |
    const std::string kStr_LoaderDozer_C                = "LoaderDozer_C";                   // | Macro | Number of Loader Dozers. |
    const std::string kStr_loose_rock                   = "loose_rock";                      // | Macro | Tile ID of loose rock(30). |
    const std::string kStr_lose                         = "lose";                            // | Event | Lose the map. |
    const std::string kStr_miner                        = "miner";                           // | Variable / Class | Miner object or class. |
    const std::string kStr_miners                       = "miners";                          // | Macro | Miners discovered or active. |
    const std::string kStr_mininglaser                  = "mininglaser";                     // | Macro | Number of Geological Centers. |
    const std::string kStr_MiningLaser_C                = "MiningLaser_C";                   // | Macro | Number of Geological Centers. |
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
    const std::string kStr_OreRefinery_C                = "OreRefinery_C";                   // | Macro | Number of Ore Refineries. |
    const std::string kStr_ore_seam                     = "ore_seam";                        // | Macro | Tile ID of an ore seam(46). |
    const std::string kStr_pan                          = "pan";                             // | Event | Pan camera. |
    const std::string kStr_pause                        = "pause";                           // | Event | Pauses the game. |
    const std::string kStr_place                        = "place";                           // | Event | Change tile. |
    const std::string kStr_power                        = "power";                           // | Data Field | Returns 1 if the building has power, 0 if it doesn't. |
    const std::string kStr_powered                      = "powered";                         // | Data Field | Same as power. |
    const std::string kStr_poweroff                     = "poweroff";                        // | Data Field Trigger | Trigger when power is deactivated for a building.
    const std::string kStr_poweron                      = "poweron";                         // | Data Field Trigger | Trigger when power is activated for a building.
    const std::string kStr_powerpath                    = "powerpath";                       // | Paramter | enable/disable parameter
    const std::string kStr_PowerPath_C                  = "PowerPath_C";                     // | Macro | Number of power paths |
    const std::string kStr_powerstation                 = "powerstation";                    // | Macro | Number of Power Stations. |
    const std::string kStr_PowerStation_C               = "PowerStation_C";                  // | Macro | Number of Power Stations. |
    const std::string kStr_progress_path                = "progress_path";                   // | Macro | Tile id of a progress path(13). |
    const std::string kStr_qmsg                         = "qmsg";                            // | Event | Display message to user. |
    const std::string kStr_rapidrider                   = "rapidrider";                      // | Macro | Number of Rapid Riders. |
    const std::string kStr_RapidRider_C                 = "RapidRider_C";                    // | Macro | Number of Rapid Riders. |
    const std::string kStr_random                       = "random";                          // | Macro | return random number, random(low)(high) |
    const std::string kStr_randomspawn                  = "randomspawn";                     // | Event | Spawn the given creature class in random location |
    const std::string kStr_RechargeSeamGoal_C           = "RechargeSeamGoal_C";              // | Collection | Visible recharge seams. |
    const std::string kStr_red                          = "red";                             // | Color | Arrow colors. |
    const std::string kStr_reinforce                    = "reinforce";                       // | Trigger | Trigger when wall is reinforced. |
    const std::string kStr_reset                        = "reset";                           // | Event | Resets the player's selection |
    const std::string kStr_resetspeed                   = "resetspeed";                      // | Event | Loads the game speed from settings again. |
    const std::string kStr_resume                       = "resume";                          // | Event | Same as unpause. |
    const std::string kStr_return                       = "return";                          // | Event | return event |                          
    const std::string kStr_RockMonster                  = "RockMonster";                     // | Collection | Rock Monsters. |
    const std::string kStr_RockMonster_C                = "RockMonster_C";                   // | Collection | Rock Monsters. |
    const std::string kStr_row                          = "row";                             // | Data Field | Object row. |
    const std::string kStr_S                            = "S";                               // | Emerge Direction | South. |
    const std::string kStr_save                         = "save";                            // | Event | Save last miner that activated a trigger into a variable. |
    const std::string kStr_savebuilding                 = "savebuilding";                    // | Event | Save last building that activated a trigger into a variable.
    const std::string kStr_savecreature                 = "savecreature";                    // | Event | Save last creature that activated a trigger into a variable.
    const std::string kStr_savevehicle                  = "savevehicle";                     // | Event | Save last vehicle that activated a triggger into a variable.
    const std::string kStr_shake                        = "shake";                           // | Event | Shake camera. |
    const std::string kStr_showarrow                    = "showarrow";                       // | Event | Show arrow object. |
    const std::string kStr_SlimySlug                    = "SlimySlug";                       // | Collection | Slimy Slugs. |
    const std::string kStr_SlimySlug_C                  = "SlimySlug_C";                     // | Collection | Slimy Slugs. |
    const std::string kStr_slug_hole                    = "slug_hole";                       // | Macro | Tile id of slimy slug hole(12). |
    const std::string kStr_slugs                        = "slugs";                           // | Macro | Number of slimy slugs. |
    const std::string kStr_smalldigger                  = "smalldigger";                     // | Macro | Number of Small Diggers. |
    const std::string kStr_SmallDigger_C                = "SmallDigger_C";                   // | Macro | Number of Small Diggers. |
    const std::string kStr_SmallSpider                  = "SmallSpider";                     // | Collection | Small Spiders. |
    const std::string kStr_SmallSpider_C                = "SmallSpider_C";                   // | Collection | Small Spiders. |
    const std::string kStr_smalltransporttruck          = "smalltransporttruck";             // | Macro | Number of Small Transport Trucks. |
    const std::string kStr_SmallTransportTruck_C        = "SmallTransportTruck_C";           // | Macro | Number of Small Transport Trucks. |
    const std::string kStr_SMLC                         = "SMLC";                            // | Macro | Number of Small Mobile Laser Cutters |
    const std::string kStr_SMLC_C                       = "SMLC_C";                          // | Macro | Number of Small Mobile Laser Cutters |
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
    const std::string kStr_SuperTeleport_C              = "SuperTeleport_C";                 // | Macro | Number of Super Teleports. |
    const std::string kStr_supportstation               = "supportstation";                  // | Macro | Number of Support Stations. |
    const std::string kStr_SupportStation_C             = "SupportStation_C";                // | Macro | Number of Support Stations. |
    const std::string kStr_teleportpad                  = "teleportpad";                     // | Macro | Numbewr of Teleport Pads. |
    const std::string kStr_TeleportPad_C                = "TeleportPad_C";                   // | Macro | Numbewr of Teleport Pads. |
    const std::string kStr_tick                         = "tick";                            // | Event Chain | Called on every engine tick.Not recommended. |
    const std::string kStr_tile                         = "tile";                            // | Data Field | TileID for objectt. |
    const std::string kStr_tileid                       = "tileid";                          // | Data Field | same as tile. |
    const std::string kStr_time                         = "time";                            // | Macro / Trigger | Game time or trigger. |
    const std::string kStr_timer                        = "timer";                           // | Variable | Timer object. |
    const std::string kStr_toolstore                    = "toolstore";                       // | Macro | Returns number of toolstores. |
    const std::string kStr_Toolstore_C                  = "Toolstore_C";                     // | Macro | Returns number of toolstores. |
    const std::string kStr_true                         = "true";                            // | bool value or 1.
    const std::string kStr_truewait                     = "truewait";                        // | Event | Suspend event chain for real user time period. |
    const std::string kStr_tunnelscout                  = "tunnelscout";                     // | Macro | Number of Tunnel Scouts. |
    const std::string kStr_TunnelScout_C                = "TunnelScout_C";                   // | Macro | Number of Tunnel Scouts. |
    const std::string kStr_tunneltransport              = "tunneltransport";                 // | Macro | Number of Tunnel Transports. |
    const std::string kStr_TunnelTransport_C            = "TunnelTransport_C";               // | Macro | Number of Tunnel Transports. |
    const std::string kStr_upgrade                      = "upgrade";                         // | Trigger when vehicle is upgraded. |
    const std::string kStr_upgraded                     = "upgraded";                        // | Trigger | Not working, don't use. |`
    const std::string kStr_upgradestation               = "upgradestation";                  // | Macro | Number of Upgrade Stations. |
    const std::string kStr_UpgradeStation_C             = "UpgradeStation_C";                // | Macro | Number of Upgrade Stations. |
    const std::string kStr_unpause                      = "unpause";                         // | Event | Resumes the game if paused. |
    const std::string kStr_variable                     = "variable";                        // | objective section reserved word |
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
    const std::string kStr_wake                         = "wake";                            // | Event | Wakeup monster | monster variable. |
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
    const std::string kS_actionstations               = MMUtil::toLower( kStr_actionstations               );   // enable/disable value
    const std::string kS_A                            = MMUtil::toLower( kStr_A                            );   // emerge direction
    const std::string kS_addrandomspawn               = MMUtil::toLower( kStr_addrandomspawn               );   // event
    const std::string kS_air                          = MMUtil::toLower( kStr_air                          );   // macro int, amount of air
    const std::string kS_arrow                        = MMUtil::toLower( kStr_arrow                        );   // variable type
    const std::string kS_Barrier_C                    = MMUtil::toLower( kStr_Barrier_C                    );   // collection, building
    const std::string kS_Bat                          = MMUtil::toLower( kStr_Bat                          );   // collection, creature
    const std::string kS_Bat_C                        = MMUtil::toLower( kStr_Bat_C                        );   // collection, creature
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
    const std::string kS_Canteen_C                    = MMUtil::toLower( kStr_Canteen_C                    );   // macro, int, number of Canteens.
    const std::string kS_cargocarrier                 = MMUtil::toLower( kStr_cargocarrier                 );   // macro, int number of Cargo Carriers.
    const std::string kS_CargoCarrier_C               = MMUtil::toLower( kStr_CargoCarrier_C               );   // macro, int number of Cargo Carriers.
    const std::string kS_change                       = MMUtil::toLower( kStr_change                       );   // trigger
    const std::string kS_chromecrusher                = MMUtil::toLower( kStr_chromecrusher                );   // macro int, number of Chrome Crushers
    const std::string kS_ChromeCrusher_C              = MMUtil::toLower( kStr_ChromeCrusher_C              );   // macro int, number of Chrome Crushers
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
    const std::string kS_Docks_C                      = MMUtil::toLower( kStr_Docks_C                      );   // Macro, Number of Docks.
    const std::string kS_drain                        = MMUtil::toLower( kStr_drain                        );   // event, int, number of crystals to drain
    const std::string kS_drill                        = MMUtil::toLower( kStr_drill                        );   // Event / Trigger, Drill tile.
    const std::string kS_drive                        = MMUtil::toLower( kStr_drive                        );   // Trigger, Trigger when vehicle over tile.
    const std::string kS_driven                       = MMUtil::toLower( kStr_driven                       );   // Data Field Trigger|Trigger when a miner enters a vehicle.
    const std::string kS_driver                       = MMUtil::toLower( kStr_driver                       );   // Data Field, miner id of the driver.Same as driverid.
    const std::string kS_driverid                     = MMUtil::toLower( kStr_driverid                     );   // Data Field, miner id of the driver.Same as driver.
    const std::string kS_dynamite                     = MMUtil::toLower( kStr_dynamite                     );   // Parameter, used with enable/disable.
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
    const std::string kS_false                        = MMUtil::toLower( kStr_false                        );   // | bool value or 0. |
    const std::string kS_flee                         = MMUtil::toLower( kStr_flee                         );   // | Event | Monster flee | Monster variable |
    const std::string kS_float                        = MMUtil::toLower( kStr_float                        );   // | Variable | Floating point number. |
    const std::string kS_geologicalcenter             = MMUtil::toLower( kStr_geologicalcenter             );   // | Macro | Number of Geological Centers. |
    const std::string kS_GeologicalCenter_C           = MMUtil::toLower( kStr_GeologicalCenter_C           );   // | Macro | Number of Geological Centers. |
    const std::string kS_get                          = MMUtil::toLower( kStr_get                          );   // | Macro | Get tile ID. |
    const std::string kS_granitegrinder               = MMUtil::toLower( kStr_granitegrinder               );   // | Macro | Number of Granite Grinders. |
    const std::string kS_GraniteGrinder_C             = MMUtil::toLower( kStr_GraniteGrinder_C             );   // | Macro | Number of Granite Grinders. |
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
    const std::string kS_HoverScout_C                 = MMUtil::toLower( kStr_HoverScout_C                 );   // | Macro | Number of Hover Scouts. |
    const std::string kS_hp                           = MMUtil::toLower( kStr_hp                           );   // | Data Field | Object hitpoints.Same has health. |
    const std::string kS_hurt                         = MMUtil::toLower( kStr_hurt                         );   // | Data Field Trigger | Trigger when object takes damage. |
    const std::string kS_IceMonster                   = MMUtil::toLower( kStr_IceMonster                   );   // | Collection | Ice Monsters. |
    const std::string kS_IceMonster_C                 = MMUtil::toLower( kStr_IceMonster_C                 );   // | Collection | Ice Monsters. |
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
    const std::string kS_LavaMonster_C                = MMUtil::toLower( kStr_LavaMonster_C                );   // | Collection | Lava Monsters. |
    const std::string kS_level                        = MMUtil::toLower( kStr_level                        );   // | Data Field | Returns upgrade level of the building. |
    const std::string kS_levelup                      = MMUtil::toLower( kStr_levelup                      );   // | Data Field Trigger | Building upgraded. |
    const std::string kS_light                        = MMUtil::toLower( kStr_light                        );   // | Parameter | enable / disable parameter. |
    const std::string kS_lights                       = MMUtil::toLower( kStr_lights                       );   // | Parameter | same as light. |
    const std::string kS_LMLC                         = MMUtil::toLower( kStr_LMLC                         );   // | Macro | Number of Large Mobile Laser Cutters. |
    const std::string kS_LMLC_C                       = MMUtil::toLower( kStr_LMLC_C                       );   // | Macro | Number of Large Mobile Laser Cutters. |
    const std::string kS_loaderdozer                  = MMUtil::toLower( kStr_loaderdozer                  );   // | Macro | Number of Loader Dozers. |
    const std::string kS_LoaderDozer_C                = MMUtil::toLower( kStr_LoaderDozer_C                );   // | Macro | Number of Loader Dozers. |
    const std::string kS_loose_rock                   = MMUtil::toLower( kStr_loose_rock                   );   // | Macro | Tile ID of loose rock(30). |
    const std::string kS_lose                         = MMUtil::toLower( kStr_lose                         );   // | Event | Lose the map. |
    const std::string kS_miner                        = MMUtil::toLower( kStr_miner                        );   // | Variable / Class | Miner object or class. |
    const std::string kS_miners                       = MMUtil::toLower( kStr_miners                       );   // | Macro | Miners discovered or active. |
    const std::string kS_mininglaser                  = MMUtil::toLower( kStr_mininglaser                  );   // | Macro | Number of Mining Lasers. |
    const std::string kS_MiningLaser_C                = MMUtil::toLower( kStr_MiningLaser_C                );   // | Macro | Number of Mining Lasers. |
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
    const std::string kS_OreRefinery_C                = MMUtil::toLower( kStr_OreRefinery_C                );   // | Macro | Number of Ore Refineries. |
    const std::string kS_ore_seam                     = MMUtil::toLower( kStr_ore_seam                     );   // | Macro | Tile ID of an ore seam(46). |
    const std::string kS_pan                          = MMUtil::toLower( kStr_pan                          );   // | Event | Pan camera. |
    const std::string kS_pause                        = MMUtil::toLower( kStr_pause                        );   // | Event | Pauses the game. |
    const std::string kS_place                        = MMUtil::toLower( kStr_place                        );   // | Event | Change tile. |
    const std::string kS_power                        = MMUtil::toLower( kStr_power                        );   // | Data Field | Returns 1 if the building has power, 0 if it doesn't. |
    const std::string kS_powered                      = MMUtil::toLower( kStr_powered                      );   // | Data Field | Same as power. |
    const std::string kS_poweroff                     = MMUtil::toLower( kStr_poweroff                     );   // | Data Field Trigger | Trigger when power is deactivated for a building.
    const std::string kS_poweron                      = MMUtil::toLower( kStr_poweron                      );   // | Data Field Trigger | Trigger when power is activated for a building.
    const std::string kS_powerpath                    = MMUtil::toLower( kStr_powerpath                    );   // | Paramter | enable/disable parameter
    const std::string kS_PowerPath_C                  = MMUtil::toLower( kStr_PowerPath_C                  );   // | Macro | Number of power paths |
    const std::string kS_powerstation                 = MMUtil::toLower( kStr_powerstation                 );   // | Macro | Number of Power Stations. |
    const std::string kS_PowerStation_C               = MMUtil::toLower( kStr_PowerStation_C               );   // | Macro | Number of Power Stations. |
    const std::string kS_progress_path                = MMUtil::toLower( kStr_progress_path                );   // | Macro | Tile id of a progress path(13). |
    const std::string kS_qmsg                         = MMUtil::toLower( kStr_qmsg                         );   // | Event | Display message to user. |
    const std::string kS_random                       = MMUtil::toLower( kStr_random                       );   // | Macro | return random number, random(low)(high) |
    const std::string kS_randomspawn                  = MMUtil::toLower( kStr_randomspawn                  );   // | Event | Spawn the given creature class in random location
    const std::string kS_rapidrider                   = MMUtil::toLower( kStr_rapidrider                   );   // | Macro | Number of Rapid Riders. |
    const std::string kS_RapidRider_C                 = MMUtil::toLower( kStr_RapidRider_C                 );   // | Macro | Number of Rapid Riders. |
    const std::string kS_RechargeSeamGoal_C           = MMUtil::toLower( kStr_RechargeSeamGoal_C           );   // | Collection | Visible recharge seams. |
    const std::string kS_red                          = MMUtil::toLower( kStr_red                          );   // | Color | Arrow colors. |
    const std::string kS_reinforce                    = MMUtil::toLower( kStr_reinforce                    );   // | Trigger | Trigger when wall is reinforced. |
    const std::string kS_reset                        = MMUtil::toLower( kStr_reset                        );   // | Event | Resets the player's selection |
    const std::string kS_resetspeed                   = MMUtil::toLower( kStr_resetspeed                   );   // | Event | Loads the game speed from settings again. |
    const std::string kS_resume                       = MMUtil::toLower( kStr_resume                       );   // | Event | Same as unpause. |
    const std::string kS_return                       = MMUtil::toLower( kStr_return                       );   // | Event | return event |
    const std::string kS_RockMonster                  = MMUtil::toLower( kStr_RockMonster                  );   // | Collection | Rock Monsters. |
    const std::string kS_RockMonster_C                = MMUtil::toLower( kStr_RockMonster_C                );   // | Collection | Rock Monsters. |
    const std::string kS_row                          = MMUtil::toLower( kStr_row                          );   // | Data Field | Object row. |
    const std::string kS_S                            = MMUtil::toLower( kStr_S                            );   // | Emerge Direction | South. |
    const std::string kS_save                         = MMUtil::toLower( kStr_save                         );   // | Event | Save last miner that activated a trigger into a variable. |
    const std::string kS_savebuilding                 = MMUtil::toLower( kStr_savebuilding                 );   // | Event | Save last building that activated a trigger into a variable.
    const std::string kS_savecreature                 = MMUtil::toLower( kStr_savecreature                 );   // | Event | Save last creature that activated a trigger into a variable.
    const std::string kS_savevehicle                  = MMUtil::toLower( kStr_savevehicle                  );   // | Event | Save last vehicle that activated a triggger into a variable.
    const std::string kS_shake                        = MMUtil::toLower( kStr_shake                        );   // | Event | Shake camera. |
    const std::string kS_showarrow                    = MMUtil::toLower( kStr_showarrow                    );   // | Event | Show arrow object. |
    const std::string kS_SlimySlug                    = MMUtil::toLower( kStr_SlimySlug                    );   // | Collection | Slimy Slugs. |
    const std::string kS_SlimySlug_C                  = MMUtil::toLower( kStr_SlimySlug_C                  );   // | Collection | Slimy Slugs. |
    const std::string kS_slug_hole                    = MMUtil::toLower( kStr_slug_hole                    );   // | Macro | Tile id of slimy slug hole(12). |
    const std::string kS_slugs                        = MMUtil::toLower( kStr_slugs                        );   // | Macro | Number of slimy slugs. |
    const std::string kS_smalldigger                  = MMUtil::toLower( kStr_smalldigger                  );   // | Macro | Number of Small Diggers. |
    const std::string kS_SmallDigger_C                = MMUtil::toLower( kStr_SmallDigger_C                );   // | Macro | Number of Small Diggers. |
    const std::string kS_SmallSpider                  = MMUtil::toLower( kStr_SmallSpider                  );   // | Collection | Small Spiders. |
    const std::string kS_SmallSpider_C                = MMUtil::toLower( kStr_SmallSpider_C                );   // | Collection | Small Spiders. |
    const std::string kS_smalltransporttruck          = MMUtil::toLower( kStr_smalltransporttruck          );   // | Macro | Number of Small Transport Trucks. |
    const std::string kS_SmallTransportTruck_C        = MMUtil::toLower( kStr_SmallTransportTruck_C        );   // | Macro | Number of Small Transport Trucks. |
    const std::string kS_SMLC                         = MMUtil::toLower( kStr_SMLC                         );   // | Macro | Number of Small Mobile Laser Cutters |
    const std::string kS_SMLC_C                       = MMUtil::toLower( kStr_SMLC_C                       );   // | Macro | Number of Small Mobile Laser Cutters |
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
    const std::string kS_SuperTeleport_C              = MMUtil::toLower( kStr_SuperTeleport_C              );   // | Macro | Number of Super Teleports. |
    const std::string kS_supportstation               = MMUtil::toLower( kStr_supportstation               );   // | Macro | Number of Support Stations. |
    const std::string kS_SupportStation_C             = MMUtil::toLower( kStr_SupportStation_C             );   // | Macro | Number of Support Stations. |
    const std::string kS_teleportpad                  = MMUtil::toLower( kStr_teleportpad                  );   // | Macro | Numbewr of Teleport Pads. |
    const std::string kS_TeleportPad_C                = MMUtil::toLower( kStr_TeleportPad_C                );   // | Macro | Numbewr of Teleport Pads. |
    const std::string kS_tick                         = MMUtil::toLower( kStr_tick                         );   // | Event Chain | Called on every engine tick.Not recommended. |
    const std::string kS_tile                         = MMUtil::toLower( kStr_tile                         );   // | Data Field | TileID for objectt. |
    const std::string kS_tileid                       = MMUtil::toLower( kStr_tileid                       );   // | Data Field | same as tile. |
    const std::string kS_time                         = MMUtil::toLower( kStr_time                         );   // | Macro / Trigger | Game time or trigger. |
    const std::string kS_timer                        = MMUtil::toLower( kStr_timer                        );   // | Variable | Timer object. |
    const std::string kS_toolstore                    = MMUtil::toLower( kStr_toolstore                    );   // | Macro | Returns number of toolstores. |
    const std::string kS_Toolstore_C                  = MMUtil::toLower( kStr_Toolstore_C                  );   // | Macro | Returns number of toolstores. |
    const std::string kS_true                         = MMUtil::toLower( kStr_true                         );   // | bool value or 1. |
    const std::string kS_truewait                     = MMUtil::toLower( kStr_truewait                     );   // | Event | Suspend event chain for real user time period. |
    const std::string kS_tunnelscout                  = MMUtil::toLower( kStr_tunnelscout                  );   // | Macro | Number of Tunnel Scouts. |
    const std::string kS_TunnelScout_C                = MMUtil::toLower( kStr_TunnelScout_C                );   // | Macro | Number of Tunnel Scouts. |
    const std::string kS_tunneltransport              = MMUtil::toLower( kStr_tunneltransport              );   // | Macro | Number of Tunnel Transports. |
    const std::string kS_TunnelTransport_C            = MMUtil::toLower( kStr_TunnelTransport_C            );   // | Macro | Number of Tunnel Transports. |
    const std::string kS_upgrade                      = MMUtil::toLower( kStr_upgrade                      );   // | Trigger when vehicle is upgraded. |
    const std::string kS_upgraded                     = MMUtil::toLower( kStr_upgraded                     );   // | Trigger | Not working, don't use. |
    const std::string kS_upgradestation               = MMUtil::toLower( kStr_upgradestation               );   // | Macro | Number of Upgrade Stations. |
    const std::string kS_UpgradeStation_C             = MMUtil::toLower( kStr_UpgradeStation_C             );   // | Macro | Number of Upgrade Stations. |
    const std::string kS_unpause                      = MMUtil::toLower( kStr_unpause                      );   // | Event | Resumes the game if paused. |
    const std::string kS_variable                     = MMUtil::toLower( kStr_variable                     );   // | objective section reserved word |
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
    const std::string kS_wake                         = MMUtil::toLower( kStr_wake                         );   // | Event | Monster wakes | Monster ID |
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
    using  ReservedWordsType = std::unordered_map<std::string, std::string>;

    // map contains all reserved words, key is lower case, value is friendly name
    // init and tick are not added since the script developer provides those optional event chains.
    // thus variable names are also checked for init and tick since they are event chain only.
    const ReservedWordsType ReservedWordsMap =
    {
        { kS_A                            , kStr_A                            },   // emerge direction
        { kS_actionstations               , kStr_actionstations               },   // enable/disable value
        { kS_addrandomspawn               , kStr_addrandomspawn               },   // event
        { kS_air                          , kStr_air                          },   // macro int, amount of air
        { kS_arrow                        , kStr_arrow                        },   // variable type
        { kS_Barrier_C                    , kStr_Barrier_C                    },   // collection, building
        { kS_Bat                          , kStr_Bat                          },   // collection, creature
        { kS_Bat_C                        , kStr_Bat_C                        },   // collection, creature
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
        { kS_Canteen_C                    , kStr_Canteen_C                    },   // macro, int, number of Canteens.
        { kS_cargocarrier                 , kStr_cargocarrier                 },   // macro, int number of Cargo Carriers.
        { kS_CargoCarrier_C               , kStr_CargoCarrier_C               },   // macro, int number of Cargo Carriers.
        { kS_change                       , kStr_change                       },   // trigger
        { kS_chromecrusher                , kStr_chromecrusher                },   // macro int, number of Chrome Crushers
        { kS_ChromeCrusher_C              , kStr_ChromeCrusher_C              },   // macro int, number of Chrome Crushers
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
        { kS_Docks_C                      , kStr_Docks_C                      },   // Macro, Number of Docks.
        { kS_drain                        , kStr_drain                        },   // event, int, number of crystals to drain
        { kS_drill                        , kStr_drill                        },   // Event / Trigger, Drill tile.
        { kS_drive                        , kStr_drive                        },   // Trigger, Trigger when vehicle over tile.
        { kS_driven                       , kStr_driven                       },   // Data Field Trigger|Trigger when a miner enters a vehicle.
        { kS_driver                       , kStr_driver                       },   // Data Field, miner id of the driver.Same as driverid.
        { kS_driverid                     , kStr_driverid                     },   // Data Field, miner id of the driver.Same as driver.
        { kS_dynamite                     , kStr_dynamite                     },   // Parameter, used with enable/disable.
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
        { kS_false                        , kStr_false                        },   // | bool value or 0. |
        { kS_flee                         , kStr_flee                         },   // | Event | Monster flees | Monster ID |
        { kS_float                        , kStr_float                        },   // | Variable | Floating point number. |
        { kS_geologicalcenter             , kStr_geologicalcenter             },   // | Macro | Number of Geological Centers. |
        { kS_GeologicalCenter_C           , kStr_GeologicalCenter_C           },   // | Macro | Number of Geological Centers. |
        { kS_get                          , kStr_get                          },   // | Macro | Get tile ID. |
        { kS_granitegrinder               , kStr_granitegrinder               },   // | Macro | Number of Granite Grinders. |
        { kS_GraniteGrinder_C             , kStr_GraniteGrinder_C             },   // | Macro | Number of Granite Grinders. |
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
        { kS_HoverScout_C                 , kStr_HoverScout_C                 },   // | Macro | Number of Hover Scouts. |
        { kS_hp                           , kStr_hp                           },   // | Data Field | Object hitpoints.Same has health. |
        { kS_hurt                         , kStr_hurt                         },   // | Data Field Trigger | Trigger when object takes damage. |
        { kS_IceMonster                   , kStr_IceMonster                   },   // | Collection | Ice Monsters. |
        { kS_IceMonster_C                 , kStr_IceMonster_C                 },   // | Collection | Ice Monsters. |
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
        { kS_LavaMonster_C                , kStr_LavaMonster_C                },   // | Collection | Lava Monsters. |
        { kS_level                        , kStr_level                        },   // | Data Field | Returns upgrade level of the building. |
        { kS_levelup                      , kStr_levelup                      },   // | Data Field Trigger| Buklding upgraded. |
        { kS_light                        , kStr_light                        },   // | Parameter | enable / disable parameter. |
        { kS_lights                       , kStr_lights                       },   // | Parameter | same as light. |
        { kS_LMLC                         , kStr_LMLC                         },   // | Macro | Number of Large Mobile Laser Cutters. |
        { kS_LMLC_C                       , kStr_LMLC_C                       },   // | Macro | Number of Large Mobile Laser Cutters. |
        { kS_loaderdozer                  , kStr_loaderdozer                  },   // | Macro | Number of Loader Dozers. |
        { kS_LoaderDozer_C                , kStr_LoaderDozer_C                },   // | Macro | Number of Loader Dozers. |
        { kS_loose_rock                   , kStr_loose_rock                   },   // | Macro | Tile ID of loose rock(30). |
        { kS_lose                         , kStr_lose                         },   // | Event | Lose the map. |
        { kS_miner                        , kStr_miner                        },   // | Variable / Class | Miner object or class. |
        { kS_miners                       , kStr_miners                       },   // | Macro | Miners discovered or active. |
        { kS_mininglaser                  , kStr_mininglaser                  },   // | Macro | Number of Mining Lasers. |
        { kS_MiningLaser_C                , kStr_MiningLaser_C                },   // | Macro | Number of Mining Lasers. |
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
        { kS_OreRefinery_C                , kStr_OreRefinery_C                },   // | Macro | Number of Ore Refineries. |
        { kS_ore_seam                     , kStr_ore_seam                     },   // | Macro | Tile ID of an ore seam(46). |
        { kS_pan                          , kStr_pan                          },   // | Event | Pan camera. |
        { kS_pause                        , kStr_pause                        },   // | Event | Pauses the game. |
        { kS_place                        , kStr_place                        },   // | Event | Change tile. |
        { kS_power                        , kStr_power                        },   // | Data Field | Returns 1 if the building has power, 0 if it doesn't. |
        { kS_powered                      , kStr_powered                      },   // | Data Field | Same as power. |
        { kS_poweroff                     , kStr_poweroff                     },   // | Data Field Trigger | Trigger when power is deactivated for a building.
        { kS_poweron                      , kStr_poweron                      },   // | Data Field Trigger | Trigger when power is activated for a building.
        { kS_powerpath                    , kStr_powerpath                    },   // | Paramter | enable/disable parameter
        { kS_PowerPath_C                  , kStr_PowerPath_C                  },   // | Macro | Number of power paths |
        { kS_powerstation                 , kStr_powerstation                 },   // | Macro | Number of Power Stations. |
        { kS_PowerStation_C               , kStr_PowerStation_C               },   // | Macro | Number of Power Stations. |
        { kS_progress_path                , kStr_progress_path                },   // | Macro | Tile id of a progress path(13). |
        { kS_qmsg                         , kStr_qmsg                         },   // | Event | Display message to user. |
        { kS_random                       , kStr_random                       },   // | Macro | return random number, random(low)(high) |
        { kS_randomspawn                  , kStr_randomspawn                  },   // | Event | Spawn the given creature class in random location
        { kS_rapidrider                   , kStr_rapidrider                   },   // | Macro | Number of Rapid Riders. |
        { kS_RapidRider_C                 , kStr_RapidRider_C                 },   // | Macro | Number of Rapid Riders. |
        { kS_RechargeSeamGoal_C           , kStr_RechargeSeamGoal_C           },   // | Collection | Visible recharge seams. |
        { kS_red                          , kStr_red                          },   // | Color | Arrow colors. |
        { kS_reinforce                    , kStr_reinforce                    },   // | Trigger | Trigger when wall is reinforced. |
        { kS_reset                        , kStr_reset                        },   // | Event | Resets the player's selection |
        { kS_resetspeed                   , kStr_resetspeed                   },   // | Event | Loads the game speed from settings again. |
        { kS_resume                       , kStr_resume                       },   // | Event | Same as unpause. |
        { kS_return                       , kStr_return                       },   // | Event | return event |
        { kS_RockMonster                  , kStr_RockMonster                  },   // | Collection | Rock Monsters. |
        { kS_RockMonster_C                , kStr_RockMonster_C                },   // | Collection | Rock Monsters. |
        { kS_row                          , kStr_row                          },   // | Data Field | Object row. |
        { kS_S                            , kStr_S                            },   // | Emerge Direction | South. |
        { kS_save                         , kStr_save                         },   // | Event | Save last miner that activated a trigger into a variable. |
        { kS_savebuilding                 , kStr_savebuilding                 },   // | Event | Save last building that activated a trigger into a variable.
        { kS_savecreature                 , kStr_savecreature                 },   // | Event | Save last creature that activated a trigger into a variable.
        { kS_savevehicle                  , kStr_savevehicle                  },   // | Event | Save last vehicle that activated a triggger into a variable.
        { kS_shake                        , kStr_shake                        },   // | Event | Shake camera. |
        { kS_showarrow                    , kStr_showarrow                    },   // | Event | Show arrow object. |
        { kS_SlimySlug                    , kStr_SlimySlug                    },   // | Collection | Slimy Slugs. |
        { kS_SlimySlug_C                  , kStr_SlimySlug_C                  },   // | Collection | Slimy Slugs. |
        { kS_slug_hole                    , kStr_slug_hole                    },   // | Macro | Tile id of slimy slug hole(12). |
        { kS_slugs                        , kStr_slugs                        },   // | Macro | Number of slimy slugs. |
        { kS_smalldigger                  , kStr_smalldigger                  },   // | Macro | Number of Small Diggers. |
        { kS_SmallDigger_C                , kStr_SmallDigger_C                },   // | Macro | Number of Small Diggers. |
        { kS_SmallSpider                  , kStr_SmallSpider                  },   // | Collection | Small Spiders. |
        { kS_SmallSpider_C                , kStr_SmallSpider_C                },   // | Collection | Small Spiders. |
        { kS_smalltransporttruck          , kStr_smalltransporttruck          },   // | Macro | Number of Small Transport Trucks. |
        { kS_SmallTransportTruck_C        , kStr_SmallTransportTruck_C        },   // | Macro | Number of Small Transport Trucks. |
        { kS_SMLC                         , kStr_SMLC                         },   // | Macro | Number of Small Mobile Laser Cutters |
        { kS_SMLC_C                       , kStr_SMLC_C                       },   // | Macro | Number of Small Mobile Laser Cutters |
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
        { kS_SuperTeleport_C              , kStr_SuperTeleport_C              },   // | Macro | Number of Super Teleports. |
        { kS_supportstation               , kStr_supportstation               },   // | Macro | Number of Support Stations. |
        { kS_SupportStation_C             , kStr_SupportStation_C             },   // | Macro | Number of Support Stations. |
        { kS_teleportpad                  , kStr_teleportpad                  },   // | Macro | Numbewr of Teleport Pads. |
        { kS_TeleportPad_C                , kStr_TeleportPad_C                },   // | Macro | Numbewr of Teleport Pads. |
        { kS_tick                         , kStr_tick                         },   // | Event Chain | Called on every engine tick.Not recommended. |
        { kS_tile                         , kStr_tile                         },   // | Data Field | TileID for objectt. |
        { kS_tileid                       , kStr_tileid                       },   // | Data Field | same as tile. |
        { kS_time                         , kStr_time                         },   // | Macro / Trigger | Game time or trigger. |
        { kS_timer                        , kStr_timer                        },   // | Variable | Timer object. |
        { kS_toolstore                    , kStr_toolstore                    },   // | Macro | Returns number of toolstores. |
        { kS_Toolstore_C                  , kStr_Toolstore_C                  },   // | Macro | Returns number of toolstores. |
        { kS_true                         , kStr_true                         },   // | bool value or 1. |
        { kS_truewait                     , kStr_truewait                     },   // | Event | Suspend event chain for real user time period. |
        { kS_tunnelscout                  , kStr_tunnelscout                  },   // | Macro | Number of Tunnel Scouts. |
        { kS_TunnelScout_C                , kStr_TunnelScout_C                },   // | Macro | Number of Tunnel Scouts. |
        { kS_tunneltransport              , kStr_tunneltransport              },   // | Macro | Number of Tunnel Transports. |
        { kS_TunnelTransport_C            , kStr_TunnelTransport_C            },   // | Macro | Number of Tunnel Transports. |
        { kS_upgrade                      , kStr_upgrade                      },   // | Trigger when vehicle is upgraded. |
        { kS_upgraded                     , kStr_upgraded                     },   // | Trigger | Not working, don't use. |
        { kS_upgradestation               , kStr_upgradestation               },   // | Macro | Number of Upgrade Stations. |
        { kS_UpgradeStation_C             , kStr_UpgradeStation_C             },   // | Macro | Number of Upgrade Stations. |
        { kS_unpause                      , kStr_unpause                      },   // | Event | Resumes the game if paused. |
        { kS_variable                     , kStr_variable                     },   // | objective section reserved word |
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
        { kS_wake                         , kStr_wake                         },   // | Event | Wake monster | Monster ID |
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
        { kS_Bat_C                        , eMacroFlagReturnNumeric },   // collection, creature
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
        { kS_Canteen_C                    , eMacroFlagReturnNumeric },   // macro, int, number of Canteens.
        { kS_cargocarrier                 , eMacroFlagReturnNumeric },   // macro, int number of Cargo Carriers.
        { kS_CargoCarrier_C               , eMacroFlagReturnNumeric },   // macro, int number of Cargo Carriers.
        { kS_chromecrusher                , eMacroFlagReturnNumeric },   // macro int, number of Chrome Crushers
        { kS_ChromeCrusher_C              , eMacroFlagReturnNumeric },   // macro int, number of Chrome Crushers
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
        { kS_Docks_C                      , eMacroFlagReturnNumeric },   // Macro, Number of Docks.
        { kS_Dynamite_C                   , eMacroFlagReturnNumeric },   // Collection, All dynamite outside of toolstore.
        { kS_electricfence                , eMacroFlagReturnNumeric },   // Macro, Number of Fences.
        { kS_ElectricFence_C              , eMacroFlagReturnNumeric },   // Macro | Number of Fence objects.Not a collection. |
        { kS_EventErosion_C               , eMacroFlagReturnNumeric },   // | Collection | Active erosions. |
        { kS_EventLandslide_C             , eMacroFlagReturnNumeric },   // | Collection | Active active landslides. |
        { kS_erosionscale                 , eMacroFlagReturnNumeric },   // | Macro | Global erosion scale factor |
        { kS_false                        , eMacroFlagReturnNumeric },   // | Macro | returns false or 0 as numeric |
        { kS_geologicalcenter             , eMacroFlagReturnNumeric },   // | Macro | Number of Geological Centers. |
        { kS_GeologicalCenter_C           , eMacroFlagReturnNumeric },   // | Macro | Number of Geological Centers. |
        { kS_get                          , eMacroFlagReturnNumeric | eMacroFlag2Param },   // | Macro | Get tile ID. |
        { kS_granitegrinder               , eMacroFlagReturnNumeric },   // | Macro | Number of Granite Grinders. |
        { kS_GraniteGrinder_C             , eMacroFlagReturnNumeric },   // | Macro | Number of Granite Grinders. |
        { kS_hard_rock                    , eMacroFlagReturnNumeric },   // | Macro | Tile ID of hard rock(34). |
        { kS_hostiles                     , eMacroFlagReturnNumeric },   // | Macro | Number of hostile creatures. |
        { kS_hoverscout                   , eMacroFlagReturnNumeric },   // | Macro | Number of Hover Scouts. |
        { kS_HoverScout_C                 , eMacroFlagReturnNumeric },   // | Macro | Number of Hover Scouts. |
        { kS_IceMonster                   , eMacroFlagReturnNumeric },   // | Collection | Ice Monsters. |
        { kS_IceMonster_C                 , eMacroFlagReturnNumeric },   // | Collection | Ice Monsters. |
        { kS_lava                         , eMacroFlagReturnNumeric },   // | Macro | Tile ID of lava(6). |
        { kS_LavaMonster                  , eMacroFlagReturnNumeric },   // | Collection | Lava Monsters. |
        { kS_LavaMonster_C                , eMacroFlagReturnNumeric },   // | Collection | Lava Monsters. |
        { kS_LMLC                         , eMacroFlagReturnNumeric },   // | Macro | Number of Large Mobile Laser Cutters. |
        { kS_LMLC_C                       , eMacroFlagReturnNumeric },   // | Macro | Number of Large Mobile Laser Cutters. |
        { kS_loaderdozer                  , eMacroFlagReturnNumeric },   // | Macro | Number of Loader Dozers. |
        { kS_loose_rock                   , eMacroFlagReturnNumeric },   // | Macro | Tile ID of loose rock(30). |
        { kS_miners                       , eMacroFlagReturnNumeric },   // | Macro | Miners discovered or active. |
        { kS_mininglaser                  , eMacroFlagReturnNumeric },   // | Macro | Number of Geological Centers. |
        { kS_MiningLaser_C                , eMacroFlagReturnNumeric },   // | Macro | Number of Geological Centers. |
        { kS_monsters                     , eMacroFlagReturnNumeric },   // | Macro | Number of monsters. |
        { kS_NavModifierLava_C            , eMacroFlagReturnNumeric },   // | Collection | Amount of lava tiles. | NavModifierLava_C | Collection | Amount of lava tiles. |
        { kS_NavModifierPowerPath_C       , eMacroFlagReturnNumeric },   // | Collection | Amount of Power Path tiles, any type.NavModifierPowerPath_C | Collection | Amount of Power Path tiles, any type.Only finished patOnly finished paths. |
        { kS_NavModifierRubble_C          , eMacroFlagReturnNumeric },   // | Collection | Amount of Rubble tiles, any stage. |
        { kS_NavModifierWater_C           , eMacroFlagReturnNumeric },   // | Collection | Amount of water tiles. |
        { kS_ore                          , eMacroFlagReturnNumeric },   // | Macro | Ore count. |
        { kS_Ore_C                        , eMacroFlagReturnNumeric },   // | Collection | All ore. |
        { kS_orerefinery                  , eMacroFlagReturnNumeric },   // | Macro | Number of Ore Refineries. |
        { kS_OreRefinery_C                , eMacroFlagReturnNumeric },   // | Macro | Number of Ore Refineries. |
        { kS_ore_seam                     , eMacroFlagReturnNumeric },   // | Macro | Tile ID of an ore seam(46). |
        { kS_PowerPath_C                  , eMacroFlagReturnNumeric },   // | Macro | Number of Power Paths. |
        { kS_powerstation                 , eMacroFlagReturnNumeric },   // | Macro | Number of Power Stations. |
        { kS_PowerStation_C               , eMacroFlagReturnNumeric },   // | Macro | Number of Power Stations. |
        { kS_progress_path                , eMacroFlagReturnNumeric },   // | Macro | Tile id of a progress path(13). |
        { kS_random                       , eMacroFlagReturnNumeric | eMacroFlag2Param },   // | Macro | return random number, random(low)(high) |
        { kS_rapidrider                   , eMacroFlagReturnNumeric },   // | Macro | Number of Rapid Riders. |
        { kS_RapidRider_C                 , eMacroFlagReturnNumeric },   // | Macro | Number of Rapid Riders. |
        { kS_RechargeSeamGoal_C           , eMacroFlagReturnNumeric },   // | Collection | Visible recharge seams. |
        { kS_RockMonster                  , eMacroFlagReturnNumeric },   // | Collection | Rock Monsters. |
        { kS_RockMonster_C                , eMacroFlagReturnNumeric },   // | Collection | Rock Monsters. |
        { kS_SlimySlug                    , eMacroFlagReturnNumeric },   // | Collection | Slimy Slugs. |
        { kS_SlimySlug_C                  , eMacroFlagReturnNumeric },   // | Collection | Slimy Slugs. |
        { kS_slug_hole                    , eMacroFlagReturnNumeric },   // | Macro | Tile id of slimy slug hole(12). |
        { kS_slugs                        , eMacroFlagReturnNumeric },   // | Macro | Number of slimy slugs. |
        { kS_smalldigger                  , eMacroFlagReturnNumeric },   // | Macro | Number of Small Diggers. |
        { kS_SmallDigger_C                , eMacroFlagReturnNumeric },   // | Macro | Number of Small Diggers. |
        { kS_SmallSpider                  , eMacroFlagReturnNumeric },   // | Collection | Small Spiders. |
        { kS_SmallSpider_C                , eMacroFlagReturnNumeric },   // | Collection | Small Spiders. |
        { kS_smalltransporttruck          , eMacroFlagReturnNumeric },   // | Macro | Number of Small Transport Trucks. |
        { kS_SmallTransportTruck_C        , eMacroFlagReturnNumeric },   // | Macro | Number of Small Transport Trucks. |
        { kS_SMLC                         , eMacroFlagReturnNumeric },   // | Macro | Number of Small Mobile Laser Cutters |
        { kS_SMLC_C                       , eMacroFlagReturnNumeric },   // | Macro | Number of Small Mobile Laser Cutters |
        { kS_solid_rock                   , eMacroFlagReturnNumeric },   // | Macro | Tile ID of solid rock(38). |
        { kS_Stud_C                       , eMacroFlagReturnNumeric },   // | Collection | All building studs. |
        { kS_studs                        , eMacroFlagReturnNumeric },   // | Macro | Building Stud count. |
        { kS_superteleport                , eMacroFlagReturnNumeric },   // | Macro | Number of Super Teleports. |
        { kS_SuperTeleport_C              , eMacroFlagReturnNumeric },   // | Macro | Number of Super Teleports. |
        { kS_supportstation               , eMacroFlagReturnNumeric },   // | Macro | Number of Support Stations. |
        { kS_SupportStation_C             , eMacroFlagReturnNumeric },   // | Macro | Number of Support Stations. |
        { kS_teleportpad                  , eMacroFlagReturnNumeric },   // | Macro | Numbewr of Teleport Pads. |
        { kS_TeleportPad_C                , eMacroFlagReturnNumeric },   // | Macro | Numbewr of Teleport Pads. |
        { kS_time                         , eMacroFlagReturnNumeric },   // | Macro / Trigger | Game time or trigger. |
        { kS_toolstore                    , eMacroFlagReturnNumeric },   // | Macro | Returns number of toolstores. |
        { kS_Toolstore_C                  , eMacroFlagReturnNumeric },   // | Macro | Returns number of toolstores. |
        { kS_true                         , eMacroFlagReturnNumeric },   // | Macro | returns true or 1 as numeric |
        { kS_tunnelscout                  , eMacroFlagReturnNumeric },   // | Macro | Number of Tunnel Scouts. |
        { kS_TunnelScout_C                , eMacroFlagReturnNumeric },   // | Macro | Number of Tunnel Scouts. |
        { kS_tunneltransport              , eMacroFlagReturnNumeric },   // | Macro | Number of Tunnel Transports. |
        { kS_TunnelTransport_C            , eMacroFlagReturnNumeric },   // | Macro | Number of Tunnel Transports. |
        { kS_upgradestation               , eMacroFlagReturnNumeric },   // | Macro | Number of Upgrade Stations. |
        { kS_UpgradeStation_C             , eMacroFlagReturnNumeric },   // | Macro | Number of Upgrade Stations. |
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

    const std::unordered_set<std::string> arrowColors =
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
        return arrowColors.contains(MMUtil::toLower(color));
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
        for (auto const & it : ReservedWordsMap)
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
    // everytime called, return a unique name
    class Defines   // we support #pragma define key=value, there are also predefined ones
    {
    public:
        class DefineKeyValue
        {
        public:
            DefineKeyValue() = default;
            DefineKeyValue( const DefineKeyValue &) = default;
            DefineKeyValue(const std::string& key, const std::string& value, InputLinePtr const &iline) { setKeyValue( key, value, iline); }
            ~DefineKeyValue() = default;
            DefineKeyValue& operator=(const DefineKeyValue&) = default;

            bool operator == (const DefineKeyValue& rhs) const  // for collections
            {
                return m_keylc == rhs.m_keylc;
            }
            bool operator < (const DefineKeyValue& rhs) const   // for collections
            {
                return m_keylc < rhs.m_keylc;
            }

            void changeValue(const std::string& value)
            {   if (!m_uniquename)
                    m_value = value;
            }
            bool evaluated() const { return m_evaluated; }
            void setEvaluated() { m_evaluated = true; }

            const std::string & getKey() const   { return m_key; }
            std::string getValue()
            {
                if (m_uniquename)
                    return getUniqueName();
                return m_value;
            }
            const std::string & getkeylc() const { return m_keylc; }

            const InputLinePtr& inputLine() const { return m_iline; }   // used in case of error

        protected:
            void setKeyValue(const std::string &key, const std::string& value, InputLinePtr const &iline )
            {
                m_iline = iline;                // save input line where defined - used in case of evaluating error
                m_key = key;
                m_keylc = MMUtil::toLower(key);
                if (m_keylc == "tyabuniquename")  // value is empty, we generic value at runtime.
                {
                    m_uniquename = true;
                    m_evaluated = true;  // nothing to scan, contains nothing but what it computes unqiue everytime
                }
                else
                    m_value = value;
            }

            std::string getUniqueName()
            {
                return std::string("TyUnique") + MMUtil::to_string_hex(++m_uniqueNameVal);
            }
    
            std::string   m_key;                // user key
            std::string   m_keylc;              // key to compare against
            std::string   m_value;              // value of key
            InputLinePtr  m_iline;              // line where macro was defined
            int32_t       m_uniqueNameVal = 0; // used to generate unique names
            bool          m_evaluated = false;    // true = fully evaluated, false = need evaluation
            bool          m_uniquename = false;   // true = macro value is dynamically computed in some form (currently only used by TyUniqueName)
        };

        Defines()
        {
            m_defines.reserve(128);
        }
        ~Defines() = default;

        bool contains(const std::string & key) const
        {
            return m_defines.find(MMUtil::toLower(key)) != m_defines.cend();
        }

        std::string getValue(const std::string & key, ErrorWarning &errors)
        {
            std::unordered_set<std::string> circular;   // empty nested macro list
            DefineKeyValue *keyvalue = iEvaluate(key, circular, errors);
            return keyvalue ? keyvalue->getValue() : std::string();
        }

        void addKeyValue(const std::string & key, const std::string & value, InputLinePtr const &iline)
        {
            std::string keylc = MMUtil::toLower(key);
            auto it = m_defines.find(keylc);
            if (it != m_defines.end())      // already exists
            {
                (*it).second.changeValue(value);       // change value
            }
            else
            {
                m_defines.emplace(keylc, DefineKeyValue(key, value, iline) );
            }
        }

    protected:
        // initially all macros are not evaluated. On first reference, look at the macro value and see if there are embedded macros
        // if so, evaluate them. Once any macrro is evaluate, it is not evaluated again.
        // Circular references are detected and generate an error
        // keyval is filled in by searching for the given key.
        // return false if key not found. If false, keyval is epty.
        DefineKeyValue * iEvaluate(const std::string& key, std::unordered_set<std::string> &circular, ErrorWarning &errors) 
        {
            DefineKeyValue * keyval = igetKeyValue( key );  // get the DefineKeyValue if it exists
            if (keyval && !keyval->evaluated())
            {
                if (circular.contains(keyval->getkeylc()))
                {
                    errors.setError(keyval->inputLine(),"Macro expansion has circular reference: " + key);
                    return nullptr;
                }
                circular.insert(keyval->getkeylc());     // make sure embedded macros don't match to this name
                std::string evalValue = evaluateString(keyval->getValue(), circular, errors);
                keyval->changeValue(evalValue);          // set the new for the macro
                keyval->setEvaluated();                  // it is now evaluated and the value is fixed.
                circular.erase(keyval->getkeylc());     // finished with this macro
            }
            return keyval;
        }

        // value is the unprocessed macro value to process. Circular is set of macro names (lower case) that have been recursed into
        // evaluate the string, any embedded macros call back into this recursivly to expand them
        std::string evaluateString(std::string const &value, std::unordered_set<std::string> &circular, ErrorWarning &errors) // perform recursive evaluation
        {
            std::string retstr = value;
            if (retstr.length())
            {
                std::string::size_type startpos = 0;
                for (;startpos < retstr.length();)    // stay in loop until no more
                {
                    startpos = retstr.find("$(", startpos);
                    if (startpos == retstr.npos)
                        break;  // all done with processing.
                    // found  the leader. Look for closing )
                    std::string::size_type scanpos = startpos + 2;
                    bool bFound = false;
                    for (; scanpos < retstr.length(); scanpos++)   // find closing )
                    {
                        if (std::isalnum((unsigned char)retstr[scanpos]) || (retstr[scanpos] == '_') || std::isblank((unsigned char)retstr[scanpos]))
                            continue;
                        if (retstr[scanpos] == ')')
                        {
                            bFound = true;
                            break;
                        }
                    }
                    if (!bFound)  // never found closing, try again
                        startpos+=2;
                    else  // found closing
                    {
                        std::string submacro = retstr.substr(startpos+2,scanpos-startpos-2);
                        submacro = MMUtil::removeLeadingAndTrailingWhite(submacro);
                        if (submacro.length() && MMUtil::isAlphaNumericName(submacro) && contains(submacro))
                        {
                            DefineKeyValue *keyvalue = iEvaluate(submacro, circular, errors);    // now evaluate this macro (which checks for circular)
                            if (keyvalue)
                            {
                                std::string subvalue = keyvalue->getValue();
                                retstr.replace(startpos,scanpos-startpos+1,subvalue);
                                startpos = 0;   // restart at beginning since this macro may now complete other macros
                            }
                            else
                            {
                                startpos += 2; // don't subsitute macro, most likely a circular reference and we have logged an error
                            }
                        }
                        else // invalid or unknown macro name, possibly the macro name is built by other macros, keep scanning
                        {
                            startpos += 2;
                        }
                    }
                }
            }
            return retstr;
        }

        // return true if found, and DefineKeyValue is filled in.
        DefineKeyValue * igetKeyValue(const std::string & key )
        {
            auto it = m_defines.find(MMUtil::toLower(key));
            return (it != m_defines.cend()) ? &(it->second) : nullptr;
        }

        // return true if found, and DefineKeyValue is filled in.
        const DefineKeyValue * igetKeyValue(const std::string & key ) const
        {
            auto it = m_defines.find(MMUtil::toLower(key));
            return (it != m_defines.cend()) ? &(it->second) : nullptr;
        }


        std::unordered_map<std::string, DefineKeyValue> m_defines;
    };

    // when we tokenize a line, each token will be one of these.
    inline static constexpr uint64_t eUnknown             = 0x0000000000000000ull;  // unknown
    inline static constexpr uint64_t eTokenFloat          = 0x0000000000000001ull;  // some sort of floating point number
    inline static constexpr uint64_t eTokenInt            = 0x0000000000000002ull;  // number 
    inline static constexpr uint64_t eTokenName           = 0x0000000000000004ull;  // starts with alpha, has nothing but more alpha or digits. If it starts with a numeric, it must have an alpha to identify it as a variable
    inline static constexpr uint64_t eTokenSpace          = 0x0000000000000008ull;  // single space
    inline static constexpr uint64_t eTokenSpaces         = 0x0000000000000010ull;  // more than 1 space
    inline static constexpr uint64_t eTokenDot            = 0x0000000000000020ull;  // .
    inline static constexpr uint64_t eTokenSemi           = 0x0000000000000040ull;  // ;
    inline static constexpr uint64_t eTokenColon          = 0x0000000000000080ull;  // :
    inline static constexpr uint64_t eTokenDColon         = 0x0000000000000100ull;  // ::
    inline static constexpr uint64_t eTokenOBrace         = 0x0000000000000200ull;  // (
    inline static constexpr uint64_t eTokenDOBrace        = 0x0000000000000400ull;  // ((
    inline static constexpr uint64_t eTokenCBrace         = 0x0000000000000800ull;  // )
    inline static constexpr uint64_t eTokenDCBrace        = 0x0000000000001000ull;  // ))
    inline static constexpr uint64_t eTokenOBracket       = 0x0000000000002000ull;  // [
    inline static constexpr uint64_t eTokenCBracket       = 0x0000000000004000ull;  // ]
    inline static constexpr uint64_t eTokenPlus           = 0x0000000000008000ull;  // +
    inline static constexpr uint64_t eTokenPlusAssign     = 0x0000000000010000ull;  // +=
    inline static constexpr uint64_t eTokenMinus          = 0x0000000000020000ull;  // -
    inline static constexpr uint64_t eTokenMinusAssign    = 0x0000000000040000ull;  // -=
    inline static constexpr uint64_t eTokenMultiply       = 0x0000000000080000ull;  // *
    inline static constexpr uint64_t eTokenMultiplyAssign = 0x0000000000100000ull;  // *=
    inline static constexpr uint64_t eTokenFslash         = 0x0000000000200000ull;  //  /  special case, follows is an objective treat as a string
    inline static constexpr uint64_t eTokenDivide         = 0x0000000000400000ull;  // //  divide   not supported on older versions of the engine
    inline static constexpr uint64_t eTokenDivideAssign   = 0x0000000000800000ull;  // /=
    inline static constexpr uint64_t eTokenAssignment     = 0x0000000001000000ull;  // =
    inline static constexpr uint64_t eTokenEqual          = 0x0000000002000000ull;  // ==
    inline static constexpr uint64_t eTokenObjStr         = 0x0000000004000000ull;  //  string after / is objective, until end of line
    inline static constexpr uint64_t eTokenLess           = 0x0000000008000000ull;  // <
    inline static constexpr uint64_t eTokenLessEqual      = 0x0000000010000000ull;  // <=
    inline static constexpr uint64_t eTokenGreater        = 0x0000000020000000ull;  // >
    inline static constexpr uint64_t eTokenGreaterEqual   = 0x0000000040000000ull;  // >=
    inline static constexpr uint64_t eTokenNot            = 0x0000000080000000ull;  // !  mm engine does not support this currently, we will generate an error
    inline static constexpr uint64_t eTokenNotEqual       = 0x0000000100000000ull;  // !=
    inline static constexpr uint64_t eTokenComma          = 0x0000000200000000ull;  // ,
    inline static constexpr uint64_t eTokenString         = 0x0000000400000000ull;  // string literal defined with double quotes, quotes part of string
    inline static constexpr uint64_t eTokenComment        = 0x0000000800000000ull;  // contains comment and optional leading space prior to comment
    inline static constexpr uint64_t eTokenCommentLine    = 0x0000001000000000ull;  // entire line is comment, may have space in front
    inline static constexpr uint64_t eTokenBlankLine      = 0x0000002000000000ull;  // entire line is empty and not a comment. Will end event chain if within one
    inline static constexpr uint64_t eTokenVariable       = 0x0000004000000000ull;  // also has eTokenName set, this is a user variable
    inline static constexpr uint64_t eTokenEventChain     = 0x0000008000000000ull;  // also has eTokenName set, this is a user event chain name
    inline static constexpr uint64_t eTokenBoolFalse      = 0x0000010000000000ull;  // also has eTokenInt set
    inline static constexpr uint64_t eTokenBoolTrue       = 0x0000020000000000ull;  // also has eTokenInt set
    inline static constexpr uint64_t eTokenArrowColor     = 0x0000040000000000ull;  // one of the arrow colors
    inline static constexpr uint64_t eTokenSoundPath      = 0x0000080000000000ull;  // token is a path
    inline static constexpr uint64_t eTokenNoQuoteString  = 0x0000100000000000ull;  // string token, no double quotes surrounding it
    inline static constexpr uint64_t eTokenEventList      = 0x0000200000000000ull;  // ? preceding event for random selected event
    inline static constexpr uint64_t eTokenFailedEmerge   = 0x0000400000000000ull;  // ~ preceding event for failed emerge event
    inline static constexpr uint64_t eTokenCLSpace        = 0x0000800000000000ull;  // entire line is comment, it did have space in front eTokenCommentLine is also set
    inline static constexpr uint64_t eTokenChainNoOptimize= 0x2000000000000000ull;  // set if token is chainname and it should not be optimized
    inline static constexpr uint64_t eTokenOptional       = 0x4000000000000000ull;  // set if token is optional
    inline static constexpr uint64_t eTokenIgnore         = 0x8000000000000000ull;  // if set ignore the token completely. Set when ignorning invalid spaces

    // multiple owners so all access will be via a shared_ptr.
    class variableType
    {
    public:

        enum varType : int {
            eVarTypeBool = 0,   // variable holds bool
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
        variableType(const std::string& name, varType type) : m_name(name), m_namelc(MMUtil::toLower(name)), m_type(type), m_count(1) {}
        ~variableType() = default;

        void setLine( const InputLinePtr &iline ) { m_line = iline; }
        const InputLinePtr &getLine() const { return m_line; }

        bool hasData() const { return m_hasValue; }
        void setValueString( const std::string &val ) { m_string = val; }
        void setValueTimer(float delay, float min, float max, const std::string& event)
        {
            m_timerdelay = delay;
            m_timermin = min;
            m_timermax = max;
            m_timerevent = event;
        }
        void setValueBool( bool val ) { m_bool = val; m_hasValue = true; }
        void setValueInt( int val ) { m_int = val; m_hasValue = true; }
        void setValueFloat( float val ) { m_float = val; m_hasValue = true; }
        void setValueID( int id ) { m_id = id; m_hasValue = true; }
        void setValueBuilding( int row, int col ) { m_row = row; m_col = col; m_hasValue = true; }

        std::string const& getTimerEventName() const
        {
            assert(m_type == varType::eVarTypeTimer);
            return m_timerevent;
        }

        int getType() const { return (int)m_type; }
        const std::string & getName() const { return m_name; }
        const std::string & getNamelc() const { return m_namelc; }
        void setOptName( std::string &optname ) { m_optname = optname; }

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

        // return ref to either original name or optimized name if it has one
        const std::string & getOptName() const { return m_optname; }

        void incCount() { m_count++; }
        int getCount() const { return m_count; }


    protected:
        InputLinePtr  m_line;       // line that defines variable. Used to generate warnings if never used
        std::string   m_namelc;     // lower case name
        std::string   m_name;       // original name may be mixed case
        std::string   m_optname;    // optimized name, all instances of this variable will be written with this name
        int           m_count = 0;  // number of references. Used for generation of new names
        varType       m_type = eVarTypeBool;  // defines type of variable

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
        bool        m_hasValue = false; // set to true if any of the above have data
    };

    typedef std::shared_ptr<variableType> variableTypeSP;   // all access is via the shared_ptr
    

    std::unordered_map<std::string,variableType::varType> m_varTypeMap = 
    {
        { kS_bool,     variableType::eVarTypeBool },
        { kS_int,      variableType::eVarTypeInt },
        { kS_float,    variableType::eVarTypeFloat },
        { kS_intarray, variableType::eVarTypeIntArray },
        { kS_string,   variableType::eVarTypeString },
        { kS_miner,    variableType::eVarTypeMiner },
        { kS_building, variableType::eVarTypeBuilding },
        { kS_vehicle,  variableType::eVarTypeVehicle },
        { kS_creature, variableType::eVarTypeCreature },
        { kS_arrow,    variableType::eVarTypeArrow },
        { kS_timer,    variableType::eVarTypeTimer }
    };


    // holds all user variables
    // the class variableType is the information for a single user variable. The collections all store
    // a shared_ptr to it, so all the collections can work on the same object and we don't worry about ownership.
    class allUserVariables
    {
    public:

        // In addition to the entire collection, we duplicate into variable type collections.
        // This allows faster checks to identify duplicate variables to same object (which is an error)
        class userVariables
        {
        public:
            userVariables()
            {
                for (auto & it : m_variables)
                {
                    it.reserve(256);
                }
            }
            ~userVariables() = default;

            void add(const std::string& key, variableTypeSP& p)
            {
                assert( key.empty() == false );
                m_variables[p->getType()].emplace( key, p );
            }

            bool contains(const std::string& key, variableType::varType type)
            {
                return m_variables[type].contains(MMUtil::toLower(key));
            }

            variableTypeSP find(const std::string& key, variableType::varType type)
            {
                auto it = m_variables[type].find(MMUtil::toLower(key));
                return it->second;
            }

            const variableTypeSP find(const std::string& key, variableType::varType type) const
            {
                auto it = m_variables[type].find(MMUtil::toLower(key));
                return it->second;
            }

            // compare this variable to the passed in one, and return true if the values are the same.
            // can return true only for miner, creature, vehicle, building variables
            bool isValueDuplicated(const variableTypeSP &var) const
            {
                for (auto const & it : m_variables[var->getType()])
                {
                    if (it.second->isDuplicateValue(*var))
                        return true;
                }
                return false;
            }

            std::unordered_map<std::string, variableTypeSP> const& getTimerVars() const  // used to update ref counts for event chain names
            {
                return m_variables[variableType::eVarTypeTimer];
            }
        protected:

            std::unordered_map<std::string, variableTypeSP> m_variables[variableType::eVarTypeLAST+1];  
        };


        allUserVariables() { m_allvariables.reserve(1024); }
        ~allUserVariables() = default;

        void add(variableTypeSP& variable)
        {
            const std::string & key = variable->getNamelc();
            if (!contains(key)) // have to check since it could already be a reported error
            {
                m_allvariables.emplace(key,variable);
                m_typevariables.add(key, variable);
            }
        }

        bool contains(const std::string& key) const
        {
            return m_allvariables.contains( MMUtil::toLower(key) );
        }

        void incCount(const std::string& name)
        {
            auto it =  m_allvariables.find( MMUtil::toLower(name) );
            assert(it !=  m_allvariables.end());
            if (it !=  m_allvariables.end())
            {
                (*it).second->incCount();
            }
        }

        variableTypeSP find(const std::string& key)
        {
            auto it = m_allvariables.find( MMUtil::toLower(key));
            return it->second;
        }

        const variableTypeSP find(const std::string& key) const
        {
            auto it = m_allvariables.find( MMUtil::toLower(key));
            if (it == m_allvariables.end())
                return nullptr;
            return it->second;
        }

        std::string getOptName(const std::string& key) const
        {
            auto it = m_allvariables.find( MMUtil::toLower(key));
            if (it != m_allvariables.end())
                return it->second->getOptName();
            return std::string();
        }

        // used to determine for miner, vehicle, building, creature if the value is already assigned.
        // return true if it already exists. false if not duplicated
        bool isValueDuplicated(const variableTypeSP & variable) const
        {
            return m_typevariables.isValueDuplicated( variable );

        }

        size_t size() const { return m_allvariables.size(); }


        std::unordered_map<std::string, variableTypeSP> const & allVariables() const { return m_allvariables; }
        std::unordered_map<std::string, variableTypeSP> const & getTimerVars() const { return m_typevariables.getTimerVars(); }

    protected:
        std::unordered_map<std::string, variableTypeSP> m_allvariables;
        userVariables                                   m_typevariables;
    };


    // multiple owners during name optimization so all access is via shared_ptr
    // similar to variableType, but used to hold event chain names
    class eventChainName
    {
    public:
        eventChainName() = default;
        eventChainName(const std::string& name) : m_name(name), m_namelc(MMUtil::toLower(name)), m_count(1) {}
        ~eventChainName() = default;

        void setLine( const InputLinePtr &iline ) { m_line = iline; }
        const InputLinePtr &getLine() const { return m_line; }

        void incCount() { m_count++; }
        int  getCount() const { return m_count; }
        void setOptName( const std::string &optname ) { m_optname = optname; }

        const std::string& getOptName() const { return m_optname; }
        const std::string& getName()    const { return m_name; }
        const std::string& getNamelc()  const { return m_namelc; }

    protected:
        InputLinePtr m_line;      // generate warnings if not referenced
        std::string  m_name;      // original name
        std::string  m_namelc;    // lower case version of name
        std::string  m_optname;   // optimized name
        int          m_count = 0; // number of times name is used
    };

    typedef std::shared_ptr<eventChainName> eventChainNameSP;


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
        void add(const std::string& name, InputLinePtr const &iline )
        {
            std::string namelc = MMUtil::toLower(name);
            assert(m_eventchainnames.contains(namelc) == false);

            eventChainNameSP pName = std::make_shared<eventChainName>(name);
            pName->setLine( iline );
            m_eventchainnames.emplace(namelc,pName);
        }

        void incCount(const std::string& name)
        {
            auto it = m_eventchainnames.find( MMUtil::toLower(name) );
            assert(it != m_eventchainnames.end());
            if (it != m_eventchainnames.end())
            {
                (*it).second->incCount();
            }
        }

        void setOptName(const std::string& name, const std::string& optName)
        {
            auto it = m_eventchainnames.find( MMUtil::toLower(name));
            if (it != m_eventchainnames.end())
            {
                (*it).second->setOptName( optName );
            }
        }

        std::string getOptName(const std::string& name) const
        {
            std::string namelc = MMUtil::toLower(name);
            auto it = m_eventchainnames.find( MMUtil::toLower(name));
            if (it != m_eventchainnames.end())
            {
                return (*it).second->getOptName();
            }
            return std::string();
        }

        eventChainNameSP find(std::string str) const
        {
            auto it = m_eventchainnames.find(MMUtil::toLower(str));
            return it->second;
        }

        std::unordered_map<std::string, eventChainNameSP> const &allChainNames() const { return m_eventchainnames; }

        size_t size() const { return m_eventchainnames.size(); }


    protected:
        std::unordered_map<std::string, eventChainNameSP> m_eventchainnames;
    };

    class ScriptToken   
    {
    public:
        ScriptToken() = default;
        ~ScriptToken() = default;

        ScriptToken(const std::string& str, uint64_t id) : m_token(str), m_tokenlc(MMUtil::toLower(m_token)), m_id(id) {}
        ScriptToken(const std::string& str, const std::string& strlc, uint64_t id) : m_token(str), m_tokenlc(strlc), m_id(id) {}
        ScriptToken(std::string_view str, uint64_t id)   : m_token(str), m_tokenlc(MMUtil::toLower(m_token)), m_id(id) {}

        void setToken( const std::string &str) { m_token = str; m_tokenlc = MMUtil::toLower(str); }     // used to convert multiple spaces to single space

        bool operator == (const ScriptToken &rhs ) { return m_tokenlc == rhs.m_tokenlc; }   // for collections
        bool operator <  (const ScriptToken &rhs ) { return m_tokenlc < rhs.m_tokenlc; }    // for collections

        const std::string & getToken() const   { return m_token; }
        const std::string & getTokenlc() const { return m_tokenlc; }
        uint64_t getID() const { return m_id; }
        void orID( uint64_t value ) { m_id |= value; }      // used to refine the token type during processing.
        void removeID( uint64_t value ) { m_id &= ~value; } // mainly used to remove name for sound paths
        void setID( uint64_t value ) { m_id = value; }      // used to set the sound path start

    protected:
        std::string m_token;
        std::string m_tokenlc;  // converted to lower case
        uint64_t    m_id;       // identifies type of token
    };

    // holds a line and all tokens for that line
    // caller takes care of pragma processing
    class ScriptLine
    {
    public:
        ScriptLine() = default;
        ScriptLine(const InputLinePtr & line) : m_line(line) {}
        ~ScriptLine() = default;

        void setTokens(std::deque<ScriptToken>&& tokens)    // move
        {
            m_tokens = tokens;
        }
        void setTokens(std::deque<ScriptToken> const & tokens) // copy
        {
            m_tokens = tokens;
        }

        void setCommentLineSpace() { m_bCommentLineSpace = true; }
        bool getCommentLineSpace() const { return m_bCommentLineSpace; }
        void setCommentLineNoSpace() { m_bCommentLineNoSpace = true; }
        bool getCommentLineNoSpace() const { return m_bCommentLineNoSpace; }

        InputLinePtr const & getLine() const { return m_line; }
        InputLinePtr       & getLine()       { return m_line; }

        // build line from the parsed tokens
        // set bSkip to true to ignore this line, caller will ignore. Caller will do other checks for lines with eTokenCommentLine and eTokenCLSpace
        std::string serialize_out( const ScriptEngine & se, bool bNoComments, bool bOptimizeNames, bool &bSkip ) const
        {
            bSkip = false;
            std::string str;
            std::size_t len = m_line->getLine().length();
            str.reserve( len ? len : 256 );  // serialize out never grows the output so this is ok.
            for (auto const & it : m_tokens)
            {
                if (it.getID() & eTokenIgnore)  // if token is flag for ignore
                    continue;                   // don't add it to output. Used to remove invalid space
                if (it.getID() & eTokenVariable) // token is some sort of user variable, it may be renamed
                {
                    const allUserVariables & var = se.getUserVariables();
                    std::string optname = var.getOptName(it.getToken());
                    if (!optname.empty() && bOptimizeNames)
                        str += optname;       // replace with generated optimized name
                    else
                        str += it.getToken();          // keep case used in this instance
                }
                else if (it.getID() & eTokenEventChain) // token is an event chain name
                {
                    const allEventChainNames & var = se.getEventChainNames();
                    std::string optname = var.getOptName( it.getToken() );
                    if (!(it.getID() & eTokenChainNoOptimize) && !optname.empty() && bOptimizeNames)
                        str += optname;
                    else
                        str += it.getToken();          // keep case used in this instance
                }
                // check if removing non-essential comments
                else if ((it.getID() & eTokenComment) && bNoComments && isIgnorableComment(it.getTokenlc()))
                    continue;
                else if ((it.getID() & eTokenCommentLine) && bNoComments && isIgnorableComment(it.getTokenlc()))
                {
                    bSkip = true;   // full line comment skip
                    break;          // all done nothing else on this line
                }
                else
                    str += it.getToken();
            }
            if (!str.empty() && bSkip)  // should never happen
                bSkip = false;          // return what it has
            return str;
        }

        // script language does not allow any leading spaces on a line that is not a comment line.
        void processInitialSpace(bool bFixSpace, ErrorWarning errors)
        {
            if (m_bProcessed)
                return;
            if (m_tokens.empty())   // should not happen, we always have some token for a line even if the line is blank.
                return;
            ScriptToken &it = m_tokens[0];     // first token
            if (it.getID() & (eTokenSpace | eTokenSpaces))  // first token is a space or spaces
            {
                it.orID( eTokenIgnore );    // ignore leading space (allows more errors to be detected)
                if (!bFixSpace)
                    errors.setError(m_line,"Leading space not allowed. Use -sfixspace to correct");
            }
        }

        // script language does not allow any trailing spaces on a line that is not a comment line.
        void processLastSpace(bool bFixSpace, ErrorWarning errors)
        {
            if (m_bProcessed)
                return;
            if (m_tokens.empty())   // should not happen, we always have some token for a line even if the line is blank.
                return;
            ScriptToken & it = m_tokens.back();      // last token
            if (it.getID() & (eTokenSpace | eTokenSpaces))  // first token is a space or spaces
            {
                it.orID( eTokenIgnore );    // ignore trailing space (allows more errors to be detected)
                if (!bFixSpace)
                    errors.setError(m_line,"trailing space not allowed. Use -sfixspace to correct");
            }
        }

        // looking for if/when
        void processTrigger(ScriptEngine& se, [[maybe_unused]] bool bFixSpace, [[maybe_unused]] ErrorWarning& m_errors)  // currently this is just to set the line type as a trigger
        {
            if (m_bProcessed)    // line has been processed, don't process again.
                return;
            if (m_tokens.empty())  // should never happen
                return;
            if (m_bVariableDecl)   // ignore variables declerations
                return;
            if (m_bEventChain)     // ignore event chain definations
                return;

            std::size_t index = 0;
            if (!se.getNextToken(m_tokens,(size_t)((intmax_t)(-1)), index))    // start at beginning - find first non-ignored token
                return;               // no more tokens, not a variable line

            ScriptToken &it = m_tokens[index];  // get first usable token
            if ((index <= 1) && (it.getID() & eTokenName))  // have token, see if it is if or when
            {
                if ((it.getTokenlc() == se.kS_if) || (it.getTokenlc() == se.kS_when))
                    m_bTrigger = true;
            }
        }

        // if the line last token (ignoring comments) is a simi then consider it an event.
        // This is not full event detection, it does not detect events that have no semi (possible last event in a chain).
        // The purpose of this is for blank line optimization. If enabled we can optimize out blank lines ending chains
        // by not outputing the semi
        void processEventDetection(ScriptEngine& se, [[maybe_unused]] bool bFixSpace, [[maybe_unused]] ErrorWarning errors)
        {
            if (m_bProcessed)
                return;
            if (m_bTrigger || m_bVariableDecl)  // skip triggers, variable declerations
                return;
            if (m_tokens.empty())   // should not happen but just in case
                return;
            if (m_tokens[0].getID() & (eTokenBlankLine || eTokenCommentLine)) // skip blank and comment lines
                return;
            // the only lines left are event chain declerations or event lines
            // we want to see what is the last token that is not a comment or ignore. If a semi then this is an event line inside of event chain. If anything else, it is an event ending the chain
            size_t index = m_tokens.size();
            index-=1;   // index of last item
            uint64_t id = m_tokens[index].getID();

            if (id & se.eTokenComment)  // token comment
            {
                if (index < 1)  // should never happen, that would be a commentline. Just in case
                    return;
                index--;
                id = m_tokens[index].getID(); // get id on prior comment
            }
            if (id & se.eTokenIgnore)  // trailing spaces they have already been tagged as ignore
            {
                if (index < 1)
                    return;
                index--;
                id = m_tokens[index].getID();
            }
            m_bEvent = true;     // ends in semi or not, it is part of event chain.
            if (!(id & se.eTokenSemi))  // if no semi, then this also ends the event chain.
                m_bEventChainEnd = true; // this also ends the event chain

        }

        bool isEmpty() const
        {
            if (m_tokens.empty())
                return true;
            if (m_tokens[0].getID() & eTokenBlankLine)
                return true;
            return false;
        }

        // this is full line comment, it has leading spaces. Convert to a comment with no leading spaces
        // it was in the middle of event chain and we want to fix it and not end the chain.
        void convert2CommentLineNoSpace()
        {
            m_bCommentLineNoSpace = true;
            m_bCommentLineSpace = false;
            m_bEventChainEnd = false;  // should already be false, but just in case
            m_bProcessed = true;   // no need to process this line again
            std::string token = m_tokens[0].getToken();
            token = MMUtil::removeLeadingWhite(token);  // get token with leading spaces, remove them.
            m_tokens.clear();
            m_tokens.emplace_back(token, eTokenCommentLine );
        }

        void setEventChainEnd() { m_bEventChainEnd = true; }

        // process the tokens, see if this is a variable declaration. if so return true, if not return false
        // errors can be filled in if invalid, caller checks for any errors added.
        // spaces can be removed if bFixSpace
        // vars is updated as complete declarations are detected.
        void processVariableDecleration(ScriptEngine& se, allUserVariables& vars, bool bFixSpace, ErrorWarning& errors)
        {
            if (m_bProcessed)    // line has been processed, don't process again.
                return;
            if (m_tokens.empty())  // should never happen
                return;

            std::size_t index = 0;
            if (!se.getNextToken(m_tokens,(size_t)((intmax_t)(-1)), index))    // start at beginning - find first non-ignored token
                return;               // no more tokens, not a variable line

            ScriptToken &it = m_tokens[index];  // get first usable token
            if ((index <= 1) && (it.getID() & se.eTokenName))  // have token, see if it is one of the variable types (bool, int, float, string, intarrray, miner, building, vehicle, creature, arrow, timer)
            {
                assert(it.getTokenlc().empty() == false);
                assert(it.getToken().empty() == false);
                auto mapit = se.m_varTypeMap.find(it.getTokenlc());
                if (mapit != se.m_varTypeMap.cend())        // this is one of our variable types
                {
                    processVariableDecl(mapit->second, index, se, vars, bFixSpace, errors);
                    m_bProcessed = true;     // no need to process this line again.
                    m_bVariableDecl = true;  // this is a variable declaration
                }
            }
        }

        // process the tokens, see if this is an event chain
        // errors can be filled in if invalid, caller checks for any errors added.
        // spaces can be removed if bFixSpace
        void processEventChainName(ScriptEngine& se, allEventChainNames& eventchainnames, bool bFixSpace, ErrorWarning& errors)
        {
            if (m_bProcessed)    // line has been processed, don't process again.
                return;
            if (m_tokens.empty())  // should never happen
                return;

            std::size_t index = 0;
            if (!se.getNextToken(m_tokens,(size_t)((intmax_t)(-1)), index))    // start at beginning - find first non-ignored token
                return;               // no more tokens, not a variable line

            ScriptToken &it = m_tokens[index];  // get first usable token
            if ((index <= 1) && (it.getID() & se.eTokenName))  // have token, it is a name type
            {
                assert(it.getTokenlc().empty() == false);
                assert(it.getToken().empty() == false);

                // an event chain name is a unique name followed by :: See if the next non-space token is ::
                size_t nameindex = index;  // save index of possible event chain name

                if (!se.getNextTokenProcessSpaces(m_tokens,index, index, bFixSpace, 0, m_line, errors))    // move to next token
                    return;                         // no more tokens, not an event chain name

                // see if token is ::
                if (!(m_tokens[index].getID() & se.eTokenDColon))
                    return;                         // not event chain name

                // found :: Set flag so later processing will skip everything prior to the ::
                m_bEventChain = true;    // found :: treat as event chain name def even if error

                if (se.isReservedEvent(m_tokens[nameindex].getToken()))   // cannot be a reserved word
                {
                    errors.setWarning(m_line,"Event Chain name is reserved word");
                    return;
                }

                if (eventchainnames.contains(m_tokens[nameindex].getToken())) // check for duplicate
                {
                    errors.setWarning(m_line,"Event Chain already defined");
                    return;
                }
                eventchainnames.add(m_tokens[nameindex].getToken(), m_line );
                // set token type as event chain name  (allows name optimization)
                m_tokens[nameindex].orID(eTokenEventChain);

                if (m_tokens[nameindex].getTokenlc() == se.kS_init) // init is not to be optimized
                    m_tokens[nameindex].orID(eTokenChainNoOptimize);
                else if (m_tokens[nameindex].getTokenlc() == se.kS_tick) // tick is not to be optimized
                    m_tokens[nameindex].orID(eTokenChainNoOptimize);

                // we want to know if this line ends is a semi so we know the event chain may continue. If it ends in a space, it ends the chain.




            }
        }

        // this will look for sound: event. If found the stuff after sound is combined into a single soundpath object.
        // end of token are ; and ] or the end comment
        void processSoundPath(ScriptEngine& se, bool bFixSpace, ErrorWarning& errors)
        {
            if (m_bProcessed)    // line has been processed, don't process again.
                return;
            if (m_tokens.empty())  // should never happen
                return;

            size_t index = (size_t)(-1LL);
            for (; se.getNextToken(m_tokens, index, index);)    // start at beginning - find first non-ignored token
            {
                ScriptToken & it = m_tokens[index];
                if (!(it.getID() & eTokenIgnore) && (it.getID() & eTokenName))  // have a name
                {
                    if (it.getTokenlc() == se.kS_sound) // sound token
                        break;
                }
            }
            if (index < m_tokens.size())  // sound was found
            {
                if ((se.getNextTokenProcessSpaces(m_tokens, index, index, bFixSpace, eTokenSpace | eTokenSpaces | eTokenOptional, m_line, errors)))
                {
                    if (m_tokens[index].getID() & eTokenColon)  // have the :
                    {
                        if ((se.getNextTokenProcessSpaces(m_tokens, index, index, bFixSpace, eTokenSpace | eTokenSpaces | eTokenOptional, m_line, errors)))
                        {
                            // I want to find all of the tokens for the sound path
                            size_t soundIndexStart = index;
                            size_t soundIndexEnd = index;
                            for (; se.getNextToken(m_tokens, index, index);)
                            {
                                // all tokens are part of the path until ; ] comment
                                if (m_tokens[index].getID() & (eTokenCBracket | eTokenSemi | eTokenComment))
                                    break;
                            }
                            soundIndexEnd = index;  // this token is not part of the path

                            if (soundIndexStart < soundIndexEnd)
                            {
                                // first one gets everything
                                m_tokens[soundIndexStart].setID(eTokenSoundPath);
                                for (index = soundIndexStart+1; index < soundIndexEnd; index++)
                                {
                                    m_tokens[index].orID(eTokenIgnore);  // ignore them
                                    m_tokens[soundIndexStart].setToken(m_tokens[soundIndexStart].getToken()+m_tokens[index].getToken());
                                    m_tokens[index].setToken(std::string());
                                }
                            }
                        }
                    }
                }
            }
        }


        void identifyTokens(ScriptEngine& se, bool bFixSpace, ErrorWarning& errors)
        {
            if (m_bProcessed)    // line has been processed, don't process again.
                return;
            if (m_tokens.empty())  // should never happen
                return;

            auto const & vars = se.getUserVariables();
            auto const & chains = se.getEventChainNames();

            std::size_t index = 0;
            if (!se.getNextToken(m_tokens,(size_t)((intmax_t)(-1)), index))    // start at beginning - find first non-ignored token
                return;

            if (m_bEventChain)  // this line has been identified as one that contains an event chain name, it has the :: token.
            {
                assert(m_tokens[index].getID() & eTokenEventChain);
                if (!se.getNextToken(m_tokens,index, index))    // move to next token
                    return;                         // no more tokens, not an event chain name
                assert(m_tokens[index].getID() & eTokenDColon);   // we have already done space fixing, so this must be the double colon

                // there must be one more token after the double colon,
                if (!se.getNextTokenProcessSpaces(m_tokens,index, index,bFixSpace,0,m_line,errors))    // move to next token
                {
                    errors.setError(m_line, "Missing semi-colon after EventChain name double colon.");
                    return;
                }
                // index is now at the next token. It cannot be a comment (missing semi-colon)
                if (m_tokens[index].getID() & eTokenComment)
                {
                    errors.setError(m_line, "Missing semi-colon after EventChain name double colon.");
                    return;
                }
                if (m_tokens[index].getID() & eTokenSemi)  // event chain has no event on the decleration line, nothing but comments are allowed next.
                {
                    if (!se.getNextToken(m_tokens, index, index))  // end of line and no comment
                    {
                        m_bProcessed = true;
                    }
                    else if (m_tokens[index].getID() && eTokenComment)
                    {
                        m_bProcessed = true;
                    }
                    else
                    {
                        errors.setError(m_line, "Nothing allowed after semi-colon except a comment");
                    }
                    return;
                }
                // index is now at the first token after ;
            }
            // index is at the first non-space token. Parse the rest of the line.
            // look for eTokenName and see if it is a reserved word. If so bring some of those bits in.
            // If not, then see if it matches either the eventchain names or variable names and set those bits if so.
            for (;;)  // process remaining tokens in line
            {
                if ((m_tokens[index].getID() & eTokenName) && !(m_tokens[index].getID() & (eTokenVariable | eTokenEventChain)))  // not yet assigned unknown tokens
                {
                    // see if token is reserved word, if so use those bits
                    std::string const &str = m_tokens[index].getToken();
                    if (se.isReservedEvent(str))  // excluded init and tick
                    {
                        // TODO need to figure out how to do generic event syntax checking...
                    }
                    else if (vars.contains(str))
                    {
                        m_tokens[index].orID(eTokenVariable);   // token is a user defined variable
                        auto it = vars.find(str);
                        it->incCount();
                    }
                    else if (chains.contains(str))
                    {
                        m_tokens[index].orID(eTokenEventChain);   // token is an event chain name
                        auto it = chains.find(str);
                        it->incCount();
                    }
                    else // must be undefined var or chain
                    {
                        errors.setWarning(m_line,std::string("Unknown name: ") + str + " - undeclared variable or event chain");
                    }
                }
                if (!se.getNextTokenProcessSpaces(m_tokens, index,index, bFixSpace, 0, m_line, errors))         // move to next token
                    break;                              // no tokens left, done
            } // for
        }

    protected:
        bool isIgnorableComment(const std::string& str) const
        {
            bool bCanBeIgnored = false;
            std::string_view strview = MMUtil::removeLeadingWhite(str);
            if (strview.find('#') == 0) // some sort of comment (should always be true since token type caller check for was a comment type)
            {
                if( (strview.find("#.") != 0) && (strview.find("##pragma") != 0) )
                    bCanBeIgnored = true;
            }
            return bCanBeIgnored;
        }


        // index is the token for type. format is space name optional =value(s) depending on type of variable.
        void processVariableDecl(variableType::varType type, std::size_t index, ScriptEngine& se, allUserVariables& vars, bool bFixSpace, ErrorWarning& errors)
        {
            m_bProcessed = true;     // no matter what, line has now been processed
            if (!se.getNextTokenProcessSpaces(m_tokens,index,index, bFixSpace, eTokenSpace | eTokenSpaces, m_line, errors ))
            {
                errors.setError(m_line, std::string("missing variable name"));
                return;
            }

            ScriptToken & nameToken = m_tokens[index];
            if (!(nameToken.getID() & eTokenName))
            {
                errors.setError(m_line, std::string("invalid variable name: ") + nameToken.getToken());
                return;
            }

            if (se.isReservedVar(nameToken.getTokenlc()))
            {
                errors.setWarning(m_line, std::string("variable name is reserved keyword: ") + nameToken.getTokenlc());
                return;
            }

            if (vars.contains(nameToken.getTokenlc()))
            {
                errors.setWarning(m_line,std::string("Duplicate variable name: ") + nameToken.getToken());
                return;
            }

            if (se.getEventChainNames().contains(nameToken.getTokenlc()))
            {
                errors.setWarning(m_line,std::string("Variable name duplicates EventChain name: ")+ nameToken.getTokenlc());
                return;
            }

            // variable name is valid, and not already in use. start building the variable data
            // variable data is owned by itself, all access is via shared_ptr so ownership is never an issue and multiple collections may have it
            variableTypeSP vt(std::make_shared<variableType>(nameToken.getToken(), type)); // everyone only has a shared_ptr to it
            vt->setLine(m_line);            // save line defining
            nameToken.orID(se.eTokenVariable);     // name is a variable name, allows optimization

            se.getNextTokenProcessSpaces(m_tokens,index,index, bFixSpace,eTokenSpace | eTokenSpaces | eTokenOptional,m_line,errors);   // there is another token

            if ((index < m_tokens.size()) && !(m_tokens[index].getID() & eTokenComment))  // have non-space, non-comment token
            {
                // have a non-comment token.
                if (vt->getType() == variableType::eVarTypeIntArray)
                {
                    errors.setError(m_line,"invalid syntax after intarray name.");
                }
                else if (m_tokens[index].getID() & eTokenAssignment)  // only token allowed by all other variables is =
                {
                    se.getNextTokenProcessSpaces(m_tokens,index,index, bFixSpace, eTokenSpace | eTokenSpaces | eTokenOptional, m_line, errors);   // there is anothere token
                    if (index < m_tokens.size())  // have more tokens
                    {
                        ScriptToken & it = m_tokens[index];
                        switch (vt->getType())
                        {
                        case variableType::eVarTypeBool:

                            if (it.getID() & (eTokenBoolFalse | eTokenBoolTrue))    // have an actual boolean.
                            {
                                vt->setValueBool( (it.getID() & eTokenBoolFalse) ? false : true );
                            }
                            else if (it.getID() & (eTokenInt | eTokenFloat))
                            {
                                errors.setWarning(m_line,"numeric value assigned to bool - should be true or false");
                                int value = std::stoi(it.getToken());
                                vt->setValueBool( value ? true : false );
                            }
                            else
                                errors.setError(m_line,"invalid boolean value");

                            break;

                        case variableType::eVarTypeInt:
                            if (it.getID() & (eTokenBoolFalse | eTokenBoolTrue))    // have an actual boolean.
                            {
                                errors.setWarning(m_line,"boolean assigned to int");
                                vt->setValueInt( (it.getID() & eTokenBoolFalse) ? 0 : 1 );
                            }
                            else if (it.getID() & (eTokenInt | eTokenFloat))
                            {
                                int value = std::stoi(it.getToken());
                                vt->setValueInt( value );
                            }
                            else
                                errors.setError(m_line,"invalid int value");
                            break;

                        case variableType::eVarTypeFloat:
                            if (it.getID() & (eTokenBoolFalse | eTokenBoolTrue))    // have an actual boolean.
                            {
                                errors.setWarning(m_line,"boolean assigned to float");
                                vt->setValueFloat( (it.getID() & eTokenBoolFalse) ? 0.0f : 1.0f );
                            }
                            else if (it.getID() & (eTokenInt | eTokenFloat))
                            {
                                float value = std::stof(it.getToken());
                                vt->setValueFloat( value );
                            }
                            else
                                errors.setError(m_line,"invalid float value");
                            break;

                        case variableType::eVarTypeMiner:
                            processAssignmentTokenID( it, vt, vars, errors, se.kS_miner);
                            break;

                        case variableType::eVarTypeVehicle:
                            processAssignmentTokenID( it, vt, vars, errors, se.kS_vehicle);
                            break;

                        case variableType::eVarTypeCreature:
                            processAssignmentTokenID(it, vt, vars, errors, se.kS_creature);
                            break;

                        case variableType::eVarTypeArrow:
                            if (it.getID() & eTokenArrowColor)
                            {
                                vt->setValueString( it.getToken() );
                            }
                            else
                                errors.setError(m_line,"invalid arrow color");
                            break;

                        case variableType::eVarTypeTimer:
                        {
                            float values[3] = { 0 };
                            std::string eventchain;

                            if (processfloats(se, index, values, 3, bFixSpace, errors))
                            {
                                ScriptToken &tit = m_tokens[index]; // event chain to call
                                if (tit.getID() & eTokenName)
                                {
                                    vt->setValueTimer(values[0], values[1], values[2], tit.getTokenlc());
                                    if (!se.isReservedVar(tit.getTokenlc()))
                                    {
                                        // last token is event chain name. Set type for later optimization
                                        tit.orID(eTokenEventChain);   // it is an event chain name
                                        break;
                                    }
                                    else
                                    {
                                        errors.setError(m_line,"timer event chain name is reserved word");
                                    }
                                }

                            }
                            errors.setError(m_line, "Invalid timer sytax or values");
                        }
                        break;

                        case variableType::eVarTypeBuilding:
                            if ((it.getID() & eTokenInt) && !(it.getID() & (eTokenBoolFalse | eTokenBoolTrue)))
                            {
                                int row = 0;
                                int col = 0;
                                row = std::stoi(it.getTokenlc());

                                if (se.getNextTokenProcessSpaces(m_tokens, index,index,bFixSpace,0, m_line, errors))    // have another token
                                {
                                    if (m_tokens[index].getID() == eTokenComma)
                                    {
                                        if (se.getNextTokenProcessSpaces(m_tokens, index, index, bFixSpace, 0, m_line, errors))
                                        {
                                            ScriptToken &tit = m_tokens[index];
                                            if ((tit.getID() & eTokenInt) && !(tit.getID() & (eTokenBoolFalse | eTokenBoolTrue)))
                                            {
                                                col = std::stoi(tit.getTokenlc());
                                                vt->setValueBuilding(row,col);
                                                if ((row < se.getRows()) && (col < se.getCols()))
                                                {
                                                    if (!vars.isValueDuplicated(vt))
                                                        break;
                                                    else
                                                    {
                                                        errors.setWarning(m_line, "Duplicate building variable row,col. Only one variable per building location allowed");
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    errors.setWarning(m_line,"row,col out of range");
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            errors.setWarning(m_line,"Invalid Building row,col");
                            break;

                        case variableType::eVarTypeString:  // string variables may have either quoted string, or unquoted. If unquoted, combine every following token into a single token.
                            if (it.getID() & eTokenString)  // quoted string, save it with the quotes
                            {
                                vt->setValueString( it.getToken() );
                                bool bExtraStuff = false;
                                for (size_t nextindex = index + 1; nextindex < m_tokens.size(); nextindex++)
                                {
                                    uint64_t id = m_tokens[nextindex].getID();
                                    if (id & eTokenComment)   // comments still stop the processing
                                        break;
                                    m_tokens[nextindex].orID( eTokenNoQuoteString );
                                    if (id & (eTokenIgnore | eTokenSpace | eTokenSpaces))
                                        continue;
                                    bExtraStuff = true;
                                }
                                if (bExtraStuff)
                                    errors.setWarning(m_line,"Additional characters not part of string");
                            }
                            else    // another format supported is unquoted string. What we do here is combine every following token into a a single unquoted string token.
                            {
                                std::string unquotedString = m_tokens[index].getToken();  // get current token.
                                m_tokens[index].orID( eTokenNoQuoteString );

                                bool bEmbeddedQuoteString = false;
                                size_t nextindex = index+1;
                                for (; nextindex < m_tokens.size(); nextindex++)
                                {
                                    uint64_t id = m_tokens[nextindex].getID();
                                    if (id & eTokenComment)   // comments still stop the processing
                                        break;
                                    if (id & eTokenIgnore)
                                        continue;
                                    if (id & eTokenString)
                                        bEmbeddedQuoteString = true;                    // remember that there are embedded quoted strings

                                    unquotedString += m_tokens[nextindex].getToken();
                                    m_tokens[nextindex].orID( eTokenIgnore );   // these tokens are no longer used
                                }
                                m_tokens[index].setToken( unquotedString );     // now all of the tokens are merged into the single unquoted string.
                                index = nextindex;
                                if (bEmbeddedQuoteString)
                                    errors.setWarning(m_line,"Embedded quoted strings - verify string contents.");
                            }
                            break;

                        } // switch
                        if (se.getNextTokenProcessSpaces(m_tokens, index,index,bFixSpace,0, m_line, errors))
                        {
                            if (!(m_tokens[index].getID() & eTokenComment))
                                errors.setError(m_line,"unexpected characters");
                        }
                    }
                    else
                        errors.setError(m_line,"Missing data after =");
                }
                else // all other tokens are invalid  Even with errors, we still add variable to detect more errors
                    errors.setError(m_line,"Missing =");
            }

            // timer variable is the only one that must have an assignment
            if ((vt->getType() == variableType::eVarTypeTimer) && !vt->getTimerEventName().length())
            {
                errors.setError(m_line,"timer variable invalid format");
            }

            vars.add( vt );  // add the variable even if we had errors.

        }

        void processAssignmentTokenID(const ScriptToken& it, variableTypeSP & vt, const allUserVariables& vars, ErrorWarning& errors, const std::string &typeName )
        {
            if (it.getID() & (eTokenFloat))
            {
                errors.setError(m_line, std::string("floating point value is not a valid ") + typeName + " id");
                float value = std::stof(it.getToken());
                vt->setValueID((int)value);
            }
            else if ((it.getID() & (eTokenInt)) && !(it.getID() & (eTokenBoolFalse | eTokenBoolTrue))) // ids are int and not bools
            {
                int value = std::stoi(it.getToken());
                vt->setValueID(value);
                if (vars.isValueDuplicated(vt))
                {
                    errors.setError(m_line, std::string("Duplicate ") + typeName + " ID - only one variable per " + typeName + " id allowed");
                }
            }
            else
                errors.setError(m_line, std::string("invalid ") + typeName + " ID - expecting integer constant");
        }

        // look for the number of floats, fill in data with the values, fill in errors if any issue.
        // input returned pointing to index of last float
        // return true if no errors, return false if any issues.
        bool processfloats(ScriptEngine &se, std::size_t& index, float* data, int numfloats, bool bFixSpace, ErrorWarning & errors)
        {
            int floatloop = 0;
            for (; floatloop < numfloats; floatloop++)
            {
                ScriptToken & it = m_tokens[index];
                if (it.getID() & eTokenMinus)  // "Levels\LRRR\oresome.dat" has -1 for delay, ignore the -
                {
                    errors.setWarning(m_line,"timer negative delay");
                    index++;
                    it = m_tokens[index];
                }
                if (!(it.getID() & (eTokenInt | eTokenFloat)))
                {
                    errors.setWarning(m_line,"timer invalid delay");
                    data[floatloop] = 0;
                }
                else
                    data[floatloop] = stof(it.getTokenlc());
                if (!se.getNextTokenProcessSpaces(m_tokens, index, index, bFixSpace, 0, m_line, errors))    // have another token
                    break;

                if (!(m_tokens[index].getID() == eTokenComma))
                {
                    errors.setError(m_line, "missing comma");
                    break;
                }

                if (!se.getNextTokenProcessSpaces(m_tokens, index, index, bFixSpace, 0, m_line, errors))
                    break;
            }
            return floatloop == numfloats;
        }

        InputLinePtr            m_line;                // holds the line, linenumber, filename
        std::deque<ScriptToken> m_tokens;              // parsed tokens for this line

        // todo might be better to use a flags setup for these.
    public:
        bool                    m_bProcessed = false;   // set to true when line has been completely processed

        bool                    m_bEventChain = false;  // true = this line starts an event chain.
        bool                    m_bEventChainEnd = false; // true = this line ends an event chain.
        bool                    m_bEvent = false;       // true = event part of event chain.
        bool                    m_eventlist = false;   // true = ? event part of event list 
        bool                    m_failedEvent = false; // true = ~ event for failed emerge
        bool                    m_bVariableDecl = false; // true = variable decleration
        bool                    m_bTrigger = false;      // true = trigger defination
        bool                    m_bCommentLineSpace = false;  // true = line is a full comment line with spaces before it.
        bool                    m_bCommentLineNoSpace = false;  // true = line is a full comment line with no spaces before it.
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
    bool loadScript(const std::deque<InputLinePtr>& scriptSectionLines, const std::deque<std::filesystem::path> &incDirs, bool bAnsi )
    {
        bool retval = true;     // assume ok
        if (m_filename.empty())
            m_inputlines = scriptSectionLines;
        else
        {
            std::deque<std::filesystem::path> processed;    // how we check for circular reference and more directories to search
            retval = loadScriptFile(m_filename,incDirs, processed, std::make_shared<InputLine>(), bAnsi, "  ");
        }
        return retval;
    }

    // setup script macros with predefined ones. We have builtin ones plus any from the command line
    int addCmdDefines(const CommandLineOptions& cmdline, int rows, int cols, const std::string &mapname)
    {
        // first add the predefined ones
        InputLinePtr emptyLine;

        m_defines.addKeyValue("TyabMapRows", std::to_string(rows), emptyLine);
        m_defines.addKeyValue("TyabMapCols", std::to_string(cols), emptyLine);
        m_defines.addKeyValue("TyabMapName", mapname,emptyLine);

        // compute date for main script
        std::string sdate = FileIO::getDateStr(m_filename, cmdline.m_datestr);
        if (sdate.empty() && !m_inputlines.empty())
        {
            sdate = FileIO::getDateStr(std::filesystem::path(Unicode::utf8_to_wstring(m_inputlines[0]->getFileName())), cmdline.m_datestr);
        }

        m_defines.addKeyValue("TyabScriptDate", sdate, emptyLine);        // main script we know its date
        m_defines.addKeyValue("TyabMMUtilDate", __DATE__, emptyLine );    // compile date for this utility
        m_defines.addKeyValue("TyabUniqueName","", emptyLine );             // every query returns a unique name

        for (auto const & it : cmdline.m_defines)
        {
            if (m_defines.contains(it.key()))
            {
                m_errors.setError(std::make_shared<InputLine>(), "Duplicate name=value from command line: " + std::string(it.key()));
                return 1;
            }
            if (isReservedWord(it.key()))
            {
                m_errors.setError(std::make_shared<InputLine>(), "name=value from command line is reserved name: " + std::string(it.key()));
                return 1;
            }

            m_defines.addKeyValue(it.key(), it.value(),std::make_shared<InputLine>());

        }
        return 0;
    }

    void processInputLines(bool bFixSpace, bool bOptimizeNames)
    {

        // pass 0 is tokenizing every line and processing pragmas (defines, includes, comments, etc)
        pass0Processing(bFixSpace);

        if (!m_errors.emptyErrors())    // have errors, return empty output
            return;

        // pass 1 is collecting up all variables and event chain names
        // pass 1 also fixes sound: events, combining following paths into a single token
        pass1Processing(bFixSpace);
        if (!m_errors.emptyErrors())    // have errors, return empty output
            return;

        // prior to pass 2 need to add those event chains defined by the block system, and they cannot be optimized
        // also those chains called by the block system need to be properly tagged as non optimizable
        std::unordered_set<std::string> optIgnore; // just the lower case name
        optIgnore.reserve(64);                     // help prevent reallocations

        processTriggerEventChain(optIgnore);   // chains defined in block system
        processEventCallEvent(optIgnore);      // variables or chains used by block system
        processObjectiveVars(optIgnore);       // variables used by objective section need to be defined in script and cannot have names optimized
        processBlockTimerVars(optIgnore);      // timer variables defined in block system

        if (!m_errors.emptyErrors())    // have errors, return empty output
            return;

        if (!optIgnore.contains(kS_init)) optIgnore.insert(kS_init);    // cannot optimize init chain name
        if (!optIgnore.contains(kS_tick)) optIgnore.insert(kS_tick);    // cannot optimize tick chain name

        // pass 2 is looking at all lines, adding token info (eventchain, variable) and types if reserved word.
        // also builds usage counts for variables and event chain names
        // spaces inside of lines are removed here
        pass2Processing(bFixSpace);
        if (!m_errors.emptyErrors())    // have errors, return empty output
            return;

        // todo any more processing or syntax checking

        // end processing.

        // build new names for user variables and event chain names based on usage
        if (bOptimizeNames)
            optimizeNames(optIgnore);

    }

    // build script output lines. Options are to remove comments, optimize names, and optimize blank lines

    std::deque<InputLinePtr> buildOutputLines( bool bNoComments, bool bOptimizeNames, bool bOptimizeBlank )
    {
                // now generate the output. have each line serialize itself out, make into a new InputLine for output
        std::deque<InputLinePtr> output;
        int linenum = 1;
        std::string filename;       // blank filename for output

        const ScriptLine * lastNonBlankLine = nullptr;
        bool bLastBlank = false;
        for (auto itp = m_scriptlines.cbegin(); itp != m_scriptlines.cend(); itp++)  // using old school iterator so we can get a pointer
        {
            const ScriptLine * slp = &(*itp);   // pointer to the script line
            bool bSkip = false;
            std::string str = slp->serialize_out(*this, bNoComments, bOptimizeNames, bSkip);
            if (bSkip)   // recommendation is to skip but we need to do more full comment line checking.
            {
                // we already know if a comment line with spaces is needed so just check the flags
                if (slp->getCommentLineSpace())     // this is a full comment line with spaces before it.
                {
                    if (bNoComments)
                    {
                        if (slp->m_bEventChainEnd) // this comment is ending an event chain, so we must convert to blank lines
                            str.clear();  // remove the comment, but keep the blank line
                        else
                            continue;     // skip the line
                    }
                }
                else   // not a full comment line with spaces, so honor the skip
                    continue;
            }

            if (bOptimizeBlank)   // optimizing blank lines
            {
                if (str.empty())  // this is a blank line
                {
                    bLastBlank = true;    // remember that we have a blank line pending
                    continue;             // don't output it yet, it may get optimized out
                }
                // a non-blank line. If we have a pending blank line it may or maynot be output depending on what this is and what the last non-blank was.
                if (bLastBlank)   // have one or more sequences of blank lines before this line
                {
                    bool bOutputBlank = false;
                    if (lastNonBlankLine) // not the first non-blank line, so need to see if this line needs a blank line based on the last non-blank line
                    {
                        // trigger,variables can skip if the prior was also a trigger or variable
                        if (slp->m_bTrigger || slp->m_bVariableDecl)  // this line is trigger or var
                        {
                            if (!lastNonBlankLine->m_bTrigger && !lastNonBlankLine->m_bVariableDecl)
                                bOutputBlank = true;
                        }
                        else if (slp->m_bEventChain) // event chain is ok after a variable or trigger
                        {
                            if (!lastNonBlankLine->m_bTrigger && !lastNonBlankLine->m_bVariableDecl)
                                bOutputBlank = true;
                        }
                        else
                            bOutputBlank = true;  // comment lines mostly, could also be event where the prior line was blank (invalid syntax but we are not fixing that)
                    }
                    if (bOutputBlank) // output the blank line skipped
                    {
                        output.push_back(std::make_shared<InputLine>(std::string(), linenum++, filename));
                    }
                }
                bLastBlank = false;     // last line not blank
                lastNonBlankLine = slp; // save current line
            }
            // output the line
            output.push_back(std::make_shared<InputLine>(str, linenum++, filename));
        } // for m_scriptlines
        return output;
    }

protected:
    // pass 0 is parsing all input lines into tokens.
    void pass0Processing( bool bFixSpace )
    {
        for (auto & it : m_inputlines)  // macros may change the input line
        {
            ScriptLine sl(it);
            auto tokens = rawParseLine( sl.getLine(), sl.getLine()->getLine(), true, true, bFixSpace, m_errors);
            sl.setTokens(tokens);

            // make sure the comment lines flags are set properly
            if (!tokens.empty())
            {
                if (tokens[0].getID() & eTokenCLSpace)  // comment with spaces
                    sl.setCommentLineSpace();  // line is a full comment line with spaces before it.
                else if (tokens[0].getID() & eTokenCommentLine) // comment with no leading spaces
                    sl.setCommentLineNoSpace();  // line is a full comment line with no leading spaces.
            }
            m_scriptlines.push_back(sl);
        }
    }

    // pass 1 is collecting up all variables and event chain names.
    void pass1Processing( bool bFixSpace )
    {
        for (auto & it : m_scriptlines)
        {
            // do spaces first
            // if first token is some sort of space, error or fix.
            it.processInitialSpace( bFixSpace, m_errors );
            // if last token is some sort of space, error or fix.
            it.processLastSpace( bFixSpace, m_errors );

            // see if any of our variable declerations
            it.processVariableDecleration( *this, m_variableNames, bFixSpace, m_errors );

            // see if starting eventchain
            it.processEventChainName( *this, m_eventChainNames, bFixSpace, m_errors );

            // currently this is just to set the line type as a trigger
            // TODO rework later with full analysis detection
            it.processTrigger( *this, bFixSpace, m_errors );  

            // used to tag line as ending in semi, used for blank line optimization
            // TODO rework later with full analysis detection
            it.processEventDetection(*this, bFixSpace, m_errors);

            // custom processing for sound event.
            // TODO rework later with full analysis detection
            it.processSoundPath( *this, bFixSpace, m_errors );
        }

        fullCommentLineWithSpacesProcessing( bFixSpace );

        // now we need to make sure ref counts are correct. Timer variables reference an event chain. If the chain is not defined, issue a warning.
        // TODO we need block processing so a timer is able to call a block event directly, and blocks can call an event chain directly
        for (auto const & it : m_variableNames.getTimerVars())
        {
            const std::string &TimerEventChainName = it.second->getTimerEventName();
            if (!TimerEventChainName.empty())  // timer vars from block system do not have a chain name so skip them.
            {
                if (m_eventChainNames.contains(TimerEventChainName))
                {
                    m_eventChainNames.incCount(TimerEventChainName);
                }
                else
                {
                    m_errors.setWarning(it.second->getLine(), "Timer event chain name does not exist - timer has no effect");
                }
            }
        }
    } // pass1Processing

    // perform full line comments with spaces processing
    // Since we have the sfixspace options, comments with leading spaces may be inside of an event chain and those spaces need to be removed, turning it into a full comment line with no leading spaces.
    // also comments with leading spaces may be at the end of an event chain. We need to tag that so the spaces are not removed.
    void fullCommentLineWithSpacesProcessing(bool bFixSpace)
    {
        bool bInEventChain = false;
        for (size_t index = 0; index < m_scriptlines.size(); index++)  // doing it this way for easy lookahead
        {
            ScriptLine& it = m_scriptlines[index];
            if (bInEventChain)
            {
                if (it.m_bTrigger || it.m_bVariableDecl) // triggers and variables and new event chains not valid inside of event chain
                {
                    bInEventChain = false;  // these also end the event chain after this error
                    m_errors.setWarning(it.getLine(), "Prior event chain not ended, trigger/variable ignored");
                }
                else if (it.m_bEventChain) // Event chain cannot be inside of an event chain
                {
                    m_errors.setWarning(it.getLine(), "Prior event chain not ended, contents of this chain now part of prior chain");
                }
                else if (it.isEmpty())
                {
                    it.setEventChainEnd();   // this ends the event chain
                    bInEventChain = false; // blank line ends event chain
                }
                else if (it.getCommentLineNoSpace())    // can ignore full comment line with no spaces
                    continue;
                else if (it.getCommentLineSpace())  // full comment line with spaces
                {
                    // inside event chain and we have a comment with spaces. 
                    // we have to determine what comes next to decide what to do.
                    if (!bFixSpace)   // we are not fixing spaces, so it ends the chain
                    {
                        it.setEventChainEnd();   // this ends the event chain
                        bInEventChain = false;   // ends the event chain
                    }
                    else  // remove spaces if this is inside of chain, otherwise it ends the chain.
                    {
                        bool bInside = false;
                        for (size_t nextindex = index + 1; nextindex < m_scriptlines.size(); nextindex++)
                        {
                            ScriptLine& nit = m_scriptlines[nextindex];
                            if (nit.getCommentLineSpace()) // another comment with spaces, keep looking (that one may also be inside of chain)
                                continue;
                            else if (nit.getCommentLineNoSpace()) // ignore comment
                                continue;
                            else if (nit.isEmpty())  // blank line ends the chain so our comment did end the chain
                                break;
                            else if (nit.m_bEventChain || nit.m_bTrigger || nit.m_bVariableDecl) // comment ended the chain
                                break;
                            else  // all that remains is event so we are inside of the chain. Safe to optimize
                            {
                                bInside = true;
                                break;
                            }
                        }
                        if (bInside)  // save to convert to full line no comment
                        {
                            it.convert2CommentLineNoSpace();
                        }
                        // and continue inside the chain looking at next line
                    }
                }
            }  // bInEventChain
            else
            {
                if (it.m_bTrigger || it.m_bVariableDecl || it.isEmpty() || it.getCommentLineNoSpace() || it.getCommentLineSpace()) // all of can be ignored outside of event chain
                    continue;
                else if (it.m_bEventChain)
                {
                    bInEventChain = true;
                    if (it.m_bEventChainEnd)    // single line event chain (does not end in semi)
                        bInEventChain = false;
                }
                else  // the only thing left is an event and they are not allowed outside of an event chain
                {
                    m_errors.setWarning(it.getLine(), "Event outside of event chain");
                }
            }
        } // for all lines
    }


    void pass2Processing(bool bFixSpace)
    {
        for (auto& it : m_scriptlines)
        {
            it.identifyTokens( *this, bFixSpace, m_errors );
        }
    }


    // this is recursive. The passed in filename is to be loaded and lines added to input lines.
    // return false if unable to load file. Return true if file is loaded
    // pragmas are processed here.
    bool loadScriptFile(const std::filesystem::path & filename, const std::deque<std::filesystem::path> & incdirs, std::deque<std::filesystem::path> & processed, const InputLinePtr &inputline, bool bAnsi, const std::string & indent )
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
        for (auto const & it : processed)
        {
            if (it.empty())     // should never happen we don't store empty paths
                continue;
            std::filesystem::path fnameonly = it.filename();
            if (filenameonly == fnameonly)
            {
                m_inputlines.push_back(std::make_shared<InputLine>(std::string("## WARNING: Include IGNORED: ")+Unicode::wstring_to_utf8(filename.wstring())+" - file already included",inputline->getLineNum(),inputline->getFileName()));
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

        FileIO                   fileio;      // how we read files.
        std::deque<InputLinePtr> lines;       // lines read from file.
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
        for (auto const & iline : lines)
        {
            linenum++;
            std::string_view lineview = iline->getLine();    // string data.

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
                m_errors.setWarning(iline,indent+"#pragma ignored if not in col 0 or missing option");
                m_inputlines.push_back(iline);
                continue;
            }

            // have "#pragma " in pos 0.
            linelcview = MMUtil::removeLeadingWhite(linelcview.substr(pos+std::size(kPragma1)-1));

            if (linelcview.find("define ") == 0)    // define new macro key=value
            {
                if (!processPragmaDefine(iline, linelcview, sizeof(kdefine),indent))
                    return false;
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( std::make_shared<InputLine> (std::string("#")+iline->getLine(),iline->getLineNum(), iline->getFileName()) );
            }
            else if (linelcview.find(ktyabscriptdate) == 0) // add in file time info from main script file
            {
                std::string value;
                if (!processPragmatyabscriptdate(iline, linelcview, sizeof(ktyabscriptdate), value, indent))
                    return false;
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( std::make_shared<InputLine> (std::string("#")+iline->getLine()+" "+value, iline->getLineNum(), iline->getFileName()));
            }
            else if (linelcview.find(ktyabscriptincdate) == 0) // add in file time info from current included script file
            {
                std::string value;
                if (!processPragmatyabscriptincdate(iline, linelcview, sizeof(ktyabscriptincdate), foundfullpath, value, indent))
                    return false;
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( std::make_shared<InputLine> (std::string("#")+iline->getLine()+" "+value, iline->getLineNum(), iline->getFileName()));
            }
            else if (linelcview.find(kinclude) == 0)    // include another script file
            {
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( std::make_shared<InputLine> (std::string("#")+iline->getLine(),iline->getLineNum(), iline->getFileName()) );
                if (!processPragmaInclude(linelcview,sizeof(kinclude),incdirs, processed, iline, bAnsi, indent))
                    return false;
            }
            else if (linelcview.find(ktyabMMDatUtil) == 0)  // info on this utility
            {
                std::string value;
                processPragmatyabMMDatUtil(value);
                // change line we store to ##pragma so it can't be processed again
                m_inputlines.push_back( std::make_shared<InputLine> (std::string("#")+iline->getLine()+" "+value, iline->getLineNum(), iline->getFileName()));
            }
            else
            {
                m_errors.setWarning(iline, indent+"unknown pragma, ignored");
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

    bool processPragmaInclude(std::string_view line, std::size_t len, const std::deque<std::filesystem::path> & incdirs, std::deque<std::filesystem::path> & processed, const InputLinePtr &lineit, bool bAnsi, const std::string & indent)
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



    bool processPragmatyabscriptincdate(const InputLinePtr & /*lineit*/, std::string_view line, std::size_t len, const std::filesystem::path& fname, std::string& value, const std::string& /*indent*/)
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


    bool processPragmatyabscriptdate(const InputLinePtr& /*lineit*/, std::string_view line, std::size_t len, std::string& value, const std::string& /*indent*/)
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
                value = FileIO::getDateStr(m_filename.empty() ? std::filesystem::path(Unicode::utf8_to_wstring(m_inputlines[0]->getFileName())) : m_filename, format);
            }
            else
            {
                ErrorWarning errors;    // dummy errors, errors are only for recursive macros
                value = m_defines.getValue("TyabScriptDate",errors);
            }
        }
        else // no format string, so take the default
        {
            ErrorWarning errors;      // dummy errors, errors are only for recursive macros
            value = m_defines.getValue("TyabScriptDate",errors);
        }
        return true;
    }


    // return false if invalid pragma define, true if pragma added
    bool processPragmaDefine(const InputLinePtr &iline, std::string_view line, std::size_t len, const std::string & indent)
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
                    else    // scan until # char - we allow initial/ending spaces to be part of value
                    {
                        for(pos = 1; (pos < value.size()) && (value[pos] != '#');pos++) ;

                        value = value.substr(0,pos);
                        value = MMUtil::removeLeadingAndTrailingWhite(value);   // remove leading and trailing white spaces
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
                        m_defines.addKeyValue(keystr,std::string(value),iline);
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


    // generic raw parser. Used to parse every script line. Can also be used to parse objectives and events in blocks
    // returns a collection of tokens found
    // InputLinePtr is the source input.
    // bAllowMacros is set to true to allow macros, Set to false for blocks and objective lines.
    // errors will be filled in if any errors
    // for script, macros will change the inputline to result after all macros expanded
    std::deque<ScriptToken> rawParseLine(InputLinePtr const & iline, std::string const &parseLine, bool bAllowComments, bool bAllowMacros, [[maybe_unused]] bool bFixSpace, ErrorWarning & errors)
    {
        std::deque<ScriptToken> parsedTokens;
        Defines * defines = bAllowMacros ? &m_defines : nullptr;  // can be used in blocks and objects, they do not support macros

        std::string localline = parseLine;              // this is what we parse into tokens. It can be subject to macro expansion

        bool bMacro = false;        // if a macro is found, reparse the line after macro expansion.
        do   // stay in loop retokenizing entire line until no more macros
        {
            bMacro = false;                             // start off no macro in this line so far
            parsedTokens.clear();                       // start off with no tokens
            std::string_view input = localline;         // view into entire line
            std::size_t inputLen = input.length();

            // check for comment
            std::size_t commentpos = input.find('#');
            if (!bAllowComments && (commentpos != input.npos))
            {
                m_errors.setWarning(iline,"Script comments not allowed");
                break;
            }
            if (commentpos == 0)                          // line starts with comment
            {
                std::string comment(input);
                if (comment.size() > 1 && comment[1] == '.')    // comments starting with #. we will expand macros into
                {
                    if (!embededMacro( iline, comment, errors, defines))
                        break;
                }

                parsedTokens.emplace_back(comment, eTokenCommentLine);     // entire line is comment
                break;   // full length comment
            }
            // full line comments that have leading space are allowed. They can be used to end event chains. But if we are fixing spaces, this is could just be an indented comment.
            // we also include the eTokenCLSpace tag so we know they need more processing during output
            if ((commentpos > 0) && (commentpos < inputLen))  // see if all blank before comment
            {
                bool bWhite = MMUtil::isEmptyStr(input.substr(0, commentpos));
                if (bWhite)   // all spaces prior to comment - this may be used to end event chains OR it could just be an indented comment via sfixspace option.
                {
                    std::string comment(input);         // use entire line as comment token
                    if (comment.size() > commentpos && comment[commentpos+1] == '.')    // comments starting with #. we will expand macros
                    {
                        if (!embededMacro( iline, comment, errors, defines))
                            break;
                    }
                    parsedTokens.emplace_back(comment, eTokenCommentLine | eTokenCLSpace);     // entire line is comment but has leading spaces
                    break;
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
                // see if entire line is empty. engine allows multiple spaces on blank line
                if (MMUtil::isEmptyStr(input))
                {
                    std::string empty;     // automatically convert it to a completely empty line
                    parsedTokens.emplace_back(empty, empty, eTokenBlankLine);   // entire line is empty - it will terminate an event chain
                }
                commentpos = inputLen;    // pretend we have a comment after end of line - this allows us to use this to stop parsing
            }


            // now sit in loop parsing tokens. Stop when we hit the comment start location (or end of string)
            std::string_view token;
            for (std::size_t npos = 0; (npos < commentpos) && !bMacro;)
            {
                std::size_t epos = npos + 1;
                // if we have an alpha, treat as some sort of name for the remaining alpha, digits and underbar
                uint8_t ch = input[npos];
                if (std::isalpha((unsigned char)ch) || (ch == '_'))   // have an alpha char. Continue while more alpha or digits
                {
                    for (; epos < commentpos; epos++)
                    {
                        if (!std::isalnum((unsigned char)input[epos]) && (input[epos] != '_')) // this char ends the token
                            break;
                    }
                    token = input.substr(npos, epos - npos);
                    // check for special true/false, we treat them as int numeric constants
                    std::string tokenlc = MMUtil::toLower(token);
                    if (tokenlc == kS_false)
                        parsedTokens.emplace_back(token, eTokenInt | eTokenBoolFalse);
                    else if (tokenlc == kS_true)
                        parsedTokens.emplace_back(token, eTokenInt | eTokenBoolTrue);
                    else if (isColor(tokenlc))
                        parsedTokens.emplace_back(token, eTokenArrowColor);
                    else
                        parsedTokens.emplace_back(token, eTokenName);
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
                        parsedTokens.emplace_back(token, token.length() > 1 ? eTokenSpaces : eTokenSpace);
                        break;
                    }

                    case '.':   // this may be a dot (datafield) not a number
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
                        if (ch != '.')  // .\ can be a path (not sure if MM even allows that but might as well support it)
                        {
                            for (; (epos < commentpos) && std::isdigit((unsigned char)input[epos]); epos++)
                                ;  // scan until first non-number
                            if ((epos < commentpos) && (isalpha((unsigned char)input[epos]) || (input[epos] == '_')))  // we have an alpha, so treat as an name
                            {
                                for (epos++; epos < commentpos && (std::isalnum((unsigned char)input[epos]) || (input[epos] == '_')); epos++)
                                    ;
                                token = input.substr(npos, epos - npos);
                                parsedTokens.emplace_back(token, eTokenName);
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
                                parsedTokens.emplace_back(token, eTokenDot);
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
                        parsedTokens.emplace_back(token, bFloat ? eTokenFloat : eTokenInt);
                        break;
                    }

                    case ';':
                    {
                        token = input.substr(npos, 1);
                        parsedTokens.emplace_back(token, eTokenSemi);
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
                        parsedTokens.emplace_back(token, tag);
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
                        parsedTokens.emplace_back(token, tag);
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
                        parsedTokens.emplace_back(token, tag);
                        break;
                    }

                    case '[':
                    {
                        token = input.substr(npos, 1);
                        parsedTokens.emplace_back(token, eTokenOBracket);
                        break;
                    }

                    case ']':
                    {
                        token = input.substr(npos, 1);
                        parsedTokens.emplace_back(token, eTokenCBracket);
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
                        parsedTokens.emplace_back(token, tag);
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
                        parsedTokens.emplace_back(token, tag);
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
                        parsedTokens.emplace_back(token, tag);
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
                        parsedTokens.emplace_back(token, tag);
                        if (tag == eTokenFslash)  // if objective, everything after the / is object and treat as string
                        {
                            token = input.substr(epos, commentpos - epos);
                            parsedTokens.emplace_back(token, eTokenObjStr);
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
                        parsedTokens.emplace_back(token, tag);
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
                        parsedTokens.emplace_back(token, tag);
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
                        parsedTokens.emplace_back(token, tag);
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
                        parsedTokens.emplace_back(token, tag);
                        break;
                    }

                    case ',':
                    {
                        token = input.substr(npos, 1);
                        parsedTokens.emplace_back(token, eTokenComma);
                        break;
                    }

                    case '"':  // start quoted string. We want to keep the quotes. The engine allows embeded quotes for literals, and drops for variables. We will just support the nested version
                    {
                        size_t lastquote = nestedQuotes( input, npos, commentpos);
                        if (lastquote)   // we have a string with correctly balanced quotes from npos to lastquote
                        {
                            token = input.substr(npos, lastquote - npos + 1);
                            epos = lastquote + 1;
                        }
                        else
                        {
                            for (; epos < commentpos && input[epos] != '"'; epos++)
                                ;
                            if ((epos >= commentpos) || (input[epos] != '"'))   // missing ending quote
                            {
                                errors.setWarning(iline, "Missing ending double quote, string contents may be incorrect");
                            }
                            if (epos == npos)
                                token = "\"\"";
                            else
                                token = input.substr(npos, epos++ - npos + 1);
                        }

                        std::string quotetoken(token);  // saved quoted string into string so we can deal with macro expansion
                        if (!embededMacro(iline, quotetoken, errors, defines))
                            break;     // error

                        parsedTokens.emplace_back(quotetoken, eTokenString);

                        break;
                    }

                    case '$':   // possible start of macro. Format is $(macro). macro is only alpha/digits, must start with alpha
                    {
                        std::size_t endpos;
                        std::string macro;
                        if (bAllowMacros)
                        {
                            if (isMacro(iline, errors, input, npos, endpos, macro))
                            {
                                if (!defines->contains(macro))
                                {
                                    errors.setError(iline, std::string("Unknown macro name: ") + macro);
                                    break;
                                }
                                std::string macrovalue = defines->getValue(macro, errors);
                                if (macrovalue.empty())
                                {
                                    errors.setError(iline, "Empty macro value");
                                    break;
                                }
                                // have macro, replace data in line, expanded line is now the input line (allows errors to capture it), and start reprocessing entire line again.
                                // this allows macros to be almost every token
                                localline.replace(npos, endpos - npos + 1, macrovalue);
                                bMacro = true;                      // will cause token processing to start all over for this line.
                                break;
                            }
                        }
                        // invalid starting $ sequence treat same as default case
                        // for now take all the unknown chars and make a token out of it (may have to also allow alpha and digits if mm engine supports unicode var names)
                        for (; (epos < commentpos) && !isKnown(input[epos]); epos++)
                            ;
                        token = input.substr(npos, epos - npos);
                        parsedTokens.emplace_back(token, eUnknown);
                        break;
                    }

                    case '?':  // random event list
                    {
                        token = input.substr(npos, epos - npos);
                        parsedTokens.emplace_back(token, eTokenEventList);
                        break;
                    }

                    case '~':  // failed emerge event
                    {
                        token = input.substr(npos, epos - npos);
                        parsedTokens.emplace_back(token, eTokenFailedEmerge);
                        break;
                    }


                    default:  // no idea. Posssible a unicode variable name (TODO - does MM engine deal with this case?)
                    {
                        // for now take all the unknown chars and make a token out of it (may have to also allow alpha and digits if mm engine supports unicode var names)
                        for (; (epos < commentpos) && !isKnown(input[epos]); epos++)
                            ;
                        token = input.substr(npos, epos - npos);
                        parsedTokens.emplace_back(token, eUnknown);
                        break;
                    }

                    } // switch
                } // else
                npos = epos;
                if (!errors.emptyErrors())  // we have processing errors
                {
                    bMacro = false;
                    break;
                }
            } // for

            // we have parsed everything not a comment. If we have a comment, now add it
            if (!bMacro && (commentpos < inputLen) && errors.emptyErrors())
            {
                std::string_view commentview = input.substr(commentpos);   // may have spaces before comment.
                std::string commentstr(commentview);                      // string that may be modified
                if (MMUtil::removeLeadingWhite(commentview).find("#.") == 0)
                {
                    if (!embededMacro(iline, commentstr, errors, defines))
                        break;
                }

                parsedTokens.emplace_back(commentstr, eTokenComment);
            }
        }
        while (bMacro == true);  // until no more macros

        return parsedTokens;
    }

    // look at the line, starting at the sindex (this should be the starting double quote)
    // scan forward and see if there are nested quotes. If there is the correct ending quote
    // before the eindex, then return the index for the final closing quote
    // return 0 if not valid nested quotes (odd number of quotes)
    size_t nestedQuotes(std::string_view line, size_t sindex, size_t eindex)
    {
        if (line[sindex] == '"')  // must start with a double quote.
        {
            size_t count = 1;     // on the current "
            size_t lastfound = sindex;
            for ( ;; count++)
            {
                size_t pos = line.find('"',lastfound + 1);
                if (pos >= eindex)
                    break;
                lastfound = pos;
            }
            if ((count & 1) == 0)  // even count
                return lastfound;
        }
        return 0;
    }


    // input is the collecton of raw objective lines.
    // scan and build a collection of every variable referenced.
    // The same variable may be used mulitple times and thus have an entry per reference
  public:
    void collectObjectiveVars(std::deque<InputLinePtr> const& objectives)
    {
        std::string variablekw = kS_variable + ':';     // variable: keyword

        for (auto const& it : objectives)  // for each objective line
        {
            std::string linelc = MMUtil::toLower(it->getLine());

            // looking for variable: If found, everything until / is part of the script conditional for the objective.
            auto startpos = linelc.find(variablekw);
            if (startpos != linelc.npos)    // found it
            {
                auto endpos = linelc.find('/',startpos+variablekw.length());
                if (endpos != linelc.npos)
                {
                    startpos += variablekw.length();
                    std::string objcond = linelc.substr(startpos,endpos-startpos);
                    if (!objcond.empty())
                    {
                        std::deque<ScriptToken> tokens = rawParseLine(it, objcond, false, false, false, m_errors );
                        for (auto const& tit : tokens)  // for all tokens.
                        {
                            if ((tit.getID() & eTokenName) && !isReservedVar(tit.getToken()))
                            {
                                 m_objectiveVars.emplace_back(it,tit.getToken());
                            }
                        }
                    }
                }
            }
        }
    }

    // input is collection of block section lines
    // scan and build list of event chains used by blocks. There are two types,
    // ones that are called by blocks into script, and ones called by script into blocks
    // The ones called by script into the block system must all be unique and later
    // when processed they are checked to ensure they are not defined by script
    // The ones block uses to call into script can be any event - not just an event chain.
    // Thus it is a script single event (one of which can be an eventchain), thus it can be references to script
    // variables. Here we collect up every name reference, similar to collectObjectiveVars.
    // also collect up timer variables defined in blocks
    // 
    void collectBlockNames(std::deque<InputLinePtr> const& blocks)
    {
        collectTriggerEventChain( blocks );
        collectEventCallEvent( blocks );
        collectTimerVars( blocks );           // timer variables used in script
    }

    struct ScriptNameStats
    {
        size_t m_numvars = 0;
        size_t m_numchains = 0;
    };

    ScriptNameStats getScriptNameStats()
    {
        ScriptNameStats stats;
        stats.m_numvars = m_variableNames.size();
        stats.m_numchains = m_eventChainNames.size();
        return stats;
    }

    // return true if another index exists, false if end of line. Skip all ignored
    // input index: token index to start. Use -1 to start from beginning.
    // output newindex: token index of token that is not already ignored
    bool getNextToken(std::deque<ScriptToken> const & tokens, std::size_t index, std::size_t& newindex) const
    {
        if ((intmax_t)index < 0)
            newindex = 0;   // first time start at 0
        else
            newindex = index + 1;  // else move to next token
        for (;newindex < tokens.size() && (tokens[newindex].getID() & eTokenIgnore); newindex++) // if ignored, move to next token
            ;

        return (newindex < tokens.size()) ? true : false;
    }

    // same as above but combined with process spaces.
    bool getNextTokenProcessSpaces(std::deque<ScriptToken> & tokens, std::size_t startindex, std::size_t& newindex, bool bFixSpace, int64_t spaceflags, InputLinePtr const &iline, ErrorWarning &errors)
    {
        std::size_t index = startindex;
        if (getNextToken(tokens, index, index))        // get next token
        {
            bool bFoundSpace = processSpaces(tokens[index], bFixSpace, spaceflags, iline, errors);  // see if space
            if (bFoundSpace)           // if we found a space
            {
                getNextToken(tokens, index, index);  // move to next token which cannot be a space
            }
        }
        newindex = index;    // index of either past end or non-space token
        return index < tokens.size();  // true - index is valid, false no more tokens.
    }

    // mask is 0 or eTokenSpace or (eTokenSpaces | eTokenSpace) bits.
    // There is never a case where we have to have eTokenSpaces
    // return true means to skip to next token since a space(s) were processed. False means to stay with current token
    bool processSpaces(ScriptToken& it, bool bFixSpace, uint64_t mask, InputLinePtr const & iline, ErrorWarning& errors)
    {
        if (it.getID() & (eTokenSpace | eTokenSpaces))  // token is some form of space
        {
            if (it.getID() & eTokenIgnore)           // already told to ignore this token
                return true;                         // ignore - tell caller to look at next token

            if (mask & (eTokenSpace | eTokenSpaces)) // some sort of spacing is allowed
            {
                if (!(mask & eTokenSpaces))          // multiple spaces not allowed
                {
                    if (it.getID() & eTokenSpaces)  // single space allowed but have multiple spaces
                    {
                        if (bFixSpace)              // allowed to fix
                        {
                            if (mask & eTokenOptional)      // if space is optional, remove it if fixing
                                it.orID( eTokenIgnore );
                            else
                                it.setToken(" ");       // change to single space
                        }
                        else
                        {
                            errors.setError(iline, "Invalid spaces - only single space allowed");
                        }
                    }
                }
                else  // multiple spaces are allowed.
                {
                    if (bFixSpace && (mask & eTokenOptional))  // if spaces are optional
                        it.orID( eTokenIgnore );               // then remove them
                }
            }
            else    // spaces not allowed
            {
                if (bFixSpace)              // allowed to fix
                {
                    it.orID( eTokenIgnore );    // set ignore to comp
                }
                else
                {
                    errors.setWarning(iline, "spaces not allowed here");
                }
            }
            return true;    // current token is a space
        }

        // token is not a space. If spaces are optional, then no error
        if (mask & (eTokenSpace | eTokenSpaces)) // some sort of spacing is allowed/required
        {
            if (!(mask & eTokenOptional))     // space is required
            {
                errors.setError(iline, "Missing required space");
            }
        }
        return false;   // token is not a space
    }


protected:

    // these are event chains defined by block system. It provides a way for script to call into block system
    void collectTriggerEventChain(std::deque<InputLinePtr> const& blocks)
    {
        constexpr std::string_view tev = "TriggerEventChain";

        for (auto const& it : blocks)  // every block line
        {
            std::string_view line = it->getLine();
            auto start = line.find(tev);
            if (start != line.npos)
            {
                start = line.find_last_of(',');     // seperator of last parameter
                if (start != line.npos)
                {
                    std::string name(MMUtil::removeLeadingAndTrailingWhite(line.substr(start + 1)));  // get the name
                    if (!name.empty())
                    {
                        if (MMUtil::isAlphaNumericName(name))
                        {
                            // should be the name of a unique exported script.
                            if (!isReservedVar(name))    // must not be reserved word
                            {
                                noOptimizeName nooptname(it, name);
                                if (!m_blockTriggerEventChains.contains(nooptname.namelc()))
                                {
                                    m_blockTriggerEventChains.emplace(nooptname.namelc(), nooptname);
                                }
                                else
                                    m_errors.setWarning(it, "Block section TriggerEventChain has duplicate exported event chain: " + name);
                            }
                            else
                                m_errors.setWarning(it, "Block section TriggerEventChain name is reserved word: " + name);
                        }
                        else
                            m_errors.setWarning(it,"Block section TriggerEventChain event chain name has invalid format");
                    }
                    else
                        m_errors.setWarning(it,"Block section TriggerEventChain name is empty");
                }
                else
                    m_errors.setWarning(it,"Block section TriggerEventChain has invalid format");
            }
        }
    }

    void collectEventCallEvent(std::deque<InputLinePtr> const& blocks)
    {
        constexpr std::string_view tev = "EventCallEvent";

        for (auto const& it : blocks)  // every block line
        {
            std::string_view line = it->getLine();
            auto start = line.find(tev);
            if (start != line.npos)
            {
                start = line.find_last_of(',');     // seperator of last parameter
                if (start != line.npos)
                {
                    std::string name(line.substr(start + 1));  // get the name
                    if (!name.empty())
                    {
                        // normally this is just an event chain name, but it can be any valid event. Thus it can use macros, variables, etc.
                        // parse it up into tokens
                        std::deque<ScriptToken> tokens = rawParseLine( it, name, false, false, false, m_errors);
                        for (auto const& tit : tokens)
                        {
                            if ((tit.getID() & eTokenName) && !isReservedEvent(tit.getToken()))
                                m_blockEventCallEvent.emplace_back(it,tit.getTokenlc());    // add name to list that must not be optimized
                        }
                    }
                    else
                    {
                        m_errors.setWarning(it,"Block section EventCallEvent name is empty");
                    }
                }
                else
                {
                    m_errors.setWarning(it,"Block section EventCallEvent has invalid format");
                }
            }
        }
    }

    // block section timer variables, add to initial list of timer variables and to list of timer variables to not optimize names for
    // ID/TriggerTimer:ROW,COL,NAME,DELAY,MAX,MIN
    // there is an older format where an extra empty parameter is between the col and name
    void collectTimerVars(std::deque<InputLinePtr> const& blocks)
    {
        constexpr std::string_view tev = "TriggerTimer";

        for (auto const& it : blocks)  // every block line
        {
            std::string_view line = it->getLine();
            auto start = line.find(tev);
            if (start != line.npos)
            {
                std::string name;
                start += tev.length() + 1;     // past : char
                start = line.find(',',start);  // end of ROW
                if (start != line.npos)
                {
                    start = line.find(',',start+1);  // end of COL
                    if (start != line.npos)
                    {
                        size_t endpos = line.find(',',start+1);  // end of NAME
                        if (endpos == start + 1)  // empty parameter, lets see if the next one has the name (older format)
                        {
                            start = endpos;
                            endpos = line.find(',',start+1);  // end of NAME
                        }
                        if ((endpos != line.npos) && (endpos > start+2))
                        {
                            name = MMUtil::removeLeadingAndTrailingWhite(line.substr(start+1,endpos-start-1)); // block timer name 
                            if (MMUtil::isAlphaNumericName(name) && !isReservedVar(name))
                            {
                                if (!m_variableNames.contains(name))
                                {
                                    variableTypeSP vt(std::make_shared<variableType>(name, variableType::eVarTypeTimer)); // everyone only has a shared_ptr to it
                                    vt->setLine(it);            // save line defining
                                    m_variableNames.add(vt);        // save as a timer variable (not saving any of the timer parameters - no reason)
                                    m_blockTimerVars.emplace_back(noOptimizeName(it, name)); // add to list of timer variables to ignore during name optimization
                                }
                                else
                                {
                                    m_errors.setWarning(it, "Block section TriggerTimer has duplicate timer name: " + name);
                                }
                            }
                            else
                            {
                                m_errors.setWarning(it, "Block section TriggerTimer has invalid timer name: " + name);
                            }
                        }
                    }
                }
                if (name.empty())
                {
                    m_errors.setWarning(it, "Block section TriggerTimer has invalid format or empty timer name");

                }
            }
        }

    }

    // process the block TriggerEventChains (chains defined by block system). Add as a defined chain or generate warning.
    // in any case that name is not allowed to be optimized
    void processTriggerEventChain(std::unordered_set<std::string>& noOptNames)
    {
        for (auto const& it : m_blockTriggerEventChains)
        {
            std::string const & name = it.second.name();      // name of event chain defined by block system
            std::string const & namelc = it.second.namelc();  // lower case name
            if (!noOptNames.contains(namelc))
                noOptNames.insert(namelc);        // add  name to not optimize
            if (!m_eventChainNames.contains(name))
                m_eventChainNames.add(name,it.second.iline());  // add that name is defined
            else
                m_errors.setWarning(it.second.iline(),"Block section TriggerEventChain name is already defined in script: "+name);
        }
    }

    // process the events called by block section. Usually these are event chains called, but they can be any event which can use variables also
    // all of the items in this list must exist as defined chains or variables.
    void processEventCallEvent(std::unordered_set<std::string>& noOptNames)
    {
        for (auto const& it : m_blockEventCallEvent)
        {
            std::string const & name = it.name();      // name of event chain defined by block system
            std::string const & namelc = it.namelc();  // lower case name
            if (!noOptNames.contains(namelc))
                noOptNames.insert(namelc);        // add  name to not optimize
            if (m_eventChainNames.contains(name))
                m_eventChainNames.incCount(name);
            else if (m_variableNames.contains(name))
                m_variableNames.incCount(name);
            else
                m_errors.setWarning(it.iline(),"Block section EventCallEvent uses undefined script name: "+name);
        }
    }

    void processObjectiveVars(std::unordered_set<std::string>& noOptNames)      // variables used by objective section need to be defined in script and cannot have names optimized
    {
        for (auto const& it : m_objectiveVars)
        {
            std::string const & name = it.name();      // name of event chain defined by block system
            std::string const & namelc = it.namelc();  // lower case name
            if (!noOptNames.contains(namelc))
                noOptNames.insert(namelc);        // add  name to not optimize
            if (m_variableNames.contains(name))
                m_variableNames.incCount(name);
            else
                m_errors.setWarning(it.iline(),"Objectives section uses undefined variable: "+name);
        }

    }

    void processBlockTimerVars(std::unordered_set<std::string>& noOptNames)
    {
        for (auto const& it : m_blockTimerVars)
        {
            std::string const & namelc = it.namelc();  // lower case name
            if (!noOptNames.contains(namelc))
                noOptNames.insert(namelc);        // add  name to not optimize
        }
    }


    // look at line starting at pos (it is the first $ char). Return true if we have valid $(name) format.
    // error will be filled in if $( found but name is invalid or no closing ) and false returned
    // if true returned, the  macronamelc is filled in with lower case name of the macro
    // if true returned, epos is next char past the closing )
    bool isMacro(const InputLinePtr &iline, ErrorWarning & errors, std::string_view line, std::size_t spos, std::size_t & epos, std::string& macroname )
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

    // if macros are not allowed, defines will be nullptr
    // return true if no error, false is line has a syntax error
    bool embededMacro(InputLinePtr const &iline, std::string& token, ErrorWarning& errors, Defines  *defines)
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
                if (isMacro(iline, errors, token, stpos, endpos, macroname))  // have macro
                {
                    std::string macrovalue;
                    if (!defines)
                    {
                        errors.setError(iline, "Macros not allowed in this line");
                        return false;  // syntax error
                    }
                    else if (!defines->contains(macroname))
                    {
                        errors.setError(iline, std::string("Unknown macro name: ") + macroname);
                        return false;   // syntax error
                    }
                    else                 // normal macro, get the value
                        macrovalue = defines->getValue(macroname,errors);

                    if (macrovalue.empty())  // we have a macro but its empty
                    {
                        errors.setError(iline, "Empty macro value");
                        return false;  // syntax error
                    }
                    token.replace(stpos, endpos - stpos + 1, MMUtil::removeAllDoubleQuotes(macrovalue));
                    bqMacro = true;
                }
            }
        } while (bqMacro);
        return true; // no syntax error
    }

    // this defines the allowed first char of a token. Note it does not allow any non-ascii variable names
    bool isKnown(char ch)
    {
        if (std::isalnum((unsigned char)ch) || std::isblank((unsigned char)ch) || (ch == '_'))
            return true;

        std::size_t npos =
            std::string_view("\",!=<>/*\\-+()[]:;.?~").find(ch);  
        return npos != std::string_view::npos;
    }

    // walk the event chain and variable names, ignore init and tick, sort in order of reference,
    // and generate new names that are as short as possible
    void optimizeNames(std::unordered_set<std::string> const &optIgnore)
    {
        class EventChainOrVariableName
        {
          public:
            EventChainOrVariableName() = default;
            EventChainOrVariableName(variableTypeSP& var)     : m_pVar(var) {}
            EventChainOrVariableName(eventChainNameSP& chain) : m_pChain(chain) {}

            void setSP(variableTypeSP& var) { m_pVar = var; }
            void setSP(eventChainNameSP& chain) { m_pChain = chain; }

            ~EventChainOrVariableName() = default;

            bool operator < (const EventChainOrVariableName& rhs) const   // used to sort. Inverted so we get descending
            {
                return getCount() > rhs.getCount(); // invert to get decending
            }
            bool operator == (const EventChainOrVariableName& rhs) const   // having both < and == gives strong ordering
            {
                return getCount() == rhs.getCount();
            }

            int getCount() const
            {
                if (m_pVar != nullptr)
                {
                    return m_pVar->getCount();
                }
                else if (m_pChain != nullptr)
                {
                    return m_pChain->getCount();
                }
                else
                    return 0;
            }
            bool isVar()   const { return m_pVar != nullptr; }
            bool isChain() const { return m_pChain != nullptr; }

            std::string getNamelc() const
            {
                std::string name;
                if (m_pVar != nullptr)
                {
                    name = m_pVar->getNamelc();
                }
                else if (m_pChain != nullptr)
                {
                    name = m_pChain->getNamelc();
                }
            }

            void setOptName(std::string& optname)
            {
                if (m_pVar != nullptr)
                {
                    m_pVar->setOptName( optname );
                }
                else if (m_pChain != nullptr)
                {
                    m_pChain->setOptName( optname );
                }

            }

        protected:
            variableTypeSP   m_pVar   = nullptr;
            eventChainNameSP m_pChain = nullptr;

        };

        // build an array of EventChainOrVariableName's and sort, and assign new optimized names.
        auto vars = getUserVariables();
        auto chains = getEventChainNames();

        size_t count = vars.size() + chains.size();
        std::vector<EventChainOrVariableName> names;    // where we put names for sorting
        names.reserve( count );                         // prevent reallocations

        // load in all the names, but check to see if it is excluded first
        for (auto const & it : vars.allVariables())  // all variables
        {
            auto sp = it.second;
            if (!optIgnore.contains(sp->getNamelc()))   // check exclusion list
                names.push_back( EventChainOrVariableName(sp) );
        }

        for (auto const & it : chains.allChainNames())  // all names that are allowed to be optimized
        {
            auto sp = it.second;
            if (!optIgnore.contains(sp->getNamelc()))   // check exclusion list (init,tick,block system names)
                names.push_back( EventChainOrVariableName(sp) );
        }

        std::sort( names.begin(), names.end() );    // sort in decending order

        // now assign optimized names
        std::string optname;    // start at beginning
        for (auto & it : names)
        {
            optname = nextOptimizedName(optname);
            it.setOptName(optname);                 // will set the optimized name into the correct data
        }
    }


    // given the last optimized name returned give the next one. Start with nullstr to start from beginning
    // first char is a-z, _  Numerics are not allowed to be first char
    // Additional chars are 0-9, a-z, _ in that order
    std::string iNextName(const std::string& name ) const
    {
        std::size_t len = name.length();
        if (!len)
            return std::string("a");        // handle first time and wrapping to larger name
        if (len == 1)  // single length is only a-z and _, and wraps after z to _ and then to a0
        {
            char current = name[0];
            if (current == 'z')
                return std::string("_");

            if (current == '_')
                return std::string("a0");   // wrap to 2 chars

            // handle the a-y cases
            char str[2];
            str[0] = current + 1;
            str[1] = 0;
            return std::string(str);
        }
        // multichars
        std::string prev = name.substr(0,len-1);
        char current = name[len-1];
        if (current == '9')  // after 9 is a
        {
            return prev + 'a';
        }
        if (current == 'z')  // after z is _
            return prev + '_';
        if (current != '_')  // handle the 0-8 and a-y cases
        {
            current++;      // next numeric or next alpha
            return prev + current;
        }
        // wrap to 0, so increment the prev substring allowing a_ -> b0 and __ -> a00
        current = '0';      // wrap to 0.
        return iNextName(prev) + current;  // recursive
    }

    // uses iNextName to generate next name. Skips reserved words.
    // pass in empty string to start.
    std::string nextOptimizedName(const std::string& last) const
    {
        std::string next = last;
        do
        {
            next = iNextName(next);    // get the next one
        }
        while (isReservedVar(next));    // skip over reserved words
        return next;
    }


    std::filesystem::path          m_filename;     // empty unless script file is replacing map script section. This is the main file.
    std::deque<InputLinePtr>       m_inputlines;   // lines to process after all include and other pragmas 
    std::deque<InputLinePtr>       m_outputlines;  // lines all processed ready to replace the script section

    std::deque<ScriptLine>         m_scriptlines;   // every line of script processed into tokens.

    allEventChainNames             m_eventChainNames;  // all event chain names
    allUserVariables               m_variableNames;    // all variable names

    // var and event chain names that are not optimized, used in objectives and block sections
    std::list<noOptimizeName>                       m_objectiveVars;             // variables used in objective section
    std::unordered_map<std::string, noOptimizeName> m_blockTriggerEventChains;   // unique
    std::list<noOptimizeName>                       m_blockEventCallEvent;       // names defined in script (usually event chain but can be variables as part of any single event).
    std::list<noOptimizeName>                       m_blockTimerVars;            // timer variables defined in block section

//    MMMap::blockEvents_t           m_blockEvents;    // data about event chains from block system

    ErrorWarning                   m_errors;
    Defines                        m_defines;      // user macros

    int                            m_rows = 0;
    int                            m_cols = 0;
};


#pragma pop_macro("min")
#pragma pop_macro("max")

#endif // _MMSCRIPT_H
