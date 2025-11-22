#include "Map.hpp"




/* ********************************* Point ********************************* */
Point::Point(LOCATION sPointLocation, const std::string& strPointDescription, bool bHasVisited, 
	POINT_TYPE ePointType) :
	m_sPointLocation(sPointLocation), m_strPointDescription(strPointDescription), m_bHasVisited(bHasVisited),
	m_ePointType(ePointType)
{
	////////////////////// Temporary //////////////////////
	m_strPointDescription = "Point " + std::to_string(m_sPointLocation.m_nPointIndex);
	////////////////////// Temporary //////////////////////
}

LOCATION Point::GetLocation() const
{
	return m_sPointLocation;
}

POINT_TYPE Point::GetPointType() const
{
	return m_ePointType;
}

void Point::SetPointType(POINT_TYPE ePointType)
{
	m_ePointType = ePointType;
}








/* ********************************* Room ********************************* */
Room::Room(AREA_NAME eAreaName, int nRoomIndex, const std::string& strRoomDescription)
	: m_eAreaName(eAreaName), m_nRoomIndex(nRoomIndex), m_strRoomDescription(strRoomDescription)
{
	for (int pointIndex = 0; pointIndex < G_MAX_ROOM_SIZE; ++pointIndex)
	{
		LOCATION sPointLocation(eAreaName, nRoomIndex, pointIndex);
		m_vPoints.push_back(std::make_unique<Point>(sPointLocation));

		////////////////////// Temporary //////////////////////
		m_vPoints[pointIndex]->SetPointType(static_cast<POINT_TYPE>(pointIndex % POINT_TYPE_LAST));

		////////////////////// Temporary //////////////////////



	}

	////////////////////// Temporary //////////////////////
	m_strRoomDescription = "Room " + std::to_string(nRoomIndex);
	
	////////////////////// Temporary //////////////////////
}

std::string Room::GetRoomDescription() const
{
	return m_strRoomDescription;
}

Point* Room::GetPoint(int nPointIndex)
{
	if (nPointIndex > G_MAX_ROOM_SIZE)
		return nullptr;

	return m_vPoints[nPointIndex].get();
}

size_t Room::GetNumberOfPoints() const
{
	return m_vPoints.size();
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

Room* Area::GetRoom(int nRoomIndex)
{
	if (nRoomIndex > G_MAX_AREA_SIZE)
		return nullptr;

	return m_vRooms[nRoomIndex].get();
}

size_t Area::GetNumberOfRooms() const
{
	return m_vRooms.size();
}




/* ********************************* Map ********************************* */

Map::Map()
{
	for (int area = 0; area < AREA_NAME_LAST; ++area)
	{
		AREA_NAME eAreaName = static_cast<AREA_NAME>(area);
		m_mapAreas[eAreaName] = std::make_unique<Area>(eAreaName);
	}	
}

size_t Map::GetNumberOfAreas() const
{
	return m_mapAreas.size();
}

Room* Map::GetRoom(AREA_NAME eAreaName, int nRoomIndex)
{
	return GetArea(eAreaName)->GetRoom(nRoomIndex);
}

Area* Map::GetArea(AREA_NAME eAreaName)
{
	return m_mapAreas[eAreaName].get();
}
