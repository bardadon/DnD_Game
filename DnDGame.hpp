#ifndef __DNDGAME__
#define __DNDGAME__

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

constexpr int G_MAX_ROOM_SIZE = 50 * 50; // Each room is a 50x50 grid of points
constexpr int G_MAX_AREA_SIZE = 10 * 10; // Each area is a 10x10 grid of rooms

class System;
class Area;
class Room;
class Monster;

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

struct LOCATION
{
	LOCATION(AREA_NAME eAreaName = AREA_NAME_NONE, uint32_t nRoomIndex = 0, uint32_t nPointIndex = 0) :
		m_eAreaName(eAreaName), m_nRoomIndex(nRoomIndex), m_nPointIndex(nPointIndex)
	{
	}

	bool IsValidLocation() const
	{
		return (m_eAreaName == AREA_NAME_NONE) && (m_nRoomIndex < G_MAX_AREA_SIZE) && (m_nPointIndex < G_MAX_ROOM_SIZE);
	}

	AREA_NAME m_eAreaName;
	uint32_t m_nRoomIndex;
	uint32_t m_nPointIndex;
};












/* ********************************* Class ********************************* 
Class holds the information and logic required to handle the differences between
different classes, such as Wizard, Cleric, Rogue, etc

Each class will have their own specialties, skills and so on.
Some will manifest when lvling up, some when interacting with npcs, when trying to perform
save checks, lockpicking and so on
*/

class Class
{
public:
	Class() = default;
	virtual ~Class() = default;

	virtual void PrintClassDescription() const = 0;


}; // Class

/* ********************************* Wizard ********************************* */
class Wizard : public Class
{
public:
	Wizard() = default;
	~Wizard() = default;

	void PrintClassDescription() const;

};


/* ********************************* Fighter ********************************* */
class Fighter : public Class
{
public:
	Fighter() = default;
	~Fighter() = default;
	void PrintClassDescription() const;

};




/* ********************************* Player *********************************
Player class in charge of keeping track of player information and stats
player location, inventory, party members
Perform actions such as combat actions or interaction with the system such as sending user commands such as(look, inspect, get, kill, etc)
*/

class Player
{
public:
	Player();
	~Player() = default;
	void BeginCharacterCreation();
	void SetFaction(FACTION eFaction);
	FACTION GetFaction() const;
	std::string GetFactionAsString() const;
	bool SetCurrentLocation(LOCATION sCurrentLocation);
	bool GetCurrentLocation(LOCATION& sOutCurrentLocation) const;
private:
	std::string m_strPlayerName;
	int m_PlayerAge;
	std::unique_ptr<Class> m_PlayerClass;
	FACTION m_eFaction;
	LOCATION m_sCurrentLocation;


}; // Player



/* ********************************* Map *********************************
Map class can generate Areas
	- Each Area is consisted of multiple Rooms
	- Each Room is a matrix that is filled with npcs, monsters and so on

The entire world map is a vector of areas
Each area is an NxM grid of rooms
And each room is an NxM grid of points

We dont need to load the entire world map and store it in memory constantly, only when needed

A player location is noted by:
	- Area index in Map::m_vAreas which leads to a certain area
	- cord (x, y) which leads to a room in area
	- cord (x, y) which leads to a point in room


*/
class Map
{
public:
	Map();
	virtual ~Map() = default;

	void GenerateArea(AREA_NAME eAreaName);
	bool GetRoom(LOCATION sRoomLocation, Room* pOutCurrentRoom) const;
private:
	std::unordered_map<AREA_NAME, std::unique_ptr<Area>> m_mapAreas;





}; // Map



/* ********************************* Area *********************************
Area class can geneerate Rooms
	- Each area has its own Biome, difficulty lvl and so on
	- Area is a grid of 10x10 rooms
*/
class Area
{
public:
	Area(AREA_NAME eAreaName = AREA_NAME_NONE);
	 ~Area() = default;

	void GenerateRoom();
	bool GetRoom(int nRoomIndex, Room* pOutCurrentRoom) const;
private:
	AREA_NAME m_eAreaName;

	/*
	Some data structure to hold all rooms
	*/
	std::vector<std::unique_ptr<Room>> m_vRooms; // 10x10 grid of Rooms

}; // Area

/* ********************************* Room *********************************
Room is a grid of 10x10 points
Room keeps track of the npcs, monsters and objects in the room
It is also in charge of moving them every N time units

*/
class Room
{
public:
	Room(AREA_NAME eAreaName, int nRoomIndex, const std::string& strRoomDescription = "room");

	std::string GetRoomDescription() const;

private:
	std::string m_strRoomDescription;
	AREA_NAME m_eAreaName;
	int m_nRoomIndex;
	std::vector<int> m_vPoints; // 50x50 grid of points
	std::vector<std::unique_ptr<Monster>> m_vMonsters;
	/*
	vector of Monsters
	vector of npcs
	vector of Objects
	*/
};

/* ********************************* Point *********************************
A point is the smallest object in the world map. It is about 20 square meters.
It holds information such as, has the player visited before
*/
//class Point : public Room
//{
//
//};


/* ********************************* Monster *********************************
Moveable and fightable npcs such as wolves, bears, demons and so on
*/
class Monster
{
public:
	Monster(LOCATION eCurrentLocation);

	void Attack();

private:
	LOCATION m_eCurrentLocation;
};


/* ********************************* CommandDispacher *********************************
CommandDispacher is basically a map between a command(string) and a function that processes this command.

*/
class CommandDispacher
{
public:
	CommandDispacher(System& pSystem);
	void RegisterCommands();

	bool ExecuteCommand(const std::string& strCommand);
	bool Look(const std::string& strCommand);

private:
	System& m_pSystem;
	using UserCommand = std::string;
	std::unordered_map<UserCommand, std::function<bool(const UserCommand&)>> m_mapCommandFunctions;
};


/* ********************************* System *********************************
System class is charge of running the Game loop.
It will recieve the commands from player and respond accordingly
It will keep a Lore Class that will assist with the handling of interactions with the world
such as npc conversions handling, conversiations history, quests history,
lore history about landmarks in rooms, lore about npcs, general world lore etc

*/
class System
{
public:
	System();
	void GameLoop();
	void LogIn();
	void CreateNewJourney();

	
	bool GetCurrentRoomDescription(std::string& strOutRoomDescription) const;
private:
	std::unique_ptr<Player> m_pPlayer;
	std::unique_ptr<Map> m_pMap;
	std::unique_ptr<CommandDispacher> m_pCommandDispacher;

}; // System


#endif // !__DNDGAME__