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
class Player;
class CommandDispacher;

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
	virtual std::vector<ClassFeature>* GetLevelFeatures(int nPlayerLevel) = 0;

	virtual void PrintClassDescription() const = 0;
	virtual int RollHitPointDie() const = 0;
	virtual void ApplyFeature(Player* pPlayer, ClassFeature& feature, CommandDispacher* pDispatcher) = 0;
protected:
	std::vector<ClassFeature> m_vLevelFeatures[G_MAX_CLASS_LEVEL];

}; // Class

/* ********************************* Wizard ********************************* */
//class Wizard : public Class
//{
//public:
//	Wizard() = default;
//	~Wizard() = default;
//
//	void PrintClassDescription() const;
//
//};
//
//
///* ********************************* Fighter ********************************* */
//class Fighter : public Class
//{
//public:
//	Fighter() = default;
//	~Fighter() = default;
//	void PrintClassDescription() const;
//
//};

/* ********************************* Barbarian ********************************* */
class Barbarian : public Class 
{
public:
	Barbarian();
	~Barbarian() = default;
	void PrintClassDescription() const;
	int RollHitPointDie() const;
	void ApplyFeature(Player* pPlayer, ClassFeature& feature, CommandDispacher* pDispatcher);
	std::vector<ClassFeature>* GetLevelFeatures(int nPlayerLevel);

private:
	void Range();
};



/* ********************************* Equipment *********************************
Contains all the information and actions needed to handle both Equipped weapons and armor
and inventory

*/

class Equipment
{
public:
	Equipment();
	void SetMainHandWeapon(std::unique_ptr<Weapon> pWeapon);

	void MainWeaponAttack(ATTACK_INFO& eOutAttackInfo);

private:
	size_t m_nGold;
	std::vector<GameObject*> m_vInventory;
	std::unique_ptr<Weapon> m_pMainHandWeapon;
	std::unique_ptr<Weapon> m_pOffHandWeapon;

	/*
	Armor* Helmet,
	Armor* Bracelet,
	Armor* armor
 	etc...
	
	*/

};





/* ********************************* Player *********************************
Player class in charge of keeping track of player information and stats
player location, inventory, party members
Perform actions such as combat actions or interaction with the system such as sending user commands such as(look, inspect, get, kill, etc)
*/

class Player
{
public:
	Player(CommandDispacher* pDispatcher = nullptr);
	Player(PLAYER_ATTRIBUTES m_ePlayerAttributes, CommandDispacher* pDispatcher = nullptr);
	~Player() = default;
	void BeginCharacterCreation();

	// Getters/Setters
	void SetFaction(FACTION eFaction);
	FACTION GetFaction() const;
	std::string GetFactionAsString() const;
	void SetCurrentLocation(LOCATION sCurrentLocation);
	LOCATION GetCurrentLocation() const;
	std::vector<Monster*> GetVisibleMonsters() const;

	void TakeDamage(ATTACK_INFO& eOutAttackInfo);
	int GetHealth() const;
	int GetLevel() const;
	void SetVisibileMonsters(const std::vector<Monster*>& vVisibleMonsters);
	void SetDefaultBarbarianEquipment();
	void GainExp(int nExpFromMonster);
	void SetResistance(DAMAGE_TYPE eDamageType, bool bVal);
	void SetBonusDamage(ATTACK_ABILITY eAttackAbility, int nDamage);
	int GetBonusDamage(ATTACK_ABILITY eAttackAbility) const;
	
	bool GetIsPlayerInCombat() const;
	void SetIsPlayerInCombat(bool bVal);
	void MainHandAttack(ATTACK_INFO& eOutAttackInfo);

private:
	void PopulateLevelProgression();
	void LevelUp();


private:
	std::unique_ptr<Equipment> m_pEquipment;

	PLAYER_ATTRIBUTES m_ePlayerAttributes;
	std::string m_strPlayerName;
	int m_PlayerAge;
	std::unique_ptr<Class> m_PlayerClass;
	FACTION m_eFaction;
	LOCATION m_sCurrentLocation;

	std::vector<Monster*> m_vVisibleMonsters;
	std::vector<LevelProgression> m_vLevelTable;
	std::vector<bool> m_vResistances; // List of damage types(piercing, slashing, etc...) a player is resistant to
	std::vector<int> m_vBonuses; // List of damage abilities(str, dex, int, etc...) a player has a bonus to
	CommandDispacher* m_pDispatcher;
	bool m_bIsInCombat;
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


	CommandDispacher(System& system, Trie& commandLookUpTrie);
	void RegisterCommands();
	void RegisterCommand(const std::string& name,
		std::function<ERROR_CODE(const UserCommand&, OutParameters*)> func);
	ERROR_CODE ExecuteCommand(const std::string& strCommand, OutParameters* sOutParams = nullptr);
	

	ERROR_CODE Look(const std::string& strCommand);
	ERROR_CODE Walk(const std::string& strCommand, DIRECTION eDirection);
	ERROR_CODE StartCombat(const std::string& strCommand);
	ERROR_CODE MainHandAttack(const std::string& strCommand, OutParameters* sOutParams);
	ERROR_CODE EndTurn(const std::string& strCommand, OutParameters* sOutParams);
	ERROR_CODE Rage(const std::string& strCommand, OutParameters* sOutParams);

private:
	System& m_system;
	Trie& m_commandLookUpTrie;
	std::unordered_map<UserCommand, std::function<ERROR_CODE(const UserCommand&, OutParameters* sOutParams) >> m_mapCommandFunctions;
	
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
	bool PlayerChangeLocation(DIRECTION eDirection, int nNumOfSteps);

	std::string GetDynamicDescriptionForMonsters(LOCATION sCurrentLocation);
	LOCATION GetPlayerLocation() const;
	std::vector<Monster*> GetVisibleMonsters() const;
	bool BeginCombat(Monster* pMonsterInCombat);
	void MainHandAttack(ATTACK_INFO& eOutAttackInfo);
	void SetResistance(DAMAGE_TYPE eDamageType, bool bVal);
	void SetBonusDamage(ATTACK_ABILITY eAttackAbility, int nDamage);
	int GetPlayerLevel() const;
	bool GetIsPlayerInCombat() const;
	void SetIsPlayerInCombat(bool bVal);
private:
	void PopulateWorld(); // Populate world with a generic setting of Monsters and NPCs
	void PopulateArea(AREA_NAME eAreaName);
	void RollAndPopulateRandomMonster(const LOCATION& sMonsterLocation);
	void HandleMonsterTurn(Monster* pMonsterInCombat);
	void HandlePlayerTurn(Monster* pMonsterInCombat, TurnContext& sTurnContext);
	bool HandleMonsterLoss(Monster* pMonsterInCombat);
	void HandleTurnContext(TurnContext& sPlayerTurnContext);
	void PopulatePlayerVisibleMonsters();
	void UnRage(TurnContext& sPlayerTurnContext);

private:
	std::unique_ptr<Player> m_pPlayer;
	std::unique_ptr<Map> m_pMap;
	std::unique_ptr<CommandDispacher> m_pCommandDispacher;
	std::unique_ptr<Trie> m_pCommandLookupTrie;
	std::unique_ptr<ObjectManager> m_pObjectManager;
	size_t m_nGameTime; // variable to keep track of the game time. Each command a user sends is equal to 1 game unit

}; // System







#endif // !__DNDGAME__