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


void Monster::Damage(ATTACK_INFO& eOutAttackInfo)
{
	switch (eOutAttackInfo.eAttackType)
	{
	case ATTACK_TYPE_SHORT_RANGE:
		ShortRangeDamage(eOutAttackInfo);
		break;
	case ATTACK_TYPE_REACH_RANGE:
		ReachRangeDamage(eOutAttackInfo);
		break;
	case ATTACK_TYPE_LONG_RANGE:
		LongRangeDamage(eOutAttackInfo);
		break;
	default:
		break;
	}

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

int Monster::GetMovementRollThreshold() const
{
	return m_sMonsterAttributes.m_nMovementRollThreshold;
}

int Monster::GetSpeed() const
{
	return m_sMonsterAttributes.m_sStats.m_nSpeed;
}

int Monster::SetSpeed(int nSpeed)
{
	return m_sMonsterAttributes.m_sStats.m_nSpeed = nSpeed;
}

int Monster::ResetMovement()
{
	return SetSpeed(0);
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

void Monster::TakeDamage(ATTACK_INFO& eOutAttackInfo)
{
	if (eOutAttackInfo.nDamage <= 0)
		return;

	std::cout << Helper::ConvertMonsterTypeToString(m_sMonsterAttributes.m_eType) + " has taken " + std::to_string(eOutAttackInfo.nDamage) + " points of " + Helper::DamageTypeToString(eOutAttackInfo.eDamageType) + " Damage\n";
	m_sMonsterAttributes.m_sStats.m_nHealth -= eOutAttackInfo.nDamage;
}

int Monster::GetHealth() const
{
	return m_sMonsterAttributes.m_sStats.m_nHealth;
}

bool Monster::GetIsInRangeToAttack() const
{
	//if (Helper::Distance(nPlayerLocationInCombatGrid, m_eMonsterLocation) == 1)
	//	return true;
	//else
	//	return false;

	return true;
}

bool Monster::GetHasLongRangeAttack() const
{
	return m_sMonsterAttributes.m_bHasLongRangeAttack;
}

int Monster::GetAC() const
{
	return m_sMonsterAttributes.m_sStats.m_nArmorClass;
}

int Monster::GetConstitutionModifier() const
{
	return Helper::GetModifier(m_sMonsterAttributes.m_sStats.m_nConstitution);
}

/* ********************************* Wolf ********************************* */

Wolf::Wolf(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation):
	Monster(sMonsterAttributes, sMonsterLocation)
{
	m_sMonsterAttributes.m_bHasLongRangeAttack = false;

	m_sMonsterAttributes.m_sStats.m_nArmorClass = 12;
	m_sMonsterAttributes.m_sStats.m_nHealth = 11;
	m_sMonsterAttributes.m_sStats.m_nSpeed = 4;

	m_sMonsterAttributes.m_sStats.m_nStrength = 14;
	m_sMonsterAttributes.m_sStats.m_nDextirity = 15;
	m_sMonsterAttributes.m_sStats.m_nConstitution = 12;
	m_sMonsterAttributes.m_sStats.m_nIntelligence = 3;
	m_sMonsterAttributes.m_sStats.m_nWisdom = 12;
	m_sMonsterAttributes.m_sStats.m_nCharisma = 6;

	m_sMonsterAttributes.m_sSkills.m_nPerception = 5;
	m_sMonsterAttributes.m_sSkills.m_nStealth = 4;

	m_sMonsterAttributes.m_nExp = 50;
}

void Wolf::ShortRangeDamage(ATTACK_INFO& eOutAttackInfo)
{
	Bite(eOutAttackInfo);
}

void Wolf::Bite(ATTACK_INFO& eOutAttackInfo)
{
	eOutAttackInfo.eAttackAbility = ATTACK_ABILITY_STRENGTH;
	eOutAttackInfo.eDamageType = DAMAGE_TYPE_PIERCING;
	eOutAttackInfo.eAttackType = ATTACK_TYPE_SHORT_RANGE;
	eOutAttackInfo.eWeaponType = WEAPON_TYPE_NONE;
	eOutAttackInfo.eActionType = ACTION_TYPE_ACTION;

	eOutAttackInfo.nDamage = Helper::RollD(6) + Helper::GetModifier(m_sMonsterAttributes.m_sStats.m_nStrength);
	std::cout << "Wolf has used Bite for " + std::to_string(eOutAttackInfo.nDamage) + " points of " + Helper::DamageTypeToString(eOutAttackInfo.eDamageType) + " Damage!\n";
}




/* ********************************* ObjectManager ********************************* */

ObjectManager::ObjectManager()
	:m_pSpawnSystem(std::make_unique<SpawnSystem>())
{
}

void ObjectManager::CreateMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)
{
	m_mapMonsterLocator[Helper::ConvertLocationToWorldIndex(sMonsterLocation)] = 
		m_pSpawnSystem->m_monsterFactory.CreateInstance(sMonsterAttributes.m_eType, sMonsterAttributes, sMonsterLocation);

	////std::cout << "Inserting monster at: area=" << sMonsterLocation.m_eAreaName
	////	<< " room=" << sMonsterLocation.m_nRoomIndex
	////	<< " point=" << sMonsterLocation.m_nPointIndex << std::endl;
	////size_t idx = Helper::ConvertLocationToWorldIndex(sMonsterLocation);
	////std::cout << "World index: " << idx << std::endl;
}

void ObjectManager::CreatePackOfMonster(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation, std::vector<MONSTER_ATTRIBUTES> vPackMonstersAttributes)
{
	// Create the main monster
	CreateMonster(sMonsterAttributes, sMonsterLocation);

	auto& monster = m_mapMonsterLocator[Helper::ConvertLocationToWorldIndex(sMonsterLocation)];
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
	/*
	Add another data strucuter that keeps track of all the existing monsters. 
	m_mapMonsterLocator is basically the size of the entire world, which is too much to iterate over every run
	*/


	// Store pending movements (fromIndex -> toIndex)
	std::vector<std::pair<size_t, size_t>> vecPendingMoves;

	for (const auto& pair : m_mapMonsterLocator)
	{
		const auto& monster = pair.second;
		if (!monster)
			continue;

		// Roll to check if the monster moves this turn
		int nMovementRoll = (std::rand() % monster->GetMovementRollThreshold()) + 1;
		if (nMovementRoll != 1)
			continue;

		// Roll random direction
		DIRECTION eNewDirection = static_cast<DIRECTION>(std::rand() % DIRECTION_LAST);
		int nNumOfSteps = 1;

		LOCATION oldLocation = monster->GetLocation();
		LOCATION newLocation(oldLocation);

		if (!Helper::ChangeLocation(eNewDirection, nNumOfSteps, newLocation))
			continue;

		size_t newWorldIndex = Helper::ConvertLocationToWorldIndex(newLocation);

		// Check if target location is occupied
		const auto& MonsterAtNewLocation = m_mapMonsterLocator[newWorldIndex];
		if (MonsterAtNewLocation)
			continue;

		size_t oldWorldIndex = pair.first;
		vecPendingMoves.emplace_back(oldWorldIndex, newWorldIndex);
	}

	// Apply movements
	for (const auto& move : vecPendingMoves)
	{
		size_t fromIndex = move.first;
		size_t toIndex = move.second;

		auto it = m_mapMonsterLocator.find(fromIndex);
		if (it == m_mapMonsterLocator.end())
			continue;

		auto monster = std::move(it->second);
		m_mapMonsterLocator.erase(it);

		monster->SetLocation(
			Helper::ConvertWorldIndexToLocation(toIndex)
		);

		m_mapMonsterLocator[toIndex] = std::move(monster);
	}
}


//std::unique_ptr<Monster>* ObjectManager::GetMonstersInPoint(LOCATION eLocation)
//{
//	auto vMonsters = m_mapMonsterLocator.find(Helper::ConvertLocationToWorldIndex(eLocation));
//	if (vMonsters == m_mapMonsterLocator.end())
//		return nullptr;
//
//	return &vMonsters->second;
//}


Monster* ObjectManager::GetMonsterInPoint(const LOCATION& loc)
{
	//std::cout << "Looking up monster at: area=" << loc.m_eAreaName
	//	<< " room=" << loc.m_nRoomIndex
	//	<< " point=" << loc.m_nPointIndex << std::endl;
	//size_t idx = Helper::ConvertLocationToWorldIndex(loc);
	//std::cout << "World index: " << idx << std::endl;

	auto it = m_mapMonsterLocator.find(Helper::ConvertLocationToWorldIndex(loc));
	return (it != m_mapMonsterLocator.end()) ? it->second.get() : nullptr;
}

bool ObjectManager::RemoveMonster(const LOCATION& loc)
{
	size_t nWorldIdx = Helper::ConvertLocationToWorldIndex(loc);
	auto iter = m_mapMonsterLocator.find(nWorldIdx);
	if (iter == m_mapMonsterLocator.end())
		return false;

	m_mapMonsterLocator.erase(iter);
	return true;
}

/* ********************************* SpawnSystem ********************************* */
SpawnSystem::SpawnSystem()
{
	m_monsterFactory.RegisterCreator(MONSTER_TYPE_WOLF,
		[this](MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)
		{
			return CreateWolf(sMonsterAttributes, sMonsterLocation);
		}
	);
}

std::unique_ptr<Wolf> SpawnSystem::CreateWolf(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)
{
	return std::make_unique<Wolf>(sMonsterAttributes, sMonsterLocation);

}


/* ********************************* Weapon ********************************* */

Weapon::Weapon(WEAPON_TYPE eWeaponType, int nRangeInSquares, bool bIsTwoHanded) :
	m_eWeaponType(eWeaponType), m_nRangeInSquares(nRangeInSquares), m_bIsTwoHanded(bIsTwoHanded)
{
}


/* ********************************* ShortSword ********************************* */
ShortSword::ShortSword(WEAPON_TYPE eWeaponType, int nRangeInSquares):
	Weapon(eWeaponType, nRangeInSquares)
{
}

WEAPON_TYPE ShortSword::GetWeaponType() const
{
	return m_eWeaponType;
}

void ShortSword::Damage(ATTACK_INFO& eOutAttackInfo)
{
	eOutAttackInfo.eAttackType = ATTACK_TYPE_SHORT_RANGE;
	eOutAttackInfo.eAttackAbility = ATTACK_ABILITY_STRENGTH;
	eOutAttackInfo.eWeaponType = WEAPON_TYPE_SIMPLE_MELEE;
	eOutAttackInfo.eDamageType = DAMAGE_TYPE_PIERCING;
	eOutAttackInfo.eActionType = ACTION_TYPE_ACTION;
	eOutAttackInfo.nDamage = Helper::RollD(6);
}


/* ********************************* Greataxe ********************************* */
Greataxe::Greataxe(WEAPON_TYPE eWeaponType, int nRangeInSquares) :
	Weapon(eWeaponType, nRangeInSquares)
{
}

WEAPON_TYPE Greataxe::GetWeaponType() const
{
	return m_eWeaponType;
}

void Greataxe::Damage(ATTACK_INFO& eOutAttackInfo)
{
	eOutAttackInfo.eAttackType = ATTACK_TYPE_SHORT_RANGE;
	eOutAttackInfo.eAttackAbility = ATTACK_ABILITY_STRENGTH;
	eOutAttackInfo.eWeaponType = WEAPON_TYPE_MARTIAL_MELEE;
	eOutAttackInfo.eDamageType = DAMAGE_TYPE_SLASHING;
	eOutAttackInfo.eActionType = ACTION_TYPE_ACTION;
	eOutAttackInfo.nDamage = Helper::RollD(12);
}
