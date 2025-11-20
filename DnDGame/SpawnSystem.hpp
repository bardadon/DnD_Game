#ifndef __SPAWN_SYSTEM_HPP__
#define __SPAWN_SYSTEM_HPP__

#include <unordered_map>
#include <functional>
#include <memory>

enum INSTANCE_TYPE
{
	INSTANCE_TYPE_MONSTER = 0,
	INSTANCE_TYPE_NPCS,
	INSTANCE_TYPE_OBJECTS
};

template <typename KEY, typename BASE, typename ... ARGS>
class Factory
{
public:
	Factory() = default;
	~Factory() = default;

	void RegisterCreator(KEY key, std::function<std::unique_ptr<BASE>(ARGS ...)> creator);
	std::unique_ptr<BASE> CreateInstance(KEY key, ARGS ... args);

private:
	std::unordered_map < KEY, std::function<std::unique_ptr<BASE>(ARGS ...)>> m_mapCreators;

}; // Factory


template<typename KEY, typename BASE, typename ...ARGS>
inline void Factory<KEY, BASE, ARGS...>::RegisterCreator(KEY key, std::function<std::unique_ptr<BASE>(ARGS...)> creator)
{
	m_mapCreators[key] = creator;
}

template<typename KEY, typename BASE, typename ...ARGS>
inline std::unique_ptr<BASE> Factory<KEY, BASE, ARGS...>::CreateInstance(KEY key, ARGS ...args)
{
	return m_mapCreators.at(key)(args ... );
}








/* **************************** SpawnSystem ********************************
SpawnSystem is a generic factory used for spawnning the basic instances a player can see 
throughout the game, such as Monsters, NPCS and Objects.

*/















#endif // !__SPAWN_SYSTEM_HPP__

