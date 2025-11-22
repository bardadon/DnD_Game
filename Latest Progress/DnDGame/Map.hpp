#ifndef __MAP_HPP__
#define __MAP_HPP__

#include <unordered_map>
#include <memory>

#include "Helper.hpp"
#include "GameObject.hpp"

class Area;
class Room;
class Point;

/* ********************************* Map *********************************
Map class can generate Areas
	- Each Area is consisted of multiple Rooms
	- Each Room is a matrix that is filled with npcs, monsters and so on

The entire world map is a vector of areas
Each area is an NxM grid of rooms
And each room is an NxM grid of points

We dont need to load the entire world map and store it in memory constantly, only when needed

A player location is noted by:
	- Area index in Map::m_vAreas which leads to a certain area
	- cord (x, y) which leads to a room in area
	- cord (x, y) which leads to a point in room


*/
class Map
{
public:
	Map();
	virtual ~Map() = default;
	size_t GetNumberOfAreas() const;
	Room* GetRoom(AREA_NAME eAreaName, int nRoomIndex);
	Area* GetArea(AREA_NAME eAreaName);
private:
	std::unordered_map<AREA_NAME, std::unique_ptr<Area>> m_mapAreas;





}; // Map



/* ********************************* Area *********************************
Area class can geneerate Rooms
	- Each area has its own Biome, difficulty lvl and so on
	- Area is a grid of 10x10 rooms
*/
class Area
{
public:
	Area(AREA_NAME eAreaName = AREA_NAME_NONE);
	~Area() = default;

	void GenerateRoom();
	Room* GetRoom(int nRoomIndex);
	size_t GetNumberOfRooms() const;
private:
	AREA_NAME m_eAreaName;

	/*
	Some data structure to hold all rooms
	*/
	std::vector<std::unique_ptr<Room>> m_vRooms; // 10x10 grid of Rooms

}; // Area

/* ********************************* Room *********************************
Room is a grid of 10x10 points
Room keeps track of the npcs, monsters and objects in the room
It is also in charge of moving them every N time units

*/
class Room
{
public:
	Room(AREA_NAME eAreaName, int nRoomIndex, const std::string& strRoomDescription = "room");

	std::string GetRoomDescription() const;
	Point* GetPoint(int nPointIndex);
	size_t GetNumberOfPoints() const;

private:
	std::string m_strRoomDescription;
	AREA_NAME m_eAreaName;
	int m_nRoomIndex;
	std::vector<std::unique_ptr<Point>> m_vPoints; // 50x50 grid of points
};

/* ********************************* Point *********************************
A point is the smallest object in the world map. It is about 20 square meters.
It holds information such as, has the player visited before
*/
class Point
{
public:
	Point(LOCATION sPointLocation, const std::string& strPointDescription = "", bool bHasVisited = false, 
		POINT_TYPE ePointType = POINT_TYPE_GENERIC);
	LOCATION GetLocation() const;
	POINT_TYPE GetPointType() const;
	void SetPointType(POINT_TYPE ePointType);
private:
	std::string m_strPointDescription;
	LOCATION m_sPointLocation;
	bool m_bHasVisited;
	POINT_TYPE m_ePointType;
	//std::vector<std::unique_ptr<Monster>> m_vMonsters;
	/*
	*
	*
	vector of Monsters
	vector of npcs
	vector of Objects
	*/
};


#endif // !__MAP_HPP__
