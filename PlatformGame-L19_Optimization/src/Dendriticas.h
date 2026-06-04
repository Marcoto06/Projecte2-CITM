#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Pathfinding.h"


struct SDL_Texture;
class Player;

class Dendriticas : public Entity
{
public:

	Dendriticas();
	virtual ~Dendriticas();
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	bool IsEnemyStunned();
	bool Destroy() override;
	bool Destroy(Player* player);
	Player* player = nullptr;

private:
	void PerformPathfinding();
	void GetPhysicsValues();
	void Func_EnemyStates(float dt);
	void Move();
	void ApplyPhysics();
	void Draw(float dt);
	bool IsPlayerDetected() const;
	Vector2D GetNextPathTile() const;

public:

	//Declare enemy parameters
	float speed = 2.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;
	PhysBody* pbody;

	float stuntimer = 7.0f;
	Timer timer_01;
	Timer suckTimer;

	//audio fx
	int idleFxId = -1;
	int walkFxId = -1;
	int stunFxId = -1;
	int deathFxId = -1;
	int attackFxId = -1;
	Timer audioTimer;      
	bool stunSoundPlaying = false;

	//bools
	bool isStunned = false;
	bool isFacingRight = false;
	bool isBeingSucked = false;

	Player* attackingPlayer = nullptr;

	int pathfindingFrameCount = 0;
	const int pathfindingUpdateRate = 30; // frequency in frames

	bool isPlayerDetected = false;
	float detectionRange = 450.0f;

private:
	b2Vec2 velocity;
	AnimationSet anims;
	std::shared_ptr<Pathfinding> pathfinding;

	enum class ENEMYSTATES
	{
		WALKING,
		CHASING, //idk if we're going to use this
		STUNED,
		ATTACK,
		DEATH
	};

	ENEMYSTATES currentEState = ENEMYSTATES::WALKING;
};


