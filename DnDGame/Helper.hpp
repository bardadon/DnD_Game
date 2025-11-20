#ifndef __HELPER__
#define __HELPER__

#include <string>
#include <algorithm>

constexpr int G_ROOM_LENGTH = 50;
constexpr int G_AREA_LENGTH = 10;

constexpr int G_MAX_ROOM_SIZE = 50 * 50; // Each room is a 50x50 grid of points
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


struct LOCATION
{
	LOCATION(AREA_NAME eAreaName = AREA_NAME_NONE, uint32_t nRoomIndex = 0, uint32_t nPointIndex = 0) :
		m_eAreaName(eAreaName), m_nRoomIndex(nRoomIndex), m_nPointIndex(nPointIndex)
	{
	}

	bool IsValidLocation() const
	{
		return (m_eAreaName != AREA_NAME_NONE) && (m_nRoomIndex < G_MAX_AREA_SIZE) && (m_nPointIndex < G_MAX_ROOM_SIZE);
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
};



#endif // !__HELPER__
