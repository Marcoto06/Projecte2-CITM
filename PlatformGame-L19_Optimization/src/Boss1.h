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

	int life = 3;
	

private:
	Vector2D initialHeadPos;
	Vector2D stunHeadPos;

	Vector2D idleRHandPos;
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

	int currentRHandFrame = 1;
	int totalRHandAnimFrames = 1;

	bossAnimation* currentBodyAnimation;
	bossAnimation* currentRHandAnimation;

private:
	void GetPhysicsValues();
	void Func_EnemyStates(float dt);
	void ApplyPhysics();
	void Draw(float dt);

	void AnimationFinished(bossAnimation* animation);
	void PlayAnimation(bossAnimation* animation);

public: //COSES ANTIGUES DE ENEMY -------------
	PhysBody* head_body;
	PhysBody* L_hand_body;
	PhysBody* R_hand_body;
	b2Vec2 R_hand_velocity;

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