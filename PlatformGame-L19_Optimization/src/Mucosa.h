#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include "Vector2D.h"
#include "Physics.h"

class Mucosa : public Entity
{
public:
	Mucosa();
	virtual ~Mucosa();

	bool Awake() override;
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool Destroy() override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

	void Spawn();

private:
	void Draw(float dt);
	void ApplyPhysics();

public:
	int state = 0;
	int texW, texH;
	Vector2D spawnPos;
	PhysBody* pbody;
	b2Vec2 velocity;

	SDL_Texture* projectileTexture = nullptr;
	SDL_Texture* staticTexture = nullptr;
	SDL_Texture* electricTexture = nullptr;

private:
	AnimationSet projectileAnims;
	AnimationSet electricAnims;
	Timer electricTimer;
	int electricTime = 1;
};