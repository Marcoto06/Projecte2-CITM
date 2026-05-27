#pragma once
#include "Entity.h"
#include "Textures.h"
#include "Animation.h"
#include "Physics.h"
#include "Scene.h"

class Collectibles : public Entity
{
public:
	Collectibles();
	virtual ~Collectibles();
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	bool Destroy() override;

public:
	/*static std::vector<Collectibles*> allCollectibles;*/
	SDL_Texture* texture = NULL;
	int w, h;
	bool picked = false;
	int c_num = 0;

	std::string destMap = "";
	float destX = 0.0f;
	float destY = 0.0f;

private:
	AnimationSet anims;
	PhysBody* pbody;
};

