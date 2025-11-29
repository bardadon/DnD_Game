#ifndef __DNDGAME__
#define __DNDGAME__

#include <string>
#include <memory>
#include <functional>
#include <algorithm>

#include "Map.hpp"
#include "Helper.hpp"
#include "Trie.hpp"
#include "GameObject.hpp"


class System;


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

	// Getters/Setters
	void SetFaction(FACTION eFaction);
	FACTION GetFaction() const;
	std::string GetFactionAsString() const;
	void SetCurrentLocation(LOCATION sCurrentLocation);
	LOCATION GetCurrentLocation() const;

private:
	std::string m_strPlayerName;
	int m_PlayerAge;
	std::unique_ptr<Class> m_PlayerClass;
	FACTION m_eFaction;
	LOCATION m_sCurrentLocation;


}; // Player



/*
class SpawnSystem

	factory for each of the major objets in the game, monsters, npcs and objects

	bool SpawnMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation);
		- m_pMonsterFactory(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)

		



		// For now every 5 steps there will be a monster
		if (pointIndex % 5 == 0)
		{
			LOCATION sMonsterLocation(m_eAreaName, m_nRoomIndex, pointIndex);
			MONSTER_ATTRIBUTES sMonsterAttributes; // Randomize some attributes based on area, room index(especiially after dividing the areas into rooms with themes such as entrance, middle of forest etc
			//SpawnSystem->SpawnMonster(sMonsterAttributes, sMonsterLocation)
		}

		
		
		*/



/* ********************************* CommandDispacher *********************************
CommandDispacher is basically a map between a command(string) and a function that processes this command.

*/
class CommandDispacher
{
public:
	CommandDispacher(System& system, Trie& commandLookUpTrie);
	void RegisterCommands();

	ERROR_CODE ExecuteCommand(const std::string& strCommand);
	

	ERROR_CODE Look(const std::string& strCommand);
	ERROR_CODE Walk(const std::string& strCommand, DIRECTION eDirection);
	ERROR_CODE StartCombat(const std::string& strCommand);


private:
	System& m_system;
	Trie& m_commandLookUpTrie;
	using UserCommand = std::string;
	std::unordered_map<UserCommand, std::function<ERROR_CODE(const UserCommand&)>> m_mapCommandFunctions;
	
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

	// Room related functions
	bool GetCurrentRoomDescription(std::string& strOutRoomDescription) const;

	// Player related functions
	bool PlayerChangeLocation(DIRECTION eDirection, int nNumOfSteps);

	std::string GetDynamicDescriptionForMonsters(LOCATION sCurrentLocation);
	LOCATION GetPlayerLocation() const;
	std::vector<std::unique_ptr<Monster>>* GetMonstersInPoint(LOCATION eLocation);
	/*
	Create all generic monsters and npcs(and objects ?) in the world.
	Probably will be used mainly for debugging, but maybe I'll keep it
	just as a starting population mechanism for the world
	*/
	void PopulateWorld(); // Populate world with a generic setting of Monsters and NPCs

	bool BeginCombat(const std::vector<Monster*>& vMonstersInCombat);

private:
	std::unique_ptr<Player> m_pPlayer;
	std::unique_ptr<Map> m_pMap;
	std::unique_ptr<CommandDispacher> m_pCommandDispacher;
	std::unique_ptr<Trie> m_pCommandLookupTrie;
	std::unique_ptr<ObjectManager> m_pObjectManager;
	size_t m_nGameTime; // variable to keep track of the game time. Each command a user sends is equal to 1 game unit
}; // System







#endif // !__DNDGAME__