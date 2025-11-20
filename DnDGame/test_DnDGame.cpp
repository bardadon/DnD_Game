#include <iostream>

#include "DndGame.hpp"
#include "Trie.hpp"
#include "SpawnSystem.hpp"

std::unique_ptr<Monster> MonsterCreatorFunc(MONSTER_ATTRIBUTES sMonsterAttributes, LOCATION sMonsterLocation)
{
	return std::make_unique<Monster>(sMonsterAttributes, sMonsterLocation);
}

int main(void)
{
	//System sys;


	//sys.GameLoop();

	Factory < INSTANCE_TYPE, Monster, MONSTER_ATTRIBUTES, LOCATION> factory;
	MONSTER_ATTRIBUTES sMonsterAttributes;
	LOCATION sMonsterLocation;

	factory.RegisterCreator(INSTANCE_TYPE::INSTANCE_TYPE_MONSTER, MonsterCreatorFunc);

	std::unique_ptr<Monster> monster = factory.CreateInstance(INSTANCE_TYPE::INSTANCE_TYPE_MONSTER, sMonsterAttributes, sMonsterLocation);


	return 0;
}




