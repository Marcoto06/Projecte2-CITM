#pragma once

#include "Entity.h"
#include "Player.h"
#include "Animation.h"
#include "Timer.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>

struct SDL_Texture;

class Eosinofilo : public Entity
{
public:

	Eosinofilo();
	virtual ~Eosinofilo();
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	bool Destroy() override;
	bool Destroy(Player* player);

private:
	void GetPhysicsValues();
	void Func_EnemyStates(float dt);
	void Move();
	void ApplyPhysics();
	void Draw(float dt);
	bool IsPlayerDetected() const;

public:

	//Declare enemy parameters
	float speed = 3.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;
	PhysBody* pbody;

	float stuntimer = 7.0f;
	Timer timer_01;
	Timer suckTimer;

	//bools
	bool isStunned = false;
	bool isFacingRight = false;
	bool isBeingSucked = false;

	Player* attackingPlayer = nullptr;

	bool isPlayerDetected = false;
	float detectionRange = 450.0f;

private:
	b2Vec2 velocity;
	AnimationSet anims;

	enum class ENEMYSTATES
	{
		PATROLING,
		MOVING,
		ATTACKING,
		STUNED
	};

	ENEMYSTATES currentEState = ENEMYSTATES::PATROLING;

	Timer waitTimer;
	Timer moveTimer;
};