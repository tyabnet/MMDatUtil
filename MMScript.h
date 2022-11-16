// CompressScript.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifndef _MMSCRIPT_H
#define _MMSCRIPT_H

#include <stdio.h>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include "MMDatUtil.h"


// every token string and what type of token it is - use for both predefined and parsed tokens
// note that we can have multiple names, one a macro and one a trigger. This class is setup for direct use in unordered_set
// where the key is lower case string
// thus to search, do it twice, once as a macro and once as a trigger.
// saves a ref to the parsed token, a lower case version of that as a string, and the parse ID
// this class does not itself know anything about meaning, predefineds, etc.

class TokenAndId
{
public:
    TokenAndId() = default;
    ~TokenAndId() = default;

    TokenAndId(std::string_view token, uint64_t id ) { setTokenAndId(token, id); }

    void setTokenAndId(std::string_view token, uint64_t id)
    {
        m_token = token;
        m_tokenlc = toLower(token);  // make copy so we can covert to lower case
        m_id = id;
    }

    std::string_view   getToken()    const { return m_token; }    // original string
    std::string_view   getTokenLC()  const { return m_tokenlc; }  // get lower case version - use for comparisions
    const std::string &getTokenLCS() const { return m_tokenlc; }  // get lower case version - use for comparisions
    uint64_t           getTokenId()  const { return m_id; }

    bool operator==(const TokenAndId& rhs) const
    {
        return m_tokenlc == rhs.m_tokenlc;
    }

    bool operator==(std::string_view sv) const
    {
        return sv == m_tokenlc;
    }


    // just for the std::unordered_set hash and comparision functions
    class compHash {
    public:
        // comparision operator for hash
        bool operator() (const TokenAndId & lhs, const TokenAndId & rhs) const
        {
            return lhs.getTokenLCS() == rhs.getTokenLCS();    // only compare lower case
        }

        // hash operator for hash - just the lower case
        std::size_t operator()(const TokenAndId & s)const noexcept
        {
            return std::hash<std::string>{}(s.getTokenLCS());
        }
    };



protected:
    std::string_view    m_token;         // original 
    std::string         m_tokenlc;       // lower case of token
    uint64_t            m_id = 0;
};


// this class contains predefined events/macros/triggers and can perform raw tokenization.
class LangDef
{
public:
    LangDef() {}

    // returns 0 if not found, else the id flags
    uint64_t getBuiltInId(const std::string& str, uint64_t id)  // return the id if a match
    {
        TokenAndId key(str, id);
        auto it = m_builtins.find(key);
        if (it == m_builtins.end())
            return 0;
        return (*it).getTokenId();
    }

    // these bits define the meaning of the predefined macros/events/triggers, what parameters they take, what return values
    enum Flags : uint64_t {
        eUnknown = 0x0000000000000000ull,     // unknown
        eMacro = 0x0000000000000001ull,     // macro used in event or event chain
        e0Parms = 0x0000000000000002ull,     // no parameters
        e1Parms = 0x0000000000000004ull,     // 1 parameter
        e2Parms = 0x0000000000000008ull,     // 2 parameters
        e3Parms = 0x0000000000000010ull,     // 3 parameters
        eParmsInt = 0x0000000000000020ull,     // parameters are int
        eParmsFloat = 0x0000000000000040ull,     // parameters are float
        eParmsString = 0x0000000000000080ull,     // parameters are string
        eParmsVar = 0x0000000000000100ull,     // parameters is a variable usually a type of miner, building, vehicle
        eRetInt = 0x0000000000000200ull,     // returns an int
        eRetFloat = 0x0000000000000400ull,     // returns an float
        eRetMiner = 0x0000000000000800ull,     // returns a miner
        eRetVehicle = 0x0000000000001000ull,     // returns a vehicle
        eRetBuilding = 0x0000000000002000ull,     // returns a building
        eCollection = 0x0000000000004000ull,     // part of a collection
        eMiner = 0x0000000000008000ull,     // a type of miner
        eBuilding = 0x0000000000010000ull,     // a type of building
        eVehicle = 0x0000000000020000ull,     // a type of vehicle
        eDefArrow = 0x0000000000040000ull,     // define an arrow variable
        eDataField = 0x0000000000080000ull,     // datafield modifier
        eEnvironment = 0x0000000000100000ull,     // environment type of object
        eDefMiner = 0x0000000000200000ull,     // define a miner variable
        eDefVehicle = 0x0000000000400000ull,     // define a vehicle variable
        eDefBuilding = 0x0000000000800000ull,     // define a building variable
        eDefInt = 0x0000000001000000ull,     // define a int variable
        eDefFloat = 0x0000000002000000ull,     // define a float variable
        eDefString = 0x0000000004000000ull,     // define a string variable
        eDefArray = 0x0000000008000000ull,     // define a intarray variable
        eDefBool = 0x0000000010000000ull,     // define a boolean variable
        eDefObjective = 0x0000000020000000ull,     // define a objective
        eOccurance = 0x0000000040000000ull,     // occurance object (if when)
        eEvent = 0x0000000080000000ull,     // single event
        eParmsLastArrow = 0x0000000100000000ull,     // last parameter is arrow variable
        eEnableDisable = 0x0000000200000000ull,     // enable or disable event
        eEnableDisableParm = 0x0000000400000000ull,     // parameter for enable or disable
        eTrigger = 0x0000000800000000ull,     // trigger
        e3rdOptional = 0x0000001000000000ull,     // 3rd parameter is optional
        eRetBool = 0x0000002000000000ull,     // returns a bool
        eDefInit = 0x0000004000000000ull,     // special init event 
        eBoolValue = 0x0000008000000000ull,     // boolean true/false
        eColor = 0x0000010000000000ull,     // arrow colors
        eVarNameUnique = 0x0000020000000000ull,
        eEventChainUnique = 0x0000040000000000ull,
        eOptional = 0x0000080000000000ull,
        eOptionalDependent = 0x0000100000000000ull,
        eEmerge = 0x0000200000000000ull,     // emerge statement - custom due to unique parameters
        eEmergeDir = 0x0000400000000000ull,     // N S E W A 
        eCreature = 0x0000800000000000ull,     // creature name
    };



    // when we tokenize a line, each token will be one of these.
    enum TokenTypes : uint64_t
    {
        eTokenFloat = 0x0000000000000001ull,  // some sort of floating point number
        eTokenInt = 0x0000000000000002ull,  // number 
        eTokenName = 0x0000000000000004ull,  // starts with alpha, has nothing but more alpha or digits. If it starts with a numeric, it must have an alpha to identify it as a variable
        eTokenSpace = 0x0000000000000008ull,  // single space
        eTokenSpaces = 0x0000000000000010ull,  // more than 1 space
        eTokenDot = 0x0000000000000020ull,  // .
        eTokenSemi = 0x0000000000000040ull,  // ;
        eTokenColon = 0x0000000000000080ull,  // :
        eTokenDColon = 0x0000000000000100ull,  // ::
        eTokenOBrace = 0x0000000000000200ull,  // (
        eTokenDOBrace = 0x0000000000000400ull,  // ((
        eTokenCBrace = 0x0000000000000800ull,  // )
        eTokenDCBrace = 0x0000000000001000ull,  // ))
        eTokenOBracket = 0x0000000000002000ull,  // [
        eTokenCBracket = 0x0000000000004000ull,  // ]
        eTokenPlus = 0x0000000000008000ull,  // +
        eTokenPlusAssign = 0x0000000000010000ull,  // +=
        eTokenMinus = 0x0000000000020000ull,  // -
        eTokenMinusAssign = 0x0000000000040000ull,  // -=
        eTokenMultiply = 0x0000000000080000ull,  // *
        eTokenMultiplyAssign = 0x0000000000100000ull,  // *=
        eTokenFslash = 0x0000000000200000ull,  //  /  special case, follows is an objective treat as a string
        eTokenDivide = 0x0000000000400000ull,  // //  divide   not supported on older versions of the engine
        eTokenDivideAssign = 0x0000000000800000ull,  // /=
        eTokenAssignment = 0x0000000001000000ull,  // =
        eTokenEqual = 0x0000000002000000ull,  // ==
        eTokenObjStr = 0x0000000004000000ull,  //  string after / is objective, until end of line
        eTokenLess = 0x0000000008000000ull,  // <
        eTokenLessEqual = 0x0000000010000000ull,  // <=
        eTokenGreater = 0x0000000020000000ull,  // >
        eTokenGreaterEqual = 0x0000000040000000ull,  // >=
        eTokenNot = 0x0000000080000000ull,  // !  mm engine does not support this currently, we will generate an error
        eTokenNotEqual = 0x0000000100000000ull,  // !=
        eTokenComma = 0x0000000200000000ull,  // ,
        eTokenString = 0x0000000400000000ull,  // string literal (was defined within quotes), quotes not part of string
        eTokenComment = 0x0000000800000000ull,  // contains comment and optional leading space prior to comment
        eTokenBlankLine = 0x0000001000000000ull,  // entire line is empty and not a comment. Will end event chain if within one

        // remaing tokens define operations that need more processing to break down. There will be a defination for these or code
        eTokenEvent = 0x0000002000000000ull,  // next set of tokens define a event, needs event processing
        eTokenTrigger = 0x0000004000000000ull,  // next set of tokens define a trigger
        eTokenCondition = 0x0000008000000000ull,  // next set of tokens define a condition
        eTokenValue = 0x0000010000000000ull,  // next set of tokens have a value
        eTokenLogical = 0x0000020000000000ull,  // logical operation. Additional processing to ensure compatible LH and RH types



        eTokenLogicalOp = eTokenLess | eTokenLessEqual | eTokenGreater | eTokenGreaterEqual | eTokenEqual | eTokenNotEqual,  // just makes it easy when processing
    };


    struct cmdPair_t
    {
        uint64_t tokenid;
        uint64_t tokenflags;
    };

    typedef std::vector<cmdPair_t> Defination;

    Defination cmdInt = { { eTokenName, eDefInt },       { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenInt | eTokenFloat, eOptionalDependent } };
    Defination cmdFloat = { { eTokenName, eDefFloat },     { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenInt | eTokenFloat, eOptionalDependent } };
    Defination cmdArray = { { eTokenName, eDefArray },     { eTokenSpace, 0 }, { eTokenName,eVarNameUnique } };
    Defination cmdString = { { eTokenName, eDefString },    { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenString, eOptionalDependent } };
    Defination cmdBool = { { eTokenName, eDefBool },      { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenName, eOptionalDependent | eBoolValue } };
    Defination cmdArrow = { { eTokenName, eDefArrow },     { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenName, eOptionalDependent | eColor } };
    Defination cmdMiner = { { eTokenName, eDefMiner },     { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, 0}, { eTokenInt, 0 } };
    Defination cmdVehicle = { { eTokenName, eDefVehicle },   { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, 0}, { eTokenInt, 0 } };
    Defination cmdBuilding = { { eTokenName, eDefBuilding },  { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, 0}, { eTokenInt, 0 } };

    Defination cmdChainStart = { { eTokenName, eVarNameUnique | eDefInit },{ eTokenDColon, 0 }, { eTokenEvent,eOptional }, { eTokenSemi,0 } };

    Defination cmdOccurance = { { eTokenName, eOccurance}, {eTokenOBrace, 0}, {eTokenTrigger, 0}, { eTokenCBrace, 0 }, {eTokenDOBrace, eOptional}, { eTokenCondition, eOptionalDependent}, { eTokenDCBrace, eOptionalDependent},
                                  { eTokenOBracket, 0}, {eTokenEvent,0 }, {eTokenCBracket, 0}, { eTokenOBracket, eOptionalDependent }, { eTokenEvent, eOptionalDependent }, { eTokenCBracket,  eOptionalDependent } };

    Defination cmdObjective = { { eTokenName, eDefObjective}, {eTokenColon,0}, {eTokenCondition, 0}, {eTokenFslash, 0}, {eTokenObjStr, 0} };

    Defination cmdCondition = { { eTokenValue, 0 } , { eTokenLogical, 0 }, {eTokenValue, 0} };

    Defination cmdEmerge = { { eTokenName, eEmerge }, { eTokenColon, 0 }, { eTokenInt, 0 }, { eTokenComma, 0 }, { eTokenInt, 0 }, { eTokenComma,0 }, { eTokenName, eEmergeDir }, { eTokenComma, 0 }, { eTokenName, eCollection | eCreature }, { eTokenComma, 0 },  { eTokenInt, 0 } };

    // return true if this string is already defined in any of the arrays, false if not
    bool contains(std::string_view str) const 
    {
        TokenAndId key(str, 0);
        bool found = (m_builtins.find(key) != m_builtins.cend());
        if (!found)
        {
            found = (m_BuiltInTriggers.find(key) != m_BuiltInTriggers.cend());
        }
        return found;
    }

protected:
    typedef std::unordered_set<TokenAndId, TokenAndId::compHash, TokenAndId::compHash> TokenSet_t;

    // preload the hash at constructor
    TokenSet_t m_builtins =
    {
        { "crystals",                     eMacro | e0Parms | eRetInt },
        { "ore",                          eMacro | e0Parms | eRetInt },
        { "studs",                        eMacro | e0Parms | eRetInt },
        { "air",                          eMacro | e0Parms | eRetInt },
        { "miners",                       eMacro | e0Parms | eRetInt | eEnableDisableParm },
        { "vehicles",                     eMacro | e0Parms | eRetInt | eEnableDisableParm },
        { "buildings",                    eMacro | e0Parms | eRetInt | eEnableDisableParm },
        { "time",                         eMacro | e0Parms | eRetInt | eRetFloat },
        { "get",                          eMacro | e2Parms | eRetInt | eParmsInt },
        { "lastminer",                    eMacro | e0Parms | eRetMiner },
        { "lastvehicle",                  eMacro | e0Parms | eRetVehicle },
        { "lastbuilding",                 eMacro | e0Parms | eRetBuilding },
        { "ConstructedBuilding",          eMacro | e0Parms | eRetBuilding },
        { "random",                       eMacro | e2Parms | eRetInt | eRetFloat | eParmsInt | eParmsFloat },

        { "init",                         eDefInit },

        { "BuildingToolStore_C",          eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingTeleportPad_C",        eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingDocks_C",              eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingPowerStation_C" ,      eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingSupportStation_C",     eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingUpgradeStation_C",     eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingGeologicalCenter_C",   eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingOreRefinery_C",        eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingMiningLaser_C",        eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingSuperTeleport_C",      eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingElectricFence_C",      eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "BuildingPowerPath_C",          eMacro | e0Parms | eRetInt | eCollection | eBuilding | eEnableDisableParm },
        { "VehicleHoverScout_C",          eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleSmallDigger_C",         eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleSmallTransportTruck_C", eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleSMLC_C",                eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleRapidRider_C",          eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleTunnelScout_C",         eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleLoaderDozer_C",         eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleGraniteGrinder_C",      eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleChromeCrusher_C",       eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleLMLC_C",                eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleCargoCarrier_C",        eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },
        { "VehicleTunnelTransport_C",     eMacro | e0Parms | eRetInt | eCollection | eVehicle | eEnableDisableParm },

        { "EventErosion_C",               eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "EventLandslide_C",             eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "RechargeSeamGoal_C",           eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "NavModifierLava_C",            eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "NavModifierWater_C",           eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "NavModifierPowerPath_C",       eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "NavModifierRubble_C",          eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test

        { "Crystal_C",                    eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "Ore_C",                        eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "Stud_C",                       eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "Barrier_C",                    eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "Dynamite_C",                   eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test
        { "ElectricFence_C",              eMacro | e0Parms | eRetInt | eCollection | eEnvironment },  // todo test

        { "CreatureRockMonster_C",        eMacro | e0Parms | eRetInt | eCollection | eCreature },   // todo test
        { "CreatureLavaMonster_C",        eMacro | e0Parms | eRetInt | eCollection | eCreature },   // todo test
        { "CreatureIceMonster_C",         eMacro | e0Parms | eRetInt | eCollection | eCreature },   // todo test
        { "CreatureSmallSpider_C",        eMacro | e0Parms | eRetInt | eCollection | eCreature },   // todo test
        { "CreatureSlimySlug_C",          eMacro | e0Parms | eRetInt | eCollection | eCreature },   // todo test

        { "power",                        eMacro | eDataField | eBuilding | eRetBool },

        { "miner",                        eDefMiner },
        { "building",                     eDefBuilding },
        { "vehicle",                      eDefVehicle },
        { "int",                          eDefInt },
        { "float",                        eDefFloat },
        { "string",                       eDefString },
        { "intarray",                     eDefArray },
        { "bool",                         eDefBool },
        { "variable",                     eDefObjective },
        { "arrow",                        eDefArrow },

        { "if",                           eOccurance },
        { "when",                         eOccurance },

        { "n",                            eEmergeDir },  // direction parameter for emerge
        { "s",                            eEmergeDir },  // direction parameter for emerge
        { "e",                            eEmergeDir },  // direction parameter for emerge
        { "w",                            eEmergeDir },  // direction parameter for emerge
        { "a",                            eEmergeDir },  // direction parameter for emerge

        { "drill",                        eEvent | e2Parms | eParmsInt | eTrigger },
        { "msg",                          eEvent | e1Parms | eParmsString },
        { "place",                        eEvent | e3Parms | eParmsInt },
        { "wait",                         eEvent | e1Parms | eParmsInt },
        { "truewait",                     eEvent | e1Parms | eParmsInt },
        { "win",                          eEvent | e1Parms | eParmsString },
        { "lose",                         eEvent | e1Parms | eParmsString },
        { "sound",                        eEvent | e1Parms | eParmsString },  // TODO check
        { "pan",                          eEvent | e2Parms | eParmsInt },
        { "shake",                        eEvent | e1Parms | eParmsFloat },
        { "save",                         eEvent | e1Parms | eParmsVar },

        { "reset",                        eEvent | e0Parms },
        { "pause",                        eEvent | e0Parms },
        { "unpause",                      eEvent | e0Parms },
        { "resume",                       eEvent | e0Parms },
        { "speed",                        eEvent | e1Parms | eParmsInt },
        { "resetspeed",                   eEvent | e0Parms },

        { "emerge",                       eEvent | eEmerge },   // emerge is custom

        { "showarrow",                    eEvent | e3Parms | eParmsInt | eParmsLastArrow },
        { "highlight",                    eEvent | e3Parms | eParmsInt | eParmsLastArrow },
        { "hidearrow",                    eEvent | e1Parms | eParmsLastArrow },
        { "enable",                       eEvent | e1Parms | eEnableDisable },
        { "disable",                      eEvent | e1Parms | eEnableDisable },

        { "spawncap",                     eEvent | e3Parms },  // TODO PARMS wrong, finish
        { "startrandomspawn",             eEvent },            // TODO parms wrong, finish
        { "addrandomspawn",               eEvent },            // TODO parms wrong, finish
        { "spawnwave",                    eEvent },            // TODO parms wrong, finish
        { "rockmonster",                  eCreature },         // TODO test these with the spawn events 
        { "lavamonster",                  eCreature },         // TODO test these with the spawn events 
        { "icemonster",                   eCreature },         // TODO test these with the spawn events 
        { "slimyslug",                    eCreature },         // TODO test these with the spawn events 

        { "light",                        eEnableDisable },
        { "lights",                       eEnableDisable },


        { "true",                         eBoolValue },
        { "false",                        eBoolValue },
        { "red",                          eColor },
        { "green",                        eColor },
        { "blue",                         eColor },
        { "darkgreen",                    eColor },
        { "yellow",                       eColor },
        { "white",                        eColor },
        { "black",                        eColor },
    };

    // triggers are used in occurance statmements to define the trigger conditions.
    TokenSet_t m_BuiltInTriggers =
    {
        { "built",                        eTrigger | e2Parms | eParmsInt },
        { "laser",                        eTrigger | e2Parms | eParmsInt },
        { "laserhit",                     eTrigger | e2Parms | eParmsInt },
        { "change",                       eTrigger | e3Parms | eParmsInt | e3rdOptional },
        { "reinforce",                    eTrigger | e2Parms | eParmsInt },
        { "time",                         eTrigger | e1Parms | eParmsInt },
        { "hover",                        eTrigger | e2Parms | eParmsInt },
        { "click",                        eTrigger | e2Parms | eParmsInt },
        { "walk",                         eTrigger | e3Parms | eParmsInt | eMiner | e3rdOptional },
        { "drive",                        eTrigger | e3Parms | eParmsInt | eVehicle | e3rdOptional },
        { "enter",                        eTrigger | e3Parms | eParmsInt | eVehicle | eMiner | e3rdOptional },

        // collections can be used with datafields in triggers
        { "BuildingToolStore_C",          eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingTeleportPad_C",        eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingDocks_C",              eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingPowerStation_C" ,      eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingSupportStation_C",     eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingUpgradeStation_C",     eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingGeologicalCenter_C",   eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingOreRefinery_C",        eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingMiningLaser_C",        eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingSuperTeleport_C",      eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingElectricFence_C",      eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "BuildingPowerPath_C",          eTrigger | e0Parms | eRetInt | eCollection | eBuilding },
        { "VehicleHoverScout_C",          eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleSmallDigger_C",         eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleSmallTransportTruck_C", eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleSMLC_C",                eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleRapidRider_C",          eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleTunnelScout_C",         eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleLoaderDozer_C",         eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleGraniteGrinder_C",      eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleChromeCrusher_C",       eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleLMLC_C",                eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleCargoCarrier_C",        eTrigger | e0Parms | eRetInt | eCollection | eVehicle },
        { "VehicleTunnelTransport_C",     eTrigger | e0Parms | eRetInt | eCollection | eVehicle },

        { "dead",                         eTrigger | eDataField | eMiner | eVehicle | eBuilding },
        { "new",                          eTrigger | eDataField | eMiner | eVehicle | eBuilding },
        { "click",                        eTrigger | eDataField | eMiner | eVehicle | eBuilding },
        { "hurt",                         eTrigger | eDataField | eBuilding },   // TODO does this work with miners or vehicles?
        { "built",                        eTrigger | eDataField | eBuilding },
        { "power",                        eTrigger | eDataField | eBuilding | eRetBool },
        { "poweron",                      eTrigger | eDataField | eBuilding },
        { "poweroff",                     eTrigger | eDataField | eBuilding },
    };
};


// wraps up the processing of an entire script.
class ScriptProcessor
{
public:
    class Defines   // we support #pragma define key=value, there are also predefined ones
    {
    public:
        class DefineKeyValue
        {
        public:
            DefineKeyValue() = default;
            DefineKeyValue(std::string_view sv) { setKeyValue(sv, std::string_view()); }
            DefineKeyValue(std::string_view key, std::string_view value) { setKeyValue(key, value); }
            ~DefineKeyValue() = default;

            void setKeyValue(std::string_view key, std::string_view value)
            {
                m_key = key;
                m_keylc = toLower(key);
                m_value = value;
            }
            bool operator==(const DefineKeyValue& rhs) const
            {
                return m_keylc == rhs.m_keylc;
            }

            bool operator==(std::string_view sv) const
            {
                return sv == m_keylc;
            }


            std::string_view getKeyLC() const { return m_keylc; }
            const std::string &getKeyLCS() const { return m_keylc; }
            const std::string& getValueS() const { return m_value; }
            std::string_view  getValue() const { return m_value; }

            // just for the std::unordered_set hash and comparision functions
            class compHash {
            public:
                // comparision operator for hash
                bool operator() (const DefineKeyValue & lhs, const DefineKeyValue & rhs) const
                {
                    return lhs.getKeyLCS() == rhs.getKeyLCS();    // only compare lower case
                }

                // hash operator for hash - just the lower case
                std::size_t operator()(const DefineKeyValue & s)const noexcept
                {
                    return std::hash<std::string>{}(s.getKeyLCS());
                }
            };


        protected:
            std::string   m_key;    // store copies since base storage may be rebuilt
            std::string   m_keylc;
            std::string   m_value;
        };

        Defines() = default;
        ~Defines() = default;

        bool contains(std::string_view key) const
        {
            return m_defines.find(DefineKeyValue(key)) != m_defines.cend();
        }

        std::string_view getValue(std::string_view key) const
        {
            auto it = m_defines.find(DefineKeyValue(key));
            if (it != m_defines.cend())
                return (*it).getValue();
            return std::string_view();
        }

        void addKeyValue(std::string_view key, std::string_view value)
        {
            m_defines.insert(DefineKeyValue (key, value));
        }


    protected:

        std::unordered_set<DefineKeyValue, DefineKeyValue::compHash, DefineKeyValue::compHash> m_defines;
    };




    // holds ScriptLine plus all of the tokens for that line
    class ScriptProcessLine
    {
    public:
        ScriptProcessLine() = default;
        ScriptProcessLine(const ScriptFile::ScriptLine& line) { setSLine(line); }
        ~ScriptProcessLine() = default;

        // we start off with this holding nothing but references. After processing, it may own a modified string
        void setSLine(const ScriptFile::ScriptLine& line)
        {
            m_line.setLine(InputLine(line.getInputLine().getLine(), line.getInputLine().getLineNum()));
            m_line.setFileName(line.getFileName());
        }

        // change to hold an actual string
        void setLine(const std::string line)
        {
            m_line.setLine(InputLine(line, m_line.getInputLine().getLineNum()));
        }

        // no syntax analysis, simply break up into tokens
        // this will break up a single line into tokens.
        void rawParse();

        const ScriptFile::ScriptLine& getLine() const
        {
            return m_line;
        }

        int getLineNum() const
        {
            return m_line.getInputLine().getLineNum();
        }

        std::string getFileName() const
        {
            return std::string(m_line.getFileName());
        }

        std::string buildString() const
        {
            std::string str;
            str = m_line.getInputLine().getLine();
            return str;
        }

        const std::vector<TokenAndId> & getTokens() const { return m_tokens; }

        void setTokens(std::vector<TokenAndId>& tokens)
        {
            m_tokens = tokens;
        }

        bool processed() const { return m_bProcessed; }
        void setProcessed( bool val ) { m_bProcessed = val; }
        bool commentLine() const { return m_bComment; }   // return true if entire line is comment

        bool isSpace(char ch) const { return (ch == ' ') || (ch == 9); }

    protected:
        // we only allow space and tab so can't use isspace.

        // this defines the allowed first char of a token. Note it does not allow any non-ascii variable names
        bool isKnown(char ch)
        {
            if (isdigit(ch) || isalpha(ch) || isSpace(ch))
                return true;

            std::size_t npos =
                std::string_view("\",!=<>/*\\-+()[]:;.").find(ch);   // faster - no allocation needed
            return npos != std::string::npos;
        }


        ScriptFile::ScriptLine  m_line;                // holds the line, linenumber, filename
        std::vector<TokenAndId> m_tokens;              // parsed tokens for this line
        bool                    m_bComment = false;    // true if comment line 
        bool                    m_bHasComment = false; // true if line also contains context with a comment at end of line
        bool                    m_bModifed = false;
        bool                    m_bProcessed = false;  // true if fully processed
    };


    ScriptProcessor() = default;
    ~ScriptProcessor() = default;

    const ErrorWarning& getError() const { return m_errors; }

    // this is the script file, with all includes already merged in. Each line has a filename (or null if this is map embedded)
    void setScriptFile(const ScriptFile& sf)
    {
        // get a reference to all the lines
        m_lines.reserve(sf.getLines().size());

        for (std::size_t i = 0; i < sf.getLines().size(); i++)
        {
            m_lines.push_back(ScriptProcessLine(sf.getLines()[i]));
        }

    }

    int ProcessScript( const CommandLineOptions & options)
    {
        // now perform raw parsing on every line breaking up every line into an array of tokens
        for (std::size_t i = 0; i < m_lines.size(); i++)
        {
            m_lines[i].rawParse();               // returns all tokens
            ScriptPass0( m_lines[i], options );  // process pragams
        }
        if (!m_errors.getErrors().empty())
            return 1;

        // now to 1nd pass script processing
        for (std::size_t i = 0; i < m_lines.size(); i++)
        {
            ScriptPass1(m_lines[i], options);  // process tokens as part of pass 1.
        }
        if (!m_errors.getErrors().empty())
            return 1;


        // now to 2nd pass script processing
        for (std::size_t i = 0; i < m_lines.size(); i++)
        {
            ScriptPass2(m_lines[i], options );  // process tokens as part of pass 1.
        }
        if (!m_errors.getErrors().empty())
            return 1;

        return 0;
    }

    ScriptFile getScriptFile() const
    {
        ScriptFile sf;
        for (std::size_t i = 0; i < m_lines.size(); i++)
        {
            InputLine il( m_lines[i].buildString(), m_lines[i].getLineNum());
            sf.addLine(il, m_lines[i].getFileName());
        }
        return sf;
    }

    // return non-zero if error, 0 if ok
    int addCmdDefines(const CommandLineOptions& cmdline, const RRMap &outmap)
    {
        // first add the predefined ones
        m_defines.addKeyValue("TyabMapRows", std::to_string(outmap.height()));
        m_defines.addKeyValue("TyabMapCols", std::to_string(outmap.width()));

        // compute date for main script
        std::string sdate;
        if (!m_lines.empty())
        {
            sdate = getDateStr(m_lines[0].getFileName(),"y.m.d");
        }

        m_defines.addKeyValue("TyabScriptDate", sdate);        // main script we know its date
        m_defines.addKeyValue("TyabScriptIncDate", "y.m.d");   // included scripts we will compute when referenced

        for (std::size_t i = 0; i < cmdline.m_defines.size(); i++)
        {
            if (m_defines.contains(cmdline.m_defines[i].key()))
            {
                m_errors.setError(InputLine(), "Duplicate name=value from command line: " + std::string(cmdline.m_defines[i].key()));
                return 1;
            }
            if (m_langdef.contains(cmdline.m_defines[i].key()))
            {
                m_errors.setError(InputLine(), "name=value from command line is reserved name: " + std::string(cmdline.m_defines[i].key()));
                return 1;
            }

            m_defines.addKeyValue(cmdline.m_defines[i].key(), cmdline.m_defines[i].value());

        }
        return 0;
    }

protected:
    void ScriptPass0(ScriptProcessLine& pl, const CommandLineOptions& options);  // process comment pragmas pass 0
    void ScriptPass1(ScriptProcessLine &pl, const CommandLineOptions& options);  // process tokens as part of pass 1
    void ScriptPass2(ScriptProcessLine &pl, const CommandLineOptions& options);  // process tokens as part of pass 2


    std::vector<ScriptProcessLine> m_lines;
    ErrorWarning                   m_errors;

    Defines                        m_defines;   // user macros
    LangDef                        m_langdef;



};

#endif // _MMSCRIPT_H
