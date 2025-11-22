#ifndef __HELPER__
#define __HELPER__

#include <string>
#include <algorithm>
#include <exception>

constexpr int G_ROOM_LENGTH = 10;
constexpr int G_AREA_LENGTH = 10;

constexpr int G_MAX_ROOM_SIZE = 10 * 10; // Each room is a 10x10 grid of points
constexpr int G_MAX_AREA_SIZE = 10 * 10; // Each area is a 10x10 grid of rooms



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
	DIRECTION_WEST
};


enum MONSTER_TYPE
{
	MONSTER_TYPE_WOLF,
	MONSTER_TYPE_BEAR,
	MONSTER_TYPE_DEMON,
	MONSTER_TYPE_SPIDER,
	MONSTER_TYPE_UNDEAD,
	MONSTER_TYPE_GOBLIN,
	MONSTER_TYPE_TROLL,
	MONSTER_TYPE_DRAGON,
	MONSTER_TYPE_UNKNOWN,

	MONSTER_TYPE_LAST
};


struct MONSTER_ATTRIBUTES
{
	MONSTER_ATTRIBUTES(int nStrength = 10, int nVisibilityThreshold = 10, MONSTER_TYPE eType = MONSTER_TYPE_UNKNOWN) :
		m_nStrength(nStrength), m_nVisibilityThreshold(nVisibilityThreshold), m_eType(eType)
	{
	}

	int m_nStrength;
	/*
	intelligence
	stealth
	dextiriy

	etcc

	*/
	int m_nVisibilityThreshold;
	MONSTER_TYPE m_eType;
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

/* ********************************* Helper *********************************
Helper is a general utility class with useful static functions
*/
class Helper
{
public:

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

};



#endif // !__HELPER__
