#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include "Collectibles.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();

	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Destroy();

	bool TryStepUp();

	//Atack variables float and bools for the attack state and timers for the attack duration and cooldown.
	bool isAtacking = false;

	float attackDuration = 0.20f;

	float attackCooldown = 0.40f;

	float attackTimer = 0.0f;

	float attackCooldownTimer = 0.0f;

	bool attackHitBoxActive = false;

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	Vector2D GetPosition();
	void SetPosition(Vector2D pos);

	void SetRespawnPosition(Vector2D pos);
	Vector2D GetRespawnPosition() const { return respawnPosition; }

private:

	void GetPhysicsValues();
	void Move();
	void Jump(float dt);
	void Func_PlayerState();
	void Func_Attacks(float dt);
	void Teleport();
	void ApplyPhysics();
	void Draw(float dt);
	void Func_BoostMovement();
	void AutoStepUp();

	// Methods of Atack

	void Attack();

	void UpdateAttack(float dt);

	void StartAttackHitBox();

	void StopAttackHitBox();

public:

	void ActivateSpeedBoost();

	//Speed control
	float normalSpeed = 7.5f;      	
	float boostSpeed = 10.0f;      

	int stepHeight = 64;
	int stepForward = 6;
	int maxStepHeight = 70;
	int stepCheckDistance = 8;

	SDL_Texture* texture = NULL;

	SDL_Texture* healText = NULL;

	int texW, texH;

	//Audio fx
	int pickCoinFxId; //??????
	
	PhysBody* pbody;

	float jumpForce = 20.5f;

	float jumpHoldTime = 0.0f;
	float maxJumpHoldTime = 150.0f;
	float extraJumpForce = 0.8f;

	bool isHoldingJump = false;

	int kill_counter = 0;

	int playerMaxHp = 10;
	int playerCurrentHp;

	Vector2D respawnPosition;

	//STATES
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool controllerJumpState = false;
	bool controllerSuckState = false;
	bool facingRight = true;
	bool isAttacking = false;
	bool isSucking = false;
	bool isMoving = false;
	bool hasASpeedBoost = false;
	bool onGround = false;
	int groundContacts = 0;
	bool nextToWall = false;
	bool canMove = true;
	bool canJump = true;
	bool canAttack = true;
	bool isHurt = false;
	bool isSteppingUp = false;

	//CHARGES
	bool hasPowerJump = false;
	float powerJumpForce = 30.0f;
	float stepUpTimer = 0.0f;
	float stepUpCooldown = 120.0f;
	
	bool healing = false;

	//PERMANENT UPGRADES
	bool hasWallJump;
	bool hasSlide;
	bool hasAcidResistance;
	bool hasAscend;


	Timer boostTimer_01;
	Timer hurtTimer;
	AnimationSet effectAnims;

	std::vector<int> list_collectibles;
private:
	b2Vec2 velocity;
	AnimationSet anims;
	

	//Fixture
	PhysBody* syringeBody = nullptr;
	PhysBody* suckBody = nullptr;

	enum class PLAYERSTATE {
		IDLE,
		MOVE,
		ATTACK,
		SUCKING,
		PREPARE_JUMP,
		JUMPING,
		JUMPING2,
		FALLING_JUMP,
		END_JUMP,
		DEATH
	};

	PLAYERSTATE currentState = PLAYERSTATE::IDLE;
};