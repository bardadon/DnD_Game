#include <iostream>
#include <thread>
#include "System.hpp"

std::unordered_map<std::string, MONSTER_TYPE> Helper::m_mapStrToMonsterType;


/* ********************************* Wizard ********************************* */
void Wizard::PrintClassDescription() const
{
	std::cout << R"(
A scholar of the arcane arts. 
You wield powerful spells drawn from ancient tomes and forbidden knowledge. Fragile in body, limitless in potential.
)";
}



/* ********************************* Fighter ********************************* */
void Fighter::PrintClassDescription() const
{
	std::cout << R"(
A master of weapons and tactics. You rely on strength, endurance, and skill in battle. 
From heavy armor to precise swordplay, you are the backbone of every adventuring party.
)";
}


/* ********************************* Player ********************************* */
Player::Player()
{
	/*
	Begin char creation process
	*/
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
   Strength: 8   Dexterity: 6   Intelligence: 4   Constitution: 8   Perception: 5\n
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
)";

	std::cout << "\nEnter your choice (1-4): ";
	std::cin >> nClass;

	switch (nClass)
	{
	case 1: 
		m_PlayerClass = std::make_unique<Fighter>();
		break;
	case 2: 
		m_PlayerClass = std::make_unique <Wizard>();
		break;
	//case 3: 
	//	m_PlayerClass = "Cleric"; 
	//	break;
	//case 4: 
	//	m_PlayerClass = "Rogue"; 
	//	break;
	default: 
		break;
	}

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
	PopulateWorld();
	/////////////////////////// Temporary //////////////////////////////////


	m_pPlayer = std::make_unique<Player>(); // Start character creation process
	

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
		std::vector<std::unique_ptr<Monster>>& vMonstersInPoint =
			m_pObjectManager->m_mapMonsterLocator[Helper::ConvertLocationToWorldIndex(pPointLocation)];

		for (const auto& monster : vMonstersInPoint)
		{
			// Print dynamic desc for monster
			int nPlayerVisibilityFactor = 10; // CalculateVisibilityFactor(monster.get());
			int nMonsterVisibilityThreshold = monster->GetVisibilityThreshold();

			if (nPlayerVisibilityFactor >= nMonsterVisibilityThreshold)
			{
				const MONSTER_TYPE type = monster->GetMonsterAttributes().m_eType;
				const int strength = monster->GetMonsterAttributes().m_nStrength;

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
				if (monster->IsPartOfAPack())
				{
					const std::vector<MONSTER_ATTRIBUTES>* pack = monster->GetPackAttributes();

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
	}

	return strDynamicMsg;
}

LOCATION System::GetPlayerLocation() const
{
	return m_pPlayer->GetCurrentLocation();
}

std::vector<std::unique_ptr<Monster>>* System::GetMonstersInPoint(LOCATION eLocation)
{
	return m_pObjectManager->GetMonstersInPoint(eLocation);
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


	while (bIsPlaying)
	{
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
		if (ERROR_CODE_CONTINUE_GAME_LOOP != m_pCommandDispacher->ExecuteCommand(strUserCommand))
			continue;


		// Move all monsters in the area
		m_pObjectManager->MoveAllMonsters();






	}

	/*
	Save current progress
	*/
}




///////////////////////////////////////////////
// Populate world with a generic setting of Monsters and NPCs
// 
// The rest of the Monsters, Npcs and Objects are filled manually according to world lore and quests
///////////////////////////////////////////////
void System::PopulateWorld()
{
	int singleMonsterCounter = 0;
	int packMonsterCounter = 0;


	std::cout << "System::PopulateWorld()::Populating World\n";
	///////////////////// Temporary ////////////////////////
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

				int rollForSingleMonster = std::rand() % 10;
				int rollForPackMonster = std::rand() % 10;

				if (rollForSingleMonster == 1)
				{
					MONSTER_ATTRIBUTES sAttr;
					int rollForType = std::rand() % MONSTER_TYPE_LAST;
					sAttr.m_eType = static_cast<MONSTER_TYPE>(rollForType);
					m_pObjectManager->CreateMonster(sAttr, pPoint->GetLocation());
				}
				if (rollForPackMonster == 1)
				{
					MONSTER_ATTRIBUTES sAttr;
					int rollForType = std::rand() % MONSTER_TYPE_LAST;
					sAttr.m_eType = static_cast<MONSTER_TYPE>(rollForType);

					std::vector<MONSTER_ATTRIBUTES> vPack;
					for (int i = 0; i < 5; ++i)
						vPack.push_back(sAttr);

					m_pObjectManager->CreatePackOfMonster(sAttr, pPoint->GetLocation(), vPack);
				}


			}
		}

	}
	///////////////////// Temporary ////////////////////////


}

bool System::BeginCombat(const std::vector<Monster*>& vMonstersInCombat)
{
	std::cout << "combat begins\n";
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
	m_mapCommandFunctions["look"] = [this](const UserCommand& cmd) {return this->Look(cmd); };

	// ------------------- NORTH -------------------
	m_mapCommandFunctions["north"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_NORTH); };
	m_mapCommandFunctions["n"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_NORTH); };
	m_mapCommandFunctions["go north"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_NORTH); };
	m_mapCommandFunctions["walk north"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_NORTH); };
	m_mapCommandFunctions["up"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_NORTH); };

	// ------------------- SOUTH -------------------
	m_mapCommandFunctions["south"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_SOUTH); };
	m_mapCommandFunctions["s"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_SOUTH); };
	m_mapCommandFunctions["go south"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_SOUTH); };
	m_mapCommandFunctions["walk south"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_SOUTH); };
	m_mapCommandFunctions["down"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_SOUTH); };

	// ------------------- EAST -------------------
	m_mapCommandFunctions["east"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_EAST); };
	m_mapCommandFunctions["e"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_EAST); };
	m_mapCommandFunctions["go east"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_EAST); };
	m_mapCommandFunctions["walk east"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_EAST); };
	m_mapCommandFunctions["right"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_EAST); };

	// ------------------- WEST -------------------
	m_mapCommandFunctions["west"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_WEST); };
	m_mapCommandFunctions["w"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_WEST); };
	m_mapCommandFunctions["go west"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_WEST); };
	m_mapCommandFunctions["walk west"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_WEST); };
	m_mapCommandFunctions["left"] = [this](const UserCommand& cmd) { return this->Walk(cmd, DIRECTION_WEST); };

	// ------------------- Start Combat -------------------
	m_mapCommandFunctions["attack"] = [this](const UserCommand& cmd) { return this->StartCombat(cmd); };
	m_mapCommandFunctions["kill"] = [this](const UserCommand& cmd) { return this->StartCombat(cmd); };


	// Register all commands to Trie lookup
	for (const auto& command : m_mapCommandFunctions)
	{
		m_commandLookUpTrie.Insert(command.first);
	}

}

/*
True - continuing game loop and move all monsters
False - Get next command instead of continuing game loop
*/
ERROR_CODE CommandDispacher::ExecuteCommand(const std::string& strUserCommand)
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
	return m_mapCommandFunctions.at(strValidCommand)(strUserCommand); // For example: Walk("North 2")
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
	std::vector<std::unique_ptr<Monster>>* vMonstersInPoint = m_system.GetMonstersInPoint(ePlayerLocation);
	if (!vMonstersInPoint || vMonstersInPoint->empty())
	{
		std::cout << "There's no one to attack\n";
		return ERROR_CODE_GET_NEXT_COMMAND;
	}

	std::vector<Monster*> vMonstersInCombat;
	for (int i = 0; i < vMonstersInPoint->size(); ++i)
	{
		MONSTER_TYPE eMonsterType = Helper::ConvertStringToMonsterType(monsterToAttack);
		if (eMonsterType == (*vMonstersInPoint)[i]->GetMonsterType())
			vMonstersInCombat.push_back((*vMonstersInPoint)[i].get());
			/*
			Add the whole pack that go along with this monster
			*/


	}
	
	////////////////// Temporary ////////////////////

	if (vMonstersInCombat.empty())
	{
		std::cout << "There is no " + monsterToAttack + " here\n";
		return ERROR_CODE_GET_NEXT_COMMAND;
	}
		


	m_system.BeginCombat(vMonstersInCombat);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	return ERROR_CODE_CONTINUE_GAME_LOOP;
}


