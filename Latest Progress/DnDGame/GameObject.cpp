#include <iostream>

#include "GameObject.hpp"

int GameObject::m_nLastObjectId = 0;

GameObject::GameObject(int nVisibilityThreshold):
	m_nObjectId(++m_nLastObjectId), m_nVisibilityThreshold(nVisibilityThreshold)
{
}

int GameObject::GetVisibilityThreshold() const
{
	return m_nVisibilityThreshold;
}



/* ********************************* Monster ********************************* */
Monster::Monster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation) :
	GameObject(sMonsterAttributes.m_nVisibilityThreshold),
	m_sMonsterAttributes(sMonsterAttributes), m_eMonsterLocation(sMonsterLocation)
{
	/////////////////////////////// TEMPORARY ////////////////////////////////////////
	m_sMonsterAttributes.m_eType = static_cast<MONSTER_TYPE>(m_nObjectId % MONSTER_TYPE_LAST);
	/////////////////////////////// TEMPORARY ////////////////////////////////////////
}


void Monster::Attack()
{
	std::cout << "Attack\n";
}

int Monster::GetObjectId() const
{
	return m_nObjectId;
}

MONSTER_ATTRIBUTES Monster::GetMonsterAttributes() const
{
	return m_sMonsterAttributes;
}



/* ********************************* ObjectManager ********************************* */

ObjectManager::ObjectManager()
	:m_pSpawnSystem(std::make_unique<SpawnSystem>())
{
}

void ObjectManager::CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)
{
	m_mapMonsterLocator[sMonsterLocation].emplace_back(m_pSpawnSystem->CreateMonster(sMonsterAttributes, sMonsterLocation));
//	m_vMonsters.push_back(static_cast<Monster*>(m_mapMonsterLocator[sMonsterLocation].back().get()));

	std::cout << "ObjectManager::CreateMonster::Monster Created at Location:: " + Helper::PrintLocation(sMonsterLocation) + "\n";
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



std::unique_ptr<Monster> SpawnSystem::CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)
{
	return std::make_unique<Monster>(sMonsterAttributes, sMonsterLocation);
}