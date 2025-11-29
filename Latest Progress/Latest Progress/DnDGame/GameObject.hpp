#ifndef __COMMAND_GAME_OBJECTS_HPP__
#define __COMMAND_GAME_OBJECTS_HPP__

#include <memory>
#include <functional>

#include "Helper.hpp"

class SpawnSystem;


/* ********************************* GameObject *********************************
This class is the parent virtual class that will store common data about objects in the game
The classes Monster, Object and NPC will inherit from GameObject

*/
class GameObject
{
public:
	GameObject(int nVisibilityThreshold);
	virtual ~GameObject() = default;

	virtual int GetObjectId() const = 0;
	virtual int GetVisibilityThreshold() const;

protected:
	static int m_nLastObjectId;
	int m_nObjectId;
	int m_nVisibilityThreshold; // A threshold that determinces whether a player can see the object



}; // GameObject

/* ********************************* Monster *********************************
Moveable and fightable npcs such as wolves, bears, demons and so on


Monster represent a single or a pack of monsters. If a monster is in a pack, the attributes of all other monsters 
will be stored in m_vPackMonstersAttributes
*/
class Monster : public GameObject
{
public:
	//Monster() = default;
	Monster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation);

	void Attack();
	int GetObjectId() const;
	MONSTER_ATTRIBUTES GetMonsterAttributes() const;
	MONSTER_TYPE GetMonsterType() const;
	int GetMovementFactor() const;
	std::vector<MONSTER_ATTRIBUTES>* GetPackAttributes();


	bool IsPartOfAPack() const;
	LOCATION GetLocation() const;
	void SetLocation(LOCATION sMonsterLocation);
	void SetVisibilityThreshold(int nVisibilityThreshold);
	void InsertMonsterToPack(MONSTER_ATTRIBUTES sMonsterAttr);

private:
	LOCATION m_eMonsterLocation;
	MONSTER_ATTRIBUTES m_sMonsterAttributes;
	std::vector<MONSTER_ATTRIBUTES> m_vPackMonstersAttributes; // If the monster is in a pack, this will store the attributes of all OTHER monsters in the pack
	
};























/* ********************************* ObjectManager *********************************
This class will be incharge of owning the objects in the game and keeping track of where they are.
Using Location to a point, it will be able to check in O(1) exactly what Monsters, NPCS and Objects are in the point


The main locator and owner for monsters will be a map, where each key in the map represents a point index in the world.
Lets call it worldIndex
And each value will be a pointer to a vector of monsters

Each Area has a size of 100 Rooms and each Room has a size of 100 points. So each Area is 100*100 = 100,000 points
So:
	- worldIndex = 0 means Area = 0, room index = 0, point index = 0 -----> points to a vector of monsters in the first point
	- worldIndex = 1 means Area = 0, room index = 0, point index = 1
	...
	- worldIndex = 100,000 means Area = 1, room index = 0, point index = 0

*/
class ObjectManager
{
	friend class System;

public:
	ObjectManager();

	void CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation);
	void CreatePackOfMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation, std::vector<MONSTER_ATTRIBUTES> vPackMonstersAttributes);

	void MoveAllMonsters(); // Move all monsters in the world

	std::vector<std::unique_ptr<Monster>>* GetMonstersInPoint(LOCATION eLocation);
private:
	std::unique_ptr<SpawnSystem> m_pSpawnSystem;

	using MonstersInPoint = std::vector<std::unique_ptr< Monster>>;
	using WorldIndex = size_t;
	std::unordered_map<WorldIndex, MonstersInPoint> m_mapMonsterLocator;


	// In addition we add vectors for all Monsters and all NPCS to allow for faster iteration
	std::vector<Monster*> m_vMonsters;
	//std::vector<NPC*> m_vNPC;

}; // ObjectManager




template <typename KEY, typename BASE, typename ... ARGS>
class Factory
{
public:
	Factory() = default;
	~Factory() = default;

	void RegisterCreator(KEY key, std::function<std::unique_ptr<BASE>(ARGS ...)> creator);
	std::unique_ptr<BASE> CreateInstance(KEY key, ARGS ... args);

private:
	std::unordered_map < KEY, std::function<std::unique_ptr<BASE>(ARGS ...)>> m_mapCreators;

}; // Factory


template<typename KEY, typename BASE, typename ...ARGS>
inline void Factory<KEY, BASE, ARGS...>::RegisterCreator(KEY key, std::function<std::unique_ptr<BASE>(ARGS...)> creator)
{
	m_mapCreators[key] = creator;
}

template<typename KEY, typename BASE, typename ...ARGS>
inline std::unique_ptr<BASE> Factory<KEY, BASE, ARGS...>::CreateInstance(KEY key, ARGS ...args)
{
	return m_mapCreators.at(key)(args ...);
}








/* **************************** SpawnSystem ********************************
SpawnSystem is a collection of factories used for spawnning any instances a player can see and interact with
throughout the game, such as Monsters, NPCS and Objects.

*/

enum INSTANCE_TYPE
{
	INSTANCE_TYPE_MONSTER = 0,
	INSTANCE_TYPE_NPCS,
	INSTANCE_TYPE_OBJECTS
};



class SpawnSystem
{
public:
	SpawnSystem();
	std::unique_ptr<Monster> CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation);
	/*
	Add a creator for a pack of monsters. we basically create a vector of N monsters or a linked list
	where each has a pointer to the next. But all of them have the same location

	Also it doesnt have to be a pack of the same monster. a demon, gargoyle and a mutated bat can be a pack


	*/


	/*
	Add more creators such as:
	MonsterCreatorForArea1
	MonsterCreatorBossMonsters
	etc...

	*/

private:
	Factory< INSTANCE_TYPE, Monster, MONSTER_ATTRIBUTES, LOCATION> m_monsterFactory;






}; // SpawnSystem



#endif // !__COMMAND_GAME_OBJECTS_HPP__
