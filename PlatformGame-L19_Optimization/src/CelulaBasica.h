#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>

struct SDL_Texture;
class Player;

class CelulaBasica : public Entity
{
public:
	enum class CellType
	{
		FIBROBLASTO,
		STREPTOCOCCUS, //-------Add streptococcus as a new cell type instead of aspergillus, with its own unique behavior and animations.----------
		ASPERGILLUS,
		SALMONELLA,
		NEURONA
	};

public:
	CelulaBasica();
	virtual ~CelulaBasica();

	bool Awake() override;
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool Destroy() override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

	void SetCellType(CellType type);
	void Parasitize();

	void SetPosition(Vector2D pos);
	Vector2D GetPosition();

	bool IsEnemyStunned() override;

private:
	void LoadCellData();
	void GetPhysicsValues();
	void Func_CellStates(float dt);
	void MoveRandom();
	void MoveParasitized();
	void ApplyPhysics();
	void Draw(float dt);
	bool IsPlayerDetected() const;

public:
	Player* player = nullptr;

	CellType cellType = CellType::FIBROBLASTO;

	SDL_Texture* texture = nullptr;
	SDL_Texture* parasitizedTexture = nullptr;

	int texW = 256;
	int texH = 256;

	PhysBody* pbody = nullptr;
	PhysBody* attackHitbox = nullptr;

	bool attackHasHit = false;
	bool isParasitized = false;
	bool isStunned = false;
	bool isFacingRight = false;
	bool isAttacking = false;
	bool isHurt = false;
	bool canDamagePlayer = true;
	bool isTouchingPlayer = false;
	bool canTongueAttack = true;
	bool hasFloatBaseY = false;
	bool isFallingToGround = false;
	Player* touchingPlayer = nullptr;

	float normalMoveSpeed = 1.0f;
	float parasitizedMoveSpeed = 2.0f;
	float detectionRange = 450.0f;
	float hurtDurationMs = 500.0f;
	float damageCooldownMs = 1500.0f;
	float attackRange = 170.0f;
	float attackCooldownMs = 1500.0f;
	float floatBaseY = 0.0f;
	

	int contactDamage = 1;

	int maxHp = 6;
	int currentHp = 6;

	void TakeDamage(int amount);
	bool IsParasitized() const;

	Timer moveTimer;
	Timer idleTimer;
	Timer stunTimer;
	Timer damageTimer;
	Timer hurtTimer;
	Timer attackCooldownTimer;

private:
	b2Vec2 velocity;
	b2Vec2 randomDirection;

	AnimationSet normalAnims;
	AnimationSet parasitizedAnims;

	enum class CELL_STATE
	{
		IDLE,
		MOVING,
		PARASITIZED_CHASING,
		STUNED,
		DEATH
	};

	CELL_STATE currentState = CELL_STATE::IDLE;
};