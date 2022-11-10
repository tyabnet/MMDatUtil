// CompressScript.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include "MMDatUtil.h"


// every token string and what type of token it is - use for both predefined and parsed tokens
// note that we can have multiple names, one a macro and one a trigger. This class is setup for direct use in unordered_set
// where the key is both the string and the trigger and macro bits.
// thus to search, do it twice, once as a macro and once as a trigger.

// note, I did this to move to string_view (C++17) but given we need to rebuild a script
// if we are fixing errors or renaming variables, it did not really help since we need to modify tokens and rebuild lines
typedef       std::string   TokenStr;
typedef       std::string & TokenStrRef;
typedef const std::string & CTokenStrRef;

class TokenAndId
{
public:
    TokenAndId() {}
    TokenAndId(CTokenStrRef token, uint64_t id ) { setTokenAndId(token, id); }

    void setTokenAndId(CTokenStrRef token, uint64_t id)
    {
        m_token = token;
        m_tokenlc = toLower(m_token);  // make copy so we can covert to lower case
        m_id = id;
    }

    void setBuiltInId(uint64_t id) { m_builtInId = id; }  // so we can save it once we know it

    CTokenStrRef getToken()     const { return m_token; }    // original string
    CTokenStrRef getTokenLC()   const { return m_token; }  // get lower case version - use for comparisions
    uint64_t     getTokenId()   const { return m_id; }
    uint64_t     getBuiltInId() const { return m_builtInId; }

    // these are defined below after LangDef
    bool operator==(const TokenAndId& rhs) const;

    // comparision operator for hash. Note that a key to us is both the string and just the macro/event/trigger bits
    bool operator() (TokenAndId const& lhs, TokenAndId const& rhs) const;

    // hash operator for hash - we also xor in the macro and trigger bits
    std::size_t operator()(TokenAndId const& s)const noexcept;


protected:
    TokenStr    m_token;         // original 
    std::string m_tokenlc;       // lower case of token
    uint64_t    m_id = 0;
    uint64_t    m_builtInId = 0; // 0 or we match a built-in keyword (will not match triggers or e
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
        eUnknown           = 0x0000000000000000ull,     // unknown
        eMacro             = 0x0000000000000001ull,     // macro used in event or event chain
        e0Parms            = 0x0000000000000002ull,     // no parameters
        e1Parms            = 0x0000000000000004ull,     // 1 parameter
        e2Parms            = 0x0000000000000008ull,     // 2 parameters
        e3Parms            = 0x0000000000000010ull,     // 3 parameters
        eParmsInt          = 0x0000000000000020ull,     // parameters are int
        eParmsFloat        = 0x0000000000000040ull,     // parameters are float
        eParmsString       = 0x0000000000000080ull,     // parameters are string
        eParmsVar          = 0x0000000000000100ull,     // parameters is a variable usually a type of miner, building, vehicle
        eRetInt            = 0x0000000000000200ull,     // returns an int
        eRetFloat          = 0x0000000000000400ull,     // returns an float
        eRetMiner          = 0x0000000000000800ull,     // returns a miner
        eRetVehicle        = 0x0000000000001000ull,     // returns a vehicle
        eRetBuilding       = 0x0000000000002000ull,     // returns a building
        eCollection        = 0x0000000000004000ull,     // part of a collection
        eMiner             = 0x0000000000008000ull,     // a type of miner
        eBuilding          = 0x0000000000010000ull,     // a type of building
        eVehicle           = 0x0000000000020000ull,     // a type of vehicle
        eDefArrow          = 0x0000000000040000ull,     // define an arrow variable
        eDataField         = 0x0000000000080000ull,     // datafield modifier
        eEnvironment       = 0x0000000000100000ull,     // environment type of object
        eDefMiner          = 0x0000000000200000ull,     // define a miner variable
        eDefVehicle        = 0x0000000000400000ull,     // define a vehicle variable
        eDefBuilding       = 0x0000000000800000ull,     // define a building variable
        eDefInt            = 0x0000000001000000ull,     // define a int variable
        eDefFloat          = 0x0000000002000000ull,     // define a float variable
        eDefString         = 0x0000000004000000ull,     // define a string variable
        eDefArray          = 0x0000000008000000ull,     // define a intarray variable
        eDefBool           = 0x0000000010000000ull,     // define a boolean variable
        eDefObjective      = 0x0000000020000000ull,     // define a objective
        eOccurance         = 0x0000000040000000ull,     // occurance object (if when)
        eEvent             = 0x0000000080000000ull,     // single event
        eParmsLastArrow    = 0x0000000100000000ull,     // last parameter is arrow variable
        eEnableDisable     = 0x0000000200000000ull,     // enable or disable event
        eEnableDisableParm = 0x0000000400000000ull,     // parameter for enable or disable
        eTrigger           = 0x0000000800000000ull,     // trigger
        e3rdOptional       = 0x0000001000000000ull,     // 3rd parameter is optional
        eRetBool           = 0x0000002000000000ull,     // returns a bool
        eDefInit           = 0x0000004000000000ull,     // special init event 
        eBoolValue         = 0x0000008000000000ull,     // boolean true/false
        eColor             = 0x0000010000000000ull,     // arrow colors
        eVarNameUnique     = 0x0000020000000000ull,
        eEventChainUnique  = 0x0000040000000000ull,
        eOptional          = 0x0000080000000000ull,
        eOptionalDependent = 0x0000100000000000ull,
        eEmerge            = 0x0000200000000000ull,     // emerge statement - custom due to unique parameters
        eEmergeDir         = 0x0000400000000000ull,     // N S E W A 
        eCreature          = 0x0000800000000000ull,     // creature name
    };



    // when we tokenize a line, each token will be one of these.
    enum TokenTypes : uint64_t
    {
        eTokenFloat            = 0x0000000000000001ull,  // some sort of floating point number
        eTokenInt              = 0x0000000000000002ull,  // number 
        eTokenName             = 0x0000000000000004ull,  // starts with alpha, has nothing but more alpha or digits. If it starts with a numeric, it must have an alpha to identify it as a variable
        eTokenSpace            = 0x0000000000000008ull,  // single space
        eTokenSpaces           = 0x0000000000000010ull,  // more than 1 space
        eTokenDot              = 0x0000000000000020ull,  // .
        eTokenSemi             = 0x0000000000000040ull,  // ;
        eTokenColon            = 0x0000000000000080ull,  // :
        eTokenDColon           = 0x0000000000000100ull,  // ::
        eTokenOBrace           = 0x0000000000000200ull,  // (
        eTokenDOBrace          = 0x0000000000000400ull,  // ((
        eTokenCBrace           = 0x0000000000000800ull,  // )
        eTokenDCBrace          = 0x0000000000001000ull,  // ))
        eTokenOBracket         = 0x0000000000002000ull,  // [
        eTokenCBracket         = 0x0000000000004000ull,  // ]
        eTokenPlus             = 0x0000000000008000ull,  // +
        eTokenPlusAssign       = 0x0000000000010000ull,  // +=
        eTokenMinus            = 0x0000000000020000ull,  // -
        eTokenMinusAssign      = 0x0000000000040000ull,  // -=
        eTokenMultiply         = 0x0000000000080000ull,  // *
        eTokenMultiplyAssign   = 0x0000000000100000ull,  // *=
        eTokenFslash           = 0x0000000000200000ull,  //  /  special case, follows is an objective treat as a string
        eTokenDivide           = 0x0000000000400000ull,  // //  divide   not supported on older versions of the engine
        eTokenDivideAssign     = 0x0000000000800000ull,  // /=
        eTokenAssignment       = 0x0000000001000000ull,  // =
        eTokenEqual            = 0x0000000002000000ull,  // ==
        eTokenObjStr           = 0x0000000004000000ull,  //  string after / is objective, until end of line
        eTokenLess             = 0x0000000008000000ull,  // <
        eTokenLessEqual        = 0x0000000010000000ull,  // <=
        eTokenGreater          = 0x0000000020000000ull,  // >
        eTokenGreaterEqual     = 0x0000000040000000ull,  // >=
        eTokenNot              = 0x0000000080000000ull,  // !  mm engine does not support this currently, we will generate an error
        eTokenNotEqual         = 0x0000000100000000ull,  // !=
        eTokenComma            = 0x0000000200000000ull,  // ,
        eTokenString           = 0x0000000400000000ull,  // string literal (was defined within quotes), quotes not part of string
        eTokenComment          = 0x0000000800000000ull,  // contains comment and optional leading space prior to comment
        eTokenBlankLine        = 0x0000001000000000ull,  // entire line is empty and not a comment. Will end event chain if within one

        // remaing tokens define operations that need more processing to break down. There will be a defination for these or code
        eTokenEvent            = 0x0000002000000000ull,  // next set of tokens define a event, needs event processing
        eTokenTrigger          = 0x0000004000000000ull,  // next set of tokens define a trigger
        eTokenCondition        = 0x0000008000000000ull,  // next set of tokens define a condition
        eTokenValue            = 0x0000010000000000ull,  // next set of tokens have a value
        eTokenLogical          = 0x0000020000000000ull,  // logical operation. Additional processing to ensure compatible LH and RH types



        eTokenLogicalOp        = eTokenLess | eTokenLessEqual | eTokenGreater | eTokenGreaterEqual | eTokenEqual | eTokenNotEqual,  // just makes it easy when processing
    };
  

    struct cmdPair_t
    {
        uint64_t tokenid;
        uint64_t tokenflags;
    };

    typedef std::vector<cmdPair_t> Defination;

    Defination cmdInt         = { { eTokenName, eDefInt },       { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenInt | eTokenFloat, eOptionalDependent } };
    Defination cmdFloat       = { { eTokenName, eDefFloat },     { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenInt | eTokenFloat, eOptionalDependent } };
    Defination cmdArray       = { { eTokenName, eDefArray },     { eTokenSpace, 0 }, { eTokenName,eVarNameUnique } };
    Defination cmdString      = { { eTokenName, eDefString },    { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenString, eOptionalDependent } };
    Defination cmdBool        = { { eTokenName, eDefBool },      { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenName, eOptionalDependent | eBoolValue } };
    Defination cmdArrow       = { { eTokenName, eDefArrow },     { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, eOptional }, { eTokenName, eOptionalDependent | eColor } };
    Defination cmdMiner       = { { eTokenName, eDefMiner },     { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, 0}, { eTokenInt, 0 } };
    Defination cmdVehicle     = { { eTokenName, eDefVehicle },   { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, 0}, { eTokenInt, 0 } };
    Defination cmdBuilding    = { { eTokenName, eDefBuilding },  { eTokenSpace, 0 }, { eTokenName,eVarNameUnique }, { eTokenAssignment, 0}, { eTokenInt, 0 } };

    Defination cmdChainStart  = { { eTokenName, eVarNameUnique | eDefInit },{ eTokenDColon, 0 }, { eTokenEvent,eOptional }, { eTokenSemi,0 } };

    Defination cmdOccurance = { { eTokenName, eOccurance}, {eTokenOBrace, 0}, {eTokenTrigger, 0}, { eTokenCBrace, 0 }, {eTokenDOBrace, eOptional}, { eTokenCondition, eOptionalDependent}, { eTokenDCBrace, eOptionalDependent},
                                  { eTokenOBracket, 0}, {eTokenEvent,0 }, {eTokenCBracket, 0}, { eTokenOBracket, eOptionalDependent }, { eTokenEvent, eOptionalDependent }, { eTokenCBracket,  eOptionalDependent } };

    Defination cmdObjective = { { eTokenName, eDefObjective}, {eTokenColon,0}, {eTokenCondition, 0}, {eTokenFslash, 0}, {eTokenObjStr, 0} };

    Defination cmdCondition = { { eTokenValue, 0 } , { eTokenLogical, 0 }, {eTokenValue, 0} };

    Defination cmdEmerge = { { eTokenName, eEmerge }, { eTokenColon, 0 }, { eTokenInt, 0 }, { eTokenComma, 0 }, { eTokenInt, 0 }, { eTokenComma,0 }, { eTokenName, eEmergeDir }, { eTokenComma, 0 }, { eTokenName, eCollection | eCreature }, { eTokenComma, 0 },  { eTokenInt, 0 } };





  protected:
    typedef std::unordered_set<TokenAndId,TokenAndId, TokenAndId> TokenSet_t;

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



// now we can define these since they use the LangDef enums
inline bool TokenAndId::operator==(const TokenAndId& rhs) const
{
    return ((m_id & (LangDef::eMacro | LangDef::eTrigger)) == (rhs.m_id & (LangDef::eMacro | LangDef::eTrigger))) && (m_tokenlc == rhs.m_tokenlc);
}

// comparision operator for hash. Note that a key to us is both the string and just the macro/event/trigger bits
inline bool TokenAndId::operator() (TokenAndId const& lhs, TokenAndId const& rhs) const
{
    return ((lhs.m_id & (LangDef::eMacro | LangDef::eTrigger)) == (rhs.m_id & (LangDef::eMacro | LangDef::eTrigger))) && (lhs.m_tokenlc == rhs.m_tokenlc);
}

// hash operator for hash - we also xor in the macro and trigger bits
inline std::size_t TokenAndId::operator()(TokenAndId const& s)const noexcept
{
    return std::hash<std::string>{}(m_tokenlc) ^ (m_id & (LangDef::eMacro | LangDef::eTrigger));
}




// this is base storage for every line. It has the original input line and will be filled in with raw tokens
class ScriptLineAndTokens
{
  public:
    typedef std::vector<TokenAndId> LineTokens_t;

    ScriptLineAndTokens() {}
    ScriptLineAndTokens(const InputLine& iline) { setInputLine(iline); }

    void setInputLine(const InputLine& iline) { m_line = iline; }
    const LineTokens_t &getTokens() const { return m_tokens; }
          LineTokens_t& getTokens()       { return m_tokens; }
    void addToken(const TokenAndId& token)
    {
        m_tokens.push_back(token);
    }

    CTokenStrRef getLineStr() const { return std::string(m_line.getLine()); }

  protected:
    LineTokens_t   m_tokens;    // tokens in order for this line
    InputLine      m_line;      // base line
};


// class used to break up a script line into tokens
// currently in a class in case I need some sort of state
class ScriptTokenizer  
{
  public:

    // no syntax analysis, simply break up into tokens
    // this will break up a single line into tokens.
    void rawParse(ScriptLineAndTokens& sline )
    {
        CTokenStrRef input = sline.getLineStr();
        std::size_t inputLen = input.length();

        // check for comment
        std::size_t commentpos = input.find('#');
        if (commentpos == 0)
        {
            sline.addToken(TokenAndId(input, LangDef::eTokenComment));     // entire line is comment
            return;   // full length comment
        }
        if ((commentpos > 0) && (commentpos < inputLen))  // see if all blank before comment
        {
            bool bWhite = true;
            for (std::size_t i = 0; i < commentpos; i++)
            {
                if (isSpace(input[i]))
                    continue;
                else
                {
                    bWhite = false;
                    break;
                }
            }
            if (bWhite)
            {
                sline.addToken(TokenAndId(input, LangDef::eTokenComment));     // entire line is comment
                return;
            }

            // now start backwards from comment, find first non-space - this was we don't have space tokens before comment.
            std::size_t i = commentpos - 1;
            for (; i > 0; i--)
            {
                if (!isSpace(input[i]))
                {
                    break;
                }
            }
            commentpos = i + 1; // either # char or beginning of white space prior to comment
        }
        else
        {
            // see if entire line is empty
            if (isEmptyStr(input))
            {
                sline.addToken(TokenAndId(input, LangDef::eTokenBlankLine));   // entire line is empty - it will terminate an event chain
                return;
            }
            commentpos = inputLen;    // pretend we have a comment after end of line - this allows us to use this to stop parsing
        }

        // now sit in loop parsing tokens
        TokenStr token;
        token.reserve(128); // should be much bigger than any token and if not, it grows
        for (std::size_t npos = 0; npos < commentpos;)
        {
            std::size_t epos = npos + 1;
            // if we have a digit, say while we have digits. Also handle float
            uint8_t ch = input[npos];
            if (isalpha(ch))   // have an alpha char. Continue while more alpha or digits
            {
                for (; epos < commentpos; epos++)
                {
                    if (!isdigit(input[epos]) && !isalpha(input[epos]))
                        break;
                }
                token = input.substr(npos, epos - npos);
                sline.addToken(TokenAndId(token, LangDef::eTokenName));
            }
            // see if single space or sequence of spaces
            else
            {
                switch (ch)
                {
                case ' ':  // space
                case 9:    //tab
                {
                    for (; (epos < commentpos) && isSpace(input[epos]); epos++)
                        ;
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, token.length() > 1 ? LangDef::eTokenSpaces : LangDef::eTokenSpace));
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
                        for (; (epos < commentpos) && isdigit(input[epos]); epos++)
                            ;  // scan until first non-number
                        if ((epos < commentpos) && isalpha(input[epos]))  // we have an alpha, so treat as an name
                        {
                            for (epos++; epos < commentpos && (isalpha(input[epos]) || isdigit(input[epos])); epos++)
                                ;
                            token = input.substr(npos, epos - npos);
                            sline.addToken(TokenAndId(token, LangDef::eTokenName));
                            break;
                        }
                        epos = npos + 1; // back so we can build number or float
                    }

                    bool bFloat = false;
                    if (ch == '.')                  // floats can being with a .
                    {
                        if (isdigit(input[epos]))  // next is a number so treat as a float
                            bFloat = true;
                        else                       // just a dot char, so add to tokens as a dot
                        {
                            token = input.substr(npos, 1);
                            sline.addToken(TokenAndId(token, LangDef::eTokenDot));
                            break;
                        }
                    }
                    for (; epos < commentpos; epos++)  // now we treat it as either an int or float
                    {
                        if (!isdigit(input[epos]))
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
                    sline.addToken(TokenAndId(token, bFloat ? LangDef::eTokenFloat : LangDef::eTokenInt));
                    break;
                }

                case ';':
                {
                    token = input.substr(npos, 1);
                    sline.addToken(TokenAndId(token, LangDef::eTokenSemi));
                    break;
                }

                case ':':  // single or double
                {
                    uint64_t tag = LangDef::eTokenColon;
                    if ((epos < commentpos) && (input[epos] == ':'))
                    {
                        tag = LangDef::eTokenDColon;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }

                case '(':  // single or double brace
                {
                    uint64_t tag = LangDef::eTokenOBrace;
                    if ((epos < commentpos) && (input[epos] == '('))
                    {
                        tag = LangDef::eTokenDOBrace;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }

                case ')':  // single or double brace
                {
                    uint64_t tag = LangDef::eTokenCBrace;
                    if ((epos < commentpos) && (input[epos] == ')'))
                    {
                        tag = LangDef::eTokenDCBrace;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }

                case '[':
                {
                    token = input.substr(npos, 1);
                    sline.addToken(TokenAndId(token, LangDef::eTokenOBracket));
                    break;
                }

                case ']':
                {
                    token = input.substr(npos, 1);
                    sline.addToken(TokenAndId(token, LangDef::eTokenCBracket));
                    break;
                }

                case '+':  // addition or asignment
                {
                    uint64_t tag = LangDef::eTokenPlus;
                    if ((epos < commentpos) && (input[epos] == '='))
                    {
                        tag = LangDef::eTokenPlusAssign;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }

                case '-':  // addition or asignment
                {
                    uint64_t tag = LangDef::eTokenMinus;
                    if ((epos < commentpos) && (input[epos] == '='))
                    {
                        tag = LangDef::eTokenMinusAssign;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }

                case '*':  // addition or asignment
                {
                    uint64_t tag = LangDef::eTokenMultiply;
                    if ((epos < commentpos) && (input[epos] == '='))
                    {
                        tag = LangDef::eTokenMultiplyAssign;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }

                case '/':  // can be divide, objective, assignment
                {
                    uint64_t tag = LangDef::eTokenFslash;
                    if (epos < commentpos)   // possible to have 2nd char
                    {
                        if (input[epos] == '/')
                        {
                            epos++;
                            tag = LangDef::eTokenDivide;
                        }
                        else if (input[epos] == '=')
                        {
                            epos++;
                            tag = LangDef::eTokenDivideAssign;
                        }
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    if (tag == LangDef::eTokenFslash)  // if objective, everything after the / is object and treat as string
                    {
                        token = input.substr(epos, commentpos - epos);
                        sline.addToken(TokenAndId(token, LangDef::eTokenObjStr));
                        epos = commentpos;  // this forces the end of processing
                    }
                    break;
                }

                case '<':  // < or <=
                {
                    uint64_t tag = LangDef::eTokenLess;
                    if ((epos < commentpos) && (input[epos] == '='))
                    {
                        tag = LangDef::eTokenLessEqual;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }

                case '>':  // > or >=
                {
                    uint64_t tag = LangDef::eTokenGreater;
                    if ((epos < commentpos) && (input[epos] == '='))
                    {
                        tag = LangDef::eTokenGreaterEqual;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }

                case '!':  // ! or !=    ! is current an error
                {
                    uint64_t tag = LangDef::eTokenNot;
                    if ((epos < commentpos) && (input[epos] == '='))
                    {
                        tag = LangDef::eTokenNotEqual;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }

                case '=':  // = or ==   
                {
                    uint64_t tag = LangDef::eTokenAssignment;
                    if ((epos < commentpos) && (input[epos] == '='))
                    {
                        tag = LangDef::eTokenEqual;
                        epos++;
                    }
                    token = input.substr(npos, epos - npos);
                    sline.addToken(TokenAndId(token, tag));
                    break;
                }


                case ',':
                {
                    token = input.substr(npos, 1);
                    sline.addToken(TokenAndId(token, LangDef::eTokenComma));
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
                        token = input.substr(npos, epos - npos+1);
                    sline.addToken(TokenAndId(token, LangDef::eTokenString));

                    break;
                }

                default:  // no idea. Posssible a unicode variable name (TODO - does MM engine deal with this case?)
                {
                    // for now take all the unknown chars and make a token out of it (may have to also allow alpha and digits if mm engine supports unicode var names)
                    for (; (epos < commentpos) && !isKnown(input[epos]); epos++)
                        ;
                    token = input.substr(npos, epos-npos);
                    sline.addToken(TokenAndId(token, LangDef::eUnknown));
                    break;
                }

                } // switch
            } // else
            npos = epos;
        } // for
        // we have parsed everything not a comment. If we have a comment, now add it
        if (commentpos < inputLen)
        {
            sline.addToken(TokenAndId(input.substr(commentpos), LangDef::eTokenComment));
        }
    } // rawParse

  protected:
    // we only allow space and tab so can't use isspace.
    bool isSpace(char ch) const { return (ch == ' ') || (ch == 9); }

    // this defines the allowed first char of a token. Note it does not allow any non-ascii variable names
    bool isKnown(char ch)
    {
        if (isdigit(ch) || isalpha(ch) || isSpace(ch) )
            return true;

        std::size_t npos =
            std::string_view("\",!=<>/*\\-+()[]:;.").find(ch);   // faster - no allocation needed
        return npos != std::string::npos;
    }

};


// this class holds every line of a script as well as parsed tokens for each line
class ScriptSection
{
  public:
    ScriptSection() {}
    ScriptSection(const MapSection& ms) { setMapSectionData(ms); }

    void setMapSectionData(const MapSection& ms)
    {
        int len = (int)ms.getLines().size();
        for (int i = 0; i < len; i++)
        {
            m_lines.push_back(Script1Line(ms.getLines()[i]));
        }
    }

    void tokenize()  // tokenize every line
    {
        ScriptTokenizer st;

        int len = (int)m_lines.size();
        for (int i = 0; i < len; i++)
        {
            m_lines[i].tokenize(st);   // TODO how to detect errors
        }
    }

    void parseScript()
    {
        LangDef ld;   // so we have builtin hash

        tokenize();   // first tokenize the input

        // first thing we will do is building up list of names. Look for variables being defined and events being defined
        // If the first token for each line is eTokenName then match it to predefined values. Other tokens will be matched later
        // based on context
        for (int lines = 0; lines < m_lines.size(); lines++)
        {
            Script1Line & s1l = m_lines[lines];
            ScriptLineAndTokens& slat = s1l.getLine();
            ScriptLineAndTokens::LineTokens_t & tokens = slat.getTokens();
            if (!tokens.empty())
            {
                TokenAndId& token = tokens[0];
                uint64_t id = token.getTokenId();   // id for the scanned token.
                switch (id)
                {
                  case 0:
                  {             // unknown token - TODO error
                      break;    
                  }

                  case LangDef::eTokenBlankLine:
                  case LangDef::eTokenComment:
                      continue;  // ignore this line completely
                  
                  case LangDef::eTokenName: // we expect some sort of name
                  {
                      // first see if it is a normal builtin
                      uint64_t bid = ld.getBuiltInId(token.getToken(), 0);
                      if (bid & (LangDef::eOccurance | LangDef::eDefObjective))
                          continue;        // when, if, init, objective do not define a user name - we will parse them later

                      else if (bid &
                          ( LangDef::eDefArrow
                          | LangDef::eDefMiner
                          | LangDef::eDefVehicle
                          | LangDef::eDefBuilding
                          | LangDef::eDefInt
                          | LangDef::eDefFloat
                          | LangDef::eDefString
                          | LangDef::eDefArray
                          | LangDef::eDefBool))  // all are variable definations
                      {
                          // next token must be a space
                          if (tokens.size() < 3)
                          {
                              // generate error
                              continue;
                          }
                          
                          if (tokens[1].getTokenId() != LangDef::eTokenSpace)
                          {
                              // generate error
                              continue;
                          }

                          if (tokens[2].getTokenId() != LangDef::eTokenName)
                          {
                              // generate error
                              continue;
                          }

                          // we have a name. It cannot be ANY builtin name. Compare to three types
                          if (ld.getBuiltInId(tokens[2].getToken(), 0)
                              || ld.getBuiltInId(tokens[2].getToken(), LangDef::eMacro)
                              || ld.getBuiltInId(tokens[2].getToken(), LangDef::eTrigger))
                          {
                              // error - reserved name
                              continue;
                          }

                          // name is not builtin. Check to see if we have this name already
                          UserName username(tokens[2].getToken(), tokens[2].getTokenLC());
                          auto it = m_UserNames.find(username);
                          if (it != m_UserNames.end())
                          {
                              // error defining same name twice
                              continue;
                          }
                          // have a good name. We will add it if the rest of the line is ok
                          // if anything more we either have a comment  or =
                          if (tokens.size() > 3)
                          {
                              if (tokens[3].getTokenId() != LangDef::eTokenComment && tokens[3].getTokenId() != LangDef::eTokenAssignment)
                              {
                                  // syntax error
                                  continue;
                              }
                              if (tokens[3].getTokenId() == LangDef::eTokenAssignment)
                              {
                                  // next token must be
                              }

                          }









                          username.incCount();  // count is one.
                          m_UserNames.insert(username);


                      }
                      else if (bid & LangDef::eDefInit) // predefined init event
                      {
                      }
                      else { // may be event chain. See if next token is ::

                      }
                      break;
                  }
                  
                  
                  default:
                      break;
                      // ERROR invalid syntax
                } // switch
            }
        }

    }

protected:
    class Script1Line
    {
      public:
        Script1Line() {}
        Script1Line(const InputLine& iline) : m_line(iline) {}

        void tokenize(ScriptTokenizer& st)
        {
            st.rawParse(m_line);
        }

        ScriptLineAndTokens& getLine() { return m_line; }

      protected:
        ScriptLineAndTokens m_line;

        // lots more to come

        bool                m_eventChain = false;   // true if this line is part of an event chain (or defines one)
    };

    // used to hold user defined names - variables and event chains and a count.
    // counts are used when redefining names, we do so based on usage
    class UserName
    {
    public:
        UserName() {}
        UserName(const std::string& name, const std::string& namelc) { setName(name, namelc); }
        UserName(const std::string& name) { setName(name); }

        void incCount() { m_count++; }  // name has been seen again, inc the count

        // these are defined below after LangDef
        bool operator==(const UserName& rhs) const
        {
            return m_namelc == rhs.m_namelc;
        }

        // comparision operator for hash. Note that a key to us is both the string and just the macro/event/trigger bits
        bool operator() (UserName const& lhs, UserName const& rhs) const
        {
            return lhs.m_namelc == rhs.m_namelc;
        }

        // hash operator for hash
        std::size_t operator()(UserName const& s)const noexcept
        {
            return std::hash<std::string>{}(s.m_namelc);
        }


    private:
        void setName(const std::string& name, const std::string& namelc)
        {
            m_name = name;
            m_namelc = namelc;
            m_count = 1;
        }

        void setName(const std::string& name)
        {
            m_name = name;
            m_namelc = toLower(name);
            m_count = 1;
        }

        std::string m_name;              // name (original case)
        std::string m_namelc;            // lower case name
        int         m_count = 0;         // number of times we have seen this name (at least 1)
    };

    std::vector<Script1Line>                         m_lines;     // the entire script
    std::unordered_set<UserName, UserName, UserName> m_UserNames; // track user names
};







#if 0

class ScriptFile;   // forward

class ScriptLine
{
  public:
    ScriptLine() {};
    ScriptLine(std::string line, int num ) : m_sLine(line), m_nLine(num) {};

    void process()
    {
        m_bBlank = isEmptyStr(m_sLine);
        if (m_sLine.length() > 0)
        {
            printf("  Warning: line:%d Empty line has white space - white spaces removed\n", m_nLine);
        }
        if (m_bBlank)
            m_sLine.clear();
        else   // not a blank line
        {
            if (m_sLine[0] <= ' ')
            {
                printf("  Warning: line:%d Leading white space not allowed - white spaces removed\n", m_nLine);
//                m_sLine = removeLeadingWhite(m_sLine);
            }
            if (m_sLine[0] == '#')  // comment only line
            {
                m_bComment = true;
            }

        }
        // check for leading white space


    }

  protected:
    std::string  m_sLine;               // this line
    ScriptFile*  m_pInclude = nullptr;  // if include line, points to file to include
    int          m_nLine = 0;           // line number (starts at 1)
    bool         m_bBlank = false;      // true if blank line
    bool         m_bComment = false;    // true if comment line 
    bool         m_bHasComment = false; // true if line also contains context with a comment at end of line
    bool         m_bInclude = false;    // true if this line is an include
};


class ScriptFile
{
  public:
      ScriptFile(std::string name) : m_sName(name) { m_lines.reserve(4096); }

      int open(const char* mode)
      {
          errno_t err = 0;
          m_fp = nullptr;
          err = fopen_s(&m_fp, m_sName.c_str(), mode );
          if (err || !m_fp)
          {
              return err ? err : 1;
          }
          return 0;
      }
      const std::string & name() const { return m_sName; }
      std::string name() { return m_sName; }

      // read in all lines
      int read()
      {
          if (m_fp)
          {
              int linenum = 1;
              std::string line;
              for (; !feof(m_fp); linenum++)
              {
                  //line = readLine(m_fp);
                  if (line.empty() && feof(m_fp))   // all done
                      break;
                  ScriptLine sline(line,linenum);
                  sline.process();
                  m_lines.push_back(sline);
              }
          }
          return 0;
      }

  protected:
    ErrorLine               m_error;
    std::string             m_sName;          // name of file
    FILE*                   m_fp = nullptr;   // file handle
    std::vector<ScriptLine> m_lines;          // all the lines it holds
};
#endif

