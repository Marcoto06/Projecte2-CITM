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
	Player* player = nullptr;

private:
	enum Body_Parts {
		BODY,
		L_HAND,
		R_HAND
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
	bossAnimation* idle_body;
	bossAnimation* intro_body;
	
	int currentBodyFrame = 1;
	int totalBodyAnimFrames = 1;

	bossAnimation* currentBodyAnimation;
	bossAnimation* currentHandAnimation;

private:
	void GetPhysicsValues();
	void Func_EnemyStates(float dt);
	void ApplyPhysics();
	void Draw(float dt);

	void AnimationFinished(bossAnimation* animation);
	void PlayAnimation(bossAnimation* animation);

public: //COSES ANTIGUES DE ENEMY -------------

	//Declare enemy parameters
	float speed = 2.0f;
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

	int pathfindingFrameCount = 0;
	const int pathfindingUpdateRate = 30; // frequency in frames

	bool isPlayerDetected = false;
	float detectionRange = 450.0f;

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