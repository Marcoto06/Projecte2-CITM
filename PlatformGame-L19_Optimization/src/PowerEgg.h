#pragma once
#include "Entity.h"
#include "Textures.h"
#include "Animation.h"
#include "Physics.h"
#include "Player.h"

class PowerEgg : public Entity
{
public:
	PowerEgg();
	virtual ~PowerEgg();
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
	int w = 128;
	int h = 128;
	bool picked = false;
	int assimilate;

	std::shared_ptr<Player> player;

private:
	AnimationSet anims;
	PhysBody* pbody;
};