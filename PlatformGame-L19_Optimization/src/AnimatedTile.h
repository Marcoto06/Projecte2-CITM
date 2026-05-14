#pragma once

#include "Entity.h"
#include "Animation.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

class AnimatedTile : public Entity
{
public:

	AnimatedTile();
	virtual ~AnimatedTile();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Destroy();

	void LoadTexture(std::string _texture);

	AnimationSet anims;
	SDL_Texture* texture = nullptr;

	bool draw = false;
};
