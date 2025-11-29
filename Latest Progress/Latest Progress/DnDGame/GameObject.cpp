#include <iostream>
#include <random>

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

MONSTER_TYPE Monster::GetMonsterType() const
{
	return m_sMonsterAttributes.m_eType;
}

int Monster::GetMovementFactor() const
{
	return m_sMonsterAttributes.m_nMovementFactor;
}

std::vector<MONSTER_ATTRIBUTES>* Monster::GetPackAttributes()
{
	return &m_vPackMonstersAttributes;
}

bool Monster::IsPartOfAPack() const
{
	return !m_vPackMonstersAttributes.empty();
}

LOCATION Monster::GetLocation() const
{
	return m_eMonsterLocation;
}

void Monster::SetLocation(LOCATION sMonsterLocation)
{
	m_eMonsterLocation = sMonsterLocation;
}

void Monster::InsertMonsterToPack(MONSTER_ATTRIBUTES sMonsterAttr)
{
	m_vPackMonstersAttributes.push_back(sMonsterAttr);
}

void Monster::SetVisibilityThreshold(int nVisibilityThreshold)
{
	m_sMonsterAttributes.m_nVisibilityThreshold = nVisibilityThreshold;
}



/* ********************************* ObjectManager ********************************* */

ObjectManager::ObjectManager()
	:m_pSpawnSystem(std::make_unique<SpawnSystem>())
{
}

void ObjectManager::CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)
{
	m_mapMonsterLocator[Helper::ConvertLocationToWorldIndex(sMonsterLocation)].emplace_back(m_pSpawnSystem->CreateMonster(sMonsterAttributes, sMonsterLocation));
	std::cout << "ObjectManager::CreateMonster::Monster Created at Location:: " + Helper::PrintLocation(sMonsterLocation) + "\n";
}

void ObjectManager::CreatePackOfMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation, std::vector<MONSTER_ATTRIBUTES> vPackMonstersAttributes)
{
	// Create the main monster
	CreateMonster(sMonsterAttributes, sMonsterLocation);

	auto& monster = m_mapMonsterLocator[Helper::ConvertLocationToWorldIndex(sMonsterLocation)].back();
	for (auto monsterAttribute : vPackMonstersAttributes)
		monster->InsertMonsterToPack(monsterAttribute);

	std::cout << "ObjectManager::CreatePackOfMonster::Pack of " + std::to_string(vPackMonstersAttributes.size() + 1) + " Monsters Created at Location:: " + Helper::PrintLocation(sMonsterLocation) + "\n";
}



/////////////////////////////////////////////////////////////
// Each monster will have its own movement factor according to its type.

// Small and agile monsters will move after, lets say, 3 user commands
// Large and slow monvers will move after 5 commands

// The location for which they will move to is essentially random, but I will add
// some variable to the Point class that flags whether or not monsters can enter there.

// So for example, if a Point is the entrance to the Main Citadel, no Monster can enter there

// m_nMovementFactor a factor that determines how often an Object move randomly around the area
// A movement factor of 3. Means that at random, a monster will move 1 tile every 1-3 Game time units
/////////////////////////////////////////////////////////////
void ObjectManager::MoveAllMonsters()
{
	for (auto& pointVector : m_mapMonsterLocator)
	{
		for (int i = 0; i < pointVector.second.size(); ++i)
		{
			auto& monster = pointVector.second[i];


			// Roll to check if a monster is moving this turn. The higher the movement factor the lower chance they have to move
			int nMovementRoll = (std::rand() % monster->GetMovementFactor() + 1);
			if (nMovementRoll != 1)
				continue;
			
			// Roll to get a random direction
			DIRECTION eNewDirection = static_cast<DIRECTION>(std::rand() % DIRECTION_LAST);			
			int nNumOfSteps = 1;
			LOCATION ePrevLocation = monster->GetLocation();
			LOCATION eNewLocation(ePrevLocation);
			if (!Helper::ChangeLocation(eNewDirection, nNumOfSteps, eNewLocation))
				continue;

			monster->SetLocation(eNewLocation);

			// Update Monster Locator by pulling monster from its vector and inserting in the correct one
			m_mapMonsterLocator[Helper::ConvertLocationToWorldIndex(eNewLocation)].push_back(std::move(monster));

			// Remove monster from previous vector
			std::swap(pointVector.second[i], pointVector.second.back());
			pointVector.second.pop_back();
		}
	}
}

std::vector<std::unique_ptr<Monster>>* ObjectManager::GetMonstersInPoint(LOCATION eLocation)
{
	auto vMonsters = m_mapMonsterLocator.find(Helper::ConvertLocationToWorldIndex(eLocation));
	if (vMonsters == m_mapMonsterLocator.end())
		return nullptr;

	return &vMonsters->second;
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
