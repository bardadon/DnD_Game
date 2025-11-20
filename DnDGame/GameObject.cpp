#include <iostream>

#include "GameObject.hpp"

int GameObject::m_nLastObjectId = 0;

GameObject::GameObject():
	m_nObjectId(++m_nLastObjectId)
{
}



/* ********************************* Monster ********************************* */
Monster::Monster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation) :
	GameObject(),
	m_sMonsterAttributes(sMonsterAttributes), m_eMonsterLocation(sMonsterLocation)
{

}


void Monster::Attack()
{
	std::cout << "Attack\n";
}

int Monster::GetObjectId() const
{
	return m_nObjectId;
}



/* ********************************* ObjectManager ********************************* */

ObjectManager::ObjectManager()
	:m_pSpawnSystem(std::make_unique<SpawnSystem>())
{
}

void ObjectManager::CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)
{
	m_mapObjectLocator[sMonsterLocation].emplace_back(m_pSpawnSystem->CreateMonster(sMonsterAttributes, sMonsterLocation));

	m_vMonsters.push_back(static_cast<Monster*>(m_mapObjectLocator[sMonsterLocation].back().get()));
}

void ObjectManager::PopulateWorld()
{
	MONSTER_ATTRIBUTES sMonsterAttributes;
	LOCATION sMonsterLocation;
	CreateMonster(sMonsterAttributes, sMonsterLocation);
}






SpawnSystem::SpawnSystem()
{
	m_monsterFactory.RegisterCreator(INSTANCE_TYPE_MONSTER,
		[this](MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation) 
		{ 
			return CreateMonster(sMonsterAttributes, sMonsterLocation);
		} 
	);



}



std::unique_ptr<GameObject> SpawnSystem::CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)
{
	return std::make_unique<Monster>(sMonsterAttributes, sMonsterLocation);
}