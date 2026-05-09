#pragma once

#include "Entity.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

class Door : public Entity
{
public:

	Door();
	virtual ~Door();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Destroy();

	std::string destination;
	int width, height;

	void OnCollision(PhysBody* physA, PhysBody* physB);

private:
	PhysBody* pbody;
};
