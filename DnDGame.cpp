#include <iostream>
#include <thread>
#include "DnDGame.hpp"


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

	std::cout << "Enter Your Name: ";
	std::cin >> name;
	m_strPlayerName = name;

	std::cout << "Enter Your Age: ";
	std::cin >> age;
	m_PlayerAge = age;

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
	if (!m_sCurrentLocation.IsValidLocation())
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
	LOCATION eNewLocation(m_sCurrentLocation);
	int XPointCordIndex = 0, YPointCordIndex = 0;
	m_sCurrentLocation.ConvertPointIndexToXYCord(XPointCordIndex, YPointCordIndex);

	int XRoomCordIndex = 0, YRoomCordIndex = 0;
	m_sCurrentLocation.ConvertRoomIndexToXYCord(XRoomCordIndex, YRoomCordIndex);

	switch (eDirection)
	{
	case DIRECTION_NORTH:

		// If player is not at the north edge of room. Go north
		if (YPointCordIndex > 0)
		{
			++YPointCordIndex;
			m_sCurrentLocation.ConvertXYCordToPointIndex(XPointCordIndex, YPointCordIndex);
		}
		else
		{
			// Otherwise, if player is not at the north edge of area. Go north to next room
			if (YRoomCordIndex > 0)
			{
				++YRoomCordIndex; // Player has moved to north room
				m_sCurrentLocation.ConvertXYCordToRoomIndex(XRoomCordIndex, YRoomCordIndex);

				m_sCurrentLocation.m_nPointIndex = G_ROOM_LENGTH - 1; // player is now at the bottom of new room	
			}
		}

		break;
	case DIRECTION_SOUTH:
		break;
	case DIRECTION_EAST:
		break;
	case DIRECTION_WEST:
		break;
	default:
		break;
	}

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
	m_pPlayer = std::make_unique<Player>();
	m_pMap = std::make_unique<Map>();
	m_pCommandDispacher = std::make_unique<CommandDispacher>(*this);

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

	std::cout << "\nYour journey with the " << m_pPlayer->GetFactionAsString()
		<< " begins now...\n\n";
}

bool System::GetCurrentRoomDescription(std::string& strOutRoomDescription) const
{
	LOCATION sCurrentLocation;
	if (!m_pPlayer->GetCurrentLocation(sCurrentLocation))
		return false;

	Room* pOutCurrentRoom = nullptr;
	if (!m_pMap->GetRoom(sCurrentLocation, pOutCurrentRoom))
		return false;

	if (!pOutCurrentRoom)
		return false;

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



	while (bIsPlaying)
	{

		std::string strUserCommand;
		std::cin >> strUserCommand;
		if (!m_pCommandDispacher->ExecuteCommand(strUserCommand))
		{
			;
		}

		/*
		


		Grab command from user


		Process command


		
		*/






	}

	/*
	Save current progress
	*/
}

/* ********************************* Room ********************************* */
Room::Room(AREA_NAME eAreaName, int nRoomIndex, const std::string& strRoomDescription)
	: m_eAreaName(eAreaName), m_nRoomIndex(nRoomIndex), m_strRoomDescription(strRoomDescription)
{

	/*
	For now, lets say that every 5 tiles a monster appears
	*/

	for (int i = 0; i < G_MAX_ROOM_SIZE; ++i)
	{
		if (i % 5 == 0)
		{
			LOCATION sMonsterLocation(m_eAreaName, m_nRoomIndex, i);
			m_vMonsters.push_back(std::make_unique<Monster>(sMonsterLocation));
		}
			
	}
}

std::string Room::GetRoomDescription() const
{
	return m_strRoomDescription;
}


/* ********************************* Area ********************************* */
Area::Area(AREA_NAME eAreaName)
	: m_eAreaName(eAreaName)
{
	for (size_t i = 0; i < G_MAX_AREA_SIZE; ++i)
	{
		m_vRooms.push_back(std::make_unique<Room>(eAreaName, i));
	}
}

void Area::GenerateRoom()
{
}

bool Area::GetRoom(int nRoomIndex, Room* pOutCurrentRoom) const
{
	if (nRoomIndex > G_MAX_AREA_SIZE)
		return false;

	pOutCurrentRoom = m_vRooms[nRoomIndex].get();

	return true;
}




/* ********************************* Map ********************************* */

Map::Map()
{
	
}

void Map::GenerateArea(AREA_NAME eAreaName)
{
	/*
	iterate over G_MAX_AREA_SIZE and create room objects for each



	constexpr int G_MAX_ROOM_SIZE = 50 * 50; // Each room is a 50x50 grid of points
constexpr int G_MAX_AREA_SIZE = 10 * 10; // Each area is a 10x10 grid of rooms



		For each room, randomize monsters according to the biomes


	*/

	m_mapAreas[eAreaName] = std::make_unique<Area>(eAreaName);
}




bool Map::GetRoom(LOCATION sRoomLocation, Room* pOutCurrentRoom) const
{
	if (!sRoomLocation.IsValidLocation())
		return false;

	if (!m_mapAreas.at(sRoomLocation.m_eAreaName)->GetRoom(sRoomLocation.m_nRoomIndex, pOutCurrentRoom))
		return false;

	return true;
}



/* ********************************* Monster ********************************* */
Monster::Monster(LOCATION eCurrentLocation)
	: m_eCurrentLocation(eCurrentLocation)
{
}

void Monster::Attack()
{
	std::cout << "Attack\n";
}




/* ********************************* CommandDispacher ********************************* */

CommandDispacher::CommandDispacher(System& pSystem)
	: m_pSystem(pSystem)
{
	RegisterCommands();
}

void CommandDispacher::RegisterCommands()
{
	// Look - print room description
	m_mapCommandFunctions["look"] = [this](const UserCommand& cmd) {return this->Look(cmd); };

	// Go/Walk North - Move 1 tile up
	m_mapCommandFunctions["north"] = [this](const UserCommand& cmd) {return this->WalkNorth(cmd); };
	m_mapCommandFunctions["go north"] = [this](const UserCommand& cmd) {return this->WalkNorth(cmd); };
	m_mapCommandFunctions["walk north"] = [this](const UserCommand& cmd) {return this->WalkNorth(cmd); };
	m_mapCommandFunctions["n"] = [this](const UserCommand& cmd) {return this->WalkNorth(cmd); };
	m_mapCommandFunctions["w"] = [this](const UserCommand& cmd) {return this->WalkNorth(cmd); };
	m_mapCommandFunctions["up"] = [this](const UserCommand& cmd) {return this->WalkNorth(cmd); };
}

bool CommandDispacher::ExecuteCommand(const std::string& strCommand)
{
	if (m_mapCommandFunctions.find(strCommand) == m_mapCommandFunctions.end())
	{
		std::cout << "Command doesnt exist. Try again\n";
		return false;
	}
		

	return m_mapCommandFunctions.at(strCommand)(strCommand);
}

bool CommandDispacher::Look(const std::string& strCommand)
{
	std::string strCurrentRoomDescription;
	std::cout << "Looking....\n";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	if (!m_pSystem.GetCurrentRoomDescription(strCurrentRoomDescription))
		return false;

	std::cout << strCurrentRoomDescription << "\n";
	return true;
}

bool CommandDispacher::WalkNorth(const std::string& strCommand)
{
	DIRECTION eDirection(DIRECTION_NORTH);
	int nNumOfSteps = 1;
	if (!m_pSystem.PlayerChangeLocation(eDirection, nNumOfSteps))
	{
		std::cout << "You cant go there\n";
		return false;
	}
		
	std::cout << "Walking Up....\n";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return true;
}


