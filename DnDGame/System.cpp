#include <iostream>
#include <thread>

#include "System.hpp"


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
bool Player::SetCurrentLocation(LOCATION sCurrentLocation)
{
	if (!sCurrentLocation.IsValidLocation())
		return false;

	m_sCurrentLocation = sCurrentLocation;
	return true;
}

bool Player::GetCurrentLocation(LOCATION& sOutCurrentLocation) const 
{
	if (!m_sCurrentLocation.IsValidLocation())
		return false;

	sOutCurrentLocation = m_sCurrentLocation;
	return true;
}

bool Player::PlayerChangeLocation(DIRECTION eDirection, int nNumOfSteps)
{
	int px, py;   // point XY
	int rx, ry;   // room XY
	//int ax, ay;   // area XY

	// Extract coordinates
	Helper::ConvertPointIndexToXYCord(m_sCurrentLocation.m_nPointIndex, px, py);
	Helper::ConvertRoomIndexToXYCord(m_sCurrentLocation.m_nRoomIndex, rx, ry);

	// Direction deltas
	int dx = 0, dy = 0;
	switch (eDirection)
	{
	case DIRECTION_NORTH: dy = -1; break;
	case DIRECTION_SOUTH: dy = 1; break;
	case DIRECTION_EAST:  dx = 1; break;
	case DIRECTION_WEST:  dx = -1; break;
	default:
		return false;
	}

	for (int step = 0; step < nNumOfSteps; ++step)
	{
		int new_px = px + dx;
		int new_py = py + dy;

		// ---------- POINT LEVEL MOVEMENT ----------
		if (new_px >= 0 && new_px < G_ROOM_LENGTH &&
			new_py >= 0 && new_py < G_ROOM_LENGTH)
		{
			// Move within room
			px = new_px;
			py = new_py;
		}
		else
		{
			// ---------- ROOM LEVEL MOVEMENT ----------
			int new_rx = rx;
			int new_ry = ry;

			if (new_px < 0)        new_rx--;
			if (new_px >= G_ROOM_LENGTH) new_rx++;
			if (new_py < 0)        new_ry--;
			if (new_py >= G_ROOM_LENGTH) new_ry++;

			// Check room boundaries
			if (new_rx < 0 || new_rx >= G_AREA_LENGTH ||
				new_ry < 0 || new_ry >= G_AREA_LENGTH)
			{
				std::cout << "You can't go that way.\n";
				return false;
			}

			// Commit room move
			rx = new_rx;
			ry = new_ry;

			// Wrap the point coordinates into the new room
			if (new_px < 0)                 px = G_ROOM_LENGTH - 1;
			else if (new_px >= G_ROOM_LENGTH) px = 0;
			else                              px = new_px;

			if (new_py < 0)                 py = G_ROOM_LENGTH - 1;
			else if (new_py >= G_ROOM_LENGTH) py = 0;
			else                              py = new_py;
		}
	}

	// Finally convert back to indices
	Helper::ConvertXYCordToPointIndex(px, py, m_sCurrentLocation.m_nPointIndex);
	Helper::ConvertXYCordToRoomIndex(rx, ry, m_sCurrentLocation.m_nRoomIndex);

	return true;
}









/* ********************************* System ********************************* */
/*
BEFORE GAME LOOP STARTS:

	- Begin log in, registartion process
	- Load information from disk to start running the game
			- When saving a game we serialize the Player, Party members, Map, areas and rooms
			to disk and load it all when we continue playing


*/
System::System()
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

	

}


void System::CreateNewJourney()
{
	m_pCommandLookupTrie = std::make_unique<Trie>();
	m_pCommandDispacher = std::make_unique<CommandDispacher>(*this, *m_pCommandLookupTrie);

	m_pPlayer = std::make_unique<Player>();
	m_pMap = std::make_unique<Map>();
	m_pObjectManager = std::make_unique<ObjectManager>();

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
		m_pMap->GenerateArea(sStartingLocation.m_eAreaName);
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


	/////////////////////////// Temporary //////////////////////////////////
	// Generete a starting template of Monsters, NPCS and Objects

	m_pObjectManager->PopulateWorld();




	/////////////////////////// Temporary //////////////////////////////////


	std::cout << "\nYour journey with the " << m_pPlayer->GetFactionAsString()
		<< " begins now...\n\n";
}

bool System::GetCurrentRoomDescription(std::string& strOutRoomDescription) const
{
	// Get player location
	LOCATION sCurrentLocation;
	if (!m_pPlayer->GetCurrentLocation(sCurrentLocation))
		return false;

	// Use Location to get Room
	Room* pOutCurrentRoom = nullptr;
	if (!m_pMap->GetRoom(sCurrentLocation, pOutCurrentRoom))
		return false;

	if (!pOutCurrentRoom)
		return false;

	// Print room desc
	strOutRoomDescription = pOutCurrentRoom->GetRoomDescription();
	return true;

}

bool System::PlayerChangeLocation(DIRECTION eDirection, int nNumOfSteps)
{
	return m_pPlayer->PlayerChangeLocation(eDirection, nNumOfSteps);
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
		LOCATION sCurrentLocation;
		if (!m_pPlayer->GetCurrentLocation(sCurrentLocation))
		{
			std::cout << "Player is not in a valid location...\n";
		}


		Helper::ConvertPointIndexToXYCord(sCurrentLocation.m_nPointIndex, XPoint, YPoint);

		std::cout << "\n[Location: " << Helper::GetAreaNameAsString(sCurrentLocation.m_eAreaName)
			<< " | RoomIdx: " << sCurrentLocation.m_nRoomIndex << " "
			<< " | PointIdx: " << sCurrentLocation.m_nPointIndex << " "
			<< " | Cords: [" << XPoint << "," << YPoint <<"]> ";

		std::string strUserCommand;
		
		std::getline(std::cin, strUserCommand);
		m_pCommandDispacher->ExecuteCommand(strUserCommand);

		/*
		


		Grab command from user


		Process command


		
		*/






	}

	/*
	Save current progress
	*/
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


	// Register all commands to Trie lookup
	for (const auto& command : m_mapCommandFunctions)
	{
		m_commandLookUpTrie.Insert(command.first);
	}

}

bool CommandDispacher::ExecuteCommand(const std::string& strCommand)
{
	if (!m_commandLookUpTrie.Search(strCommand))
	{
		std::cout << "Unknown command! Try 'help' to see available commands.\n";
		return false;
	}


	return m_mapCommandFunctions.at(strCommand)(strCommand);
}

bool CommandDispacher::Look(const std::string& strCommand)
{
	std::string strCurrentRoomDescription;
	std::cout << "Looking....\n";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	if (!m_system.GetCurrentRoomDescription(strCurrentRoomDescription))
		return false;

	std::cout << strCurrentRoomDescription << "\n";
	return true;
}

bool CommandDispacher::Walk(const std::string& strCommand, DIRECTION eDirection)
{
	int nNumOfSteps = 1;
	if (!m_system.PlayerChangeLocation(eDirection, nNumOfSteps))
	{
		return false;
	}
		
	std::cout << "Walking....\n";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return true;
}


//bool CommandDispacher::WalkN(const std::string& strCommand, DIRECTION eDirection)
//{
//	std::string cmdCopy = Helper::TrimTrailingSpaces(strCommand);
//	int digit = static_cast<int>(strCommand[strCommand.size() - 1]);
//
//
//	if (!m_pSystem.PlayerChangeLocation(eDirection, nNumOfSteps))
//	{
//		return false;
//	}
//		
//	std::cout << "Walking Up....\n";
//	std::this_thread::sleep_for(std::chrono::seconds(2));
//	return true;
//}
