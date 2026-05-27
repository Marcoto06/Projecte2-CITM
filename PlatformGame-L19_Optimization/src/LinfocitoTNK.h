#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Pathfinding.h"

struct SDL_Texture;
class Player;

class LinfocitoTNK : public Entity
{
public:

	LinfocitoTNK();
	virtual ~LinfocitoTNK();
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
	float speed = 1.0f;
	int armor = 5;
	int damage = 2;
	SDL_Texture* texture = NULL;
	int texW, texH;
	PhysBody* pbody;

	float stuntimer = 10.0f;
	float absorbTime = 5.0f;
	float damageCooldown = 2.0f;

	float rollSpeed = 8.0f;
	float jumpAttackSpeedX = 4.0f;

	float rollChargeTime = 500.0f;
	float rollAttackTime = 1200.0f;
	float rollCrashTime = 450.0f;

	float jumpChargeTime = 600.0f;
	float jumpUpTime = 750.0f;
	float jumpDownTime = 900.0f;
	float jumpLandTime = 1000.0f;

	float jumpArcHeight = 120.0f;

	Timer timer_01;
	Timer suckTimer;
	Timer attackTimer;
	Timer damageTimer;

	int jumpStartX = 0;
	int jumpStartY = 0;
	int jumpTargetX = 0;
	int jumpTargetY = 0;

	//bools
	bool isStunned = false;
	bool isFacingRight = false;
	bool isBeingSucked = false;
	bool isRolling = false;
	bool isJumpAttacking = false;
	bool canDamagePlayer = true;
	bool isTouchingPlayer = false;

	Player* touchingPlayer = nullptr;
	Player* attackingPlayer = nullptr;

	int attackType = 0;   // 0 none, 1 roll, 2 jump
	int attackPhase = 0;  
	int syringeHits = 0;
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
		CHASING,
		ATTACK,
		STUNED,
		DEATH
	};

	ENEMYSTATES currentEState = ENEMYSTATES::WALKING;
};

