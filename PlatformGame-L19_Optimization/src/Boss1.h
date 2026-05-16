#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>

struct SDL_Texture;
class Player;

class Boss1 : public Entity
{
public:

	Boss1();
	virtual ~Boss1();
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	bool IsEnemyStunned() override;
	bool Destroy() override;
	bool Destroy(Player* player);
	void Initialize();
	Player* player = nullptr;

	int life = 3;
	bool active = false;
	

private:
	Vector2D initialHeadPos;
	Vector2D stunHeadPos;

	enum Body_Parts {
		BODY,
		L_HAND,
		R_HAND
	};
	struct Hand {
		PhysBody* pbody = nullptr;
		Vector2D position = Vector2D(0, 0);
		b2Vec2 velocity = b2Vec2_zero;
		Vector2D idlePos = Vector2D(0, 0);
		Timer attackTimer;
		int attackCooldown = 0;
		float v_speed = 0.25f;
		bool attacking = false;
		Hand() {};
	};
	//Frame Timer
	Timer frameTimer;

	//BODY ANIMATIONS
	struct bossAnimation {
		std::vector<SDL_Texture* > animation;
		int frames = 0;
		std::string name = "";
		Body_Parts part;
		bool loop = false;
		bossAnimation(int frames, std::string name, Body_Parts part, bool loop);
	};
	//Body animations
	bossAnimation* idle_body;
	bossAnimation* intro_body;
	bossAnimation* hurt_body;
	bossAnimation* stun_body;
	
	int currentBodyFrame = 1;
	int totalBodyAnimFrames = 1;

	//HAND ANIMATIONS
	bossAnimation* intro_R_hand;
	bossAnimation* idle_R_hand;
	bossAnimation* vertical_R_hand;

	int currentRHandFrame = 1;
	int totalRHandAnimFrames = 1;

	bossAnimation* currentBodyAnimation;
	bossAnimation* currentRHandAnimation;

private:
	void GetPhysicsValues();
	void ApplyPhysics();
	void Draw(float dt);

	void AnimationFinished(bossAnimation* animation);
	void PlayAnimation(bossAnimation* animation);
	void MoveHands(float dt);
	void Attack(Hand* hand);

public:
	Hand* R_Hand;
	Hand* L_Hand;
	PhysBody* head_body = nullptr;

	//COSES ANTIGUES DE ENEMY -------------
	Timer suckTimer;

	//bools
	bool isStunned = false;

	bool isPlayerDetected = false;

private:
	enum class BOSS_STATES
	{
		WALKING,
		CHASING, //idk if we're going to use this
		STUNED,
		DEATH
	};

	BOSS_STATES currentState;
};