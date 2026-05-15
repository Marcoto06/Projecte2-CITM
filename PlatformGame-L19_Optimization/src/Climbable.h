#pragma once

#include "Entity.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

class Climbable : public Entity
{
public:

	Climbable();
	virtual ~Climbable();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Destroy();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	int width, height;

	float climbPoint;

private:
	PhysBody* pbody;
};
