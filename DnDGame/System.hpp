#ifndef __DNDGAME__
#define __DNDGAME__

#include <string>
#include <memory>
#include <unordered_map>
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
	bool SetCurrentLocation(LOCATION sCurrentLocation);
	bool GetCurrentLocation(LOCATION& sOutCurrentLocation) const;	
	bool PlayerChangeLocation(DIRECTION eDirection, int nNumOfSteps);

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

	bool ExecuteCommand(const std::string& strCommand);
	

	bool Look(const std::string& strCommand);
	bool Walk(const std::string& strCommand, DIRECTION eDirection);

	//bool WalkN(const std::string& strCommand, DIRECTION eDirection);

private:
	System& m_system;
	Trie& m_commandLookUpTrie;
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

	// Room related functions
	bool GetCurrentRoomDescription(std::string& strOutRoomDescription) const;

	// Player related functions
	bool PlayerChangeLocation(DIRECTION eDirection, int nNumOfSteps);
private:
	std::unique_ptr<Player> m_pPlayer;
	std::unique_ptr<Map> m_pMap;
	std::unique_ptr<CommandDispacher> m_pCommandDispacher;
	std::unique_ptr<Trie> m_pCommandLookupTrie;
	std::unique_ptr<ObjectManager> m_pObjectManager;
}; // System







#endif // !__DNDGAME__