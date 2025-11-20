#include "Map.hpp"




/* ********************************* Point ********************************* */
Point::Point(LOCATION sPointLocation, const std::string& strPointDescription, bool bHasVisited) :
	m_sPointLocation(sPointLocation), m_strPointDescription(strPointDescription), m_bHasVisited(bHasVisited)
{
	////////////////////// Temporary //////////////////////
	m_strPointDescription = "Point " + std::to_string(m_sPointLocation.m_nPointIndex);
	////////////////////// Temporary //////////////////////
}








/* ********************************* Room ********************************* */
Room::Room(AREA_NAME eAreaName, int nRoomIndex, const std::string& strRoomDescription)
	: m_eAreaName(eAreaName), m_nRoomIndex(nRoomIndex), m_strRoomDescription(strRoomDescription)
{
	for (int pointIndex = 0; pointIndex < G_MAX_ROOM_SIZE; ++pointIndex)
	{
		LOCATION sPointLocation(eAreaName, nRoomIndex, pointIndex);
		m_vPoints.push_back(std::make_unique<Point>(sPointLocation));
	}

	////////////////////// Temporary //////////////////////
	m_strRoomDescription = "Room " + std::to_string(nRoomIndex);
	////////////////////// Temporary //////////////////////
}

std::string Room::GetRoomDescription() const
{
	return m_strRoomDescription;
}


/* ********************************* Area ********************************* */
Area::Area(AREA_NAME eAreaName)
	: m_eAreaName(eAreaName)
{
	// Generate 10x10 rooms
	for (int i = 0; i < G_MAX_AREA_SIZE; ++i)
	{
		m_vRooms.push_back(std::make_unique<Room>(eAreaName, i));
	}
}

void Area::GenerateRoom()
{
}

bool Area::GetRoom(int nRoomIndex, Room*& pOutCurrentRoom) const
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

	m_mapAreas[eAreaName] = std::make_unique<Area>(eAreaName); // Create Area Object, with 10x10 rooms
}




bool Map::GetRoom(LOCATION sRoomLocation, Room*& pOutCurrentRoom) const
{
	if (!sRoomLocation.IsValidLocation())
		return false;

	if (!m_mapAreas.at(sRoomLocation.m_eAreaName)->GetRoom(sRoomLocation.m_nRoomIndex, pOutCurrentRoom))
		return false;

	return true;
}