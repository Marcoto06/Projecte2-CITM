#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include "Vector2D.h"
#include "Physics.h"

struct SDL_Texture;
class Eosinofilo;

class Boomerang : public Entity
{
public:
	Boomerang();
	virtual ~Boomerang();

	bool Awake() override;
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool Destroy() override;

	void SetSpawnPosition(const Vector2D& pos);
	void SetDirection(const Vector2D& dir);
	void SetOwner(Eosinofilo* eosOwner);

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

private:
	void Move();
	void Draw(float dt);
	void ApplyPhysics();
	void StartReturning();

private:
	enum class BoomerangState
	{
		OUTGOING,
		RETURNING
	};

public:
	SDL_Texture* texture = nullptr;
	int texW = 0;
	int texH = 0;

	float speed = 8.0f;
	bool facingRight = true;

	Vector2D direction = Vector2D(1, 0);
	Vector2D spawnPosition = Vector2D(0, 0);

	Timer lifeTimer;

	PhysBody* pbody = nullptr;
	Eosinofilo* owner = nullptr;

	float curveAmplitude = 25.0f;
	float curveFrequency = 0.008f;
	float knockbackForceX = 8.0f;
	float knockbackForceY = -3.0f;

	bool hasHitPlayer = false;

	BoomerangState currentState = BoomerangState::OUTGOING;

	float outgoingDurationMs = 700.0f;
	float maxLifeDurationMs = 2500.0f;
	float returnSpeedMultiplier = 1.25f;
	float catchDistance = 35.0f;

private:
	AnimationSet anims;
};