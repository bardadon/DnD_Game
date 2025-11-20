#ifndef __COMMAND_GAME_OBJECTS_HPP__
#define __COMMAND_GAME_OBJECTS_HPP__

#include <unordered_map>
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
	GameObject();
	virtual ~GameObject() = default;

	virtual int GetObjectId() const = 0;

protected:
	static int m_nLastObjectId;
	int m_nObjectId;


}; // GameObject

/* ********************************* Monster *********************************
Moveable and fightable npcs such as wolves, bears, demons and so on
*/

struct MONSTER_ATTRIBUTES
{
	MONSTER_ATTRIBUTES(int nStrength = 10) :
		m_nStrength(nStrength)
	{
	}

	int m_nStrength;
	/*
	intelligence
	stealth
	dextiriy

	etcc

	*/
};


class Monster : public GameObject
{
public:
	Monster() = default;
	Monster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation);

	void Attack();
	int GetObjectId() const;

private:
	LOCATION m_eMonsterLocation;
	MONSTER_ATTRIBUTES m_sMonsterAttributes;
};





/* ********************************* ObjectManager *********************************
This class will be incharge of owning the objects in the game and keeping track of where they are.
Using Location to a point, it will be able to check in O(1) exactly what Monsters, NPCS and Objects are in the point

*/
class ObjectManager
{
public:
	ObjectManager();

	void CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation);

	/*
	Create all generic monsters and npcs(and objects ?) in the world.
	Probably will be used mainly for debugging, but maybe I'll keep it
	just as a starting population mechanism for the world
	*/
	void PopulateWorld();

private:
	std::unique_ptr<SpawnSystem> m_pSpawnSystem;

	// Locator for Game Objects. Based on a location we get a vector of all GameObjects in that point
	// Locator is the owner of all the objects in the game
	using ObjectsInPoint = std::vector<std::unique_ptr< GameObject>>;
	std::unordered_map<LOCATION, ObjectsInPoint> m_mapObjectLocator;


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
	std::unique_ptr<GameObject> CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation);

	/*
	Add more creators such as:
	MonsterCreatorForArea1
	MonsterCreatorBossMonsters
	etc...

	*/

private:
	Factory< INSTANCE_TYPE, GameObject, MONSTER_ATTRIBUTES, LOCATION> m_monsterFactory;
}; // SpawnSystem



#endif // !__COMMAND_GAME_OBJECTS_HPP__
