#include <iostream>
#include <thread>
#include "System.hpp"

std::unordered_map<std::string, MONSTER_TYPE> Helper::m_mapStrToMonsterType;

const std::vector<int> Helper::m_vRageBonusPerLevel =
{
	2, // Level 1
	2, // Level 2
	2, // Level 3
	2, // Level 4
	2, // Level 5
	2, // Level 6
	2, // Level 7
	2, // Level 8
	3, // Level 9
	3, // Level 10
	3, // Level 11
	3, // Level 12
	3, // Level 13
	3, // Level 14
	3, // Level 15
	4, // Level 16
	4, // Level 17
	4, // Level 18
	4, // Level 19
	4  // Level 20
};

/* ********************************* Wizard ********************************* */
//void Wizard::PrintClassDescription() const
//{
//	std::cout << R"(
//A scholar of the arcane arts. 
//You wield powerful spells drawn from ancient tomes and forbidden knowledge. Fragile in body, limitless in potential.
//)";
//}
//
//
//
///* ********************************* Fighter ********************************* */
//void Fighter::PrintClassDescription() const
//{
//	std::cout << R"(
//A master of weapons and tactics. You rely on strength, endurance, and skill in battle. 
//From heavy armor to precise swordplay, you are the backbone of every adventuring party.
//)";
//}

Barbarian::Barbarian()
{
	// Level 1 features
	m_vLevelFeatures[1] = {
	{ FEATURE_TYPE_RAGE, 2 },            // 2 rages per day
	{ FEATURE_TYPE_UNARMORED_DEFENCE }   // (AC = 10 + DEX + CON)
	};

	// Level 2 features
	m_vLevelFeatures[2] = {
	{ FEATURE_TYPE_RECKLESS_ATTACK},    // Gain advantage when hitting, disadvantage when getting hit
	{ FEATURE_TYPE_DANGER_SENSE }		
	};
}

/* ********************************* Barbarian ********************************* */
void Barbarian::PrintClassDescription() const
{
	std::cout << R"(
A fierce warrior fueled by primal fury. You fight with raw strength, unbreakable toughness,
and relentless aggression. When rage takes over, you become a force of nature, striking harder,
enduring impossible wounds, and dominating the battlefield with sheer brutality.
)";
}

int Barbarian::RollHitPointDie() const
{
	return Helper::RollD(12);
}

void Barbarian::ApplyFeature(Player* pPlayer, ClassFeature& feature, CommandDispacher* pDispatcher)
{
	switch (feature.type)
	{

	/*
	Rage:
		- Damage Resistance. You have Resistance to Bludgeoning, Piercing, and Slashing damage. 
		- When making a strength attack, deal a bonus damage that increases as barbs gain levels
		- Advantage on strength checks and saving throws
		- No concentation and no spells
		- rage lasts until player doesnt make an attack or incapacitated
	*/
	case FEATURE_TYPE_RAGE:
	{
		pDispatcher->RegisterCommand("rage",
			[this, pDispatcher](const UserCommand& cmd, CommandDispacher::OutParameters* sOutParams)
			{
				return pDispatcher->Rage(cmd, sOutParams);
			}
		);
		break;
	}
	default:
		break;
	}


}

std::vector<ClassFeature>* Barbarian::GetLevelFeatures(int nPlayerLevel)
{
	return &(m_vLevelFeatures[nPlayerLevel]);
}


void Barbarian::Range()
{
	std::cout << "Rage\n";
}



/* ********************************* Equipment ********************************* */
Equipment::Equipment(): m_nGold(G_STARTING_GOLD)
{
}

void Equipment::SetMainHandWeapon(std::unique_ptr<Weapon> pWeapon)
{
	if (!pWeapon)
		return;

	m_pMainHandWeapon = std::move(pWeapon);
}

void Equipment::MainWeaponAttack(ATTACK_INFO& eOutAttackInfo)
{
	if (!m_pMainHandWeapon)
		return;

	m_pMainHandWeapon->Damage(eOutAttackInfo);
}





/* ********************************* Player ********************************* */
Player::Player(CommandDispacher* pDispatcher):
	m_pDispatcher(pDispatcher)
{
	m_vBonuses.resize(ATTACK_ABILITY_LAST, 0);

	for (int i = 0; i < DAMAGE_TYPE_LAST; ++i)
		m_vResistances.push_back(false);

	/*
	Begin char creation process
	*/
	BeginCharacterCreation();

	PopulateLevelProgression();

}

Player::Player(PLAYER_ATTRIBUTES m_ePlayerAttributes, CommandDispacher* pDispatcher):
	m_ePlayerAttributes(m_ePlayerAttributes), m_pDispatcher(pDispatcher)
{
	BeginCharacterCreation();
}

// Choose name, age, class
void Player::BeginCharacterCreation()
{
	int age, nClass;
	std::string name;

	std::cout << "-------------------------------------------------------\n";
	std::cout << "           CHARACTER CREATION - HERO AWAKENS\n";
	std::cout << "-------------------------------------------------------\n\n";

	// flush leftover input (important if previous input used >>)
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// ---- NAME INPUT ----
	std::cout << "Enter Your Name: ";
	std::getline(std::cin, name);
	m_strPlayerName = name;

	while (m_strPlayerName.empty() || Helper::HasDigit(m_strPlayerName))
	{
		std::cout << "Name cannot be empty or contain any numbers.\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		std::cout << "Enter Your Name: ";
		std::getline(std::cin, name);
		m_strPlayerName = name;
	}

	// ---- AGE INPUT ----
	std::cout << "Enter Your Age: ";

	while (true)
	{
		if (std::cin >> age && age > 0)
		{
			m_PlayerAge = age;
			break;
		}

		// bad input — reset and retry
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		std::cout << "Age must be a positive number.\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		std::cout << "Enter Your Age: ";
	}

	std::cout << R"(
Choose a Class:
1. Fighter
   HP: 30   Mana: 5
   Strength: 8   Dexterity: 6   Intelligence: 4   Constitution: 8   Perception: 5
   Notes: Durable melee specialist; excels in direct combat.

2. Wizard
   HP: 15   Mana: 25
   Strength: 3   Dexterity: 5   Intelligence: 10   Constitution: 4   Perception: 6
   Notes: Master of arcane spells; high intelligence, low HP.

3. Rogue
   HP: 20   Mana: 10
   Strength: 5   Dexterity: 9   Intelligence: 6   Constitution: 5   Perception: 9
   Notes: Stealth, traps, ambushes; excels in dexterity and perception.

4. Cleric
   HP: 25   Mana: 20
   Strength: 6   Dexterity: 5   Intelligence: 7   Constitution: 7   Perception: 6
   Notes: Healer and support; moderate melee and spellcasting.

5. Barbarian
   HP: 35   Mana: 0
   Strength: 9   Dexterity: 6   Intelligence: 3   Constitution: 9   Perception: 4
   Notes: Rage-fueled melee brute; unmatched physical power and durability.
)";

	std::cout << "\nEnter your choice (1-4): ";
	std::cin >> nClass;


	m_pEquipment = std::make_unique<Equipment>();

	switch (nClass)
	{
	//case 1: 
	//	m_PlayerClass = std::make_unique<Fighter>();
	//	break;
	//case 2: 
	//	m_PlayerClass = std::make_unique <Wizard>();
	//	break;
	//case 3: 
	//	m_PlayerClass = "Cleric"; 
	//	break;
	//case 4: 
	//	m_PlayerClass = "Rogue"; 
	//	break;
	case 5:
		m_PlayerClass = std::make_unique <Barbarian>();
		SetDefaultBarbarianEquipment(); // Set class specific Default equipment

		break;
	default: 
		break;
	}


	// Init Player

	// Apply lvl 1 features
	std::vector<ClassFeature>* vClassFeatures = m_PlayerClass->GetLevelFeatures(m_ePlayerAttributes.m_nLvl);
	for (auto& feature : *vClassFeatures)
		m_PlayerClass->ApplyFeature(this, feature, m_pDispatcher);
	
	// Set player not in combat
	m_bIsInCombat = false;


	std::cout << "\n-----------------------------------------------------\n";
	std::cout << "Welcome, " << m_strPlayerName << "!\n";
	std::cout << "Age: " << m_PlayerAge << "\n";
	std::cout << "-----------------------------------------------------\n";
}

void Player::SetFaction(FACTION eFaction)
{
	m_eFaction = eFaction;
}

FACTION Player::GetFaction() const
{
	return m_eFaction;
}

std::string Player::GetFactionAsString() const
{
	switch (m_eFaction)
	{
	case FACTION_EMBERLIGHT_COUNCIL:
		return "Emberlight Council";
		break;
	case FACTION_SHADOW_CULT:
		return "Shadow Cult";
		break;
	case FACTION_VERDANT_CIRCLE:
		return "Verdant Circle";
		break;
	case FACTION_SEABORN_GUILD:
		return "Seaborn Guild";
		break;
	case FACTION_NOMADIC_TRIBES:
		return "Nomadic Tribes";
		break;
	default:
		return "";
		break;
	}
}


//////////////////////////////////////////////////////////////////
// Get/Set for current player location
//////////////////////////////////////////////////////////////////
void Player::SetCurrentLocation(LOCATION sCurrentLocation)
{
	m_sCurrentLocation = sCurrentLocation;
}

LOCATION Player::GetCurrentLocation() const
{
	return m_sCurrentLocation;
}

std::vector<Monster*> Player::GetVisibleMonsters() const
{
	return m_vVisibleMonsters;
}

void Player::TakeDamage(ATTACK_INFO& eOutAttackInfo)
{
	// Check if player is resistant to attack
	for (const auto& resistance : m_vResistances)
	{
		if (resistance)
		{
			std::cout << "Player is resistant to " + Helper::DamageTypeToString(eOutAttackInfo.eDamageType) + " Damage and takes half\n";
			eOutAttackInfo.nDamage /= 2;
			break;
		}
	}

	m_ePlayerAttributes.m_sStats.m_nHealth -= eOutAttackInfo.nDamage;
	std::cout << C_RED << "Player has taken " + std::to_string(eOutAttackInfo.nDamage) + " points of " + Helper::DamageTypeToString(eOutAttackInfo.eDamageType) + " Damage\n";
}

int Player::GetHealth() const
{
	return m_ePlayerAttributes.m_sStats.m_nHealth;
}

int Player::GetLevel() const
{
	return m_ePlayerAttributes.m_nLvl;
}

void Player::SetVisibileMonsters(const std::vector<Monster*>& vVisibleMonsters)
{
	m_vVisibleMonsters = vVisibleMonsters;
}


/*
Default barbarian equipment includes:
	- GreatAxe
	- Armor to be included...
*/
void Player::SetDefaultBarbarianEquipment()
{
	if (!m_pEquipment)
		return;

	m_pEquipment->SetMainHandWeapon(std::make_unique<Greataxe>());

}


///////////////////////////////////////////////////////////////////////
// Populate player lvl requirements and Proficiency bonus
///////////////////////////////////////////////////////////////////////
void Player::PopulateLevelProgression()
{
	m_vLevelTable.reserve(20);

	m_vLevelTable.push_back({ 0,      2 }); // Level 1
	m_vLevelTable.push_back({ 300,    2 }); // Level 2
	m_vLevelTable.push_back({ 900,    2 }); // Level 3
	m_vLevelTable.push_back({ 2700,   2 }); // Level 4
	m_vLevelTable.push_back({ 6500,   3 }); // Level 5
	m_vLevelTable.push_back({ 14000,  3 }); // Level 6
	m_vLevelTable.push_back({ 23000,  3 }); // Level 7
	m_vLevelTable.push_back({ 34000,  3 }); // Level 8
	m_vLevelTable.push_back({ 48000,  4 }); // Level 9
	m_vLevelTable.push_back({ 64000,  4 }); // Level 10
	m_vLevelTable.push_back({ 85000,  4 }); // Level 11
	m_vLevelTable.push_back({ 100000, 4 }); // Level 12
	m_vLevelTable.push_back({ 120000, 5 }); // Level 13
	m_vLevelTable.push_back({ 140000, 5 }); // Level 14
	m_vLevelTable.push_back({ 165000, 5 }); // Level 15
	m_vLevelTable.push_back({ 195000, 5 }); // Level 16
	m_vLevelTable.push_back({ 225000, 6 }); // Level 17
	m_vLevelTable.push_back({ 265000, 6 }); // Level 18
	m_vLevelTable.push_back({ 305000, 6 }); // Level 19
	m_vLevelTable.push_back({ 355000, 6 }); // Level 20
}

///////////////////////////////////////////////////////////////////////
// Handle lvl up logic as follows:
//	- Roll for an increase in Hit points
//		- Each class rolls their hit die plus con modifier
//	- Adjust Proficiency Bonus
//	- Learn class features/feats/spells/ etc
///////////////////////////////////////////////////////////////////////
void Player::LevelUp()
{
	++m_ePlayerAttributes.m_nLvl;

	// Increase hit points
	int nIncreaseInHitPoints = m_PlayerClass->RollHitPointDie();
	m_ePlayerAttributes.m_sStats.m_nHealth += nIncreaseInHitPoints;
	
	// Update prof bonus
	m_ePlayerAttributes.m_ProfBonus = m_vLevelTable[m_ePlayerAttributes.m_nLvl].proficiency_bonus;

	// Check class features
	std::vector<ClassFeature>* vClassFeatures = m_PlayerClass->GetLevelFeatures(m_ePlayerAttributes.m_nLvl);
	for (auto& feature : *vClassFeatures)
		m_PlayerClass->ApplyFeature(this, feature, m_pDispatcher);
}



///////////////////////////////////////////////////////////////////////
// Handle increasing player exp as well as handling lvling up logic
// Gaining new class features, stat bonuses and so on
///////////////////////////////////////////////////////////////////////
void Player::GainExp(int nExpFromMonster)
{
	// Raise exp
	m_ePlayerAttributes.m_nExp += nExpFromMonster;

	// Grab lvling requirements
	if (m_ePlayerAttributes.m_nLvl < 20)
	{
		LevelProgression lvlProg = m_vLevelTable[m_ePlayerAttributes.m_nLvl + 1];
		if (lvlProg.exp_requirement <= m_ePlayerAttributes.m_nExp)
			LevelUp();
	}


}

void Player::SetResistance(DAMAGE_TYPE eDamageType, bool bVal)
{
	m_vResistances[eDamageType] = bVal;
}

void Player::SetBonusDamage(ATTACK_ABILITY eAttackAbility, int nDamage)
{
	m_vBonuses[eAttackAbility] = nDamage;
}

int Player::GetBonusDamage(ATTACK_ABILITY eAttackAbility) const
{
	return m_vBonuses[eAttackAbility];
}

bool Player::GetIsPlayerInCombat() const
{
	return m_bIsInCombat;
}

void Player::SetIsPlayerInCombat(bool bVal)
{
	m_bIsInCombat = bVal;
}

void Player::MainHandAttack(ATTACK_INFO& eOutAttackInfo)
{
	if (!m_pEquipment)
		return;

	// Grab attack info
	m_pEquipment->MainWeaponAttack(eOutAttackInfo);
	
	// Check if player has bonus damage
	for (int i = 0; i < m_vBonuses.size(); ++i)
	{
		if (m_vBonuses[i] > 0)
		{
			std::cout << "Player has " + std::to_string(GetBonusDamage((ATTACK_ABILITY)i)) + " bonus Damage to " + Helper::AttackAbilityToString((ATTACK_ABILITY)i) + " Types\n";
			eOutAttackInfo.nDamage += GetBonusDamage((ATTACK_ABILITY)i);
		}
	}

}

/* ********************************* System ********************************* */
/*
BEFORE GAME LOOP STARTS:

	- Begin log in, registartion process
	- Load information from disk to start running the game
			- When saving a game we serialize the Player, Party members, Map, areas and rooms
			to disk and load it all when we continue playing


*/
System::System():
	m_nGameTime(0)
{
	std::cout << R"(
WELCOME TO REALMS OF SHADOWFALL

Twilight settles over the shattered ruins as you awaken, the echo of forgotten magic humming in the air.
The world stirs with rising factions and ancient powers long unbound.
Your past is lost - but your destiny waits to be claimed.

Choose your path, adventurer:
1. Begin a New Journey
2. Load a Saved Adventure
3. View Lore & Legends
4. Exit the Realm
)";

	int nPlayerChoice;
	std::cin >> nPlayerChoice;
	switch (nPlayerChoice)
	{
	case 1:
		// Begin character creation process
		CreateNewJourney();
		break;
	case 2:
		LogIn();
		break;
	case 3:
		break;

	default:
		std::cout << "Fuck you :)\n";
		break;
	}

	
	Helper::RegisterMonsterTypes();

}


void System::CreateNewJourney()
{
	m_pCommandLookupTrie = std::make_unique<Trie>();
	m_pCommandDispacher = std::make_unique<CommandDispacher>(*this, *m_pCommandLookupTrie);
	m_pMap = std::make_unique<Map>();
	m_pObjectManager = std::make_unique<ObjectManager>();

	/////////////////////////// Temporary //////////////////////////////////
	// Generete a starting template of Monsters, NPCS and Objects


	/*
	Think about populating the world using multiple threads to make the process faster(and train on working with multiple threads
	for parallel processing)
	*/
	PopulateArea(AREA_NAME_EMBERLIGHT_CITADEL);
	/////////////////////////// Temporary //////////////////////////////////


	m_pPlayer = std::make_unique<Player>(m_pCommandDispacher.get()); // Start character creation process
	

	int choice = 0;

	std::cout <<
		"--------------------------------------------------------\n"
		"                CHOOSE YOUR FACTION ORIGIN              \n"
		"--------------------------------------------------------\n"
		"\n"
		"Before your journey begins, adventurer, you must declare\n"
		"the people who shaped your early life. Your faction\n"
		"determines your homeland, your allies, and the shadows\n"
		"that follow you...\n\n"

		"Select one of the Five Pillars of Eryndor:\n\n"

		"1. Emberlight Council\n"
		"   - History: Rebuilders of civilization after the Great Sundering.\n"
		"   - Goals: Reclaim their citadel, stabilize magic, restore order.\n"
		"   - Traits: Better defense against curses, faster recovery from injuries.\n\n"

		"2. Shadow Cult\n"
		"   - History: Born from forbidden magic and necromancy.\n"
		"   - Goals: Spread corruption, summon the Eternal Night.\n"
		"   - Traits: Bonus to stealth and ambush attacks, empowered shadow and necrotic spells, resistant to fear effects.\n\n"

		"3. Verdant Circle\n"
		"   - History: Ancient druids and rangers guarding the ley lines.\n"
		"   - Goals: Protect the forests and resist shadow corruption.\n"
		"   - Traits: Skilled in ranged combat and tracking, enhanced mobility in forests and wilderness, improved stealth in natural terrain.\n\n"

		"4. Seaborn Guild\n"
		"   - History: A powerful alliance of merchants, sailors, and pirates.\n"
		"   - Goals: Control maritime trade, explore arcane storms.\n"
		"   - Traits: Better navigation and mobility at sea, proficiency with ranged weapons, advantage in climbing, swimming, and traversing coastal terrain.\n\n"

		"5. Nomadic Tribes\n"
		"   - History: Ancient clans roaming plains, deserts, and tundra.\n"
		"   - Goals: Preserve migration routes and tribal autonomy.\n"
		"   - Traits: High mobility across plains and deserts, endurance in harsh conditions\n\n"


		"Enter the number of your chosen faction: ";

	std::cin >> choice;

	LOCATION sStartingLocation(AREA_NAME_NONE);
	int nRoomIndex = 0, nPointIndex = 0;

	// Assign faction & starting area
	switch (choice)
	{
	case 1:
		m_pPlayer->SetFaction(FACTION_EMBERLIGHT_COUNCIL);
		sStartingLocation = AREA_NAME_EMBERLIGHT_CITADEL;
		m_pPlayer->SetCurrentLocation(sStartingLocation);
		break;

	//case 2:
	//	m_pPlayer->SetFaction(SHADOW_CULT);
	//	m_pMap->SetStartingArea(3);   // Shadowlands
	//	break;

	//case 3:
	//	m_pPlayer->SetFaction(VERDANT_CIRCLE);
	//	m_pMap->SetStartingArea(0);   // Whispering Woods
	//	break;

	//case 4:
	//	m_pPlayer->SetFaction(SEABORN_GUILD);
	//	m_pMap->SetStartingArea(8);   // Coastline Ports
	//	break;

	//case 5:
	//	m_pPlayer->SetFaction(NOMADIC_TRIBES);
	//	m_pMap->SetStartingArea(6);   // Ashen Desert / Plains
	//	break;

	default:
		std::cout << "Invalid choice. Defaulting to Emberlight Council.\n";
		m_pPlayer->SetFaction(FACTION_EMBERLIGHT_COUNCIL);
		sStartingLocation = AREA_NAME_EMBERLIGHT_CITADEL;
		m_pPlayer->SetCurrentLocation(sStartingLocation);
		break;
	}

	std::cout << "\nYour journey with the " << m_pPlayer->GetFactionAsString()
		<< " begins now...\n\n";
}

bool System::GetCurrentRoomDescription(std::string& strOutRoomDescription) const
{
	// Get player location
	LOCATION sCurrentLocation = m_pPlayer->GetCurrentLocation();

	// Use Location to get Room
	Room* pOutCurrentRoom = m_pMap->GetRoom(sCurrentLocation.m_eAreaName, sCurrentLocation.m_nRoomIndex);
	if (!pOutCurrentRoom)
		return false;

	// Print room desc
	strOutRoomDescription = pOutCurrentRoom->GetRoomDescription();
	return true;

}

bool System::PlayerChangeLocation(DIRECTION eDirection, int nNumOfSteps)
{
	LOCATION sCurrentLocation = m_pPlayer->GetCurrentLocation();

	if (!Helper::ChangeLocation(eDirection, nNumOfSteps, sCurrentLocation))
	{
		std::cout << "You can't go that way.\n";
		return false;
	}
		
	m_pPlayer->SetCurrentLocation(sCurrentLocation);
	return true;
}



/*
When a character has entered a room, they should be able to see whether there are Monsters up to N tiles away in all directions

There will be number of factors contributing to N :
-Character perception skills
- Area biome(plains and deserts receive a bonues, Forests and mountains receive a penalty)
- Point Altitude
- Each point will have an altitude, if the altitude of the point is bigger than that of the Monster, thats a bonus, otherwise thats a penalty
- Also Depends on the type of animals.The size and Stealth of an animal is another factor
- Large animals will give the player a bonus
- Animals with stealth give the player a penalty

- The combination of all of these factors will determine the value of N for a specific animal.A different animal might have a different N


- Currently to make this simple, N factor will be a simple addition of all of these factors.Each one will contribute 1 or -1
*/
std::string System::GetDynamicDescriptionForMonsters(LOCATION sCurrentLocation)
{
	std::string strDynamicMsg;

	Room* pCurrentRoom = m_pMap->GetRoom(sCurrentLocation.m_eAreaName, sCurrentLocation.m_nRoomIndex);

	// Iterate over all points in the room
	for (int point = 0; point < pCurrentRoom->GetNumberOfPoints(); ++point)
	{
		// Grab all monsters in current point
		LOCATION pPointLocation(sCurrentLocation.m_eAreaName, sCurrentLocation.m_nRoomIndex, point);
		std::unique_ptr<Monster>& vMonsterInPoint =
			m_pObjectManager->m_mapMonsterLocator[Helper::ConvertLocationToWorldIndex(pPointLocation)];

		if (!vMonsterInPoint)
			continue;

		// Print dynamic desc for monster
		int nPlayerVisibilityFactor = 10; // CalculateVisibilityFactor(monster.get());
		int nMonsterVisibilityThreshold = vMonsterInPoint->GetVisibilityThreshold();

		if (nPlayerVisibilityFactor >= nMonsterVisibilityThreshold)
		{
			const MONSTER_TYPE type = vMonsterInPoint->GetMonsterAttributes().m_eType;
			const int strength = vMonsterInPoint->GetMonsterAttributes().m_sStats.m_nStrength;

			// --- Monster type message ---
			strDynamicMsg += Helper::MonsterTypeMessage(type);
			strDynamicMsg += " ";

			// --- Location message ---
			Point* pCurrentPoint = pCurrentRoom->GetPoint(point);
			strDynamicMsg += Helper::DescribeMonsterLocation(pCurrentPoint->GetPointType());
			strDynamicMsg += ". ";

			// --- Strength-based flavor ---
			if (strength > 25)
				strDynamicMsg += "It radiates overwhelming power.\n";
			else if (strength > 10)
				strDynamicMsg += "It looks dangerous.\n";
			else
				strDynamicMsg += "It seems relatively weak.\n";

			int x, y;
			Helper::ConvertPointIndexToXYCord(point, x, y);
			strDynamicMsg += "Point:[ " + std::to_string(x) + ", " + std::to_string(y) + " ]\n";

			// --- Monster Pack information
			if (vMonsterInPoint->IsPartOfAPack())
			{
				const std::vector<MONSTER_ATTRIBUTES>* pack = vMonsterInPoint->GetPackAttributes();

				// === PACK SIZE ===
				strDynamicMsg += "It moves with a pack of ";
				strDynamicMsg += std::to_string(pack->size());
				strDynamicMsg += ". ";

				// === PACK COMPOSITION ===
				strDynamicMsg += "The group consists of ";
				strDynamicMsg += Helper::DescribePackComposition(*pack);
				strDynamicMsg += ". ";

				// === AGGRESSION (AVERAGE) ===
				int totalAgg = 0;
				for (auto& m : *pack)
					totalAgg += m.m_nAggression;
				int avgAgg = totalAgg / (int)pack->size();

				if (avgAgg >= 8)
					strDynamicMsg += "The pack is extremely aggressive. ";
				else if (avgAgg >= 5)
					strDynamicMsg += "The pack looks tense and ready to defend itself. ";
				else
					strDynamicMsg += "The pack seems relatively calm. ";
				strDynamicMsg += ". ";
			}
		}
	
	}

	return strDynamicMsg;
}

LOCATION System::GetPlayerLocation() const
{
	return m_pPlayer->GetCurrentLocation();
}

//std::unique_ptr<Monster>* System::GetMonstersInPoint(LOCATION eLocation)
//{
//	return m_pObjectManager->GetMonstersInPoint(eLocation);
//}

std::vector<Monster*> System::GetVisibleMonsters() const
{
	return m_pPlayer->GetVisibleMonsters();
}

void System::LogIn()
{
	/*
	Grab from disk or create player
			

	Do the same for all the necessary information that needs to be taken from disk in order 
	to start playing. Lore, Map, etc


	- For now I'll create new classes each time we "log in"
	*/


}


void System::GameLoop()
{
	bool bIsPlaying = true;
	int XPoint = 0, YPoint = 0;
	ERROR_CODE eErr(ERROR_CODE_GET_NEXT_COMMAND);

	while (bIsPlaying)
	{
		// populate visible monsters to player
		PopulatePlayerVisibleMonsters();

		// Get current player location
		LOCATION sCurrentLocation = m_pPlayer->GetCurrentLocation();

		Helper::ConvertPointIndexToXYCord(sCurrentLocation.m_nPointIndex, XPoint, YPoint);

		std::cout << "\n[Location: " << Helper::GetAreaNameAsString(sCurrentLocation.m_eAreaName)
			<< " | RoomIdx: " << sCurrentLocation.m_nRoomIndex << " "
			<< " | PointIdx: " << sCurrentLocation.m_nPointIndex << " "
			<< " | Cords: [" << XPoint << "," << YPoint <<"]> ";


		// Grab command from user and process it
		std::string strUserCommand;
		std::getline(std::cin, strUserCommand);
		std::transform(strUserCommand.begin(), strUserCommand.end(), strUserCommand.begin(), ::tolower);
		eErr = m_pCommandDispacher->ExecuteCommand(strUserCommand);
		if (ERROR_CODE_CONTINUE_GAME_LOOP != eErr)
			continue;

		if (eErr == ERROR_CODE_FAILED)
			return;


		// Move all monsters in the area
		m_pObjectManager->MoveAllMonsters();






	}

	/*
	Save current progress
	*/
}


void System::RollAndPopulateRandomMonster(const LOCATION& sMonsterLocation)
{
	int rollForSingleMonster = std::rand() % 2000;
	int rollForPackMonster = std::rand() % 4000;

	if (rollForSingleMonster == 1)
	{
		MONSTER_ATTRIBUTES sAttr;
		sAttr.m_eType = MONSTER_TYPE_WOLF;
		m_pObjectManager->CreateMonster(sAttr, sMonsterLocation);

		if (sMonsterLocation.m_nPointIndex == 800)
		{
			int rollForSingleMonster = std::rand() % 2000;

		}
	}
}


///////////////////////////////////////////////
// Populate world with a generic setting of Monsters and NPCs
// 
// The rest of the Monsters, Npcs and Objects are filled manually according to world lore and quests
///////////////////////////////////////////////
void System::PopulateWorld()
{
	std::cout << "System::PopulateWorld()::Populating World\n";
	// For now, the generic setting is that every N steps there will be a generic monster
	for (int area = 0; area < AREA_NAME_LAST; ++area)
	{
		Area* pArea = m_pMap->GetArea((AREA_NAME)area);
		if (!pArea)
			continue;

		for (int room = 0; room < pArea->GetNumberOfRooms(); ++room)
		{
			Room* pRoom = pArea->GetRoom(room);
			if (!pRoom)
				continue;

			for (int point = 0; point < pRoom->GetNumberOfPoints(); ++point)
			{
				Point* pPoint = pRoom->GetPoint(point);
				if (!pPoint)
					continue;

				RollAndPopulateRandomMonster(pPoint->GetLocation());

				if (point == 800)
				{
					int rollForSingleMonster = std::rand() % 2000;
					auto pMonster = m_pObjectManager->GetMonsterInPoint(pPoint->GetLocation());

				}
			}
		}

	}
}

///////////////////////////////////////////////
// Populate Area with a generic setting of Monsters and NPCs
// 
// The rest of the Monsters, Npcs and Objects are filled manually according to world lore and quests
///////////////////////////////////////////////
void System::PopulateArea(AREA_NAME eAreaName)
{
	std::cout << "System::PopulateArea()::Populating World\n";
	Area* pArea = m_pMap->GetArea(eAreaName);
	if (!pArea)
		return;

	for (int room = 0; room < pArea->GetNumberOfRooms(); ++room)
	{
		Room* pRoom = pArea->GetRoom(room);
		if (!pRoom)
			continue;

		for (int point = 0; point < pRoom->GetNumberOfPoints(); ++point)
		{
			Point* pPoint = pRoom->GetPoint(point);
			if (!pPoint)
				continue;

			RollAndPopulateRandomMonster(pPoint->GetLocation());
			
			//MONSTER_ATTRIBUTES sAttr;
			//sAttr.m_eType = MONSTER_TYPE_WOLF;
			//m_pObjectManager->CreateMonster(sAttr, pPoint->GetLocation());
			//return;
		}
	}
}


void System::HandleMonsterTurn(Monster* pMonster)
{
	if (!pMonster || !m_pPlayer)
		return;

	auto name = Helper::ConvertMonsterTypeToString(pMonster->GetMonsterType());

	std::cout << C_RED << "---------------------------------------\n";
	std::cout << "Monster's Turn (" << name << ")\n";
	std::cout << "---------------------------------------\n" << C_RESET;

	std::this_thread::sleep_for(std::chrono::milliseconds(400));

	LOCATION monsterLoc = pMonster->GetLocation();
	LOCATION playerLoc = m_pPlayer->GetCurrentLocation();

	int distance = Helper::CalculateDistance(monsterLoc, playerLoc);
	int movement = pMonster->GetSpeed();

	std::cout << C_YELLOW << name << " is " << distance << " tiles away.\n";
	std::cout << "Movement speed: " << movement << "\n" << C_RESET;
	std::this_thread::sleep_for(std::chrono::milliseconds(350));

	ATTACK_INFO sOutAttackInfo;
	// ------------------------------------
	// LONG RANGE
	// ------------------------------------
	if (pMonster->GetHasLongRangeAttack())
	{
		int longRange = pMonster->GetRangeOfLongRangeAttack();

		std::cout << C_CYAN << name << " prepares a long-range attack (range "
			<< longRange << ").\n" << C_RESET;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		// Already in range
		if (distance <= longRange)
		{
			std::cout << C_RED << name << " fires immediately!\n" << C_RESET;
			sOutAttackInfo.eAttackType = ATTACK_TYPE_LONG_RANGE;
			pMonster->Damage(sOutAttackInfo);
			m_pPlayer->TakeDamage(sOutAttackInfo);
			return;
		}

		// Move + attack
		if (distance <= longRange + movement)
		{
			int stepsNeeded = distance - longRange;
			int stepsToMove = movement - stepsNeeded;

			std::cout << C_MAGENTA << name << " moves " << stepsToMove
				<< " tiles to get in range...\n" << C_RESET;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			Helper::MoveTowards(monsterLoc, playerLoc, stepsToMove);
			pMonster->SetLocation(monsterLoc);

			std::cout << C_RED << name << " unleashes a ranged attack!\n" << C_RESET;
			sOutAttackInfo.eAttackType = ATTACK_TYPE_LONG_RANGE;
			pMonster->Damage(sOutAttackInfo);
			m_pPlayer->TakeDamage(sOutAttackInfo);
			return;
		}

		// Move only
		std::cout << C_BLUE << name << " advances " << movement
			<< " tiles but cannot attack.\n" << C_RESET;

		Helper::MoveTowards(monsterLoc, playerLoc, movement);
		pMonster->SetLocation(monsterLoc);
		return;
	}

	// ------------------------------------
	// MELEE-ONLY
	// ------------------------------------
	int meleeRange = 1;

	if (distance <= meleeRange)
	{
		std::cout << C_RED << name << " strikes you in melee!\n" << C_RESET;
		sOutAttackInfo.eAttackType = ATTACK_TYPE_SHORT_RANGE;
		pMonster->Damage(sOutAttackInfo);
		m_pPlayer->TakeDamage(sOutAttackInfo);
		return;
	}

	if (distance <= movement + meleeRange)
	{
		int stepsNeeded = distance - meleeRange;

		std::cout << C_MAGENTA << name << " charges " << stepsNeeded
			<< " tiles!\n" << C_RESET;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		Helper::MoveTowards(monsterLoc, playerLoc, stepsNeeded);
		pMonster->SetLocation(monsterLoc);

		std::cout << C_RED << name << " attacks in melee!\n" << C_RESET;
		sOutAttackInfo.eAttackType = ATTACK_TYPE_SHORT_RANGE;
		pMonster->Damage(sOutAttackInfo);
		m_pPlayer->TakeDamage(sOutAttackInfo);
		return;
	}

	std::cout << C_BLUE << name << " moves " << movement
		<< " tiles but cannot reach you.\n" << C_RESET;

	Helper::MoveTowards(monsterLoc, playerLoc, movement);
	pMonster->SetLocation(monsterLoc);
}

///////////////////////////////////////////////
// After a monster is defeated, it is removed from the game.
// Player will gain exp and will recieve an appropriate prompt
///////////////////////////////////////////////
bool System::HandleMonsterLoss(Monster* pMonsterInCombat)
{
	if (!pMonsterInCombat)
		return false;

	m_pPlayer->GainExp(pMonsterInCombat->GetMonsterAttributes().m_nExp);

	std::cout << "\n"
		<< "The creature collapses in defeat.\n"
		<< "Its presence fades, leaving only silence behind.\n"
		<< "You gain " << pMonsterInCombat->GetMonsterAttributes().m_nExp
		<< " experience.\n";

	if (!m_pObjectManager->RemoveMonster(pMonsterInCombat->GetLocation()))
		return false;

	return true;
}

void System::HandlePlayerTurn(Monster* pMonsterInCombat, TurnContext& sTurnContext)
{
	if (!pMonsterInCombat || !m_pPlayer)
		return;

	auto enemyName = Helper::ConvertMonsterTypeToString(pMonsterInCombat->GetMonsterType());

	std::cout << C_GREEN << "=======================================\n";
	std::cout << "Player's Turn\n";
	std::cout << "Enemy: " << enemyName << "\n";
	std::cout << "=======================================\n" << C_RESET;

	std::this_thread::sleep_for(std::chrono::milliseconds(350));

	bool bUsedAction = false, bUsedBonusAction = false;
	bool bPlayersTurn = true;

	while (bPlayersTurn)
	{

		if (!bUsedAction)
		{
			std::cout << "You spot an opportunity to act. What do you do?\n";
		}
		else if (!bUsedBonusAction)
		{
			std::cout << "While the " + enemyName + " is readying its attack, you notice a moment for a brief action. What do you do?\n";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(350));
		CommandDispacher::OutParameters sOutParams;
		sOutParams.sTurnContext = sTurnContext;
		ERROR_CODE eErr(ERROR_CODE_GET_NEXT_COMMAND);
		while (eErr == ERROR_CODE_GET_NEXT_COMMAND)
		{
			std::string strUserCommand;
			std::getline(std::cin, strUserCommand);

			eErr = m_pCommandDispacher->ExecuteCommand(strUserCommand, &sOutParams);
			sTurnContext = sOutParams.sTurnContext;

			if (sOutParams.sAttackInfo.eActionType == ACTION_TYPE_ACTION && bUsedAction)
			{
				sOutParams.Clear();
				std::cout << "You dont have time for that. The " + enemyName + " is almost ready to strike!\n";
				eErr = ERROR_CODE_GET_NEXT_COMMAND;
				continue;
			}

			else if (sOutParams.sAttackInfo.eActionType == ACTION_TYPE_BONUS_ACTION && bUsedBonusAction)
			{
				sOutParams.Clear();
				std::cout << "You feel like its not necessary. You have time to prepare your attack!\n";
				eErr = ERROR_CODE_GET_NEXT_COMMAND;
				continue;
			}
			else if (sOutParams.sAttackInfo.eActionType == ACTION_TYPE_END_TURN)
			{
				std::cout << C_MAGENTA << "\nEnding turn...\n" << C_RESET;
				std::this_thread::sleep_for(std::chrono::seconds(2));
				return;
			}
			else if (sOutParams.sAttackInfo.eActionType != ACTION_TYPE_BONUS_ACTION && sOutParams.sAttackInfo.eActionType != ACTION_TYPE_ACTION)
			{
				sOutParams.Clear();
				eErr = ERROR_CODE_GET_NEXT_COMMAND;
				continue;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(350));
		}



		if (sOutParams.sAttackInfo.nDamage > 0)
		{
			std::cout << C_CYAN << "\nRolling Hit Roll...\n" << C_RESET;
			std::this_thread::sleep_for(std::chrono::seconds(2));

			int nHitRoll = Helper::RollD(20);
			std::cout << C_WHITE << "You rolled: " << nHitRoll << "\n" << C_RESET;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			if (nHitRoll > pMonsterInCombat->GetAC())
			{
				std::cout << C_GREEN << "Hit!\nRolling Damage...\n" << C_RESET;
				std::this_thread::sleep_for(std::chrono::seconds(1));

				std::cout << C_WHITE << "Damage Roll: "
					<< sOutParams.sAttackInfo.nDamage << "\n" << C_RESET;

				sOutParams.sAttackInfo.nDamage -= pMonsterInCombat->GetConstitutionModifier();
				pMonsterInCombat->TakeDamage(sOutParams.sAttackInfo);
			}
			else
			{
				std::cout << C_RED << "You miss the " << enemyName << "...\n" << C_RESET;
			}
		}

		if (sOutParams.sAttackInfo.eActionType == ACTION_TYPE_ACTION)
			bUsedAction = true;
		else if (sOutParams.sAttackInfo.eActionType == ACTION_TYPE_BONUS_ACTION)
			bUsedBonusAction = true;
	}
	
}




//void System::HandlePlayerTurn(Monster* pMonsterInCombat)
//{
//	if (!pMonsterInCombat || !m_pPlayer)
//		return;
//
//	auto enemyName = Helper::ConvertMonsterTypeToString(pMonsterInCombat->GetMonsterType());
//
//	std::cout << C_GREEN << "=======================================\n";
//	std::cout << "Player's Turn\n";
//	std::cout << "Enemy: " << enemyName << "\n";
//	std::cout << "=======================================\n" << C_RESET;
//
//	std::this_thread::sleep_for(std::chrono::milliseconds(350));
//
//	bool bUsedAction = false, bUsedBonusAction = false;
//	bool bPlayersTurn = true;
//
//	while (bPlayersTurn)
//	{
//		std::cout << C_YELLOW << "\nChoose your move:\n";
//		std::cout << "1. Action\n";
//		std::cout << "2. Bonus Action\n";
//		std::cout << "3. End Turn\n";
//		std::cout << "> " << C_RESET;
//
//		int nTypeOfAction = 0;
//		std::cin >> nTypeOfAction;
//
//		if (nTypeOfAction > 3 || nTypeOfAction < 1)
//		{
//			std::cout << C_RED << "Invalid input. Try again.\n" << C_RESET;
//			continue;
//		}
//
//		// ACTION
//		if (nTypeOfAction == 1)
//		{
//			if (bUsedAction)
//			{
//				std::cout << C_RED << "You already used your Action.\n" << C_RESET;
//				continue;
//			}
//
//			std::cout << C_CYAN << "Preparing Action...\n" << C_RESET;
//			std::this_thread::sleep_for(std::chrono::milliseconds(600));
//
//			CommandDispacher::OutParameters sOutParams;
//			ERROR_CODE eErr(ERROR_CODE_GET_NEXT_COMMAND);
//
//			Helper::CleanInputBuffer();
//			while (eErr == ERROR_CODE_GET_NEXT_COMMAND)
//			{
//				std::cout << C_WHITE << "Enter Action Command: " << C_RESET;
//				std::string strUserCommand;
//				std::getline(std::cin, strUserCommand);
//
//				std::string strActionCommand = "Action: " + strUserCommand;
//				std::transform(strActionCommand.begin(), strActionCommand.end(),
//					strActionCommand.begin(), ::tolower);
//
//				eErr = m_pCommandDispacher->ExecuteCommand(strActionCommand, &sOutParams);
//			}
//
//			if (sOutParams.sAttackInfo.nDamage > 0)
//			{
//				std::cout << C_CYAN << "\nRolling Hit Roll...\n" << C_RESET;
//				std::this_thread::sleep_for(std::chrono::seconds(2));
//
//				int nHitRoll = Helper::RollD(20);
//				std::cout << C_WHITE << "You rolled: " << nHitRoll << "\n" << C_RESET;
//				std::this_thread::sleep_for(std::chrono::milliseconds(500));
//
//				if (nHitRoll > pMonsterInCombat->GetAC())
//				{
//					std::cout << C_GREEN << "Hit!\nRolling Damage...\n" << C_RESET;
//					std::this_thread::sleep_for(std::chrono::seconds(1));
//
//					std::cout << C_WHITE << "Damage Roll: "
//						<< sOutParams.sAttackInfo.nDamage << "\n" << C_RESET;
//
//					sOutParams.sAttackInfo.nDamage -= pMonsterInCombat->GetConstitutionModifier();
//					pMonsterInCombat->TakeDamage(sOutParams.sAttackInfo);
//				}
//				else
//				{
//					std::cout << C_RED << "You miss the " << enemyName << "...\n" << C_RESET;
//				}
//			}
//
//			bUsedAction = true;
//		}
//		// BONUS ACTION
//		else if (nTypeOfAction == 2)
//		{
//			if (bUsedBonusAction)
//			{
//				std::cout << C_RED << "Bonus Action already used.\n" << C_RESET;
//				continue;
//			}
//
//			std::cout << C_CYAN << "Using Bonus Action...\n" << C_RESET;
//			std::this_thread::sleep_for(std::chrono::milliseconds(600));
//
//			bUsedBonusAction = true;
//		}
//		else
//		{
//			std::cout << C_BLUE << "You end your turn.\n" << C_RESET;
//			bPlayersTurn = false;
//		}
//	}
//
//	std::cout << C_MAGENTA << "\nEnding turn...\n" << C_RESET;
//	std::this_thread::sleep_for(std::chrono::seconds(2));
//}


void System::PopulatePlayerVisibleMonsters()
{
	std::string strDynamicMsg;
	LOCATION sCurrentLocation = m_pPlayer->GetCurrentLocation();
	Room* pCurrentRoom = m_pMap->GetRoom(sCurrentLocation.m_eAreaName, sCurrentLocation.m_nRoomIndex);
	std::vector<Monster*> vVisibleMonsters;

	// Iterate over all points in the room
	for (int point = 0; point < pCurrentRoom->GetNumberOfPoints(); ++point)
	{
		// Grab all monsters in current point
		LOCATION pointLocation = sCurrentLocation;
		pointLocation.m_nPointIndex = point;
		Monster* pMonsterInPoint = m_pObjectManager->GetMonsterInPoint(pointLocation);

		//std::unique_ptr<Monster>& pMonsterInPoint =
		//	m_pObjectManager->m_mapMonsterLocator[Helper::ConvertLocationToWorldIndex(sCurrentLocation)];

		if (!pMonsterInPoint)
			continue;

		//if (pMonsterInPoint) {
		//	LOCATION mloc = pMonsterInPoint->GetLocation();
		//	std::cout << "Monster's internal location: area=" << mloc.m_eAreaName
		//		<< " room=" << mloc.m_nRoomIndex
		//		<< " point=" << mloc.m_nPointIndex << std::endl;
		//}

		// Check if player can see monster
		int nPlayerVisibilityFactor = 10; // CalculateVisibilityFactor(monster.get());
		int nMonsterVisibilityThreshold = pMonsterInPoint->GetVisibilityThreshold();

		if (nPlayerVisibilityFactor >= nMonsterVisibilityThreshold)
			vVisibleMonsters.push_back(pMonsterInPoint);
	}

	m_pPlayer->SetVisibileMonsters(vVisibleMonsters);
}

void System::UnRage(TurnContext& sPlayerTurnContext)
{
	std::cout << "Clarity is starting to flow back into you're mind. You have calmed down.\n";
	// Damage Resistance
	SetResistance(DAMAGE_TYPE_BLUDGEONING, false);
	SetResistance(DAMAGE_TYPE_PIERCING, false);
	SetResistance(DAMAGE_TYPE_SLASHING, false);

	// Strength attacks bonus
	SetBonusDamage(ATTACK_ABILITY_STRENGTH, 0);

	sPlayerTurnContext.bIsRaging = false;
}

void System::MainHandAttack(ATTACK_INFO& eOutAttackInfo)
{
	m_pPlayer->MainHandAttack(eOutAttackInfo);
}

void System::SetResistance(DAMAGE_TYPE eDamageType, bool bVal)
{
	m_pPlayer->SetResistance(eDamageType, bVal);
}

void System::SetBonusDamage(ATTACK_ABILITY eAttackAbility, int nDamage)
{
	m_pPlayer->SetBonusDamage(eAttackAbility, nDamage);
}

int System::GetPlayerLevel() const
{
	return m_pPlayer->GetLevel();
}

bool System::GetIsPlayerInCombat() const
{
	return m_pPlayer->GetIsPlayerInCombat();
}

void System::SetIsPlayerInCombat(bool bVal)
{
	m_pPlayer->SetIsPlayerInCombat(bVal);
}

void System::HandleTurnContext(TurnContext& sPlayerTurnContext)
{
	// Turn off Rage if player hasnt attacked
	if (!sPlayerTurnContext.bHasAttackedThisTurn && sPlayerTurnContext.bIsRaging)
		UnRage(sPlayerTurnContext);
	

	sPlayerTurnContext.bHasAttackedThisTurn = false;
	sPlayerTurnContext.bUsedAction = false;
	sPlayerTurnContext.bUsedBonusAction = false;
	sPlayerTurnContext.bUsedReaction = false;
}


bool System::BeginCombat(Monster* pMonsterInCombat)
{
	if (!pMonsterInCombat)
		return false;

	SetIsPlayerInCombat(true);

	std::cout << "combat begins\n";

	// Create inititaive vector. Each monster and player roll a d20
	using RollResult = int;
	std::vector<CombatActors> vInitiative;

	int nPlayerInitiative = Helper::RollD(20);
	vInitiative.emplace_back(INITIATIVE_HELPER_PLAYER_0, nPlayerInitiative);
	for (int i = 0; i < pMonsterInCombat->GetPackAttributes()->size() + 1; ++i)
	{
		vInitiative.emplace_back((INITIATIVE_HELPER)i, Helper::RollD(20));
	}

	// Start combat
	TurnContext sPlayerTurnContext;
	while (m_pPlayer->GetHealth() > 0  && /*pMonsterInCombat->IsAnyOfPackAlive() */ pMonsterInCombat->GetHealth() > 0) // For now I will assume there's only 1 monster. Later we need to check if any of hte monters is alive, not just the main on
	{
		for (auto& object : vInitiative)
		{
			if (object.m_eCombatActor == INITIATIVE_HELPER_PLAYER_0)
			{
				HandlePlayerTurn(pMonsterInCombat, sPlayerTurnContext);
				HandleTurnContext(sPlayerTurnContext);
			}
			else
				HandleMonsterTurn(pMonsterInCombat);	
		}
	}

	SetIsPlayerInCombat(false);
	if (m_pPlayer->GetHealth() <= 0)
	{
		// HandlePlayerLoss();
		/*
		Handle player Loss:
			- either return the user to the log in screen where they can load last save.
			- Or think of a system to respawn the player somewhere, along with some penalty.
				- I prefer to go with harsh penalties, for example:
					when defeated, the player is resurrected into a new body. They reset their exp, keep their lvl but lose
					all inventory and equipment
		*/
		std::cout << "Player Lost\n";
	}
		


	if (pMonsterInCombat->GetHealth() <= 0)
	{
		std::cout << "Monster Lost\n";
		if (!HandleMonsterLoss(pMonsterInCombat))
			return false;
	}
		

	/*
	Handle monster turn
		- for now simply use regular attack against player
	
	

	Handle Player turn
		
		- grab command from user(such as cast spell FireBolt, bash wolf1, kick wolf2, dash etc)

		- Process command:
			- First we figure out if its a valid command
			- than if its an action or a bonus action
			- validate things like, do I have an action/bonus action left, do i have spell slots left, 
				- A major thing to work here is the idea of range.	
					- Maybe for now i'll just assume everyone are in the range for everyone else, but in the future 
						i want to add the concept of range. A single point can be rougly 100 meters by 100 meters.
						So there's enough space to imagine that a whole battle is taking place in a single point


			- Process command
			- Roll a hit roll to check whether the attack hit

			- FireBolt(Monster* monster)
				- check the user intelligence modifier
				- check the monster defence or something
				- Roll attack damage
				- Check firebolt lvl(the user can choose the cast it as lvl 1,2,3...
				- Update monster hp accordingly
				- Reduce a spell slot from the user
				etc

		- Update the state of the player in terms of number of spell slots left, rituals, action, bonus action and so on


	*/

















	return true;
}



/* ********************************* CommandDispacher ********************************* */

CommandDispacher::CommandDispacher(System& system, Trie& commandLookUpTrie)
	: m_system(system), m_commandLookUpTrie(commandLookUpTrie)
{
	RegisterCommands();
}

void CommandDispacher::RegisterCommands()
{
	// Look - print room description
	m_mapCommandFunctions["look"] = [this](const UserCommand& cmd, OutParameters* sOutParams) {return this->Look(cmd); };

	// ------------------- NORTH -------------------
	m_mapCommandFunctions["north"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_NORTH); };
	m_mapCommandFunctions["n"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_NORTH); };
	m_mapCommandFunctions["go north"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_NORTH); };
	m_mapCommandFunctions["walk north"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_NORTH); };
	m_mapCommandFunctions["up"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_NORTH); };

	// ------------------- SOUTH -------------------
	m_mapCommandFunctions["south"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_SOUTH); };
	m_mapCommandFunctions["s"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_SOUTH); };
	m_mapCommandFunctions["go south"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_SOUTH); };
	m_mapCommandFunctions["walk south"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_SOUTH); };
	m_mapCommandFunctions["down"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_SOUTH); };

	// ------------------- EAST -------------------
	m_mapCommandFunctions["east"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_EAST); };
	m_mapCommandFunctions["e"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_EAST); };
	m_mapCommandFunctions["go east"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_EAST); };
	m_mapCommandFunctions["walk east"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_EAST); };
	m_mapCommandFunctions["right"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_EAST); };

	// ------------------- WEST -------------------
	m_mapCommandFunctions["west"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_WEST); };
	m_mapCommandFunctions["w"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_WEST); };
	m_mapCommandFunctions["go west"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_WEST); };
	m_mapCommandFunctions["walk west"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_WEST); };
	m_mapCommandFunctions["left"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->Walk(cmd, DIRECTION_WEST); };

	// ------------------- Start Combat -------------------
	m_mapCommandFunctions["attack"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->StartCombat(cmd); };
	m_mapCommandFunctions["kill"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->StartCombat(cmd); };


	// ------------------- Player Action -------------------
	m_mapCommandFunctions["main hand attack"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->MainHandAttack(cmd, sOutParams); };
	m_mapCommandFunctions["use main weapon"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->MainHandAttack(cmd, sOutParams); };
	m_mapCommandFunctions["hit"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->MainHandAttack(cmd, sOutParams); };

	m_mapCommandFunctions["end"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->EndTurn(cmd, sOutParams); };
	m_mapCommandFunctions["end turn"] = [this](const UserCommand& cmd, OutParameters* sOutParams) { return this->EndTurn(cmd, sOutParams); };


	// Register all commands to Trie lookup
	for (const auto& command : m_mapCommandFunctions)
	{
		m_commandLookUpTrie.Insert(command.first);
	}

}

void CommandDispacher::RegisterCommand(const std::string& name, std::function<ERROR_CODE(const UserCommand&, OutParameters*)> func)
{
	m_mapCommandFunctions[name] = func;
	m_commandLookUpTrie.Insert(name);
}

/*
True - continuing game loop and move all monsters
False - Get next command instead of continuing game loop
*/
ERROR_CODE CommandDispacher::ExecuteCommand(const std::string& strUserCommand, OutParameters* sOutParams)
{
	if (strUserCommand.empty())
		return ERROR_CODE_GET_NEXT_COMMAND; // Get next command instead of continuing game loop


	// First we search the user command in TRIE. And return the closest string to the user request
	// So if user requested "North 2", we search for it in the TRIE and return the closest match, which is "North"
	std::string strValidCommand = m_commandLookUpTrie.Search(strUserCommand); 

	// Then we verify that this closest string we could find, is actually a command
	if (m_mapCommandFunctions.find(strValidCommand) == m_mapCommandFunctions.end())
	{
		std::cout << "Unknown command! Try 'help' to see available commands.\n";
		return ERROR_CODE_GET_NEXT_COMMAND;
	}

	// If so, we activate the function related to that command, and provide the function with THE COMPLETE USER STRING
	return m_mapCommandFunctions.at(strValidCommand)(strUserCommand, sOutParams); // For example: Walk("North 2")
}

ERROR_CODE CommandDispacher::Look(const std::string& strCommand)
{
	// Narration
	std::string strCurrentRoomDescription;
	std::cout << "Looking....\n";
	std::this_thread::sleep_for(std::chrono::seconds(2));

	// Room Description
	if (!m_system.GetCurrentRoomDescription(strCurrentRoomDescription))
		return ERROR_CODE_GET_NEXT_COMMAND;
	std::cout << strCurrentRoomDescription << "\n";


	///////////////////////////////////
	// Dynamic Descriptions
	///////////////////////////////////
	// Monsters in nearby points
	std::string dynamicMsgMonsters = m_system.GetDynamicDescriptionForMonsters(m_system.GetPlayerLocation());
	std::cout << dynamicMsgMonsters << "\n";
	// Objects in nearby points
	//m_system->GetDynamicDescriptionForObjects();

	return ERROR_CODE_GET_NEXT_COMMAND;
}

ERROR_CODE CommandDispacher::MainHandAttack(const std::string& strCommand, OutParameters* sOutParams)
{
	m_system.MainHandAttack(sOutParams->sAttackInfo);
	sOutParams->sTurnContext.bHasAttackedThisTurn = true;
	return ERROR_CODE_CONTINUE_GAME_LOOP;
}

ERROR_CODE CommandDispacher::EndTurn(const std::string& strCommand, OutParameters* sOutParams)
{
	sOutParams->sAttackInfo.eActionType = ACTION_TYPE_END_TURN;
	return ERROR_CODE_CONTINUE_GAME_LOOP;
}


ERROR_CODE CommandDispacher::Rage(const std::string& strCommand, CommandDispacher::OutParameters* sOutParams)
{
	std::cout << "You're blood boils as you become enraged. You feel invincible and invulnerable to attacks\n";
	// Damage Resistance
	m_system.SetResistance(DAMAGE_TYPE_BLUDGEONING, true);
	m_system.SetResistance(DAMAGE_TYPE_PIERCING, true);
	m_system.SetResistance(DAMAGE_TYPE_SLASHING, true);

	// Strength attacks bonus
	m_system.SetBonusDamage(ATTACK_ABILITY_STRENGTH, Helper::GetRageBonusForLevel(m_system.GetPlayerLevel()));

	sOutParams->sAttackInfo.eActionType = ACTION_TYPE_BONUS_ACTION;
	sOutParams->sTurnContext.bIsRaging = true;

	return ERROR_CODE_CONTINUE_GAME_LOOP;
}


ERROR_CODE CommandDispacher::Walk(const std::string& strCommand, DIRECTION eDirection)
{
	int nNumOfSteps = 1;

	// If user inserted a number of steps. Grab it from string
	size_t pos = strCommand.find(' ');
	if (pos != std::string::npos)
	{
		std::string numberOfSteps = strCommand.substr(pos);

		try
		{
			nNumOfSteps = std::stoi(numberOfSteps);
		}
		catch (...)
		{
			nNumOfSteps = 1;
		}
	}

	if (!m_system.PlayerChangeLocation(eDirection, nNumOfSteps))
	{
		return ERROR_CODE_GET_NEXT_COMMAND;
	}
		
	std::cout << "Walking....\n";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return ERROR_CODE_CONTINUE_GAME_LOOP;
}


ERROR_CODE CommandDispacher::StartCombat(const std::string& strCommand)
{
	if (m_system.GetIsPlayerInCombat())
	{
		std::cout << "You are already in Combat!\n";
		return ERROR_CODE_GET_NEXT_COMMAND;
	}


	std::string monsterToAttack;
	size_t pos = strCommand.find(' ');
	if (pos != std::string::npos)
		monsterToAttack = strCommand.substr(pos + 1);
	else
	{
		std::cout << "Attack who?\n";
		return ERROR_CODE_GET_NEXT_COMMAND;
	}
		

	
	/*
	Collect vector of monsters the player has started the combat with
	
	If its a pack of monsters, iterate over all the monsters in the pack and insert them into the vector


	*/

	//////////////// Temporary ////////////////////
	/*
	For now I'll just take the first monster in the point

	Later I'll need to use the command dispatcher and Trie to translare things like "bear" to MONSTER_TYPE_BEAR
	*/

	LOCATION ePlayerLocation = m_system.GetPlayerLocation();



	/*
	Instead of monsters in point, I need to collect all the monsters the player can see

	and then continue as usual.

	This is a good time to start thinking about visiblity skill of player and visibility threshold of monster
	*/

	//std::unique_ptr<Monster>* vMonstersInPoint = m_system.GetMonstersInPoint(ePlayerLocation);
	std::vector<Monster*> vVisibleMonsters = m_system.GetVisibleMonsters();
	if (vVisibleMonsters.empty())
	{
		std::cout << "There's no one to attack\n";
		return ERROR_CODE_GET_NEXT_COMMAND;
	}

	for (int i = 0; i < vVisibleMonsters.size(); ++i)
	{
		MONSTER_TYPE eMonsterType = Helper::ConvertStringToMonsterType(monsterToAttack);
		if (eMonsterType == vVisibleMonsters[i]->GetMonsterType())
		{
			if (!m_system.BeginCombat(vVisibleMonsters[i]))
				return ERROR_CODE_FAILED;

			return ERROR_CODE_CONTINUE_GAME_LOOP;
		}
			
	}

	std::cout << "There is no " + monsterToAttack + " here\n";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return ERROR_CODE_GET_NEXT_COMMAND;
}

