#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
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

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	Vector2D GetPosition();
	void SetPosition(Vector2D pos);

private:

	void GetPhysicsValues();
	void Move();
	void Jump();
	void Func_Attacks(float dt);
	void Teleport();
	void ApplyPhysics();
	void Draw(float dt);
	void Func_BoostMovement();

public:

	void ActivateSpeedBoost();

	//Speed control
	float normalSpeed = 5.0f;      	
	float boostSpeed = 8.0f;      	

	SDL_Texture* texture = NULL;

	int texW, texH;

	//Audio fx
	int pickCoinFxId; //??????
	
	PhysBody* pbody;
	float jumpForce = 1.75f; // The force to apply when jumping
	float attackTimer = 0.0f;
	int kill_counter = 0;


	//STATES
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool facingRight = true;
	bool isAttacking = false;
	bool isSucking = false;
	bool isMoving = false;
	bool hasASpeedBoost = false;

	//PERMANENT UPGRADES
	bool hasWallJump;
	bool hasSlide;
	bool hasAcidResistance;
	bool hasAscend;


	Timer boostTimer_01;

private:
	b2Vec2 velocity;
	AnimationSet anims;

	//Fixture
	PhysBody* syringeBody = nullptr;
	PhysBody* suckBody = nullptr;

	enum class PLAYERSTATE {
		IDLE,
		MOVE,
		JUMP,
		ATTACK,
		SUCKING
	};

	PLAYERSTATE currentState = PLAYERSTATE::IDLE;
};