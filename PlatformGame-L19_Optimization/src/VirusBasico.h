#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "ProyectilesVirusBasico.h"
#include "CelulaBasica.h"

struct SDL_Texture;
class Player;

class VirusBasico : public Entity
{
public:
	VirusBasico();
	virtual ~VirusBasico();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	bool Destroy() override;

	//audio
	int idleFxId;
	int attack1FxId;
	int attack2FxId;
	int parasiteFxId;
	int groundHitFxId;
	int deathFxId;
	int reviveFxId;
	int idleChannel = -1;

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	bool IsEnemyStunned() override;

private:
	void GetPhysicsValues();
	void Func_EnemyStates(float dt);
	void Move();
	void ApplyPhysics();
	void Draw(float dt);
	bool IsPlayerDetected() const;
	void SpawnBigProjectile();
	void SpawnWaveProjectiles();
	void SpawnParasiteProjectile();
	CelulaBasica* FindNearestCell(float range);
	void SpawnBigProjectileToTarget(Vector2D targetPos);
	void SpawnParasiteProjectileToTarget(Vector2D targetPos);

public:
	Player* player = nullptr;

	float speed = 3.0f;
	int armor = 3;

	int damageWave = 1;       
	int damageSpikes = 1;     
	int damageProjectile = 3; 
	int currentAttack = 0;


	float stuntimer = 10.0f;
	float reviveTime = 20.0f;
	float attackCooldownMs = 2500.0f;
	float projectileSpawnDelayMs = 350.0f;

	SDL_Texture* texture = nullptr;
	int texW = 256;
	int texH = 256;
	PhysBody* pbody = nullptr;

	bool isStunned = false;
	bool isFacingRight = false;
	bool hasHit = false;
	bool isPlayerDetected = false;
	bool groundHitPlayed = false;
	bool isFallingToGround = false;
	bool hasLandedStunned = false;
	bool hasSpawnedProjectile = false;
	bool canAttack = true;
	bool attackingCell = false;

	Vector2D currentAttackTarget = Vector2D(0, 0);

	float detectionRange = 800.0f;

	Timer timer_01;
	Timer reviveTimer;
	Timer attackTimer;
	Timer attackCooldownTimer;


private:
	b2Vec2 velocity;
	AnimationSet anims;

	enum class VIRUS_STATE
	{
		IDLE,
		MOVING,
		STUNED,
		ATTACKING,
		TEMP_DEATH,
		REVIVING
	};

	VIRUS_STATE currentState = VIRUS_STATE::IDLE;
};