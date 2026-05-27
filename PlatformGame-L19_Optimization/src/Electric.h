#pragma once

#include "Entity.h"
#include "Timer.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

class Electric : public Entity
{
public:

	Electric();
	virtual ~Electric();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Destroy();

	int width, height;

	int timeBetween = 8;
	int electricTime = 3;

private:
	PhysBody* pbody;
	Timer stateTimer;
};
