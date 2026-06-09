#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Mucosa.h"

struct SDL_Texture;
class Player;

class Boss2 : public Entity
{
public:

	Boss2();
	virtual ~Boss2();
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
	void Reset();
	Player* player = nullptr;

	int life = 1;
	int maxLife = 1;
	bool active = false;
	PhysBody* triggerBody;

private:

	Vector2D initialPos;
	Vector2D stunHeadPos;

	int attackTime;
	Timer attackTimer;
	int currentAttack;

	//Frame Timer
	Timer frameTimer;

	std::vector<std::vector<SDL_Texture*>*> animations;

	//BODY ANIMATIONS
	struct bossAnimation {
		std::vector<SDL_Texture* > animation;
		int frames = 0;
		std::string name = "";
		bool loop = false;
		bossAnimation(int frames, std::string name, bool loop);
	};
	//Body animations
	bossAnimation* anim_rest;
	bossAnimation* anim_intro;
	bossAnimation* anim_hit;
	bossAnimation* anim_idle;
	bossAnimation* anim_shock;
	bossAnimation* anim_death;
	bossAnimation* anim_mucose;

	int currentFrame = 1;
	int totalAnimFrames = 1;

	bossAnimation* currentAnimation;

private:
	void GetPhysicsValues();
	void ApplyPhysics();
	void Draw(float dt);
	void AnimationFinished(bossAnimation* animation);
	void PlayAnimation(bossAnimation* animation);
	void PrepareAttack();
	void Attack();

public:
	PhysBody* pbody = nullptr;

	//COSES ANTIGUES DE ENEMY -------------
	Timer suckTimer;
	//audio
	int fx_intro;
	int fx_hit;
	int fx_mucose;
	int fx_idle;
	int fx_death;

	Timer idleAudioTimer;
	int idleAudioSeconds = 1;
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

	std::vector <std::shared_ptr<Mucosa>> projectiles;
};