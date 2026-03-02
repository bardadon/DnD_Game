#pragma once

#include <string>
#include <algorithm>
#include <exception>
#include <iostream>
#include <unordered_map>
//#include <math.h>

/****************************************************
			===== COLOR CONSTANTS =====
*****************************************************/
const std::string C_RESET = "\033[0m";
const std::string C_RED = "\033[31m";
const std::string C_GREEN = "\033[32m";
const std::string C_YELLOW = "\033[33m";
const std::string C_BLUE = "\033[34m";
const std::string C_MAGENTA = "\033[35m";
const std::string C_CYAN = "\033[36m";
const std::string C_WHITE = "\033[37m";



/****************************************************
			===== VALUE CONSTANTS =====
*****************************************************/
constexpr int G_STARTING_GOLD = 20; // Start player gold
constexpr int G_STARTING_INVENTORY_CAPACITY = 50; // Starting player inventory space

// Area and Room size
constexpr int G_ROOM_LENGTH = 10;
constexpr int G_AREA_LENGTH = 1;
constexpr int G_MAX_ROOM_SIZE = G_ROOM_LENGTH * G_ROOM_LENGTH; // Each room is a 100x100 grid of points
constexpr int G_MAX_AREA_SIZE = G_AREA_LENGTH * G_AREA_LENGTH; // Each area is a 10x10 grid of rooms
constexpr int G_MAX_CLASS_LEVEL = 20; // Max char level
constexpr int G_AVERAGE_NUMBER_OF_MONSTERS_IN_ROOM = 10; // General average num of monsters in a random room



/****************************************************
			===== User Defines =====
*****************************************************/
using UserCommand = std::string;
using WorldIndex = size_t;





/****************************************************
			===== ENUMS =====
*****************************************************/
enum ERROR_CODE
{
	ERROR_CODE_CONTINUE_GAME_LOOP = 0,
	ERROR_CODE_GET_NEXT_COMMAND,
	ERROR_CODE_FAILED,
	ERROR_CODE_OK,

	ERROR_CODE_LAST
};

enum AREA_NAME
{
	AREA_NAME_WHISPERING_WOODS = 0,
	AREA_NAME_STORMVALE_PLAINS,
	AREA_NAME_PLAINS_OF_THUNDER,
	AREA_NAME_ASHEN_DESERT,
	AREA_NAME_TUNDRA_WASTES,
	AREA_NAME_EMBER_SANDS,
	AREA_NAME_FROSTWIND_CLIFFS,
	AREA_NAME_HIGH_PEAKS,
	AREA_NAME_ASHEN_PEAKS,
	AREA_NAME_EMBERLIGHT_CITADEL,
	AREA_NAME_RUINS_OF_ARVANDOR,
	AREA_NAME_ANCIENT_CATACOMBS,
	AREA_NAME_SWAMPLANDS_OF_MURK,
	AREA_NAME_CURSED_MARSHES,
	AREA_NAME_COASTLINE_PORTS,
	AREA_NAME_FORGOTTEN_COAST,
	AREA_NAME_FROSTBITE_FJORD,
	AREA_NAME_TWILIGHT_HOLLOW,
	AREA_NAME_SHADOWLANDS,

	AREA_NAME_LAST,
	AREA_NAME_NONE
};

enum POINT_VISIBILITY
{
	POINT_VISIBILITY_OPEN,      // plain sight
	POINT_VISIBILITY_LIGHT_COVER,
	POINT_VISIBILITY_HEAVY_COVER,
	POINT_VISIBILITY_ELEVATED,

	POINT_VISIBILITY_LAST
};

enum FACTION
{
	FACTION_EMBERLIGHT_COUNCIL = 0,
	FACTION_SHADOW_CULT,
	FACTION_VERDANT_CIRCLE,
	FACTION_SEABORN_GUILD,
	FACTION_NOMADIC_TRIBES
};

enum DIRECTION
{
	DIRECTION_NORTH = 0,
	DIRECTION_SOUTH,
	DIRECTION_EAST,
	DIRECTION_WEST, 

	DIRECTION_LAST
};

enum INITIATIVE_HELPER
{
	INITIATIVE_HELPER_MONSTER_0 = 0,
	INITIATIVE_HELPER_MONSTER_1,
	INITIATIVE_HELPER_MONSTER_2,
	INITIATIVE_HELPER_MONSTER_3,
	INITIATIVE_HELPER_MONSTER_4,
	INITIATIVE_HELPER_MONSTER_5,
	INITIATIVE_HELPER_MONSTER_6,
	INITIATIVE_HELPER_MONSTER_7,
	INITIATIVE_HELPER_MONSTER_8,
	INITIATIVE_HELPER_MONSTER_9,
	INITIATIVE_HELPER_MONSTER_10,

	INITIATIVE_HELPER_PLAYER_0,
	INITIATIVE_HELPER_LAST,
	INITIATIVE_HELPER_NONE
};

enum MONSTER_TYPE
{
	MONSTER_TYPE_WOLF = 0,
	MONSTER_TYPE_BEAR,
	MONSTER_TYPE_DEMON,
	MONSTER_TYPE_SPIDER,
	MONSTER_TYPE_UNDEAD,
	MONSTER_TYPE_GOBLIN,
	MONSTER_TYPE_TROLL,
	MONSTER_TYPE_DRAGON,
	MONSTER_TYPE_UNKNOWN,

	MONSTER_TYPE_LAST,
	MONSTER_TYPE_NONE
};


enum ATTACK_TYPE
{
	ATTACK_TYPE_SHORT_RANGE = 1,
	ATTACK_TYPE_REACH_RANGE,
	ATTACK_TYPE_LONG_RANGE,

	ATTACK_TYPE_UNKNOWN,
	ATTACK_TYPE_LAST,
	ATTACK_TYPE_NONE
};

enum WEAPON_TYPE
{
	WEAPON_TYPE_SIMPLE_MELEE = 0,
	WEAPON_TYPE_SIMPLE_RANGED,
	WEAPON_TYPE_MARTIAL_MELEE,
	WEAPON_TYPE_MARTIAL_RANGED,

	WEAPON_TYPE_UNKNOWN,
	WEAPON_TYPE_LAST,
	WEAPON_TYPE_NONE
};


enum DAMAGE_TYPE
{
	// Physical Damage
	DAMAGE_TYPE_BLUDGEONING = 0,
	DAMAGE_TYPE_PIERCING,
	DAMAGE_TYPE_SLASHING,

	// Elemental & Magical
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_COLD,
	DAMAGE_TYPE_LIGHTNING,
	DAMAGE_TYPE_THUNDER,
	DAMAGE_TYPE_ACID,
	DAMAGE_TYPE_POISON,

	// Esoteric / Magical
	DAMAGE_TYPE_PSYCHIC,
	DAMAGE_TYPE_RADIANT,
	DAMAGE_TYPE_NECROTIC,
	DAMAGE_TYPE_FORCE,

	// Special Cases
	DAMAGE_TYPE_TRUE,
	DAMAGE_TYPE_HEALING,

	DAMAGE_TYPE_UNKNOWN,
	DAMAGE_TYPE_LAST
};

enum ATTACK_ABILITY
{
	ATTACK_ABILITY_STRENGTH = 0,
	ATTACK_ABILITY_DEXTERITY,
	ATTACK_ABILITY_CONSTITUTION,
	ATTACK_ABILITY_INTELLIGENCE,
	ATTACK_ABILITY_WISDOM,
	ATTACK_ABILITY_CHARISMA,

	ATTACK_ABILITY_UNKNOWN,
	ATTACK_ABILITY_LAST
};

enum ACTION_TYPE
{
	ACTION_TYPE_ACTION = 0,
	ACTION_TYPE_BONUS_ACTION,
	ACTION_TYPE_REACTION,
	ACTION_TYPE_FREE_ACTION,      // talking, dropping object
	ACTION_TYPE_MOVEMENT,         // movement on your turn
	ACTION_TYPE_SPECIAL,          // class-specific unique actions
	ACTION_TYPE_END_TURN,

	ACTION_TYPE_UNKNOWN,
	ACTION_TYPE_LAST
};





/****************************************************
			===== STRUCTS =====
*****************************************************/
struct CombatActors
{
	CombatActors(INITIATIVE_HELPER eCombatActor = INITIATIVE_HELPER_NONE, int nRollResult = 0) :
		m_eCombatActor(eCombatActor), m_nRollResult(nRollResult)
	{

	}

	INITIATIVE_HELPER m_eCombatActor;
	int m_nRollResult;
};




struct STATS
{
	int m_nArmorClass;
	int m_nHealth;
	int m_nSpeed;

	int m_nStrength;
	int m_nDextirity;
	int m_nConstitution;
	int m_nIntelligence;
	int m_nWisdom;
	int m_nCharisma;

	STATS(
		int nArmorClass = 10,
		int nHealth = 20,
		int nSpeed = 5,

		int nStrength = 10,
		int nDextirity = 10,
		int nConstitution = 10,
		int nIntelligence = 10,
		int nWisdom = 10,
		int nCharisma = 10
	) :
		m_nArmorClass(nArmorClass),
		m_nHealth(nHealth),
		m_nSpeed(nSpeed),

		m_nStrength(nStrength),
		m_nDextirity(nDextirity),
		m_nConstitution(nConstitution),
		m_nIntelligence(nIntelligence),
		m_nWisdom(nWisdom),
		m_nCharisma(nCharisma)
	{
	}
};

struct SKILLS
{
	int m_nAcrobatics;
	int m_nAnimalHandling;
	int m_nArcana;
	int m_nAthletics;
	int m_nDeception;
	int m_nHistory;
	int m_nInsight;
	int m_nIntimidation;
	int m_nInvestigation;
	int m_nMedicine;
	int m_nNature;
	int m_nPerception;
	int m_nPerformance;
	int m_nPersuasion;
	int m_nReligion;
	int m_nSleightOfHand;
	int m_nStealth;
	int m_nSurvival;


	/*
	I'll describe this from the player's perspective but this also works the other way around.

	The player has a vision skill that determines how good they are at spotting monsters.
	we combine this with the monster's visiblity, which determines how likely they are to be spotted.
	And we combine that with the point's location of the monster.

	So a player can have average vision skills, they can look at a large monster that have very high visiblity
	but the monster is in a high and bushy location which will hinder the player's ability to spot them


	Helper::CalculateVisionFactor will take a vision skill, visiblity and point's location
	and return the visionFactor.

	If the visionFactor factor is higher than the visibilty of the thing we are trying to see, than 
	we can see it
	*/

	int m_nVision; // How good player/monster are at seeing and spotting other things
	double m_nVisibility; // How likely it is that other player/monster will spot you

	SKILLS(
		int acrobatics = 0,
		int animalHandling = 0,
		int arcana = 0,
		int athletics = 0,
		int deception = 0,
		int history = 0,
		int insight = 0,
		int intimidation = 0,
		int investigation = 0,
		int medicine = 0,
		int nature = 0,
		int perception = 0,
		int performance = 0,
		int persuasion = 0,
		int religion = 0,
		int sleightOfHand = 0,
		int stealth = 0,
		int survival = 0,

		int vision = 0,
		double visibiltiy = 0
	) :
		m_nAcrobatics(acrobatics),
		m_nAnimalHandling(animalHandling),
		m_nArcana(arcana),
		m_nAthletics(athletics),
		m_nDeception(deception),
		m_nHistory(history),
		m_nInsight(insight),
		m_nIntimidation(intimidation),
		m_nInvestigation(investigation),
		m_nMedicine(medicine),
		m_nNature(nature),
		m_nPerception(perception),
		m_nPerformance(performance),
		m_nPersuasion(persuasion),
		m_nReligion(religion),
		m_nSleightOfHand(sleightOfHand),
		m_nStealth(stealth),
		m_nSurvival(survival),

		m_nVision(vision),
		m_nVisibility(visibiltiy)
	{
	}
};



struct MONSTER_ATTRIBUTES
{
	MONSTER_ATTRIBUTES(
		int nVisibilityThreshold = 10,
		MONSTER_TYPE eType = MONSTER_TYPE_UNKNOWN,
		int nMovementRollThreshold = 5,
		int nAggression = 5,
		int nCowardice = 5,
		bool bHasLongRangeAttack = false,
		int nExp = 50
	) :
		m_nVisibilityThreshold(nVisibilityThreshold),
		m_eType(eType),
		m_nMovementRollThreshold(nMovementRollThreshold),
		m_nAggression(nAggression),
		m_bHasLongRangeAttack(bHasLongRangeAttack),
		m_nExp(nExp)
	{
	}

	STATS m_sStats;
	SKILLS m_sSkills;
	int m_nVisibilityThreshold; // If the player's visibilty is higher than this. They can see the monster
	MONSTER_TYPE m_eType;

	int m_nMovementRollThreshold; // likelihood to move

	// Aggression is the likelihood of the monster to either:
	//	- Run away from the player(cowardly)
	//	- Move randomly or dont move at all(not aggresive and not cowardly)
	//	- run to the player and attack them(very aggressive)
	// 
	// Aggression is like a scale starting from Cowardly ---> benign ----> Mildly Aggressive ---> Extremely Aggressive etc
	// If a monster has very high Aggression or very low Aggression(i,e cowardly and wants to run away from the player), it will increase their chance to move.
	// THe more a monster is aggresive or cowardly, the more chance they have to move to mimick the behavior of chasing a player, or running away from a player
	// However, no matter what, the player can always move each turn and even the most aggressive monster SHOULD NOT
	int m_nAggression;            
	bool m_bHasLongRangeAttack;
	int m_nExp; // Amount of exp recieved when defeated
};


struct PLAYER_ATTRIBUTES
{
	STATS m_sStats;
	SKILLS m_sSkills;

	int m_nLvl = 1; // Current player lvl
	int m_nExp = 0; // Current player exp
	int m_ProfBonus = 2; // Current player proficiency bonus
};

enum FEATURE_TYPE 
{
	// Barbarian Features
	FEATURE_TYPE_RAGE = 0,
	FEATURE_TYPE_FAST_MOVEMENT,
	FEATURE_TYPE_BRUTAL_CRITICAL,
	FEATURE_TYPE_DAMAGE_RESISTANCE,
	FEATURE_TYPE_DANGER_SENSE,
	FEATURE_TYPE_RECKLESS_ATTACK,
	FEATURE_TYPE_UNARMORED_DEFENCE,









	FEATURE_TYPE_UNKNOWN,
	FEATURE_TYPE_LAST
};

struct ClassFeature 
{
	FEATURE_TYPE type;


	// Generic types to store any kind of data needed to implement a feature
	int intValue = 0;           
	float floatValue = 0.0f;    
	bool boolValue = false;		
	std::string text;           


	// add more if needed

};

struct ATTACK_INFO
{
	ATTACK_INFO()
	{
		clear();
	}

	int nDamage;
	int nAttackDistance;
	ATTACK_TYPE eAttackType = ATTACK_TYPE_UNKNOWN;
	ATTACK_ABILITY eAttackAbility = ATTACK_ABILITY_UNKNOWN;
	DAMAGE_TYPE eDamageType = DAMAGE_TYPE_UNKNOWN;
	WEAPON_TYPE eWeaponType = WEAPON_TYPE_UNKNOWN;
	ACTION_TYPE eActionType = ACTION_TYPE_UNKNOWN;


	void clear()
	{
		nDamage = 0;
		nAttackDistance = 1;
		eAttackType = ATTACK_TYPE_UNKNOWN;
		eAttackAbility = ATTACK_ABILITY_UNKNOWN;
		eDamageType = DAMAGE_TYPE_UNKNOWN;
		eWeaponType = WEAPON_TYPE_UNKNOWN;
		eActionType = ACTION_TYPE_UNKNOWN;
	}
};


struct TurnContext
{
	TurnContext()
	{
		clear();
	}

	bool bUsedAction;
	bool bUsedBonusAction;
	bool bUsedReaction;

	bool bHasAttackedThisTurn;
	bool bIsRaging;

	int nAttacksMade;      // for Extra Attack or multiattack
	int nBonusDamage;      // rage bonus, divine smite, sneak attack etc.

	// Optional: store last action result (hit/miss/damage)
	int nLastHitRoll;
	int nLastDamageDealt;

	// Optional: action state machine
	ACTION_TYPE eCurrentAction;

	void clear()
	{
		bUsedAction = false;
		bUsedBonusAction = false;
		bUsedReaction = false;

		bHasAttackedThisTurn = false;
		bIsRaging = false;

		nAttacksMade = 0;
		nBonusDamage = 0;

		nLastHitRoll = -1;
		nLastDamageDealt = 0;

		eCurrentAction = ACTION_TYPE_UNKNOWN;
	}

};




enum POINT_TYPE
{
	POINT_TYPE_ENTRANCE,
	POINT_TYPE_CENTER,
	POINT_TYPE_DARK_CORNER,
	POINT_TYPE_BACK_WALL,
	POINT_TYPE_DEBRIS,
	POINT_TYPE_GENERIC,

	POINT_TYPE_LAST
};


struct LevelProgression
{
	size_t exp_requirement;
	int proficiency_bonus;
};


struct LOCATION
{
	LOCATION(AREA_NAME eAreaName = AREA_NAME_NONE, uint32_t nRoomIndex = 0, uint32_t nPointIndex = 0) :
		m_eAreaName(eAreaName), m_nRoomIndex(nRoomIndex), m_nPointIndex(nPointIndex)
	{
		if (!IsValidLocation()) 
		{
			throw("Invalid LOCATION created");
		}
	}

	void clear()
	{
		m_eAreaName = AREA_NAME_NONE;
		m_nRoomIndex = 0;
		m_nPointIndex = 0;
	}

	bool IsValidLocation() const
	{
		return (m_nRoomIndex < G_MAX_AREA_SIZE) && (m_nPointIndex < G_MAX_ROOM_SIZE);
	}

	AREA_NAME m_eAreaName;
	uint32_t m_nRoomIndex;   // Index between 0-99 that specifies the room
	uint32_t m_nPointIndex;  // Index between 0-2499 that specifies a point in the room

	// Equality operator
	bool operator==(const LOCATION& other) const
	{
		return m_eAreaName == other.m_eAreaName &&
			m_nRoomIndex == other.m_nRoomIndex &&
			m_nPointIndex == other.m_nPointIndex;
	}
};

// Hash for std::unordered_map
namespace std
{
	template<>
	struct hash<LOCATION>
	{
		std::size_t operator()(const LOCATION& loc) const noexcept
		{
			std::size_t h1 = std::hash<int>()(static_cast<int>(loc.m_eAreaName));
			std::size_t h2 = std::hash<uint32_t>()(loc.m_nRoomIndex);
			std::size_t h3 = std::hash<uint32_t>()(loc.m_nPointIndex);

			// Combine hashes (simple but effective)
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}


struct OutParameters
{
	OutParameters() = default;

	std::vector<std::string> messages;
	LOCATION newLocation;
	ATTACK_INFO sAttackInfo;
	TurnContext sTurnContext;

	void Clear()
	{
		messages.clear();
		newLocation.clear();
		sAttackInfo.clear();
		sTurnContext.clear();
	}
};














/****************************************************
			===== HELPER FUNCTIONS =====
*****************************************************/

/* ********************************* Helper *********************************
Helper is a general utility class with useful static functions
*/
class Helper
{
public:

	// Getter for rage bonus damage per level
	static int GetRageBonusForLevel(int level)
	{
		// Levels are 1–20
		if (level < 1 || level > 20)
			return 0;

		return m_vRageBonusPerLevel[level - 1];
	}


	// Convert a point index to x,y cords
	static void ConvertPointIndexToXYCord(uint32_t nPointIndex, int& OutX, int& OutY)
	{
		OutX = (nPointIndex % G_ROOM_LENGTH);
		OutY = (nPointIndex / G_ROOM_LENGTH);
	}

	// Convert x,y cords to point index
	static void ConvertXYCordToPointIndex(int x, int y, uint32_t& nOutPointIndex)
	{
		nOutPointIndex = y * G_ROOM_LENGTH + x;
	}

	// Convert a room index to x,y cords
	static void ConvertRoomIndexToXYCord(uint32_t nRoomIndex, int& OutX, int& OutY)
	{
		OutX = (nRoomIndex % G_AREA_LENGTH);
		OutY = (nRoomIndex / G_AREA_LENGTH);
	}

	// Convert x,y cords to room index
	static void ConvertXYCordToRoomIndex(int x, int y, uint32_t& nOutRoomIndex)
	{
		nOutRoomIndex = y * G_AREA_LENGTH + x;
	}

	// Convert AreaName enum to a string
	static std::string GetAreaNameAsString(AREA_NAME area)
	{
		switch (area)
		{
		case AREA_NAME_WHISPERING_WOODS:     return "Whispering Woods";
		case AREA_NAME_STORMVALE_PLAINS:     return "Stormvale Plains";
		case AREA_NAME_PLAINS_OF_THUNDER:    return "Plains of Thunder";
		case AREA_NAME_ASHEN_DESERT:         return "Ashen Desert";
		case AREA_NAME_TUNDRA_WASTES:        return "Tundra Wastes";
		case AREA_NAME_EMBER_SANDS:           return "Ember Sands";
		case AREA_NAME_FROSTWIND_CLIFFS:     return "Frostwind Cliffs";
		case AREA_NAME_HIGH_PEAKS:           return "High Peaks";
		case AREA_NAME_ASHEN_PEAKS:          return "Ashen Peaks";
		case AREA_NAME_EMBERLIGHT_CITADEL:   return "Emberlight Citadel";
		case AREA_NAME_RUINS_OF_ARVANDOR:    return "Ruins of Arvandor";
		case AREA_NAME_ANCIENT_CATACOMBS:    return "Ancient Catacombs";
		case AREA_NAME_SWAMPLANDS_OF_MURK:   return "Swamplands of Murk";
		case AREA_NAME_CURSED_MARSHES:       return "Cursed Marshes";
		case AREA_NAME_COASTLINE_PORTS:      return "Coastline Ports";
		case AREA_NAME_FORGOTTEN_COAST:      return "Forgotten Coast";
		case AREA_NAME_FROSTBITE_FJORD:      return "Frostbite Fjord";
		case AREA_NAME_TWILIGHT_HOLLOW:      return "Twilight Hollow";
		case AREA_NAME_SHADOWLANDS:          return "Shadowlands";
		case AREA_NAME_NONE:                 return "Unknown Area";
		default:                             return "Invalid Area";
		}
	}

	///////////////////////// General Utility Functions ///////////////////////
	static bool HasDigit(const std::string& s)
	{
		return std::any_of(s.begin(), s.end(), ::isdigit);
	}

	static bool HasAlpha(const std::string& s)
	{
		return std::any_of(s.begin(), s.end(), ::isalpha);
	}

	static std::string TrimTrailingSpaces(const std::string& s)
	{
		std::string result = s;
		result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);
		return result;
	}

	static std::string PrintLocation(LOCATION sLocation)
	{
		return GetAreaNameAsString(sLocation.m_eAreaName) + "::" + std::to_string(sLocation.m_nRoomIndex) + "::"
			+ std::to_string(sLocation.m_nPointIndex);
	}

	static std::string MonsterTypeMessage(MONSTER_TYPE type)
	{
		switch (type)
		{
		case MONSTER_TYPE::MONSTER_TYPE_WOLF:
			return "A low growl echoes... a wolf circles in the shadows.";
		case MONSTER_TYPE::MONSTER_TYPE_BEAR:
			return "A heavy rumble shakes the room-there's a massive bear nearby.";
		case MONSTER_TYPE::MONSTER_TYPE_DEMON:
			return "The air distorts and burns... a demon watches you with glowing eyes.";
		case MONSTER_TYPE::MONSTER_TYPE_SPIDER:
			return "Faint skittering taps against the floor-an oversized spider is near.";
		case MONSTER_TYPE::MONSTER_TYPE_UNDEAD:
			return "A chilling moan drifts through the air... an undead creature approaches.";
		case MONSTER_TYPE::MONSTER_TYPE_GOBLIN:
			return "You hear mischievous chattering-there’s a goblin around.";
		case MONSTER_TYPE::MONSTER_TYPE_TROLL:
			return "Heavy footsteps shake the ground-a troll is lumbering nearby.";
		case MONSTER_TYPE::MONSTER_TYPE_DRAGON:
			return "Heat radiates through the room... you sense the presence of a dragon.";
		default:
			return "You sense a creature of unknown origin lurking nearby.";
		}
	}

	static std::string DescribeMonsterLocation(POINT_TYPE pointType)
	{
		switch (pointType)
		{
		case POINT_TYPE::POINT_TYPE_ENTRANCE:   return "lurking near the entrance";
		case POINT_TYPE::POINT_TYPE_CENTER:     return "standing in the center of the room";
		case POINT_TYPE::POINT_TYPE_DARK_CORNER: return "hiding in a dark corner";
		case POINT_TYPE::POINT_TYPE_BACK_WALL:   return "waiting near the back wall";
		case POINT_TYPE::POINT_TYPE_DEBRIS:     return "creeping beside a pile of debris";
		default:                    
			return "moving around restlessly";
		}
	}


	// Change a Monster's location in the world
	static bool PlayerChangeLocation(DIRECTION eDirection, int nNumOfSteps, LOCATION& sCurrentPlayerLocation)
	{
		int px, py;   // point XY
		int rx, ry;   // room XY
		//int ax, ay;   // area XY

		// Extract coordinates
		Helper::ConvertPointIndexToXYCord(sCurrentPlayerLocation.m_nPointIndex, px, py);
		Helper::ConvertRoomIndexToXYCord(sCurrentPlayerLocation.m_nRoomIndex, rx, ry);

		// Direction deltas
		int dx = 0, dy = 0;
		switch (eDirection)
		{
		case DIRECTION_NORTH: dy = -1; break;
		case DIRECTION_SOUTH: dy = 1; break;
		case DIRECTION_EAST:  dx = 1; break;
		case DIRECTION_WEST:  dx = -1; break;
		default:
			return false;
		}

		for (int step = 0; step < nNumOfSteps; ++step)
		{
			int new_px = px + dx;
			int new_py = py + dy;

			// ---------- POINT LEVEL MOVEMENT ----------
			if (new_px >= 0 && new_px < G_ROOM_LENGTH &&
				new_py >= 0 && new_py < G_ROOM_LENGTH)
			{
				// Move within room
				px = new_px;
				py = new_py;
			}
			else
			{
				// ---------- ROOM LEVEL MOVEMENT ----------
				int new_rx = rx;
				int new_ry = ry;

				if (new_px < 0)
					new_rx--;
				if (new_px >= G_ROOM_LENGTH)
					new_rx++;
				if (new_py < 0)
					new_ry--;
				if (new_py >= G_ROOM_LENGTH)
					new_ry++;

				// Check room boundaries
				if (new_rx < 0 || new_rx >= G_AREA_LENGTH ||
					new_ry < 0 || new_ry >= G_AREA_LENGTH)
				{
					return false;
				}

				// Commit room move
				rx = new_rx;
				ry = new_ry;

				// Wrap the point coordinates into the new room
				if (new_px < 0)
					px = G_ROOM_LENGTH - 1;
				else if (new_px >= G_ROOM_LENGTH)
					px = 0;
				else
					px = new_px;

				if (new_py < 0)
					py = G_ROOM_LENGTH - 1;
				else if (new_py >= G_ROOM_LENGTH)
					py = 0;
				else
					py = new_py;
			}
		}

		// Finally convert back to indices
		Helper::ConvertXYCordToPointIndex(px, py, sCurrentPlayerLocation.m_nPointIndex);
		Helper::ConvertXYCordToRoomIndex(rx, ry, sCurrentPlayerLocation.m_nRoomIndex);

		return true;
	}


	// Change a Monster's location in the world
	static bool MonsterChangeLocation(DIRECTION eDirection, int nNumOfSteps, const LOCATION& sPlayerLocation, LOCATION& eCurrentMonsterLocation)
	{
		int px, py;   // point XY 
		int rx, ry;   // room XY
		//int ax, ay;   // area XY

		LOCATION eCurrentLocationTemp = eCurrentMonsterLocation;

		// Extract coordinates
		Helper::ConvertPointIndexToXYCord(eCurrentLocationTemp.m_nPointIndex, px, py);
		Helper::ConvertRoomIndexToXYCord(eCurrentLocationTemp.m_nRoomIndex, rx, ry);

		// Direction deltas
		int dx = 0, dy = 0;
		switch (eDirection)
		{
		case DIRECTION_NORTH: dy = -1; break;
		case DIRECTION_SOUTH: dy = 1; break;
		case DIRECTION_EAST:  dx = 1; break;
		case DIRECTION_WEST:  dx = -1; break;
		default:
			return false;
		}

		for (int step = 0; step < nNumOfSteps; ++step)
		{
			int new_px = px + dx;
			int new_py = py + dy;

			// ---------- POINT LEVEL MOVEMENT ----------
			if (new_px >= 0 && new_px < G_ROOM_LENGTH &&
				new_py >= 0 && new_py < G_ROOM_LENGTH)
			{
				// Move within room
				px = new_px;
				py = new_py;
			}
			else
			{
				// ---------- ROOM LEVEL MOVEMENT ----------
				int new_rx = rx;
				int new_ry = ry;

				if (new_px < 0)
					new_rx--;
				if (new_px >= G_ROOM_LENGTH)
					new_rx++;
				if (new_py < 0)
					new_ry--;
				if (new_py >= G_ROOM_LENGTH)
					new_ry++;

				// Check room boundaries
				if (new_rx < 0 || new_rx >= G_AREA_LENGTH ||
					new_ry < 0 || new_ry >= G_AREA_LENGTH)
				{
					return false;
				}

				// Commit room move
				rx = new_rx;
				ry = new_ry;

				// Wrap the point coordinates into the new room
				if (new_px < 0)
					px = G_ROOM_LENGTH - 1;
				else if (new_px >= G_ROOM_LENGTH)
					px = 0;
				else
					px = new_px;

				if (new_py < 0)
					py = G_ROOM_LENGTH - 1;
				else if (new_py >= G_ROOM_LENGTH)
					py = 0;
				else
					py = new_py;
			}
		}

		// Finally convert back to indices
		Helper::ConvertXYCordToPointIndex(px, py, eCurrentLocationTemp.m_nPointIndex);
		Helper::ConvertXYCordToRoomIndex(rx, ry, eCurrentLocationTemp.m_nRoomIndex);

		// Monster cannot move into the same point the player is currently at
		if (sPlayerLocation == eCurrentLocationTemp)
			return false;

		// Monster cannot move to location that doesnt exist
		if (!eCurrentLocationTemp.IsValidLocation())
			return false;

		eCurrentMonsterLocation = eCurrentLocationTemp;
		return true;
	}

	static size_t ConvertLocationToWorldIndex(const LOCATION& sLocation)
	{
		// Number of points in a room
		constexpr size_t POINTS_PER_ROOM = G_ROOM_LENGTH * G_ROOM_LENGTH; // 10,000
		// Number of rooms in an area
		constexpr size_t ROOMS_PER_AREA = G_AREA_LENGTH * G_AREA_LENGTH;  // 100

		return static_cast<size_t>(sLocation.m_eAreaName) * ROOMS_PER_AREA * POINTS_PER_ROOM
			+ static_cast<size_t>(sLocation.m_nRoomIndex) * POINTS_PER_ROOM
			+ static_cast<size_t>(sLocation.m_nPointIndex);
	}

	static LOCATION ConvertWorldIndexToLocation(size_t nWorldIndex)
	{
		LOCATION loc{};

		constexpr size_t roomsPerArea = 1;
		constexpr size_t pointsPerRoom = G_ROOM_LENGTH * G_ROOM_LENGTH; // 100

		// Extract Area
		loc.m_eAreaName = static_cast<AREA_NAME>(
			nWorldIndex / (roomsPerArea * pointsPerRoom)
			);
		nWorldIndex %= (roomsPerArea * pointsPerRoom);

		// Extract Room (will always be 0 with roomsPerArea == 1)
		loc.m_nRoomIndex = static_cast<int>(
			nWorldIndex / pointsPerRoom
			);
		nWorldIndex %= pointsPerRoom;

		// Remaining is Point
		loc.m_nPointIndex = static_cast<int>(nWorldIndex);

		return loc;
	}

	static void RegisterMonsterTypes()
	{
		m_mapStrToMonsterType =
		{
			{ "wolf",   MONSTER_TYPE_WOLF   },
			{ "bear",   MONSTER_TYPE_BEAR   },
			{ "demon",  MONSTER_TYPE_DEMON  },
			{ "spider", MONSTER_TYPE_SPIDER },
			{ "undead", MONSTER_TYPE_UNDEAD },
			{ "goblin", MONSTER_TYPE_GOBLIN },
			{ "troll",  MONSTER_TYPE_TROLL  },
			{ "dragon", MONSTER_TYPE_DRAGON },
		};
	}

	static MONSTER_TYPE ConvertStringToMonsterType(const std::string strMonster)
	{
		if(m_mapStrToMonsterType.find(strMonster) == m_mapStrToMonsterType.end())
			return MONSTER_TYPE_NONE;

		return m_mapStrToMonsterType[strMonster];
	}

	static std::string ConvertMonsterTypeToString(MONSTER_TYPE type)
	{
		switch (type)
		{
		case MONSTER_TYPE::MONSTER_TYPE_WOLF:    return "Wolf";
		case MONSTER_TYPE::MONSTER_TYPE_BEAR:    return "Bear";
		case MONSTER_TYPE::MONSTER_TYPE_DEMON:   return "Demon";
		case MONSTER_TYPE::MONSTER_TYPE_SPIDER:  return "Spider";
		case MONSTER_TYPE::MONSTER_TYPE_UNDEAD:  return "Undead";
		case MONSTER_TYPE::MONSTER_TYPE_GOBLIN:  return "Goblin";
		case MONSTER_TYPE::MONSTER_TYPE_TROLL:   return "Troll";
		case MONSTER_TYPE::MONSTER_TYPE_DRAGON:  return "Dragon";
		case MONSTER_TYPE::MONSTER_TYPE_UNKNOWN: return "Unknown";
		default:                                 return "Unknown";
		}
	}

	static std::string DescribePackComposition(const std::vector<MONSTER_ATTRIBUTES>& pack)
	{
		std::unordered_map<MONSTER_TYPE, int> count;

		for (const auto& attr : pack)
			count[attr.m_eType]++;

		std::string result;

		for (auto& item : count)
		{
			result += std::to_string(item.second) + " " + ConvertMonsterTypeToString(item.first);
			if (item.second > 1) result += "s";
			result += ", ";
		}

		if (!result.empty())
			result.erase(result.size() - 2); // remove ", "

		return result;
	}

	static MONSTER_TYPE GetDominantMonsterType(const std::vector<MONSTER_ATTRIBUTES>& pack)
	{
		std::unordered_map<MONSTER_TYPE, int> freq;

		for (auto& a : pack)
			freq[a.m_eType]++;

		return std::max_element(
			freq.begin(), freq.end(),
			[](auto& a, auto& b) { return a.second < b.second; }
		)->first;
	}



	static int RollD(int size)
	{
		return std::rand() % size + 1;
	}


	static int CalculateDistance(const LOCATION& sLoc1, const LOCATION& sLoc2)
	{
		if (sLoc1.m_eAreaName != sLoc2.m_eAreaName ||
			sLoc1.m_nRoomIndex != sLoc2.m_nRoomIndex)
		{
			return INT32_MAX;
		}

		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		ConvertPointIndexToXYCord(sLoc1.m_nPointIndex, x1, y1);
		ConvertPointIndexToXYCord(sLoc2.m_nPointIndex, x2, y2);

		return static_cast<int>(std::sqrt(
			std::pow(x1 - x2, 2) +
			std::pow(y1 - y2, 2)
		));
	}


	static void MoveTowards(LOCATION& from, const LOCATION& to, int steps)
	{
		int fx, fy, tx, ty;
		ConvertPointIndexToXYCord(from.m_nPointIndex, fx, fy);
		ConvertPointIndexToXYCord(to.m_nPointIndex, tx, ty);

		while (steps-- > 0)
		{
			if (fx < tx) fx++;
			else if (fx > tx) fx--;

			if (fy < ty) fy++;
			else if (fy > ty) fy--;
		}

		ConvertXYCordToPointIndex(fx, fy, from.m_nPointIndex);
	}

	static int GetModifier(int nStat)
	{
		return static_cast<int>((nStat - 10) / 2);
	}


	static void CleanInputBuffer()
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}


	static std::string DamageTypeToString(DAMAGE_TYPE eDamageType)
	{
		switch (eDamageType)
		{
		case DAMAGE_TYPE_BLUDGEONING:   return "Bludgeoning";
		case DAMAGE_TYPE_PIERCING:      return "Piercing";
		case DAMAGE_TYPE_SLASHING:      return "Slashing";

		case DAMAGE_TYPE_FIRE:          return "Fire";
		case DAMAGE_TYPE_COLD:          return "Cold";
		case DAMAGE_TYPE_LIGHTNING:     return "Lightning";
		case DAMAGE_TYPE_THUNDER:       return "Thunder";
		case DAMAGE_TYPE_ACID:          return "Acid";
		case DAMAGE_TYPE_POISON:        return "Poison";

		case DAMAGE_TYPE_PSYCHIC:       return "Psychic";
		case DAMAGE_TYPE_RADIANT:       return "Radiant";
		case DAMAGE_TYPE_NECROTIC:      return "Necrotic";
		case DAMAGE_TYPE_FORCE:         return "Force";

		default:
			return "Unknown";
		}
	}

	static std::string AttackAbilityToString(ATTACK_ABILITY eAbility)
	{
		switch (eAbility)
		{
		case ATTACK_ABILITY_STRENGTH:       return "Strength";
		case ATTACK_ABILITY_DEXTERITY:      return "Dexterity";
		case ATTACK_ABILITY_CONSTITUTION:   return "Constitution";
		case ATTACK_ABILITY_INTELLIGENCE:   return "Intelligence";
		case ATTACK_ABILITY_WISDOM:         return "Wisdom";
		case ATTACK_ABILITY_CHARISMA:       return "Charisma";

		default:
			return "Unknown";
		}
	}

	// This produces a value from about –1.0 (very cowardly) → +1.0 (very aggressive).
	static double CalcMonsterAggressionToPlayerPowerFactor(double dPlayerPower, int nMonsterAggression)
	{
		// Normalize player power so extremely strong players don't break the formula
		double dNormalizedPlayerPower = dPlayerPower;

		// Aggression: 0 = cowardly, 50 = neutral, 100 = extremely aggressive
		double dAggressionNormalized = (nMonsterAggression - 50.0) / 50.0;
		// gives range [-1, +1]

		// If player is much stronger, monster becomes more cowardly.
		double dPowerFactor = -std::log10(dNormalizedPlayerPower / 10.0);

		// Clamp between -1 and +1
		dPowerFactor = ClampToRange(dPowerFactor, -1.0, 1.0);

		// Final factor
		double dFinal = ClampToRange(dAggressionNormalized + dPowerFactor, -1.0, 1.0);

		return dFinal;
	}


	static DIRECTION GetDirection(const LOCATION& playerLoc, const LOCATION& monsterLoc)
	{
		int player_x = 0, player_y = 0, monster_x = 0, monster_y = 0;
		ConvertPointIndexToXYCord(playerLoc.m_nPointIndex, player_x, player_y);
		ConvertPointIndexToXYCord(monsterLoc.m_nPointIndex, monster_x, monster_y);

		int dx = player_x - monster_x;
		int dy = player_y - monster_y;

		// Prefer axis with larger distance
		if (std::abs(dx) > std::abs(dy))
		{
			return (dx > 0) ? DIRECTION_EAST : DIRECTION_WEST;
		}
		else
		{
			return (dy > 0) ? DIRECTION_SOUTH : DIRECTION_NORTH;
		}
	}

	static DIRECTION GetOppositeDirection(const LOCATION& playerLoc, const LOCATION& monsterLoc)
	{
		int player_x = 0, player_y = 0, monster_x = 0, monster_y = 0;
		ConvertPointIndexToXYCord(playerLoc.m_nPointIndex, player_x, player_y);
		ConvertPointIndexToXYCord(monsterLoc.m_nPointIndex, monster_x, monster_y);

		int dx = player_x - monster_x;
		int dy = player_y - monster_y;

		if (std::abs(dx) > std::abs(dy))
		{
			// If player is east, run west
			return (dx > 0) ? DIRECTION_WEST : DIRECTION_EAST;
		}
		else
		{
			// If player is south, run north
			return (dy > 0) ? DIRECTION_NORTH : DIRECTION_SOUTH;
		}
	}

	static double ClampToRange(double value, double minVal, double maxVal)
	{
		if (value < minVal) return minVal;
		if (value > maxVal) return maxVal;
		return value;
	}

	static double GetLocationVisibilityModifier(POINT_VISIBILITY eVisibility)
	{
		switch (eVisibility)
		{
		case POINT_VISIBILITY_OPEN:
			return 1.0;

		case POINT_VISIBILITY_LIGHT_COVER:
			return 0.75;

		case POINT_VISIBILITY_HEAVY_COVER:
			return 0.50;

		case POINT_VISIBILITY_ELEVATED:
			return 0.65;

		default:
			// Safety fallback if enum is invalid
			return 1.0;
		}
	}

	static double CalculateVisionFactor(int nVisionSkill,
		double mOpponentVisibility, int nDistanceToOpponent, 
		POINT_VISIBILITY ePointVisibility)
	{
		if (nDistanceToOpponent >= INT32_MAX)
			return -1;

		// Convert integers to double
		double dVision = static_cast<double>(nVisionSkill);
		double mOpponentVis = static_cast<double>(mOpponentVisibility);

		// Terrain modifier lookup
		double dTerrainMod = 1.0;
		if (ePointVisibility >= 0 && ePointVisibility < POINT_VISIBILITY_LAST)
			dTerrainMod = GetLocationVisibilityModifier(ePointVisibility);

		// Core formula:
		// Better vision + more visible monster + open terrain = easier to see
		double dResult = (dVision / mOpponentVis) * dTerrainMod - nDistanceToOpponent / 100;

		return dResult;
	}


public:
	static std::unordered_map<std::string, MONSTER_TYPE> m_mapStrToMonsterType;

private:
	static const std::vector<int> m_vRageBonusPerLevel;


};




