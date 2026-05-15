#pragma once

#include "Module.h"
#include "Entity.h"
#include <list>

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake();

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp(bool total);

	// Additional methods
	std::shared_ptr<Entity> CreateEntity(EntityType type);

	void DestroyEntity(std::shared_ptr<Entity> entity);

	void AddEntity(std::shared_ptr<Entity> entity);

	void ClearNonPlayerEntities();

	std::shared_ptr<Entity> GetEntityByTiledId(int id);

	bool DrawAnimatedTiles(float dt);

public:

	std::list<std::shared_ptr<Entity>> entities;
};