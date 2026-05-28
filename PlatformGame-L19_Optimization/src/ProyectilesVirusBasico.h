#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include "Vector2D.h"
#include "Physics.h"

struct SDL_Texture;

class ProyectilesVirusBasico : public Entity
{
public:
	ProyectilesVirusBasico();
	virtual ~ProyectilesVirusBasico();

	bool Awake() override;
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool Destroy() override;

	void SetSpawnPosition(const Vector2D& pos);
	void SetDirection(const Vector2D& dir);
	void SetProjectileType(int type);

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

private:
	void Draw(float dt);
	void ApplyPhysics();

public:
	SDL_Texture* texture = nullptr;
	PhysBody* pbody = nullptr;

	int texW = 64;
	int texH = 64;

	float speed = 6.0f;
	int damage = 3;

	int projectileType = 2;
	bool facingRight = true;
	bool hasHit = false;

	Vector2D direction = Vector2D(1, 0);
	Vector2D spawnPosition = Vector2D(0, 0);

	Timer lifeTimer;
	float maxLifeDurationMs = 3000.0f;

private:
	AnimationSet anims;
};