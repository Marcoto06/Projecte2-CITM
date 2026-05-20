#pragma once

#include "Entity.h"
#include "Animation.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

class Acid : public Entity
{
public:

	Acid();
	virtual ~Acid();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Destroy();

	int width, height;

	PhysBody* pbody;
};
